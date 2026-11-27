unit d3dkmt;

interface

uses
  Winapi.Windows, System.SysUtils, System.Classes, System.Generics.Collections, d3dkmthk;

type
  TD3DKMTNodeEngine = record
    Id: Cardinal;
    Typ: DXGK_ENGINE_TYPE;
    Name: String;
    LastTime: Int64;
    LastQPC: Int64;
    UsagePercent: Double;
  end;

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
    FNodeEngines: TDictionary<Cardinal,TD3DKMTNodeEngine>;
    FQPCFreq: Int64;
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
    property NodeEngines: TDictionary<Cardinal,TD3DKMTNodeEngine> read FNodeEngines;
  end;

implementation

uses
  System.StrUtils;

{ Global }

const
  TDXGKEngineTypeName: Array[DXGK_ENGINE_TYPE] of String = ('Other', '3D', 'Video Decode', 'Video Encode', 'Video Processing', 'Scene Assembly', 'Copy', 'Overlay', 'Crypto', 'Video Codec', '');

{ TD3DKMTStatistics }

constructor TD3DKMTStatistics.Create(ADevicePNP: String);
begin
  inherited Create;

  FDeviceName:='\\?\' + StringReplace(ADevicePNP, '\', '#', [rfReplaceAll, rfIgnoreCase]) + '#{1CA05180-A699-450A-9A0C-DE4FBE3DDD89}';

  FNodeEngines:=TDictionary<Cardinal,TD3DKMTNodeEngine>.Create;

  if not QueryPerformanceFrequency(FQPCFreq) then
    raise Exception.Create('QueryPerformanceFrequency failed');

  Initialize;
end;

destructor TD3DKMTStatistics.Destroy;
begin
  Finalize;
  FreeAndNil(FNodeEngines);
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
  I: Integer;
  QueryStats: D3DKMT_QUERYSTATISTICS;
  QueryAdapterInfo: D3DKMT_QUERYADAPTERINFO;
  SegmentInfo: D3DKMT_SEGMENTSIZEINFO;
  NodeMetaData: D3DKMT_NODEMETADATA;
  Engine: TD3DKMTNodeEngine;
  NowQPC: Int64;
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

      QueryPerformanceCounter(NowQPC);

      // Query nodes
      for I:=0 to FNodeCount - 1 do
      begin
        ZeroMemory(@QueryAdapterInfo, SizeOf(QueryAdapterInfo));
        ZeroMemory(@NodeMetaData, SizeOf(NodeMetaData));

        QueryAdapterInfo.hAdapter:=FOpenAdapter.hAdapter;
        QueryAdapterInfo.aType:=KMTQAITYPE_NODEMETADATA;
        QueryAdapterInfo.pPrivateDriverData:=@NodeMetaData;
        QueryAdapterInfo.PrivateDriverDataSize:=SizeOf(NodeMetaData);

        NodeMetaData.NodeOrdinalAndAdapterIndex:=(0 SHL 16) OR I;

        if Succeeded(D3DKMTQueryAdapterInfo(QueryAdapterInfo)) then
        begin
          Engine.Id:=I;
          Engine.Typ:=NodeMetaData.NodeData.EngineType;
          Engine.Name:=IfThen(Trim(String(NodeMetaData.NodeData.FriendlyName)) = '', Format('%s %d', [TDXGKEngineTypeName[NodeMetaData.NodeData.EngineType], I]), String(NodeMetaData.NodeData.FriendlyName));
          Engine.LastTime:=0;
          Engine.LastQPC:=NowQPC;
          Engine.UsagePercent:=0.0;

          FNodeEngines.Add(I, Engine);
        end;
      end;

      // Query segment size
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
  BytesCommitted: UInt64;
  Engine: TD3DKMTNodeEngine;
  NowQPC, QPCDiff: Int64;
  DeltaGPU, ElapsedHNS: Int64;
const
  HNS_PER_SEC = 10000000;
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

    QueryPerformanceCounter(NowQPC);

    for I in FNodeEngines.Keys do
    begin
      Engine:=FNodeEngines[I];

      ZeroMemory(@QueryStats, SizeOf(QueryStats));
      QueryStats.Typ:=D3DKMT_QUERYSTATISTICS_NODE;
      QueryStats.AdapterLuid:=FOpenAdapter.AdapterLuid;
      QueryStats.QueryNode.NodeId:=Engine.Id;

      if Succeeded(D3DKMTQueryStatistics(QueryStats)) then
      begin
        QPCDiff:=NowQPC - Engine.LastQPC;

        if QPCDiff > 0 then
        begin
          DeltaGPU:=QueryStats.QueryResult.NodeInformation.GlobalInformation.RunningTime.QuadPart - Engine.LastTime;
          ElapsedHNS:=(QPCDiff * HNS_PER_SEC) div FQPCFreq;

          Engine.UsagePercent:=(DeltaGPU / ElapsedHNS) * 100.0;
          Engine.LastTime:=QueryStats.QueryResult.NodeInformation.GlobalInformation.RunningTime.QuadPart;
          Engine.LastQPC:=NowQPC;
        end;

        FNodeEngines[I]:=Engine;
      end;
    end;
  end;
end;

end.

