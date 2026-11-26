{*******************************************************}
{                                                       }
{       AMDPlugin - Rainmeter AMD GPU Plugin            }
{                                                       }
{       Version 0.5                                     }
{                                                       }
{       2021-01-14 - 0.5                                }
{         Added support the OD8 API and 6000 Series     }
{       2020-11-08 - 0.4                                }
{         Updated the reading of the D3DKMT Shared and  }
{           Dedicated memory values                     }
{       2020-10-07 - 0.3                                }
{         Added "DriverDate" Measure                    }
{         Added "DriverVersion" Measure                 }
{         Added "SharedLimit" Measure                   }
{         Added "SharedUsage" Measure                   }
{         Added "DedicatedLimit" Measure                }
{         Added "DedicatedUsage" Measure                }
{       2020-10-05 - 0.2                                }
{         Added "MemoryUsage" Measure                   }
{       2020-10-03 - 0.1                                }
{         Initial Release                               }
{                                                       }
{       (c) 2021 by NaliLord                            }
{                                                       }
{*******************************************************}

library AMDPlugin;

uses
  Winapi.Windows,
  System.SysUtils,
  System.Classes,
  System.StrUtils,
  System.Generics.Collections,
  System.Math,
  adl in '..\Common\adl.pas',
  adl_defines in '..\Common\adl_defines.pas',
  adl_sdk in '..\Common\adl_sdk.pas',
  adl_structures in '..\Common\adl_structures.pas',
  d3dkmt in '..\Common\d3dkmt.pas',
  d3dkmthk in '..\Common\d3dkmthk.pas';

{$R *.res}

const
  MIN_UPDATE_TIME = 250;

type
  TMeasureID = (Unknown, Temperature, Clock, MemoryClock, Voltage, Activity, PerformanceLevel,
    PCIECurrentBusSpeed, PCIECurrentBusLanes, PCIEMaxBusLanes, FanSpeedRPM, FanSpeedPercent, MemoryType,
    MemorySize, MemoryUsage, SharedLimit, DedicatedLimit, SharedUsage, DedicatedUsage, MemoryBandwidth, AdapterName,
    AdapterActive, FanSpeedPercentMin, FanSpeedPercentMax, FanSpeedRPMMin, FanSpeedRPMMax, BiosDate, BiosVersion,
    BiosPartNumber, AdapterIdentifier, NumberOfAdapters, NumberOfDisplays, DriverDate, DriverVersion);

const
  MEASUREID_NAMES: Array[TMeasureID] of String = ('Unknown', 'Temperature', 'Clock', 'MemoryClock', 'Voltage', 'Activity', 'PerformanceLevel',
    'PCIECurrentBusSpeed', 'PCIECurrentBusLanes', 'PCIEMaxBusLanes', 'FanSpeedRPM', 'FanSpeedPercent', 'MemoryType',
    'MemorySize', 'MemoryUsage', 'SharedLimit', 'DedicatedLimit', 'SharedUsage', 'DedicatedUsage', 'MemoryBandwidth', 'AdapterName',
    'AdapterActive', 'FanSpeedPercentMin', 'FanSpeedPercentMax', 'FanSpeedRPMMin', 'FanSpeedRPMMax', 'BiosDate', 'BiosVersion',
    'BiosPartNumber', 'AdapterIdentifier', 'NumberOfAdapters', 'NumberOfDisplays', 'DriverDate', 'DriverVersion');

type
  TMeasure = class
  private
    FID: TMeasureID;
    FName: String;
    FAdapter: Integer;
    FD3DKMT: TD3DKMTStatistics;
  public
    property Name: String read FName write FName;
    property ID: TMeasureID read FID write FID;
    property Adapter: Integer read FAdapter write FAdapter;
    property D3DKMT: TD3DKMTStatistics read FD3DKMT write FD3DKMT;
  end;

var
  ADL: TADL = nil;
  List: TObjectList<TMeasure> = nil;
  LastADLUpdate: UInt64 = 0;
  LastD3DKMTUpdate: UInt64 = 0;

