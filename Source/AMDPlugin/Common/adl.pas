{*******************************************************}
{                                                       }
{       TADL Class for the AMD Display Library          }
{                                                       }
{       Version 0.2                                     }
{                                                       }
{       2020-10-05 - 0.2                                }
{         Added PNP String to TADLAdapter               }
{       2020-10-03 - 0.1                                }
{         Initial Release                               }
{                                                       }
{       (c) 2020 by NaliLord                            }
{                                                       }
{*******************************************************}

unit adl;

interface

uses
  Winapi.Windows, System.SysUtils, System.Classes, System.Win.Registry, System.Generics.Collections, adl_structures, adl_defines, adl_sdk;

type
  TADLAdapter = class
  private
    FUpdate: Boolean;
    FPNP: String;
    FIndex: Integer;
    FBiosPartNumber: String;
    FBiosVersion: String;
    FBiosDate: String;
    FMemorySize: Int64;
    FMemoryType: String;
    FMemoryBandwidth: Int64;
    FName: String;
    FDisplay: String;
    FBusNumber: Integer;
    FDeviceNumber: Integer;
    FFunctionNumber: Integer;
    FTemp: Byte;
    FFan: Byte;
    FFanRPM: Integer;
    FFanMaxRPM: Integer;
    FActivity: Byte;
    FVddc: Double;
    FVddcMax: Double;
    FClock: Integer;
    FClockMax: Integer;
    FMemory: Integer;
    FMemoryMax: Integer;
    FPerformanceLevel: Integer;
    FBusSpeed: Integer;
    FBusLanes: Integer;
    FBusLanesMax: Integer;
    FDriverDate: String;
    FDriverVersion: String;
  public
    function IsLocation(ABusNumber, ADeviceNumber, AFunctionNumber: Integer): Boolean;
    property PNP: String read FPNP;
    property BiosPartNumber: String read FBiosPartNumber;
    property BiosVersion: String read FBiosVersion;
    property BiosDate: String read FBiosDate;
    property MemorySize: Int64 read FMemorySize;
    property MemoryType: String read FMemoryType;
    property MemoryBandwidth: Int64 read FMemoryBandwidth;
    property Name: String read FName;
    property Display: String read FDisplay;
    property BusNumber: Integer read FBusNumber;
    property DeviceNumber: Integer read FDeviceNumber;
    property FunctionNumber: Integer read FFunctionNumber;
    property Temp: Byte read FTemp;
    property Fan: Byte read FFan;
    property FanRPM: Integer read FFanRPM;
    property FanMaxRPM: Integer read FFanMaxRPM;
    property Activity: Byte read FActivity;
    property Vddc: Double read FVddc;
    property VddcMax: Double read FVddcMax;
    property Clock: Integer read FClock;
    property ClockMax: Integer read FClockMax;
    property Memory: Integer read FMemory;
    property MemoryMax: Integer read FMemoryMax;
    property PerformanceLevel: Integer read FPerformanceLevel;
    property BusSpeed: Integer read FBusSpeed;
    property BusLanes: Integer read FBusLanes;
    property BusLanesMax: Integer read FBusLanesMax;
    property DriverDate: String read FDriverDate;
    property DriverVersion: String read FDriverVersion;
  end;

  TADL = class
  private
    FInitialized: Boolean;
    FDLLHandle: NativeUInt;
    FAdapters: TObjectList<TADLAdapter>;
    FMain_Control_Create: ADL_MAIN_CONTROL_CREATE;
    FMain_Control_Destroy: ADL_MAIN_CONTROL_DESTROY;
    FAdapter_NumberOfAdapters: ADL_ADAPTER_NUMBEROFADAPTERS_GET;
    FAdapter_AdapterInfo: ADL_ADAPTER_ADAPTERINFO_GET;
    FAdapter_VideoBiosInfo: ADL_ADAPTER_VIDEOBIOSINFO_GET;
    FAdapter_MemoryInfo: ADL_ADAPTER_MEMORYINFO_GET;
    FAdapter_Active: ADL_ADAPTER_ACTIVE_GET;
    FOverdrive_Caps: ADL_OVERDRIVE_CAPS;
    FOverdrive5_ThermalDevices: ADL_OVERDRIVE5_THERMALDEVICES_ENUM;
    FOverdrive5_Temperature: ADL_OVERDRIVE5_TEMPERATURE_GET;
    FOverdrive5_FanSpeed: ADL_OVERDRIVE5_FANSPEED_GET;
    FOverdrive5_FanSpeedInfo: ADL_OVERDRIVE5_FANSPEEDINFO_GET;
    FOverdrive5_CurrentActivity: ADL_OVERDRIVE5_CURRENTACTIVITY_GET;
    FOverdrive5_ODParameters: ADL_OVERDRIVE5_ODPARAMETERS_GET;
    FMain_Control_Create2: ADL2_MAIN_CONTROL_CREATE;
    FMain_Control_Destroy2: ADL2_MAIN_CONTROL_DESTROY;
    FOverdriveN_Temperature: ADL2_OVERDRIVEN_TEMPERATURE_GET;
    FOverdrive8_Init_Setting: ADL2_OVERDRIVE8_INIT_SETTINGX2_GET;
    FOverdrive8_Current_Setting: ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET;
    FNew_QueryPMLogData: ADL2_NEW_QUERYPMLOGDATA_GET;
    FAdapterInfo: PAdapterInfo;
    FNumberAdapters: Integer;
    FADL2Context: Pointer;
    function GetAdapterCount: Integer;
    function GetAdapters(Index: Integer): TADLAdapter;
  protected
    function GetAdapaterByLocation(ABusNumber, ADeviceNumber, AFunctionNumber: Integer): TADLAdapter;
    procedure Initialize;
    procedure Finalize;
  public
    constructor Create;
    destructor Destroy; override;
    procedure Update;
    property AdapterCount: Integer read GetAdapterCount;
    property Adapters[Index: Integer]: TADLAdapter read GetAdapters; default;
  end;

