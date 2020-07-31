#pragma once
#include <windows.h>
#include <intrin.h>
#include <tchar.h>


#pragma warning(disable: 4996)

#define OLS_DRIVER_FILE_NAME_WIN_9X			_T("WinRing0.vxd")
#define OLS_DRIVER_FILE_NAME_WIN_NT			_T("WinRing0.sys")
#define OLS_DRIVER_FILE_NAME_WIN_NT_X64		_T("WinRing0x64.sys")

BOOL IsNT();
BOOL IsMsr();
BOOL IsCpuid();
BOOL IsWow64();
BOOL IsX64();
BOOL RuningEn(TCHAR gDriverFileName[]);

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE hProcess, PBOOL Wow64Process);

typedef void (WINAPI* LPFN_GETNATIVESYSTEMINFO) (LPSYSTEM_INFO lpSystemInfo);