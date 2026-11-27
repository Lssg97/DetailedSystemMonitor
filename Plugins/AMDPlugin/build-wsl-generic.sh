#!/usr/bin/env bash
set -euo pipefail

# Generic WSL build helper for Delphi-on-Windows and FPC-on-Linux projects.
#
# Typical usage:
#   ./build-wsl-generic.sh Demo/TestApp.dproj Win64
#   ./build-wsl-generic.sh Demo/TestApp.lpr Linux64
#
# Useful environment variables:
#   PROJECT_ROOT=/path/to/root             Override repository/project root
#   DELPHI_PLATFORM=Win64                  Default platform when arg 2 is omitted
#   DELPHI_DEFINES="DEBUG;TRACE"           Extra defines, accepts ; , or spaces
#   BDS_VERSION=23.0                       Delphi Studio version folder, auto-detected when omitted
#   BDS=/mnt/c/.../Studio/23.0             Override Delphi Studio root
#   DELPHI_CONFIG=Release                  MSBuild config for .dproj builds
#   MSBUILD=/mnt/c/.../MSBuild.exe         Override MSBuild executable for .dproj builds
#   DCC32=/mnt/c/.../dcc32.exe             Override Win32 compiler
#   DCC64=/mnt/c/.../dcc64.exe             Override Win64 compiler
#   FPC=fpc                                Override FPC compiler
#   SOURCE_PATHS="Source;Lib;Shared"       Extra unit/source search paths, repo-relative or absolute
#   SOURCE_PATHS_RECURSIVE=1               Also add subdirectories under SOURCE_PATHS directories
#   DELPHI_LIB_WIN32="c:\\..."             Override Delphi Win32 RTL/VCL library path
#   DELPHI_LIB_WIN64="c:\\..."             Override Delphi Win64 RTL/VCL library path
#   DELPHI_NAMESPACES="Winapi;System;..."  Override namespace set
#   OUTPUT_ROOT=Bin                        Output root, repo-relative or absolute
#   UNIT_SUBDIR=Units                      Unit output subdirectory under platform output
#   RUNTIME_FILES="Runtime/Win64/a.dll"    Files to copy to output after successful build
#   PREBUILD_HOOK=./scripts/prebuild.sh    Optional hook called before compile
#   POSTBUILD_HOOK=./scripts/postbuild.sh  Optional hook called after compile
#   FPC_EXTRA_ARGS="-gl -gh"               Additional FPC args
#   DCC_EXTRA_ARGS="-Q"                    Additional DCC args

usage() {
  cat >&2 <<'USAGE'
Usage:
  build-wsl-generic.sh <project-file> [Win32|Win64|Linux64]

Examples:
  build-wsl-generic.sh Demo/TestApp.dproj Win64
  build-wsl-generic.sh Demo/TestApp.dpr Win64
  build-wsl-generic.sh apps/server.lpr Linux64
  SOURCE_PATHS="Source;ThirdParty/PasUnits" DELPHI_DEFINES="DEBUG;USE_SSL" build-wsl-generic.sh MyApp.dpr Win64

Project file may be .dproj, .dpr, .dpk, .lpr, or .pas. For Windows .dproj targets this script uses MSBuild and Delphi's project settings.
For direct Windows source builds this script expects Delphi's dcc32/dcc64.exe to be reachable from WSL.
For Linux64 it uses FPC.
USAGE
}

fail() {
  printf 'ERROR: %s\n' "$*" >&2
  exit 1
}

