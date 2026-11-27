{*******************************************************}
{                                                       }
{       TADL Class for the AMD Display Library          }
{                                                       }
{       Version 0.9                                     }
{                                                       }
{       2026-07-09 - 0.9                                }
{         Added shared-memory PMLog backend             }
{         Added PMLog sensor diagnostics                }
{         Improved active AMD adapter enumeration       }
{       2025-07-19 - 0.8                                }
{         Synced upstream ADL integration changes       }
{       2025-07-11 - 0.7                                }
{         Added edge/hotspot temperature split          }
{       2022-10-05 - 0.3                                }
{         Added APIVersion to TADLAdapter               }
{         Added support for OD7(N) Drivers/Cards        }
{       2020-10-05 - 0.2                                }
{         Added PNP String to TADLAdapter               }
{       2020-10-03 - 0.1                                }
{         Initial Release                               }
{                                                       }
{       (c) 2026 by NaliLord                            }
{                                                       }
{*******************************************************}

unit adl;

interface

uses
  Winapi.Windows, System.SysUtils, System.Classes, System.Win.Registry, System.Generics.Collections, adl_structures, adl_defines, adl_sdk;

type
  TADLPMLogBackend = (plbNone, plbLegacyQuery, plbSharedMemory);

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
    FTempHotspot: Integer;
    FTempEdge: Integer;
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
    FAPIVersion: Integer;
    FOverdriveSupported: Boolean;
    FOverdriveEnabled: Boolean;
    FPMLogSupported: Boolean;
    FPMLogSharedMemorySupported: Boolean;
    FPMLogBackend: TADLPMLogBackend;
    FPMLogDevice: ADL_D3DKMT_HANDLE;
    FPMLogSharedMemory: Pointer;
    FPMLogSensorCount: Integer;
    FPMLogSensorList: PInteger;
    FSensorSupported: Array[ADLSensorType] of Boolean;
    FSensorValues: Array[ADLSensorType] of Integer;
    FHasTemp: Boolean;
    FHasFan: Boolean;
    FHasFanRPM: Boolean;
    FHasActivity: Boolean;
    FHasVddc: Boolean;
    FHasClock: Boolean;
    FHasMemory: Boolean;
    FHasBusSpeed: Boolean;
    FHasBusLanes: Boolean;
  public
    function IsLocation(ABusNumber, ADeviceNumber, AFunctionNumber: Integer): Boolean;
    function SensorSupported(ASensor: ADLSensorType): Boolean;
    function SensorValue(ASensor: ADLSensorType): Integer;
    property Index: Integer read FIndex;
    property APIVersion: Integer read FAPIVersion;
    property OverdriveSupported: Boolean read FOverdriveSupported;
    property OverdriveEnabled: Boolean read FOverdriveEnabled;
    property PMLogSupported: Boolean read FPMLogSupported;
    property PMLogSharedMemorySupported: Boolean read FPMLogSharedMemorySupported;
    property PMLogBackend: TADLPMLogBackend read FPMLogBackend;
    property PMLogSensorSupported[ASensor: ADLSensorType]: Boolean read SensorSupported;
    property PMLogSensorValue[ASensor: ADLSensorType]: Integer read SensorValue;
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
    property Temp: Integer read FTempHotspot;
    property TempHotspot: Integer read FTempHotspot;
    property TempEdge: Integer read FTempEdge;
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
    property HasTemp: Boolean read FHasTemp;
    property HasFan: Boolean read FHasFan;
    property HasFanRPM: Boolean read FHasFanRPM;
    property HasActivity: Boolean read FHasActivity;
    property HasVddc: Boolean read FHasVddc;
    property HasClock: Boolean read FHasClock;
    property HasMemory: Boolean read FHasMemory;
    property HasBusSpeed: Boolean read FHasBusSpeed;
    property HasBusLanes: Boolean read FHasBusLanes;
  end;

  TADL = class
  private
    FInitialized: Boolean;
    FDLLHandle: NativeUInt;
    FNumberAdapters: Integer;
    FAdapters: TObjectList<TADLAdapter>;
    FData: Pointer;
    // -- ADL --
    FAdapterInfo: PAdapterInfo;
    FMain_Control_Create: ADL_MAIN_CONTROL_CREATE;
    FMain_Control_Destroy: ADL_MAIN_CONTROL_DESTROY;
    FAdapter_NumberOfAdapters: ADL_ADAPTER_NUMBEROFADAPTERS_GET;
    FAdapter_AdapterInfo: ADL_ADAPTER_ADAPTERINFO_GET;
    FAdapter_VideoBiosInfo: ADL_ADAPTER_VIDEOBIOSINFO_GET;
    FAdapter_MemoryInfo: ADL_ADAPTER_MEMORYINFO_GET;
    FAdapter_Active: ADL_ADAPTER_ACTIVE_GET;
    FOverdrive_Caps: ADL_OVERDRIVE_CAPS;
    // -- ADL2 --
    FADL2Context: Pointer;
    FADL2Initialized: Boolean;
    FMain_Control_Create2: ADL2_MAIN_CONTROL_CREATE;
    FMain_Control_Destroy2: ADL2_MAIN_CONTROL_DESTROY;
    FNew_QueryPMLogData: ADL2_NEW_QUERYPMLOGDATA_GET;
    FPMLog_SensorType_Support: ADL2_OVERDRIVE8_PMLOGSENORTYPE_SUPPORT_GET;
    FPMLog_ShareMemory_Support: ADL2_OVERDRIVE8_PMLOG_SHAREMEMORY_SUPPORT;
    FPMLog_ShareMemory_Start: ADL2_OVERDRIVE8_PMLOG_SHAREMEMORY_START;
    FPMLog_ShareMemory_Read: ADL2_OVERDRIVE8_PMLOG_SHAREMEMORY_READ;
    FPMLog_ShareMemory_Stop: ADL2_OVERDRIVE8_PMLOG_SHAREMEMORY_STOP;
    FPMLog_Device_Create: ADL2_DEVICE_PMLOG_DEVICE_CREATE;
    FPMLog_Device_Destroy: ADL2_DEVICE_PMLOG_DEVICE_DESTROY;
    // -- OD5 --
    FOverdrive5_ThermalDevices: ADL_OVERDRIVE5_THERMALDEVICES_ENUM;
    FOverdrive5_Temperature: ADL_OVERDRIVE5_TEMPERATURE_GET;
    FOverdrive5_FanSpeed: ADL_OVERDRIVE5_FANSPEED_GET;
    FOverdrive5_FanSpeedInfo: ADL_OVERDRIVE5_FANSPEEDINFO_GET;
    FOverdrive5_CurrentActivity: ADL_OVERDRIVE5_CURRENTACTIVITY_GET;
    FOverdrive5_ODParameters: ADL_OVERDRIVE5_ODPARAMETERS_GET;
    // -- OD6 --
    FOverdrive6_Temperature: ADL_OVERDRIVE6_TEMPERATURE_GET;
    // -- OD8 --
    FOverdrive8_Init_Setting: ADL2_OVERDRIVE8_INIT_SETTINGX2_GET;
    FOverdrive8_Current_Setting: ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET;
    // -- ODN --
    FOverdriveN_Capabilities: ADL2_OVERDRIVEN_CAPABILITIES_GET;
    FOverdriveN_Temperature: ADL2_OVERDRIVEN_TEMPERATURE_GET;
    FOverdriveN_FanControlGet: ADL2_OVERDRIVEN_FANCONTROL_GET;
    FOverdriveN_PerformanceStatus :ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET;
    // -- <<<
    function GetAdapterCount: Integer;
    function GetAdapters(Index: Integer): TADLAdapter;
    function GetPMLogBackendName(AAdapter: TADLAdapter): String;
  protected
    function GetAdapaterByLocation(ABusNumber, ADeviceNumber, AFunctionNumber: Integer): TADLAdapter;
    function ReadLegacyPMLog(AAdapterInfo: TAdapterInfo; AAdapter: TADLAdapter; var ALogData: TADLPMLogDataOutput): Boolean;
    function ReadSharedMemoryPMLog(AAdapterInfo: TAdapterInfo; AAdapter: TADLAdapter; var ALogData: TADLPMLogDataOutput): Boolean;
    procedure ApplyPMLogData(AAdapter: TADLAdapter; const ALogData: TADLPMLogDataOutput);
    procedure Initialize;
    procedure Finalize;
    procedure StopPMLog(AAdapter: TADLAdapter);
  public
    constructor Create;
    destructor Destroy; override;
    procedure Update;
    property AdapterCount: Integer read GetAdapterCount;
    property Adapters[Index: Integer]: TADLAdapter read GetAdapters; default;
    property Data: Pointer read FData write FData;
    property PMLogBackendName[AAdapter: TADLAdapter]: String read GetPMLogBackendName;
  end;

