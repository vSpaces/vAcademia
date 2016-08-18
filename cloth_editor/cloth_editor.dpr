program cloth_editor;

uses
  Forms,
  mainform in 'mainform.pas' {Form1},
  materialform in 'materialform.pas' {Form2},
  editmaterialform in 'editmaterialform.pas' {Form3},
  clipboxform in 'clipboxform.pas' {Form4},
  advice in 'advice.pas' {Form5};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TForm2, Form2);
  Application.CreateForm(TForm3, Form3);
  Application.CreateForm(TForm4, Form4);
  Application.CreateForm(TForm5, Form5);
  Application.Run;
end.