is_abs_linux_path() {
  [[ "$1" == /* ]]
}

resolve_path() {
  local path="$1"
  if is_abs_linux_path "$path"; then
    printf '%s\n' "$path"
  else
    printf '%s/%s\n' "$ROOT" "$path"
  fi
}

normalize_list() {
  local value="${1:-}"
  value="${value//,/;}"
  value="${value// /;}"
  printf '%s\n' "$value"
}

append_unique() {
  local item="$1"
  local existing
  for existing in "${SEARCH_PATHS_LINUX[@]}"; do
    [[ "$existing" == "$item" ]] && return
  done
  SEARCH_PATHS_LINUX+=("$item")
}

append_source_tree() {
  local root_path="$1"
  local subdir

  append_unique "$root_path"

  [[ "${SOURCE_PATHS_RECURSIVE:-1}" == "1" ]] || return

  while IFS= read -r -d '' subdir; do
    append_unique "$subdir"
  done < <(find "$root_path" -mindepth 1 -type d -print0 | sort -z)
}

join_by_semicolon() {
  local IFS=';'
  printf '%s' "$*"
}

linux_paths_to_windows_search_path() {
  local path
  local -a win_paths=()
  for path in "$@"; do
    [[ -d "$path" || -f "$path" ]] || continue
    win_paths+=("$(wslpath -w "$path")")
  done
  join_by_semicolon "${win_paths[@]}"
}

append_defines_to_fpc_args() {
  local normalized define
  normalized="$(normalize_list "$CUSTOM_DEFINES")"
  IFS=';' read -r -a DEFINE_LIST <<< "$normalized"
  for define in "${DEFINE_LIST[@]}"; do
    [[ -n "$define" ]] && FPC_ARGS+=("-d$define")
  done
}

make_delphi_define_set() {
  local normalized define result="PLATFORM_${PLATFORM}"
  normalized="$(normalize_list "$CUSTOM_DEFINES")"
  IFS=';' read -r -a DEFINE_LIST <<< "$normalized"
  for define in "${DEFINE_LIST[@]}"; do
    [[ -n "$define" ]] && result="${result};${define}"
  done
  printf '%s\n' "$result"
}

copy_runtime_files() {
  local normalized runtime_file source target
  normalized="${RUNTIME_FILES:-}"
  [[ -z "$normalized" ]] && return
  normalized="${normalized//,/;}"

  IFS=';' read -r -a RUNTIME_LIST <<< "$normalized"
  for runtime_file in "${RUNTIME_LIST[@]}"; do
    [[ -z "$runtime_file" ]] && continue
    source="$(resolve_path "$runtime_file")"
    [[ -f "$source" ]] || fail "Runtime file not found: $runtime_file"
    target="$BUILD_DIR/$(basename "$source")"
    cp -f "$source" "$target"
  done
}

run_hook() {
  local hook="${1:-}"
  [[ -z "$hook" ]] && return
  local hook_path
  hook_path="$(resolve_path "$hook")"
  [[ -x "$hook_path" ]] || fail "Hook is not executable: $hook"
  "$hook_path" "$PROJECT_PATH" "$PLATFORM" "$BUILD_DIR"
}

detect_bds_version() {
  local studio_root="/mnt/c/Program Files (x86)/Embarcadero/Studio"
  local version_dir

  [[ -d "$studio_root" ]] || { printf '23.0\n'; return; }

  while IFS= read -r version_dir; do
    [[ -x "$studio_root/$version_dir/bin/dcc32.exe" || -x "$studio_root/$version_dir/bin/dcc64.exe" ]] || continue
    printf '%s\n' "$version_dir"
    return
  done < <(find "$studio_root" -mindepth 1 -maxdepth 1 -type d -printf '%f\n' | sort -Vr)

  printf '23.0\n'
}

detect_msbuild() {
  local msbuild

  for msbuild in \
    "/mnt/c/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe" \
    "/mnt/c/Windows/Microsoft.NET/Framework64/v4.0.30319/MSBuild.exe"; do
    [[ -x "$msbuild" ]] && { printf '%s\n' "$msbuild"; return; }
  done

  fail "MSBuild.exe not found. Set MSBUILD to the Windows MSBuild executable."
}

as_windows_path() {
  local path="$1"
  if is_abs_linux_path "$path"; then
    wslpath -w "$path"
  else
    printf '%s\n' "$path"
  fi
}

build_delphi_project() {
  local config="${DELPHI_CONFIG:-Release}"
  local cmd_exe="${CMD_EXE:-/mnt/c/Windows/System32/cmd.exe}"
  local msbuild="${MSBUILD:-$(detect_msbuild)}"
  local bds_root="${BDS:-/mnt/c/Program Files (x86)/Embarcadero/Studio/${BDS_VERSION}}"
  local bds_win msbuild_win project_win build_win unit_win command path_arg

  [[ -x "$cmd_exe" ]] || fail "cmd.exe not found: $cmd_exe. Set CMD_EXE."
  [[ -x "$msbuild" || "$msbuild" != /* ]] || fail "MSBuild not found: $msbuild. Set MSBUILD."

  bds_win="$(as_windows_path "$bds_root")"
  msbuild_win="$(as_windows_path "$msbuild")"
  project_win="$(wslpath -w "$PROJECT_PATH")"
  build_win="$(wslpath -w "$BUILD_DIR")"
  unit_win="$(wslpath -w "$UNIT_DIR")"

  for path_arg in "$msbuild_win" "$project_win" "$build_win" "$unit_win"; do
    [[ "$path_arg" != *" "* ]] || fail "MSBuild path contains spaces and cannot be passed safely through cmd.exe from WSL: $path_arg"
  done

  command="set BDS=$bds_win&& $msbuild_win $project_win /t:Build /p:Config=$config /p:Platform=$PLATFORM /p:DCC_ExeOutput=$build_win /p:DCC_DcuOutput=$unit_win /v:minimal /nologo"
  "$cmd_exe" /C "$command"
}

[[ "${1:-}" == "-h" || "${1:-}" == "--help" ]] && { usage; exit 0; }

ROOT="${PROJECT_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)}"
PROJECT_REL="${1:-}"
[[ -n "$PROJECT_REL" ]] || { usage; exit 1; }
PROJECT_REL="${PROJECT_REL#./}"
PROJECT_PATH="$(resolve_path "$PROJECT_REL")"
PROJECT_DIR="$(dirname "$PROJECT_PATH")"
PROJECT_FILE="$(basename "$PROJECT_PATH")"
PROJECT_NAME="$(basename "${PROJECT_FILE%.*}")"
PROJECT_EXT="${PROJECT_FILE##*.}"
PROJECT_EXT="${PROJECT_EXT,,}"

PLATFORM="${2:-${DELPHI_PLATFORM:-Win64}}"
CUSTOM_DEFINES="${DELPHI_DEFINES:-}"
if [[ -z "${BDS_VERSION:-}" ]]; then
  BDS_VERSION="$(detect_bds_version)"
fi
OUTPUT_ROOT="$(resolve_path "${OUTPUT_ROOT:-Bin}")"
UNIT_SUBDIR="${UNIT_SUBDIR:-Units}"

[[ -f "$PROJECT_PATH" ]] || fail "Project not found: $PROJECT_REL"

case "$PLATFORM" in
  Win32|win32) PLATFORM="Win32" ;;
  Win64|win64) PLATFORM="Win64" ;;
  Linux|linux|Linux64|linux64) PLATFORM="Linux64" ;;
  *) fail "Unsupported platform: $PLATFORM. Use Win32, Win64, or Linux64." ;;
esac

BUILD_DIR="$OUTPUT_ROOT/$PLATFORM"
UNIT_DIR="$BUILD_DIR/$UNIT_SUBDIR"
mkdir -p "$BUILD_DIR" "$UNIT_DIR"

SEARCH_PATHS_LINUX=()
append_unique "$PROJECT_DIR"

SOURCE_PATHS_DEFAULT="${SOURCE_PATHS:-Source}"
SOURCE_PATHS_DEFAULT="${SOURCE_PATHS_DEFAULT//,/;}"
IFS=';' read -r -a SOURCE_PATH_LIST <<< "$SOURCE_PATHS_DEFAULT"
for source_path in "${SOURCE_PATH_LIST[@]}"; do
  [[ -z "$source_path" ]] && continue
  resolved="$(resolve_path "$source_path")"
  [[ -d "$resolved" ]] && append_source_tree "$resolved"
done

run_hook "${PREBUILD_HOOK:-}"

if [[ "$PLATFORM" == "Linux64" ]]; then
  FPC="${FPC:-fpc}"
  command -v "$FPC" >/dev/null 2>&1 || fail "Compiler not found: $FPC. Set FPC to the compiler you want to use."

  FPC_ARGS=(
    -B
    "-FU$UNIT_DIR"
    "-FE$BUILD_DIR"
    "-Fl$BUILD_DIR"
    "-k-rpath=\$ORIGIN"
    "-o$BUILD_DIR/$PROJECT_NAME"
  )

  for search_path in "${SEARCH_PATHS_LINUX[@]}"; do
    FPC_ARGS+=("-Fu$search_path")
  done

  append_defines_to_fpc_args

  if [[ -n "${FPC_EXTRA_ARGS:-}" ]]; then
    # shellcheck disable=SC2206
    EXTRA_ARGS=( ${FPC_EXTRA_ARGS} )
    FPC_ARGS+=("${EXTRA_ARGS[@]}")
  fi

  cd "$ROOT"
  "$FPC" "${FPC_ARGS[@]}" "$PROJECT_PATH"
else
  if [[ "$PROJECT_EXT" == "dproj" ]]; then
    build_delphi_project
    copy_runtime_files
    run_hook "${POSTBUILD_HOOK:-}"
    printf 'Built %s for %s in %s\n' "$PROJECT_NAME" "$PLATFORM" "$BUILD_DIR"
    exit 0
  fi

  case "$PLATFORM" in
    Win32)
      DCC="${DCC32:-/mnt/c/Program Files (x86)/Embarcadero/Studio/${BDS_VERSION}/bin/dcc32.exe}"
      DELPHI_LIB="${DELPHI_LIB_WIN32:-c:\\program files (x86)\\embarcadero\\studio\\${BDS_VERSION}\\lib\\win32\\release}"
      ;;
    Win64)
      DCC="${DCC64:-/mnt/c/Program Files (x86)/Embarcadero/Studio/${BDS_VERSION}/bin/dcc64.exe}"
      DELPHI_LIB="${DELPHI_LIB_WIN64:-c:\\program files (x86)\\embarcadero\\studio\\${BDS_VERSION}\\lib\\win64\\release}"
      ;;
  esac

  [[ -x "$DCC" ]] || fail "Compiler not found: $DCC. Set DCC32/DCC64 or BDS_VERSION."

  BUILD_DIR_WIN="${BUILD_DIR_WIN:-$(wslpath -w "$BUILD_DIR")}"
  PROJECT_DIR_WIN="$(wslpath -w "$PROJECT_DIR")"
  SEARCH_PATH_WIN="$(linux_paths_to_windows_search_path "${SEARCH_PATHS_LINUX[@]}")"
  [[ -n "$DELPHI_LIB" ]] && SEARCH_PATH_WIN="${SEARCH_PATH_WIN};${DELPHI_LIB}"

  NAMESPACE_SET="${DELPHI_NAMESPACES:-Winapi;System.Win;Data.Win;Datasnap.Win;Web.Win;Soap.Win;Xml.Win;System;Xml;Data;Datasnap;Web;Soap}"
  DEFINE_SET="$(make_delphi_define_set)"

  DCC_ARGS=(
    -B
    "-U$SEARCH_PATH_WIN"
    "-I$SEARCH_PATH_WIN"
    "-NS$NAMESPACE_SET"
    "-N0$BUILD_DIR_WIN"
    "-NU$BUILD_DIR_WIN"
    "-D$DEFINE_SET"
    "-E$BUILD_DIR_WIN"
  )

  if [[ -n "${DCC_EXTRA_ARGS:-}" ]]; then
    # shellcheck disable=SC2206
    EXTRA_ARGS=( ${DCC_EXTRA_ARGS} )
    DCC_ARGS+=("${EXTRA_ARGS[@]}")
  fi

  cd "$PROJECT_DIR"
  "$DCC" "${DCC_ARGS[@]}" "$PROJECT_FILE"
fi

copy_runtime_files
run_hook "${POSTBUILD_HOOK:-}"

printf 'Built %s for %s in %s\n' "$PROJECT_NAME" "$PLATFORM" "$BUILD_DIR"