implementation

uses
  System.Math, System.DateUtils, System.StrUtils, System.IOUtils;

const
  AMDVENDORID         = 1002;
  ADL_WARNING_NO_DATA = -100;
  PMLOG_SAMPLE_RATE   = 250;

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

    FOverdrive6_Temperature:=GetProcAddress(FDLLHandle, 'ADL_Overdrive6_Temperature_Get');

    FMain_Control_Create2:=GetProcAddress(FDLLHandle, 'ADL2_Main_Control_Create');
    FMain_Control_Destroy2:=GetProcAddress(FDLLHandle, 'ADL2_Main_Control_Destroy');

    FOverdriveN_Capabilities:=GetProcAddress(FDLLHandle, 'ADL2_OverdriveN_Capabilities_Get');
    FOverdriveN_Temperature:=GetProcAddress(FDLLHandle, 'ADL2_OverdriveN_Temperature_Get');
    FOverdriveN_FanControlGet:=GetProcAddress(FDLLHandle, 'ADL2_OverdriveN_FanControl_Get');
    FOverdriveN_PerformanceStatus:=GetProcAddress(FDLLHandle, 'ADL2_OverdriveN_PerformanceStatus_Get');


    FOverdrive8_Init_Setting:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_Init_SettingX2_Get');
    FOverdrive8_Current_Setting:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_Current_SettingX2_Get');
    FNew_QueryPMLogData:=GetProcAddress(FDLLHandle, 'ADL2_New_QueryPMLogData_Get');
    FPMLog_SensorType_Support:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_PMLogSenorType_Support_Get');
    FPMLog_ShareMemory_Support:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_PMLog_ShareMemory_Support');
    FPMLog_ShareMemory_Start:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_PMLog_ShareMemory_Start');
    FPMLog_ShareMemory_Read:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_PMLog_ShareMemory_Read');
    FPMLog_ShareMemory_Stop:=GetProcAddress(FDLLHandle, 'ADL2_Overdrive8_PMLog_ShareMemory_Stop');
    FPMLog_Device_Create:=GetProcAddress(FDLLHandle, 'ADL2_Device_PMLog_Device_Create');
    FPMLog_Device_Destroy:=GetProcAddress(FDLLHandle, 'ADL2_Device_PMLog_Device_Destroy');

    if (@FMain_Control_Create <> nil) AND
       (@FMain_Control_Destroy <> nil) AND
       (@FAdapter_NumberOfAdapters <> nil) AND
       (@FAdapter_AdapterInfo <> nil) AND
       (@FAdapter_Active <> nil) then
    begin
      if FMain_Control_Create(ADL_Main_Memory_Alloc, 1) <> ADL_OK then
        raise Exception.Create('ADL Initialization Error!');

      if (@FMain_Control_Create2 <> nil) AND
         (@FMain_Control_Destroy2 <> nil) then
        FADL2Initialized:=FMain_Control_Create2(ADL_Main_Memory_Alloc, 1, FADL2Context) = ADL_OK;

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
  I, AdapterActive, IsSupported, IsEnabled, Version, Caps, FeatureCount: Integer;
  PMLogUpdated: Boolean;
  BiosInfo: TADLBiosInfo;
  MemoryInfo: TADLMemoryInfo;
  AdapterInfo: TAdapterInfo;
  AdapterTemp: TADLTemperature;
  AdapterFanSpeed: TADLFanSpeedValue;
  AdapterFanSpeedInfo: TADLFanSpeedInfo;
  AdapterActivity: TADLPMActivity;
  ODParameters: TADLODParameters;
  LogData: TADLPMLogDataOutput;
  SettingsInit: TADLOD8InitSetting;
  SettingList: PADLOD8SingleInitSetting;
  FanControl: TADLODNFanControl;
  PerformanceStats: TADLODNPerformanceStatus;
  Capabilities: TADLODNCapabilities;
