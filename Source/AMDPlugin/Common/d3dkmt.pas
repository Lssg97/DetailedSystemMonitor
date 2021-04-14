{*******************************************************}
{                                                       }
{       TD3DKMTStatistics Class for D3DKMT              }
{                                                       }
{       Version 0.1                                     }
{                                                       }
{       2020-10-05 - 0.1                                }
{         Initial Release                               }
{                                                       }
{       (c) 2020 by NaliLord                            }
{                                                       }
{*******************************************************}

unit d3dkmt;

interface

uses
  Winapi.Windows, System.SysUtils, System.Classes, d3dkmthk;

type
  TD3DKMTStatistics = class
  private
    FDeviceName: String;
    FInitialized: Boolean;
    FOpenAdapter: D3DKMT_OPENADAPTERFROMDEVICENAME;
    FSegments: Cardinal;
    FNodeCount: Cardinal;
    FMemoryUsage: UInt64;
    FSharedLimit: UInt64;
    FDedicatedLimit: UInt64;
    FSharedUsage: UInt64;
    FDedicatedUsage: UInt64;
  protected
    procedure Initialize;
    procedure Finalize;
  public
    constructor Create(ADevicePNP: String);
    destructor Destroy; override;
    procedure Update;
    property MemoryUsage: UInt64 read FMemoryUsage;
    property SharedLimit: UInt64 read FSharedLimit;
    property DedicatedLimit: UInt64 read FDedicatedLimit;
    property SharedUsage: UInt64 read FSharedUsage;
    property DedicatedUsage: UInt64 read FDedicatedUsage;
  end;

implementation

{ TD3DKMTStatistics }

constructor TD3DKMTStatistics.Create(ADevicePNP: String);
begin
  inherited Create;

  FDeviceName:='\\?\' + StringReplace(ADevicePNP, '\', '#', [rfReplaceAll, rfIgnoreCase]) + '#{1CA05180-A699-450A-9A0C-DE4FBE3DDD89}';

  Initialize;
end;

destructor TD3DKMTStatistics.Destroy;
begin
  Finalize;

  inherited Destroy;
end;

procedure TD3DKMTStatistics.Finalize;
var
  CloseAdapter: D3DKMT_CLOSEADAPTER;
begin
  if FInitialized then
  begin
    CloseAdapter.hAdapter:=FOpenAdapter.hAdapter;
    D3DKMTCloseAdapter(CloseAdapter);
    FInitialized:=False;
  end;
end;

procedure TD3DKMTStatistics.Initialize;
var
  QueryStats: D3DKMT_QUERYSTATISTICS;
  QueryAdapterInfo: D3DKMT_QUERYADAPTERINFO;
  SegmentInfo: D3DKMT_SEGMENTSIZEINFO;
begin
  FInitialized:=True;

  ZeroMemory(@FOpenAdapter, SizeOf(FOpenAdapter));
  FOpenAdapter.pDeviceName:=PChar(FDeviceName);

  if Succeeded(D3DKMTOpenAdapterFromDeviceName(FOpenAdapter)) then
  begin
    ZeroMemory(@QueryStats, SizeOf(QueryStats));
    QueryStats.Typ:=D3DKMT_QUERYSTATISTICS_ADAPTER;
    QueryStats.AdapterLuid:=FOpenAdapter.AdapterLuid;

    if Succeeded(D3DKMTQueryStatistics(QueryStats)) then
    begin
      FSegments:=QueryStats.QueryResult.AdapterInformation.NbSegments;
      FNodeCount:=QueryStats.QueryResult.AdapterInformation.NodeCount;

      ZeroMemory(@QueryAdapterInfo, SizeOf(QueryAdapterInfo));
      ZeroMemory(@SegmentInfo, SizeOf(SegmentInfo));

      QueryAdapterInfo.hAdapter:=FOpenAdapter.hAdapter;
      QueryAdapterInfo.aType:=KMTQAITYPE_GETSEGMENTSIZE;
      QueryAdapterInfo.pPrivateDriverData:=@SegmentInfo;
      QueryAdapterInfo.PrivateDriverDataSize:=SizeOf(SegmentInfo);

      if Succeeded(D3DKMTQueryAdapterInfo(QueryAdapterInfo)) then
      begin
        FDedicatedLimit:=SegmentInfo.DedicatedVideoMemorySize;
        FSharedLimit:=SegmentInfo.SharedSystemMemorySize;
      end;
    end else
      Finalize;
  end else
    Finalize;
end;

procedure TD3DKMTStatistics.Update;
var
  I: Integer;
  QueryStats: D3DKMT_QUERYSTATISTICS;
  CommitLimit, BytesCommitted: UInt64;
begin
  FMemoryUsage:=0;
  FSharedUsage:=0;
  FDedicatedUsage:=0;

  if FInitialized then
  begin
    for I:=0 to FSegments - 1 do
    begin
      ZeroMemory(@QueryStats, SizeOf(QueryStats));
      QueryStats.Typ:=D3DKMT_QUERYSTATISTICS_SEGMENT;
      QueryStats.AdapterLuid:=FOpenAdapter.AdapterLuid;
      QueryStats.QuerySegment.SegmentId:=I;

      if Succeeded(D3DKMTQueryStatistics(QueryStats)) then
      begin
        Inc(FMemoryUsage, QueryStats.QueryResult.SegmentInformation.BytesResident);

        BytesCommitted:=QueryStats.QueryResult.SegmentInformation.BytesResident;
        if QueryStats.QueryResult.SegmentInformation.Aperture <> 0 then
          Inc(FSharedUsage, BytesCommitted)
        else
          Inc(FDedicatedUsage, BytesCommitted);
      end;
    end;
  end;
end;

end.