implementation

uses
  System.Math, System.DateUtils, System.StrUtils, System.IOUtils;

const
  AMDVENDORID         = 1002;
  ADL_WARNING_NO_DATA = -100;

{ Global }

function ADL_Main_Memory_Alloc(iSize: Integer): Pointer; stdcall;
begin
  Result:=AllocMem(iSize);
end;

procedure ADL_Main_Memory_Free(lpBuffer: PPointer) stdcall;
begin
  if Assigned(lpBuffer^) then
  begin
    FreeMem(lpBuffer^);
    lpBuffer^:=nil;
  end;
end;

procedure Log(const AMessage: String; AParams: Array of const);
begin
  AllocConsole;
  WriteLn(Format(AMessage, AParams));
end;

{ TADL }

constructor TADL.Create;
begin
  inherited Create;

  FAdapters:=TObjectList<TADLAdapter>.Create;

  Initialize;
end;

destructor TADL.Destroy;
begin
  Finalize;

  FreeAndNil(FAdapters);
end;

procedure TADL.Initialize;
begin
  FDLLHandle:=LoadLibrary('atiadlxx.dll');
  if FDLLHandle = 0 then
    FDLLHandle:=LoadLibrary('atiadlxy.dll');
  if FDLLHandle <> 0 then
  begin
    FMain_Control_Create:=GetProcAddress(FDLLHandle, 'ADL_Main_Control_Create');
    FMain_Control_Destroy:=GetProcAddress(FDLLHandle, 'ADL_Main_Control_Destroy');
    FAdapter_NumberOfAdapters:=GetProcAddress(FDLLHandle, 'ADL_Adapter_NumberOfAdapters_Get');
    FAdapter_VideoBiosInfo:=GetProcAddress(FDLLHandle, 'ADL_Adapter_VideoBiosInfo_Get');
    FAdapter_MemoryInfo:=GetProcAddress(FDLLHandle, 'ADL_Adapter_MemoryInfo_Get');
    FAdapter_AdapterInfo:=GetProcAddress(FDLLHandle, 'ADL_Adapter_AdapterInfo_Get');
    FAdapter_Active:=GetProcAddress(FDLLHandle, 'ADL_Adapter_Active_Get');
    FOverdrive_Caps:=GetProcAddress(FDLLHandle, 'ADL_Overdrive_Caps');
    FOverdrive5_ThermalDevices:=GetProcAddress(FDLLHandle, 'ADL_Overdrive5_ThermalDevices_Enum');
    FOverdrive5_Temperature:=GetProcAddress(FDLLHandle, 'ADL_Overdrive5_Temperature_Get');
    FOverdrive5_FanSpeed:=GetProcAddress(FDLLHandle, 'ADL_Overdrive5_FanSpeed_Get');
    FOverdrive5_FanSpeedInfo:=GetProcAddress(FDLLHandle, 'ADL_Overdrive5_FanSpeedInfo_Get');
    FOverdrive5_CurrentActivity:=GetProcAddress(FDLLHandle, 'ADL_Overdrive5_CurrentActivity_Get');
    FOverdrive5_ODParameters:=GetProcAddress(FDLLHandle, 'ADL_Overdrive5_ODParameters_Get');

    FMain_Control_Create2:=GetProcAddress(FDLLHandle, 'ADL2_Main_Control_Create');
    FMain_Control_Destroy2:=GetProcAddress(FDLLHandle, 'ADL2_Main_Control_Destroy');
    FOverdriveN_Temperature:=GetProcAddress(FDLLHandle, 'ADL2_OverdriveN_Temperature_Get');
    FOverdrive8_Init_Setting:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_Init_SettingX2_Get');
    FOverdrive8_Current_Setting:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_Current_SettingX2_Get');
    FNew_QueryPMLogData:=GetProcAddress(FDLLHandle, 'ADL2_New_QueryPMLogData_Get');

    if (@FMain_Control_Create <> nil) AND
       (@FMain_Control_Destroy <> nil) AND
       (@FAdapter_NumberOfAdapters <> nil) AND
       (@FAdapter_VideoBiosInfo <> nil) AND
       (@FAdapter_MemoryInfo <> nil) AND
       (@FAdapter_AdapterInfo <> nil) AND
       (@FAdapter_Active <> nil) AND
       (@FOverdrive_Caps <> nil) AND
       (@FOverdrive5_ThermalDevices <> nil) AND
       (@FOverdrive5_Temperature <> nil) AND
       (@FOverdrive5_FanSpeed <> nil) AND
       (@FOverdrive5_FanSpeedInfo <> nil) AND
       (@FOverdrive5_CurrentActivity <> nil) AND
       (@FOverdrive5_ODParameters <> nil) AND
       (@FMain_Control_Create2 <> nil) AND
       (@FMain_Control_Destroy2 <> nil) AND
       (@FOverdriveN_Temperature <> nil) AND
       (@FOverdrive8_Init_Setting <> nil) AND
       (@FOverdrive8_Current_Setting <> nil) AND
       (@FNew_QueryPMLogData <> nil) then
    begin
      if FMain_Control_Create(ADL_Main_Memory_Alloc, 1) <> ADL_OK then
        raise Exception.Create('ADL Initialization Error!');

      if FMain_Control_Create2(ADL_Main_Memory_Alloc, 1, FADL2Context) <> ADL_OK then
        raise Exception.Create('ADL2 Initialization Error!');

      FNumberAdapters:=0;
      if FAdapter_NumberOfAdapters(FNumberAdapters) <> ADL_OK then
        raise Exception.Create('Cannot get the number of adapters!');

      if FNumberAdapters > 0 then
      begin
        FAdapterInfo:=AllocMem(SizeOf(TAdapterInfo) * FNumberAdapters);
        ZeroMemory(FAdapterInfo, SizeOf(TAdapterInfo) * FNumberAdapters);

        if FAdapter_AdapterInfo(FAdapterInfo, SizeOf(TAdapterInfo) * FNumberAdapters) <> ADL_OK then
          raise Exception.Create('Cannot get adapter info!');
      end;

      FInitialized:=True;
    end else
      Finalize;
  end;
