program TestApp;

uses
  Vcl.Forms,
  Main in 'Main.pas' {frmMain},
  adl in '..\Common\adl.pas',
  adl_defines in '..\Common\adl_defines.pas',
  adl_sdk in '..\Common\adl_sdk.pas',
  adl_structures in '..\Common\adl_structures.pas',
  d3dkmthk in '..\Common\d3dkmthk.pas',
  d3dkmt in '..\Common\d3dkmt.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TfrmMain, frmMain);
  Application.Run;
end.
