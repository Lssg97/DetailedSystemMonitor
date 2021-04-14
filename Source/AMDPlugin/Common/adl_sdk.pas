unit adl_sdk;

interface

uses
  adl_structures;

///
///  Copyright (c) 2008 - 2013 Advanced Micro Devices, Inc.

///  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
///  EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
///  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

/// \file adl_sdk.h
/// \brief Contains the definition of the Memory Allocation Callback.\n <b>Included in ADL SDK</b>
///
/// \n\n
/// This file contains the definition of the Memory Allocation Callback.\n
/// It also includes definitions of the respective structures and constants.\n
/// <b> This is the only header file to be included in a C/C++ project using ADL </b>

type
  ADL_MAIN_MALLOC_CALLBACK  = function(v1: Integer): Pointer; stdcall;
  ADL_MAIN_CONTROL_CREATE = function(v1: ADL_MAIN_MALLOC_CALLBACK; iActiveOnly: Integer): Integer; stdcall;
  ADL_MAIN_CONTROL_DESTROY = function: Integer; stdcall;

  ADL_ADAPTER_NUMBEROFADAPTERS_GET = function(var iCount: Integer): Integer; stdcall;
  ADL_ADAPTER_ADAPTERINFO_GET = function(v1: PAdapterInfo; iNum: Integer): Integer; stdcall;
  ADL_ADAPTER_ACTIVE_GET = function(iNum: Integer; var iActive: Integer): Integer; stdcall;
  ADL_ADAPTER_VIDEOBIOSINFO_GET = function(iNum: Integer; var lpBiosInfo: TADLBiosInfo): Integer; stdcall;
  ADL_ADAPTER_ID_GET = function(iNum: Integer; var lpId: Integer): Integer; stdcall;
  ADL_ADAPTER_MEMORYINFO_GET = function(iNum: Integer; var lpMemoryInfo: TADLMemoryInfo): Integer; stdcall;
  ADL_ADAPTER_CONFIGMEMORY_GET = function(iAdapterIndex: Integer; iScreenWidth, iScreenHeight, displayFeatureMask, numDisplays: Integer; var displayFeatureList: TADLMemoryDisplayFeatures; var iNumMemTypes: Integer; var lppMemRequired: LPADLMemoryRequired): Integer; stdcall;

  ADL_ADAPTERX2_CAPS = function(iAdapterIndex: Integer; var iCaps: Integer): Integer; stdcall;

  ADL_OVERDRIVE_CAPS = function(iAdapterIndex: Integer; var iSupported: Integer; var iEnabled: Integer; var iVersion: Integer): Integer; stdcall;

  ADL_OVERDRIVE5_THERMALDEVICES_ENUM = function(iAdapterIndex: Integer; iThermalControllerIndex: Integer; var lpThermalControllerInfo: TADLThermalControllerInfo): Integer; stdcall;
  ADL_OVERDRIVE5_ODPARAMETERS_GET = function(iAdapterIndex: Integer; var lpOdParameters: TADLODParameters): Integer; stdcall;
  ADL_OVERDRIVE5_TEMPERATURE_GET = function(iAdapterIndex: Integer; iThermalControllerIndex: Integer; var lpTemperature: TADLTemperature): Integer; stdcall;
  ADL_OVERDRIVE5_FANSPEED_GET = function(iAdapterIndex: Integer; iThermalControllerIndex: Integer; var lpFanSpeedValue: TADLFanSpeedValue): Integer; stdcall;
  ADL_OVERDRIVE5_FANSPEEDINFO_GET = function(iAdapterIndex: Integer; iThermalControllerIndex: Integer; var lpFanSpeedInfo: TADLFanSpeedInfo): Integer; stdcall;
  ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET = function(iAdapterIndex: Integer; iDefault: Integer; var lpOdPerformanceLevels: TADLODPerformanceLevels): Integer; stdcall;
  ADL_OVERDRIVE5_CURRENTACTIVITY_GET = function(iAdapterIndex: Integer; var lpActivity: TADLPMActivity): Integer; stdcall;
  ADL_OVERDRIVE5_FANSPEED_SET = function(iAdapterIndex: Integer; iThermalControllerIndex: Integer; var lpFanSpeedValue: TADLFanSpeedValue): Integer; stdcall;
  ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET = function(iAdapterIndex: Integer; var lpOdPerformanceLevels: TADLODPerformanceLevels): Integer; stdcall;
  ADL_OVERDRIVE5_POWERCONTROL_CAPS = function(iAdapterIndex: Integer; var lpSupported: Integer): Integer; stdcall;
  ADL_OVERDRIVE5_POWERCONTROLINFO_GET = function(iAdapterIndex: Integer; var lpPowerControlInfo: TADLPowerControlInfo): Integer; stdcall;
  ADL_OVERDRIVE5_POWERCONTROL_GET = function(iAdapterIndex: Integer; var lpCurrentValue: Integer; var lpDefaultValue: Integer): Integer; stdcall;
  ADL_OVERDRIVE5_POWERCONTROL_SET = function(iAdapterIndex: Integer; iValue: Integer): Integer; stdcall;

  ADL_OVERDRIVE6_FANSPEED_GET = function(iAdapterIndex: Integer; var lpFanSpeedInfo: TADLOD6FanSpeedInfo): Integer; stdcall;
  ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS = function(iAdapterIndex: Integer; var lpThermalControllerCaps: TADLOD6ThermalControllerCaps): Integer; stdcall;
  ADL_OVERDRIVE6_TEMPERATURE_GET = function(iAdapterIndex: Integer; var lpTemperature: Integer): Integer; stdcall;
  ADL_OVERDRIVE6_CAPABILITIES_GET = function(iAdapterIndex: Integer; var lpODCapabilities: TADLOD6Capabilities): Integer; stdcall;
  ADL_OVERDRIVE6_STATEINFO_GET = function(iAdapterIndex: Integer; iStateType: Integer; var lpStateInfo: TADLOD6StateInfo): Integer; stdcall;
  ADL_OVERDRIVE6_CURRENTSTATUS_GET = function(iAdapterIndex: Integer; var lpCurrentStatus: TADLOD6CurrentStatus): Integer; stdcall;
  ADL_OVERDRIVE6_POWERCONTROL_CAPS = function(iAdapterIndex: Integer; var lpSupported: Integer): Integer; stdcall;
  ADL_OVERDRIVE6_POWERCONTROLINFO_GET = function(iAdapterIndex: Integer; var lpPowerControlInfo: TADLOD6PowerControlInfo): Integer; stdcall;
  ADL_OVERDRIVE6_POWERCONTROL_GET = function(iAdapterIndex: Integer; var lpCurrentValue: Integer; var lpDefaultValue: Integer): Integer; stdcall;
  ADL_OVERDRIVE6_FANSPEED_SET = function(iAdapterIndex: Integer; var lpFanSpeedValue: TADLOD6FanSpeedValue): Integer; stdcall;
  ADL_OVERDRIVE6_STATE_SET = function(iAdapterIndex: Integer; iStateType: Integer; var lpStateInfo: TADLOD6StateInfo): Integer; stdcall;
  ADL_OVERDRIVE6_POWERCONTROL_SET = function(iAdapterIndex: Integer; iValue: Integer): Integer; stdcall;

  ADL2_MAIN_CONTROL_CREATE = function(funcMemCallback: ADL_MAIN_MALLOC_CALLBACK; iTypes: Integer; var hContext: Pointer): Integer; stdcall;
  ADL2_MAIN_CONTROL_DESTROY = function(hContext: Pointer): Integer; stdcall;

  ADL2_ADAPTER_ACTIVE_GET = function(hContext: Pointer; iNum: Integer; var iActive: Integer): Integer; stdcall;

  ADL2_OVERDRIVE_CAPS = function(hContext: Pointer; iAdapterIndex: Integer; var iSupported: Integer; var iEnabled: Integer; var iVersion: Integer): Integer; stdcall;
  ADL2_OVERDRIVEN_CAPABILITIESX2_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpCapabilities: TADLODNCapabilitiesX2): Integer; stdcall;
  ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpStatus: ADLODNPerformanceStatus): Integer; stdcall;
  ADL2_OVERDRIVEN_FANCONTROL_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpControl: ADLODNFanControl): Integer; stdcall;
  ADL2_OVERDRIVEN_FANCONTROL_SET = function(hContext: Pointer; iAdapterIndex: Integer; var lpControl: ADLODNFanControl): Integer; stdcall;
  ADL2_OVERDRIVEN_POWERLIMIT_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpLimit: ADLODNPowerLimitSetting): Integer; stdcall;
  ADL2_OVERDRIVEN_POWERLIMIT_SET = function(hContext: Pointer; iAdapterIndex: Integer; var lpLimit: ADLODNPowerLimitSetting): Integer; stdcall;
  ADL2_OVERDRIVEN_TEMPERATURE_GET = function(hContext: Pointer; iAdapterIndex: Integer; iTemperatureType: Integer; var lpTemperature: Integer): Integer; stdcall;
  ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpLevels: TADLODNPerformanceLevelsX2): Integer; stdcall;
  ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_SET = function(hContext: Pointer; iAdapterIndex: Integer; var lpLevels: TADLODNPerformanceLevelsX2): Integer; stdcall;
  ADL2_OVERDRIVEN_MEMORYCLOCKSX2_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpLevels: TADLODNPerformanceLevelsX2): Integer; stdcall;
  ADL2_OVERDRIVEN_MEMORYCLOCKSX2_SET = function(hContext: Pointer; iAdapterIndex: Integer; var lpLevels: TADLODNPerformanceLevelsX2): Integer; stdcall;

  ADL2_OVERDRIVEN_MEMORYTIMINGLEVEL_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpSupportL: Integer; var lpCurrentValue: Integer; var lpDefaultValue: Integer; var lpNumberLevels: Integer; var lppLevelList: PInteger): Integer; stdcall;
  ADL2_OVERDRIVEN_MEMORYTIMINGLEVEL_SET = function(hContext: Pointer; iAdapterIndex: Integer; currentValue: Integer): Integer; stdcall;
  ADL2_OVERDRIVEN_ZERORPMFAN_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpSupport: Integer; var lpCurrentValue: Integer; var lpDefaultValue: Integer): Integer; stdcall;
  ADL2_OVERDRIVEN_ZERORPMFAN_SET = function(hContext: Pointer; iAdapterIndex: Integer; currentValue: Integer): Integer; stdcall;

  ADL2_OVERDRIVEN_SETTINGSEXT_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpOverdriveNExtCapabilities: Integer; var lpNumberOfODNExtFeatures: Integer; var lppInitSettingList: PADLODNExtSingleInitSetting; var lppCurrentSettingList: PInteger): Integer; stdcall;
  ADL2_OVERDRIVEN_SETTINGSEXT_SET = function(hContext: Pointer; iAdapterIndex: Integer; iNumberOfODNExtFeatures: Integer; var itemValueValidList: Integer; var lpItemValueList: Integer): Integer; stdcall;

  ADL2_OVERDRIVE8_INIT_SETTING_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpSetting: TADLOD8InitSetting): Integer; stdcall;
  ADL2_OVERDRIVE8_CURRENT_SETTING_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpSetting: TADLOD8CurrentSetting): Integer; stdcall;
  ADL2_OVERDRIVE8_SETTING_SET = function(hContext: Pointer; iAdapterIndex: Integer; var lpSetting: TADLOD8SetSetting; var lpSettingCurrent: TADLOD8CurrentSetting): Integer; stdcall;
  ADL2_NEW_QUERYPMLOGDATA_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpLog: TADLPMLogDataOutput): Integer; stdcall;

  ADL2_OVERDRIVE8_INIT_SETTINGX2_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpOverdrive8Capabilities: Integer; var lpNumberOfFeatures: Integer; var lppInitSettingList: PADLOD8SingleInitSetting): Integer; stdcall;
  ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET = function(hContext: Pointer; iAdapterIndex: Integer; var lpNumberOfFeatures: Integer; var lppCurrentSettingList: PInteger): Integer; stdcall;

implementation

end.