function RmReadString(rm: Pointer; option: PWideChar; defValue: PWideChar; replaceMeasures: BOOL): PWideChar; stdcall; external 'Rainmeter.dll';
function RmReadFormula(rm: Pointer; option: PWideChar; defValue: Double): Double; stdcall; external 'Rainmeter.dll';
function RmReplaceVariables(rm: Pointer; str: PWideChar): PWideChar; stdcall; external 'Rainmeter.dll';
function RmPathToAbsolute(rm: Pointer; relativePath: PWideChar): PWideChar; stdcall; external 'Rainmeter.dll';
procedure RmGet(rm: Pointer; typ: Integer); stdcall; external 'Rainmeter.dll';
procedure RmExecute(skin: Pointer; command: PWideChar); stdcall; external 'Rainmeter.dll';

procedure MeasureUpdate(AMeasure: TMeasure = nil);
begin
  if GetTickCount64 - LastADLUpdate >= MIN_UPDATE_TIME  then
  begin
    LastADLUpdate:=GetTickCount64;

    ADL.Update;
  end;

  if GetTickCount64 - LastD3DKMTUpdate >= MIN_UPDATE_TIME  then
  begin
    if Assigned(AMeasure) then
      if Assigned(AMeasure.D3DKMT) then
      begin
        LastD3DKMTUpdate:=GetTickCount64;

        AMeasure.D3DKMT.Update;
      end;
  end;
end;

procedure Initialize(var AData: Pointer; ARm: Pointer); stdcall;
var
  Measure: TMeasure;
begin
//  AllocConsole;

//  WriteLn('Initialize ->');

  if NOT Assigned(ADL) then
    ADL:=TADL.Create;
  if NOT Assigned(List) then
    List:=TObjectList<TMeasure>.Create;

  MeasureUpdate;

  Measure:=TMeasure.Create;
  try
    Measure.Name:=RmReadString(ARm, 'MeasureID', '', True);
    Measure.ID:=TMeasureID(Max(0, IndexStr(Measure.Name, MEASUREID_NAMES)));
    Measure.Adapter:=Trunc(RmReadFormula(ARm, 'AdapterID', 0));

//    WriteLn('  Name=', Measure.Name);
//    WriteLn('  Adapter=', Measure.Adapter);

    if Measure.ID IN [MemoryUsage, SharedLimit, DedicatedLimit, SharedUsage, DedicatedUsage] then
      if (Measure.Adapter >= 0) AND (Measure.Adapter < ADL.AdapterCount) then
        Measure.D3DKMT:=TD3DKMTStatistics.Create(ADL.Adapters[Measure.Adapter].PNP);
  finally
    List.Add(Measure);
  end;

  AData:=Measure;
//  WriteLn('<- Initialize');
end;

procedure Reload(AData: Pointer; ARm: Pointer; var AMaxValue: Double); stdcall;
begin
  MeasureUpdate;
end;

function Update(AData: Pointer): Double; stdcall;
var
  Measure: TMeasure;
begin
//  WriteLn('Update ->');
  Result:=0.0;

  if Assigned(AData) then
  begin
    Measure:=AData;

    MeasureUpdate(Measure);

//    WriteLn('  Measure: 0x', IntToHex(NativeInt(Measure), SizeOf(NativeInt) * 2));

    if (Measure.Adapter >= 0) AND (Measure.Adapter < ADL.AdapterCount) then
    begin
