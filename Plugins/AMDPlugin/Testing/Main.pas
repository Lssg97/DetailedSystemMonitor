unit Main;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  System.TypInfo, System.Math, Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.ComCtrls,
  adl, adl_defines, d3dkmt;

type
  TfrmMain = class(TForm)
    edTemp: TEdit;
    lblTemp: TLabel;
    edFan: TEdit;
    lblFan: TLabel;
    Label1: TLabel;
    edLoad: TEdit;
    Label2: TLabel;
    edClock: TEdit;
    Label3: TLabel;
    edMemory: TEdit;
    tmrUpdate: TTimer;
    TrackBar1: TTrackBar;
    Label4: TLabel;
    edVRAM: TEdit;
    GroupBox1: TGroupBox;
    lblMemoryUsage: TStaticText;
    lblSharedLimit: TStaticText;
    lblDedicatedLimit: TStaticText;
    lblSharedUsage: TStaticText;
    lblDedicatedUsage: TStaticText;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    cbAdapter: TComboBox;
    lstNodes: TListBox;
    memDiagnostics: TMemo;
    lblDiagnostics: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure tmrUpdateTimer(Sender: TObject);
    procedure cbAdapterChange(Sender: TObject);
    procedure TrackBar1Change(Sender: TObject);
  private
    FADL: TADL;
    FD3DKMT: TD3DKMTStatistics;
    function FormatStat(AValue: Integer; ASupported: Boolean): String;
    procedure UpdateDiagnostics;
    procedure UpdateNodes;
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;

implementation

{$R *.dfm}

procedure TfrmMain.cbAdapterChange(Sender: TObject);
begin
  if Assigned(FD3DKMT) then
    FreeAndNil(FD3DKMT);

  if cbAdapter.ItemIndex >= 0 then
    FD3DKMT:=TD3DKMTStatistics.Create(FADL.Adapters[cbAdapter.ItemIndex].PNP);

  UpdateDiagnostics;
end;

function TfrmMain.FormatStat(AValue: Integer; ASupported: Boolean): String;
begin
  if ASupported then
    Result:=IntToStr(AValue)
  else
    Result:='N/A';
end;

procedure TfrmMain.FormCreate(Sender: TObject);
var
  I: Integer;
begin
  FADL:=TADL.Create;
  FADL.Update;

  for I:=0 to FADL.AdapterCount - 1 do
    cbAdapter.Items.Add(Format('[%d] %s (V: %d)', [I, FADL[I].Name, FADL[I].APIVersion]));

  if cbAdapter.Items.Count > 0 then
  begin
    cbAdapter.ItemIndex:=0;
    cbAdapterChange(cbAdapter);
    tmrUpdateTimer(tmrUpdate);
  end else
    memDiagnostics.Lines.Text:='No active AMD adapters found.';
end;

procedure TfrmMain.FormDestroy(Sender: TObject);
begin
  if Assigned(FD3DKMT) then
    FreeAndNil(FD3DKMT);
  FADL.Free;
end;

procedure TfrmMain.tmrUpdateTimer(Sender: TObject);
var
  Adapter: TADLAdapter;
begin
  FADL.Update;

  if cbAdapter.ItemIndex >= 0 then
  begin
    Adapter:=FADL.Adapters[cbAdapter.ItemIndex];

    if Adapter.HasTemp then
      edTemp.Text:=Format('%d/%d', [Adapter.TempEdge, Adapter.TempHotspot])
    else
      edTemp.Text:='N/A';
    edFan.Text:=FormatStat(Adapter.FanRPM, Adapter.HasFanRPM);
    edLoad.Text:=FormatStat(Adapter.Activity, Adapter.HasActivity);
    edClock.Text:=FormatStat(Adapter.Clock, Adapter.HasClock);
    edMemory.Text:=FormatStat(Adapter.Memory, Adapter.HasMemory);
    if Adapter.MemorySize > 0 then
      edVRAM.Text:=IntToStr(Round(Adapter.MemorySize / 1024 / 1024))
    else
      edVRAM.Text:='N/A';
  end;

  if Assigned(FD3DKMT) then
  begin
    FD3DKMT.Update;

    lblMemoryUsage.Caption:=IntToStr(Round(FD3DKMT.MemoryUsage / 1024 / 1024));
    lblSharedLimit.Caption:=IntToStr(Round(FD3DKMT.SharedLimit / 1024 / 1024));
    lblDedicatedLimit.Caption:=IntToStr(Round(FD3DKMT.DedicatedLimit / 1024 / 1024));
    lblSharedUsage.Caption:=IntToStr(Round(FD3DKMT.SharedUsage / 1024 / 1024));
    lblDedicatedUsage.Caption:=IntToStr(Round(FD3DKMT.DedicatedUsage / 1024 / 1024));

    UpdateNodes;
  end;

  UpdateDiagnostics;