begin
  if FInitialized then
  begin
    for I:=0 to FAdapters.Count - 1 do
      FAdapters[I].FUpdate:=True;

    for I:=0 to FNumberAdapters - 1 do
    begin
      AdapterInfo:=PAdapterInfo(NativeUInt(I * SizeOf(TAdapterInfo)) + NativeUInt(FAdapterInfo))^;

      AdapterActive:=0;
      IsSupported:=0;
      IsEnabled:=0;
      Version:=0;

      if FAdapter_Active(AdapterInfo.iAdapterIndex, AdapterActive) <> ADL_OK then
        AdapterActive:=0;

      if (AdapterActive <> 0) AND (AdapterInfo.iVendorID = AMDVENDORID) then
      begin
        if (@FOverdrive_Caps <> nil) then
          FOverdrive_Caps(AdapterInfo.iAdapterIndex, IsSupported, IsEnabled, Version);

        Adapter:=GetAdapaterByLocation(AdapterInfo.iBusNumber, AdapterInfo.iDeviceNumber, AdapterInfo.iFunctionNumber);
        if NOT Assigned(Adapter) then
        begin
          Adapter:=TADLAdapter.Create;
          try
            Adapter.FUpdate:=True;
            Adapter.FIndex:=AdapterInfo.iAdapterIndex;
            Adapter.FName:=String(AnsiString(AdapterInfo.strAdapterName));
            Adapter.FPNP:=String(AnsiString(AdapterInfo.strPNPString));
            Adapter.FDisplay:=String(AnsiString(AdapterInfo.strDisplayName));
            Adapter.FBusNumber:=AdapterInfo.iBusNumber;
            Adapter.FDeviceNumber:=AdapterInfo.iDeviceNumber;
            Adapter.FFunctionNumber:=AdapterInfo.iFunctionNumber;

            if (@FAdapter_VideoBiosInfo <> nil) AND
               (FAdapter_VideoBiosInfo(AdapterInfo.iAdapterIndex, BiosInfo) = ADL_OK) then
            begin
              Adapter.FBiosPartNumber:=String(AnsiString(BiosInfo.strPartNumber));
              Adapter.FBiosVersion:=String(AnsiString(BiosInfo.strVersion));
              Adapter.FBiosDate:=String(AnsiString(BiosInfo.strDate));
            end;

            if (@FAdapter_MemoryInfo <> nil) AND
               (FAdapter_MemoryInfo(AdapterInfo.iAdapterIndex, MemoryInfo) = ADL_OK) then
            begin
              Adapter.FMemorySize:=MemoryInfo.iMemorySize;
              Adapter.FMemoryType:=String(AnsiString(MemoryInfo.strMemoryType));
              Adapter.FMemoryBandwidth:=MemoryInfo.iMemoryBandwidth;
            end;

            if (IsSupported <> 0) AND (IsEnabled <> 0) then
            begin
              if Version >= 8 then // If this is OD8 we need to get the sensor MAX values here...
              begin
                SettingList:=nil;
                FeatureCount:=Integer(ADLOD8SettingId.OD8_COUNT);
                if (@FOverdrive8_Init_Setting <> nil) AND
                   FADL2Initialized AND
                   (FOverdrive8_Init_Setting(FADL2Context, AdapterInfo.iAdapterIndex, Caps, FeatureCount, SettingList) = ADL_OK) then
                begin
                  SettingsInit.count:=IfThen(FeatureCount > Integer(ADLOD8SettingId.OD8_COUNT), Integer(ADLOD8SettingId.OD8_COUNT), FeatureCount);
                  SettingsInit.overdrive8Capabilities:=Caps;

                  CopyMemory(@SettingsInit.od8SettingTable[OD8_GFXCLK_FMAX], SettingList, Min(FeatureCount * SizeOf(TADLOD8SingleInitSetting), SizeOf(SettingsInit.od8SettingTable)));

                  Adapter.FClockMax:=SettingsInit.od8SettingTable[OD8_GFXCLK_FMAX].maxValue;
                  Adapter.FMemoryMax:=SettingsInit.od8SettingTable[OD8_UCLK_FMAX].maxValue;
                  Adapter.FFanMaxRPM:=SettingsInit.od8SettingTable[OD8_FAN_MIN_SPEED].maxValue;
                end;
              end else
              if Version = 7 then
              begin
                if (@FOverdriveN_Capabilities <> nil) AND
                   FADL2Initialized AND
                   (FOverdriveN_Capabilities(FADL2Context, AdapterInfo.iAdapterIndex, Capabilities) = ADL_OK) then
                begin
                  Adapter.FClockMax:=Capabilities.sEngineClockRange.iMax;
                  Adapter.FMemoryMax:=Capabilities.sMemoryClockRange.iMax;
                  Adapter.FVddcMax:=Capabilities.svddcRange.iMax;
                  Adapter.FFanMaxRPM:=Capabilities.fanSpeed.iMax;
                end;
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

        if Assigned(Adapter) then
        begin
          Adapter.FOverdriveSupported:=IsSupported <> 0;
          Adapter.FOverdriveEnabled:=IsEnabled <> 0;
          Adapter.FAPIVersion:=Version;
        end;

        if Assigned(Adapter) AND Adapter.FUpdate then
        begin
          PMLogUpdated:=ReadSharedMemoryPMLog(AdapterInfo, Adapter, LogData);
          if NOT PMLogUpdated then
            PMLogUpdated:=ReadLegacyPMLog(AdapterInfo, Adapter, LogData);

          if PMLogUpdated then
            ApplyPMLogData(Adapter, LogData)
          else
          begin
            if (IsSupported <> 0) AND (IsEnabled <> 0) AND (Version = 7) then
            begin
              if (@FOverdriveN_Temperature <> nil) AND
                 FADL2Initialized AND
                 (FOverdriveN_Temperature(FADL2Context, AdapterInfo.iAdapterIndex, Integer(CORE), Adapter.FTempHotspot) = ADL_OK) then
              begin
                Adapter.FTempHotspot:=Adapter.FTempHotspot div 1000;
                Adapter.FHasTemp:=True;
              end;

              if (@FOverdriveN_FanControlGet <> nil) AND
                 FADL2Initialized AND
                 (FOverdriveN_FanControlGet(FADL2Context, AdapterInfo.iAdapterIndex, FanControl) = ADL_OK) then
              begin
                Adapter.FFanRPM:=FanControl.iCurrentFanSpeed;
                Adapter.FHasFanRPM:=True;
                if Adapter.FFanMaxRPM > 0 then
                begin
                  Adapter.FFan:=Round(100 / Adapter.FFanMaxRPM * Adapter.FFanRPM);
                  Adapter.FHasFan:=True;
                end;
              end;

              if (@FOverdriveN_PerformanceStatus <> nil) AND
                 FADL2Initialized AND
                 (FOverdriveN_PerformanceStatus(FADL2Context, AdapterInfo.iAdapterIndex, PerformanceStats) = ADL_OK) then
              begin
                Adapter.FActivity:=PerformanceStats.iGPUActivityPercent;
                Adapter.FVddc:=PerformanceStats.iVddc / 1000;
                Adapter.FClock:=PerformanceStats.iCoreClock div 100;
                Adapter.FMemory:=PerformanceStats.iMemoryClock div 100;
                Adapter.FPerformanceLevel:=PerformanceStats.iCurrentCorePerformanceLevel;
                Adapter.FBusSpeed:=PerformanceStats.iCurrentBusSpeed div 1000;
                Adapter.FBusLanes:=PerformanceStats.iCurrentBusLanes;
                Adapter.FBusLanesMax:=PerformanceStats.iMaximumBusLanes;
                Adapter.FHasActivity:=True;
                Adapter.FHasVddc:=True;
                Adapter.FHasClock:=True;
                Adapter.FHasMemory:=True;
                Adapter.FHasBusSpeed:=True;
                Adapter.FHasBusLanes:=True;
              end;
            end else
            if (IsSupported <> 0) AND (IsEnabled <> 0) then
            begin
              AdapterTemp.iSize:=SizeOf(AdapterTemp);
              if (@FOverdrive5_Temperature <> nil) AND
                 (FOverdrive5_Temperature(AdapterInfo.iAdapterIndex, 0, AdapterTemp) = ADL_OK) then
              begin
                Adapter.FTempHotspot:=AdapterTemp.iTemperature div 1000;
                Adapter.FHasTemp:=True;
              end;

              AdapterFanSpeedInfo.iSize:=SizeOf(AdapterFanSpeedInfo);
              if (@FOverdrive5_FanSpeedInfo <> nil) AND
                 (@FOverdrive5_FanSpeed <> nil) AND
                 (FOverdrive5_FanSpeedInfo(AdapterInfo.iAdapterIndex, 0, AdapterFanSpeedInfo) = ADL_OK) then
              begin
                if (AdapterFanSpeedInfo.iFlags AND ADL_DL_FANCTRL_SUPPORTS_PERCENT_READ) = ADL_DL_FANCTRL_SUPPORTS_PERCENT_READ then
                begin
                  AdapterFanSpeed.iSize:=SizeOf(AdapterFanSpeed);
                  AdapterFanSpeed.iSpeedType:=ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
                  FOverdrive5_FanSpeed(AdapterInfo.iAdapterIndex, 0, AdapterFanSpeed);
                  Adapter.FFan:=AdapterFanSpeed.iFanSpeed;
                  Adapter.FHasFan:=True;
                end;

                if (AdapterFanSpeedInfo.iFlags AND ADL_DL_FANCTRL_SUPPORTS_RPM_READ) = ADL_DL_FANCTRL_SUPPORTS_RPM_READ then
                begin
                  AdapterFanSpeed.iSize:=SizeOf(AdapterFanSpeed);
                  AdapterFanSpeed.iSpeedType:=ADL_DL_FANCTRL_SPEED_TYPE_RPM;
                  FOverdrive5_FanSpeed(AdapterInfo.iAdapterIndex, 0, AdapterFanSpeed);
                  Adapter.FFanRPM:=AdapterFanSpeed.iFanSpeed;
                  Adapter.FHasFanRPM:=True;
                  if Adapter.FFan > 0 then // Try to calc the max RPM here
                    Adapter.FFanMaxRPM:=Round(Adapter.FFanRPM / Adapter.FFan * 100);
                end;
              end;

              AdapterActivity.iSize:=SizeOf(AdapterActivity);
              if (@FOverdrive5_CurrentActivity <> nil) AND
                 (FOverdrive5_CurrentActivity(AdapterInfo.iAdapterIndex, AdapterActivity) = ADL_OK) then
              begin
                Adapter.FActivity:=AdapterActivity.iActivityPercent;
                Adapter.FVddc:=AdapterActivity.iVddc / 1000;
                Adapter.FClock:=AdapterActivity.iEngineClock div 100;
                Adapter.FMemory:=AdapterActivity.iMemoryClock div 100;
                Adapter.FPerformanceLevel:=AdapterActivity.iCurrentPerformanceLevel;
                Adapter.FBusSpeed:=AdapterActivity.iCurrentBusSpeed div 1000;
                Adapter.FBusLanes:=AdapterActivity.iCurrentBusLanes;
                Adapter.FBusLanesMax:=AdapterActivity.iMaximumBusLanes;
                Adapter.FHasActivity:=True;
                Adapter.FHasVddc:=True;
                Adapter.FHasClock:=True;
                Adapter.FHasMemory:=True;
                Adapter.FHasBusSpeed:=True;
                Adapter.FHasBusLanes:=True;
              end;

              ODParameters.iSize:=SizeOf(ODParameters);
              if (@FOverdrive5_ODParameters <> nil) AND
                 (FOverdrive5_ODParameters(AdapterInfo.iAdapterIndex, ODParameters) = ADL_OK) then
              begin
                Adapter.FVddcMax:=ODParameters.sVddc.iMax / 1000;
                Adapter.FClockMax:=ODParameters.sEngineClock.iMax div 100;
                Adapter.FMemoryMax:=ODParameters.sMemoryClock.iMax div 100;
              end;
            end;

          end;

          Adapter.FUpdate:=False;
        end;
      end;
    end;
  end;