//      WriteLn(Format('  Name=%s'#13#10'  Adapter=%d'#13#10'  Count=%d', [Measure.Name, Measure.Adapter, ADL.AdapterCount]));
//      WriteLn(Format('  D3DKMT=%s', [BoolToStr(Assigned(Measure.D3DKMT), True)]));

      case Measure.ID of
        Temperature         : Result:=ADL.Adapters[Measure.Adapter].Temp;
        Clock               : Result:=ADL.Adapters[Measure.Adapter].Clock;
        MemoryClock         : Result:=ADL.Adapters[Measure.Adapter].Memory;
        Voltage             : Result:=ADL.Adapters[Measure.Adapter].Vddc;
        Activity            : Result:=ADL.Adapters[Measure.Adapter].Activity;
        PerformanceLevel    : Result:=ADL.Adapters[Measure.Adapter].PerformanceLevel;
        PCIECurrentBusSpeed : Result:=ADL.Adapters[Measure.Adapter].BusSpeed;
        PCIECurrentBusLanes : Result:=ADL.Adapters[Measure.Adapter].BusLanes;
        PCIEMaxBusLanes     : Result:=ADL.Adapters[Measure.Adapter].BusLanesMax;
        FanSpeedRPM         : Result:=ADL.Adapters[Measure.Adapter].FanRPM;
        FanSpeedPercent     : Result:=ADL.Adapters[Measure.Adapter].Fan;
        MemorySize          : Result:=ADL.Adapters[Measure.Adapter].MemorySize;
        MemoryUsage         : if Assigned(Measure.D3DKMT) then Result:=Measure.D3DKMT.MemoryUsage;
        SharedLimit         : if Assigned(Measure.D3DKMT) then Result:=Measure.D3DKMT.SharedLimit;
        DedicatedLimit      : if Assigned(Measure.D3DKMT) then Result:=Measure.D3DKMT.DedicatedLimit;
        SharedUsage         : if Assigned(Measure.D3DKMT) then Result:=Measure.D3DKMT.SharedUsage;
        DedicatedUsage      : if Assigned(Measure.D3DKMT) then Result:=Measure.D3DKMT.DedicatedUsage;
        MemoryBandwidth     : Result:=ADL.Adapters[Measure.Adapter].MemoryBandwidth;
        FanSpeedPercentMin  : Result:=0;
        FanSpeedPercentMax  : Result:=100;
        FanSpeedRPMMin      : Result:=0;
        FanSpeedRPMMax      : Result:=ADL.Adapters[Measure.Adapter].FanMaxRPM;
        NumberOfAdapters    : Result:=ADL.AdapterCount;
      end;
    end;
  end;

//  WriteLn('  Value=', FormatFloat('0.00', Result));
//  WriteLn('<- Update');
end;

function GetString(AData: Pointer): PWideChar; stdcall;
var
  Measure: TMeasure;
begin
//  WriteLn('GetString ->');
  Result:=nil;

  if Assigned(AData) then
  begin
    Measure:=AData;

    MeasureUpdate(Measure);

//    WriteLn('  Measure: 0x', IntToHex(NativeInt(Measure), SizeOf(NativeInt) * 2));

    if (Measure.Adapter >= 0) AND (Measure.Adapter < ADL.AdapterCount) then
    begin
//      WriteLn(Format('  Name=%s'#13#10'  Adapter=%d'#13#10'  Count=%d', [Measure.Name, Measure.Adapter, ADL.AdapterCount]));
//      WriteLn(Format('  D3DKMT=%s', [BoolToStr(Assigned(Measure.D3DKMT), True)]));

      case Measure.ID of
        MemoryType     : Result:=PWideChar(ADL.Adapters[Measure.Adapter].MemoryType);
        AdapterName    : Result:=PWideChar(ADL.Adapters[Measure.Adapter].Name);
        BiosDate       : Result:=PWideChar(ADL.Adapters[Measure.Adapter].BiosDate);
        BiosVersion    : Result:=PWideChar(ADL.Adapters[Measure.Adapter].BiosVersion);
        BiosPartNumber : Result:=PWideChar(ADL.Adapters[Measure.Adapter].BiosPartNumber);
        DriverDate     : Result:=PWideChar(ADL.Adapters[Measure.Adapter].DriverDate);
        DriverVersion  : Result:=PWideChar(ADL.Adapters[Measure.Adapter].DriverVersion);
      end;
    end;
  end;

//  if Assigned(Result) then
//    WriteLn('  Value=', Result)
//  else
//    WriteLn('  Value=<null>');

//  WriteLn('<- GetString');
end;

procedure Finalize(AData: Pointer); stdcall
var
  Measure: TMeasure;
begin
  if Assigned(AData) then
  begin
    Measure:=List.Extract(AData);
    if Assigned(Measure) then
      Measure.Free;
  end;

  if Assigned(List) AND (List.Count = 0) then
    FreeAndNil(List);
  if Assigned(ADL) AND NOT Assigned(List) then
    FreeAndNil(ADL);
end;

function GetPluginVersion: Cardinal; stdcall;
begin
  Result:=5;
end;

function GetPluginAuthor: PWideChar; stdcall;
begin
  Result:='NaliLord';
end;

exports
   Initialize,
   Reload,
   Update,
   GetString,
   Finalize,
   GetPluginVersion,
   GetPluginAuthor;

begin
end.
