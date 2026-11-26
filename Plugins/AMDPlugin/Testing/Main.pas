unit Main;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.ComCtrls, adl, d3dkmt;

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
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure tmrUpdateTimer(Sender: TObject);
    procedure cbAdapterChange(Sender: TObject);
    procedure TrackBar1Change(Sender: TObject);
  private
    FADL: TADL;
    FD3DKMT: TD3DKMTStatistics;
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
end;

procedure TfrmMain.FormCreate(Sender: TObject);
var
  I: Integer;
begin
  FADL:=TADL.Create;
  FADL.Update;

  for I:=0 to FADL.AdapterCount - 1 do
    cbAdapter.Items.Add(Format('[%d] %s', [I, FADL[I].Name]));
end;

procedure TfrmMain.FormDestroy(Sender: TObject);
begin
  FADL.Free;
end;

procedure TfrmMain.tmrUpdateTimer(Sender: TObject);
begin
  FADL.Update;

  if cbAdapter.ItemIndex >= 0 then
  begin
    edTemp.Text:=IntToStr(FADL.Adapters[cbAdapter.ItemIndex].Temp);
    edFan.Text:=IntToStr(FADL.Adapters[cbAdapter.ItemIndex].FanRPM);
    edLoad.Text:=IntToStr(FADL.Adapters[cbAdapter.ItemIndex].Activity);
    edClock.Text:=IntToStr(FADL.Adapters[cbAdapter.ItemIndex].Clock);
    edMemory.Text:=IntToStr(FADL.Adapters[cbAdapter.ItemIndex].Memory);
    edVRAM.Text:=IntToStr(Round(FADL.Adapters[cbAdapter.ItemIndex].MemorySize / 1024 / 1024));
  end;

  if Assigned(FD3DKMT) then
  begin
    FD3DKMT.Update;

    lblMemoryUsage.Caption:=IntToStr(Round(FD3DKMT.MemoryUsage / 1024 / 1024));
    lblSharedLimit.Caption:=IntToStr(Round(FD3DKMT.SharedLimit / 1024 / 1024));
    lblDedicatedLimit.Caption:=IntToStr(Round(FD3DKMT.DedicatedLimit / 1024 / 1024));
    lblSharedUsage.Caption:=IntToStr(Round(FD3DKMT.SharedUsage / 1024 / 1024));
    lblDedicatedUsage.Caption:=IntToStr(Round(FD3DKMT.DedicatedUsage / 1024 / 1024));
  end;
end;

procedure TfrmMain.TrackBar1Change(Sender: TObject);
begin
  tmrUpdate.Interval:=TrackBar1.Position;
end;

end.