end;

function TADL.ReadSharedMemoryPMLog(AAdapterInfo: TAdapterInfo; AAdapter: TADLAdapter; var ALogData: TADLPMLogDataOutput): Boolean;
var
  Supported: Integer;
begin
  Result:=False;

  if NOT FADL2Initialized OR
     (@FPMLog_ShareMemory_Support = nil) OR
     (@FPMLog_ShareMemory_Start = nil) OR
     (@FPMLog_ShareMemory_Read = nil) OR
     (@FPMLog_ShareMemory_Stop = nil) OR
     (@FPMLog_Device_Create = nil) OR
     (@FPMLog_Device_Destroy = nil) then
    Exit;

  if AAdapter.FPMLogBackend = plbLegacyQuery then
    Exit;

  if AAdapter.FPMLogBackend <> plbSharedMemory then
  begin
    Supported:=0;
    if (FPMLog_ShareMemory_Support(FADL2Context, AAdapterInfo.iAdapterIndex, Supported, 0) <> ADL_OK) OR
       (Supported = 0) then
      Exit;

    AAdapter.FPMLogSharedMemorySupported:=True;

    AAdapter.FPMLogDevice:=0;
    if FPMLog_Device_Create(FADL2Context, AAdapterInfo.iAdapterIndex, AAdapter.FPMLogDevice) <> ADL_OK then
      Exit;

    AAdapter.FPMLogSensorCount:=0;
    AAdapter.FPMLogSensorList:=nil;
    if (@FPMLog_SensorType_Support <> nil) then
      FPMLog_SensorType_Support(FADL2Context, AAdapterInfo.iAdapterIndex, AAdapter.FPMLogSensorCount, AAdapter.FPMLogSensorList);

    AAdapter.FPMLogSharedMemory:=nil;
    if FPMLog_ShareMemory_Start(FADL2Context, AAdapterInfo.iAdapterIndex, PMLOG_SAMPLE_RATE, -1, nil, AAdapter.FPMLogDevice, AAdapter.FPMLogSharedMemory, 0) <> ADL_OK then
    begin
      StopPMLog(AAdapter);
      Exit;
    end;

    AAdapter.FPMLogBackend:=plbSharedMemory;
  end;

  ZeroMemory(@ALogData, SizeOf(ALogData));
  ALogData.size:=SizeOf(ALogData);
  Result:=FPMLog_ShareMemory_Read(FADL2Context, AAdapterInfo.iAdapterIndex, AAdapter.FPMLogSensorCount, AAdapter.FPMLogSensorList, AAdapter.FPMLogSharedMemory, ALogData) = ADL_OK;
  if NOT Result then
    StopPMLog(AAdapter);