end;

procedure TfrmMain.TrackBar1Change(Sender: TObject);
begin
  tmrUpdate.Interval:=TrackBar1.Position;
end;

procedure TfrmMain.UpdateDiagnostics;
var
  Adapter: TADLAdapter;
  Lines: TStringList;
  S: ADLSensorType;
  SelStart, SelLength: Integer;
begin
  if cbAdapter.ItemIndex < 0 then
    Exit;

  if memDiagnostics.Focused then
    Exit;

  Adapter:=FADL.Adapters[cbAdapter.ItemIndex];
  if NOT Assigned(Adapter) then
    Exit;

  Lines:=TStringList.Create;
  try
    Lines.Add(Format('Adapter %d: %s', [cbAdapter.ItemIndex, Adapter.Name]));
    Lines.Add(Format('ADL index: %d', [Adapter.Index]));
    Lines.Add(Format('Location: bus %d, device %d, function %d', [Adapter.BusNumber, Adapter.DeviceNumber, Adapter.FunctionNumber]));
    Lines.Add(Format('Display: %s', [Adapter.Display]));
    Lines.Add(Format('PNP: %s', [Adapter.PNP]));
    Lines.Add(Format('Driver: %s (%s)', [Adapter.DriverVersion, Adapter.DriverDate]));
    Lines.Add(Format('Overdrive: supported=%s, enabled=%s, version=%d', [BoolToStr(Adapter.OverdriveSupported, True), BoolToStr(Adapter.OverdriveEnabled, True), Adapter.APIVersion]));
    Lines.Add(Format('PMLog: supported=%s, sharedMemorySupported=%s, backend=%s', [BoolToStr(Adapter.PMLogSupported, True), BoolToStr(Adapter.PMLogSharedMemorySupported, True), FADL.PMLogBackendName[Adapter]]));
    Lines.Add('');
    Lines.Add('Supported PMLog sensors:');

    for S:=Low(ADLSensorType) to High(ADLSensorType) do
      if Adapter.PMLogSensorSupported[S] then
        Lines.Add(Format('%2d %-34s %d', [Integer(S), GetEnumName(TypeInfo(ADLSensorType), Integer(S)), Adapter.PMLogSensorValue[S]]));

    if Lines[Lines.Count - 1] = 'Supported PMLog sensors:' then
      Lines.Add('None');

    if memDiagnostics.Lines.Text <> Lines.Text then
    begin
      SelStart:=memDiagnostics.SelStart;
      SelLength:=memDiagnostics.SelLength;
      memDiagnostics.Lines.BeginUpdate;
      try
        memDiagnostics.Lines.Assign(Lines);
        memDiagnostics.SelStart:=Min(SelStart, Length(memDiagnostics.Text));
        memDiagnostics.SelLength:=SelLength;
      finally
        memDiagnostics.Lines.EndUpdate;
      end;
    end;
  finally
    Lines.Free;
  end;
end;

procedure TfrmMain.UpdateNodes;
var
  I, TopIndex, ItemIndex: Integer;
  Lines: TStringList;
begin
  if NOT Assigned(FD3DKMT) OR lstNodes.Focused then
    Exit;

  Lines:=TStringList.Create;
  try
    for I in FD3DKMT.NodeEngines.Keys do
      Lines.Add(Format('[%d] %n%% (%s)', [FD3DKMT.NodeEngines[I].Id, FD3DKMT.NodeEngines[I].UsagePercent, FD3DKMT.NodeEngines[I].Name]));

    if lstNodes.Items.Text <> Lines.Text then
    begin
      TopIndex:=lstNodes.TopIndex;
      ItemIndex:=lstNodes.ItemIndex;
      lstNodes.Items.BeginUpdate;
      try
        lstNodes.Items.Assign(Lines);
        if lstNodes.Items.Count > 0 then
        begin
          lstNodes.TopIndex:=Min(TopIndex, lstNodes.Items.Count - 1);
          lstNodes.ItemIndex:=Min(ItemIndex, lstNodes.Items.Count - 1);
        end;
      finally
        lstNodes.Items.EndUpdate;
      end;
    end;
  finally
    Lines.Free;
  end;
end;

end.