end;

procedure TADL.Update;
var
  Registry: TRegistry;
  Adapter: TADLAdapter;
  S: ADLSensorType;
  D: ADLOD8SettingId;
  I, J, AdapterActive, IsSupported, IsEnabled, Version, Capabilities, FeatureCount: Integer;
  BiosInfo: TADLBiosInfo;
  MemoryInfo: TADLMemoryInfo;
  AdapterInfo: TAdapterInfo;
  AdapterTemp: TADLTemperature;
  AdapterFanSpeed: TADLFanSpeedValue;
  AdapterFanSpeedInfo: TADLFanSpeedInfo;
  AdapterActivity: TADLPMActivity;
  ODParameters: TADLODParameters;
  ThermalControllerInfo: TADLThermalControllerInfo;
  LogData: TADLPMLogDataOutput;
  SettingsInit: TADLOD8InitSetting;
  SettingsCurr: TADLOD8CurrentSetting;
  SettingList: PADLOD8SingleInitSetting;
begin
  if FInitialized then
  begin
    for I:=0 to FAdapters.Count - 1 do
      FAdapters[I].FUpdate:=True;

    for I:=0 to FNumberAdapters - 1 do
    begin
      AdapterInfo:=PAdapterInfo(NativeUInt(I * SizeOf(TAdapterInfo)) + NativeUInt(FAdapterInfo))^;

      FAdapter_Active(AdapterInfo.iAdapterIndex, AdapterActive);

      if (AdapterActive <> 0) AND (AdapterInfo.iVendorID = AMDVENDORID) then
      begin
        FOverdrive_Caps(AdapterInfo.iAdapterIndex, IsSupported, IsEnabled, Version);

        if (IsSupported <> 0) AND (IsEnabled <> 0) then
        begin
          Adapter:=GetAdapaterByLocation(AdapterInfo.iBusNumber, AdapterInfo.iDeviceNumber, AdapterInfo.iFunctionNumber);
          if NOT Assigned(Adapter) then
          begin
            if (FAdapter_VideoBiosInfo(AdapterInfo.iAdapterIndex, BiosInfo) = ADL_OK) AND (FAdapter_MemoryInfo(AdapterInfo.iAdapterIndex, MemoryInfo) = ADL_OK) then
            begin
              Adapter:=TADLAdapter.Create;
              try
                Adapter.FUpdate:=True;

                Adapter.FBiosPartNumber:=String(AnsiString(BiosInfo.strPartNumber));
                Adapter.FBiosVersion:=String(AnsiString(BiosInfo.strVersion));
                Adapter.FBiosDate:=String(AnsiString(BiosInfo.strDate));
                Adapter.FMemorySize:=MemoryInfo.iMemorySize;
                Adapter.FMemoryType:=String(AnsiString(MemoryInfo.strMemoryType));
                Adapter.FMemoryBandwidth:=MemoryInfo.iMemoryBandwidth;
                Adapter.FIndex:=AdapterInfo.iAdapterIndex;
                Adapter.FName:=String(AnsiString(AdapterInfo.strAdapterName));
                Adapter.FPNP:=String(AnsiString(AdapterInfo.strPNPString));
                Adapter.FDisplay:=String(AnsiString(AdapterInfo.strDisplayName));
                Adapter.FBusNumber:=AdapterInfo.iBusNumber;
                Adapter.FDeviceNumber:=AdapterInfo.iDeviceNumber;
                Adapter.FFunctionNumber:=AdapterInfo.iFunctionNumber;

                if Version >= 8 then // If this is OD8 we need to get the sensor MAX values here...
                begin
                  SettingList:=nil;
                  FeatureCount:=Integer(ADLOD8SettingId.OD8_COUNT);
                  if FOverdrive8_Init_Setting(FADL2Context, AdapterInfo.iAdapterIndex, Capabilities, FeatureCount, SettingList) = ADL_OK then
                  begin
                    SettingsInit.count:=IfThen(FeatureCount > Integer(ADLOD8SettingId.OD8_COUNT), Integer(ADLOD8SettingId.OD8_COUNT), FeatureCount);
                    SettingsInit.overdrive8Capabilities:=Capabilities;

                    CopyMemory(@SettingsInit.od8SettingTable[OD8_GFXCLK_FMAX], SettingList, Min(FeatureCount * SizeOf(TADLOD8SingleInitSetting), SizeOf(SettingsInit.od8SettingTable)));

                    Adapter.FClockMax:=SettingsInit.od8SettingTable[OD8_GFXCLK_FMAX].maxValue;
                    Adapter.FMemoryMax:=SettingsInit.od8SettingTable[OD8_UCLK_FMAX].maxValue;
                    Adapter.FFanMaxRPM:=SettingsInit.od8SettingTable[OD8_FAN_MIN_SPEED].maxValue;
                  end;
                end;

                Registry:=TRegistry.Create(KEY_READ);
                try
                  Registry.RootKey:=HKEY_LOCAL_MACHINE;
                  if Registry.OpenKey(StringReplace(String(AnsiString(PAnsiChar(@AdapterInfo.strDriverPath[0]))), '\Registry\Machine\', '', [rfIgnoreCase]), False) then
                  try
                    Adapter.FDriverDate:=Registry.ReadString('DriverDate');
                    Adapter.FDriverVersion:=Registry.ReadString('DriverVersion');
                  finally
                    Registry.CloseKey;
                  end;
                finally
                  FreeAndNil(Registry);
                end;
              finally
                FAdapters.Add(Adapter);
              end;
            end;
          end;

          if Assigned(Adapter) AND Adapter.FUpdate then
          begin
            if Version >= 8 then
            begin
              ZeroMemory(@LogData, SizeOf(LogData));
              if FNew_QueryPMLogData(FADL2Context, AdapterInfo.iAdapterIndex, LogData) = ADL_OK then
              begin
                for S:=Low(ADLSensorType) to High(ADLSensorType) do
                  with LogData.sensors[Integer(S)] do
                    if supported <> 0 then
                      case S of
                        SENSOR_MAXTYPES: ;
                        PMLOG_CLK_GFXCLK: Adapter.FClock:=value;
                        PMLOG_CLK_MEMCLK: Adapter.FMemory:=value;
                        PMLOG_CLK_SOCCLK: ;
                        PMLOG_CLK_UVDCLK1: ;
                        PMLOG_CLK_UVDCLK2: ;
                        PMLOG_CLK_VCECLK: ;
                        PMLOG_CLK_VCNCLK: ;
                        PMLOG_TEMPERATURE_EDGE: ;
                        PMLOG_TEMPERATURE_MEM: ;
                        PMLOG_TEMPERATURE_VRVDDC: ;
                        PMLOG_TEMPERATURE_VRMVDD: ;
                        PMLOG_TEMPERATURE_LIQUID: ;
                        PMLOG_TEMPERATURE_PLX: ;
                        PMLOG_FAN_RPM: Adapter.FFanRPM:=value;
                        PMLOG_FAN_PERCENTAGE: Adapter.FFan:=value;
                        PMLOG_SOC_VOLTAGE: ;
                        PMLOG_SOC_POWER: ;
                        PMLOG_SOC_CURRENT: ;
                        PMLOG_INFO_ACTIVITY_GFX: Adapter.FActivity:=value;
                        PMLOG_INFO_ACTIVITY_MEM: ;
                        PMLOG_GFX_VOLTAGE: Adapter.FVddc:=value / 1000;
                        PMLOG_MEM_VOLTAGE: ;
                        PMLOG_ASIC_POWER: ;
                        PMLOG_TEMPERATURE_VRSOC: ;
                        PMLOG_TEMPERATURE_VRMVDD0: ;
                        PMLOG_TEMPERATURE_VRMVDD1: ;
                        PMLOG_TEMPERATURE_HOTSPOT: Adapter.FTemp:=value;
                        PMLOG_TEMPERATURE_GFX: ;
                        PMLOG_TEMPERATURE_SOC: ;
                        PMLOG_GFX_POWER: ;
                        PMLOG_GFX_CURRENT: ;
                        PMLOG_TEMPERATURE_CPU: ;
                        PMLOG_CPU_POWER: ;
                        PMLOG_CLK_CPUCLK: ;
                        PMLOG_THROTTLER_STATUS: ;
                        PMLOG_CLK_VCN1CLK1: ;
                        PMLOG_CLK_VCN1CLK2: ;
                        PMLOG_SMART_POWERSHIFT_CPU: ;
                        PMLOG_SMART_POWERSHIFT_DGPU: ;
                        PMLOG_BUS_SPEED: Adapter.FBusSpeed:=value;
                        PMLOG_BUS_LANES:
                        begin
                          Adapter.FBusLanes:=value;
                          Adapter.FBusLanesMax:=Max(Adapter.FBusLanes, value);
                        end;
                        PMLOG_MAX_SENSORS_REAL: ;
                      end;
              end;
            end else
            begin
              ThermalControllerInfo.iSize:=SizeOf(ThermalControllerInfo);
              if FOverdrive5_ThermalDevices(AdapterInfo.iAdapterIndex, 0, ThermalControllerInfo) = ADL_OK then
              begin

              end;

              AdapterTemp.iSize:=SizeOf(AdapterTemp);
              if FOverdrive5_Temperature(AdapterInfo.iAdapterIndex, 0, AdapterTemp) = ADL_OK then
              begin
                Adapter.FTemp:=AdapterTemp.iTemperature div 1000;
              end;

              AdapterFanSpeedInfo.iSize:=SizeOf(AdapterFanSpeedInfo);
              if FOverdrive5_FanSpeedInfo(AdapterInfo.iAdapterIndex, 0, AdapterFanSpeedInfo) = ADL_OK then
              begin
                if (AdapterFanSpeedInfo.iFlags AND ADL_DL_FANCTRL_SUPPORTS_PERCENT_READ) = ADL_DL_FANCTRL_SUPPORTS_PERCENT_READ then
                begin
                  AdapterFanSpeed.iSize:=SizeOf(AdapterFanSpeed);
                  AdapterFanSpeed.iSpeedType:=ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
                  FOverdrive5_FanSpeed(AdapterInfo.iAdapterIndex, 0, AdapterFanSpeed);
                  Adapter.FFan:=AdapterFanSpeed.iFanSpeed;
                end;

                if (AdapterFanSpeedInfo.iFlags AND ADL_DL_FANCTRL_SUPPORTS_RPM_READ) = ADL_DL_FANCTRL_SUPPORTS_RPM_READ then
                begin
                  AdapterFanSpeed.iSize:=SizeOf(AdapterFanSpeed);
                  AdapterFanSpeed.iSpeedType:=ADL_DL_FANCTRL_SPEED_TYPE_RPM;
                  FOverdrive5_FanSpeed(AdapterInfo.iAdapterIndex, 0, AdapterFanSpeed);
                  Adapter.FFanRPM:=AdapterFanSpeed.iFanSpeed;
                  if Adapter.FFan > 0 then // Try to calc the max RPM here
                    Adapter.FFanMaxRPM:=Round(Adapter.FFanRPM / Adapter.FFan * 100);
                end;
              end;

              AdapterActivity.iSize:=SizeOf(AdapterActivity);
              if FOverdrive5_CurrentActivity(AdapterInfo.iAdapterIndex, AdapterActivity) = ADL_OK then
              begin
                Adapter.FActivity:=AdapterActivity.iActivityPercent;
                Adapter.FVddc:=AdapterActivity.iVddc / 1000;
                Adapter.FClock:=AdapterActivity.iEngineClock div 100;
                Adapter.FMemory:=AdapterActivity.iMemoryClock div 100;
                Adapter.FPerformanceLevel:=AdapterActivity.iCurrentPerformanceLevel;
                Adapter.FBusSpeed:=AdapterActivity.iCurrentBusSpeed div 1000;
                Adapter.FBusLanes:=AdapterActivity.iCurrentBusLanes;
                Adapter.FBusLanesMax:=AdapterActivity.iMaximumBusLanes;
              end;

              ODParameters.iSize:=SizeOf(ODParameters);
              if FOverdrive5_ODParameters(AdapterInfo.iAdapterIndex, ODParameters) = ADL_OK then
              begin
                Adapter.FVddcMax:=ODParameters.sVddc.iMax / 1000;
                Adapter.FClockMax:=ODParameters.sEngineClock.iMax div 100;
                Adapter.FMemoryMax:=ODParameters.sMemoryClock.iMax div 100;
              end;
            end;

            Adapter.FUpdate:=False;
          end;
        end;
      end;
    end;
  end;
end;

procedure TADL.Finalize;
begin
  if FInitialized then
  begin
    FreeMem(FAdapterInfo);
    FMain_Control_Destroy;
    FMain_Control_Destroy2(FADL2Context);
  end;

  FInitialized:=False;

  if FDLLHandle <> 0 then
    FreeLibrary(FDLLHandle);

  FDLLHandle:=0;

  @FMain_Control_Create:=nil;
  @FMain_Control_Destroy:=nil;
  @FAdapter_NumberOfAdapters:=nil;
  @FAdapter_VideoBiosInfo:=nil;
  @FAdapter_MemoryInfo:=nil;
  @FAdapter_AdapterInfo:=nil;
  @FAdapter_Active:=nil;
  @FOverdrive_Caps:=nil;
  @FOverdrive5_ThermalDevices:=nil;
  @FOverdrive5_Temperature:=nil;
  @FOverdrive5_FanSpeed:=nil;
  @FOverdrive5_FanSpeedInfo:=nil;
  @FOverdrive5_CurrentActivity:=nil;
  @FOverdrive5_ODParameters:=nil;
  @FMain_Control_Create2:=nil;
  @FMain_Control_Destroy2:=nil;
  @FOverdriveN_Temperature:=nil;
  @FOverdrive8_Init_Setting:=nil;
  @FOverdrive8_Current_Setting:=nil;
  @FNew_QueryPMLogData:=nil;
end;

function TADL.GetAdapaterByLocation(ABusNumber, ADeviceNumber, AFunctionNumber: Integer): TADLAdapter;
var
  I: Integer;
begin
  Result:=nil;

  for I:=0 to FAdapters.Count - 1 do
    if FAdapters[I].IsLocation(ABusNumber, ADeviceNumber, AFunctionNumber) then
      Result:=FAdapters[I];
end;

function TADL.GetAdapterCount: Integer;
begin
  Result:=FAdapters.Count;
end;

function TADL.GetAdapters(Index: Integer): TADLAdapter;
begin
  Result:=nil;

  if (Index >= 0) AND (Index < FAdapters.Count) then
    Result:=FAdapters[Index];
end;

{ TADLAdapter }

function TADLAdapter.IsLocation(ABusNumber, ADeviceNumber, AFunctionNumber: Integer): Boolean;
begin
  Result:=(FBusNumber = ABusNumber) AND (FDeviceNumber = ADeviceNumber) AND (FDeviceNumber = AFunctionNumber);
end;

end.