end;

function TADL.ReadLegacyPMLog(AAdapterInfo: TAdapterInfo; AAdapter: TADLAdapter; var ALogData: TADLPMLogDataOutput): Boolean;
begin
  Result:=False;

  if NOT FADL2Initialized OR
     (@FNew_QueryPMLogData = nil) then
    Exit;

  ZeroMemory(@ALogData, SizeOf(ALogData));
  ALogData.size:=SizeOf(ALogData);
  Result:=FNew_QueryPMLogData(FADL2Context, AAdapterInfo.iAdapterIndex, ALogData) = ADL_OK;
  if Result then
    AAdapter.FPMLogBackend:=plbLegacyQuery;
end;

procedure TADL.ApplyPMLogData(AAdapter: TADLAdapter; const ALogData: TADLPMLogDataOutput);
var
  S: ADLSensorType;
begin
  AAdapter.FPMLogSupported:=True;
  if AAdapter.FAPIVersion = 0 then
    AAdapter.FAPIVersion:=8;

  for S:=Low(ADLSensorType) to High(ADLSensorType) do
    with ALogData.sensors[Integer(S)] do
    begin
      AAdapter.FSensorSupported[S]:=supported <> 0;
      AAdapter.FSensorValues[S]:=value;
      if supported <> 0 then
        case S of
          SENSOR_MAXTYPES: ;
          PMLOG_CLK_GFXCLK:
          begin
            AAdapter.FClock:=value;
            AAdapter.FHasClock:=True;
          end;
          PMLOG_CLK_MEMCLK:
          begin
            AAdapter.FMemory:=value;
            AAdapter.FHasMemory:=True;
          end;
          PMLOG_CLK_SOCCLK: ;
          PMLOG_CLK_UVDCLK1: ;
          PMLOG_CLK_UVDCLK2: ;
          PMLOG_CLK_VCECLK: ;
          PMLOG_CLK_VCNCLK: ;
          PMLOG_TEMPERATURE_EDGE:
          begin
            AAdapter.FTempEdge:=value;
            AAdapter.FHasTemp:=True;
          end;
          PMLOG_TEMPERATURE_MEM: ;
          PMLOG_TEMPERATURE_VRVDDC: ;
          PMLOG_TEMPERATURE_VRMVDD: ;
          PMLOG_TEMPERATURE_LIQUID: ;
          PMLOG_TEMPERATURE_PLX: ;
          PMLOG_FAN_RPM:
          begin
            AAdapter.FFanRPM:=value;
            AAdapter.FHasFanRPM:=True;
          end;
          PMLOG_FAN_PERCENTAGE:
          begin
            AAdapter.FFan:=value;
            AAdapter.FHasFan:=True;
          end;
          PMLOG_SOC_VOLTAGE: ;
          PMLOG_SOC_POWER: ;
          PMLOG_SOC_CURRENT: ;
          PMLOG_INFO_ACTIVITY_GFX:
          begin
            AAdapter.FActivity:=value;
            AAdapter.FHasActivity:=True;
          end;
          PMLOG_INFO_ACTIVITY_MEM: ;
          PMLOG_GFX_VOLTAGE:
          begin
            AAdapter.FVddc:=value / 1000;
            AAdapter.FHasVddc:=True;
          end;
          PMLOG_MEM_VOLTAGE: ;
          PMLOG_ASIC_POWER: ;
          PMLOG_TEMPERATURE_VRSOC: ;
          PMLOG_TEMPERATURE_VRMVDD0: ;
          PMLOG_TEMPERATURE_VRMVDD1: ;
          PMLOG_TEMPERATURE_HOTSPOT:
          begin
            AAdapter.FTempHotspot:=value;
            AAdapter.FHasTemp:=True;
          end;
          PMLOG_TEMPERATURE_GFX:
          begin
            if NOT AAdapter.FHasTemp then
              AAdapter.FTempEdge:=value;
            AAdapter.FHasTemp:=True;
          end;
          PMLOG_TEMPERATURE_SOC: ;
          PMLOG_GFX_POWER: ;
          PMLOG_GFX_CURRENT: ;
          PMLOG_TEMPERATURE_CPU:
          begin
            if NOT AAdapter.FHasTemp then
              AAdapter.FTempEdge:=value;
            AAdapter.FHasTemp:=True;
          end;
          PMLOG_CPU_POWER: ;
          PMLOG_CLK_CPUCLK: ;
          PMLOG_THROTTLER_STATUS: ;
          PMLOG_CLK_VCN1CLK1: ;
          PMLOG_CLK_VCN1CLK2: ;
          PMLOG_SMART_POWERSHIFT_CPU: ;
          PMLOG_SMART_POWERSHIFT_DGPU: ;
          PMLOG_BUS_SPEED:
          begin
            AAdapter.FBusSpeed:=value;
            AAdapter.FHasBusSpeed:=True;
          end;
          PMLOG_BUS_LANES:
          begin
            AAdapter.FBusLanes:=value;
            AAdapter.FBusLanesMax:=Max(AAdapter.FBusLanes, value);
            AAdapter.FHasBusLanes:=True;
          end;
          PMLOG_TEMPERATURE_LIQUID0: ;
          PMLOG_TEMPERATURE_LIQUID1: ;
          PMLOG_CLK_FCLK: ;
          PMLOG_THROTTLER_STATUS_CPU: ;
          PMLOG_SSPAIRED_ASICPOWER: ;
          PMLOG_SSTOTAL_POWERLIMIT: ;
          PMLOG_SSAPU_POWERLIMIT: ;
          PMLOG_SSDGPU_POWERLIMIT: ;
          PMLOG_TEMPERATURE_HOTSPOT_GCD: ;
          PMLOG_TEMPERATURE_HOTSPOT_MCD: ;
          PMLOG_THROTTLE_PERCENTAGE_TEMP_GFX: ;
          PMLOG_THROTTLE_PERCENTAGE_TEMP_MEM: ;
          PMLOG_THROTTLE_PERCENTAGE_TEMP_VR: ;
          PMLOG_THROTTLE_PERCENTAGE_POWER: ;
          PMLOG_THROTTLE_PERCENTAGE_TDC: ;
          PMLOG_THROTTLE_PERCENTAGE_VMAX: ;
          PMLOG_BUS_CURR_MAX_SPEED: ;
          PMLOG_RESERVED_1: ;
          PMLOG_RESERVED_2: ;
          PMLOG_RESERVED_3: ;
          PMLOG_RESERVED_4: ;
          PMLOG_RESERVED_5: ;
          PMLOG_RESERVED_6: ;
          PMLOG_RESERVED_7: ;
          PMLOG_RESERVED_8: ;
          PMLOG_RESERVED_9: ;
          PMLOG_RESERVED_10: ;
          PMLOG_RESERVED_11: ;
          PMLOG_RESERVED_12: ;
          PMLOG_CLK_NPUCLK: ;
          PMLOG_NPU_BUSY_AVG: ;
          PMLOG_BOARD_POWER: ;
          PMLOG_TEMPERATURE_INTAKE: ;
          PMLOG_MAX_SENSORS_REAL: ;
        end;
    end;
