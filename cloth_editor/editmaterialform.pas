unit editmaterialform;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, MainForm, StrUtils, ExtCtrls;

type
  TForm3 = class(TForm)
    MaterialTypeComboBox: TComboBox;
    Label1: TLabel;
    Label2: TLabel;
    TrackBar1: TTrackBar;
    TrackBar2: TTrackBar;
    Label4: TLabel;
    TrackBar3: TTrackBar;
    CheckBox1: TCheckBox;
    CheckBox2: TCheckBox;
    Label3: TLabel;
    Edit1: TEdit;
    Button1: TButton;
    Label5: TLabel;
    Edit2: TEdit;
    Button2: TButton;
    OpenDialog1: TOpenDialog;
    Label6: TLabel;
    TrackBar4: TTrackBar;
    Button3: TButton;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    CheckBox3: TCheckBox;
    CheckBox4: TCheckBox;
    CheckBox5: TCheckBox;
    Shape1: TShape;
    Label11: TLabel;
    ColorDialog1: TColorDialog;
    Button4: TButton;
    Label12: TLabel;
    TextureFiltrationComboBox: TComboBox;
    TreatAsAlphaTestCheckBox: TCheckBox;
    procedure MaterialTypeComboBoxChange(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure TrackBar1Change(Sender: TObject);
    procedure TrackBar4Change(Sender: TObject);
    procedure TrackBar3Change(Sender: TObject);
    procedure TrackBar2Change(Sender: TObject);
    procedure CheckBox1Click(Sender: TObject);
    procedure CheckBox2Click(Sender: TObject);
    procedure CheckBox3Click(Sender: TObject);
    procedure CheckBox4Click(Sender: TObject);
    procedure CheckBox5Click(Sender: TObject);
    procedure Shape1MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Button4Click(Sender: TObject);
    procedure FormCanResize(Sender: TObject; var NewWidth, NewHeight: Integer;
      var Resize: Boolean);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form3: TForm3;

implementation

uses advice;

{$R *.dfm}

Procedure AppendMaterial(var f: TextFile);
begin
  If Form3.MaterialTypeComboBox.ItemIndex = 1 Then
    begin
      If Form3.CheckBox5.Checked Then
        begin
          WriteLn(f, '  <shader>shaders/phong_no_shadows</shader>');
        end
      else
        begin
          WriteLn(f, '  <shader>shaders/phong</shader>');
        end;
	    WriteLn(f, '  <param name="decalMap" type="texture" value="0" />');
	    WriteLn(f, '  <param name="lightness" type="predefined" value="%lightness" />');
  	  WriteLn(f, '  <param name="eyePos" type="predefined" value="%camera_position" />');
	    WriteLn(f, '  <param name="lightPos" type="predefined" value="%light_position" />');
	    WriteLn(f, '  <param name="lightColor" type="predefined" value="%light_color" />');
    end
  else If Form3.MaterialTypeComboBox.ItemIndex = 2 Then
    begin
	    WriteLn(f, '  <shader>shaders/bump_world_avatar</shader>');
  	  WriteLn(f, '  <param name="diffuseMap" type="texture" value="0" />');
	    WriteLn(f, '  <param name="normalMap" type="texture" value="1" />');
  	  WriteLn(f, '  <param name="lightness" type="predefined" value="%lightness" />');
	    WriteLn(f, '  <param name="eyePos" type="predefined" value="%camera_position" />');
  	  WriteLn(f, '  <param name="lightPos" type="predefined" value="%light_position" />');
	    WriteLn(f, '  <param name="lightColor" type="predefined" value="%light_color" />');
    end
  else if Form3.MaterialTypeComboBox.ItemIndex = 3 then
    begin
      WriteLn(f, '  <shader>shaders/bump_world_avatar_specular</shader>');
  	  WriteLn(f, '  <param name="diffuseMap" type="texture" value="0" />');
	    WriteLn(f, '  <param name="normalMap" type="texture" value="1" />');
      WriteLn(f, '  <param name="specularMap" type="texture" value="2" />');
  	  WriteLn(f, '  <param name="lightness" type="predefined" value="%lightness" />');
	    WriteLn(f, '  <param name="eyePos" type="predefined" value="%camera_position" />');
  	  WriteLn(f, '  <param name="lightPos" type="predefined" value="%light_position" />');
	    WriteLn(f, '  <param name="lightColor" type="predefined" value="%light_color" />');
    end
  else if Form3.MaterialTypeComboBox.ItemIndex = 4 Then
    begin
      WriteLn(f, '  <shader>shaders/phong_screen</shader>');
	    WriteLn(f, '  <param name="decalMap" type="texture" value="0" />');
      WriteLn(f, '  <param name="screenMap" type="texture" value="1" />');
	    WriteLn(f, '  <param name="lightness" type="predefined" value="%lightness" />');
  	  WriteLn(f, '  <param name="eyePos" type="predefined" value="%camera_position" />');
	    WriteLn(f, '  <param name="lightPos" type="predefined" value="%light_position" />');
	    WriteLn(f, '  <param name="lightColor" type="predefined" value="%light_color" />');
    end
  else If Form3.MaterialTypeComboBox.ItemIndex = 5 Then
    begin
	    WriteLn(f, '  <shader>shaders/bump_world_avatar_screen</shader>');
  	  WriteLn(f, '  <param name="diffuseMap" type="texture" value="0" />');
	    WriteLn(f, '  <param name="normalMap" type="texture" value="1" />');
      WriteLn(f, '  <param name="screenMap" type="texture" value="2" />');
  	  WriteLn(f, '  <param name="lightness" type="predefined" value="%lightness" />');
	    WriteLn(f, '  <param name="eyePos" type="predefined" value="%camera_position" />');
  	  WriteLn(f, '  <param name="lightPos" type="predefined" value="%light_position" />');
	    WriteLn(f, '  <param name="lightColor" type="predefined" value="%light_color" />');
    end
  else If Form3.MaterialTypeComboBox.ItemIndex = 6 Then
    begin
      WriteLn(f, '  <shader>shaders/phong_alphacolormask</shader>');
	    WriteLn(f, '  <param name="decalMap" type="texture" value="0" />');
	    WriteLn(f, '  <param name="lightness" type="predefined" value="%lightness" />');
  	  WriteLn(f, '  <param name="eyePos" type="predefined" value="%camera_position" />');
	    WriteLn(f, '  <param name="lightPos" type="predefined" value="%light_position" />');
	    WriteLn(f, '  <param name="lightColor" type="predefined" value="%light_color" />');
    end
  else If Form3.MaterialTypeComboBox.ItemIndex = 7 Then
    begin
	    WriteLn(f, '  <shader>shaders/bump_world_avatar_alphacolormask</shader>');
  	  WriteLn(f, '  <param name="diffuseMap" type="texture" value="0" />');
	    WriteLn(f, '  <param name="normalMap" type="texture" value="2" />');
  	  WriteLn(f, '  <param name="lightness" type="predefined" value="%lightness" />');
	    WriteLn(f, '  <param name="eyePos" type="predefined" value="%camera_position" />');
  	  WriteLn(f, '  <param name="lightPos" type="predefined" value="%light_position" />');
	    WriteLn(f, '  <param name="lightColor" type="predefined" value="%light_color" />');
    end
end;

Function MyFloatToStr(a: Real): String;
begin
  result := FloatToStr(a);
  result := AnsiReplaceText(result, ',', '.');
end;

Function GetFiltration: String;
begin
  if Form3.TextureFiltrationComboBox.Text = 'линейная' Then
    GetFiltration := '-2'
  else if Form3.TextureFiltrationComboBox.Text = 'попиксельная' Then
    GetFiltration := '-1'
  else if Form3.TextureFiltrationComboBox.Text = 'с мипмапами' Then
    GetFiltration := '0';
end;

Procedure WriteMaterial(s: string);
Type
  TColorRGBA = Array [0..3] Of Byte;
Var
  f: TextFile;
  a: ^TColorRGBA;
  b: TColor;
begin
  AssignFile(f, s);
  {$I-}
  Rewrite(f);
  {$I+}
  IF IOResult <> 0 Then Exit;
  WriteLn(f, '<material>');
  AppendMaterial(f);
  If ((Form3.MaterialTypeComboBox.ItemIndex >= 2) And (Form3.MaterialTypeComboBox.ItemIndex <= 5)) Then
    begin
      Write(f, '  <texture src="'+Form3.Edit1.Text+'" level="1"');
      WriteLn(f, ' />');
    end;
  if (Form3.MaterialTypeComboBox.ItemIndex = 3) Or
     (Form3.MaterialTypeComboBox.ItemIndex = 7) Then
    WriteLn(f, '  <texture src="'+Form3.Edit1.Text+'" level="2" />');
  if (Form3.MaterialTypeComboBox.ItemIndex = 5) then
	  WriteLn(f, '  <texture src="'+Form3.Edit2.Text+'" level="2" />');
  if ((Form3.MaterialTypeComboBox.ItemIndex >= 2) And (Form3.MaterialTypeComboBox.ItemIndex <= 3)) Or (Form3.MaterialTypeComboBox.ItemIndex = 5) Or (Form3.MaterialTypeComboBox.ItemIndex = 7) then
    WriteLn(f, '  <param name="specularPower" type="float" value="'+IntToStr(Form3.TrackBar3.Position)+'" />');
  if (Form3.MaterialTypeComboBox.ItemIndex = 2) Or (Form3.MaterialTypeComboBox.ItemIndex = 5) Or (Form3.MaterialTypeComboBox.ItemIndex = 7) then
	  WriteLn(f, '  <param name="specularIntensivity" type="float" value="'+MyFloatToStr(Form3.TrackBar4.Position / 100)+'" />');
  WriteLn(f, '  <param name="lightPower" type="float" value="'+MyFloatToStr(Form3.TrackBar1.Position / 100)+'" />');
  If Form3.CheckBox1.Checked Then
    begin
      WriteLn(f, '  <state name="alphatest" value="true" />');
      WriteLn(f, '  <state name="alphafunc" value="greater" />');
      WriteLn(f, '  <state name="alphavalue" value="'+IntToStr(Form3.TrackBar2.Position)+'" />');
    end;
  If Form1.HairRadioButton.Checked = true Then
    begin
      WriteLn(f, '  <state name="treat_as_alphatest" value="true" />');
    end;
  If Form3.CheckBox2.Checked Then
    WriteLn(f, '  <state name="two_sided" value="true" />');
  If Form3.CheckBox3.Checked Then
    WriteLn(f, '  <state name="blend" value="true" />');
  If Form3.CheckBox4.Checked Then
    WriteLn(f, '  <state name="blend" value="false" />');
  If Form3.TreatAsAlphaTestCheckBox.Checked Then
    WriteLn(f, '  <state name="treat_as_alphatest" value="true" />');
  If Form3.Shape1.Brush.Color <> clWhite Then
    begin
      b := Form3.Shape1.Brush.Color;
      a := Addr(b);
      Write(f, '  <state name="color" value="');
      Write(f, IntToStr(a^[0]));
      Write(f, ';');
      Write(f, IntToStr(a^[1]));
      Write(f, ';');
      Write(f, IntToStr(a^[2]));
      WriteLn(f, '" />');
    end;
  materials[Form3.Tag] := '1';
  WriteLn(f, '  <state name="anisotropy" value="' + GetFiltration + '" />');
  WriteLn(f, '</material>');
  CloseFile(f);
end;

procedure OnMaterialChange;
Var
  h: HWND;
  s: String;
begin
  s := Form1.ClientPathEdit.Text;
  if s[Length(s)] <> '\' then
    s := s + '\';
  s := s + 'ui\resources\avatars\editor\' + Form1.AvatarComboBox.Text;
  if Form1.ClothesRadioButton.Checked then
    s := s + '\clothes\'
  else
    s := s + '\hair\';
  s := s + Form1.ModelName.Text + '\';
  WriteMaterial(s + 'mat' + IntToStr(Form3.Tag) + '.xml');
  h := FindWindow('VSpaces virtual worlds player', nil);
  if h <> 0 then
    begin
      SendMessage(h, WM_KEYDOWN, Ord('B'), Ord('L') + 1);
    end;
end;

Procedure TypeChanged;
Var
  t: Integer;
begin
  Form3.TrackBar1Change(nil);
  Form3.TrackBar2Change(nil);
  Form3.TrackBar3Change(nil);
  Form3.TrackBar4Change(nil);

  t := Form3.MaterialTypeComboBox.ItemIndex;

  if t <> 0 then
    begin
      Form3.Label2.Enabled := true;
      Form3.TrackBar1.Enabled := true;
    end
  else
    begin
      Form3.Label2.Enabled := false;
      Form3.TrackBar1.Enabled := false;
    end;

  if (t = 2) Or (t = 3) Or (t = 5) or (t = 7) then
    begin
      Form3.Label4.Enabled := true;
      Form3.TrackBar3.Enabled := true;
    end
  else
    begin
      Form3.Label4.Enabled := false;
      Form3.TrackBar3.Enabled := false;
    end;

  if t <> 0 then
    begin
      Form3.TrackBar2.Enabled := true;
    end
  else
    begin
      Form3.TrackBar2.Enabled := false;
    end;

  if t <> 0 then
    begin
      Form3.CheckBox1.Enabled := true;
    end
  else
    begin
      Form3.CheckBox1.Enabled := false;
    end;

  if t <> 0 then
    Form3.CheckBox2.Enabled := true
  else
    Form3.CheckBox2.Enabled := false;

  if ((t >= 2) And (t <= 5)) Or (t = 7) then
    begin
      Form3.Label3.Enabled := true;
      Form3.Edit1.Enabled := true;
      Form3.Button1.Enabled := true;
    end
  else
    begin
      Form3.Label3.Enabled := false;
      Form3.Edit1.Enabled := false;
      Form3.Button1.Enabled := false;
    end;

  if (t = 3) Or (t = 5) then
    begin
      Form3.Label5.Enabled := true;
      Form3.Edit2.Enabled := true;
      Form3.Button2.Enabled := true;
    end
  else
    begin
      Form3.Label5.Enabled := false;
      Form3.Edit2.Enabled := false;
      Form3.Button2.Enabled := false;
    end;

  if (t = 2) Or (t = 5) Or (t = 7) then
    begin
      Form3.Label6.Enabled := true;
      Form3.TrackBar4.Enabled := true;
    end
  else
    begin
      Form3.Label6.Enabled := false;
      Form3.TrackBar4.Enabled := false;
    end;

  If t = 4 Then
    Form3.Label3.Caption := 'Текстура нашлепки'
  else
    Form3.Label3.Caption := 'Карта нормалей';

  If t = 5 Then
    Form3.Label5.Caption := 'Текстура нашлепки'
  else
    Form3.Label5.Caption := 'Карта блеска';

  if (t = 6) Or (t = 7) then
    begin
      Form3.CheckBox3.Checked := false;
      Form3.CheckBox4.Checked := true;
      Form3.CheckBox3.Enabled := false;
      Form3.CheckBox4.Enabled := false;
    end
  else
    begin
      Form3.CheckBox3.Enabled := true;
      Form3.CheckBox4.Enabled := true;
    end;

  OnMaterialChange;
end;

procedure TForm3.Button1Click(Sender: TObject);
begin
  OpenDialog1.InitialDir := programDir;
  If OpenDialog1.Execute Then
    begin
      Edit1.Text := ExtractFileName(OpenDialog1.FileName);
      Form1.CopyModelTextures(Form1.GetModelPath);
      OnMaterialChange;
    end;
end;

procedure TForm3.Button2Click(Sender: TObject);
begin
  OpenDialog1.InitialDir := programDir;
  If OpenDialog1.Execute Then
    begin
      Edit2.Text := ExtractFileName(OpenDialog1.FileName);
      Form1.CopyModelTextures(Form1.GetModelPath);
      OnMaterialChange;
    end;
end;

procedure TForm3.Button3Click(Sender: TObject);
Type
  TColorRGBA = Array [0..3] Of Byte;
Var
  f: TextFile;
  h: HWND;
  a: ^TColorRGBA;
  b: TColor;

begin
  AssignFile(f, programDir + '\mat' + IntToStr(Form3.Tag) + '.cfg');
  Rewrite(f);
  WriteLn(f, MaterialTypeComboBox.ItemIndex);
  WriteLn(f, TrackBar1.Position);
  WriteLn(f, TrackBar2.Position);
  WriteLn(f, TrackBar3.Position);
  WriteLn(f, TrackBar4.Position);
  If Edit1.Enabled Then
    WriteLn(f, Edit1.Text)
  else
    WriteLn(f, '');
  If Edit2.Enabled Then
    WriteLn(f, Edit2.Text)
  else
    WriteLn(f, '');
  WriteLn(f, CheckBox1.Checked);
  WriteLn(f, CheckBox2.Checked);
  WriteLn(f, CheckBox3.Checked);
  WriteLn(f, CheckBox4.Checked);
  WriteLn(f, CheckBox5.Checked);
  b := Form3.Shape1.Brush.Color;
  a := Addr(b);
  WriteLn(f, a^[0]);
  WriteLn(f, a^[1]);
  WriteLn(f, a^[2]);
  WriteLn(f, TextureFiltrationComboBox.ItemIndex);
  WriteLn(f, TreatAsAlphaTestCheckBox.Checked);
  CloseFile(f);

  WriteMaterial(programDir + '\mat' + IntToStr(Form3.Tag) + '.xml');

  Form3.Close;
end;

procedure TForm3.Button4Click(Sender: TObject);
begin
  Form5.ShowModal;
end;

procedure TForm3.CheckBox1Click(Sender: TObject);
begin
  OnMaterialChange;
end;

procedure TForm3.CheckBox2Click(Sender: TObject);
begin
  OnMaterialChange;
end;

procedure TForm3.CheckBox3Click(Sender: TObject);
begin
  CheckBox4.Checked := not CheckBox3.Checked;
  OnMaterialChange;
end;

procedure TForm3.CheckBox4Click(Sender: TObject);
begin
  CheckBox3.Checked := not CheckBox4.Checked;
  OnMaterialChange;
end;

procedure TForm3.CheckBox5Click(Sender: TObject);
begin
  OnMaterialChange;
end;

procedure TForm3.MaterialTypeComboBoxChange(Sender: TObject);
begin
  TypeChanged;
end;

procedure TForm3.FormCanResize(Sender: TObject; var NewWidth,
  NewHeight: Integer; var Resize: Boolean);
begin
  resize := false;
end;

procedure TForm3.FormShow(Sender: TObject);
Var
  f: TextFile;
  s, colorR, colorG, colorB: String;
  index: Integer;
begin
  AssignFile(f, programDir + '\mat' + IntToStr(Form3.Tag) + '.cfg');
  {$I-}
  Reset(f);
  {$I+}
  If IOResult <> 0 Then
    begin
      MaterialTypeComboBox.ItemIndex := 0;
      TypeChanged;
      Exit;
    end;
  ReadLn(f, s);
  MaterialTypeComboBox.ItemIndex := StrToInt(s);
  ReadLn(f, s);
  TrackBar1.Position := StrToInt(s);
  ReadLn(f, s);
  TrackBar2.Position := StrToInt(s);
  ReadLn(f, s);
  TrackBar3.Position := StrToInt(s);
  ReadLn(f, s);
  TrackBar4.Position := StrToInt(s);
  ReadLn(f, s);
  Edit1.Text := s;
  ReadLn(f, s);
  Edit2.Text := s;
  ReadLn(f, s);
  CheckBox1.Checked := (s = 'TRUE');
  ReadLn(f, s);
  CheckBox2.Checked := (s = 'TRUE');
  ReadLn(f, s);
  CheckBox3.Checked := (s = 'TRUE');
  ReadLn(f, s);
  CheckBox4.Checked := (s = 'TRUE');
  ReadLn(f, s);
  CheckBox5.Checked := (s = 'TRUE');
  if s = '' then
    CheckBox5.Checked := true;
  ReadLn(f, colorR);
  ReadLn(f, colorG);
  ReadLn(f, colorB);
  if (colorR = '') Then
    colorR := '255';
  if (colorG = '') Then
    colorG := '255';
  if (colorB = '') Then
    colorB := '255';
  Form3.Shape1.Brush.Color := rgb(StrToInt(colorR), StrToInt(colorG),
    StrToInt(colorB));
  ReadLn(f, s);
  if (s = '') Then
    begin
      Form3.TextureFiltrationComboBox.ItemIndex := 0;
    end
  else
    begin
      index := StrToInt(s);
      if index = -1 Then
        begin
          index := 0;
        end;
      Form3.TextureFiltrationComboBox.ItemIndex := index;
    end;
  ReadLn(f, s);
  if (s = '') Then
    begin
      TreatAsAlphaTestCheckBox.Checked := false;
    end
  else
    begin
      TreatAsAlphaTestCheckBox.Checked := (s = 'TRUE');
    end;
  CloseFile(f);

  TypeChanged;
end;

procedure TForm3.TrackBar1Change(Sender: TObject);
begin
  Label7.Caption := IntToStr(TrackBar1.Position) + ' %';
  OnMaterialChange;
end;

procedure TForm3.TrackBar2Change(Sender: TObject);
begin
  Label10.Caption := IntToStr(TrackBar2.Position);
  OnMaterialChange;
end;

procedure TForm3.TrackBar3Change(Sender: TObject);
begin
  Label9.Caption := IntToStr(TrackBar3.Position);
  OnMaterialChange;
end;

procedure TForm3.TrackBar4Change(Sender: TObject);
begin
  Label8.Caption := IntToStr(TrackBar4.Position) + ' %';
  OnMaterialChange;
end;

procedure TForm3.Shape1MouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  ColorDialog1.Color := Shape1.Brush.Color; 
  If ColorDialog1.Execute Then
    begin
      Shape1.Brush.Color := ColorDialog1.Color;
      OnMaterialChange;
    end;
end;

end.