end;

procedure TADL.StopPMLog(AAdapter: TADLAdapter);
var
  SensorList: Pointer;
begin
  if NOT Assigned(AAdapter) then
    Exit;

  if (AAdapter.FPMLogBackend = plbSharedMemory) AND
     FADL2Initialized then
  begin
    if (@FPMLog_ShareMemory_Stop <> nil) AND
       (AAdapter.FPMLogDevice <> 0) then
      FPMLog_ShareMemory_Stop(FADL2Context, AAdapter.FIndex, AAdapter.FPMLogDevice);

    if (@FPMLog_Device_Destroy <> nil) AND
       (AAdapter.FPMLogDevice <> 0) then
      FPMLog_Device_Destroy(FADL2Context, AAdapter.FPMLogDevice);
  end;

  if Assigned(AAdapter.FPMLogSensorList) then
  begin
    SensorList:=AAdapter.FPMLogSensorList;
    ADL_Main_Memory_Free(@SensorList);
    AAdapter.FPMLogSensorList:=nil;
  end;

  AAdapter.FPMLogDevice:=0;
  AAdapter.FPMLogSharedMemory:=nil;
  AAdapter.FPMLogSensorCount:=0;
  AAdapter.FPMLogBackend:=plbNone;
end;

procedure TADL.Finalize;
var
  I: Integer;
begin
  if FInitialized then
  begin
    for I:=0 to FAdapters.Count - 1 do
      StopPMLog(FAdapters[I]);

    FreeMem(FAdapterInfo);
    if (@FMain_Control_Destroy <> nil) then
      FMain_Control_Destroy;
    if FADL2Initialized AND
       (@FMain_Control_Destroy2 <> nil) then
      FMain_Control_Destroy2(FADL2Context);
  end;

  FInitialized:=False;
  FADL2Initialized:=False;
  FADL2Context:=nil;

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
  @FOverdrive6_Temperature:=nil;
  @FMain_Control_Create2:=nil;
  @FMain_Control_Destroy2:=nil;
  @FOverdriveN_Capabilities:=nil;
  @FOverdriveN_Temperature:=nil;
  @FOverdriveN_FanControlGet:=nil;
  @FOverdriveN_PerformanceStatus:=nil;
  @FOverdrive8_Init_Setting:=nil;
  @FOverdrive8_Current_Setting:=nil;
  @FNew_QueryPMLogData:=nil;
  @FPMLog_SensorType_Support:=nil;
  @FPMLog_ShareMemory_Support:=nil;
  @FPMLog_ShareMemory_Start:=nil;
  @FPMLog_ShareMemory_Read:=nil;
  @FPMLog_ShareMemory_Stop:=nil;
  @FPMLog_Device_Create:=nil;
  @FPMLog_Device_Destroy:=nil;
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

function TADL.GetPMLogBackendName(AAdapter: TADLAdapter): String;
begin
  Result:='None';

  if NOT Assigned(AAdapter) then
    Exit;

  case AAdapter.PMLogBackend of
    plbNone: Result:='None';
    plbLegacyQuery: Result:='LegacyQuery';
    plbSharedMemory: Result:='SharedMemory';
  end;
end;

{ TADLAdapter }

function TADLAdapter.IsLocation(ABusNumber, ADeviceNumber, AFunctionNumber: Integer): Boolean;
begin
  Result:=(FBusNumber = ABusNumber) AND (FDeviceNumber = ADeviceNumber) AND (FFunctionNumber = AFunctionNumber);
end;

function TADLAdapter.SensorSupported(ASensor: ADLSensorType): Boolean;
begin
  Result:=FSensorSupported[ASensor];
end;

function TADLAdapter.SensorValue(ASensor: ADLSensorType): Integer;
begin
  Result:=FSensorValues[ASensor];
end;

end.
