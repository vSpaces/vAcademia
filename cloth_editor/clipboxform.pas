unit clipboxform;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, IniFiles, MainForm, ExtCtrls;

type
  TForm4 = class(TForm)
    CheckBox2: TCheckBox;
    Label1: TLabel;
    Edit1: TEdit;
    Edit2: TEdit;
    Label2: TLabel;
    Edit3: TEdit;
    Edit4: TEdit;
    Label3: TLabel;
    Edit5: TEdit;
    Edit6: TEdit;
    Label4: TLabel;
    ComboBox1: TComboBox;
    CheckBox3: TCheckBox;
    Label5: TLabel;
    Edit7: TEdit;
    Edit8: TEdit;
    Label6: TLabel;
    Edit9: TEdit;
    Edit10: TEdit;
    Label7: TLabel;
    Edit11: TEdit;
    Edit12: TEdit;
    Label8: TLabel;
    ComboBox2: TComboBox;
    CheckBox4: TCheckBox;
    Label9: TLabel;
    Edit13: TEdit;
    Edit14: TEdit;
    Label10: TLabel;
    Edit15: TEdit;
    Edit16: TEdit;
    Label11: TLabel;
    Edit17: TEdit;
    Edit18: TEdit;
    Label12: TLabel;
    ComboBox3: TComboBox;
    CheckBox5: TCheckBox;
    Label13: TLabel;
    Edit19: TEdit;
    Edit20: TEdit;
    Label14: TLabel;
    Edit21: TEdit;
    Edit22: TEdit;
    Label15: TLabel;
    Edit23: TEdit;
    Edit24: TEdit;
    Label16: TLabel;
    ComboBox4: TComboBox;
    Button1: TButton;
    Label17: TLabel;
    Label18: TLabel;
    Label19: TLabel;
    Label20: TLabel;
    Label21: TLabel;
    Label22: TLabel;
    Label23: TLabel;
    Label24: TLabel;
    CheckBox1: TCheckBox;
    Edit25: TEdit;
    Edit26: TEdit;
    Edit27: TEdit;
    Edit28: TEdit;
    Edit29: TEdit;
    Edit30: TEdit;
    ComboBox5: TComboBox;
    Image1: TImage;
    Image2: TImage;
    Image3: TImage;
    Image4: TImage;
    Image5: TImage;
    Image6: TImage;
    Image7: TImage;
    Image8: TImage;
    Image9: TImage;
    Image10: TImage;
    Image11: TImage;
    Image12: TImage;
    CheckBox6: TCheckBox;
    procedure Button1Click(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure CheckBox2Click(Sender: TObject);
    procedure CheckBox3Click(Sender: TObject);
    procedure CheckBox4Click(Sender: TObject);
    procedure CheckBox5Click(Sender: TObject);
    procedure Edit2Change(Sender: TObject);
    procedure Edit1Change(Sender: TObject);
    procedure SaveClipboxes(sini: Boolean);
    procedure ComboBox1Change(Sender: TObject);
    procedure CheckBox6Click(Sender: TObject);
    procedure UpdateClipboxs;
    procedure UpdatePermittedClipboxs;
    procedure FormCanResize(Sender: TObject; var NewWidth, NewHeight: Integer;
      var Resize: Boolean);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  TClipBox = Record
    id: Integer;
    x1, x2: Real;
    y1, y2: Real;
    z1, z2: Real;
    enabled: Boolean;
  End;

  TClipboxIDsArray = Array [1..10] Of Integer;

var
  Form4: TForm4;
  doNotExportClipboxs: Boolean;
  clipBoxs: Array [1..5] Of TClipBox;

implementation

{$R *.dfm}

Function GetPermittedClipboxs(s: string): TClipboxIDsArray;
Var
  res: TClipboxIDsArray;
begin
  if s = 'super_dress' then
    begin
      res[1] := 1;
      res[2] := 2;
      res[3] := 3;
      res[4] := 5;
      res[5] := 9;
      res[6] := 0;
    end
  else if s = 'dress' then
    begin
      res[1] := 1;
      res[2] := 3;
      res[3] := 4;
      res[4] := 5;
      res[5] := 6;
      res[6] := 0;
    end
  else if (s = 'tors') OR (s = 'hands') then
    begin
      res[1] := 3;
      res[2] := 4;
      res[3] := 5;
      res[4] := 6;
      res[5] := 0;
    end
  else if s = 'legs' then
    begin
      res[1] := 1;
      res[2] := 0;
    end
  else if s = 'hair' then
    begin
      res[1] := 9;
      res[2] := 0;
    end
  else if (s = 'socks') OR (s = 'boots') then
    begin
      res[1] := 2;
      res[2] := 7;
      res[3] := 0;
    end
  else
    begin
      res[1] := 0;
    end;

  GetPermittedClipboxs := res;
end;

Function IsNumeric(s: String): Boolean;
Var
  r: Boolean;
  i: Integer;
begin
  r := true;
  For i := 1 To length(s) Do
  if ((Ord(s[i]) < Ord('0')) Or (Ord(s[i]) > Ord('9'))) And
    (s[i] <> '-') And (s[i] <> ',') Then
    r := false;
  IsNumeric := r;
end;

Procedure SetClipbox(var a: TIniFile; num, id: Integer; enabled: Boolean; x1, x2, y1, y2, z1, z2: String);
Var
  _x1, _x2, _y1, _y2, _z1, _z2: Real;
  n: String;
begin
  if (x1 <> '') And (x1 <> '-') And (IsNumeric(x1)) then
    _x1 := StrToFloat(x1)
  else
    _x1 := 10000;
  if (x2 <> '') And (x2 <> '-') And (IsNumeric(x2)) then
    _x2 := StrToFloat(x2)
  else
    _x2 := -10000;

  if (y1 <> '') And (y1 <> '-') And (IsNumeric(y1)) then
    _y1 := StrToFloat(y1)
  else
    _y1 := 10000;
  if (y2 <> '') And (y2 <> '-') And (IsNumeric(y2)) then
    _y2 := StrToFloat(y2)
  else
    _y2 := -10000;

  if (z1 <> '') And (z1 <> '-') And (IsNumeric(z1)) then
    _z1 := StrToFloat(z1)
  else
    _z1 := 10000;
  if (z2 <> '') And (z2 <> '-') And (IsNumeric(z2)) then
    _z2 := StrToFloat(z2)
  else
    _z2 := -10000;

  n := 'box' + IntToStr(num);
  if a <> nil then
    begin
      a.WriteString(n, 'id', IntToStr(id));
      a.WriteString(n, 'x1', x1);
      a.WriteString(n, 'x2', x2);
      a.WriteString(n, 'y1', y1);
      a.WriteString(n, 'y2', y2);
      a.WriteString(n, 'z1', z1);
      a.WriteString(n, 'z2', z2);
    end;

  clipBoxs[num].enabled := enabled;
  clipBoxs[num].x1 := _x2;
  clipBoxs[num].x2 := _x1;
  clipBoxs[num].y1 := _y2;
  clipBoxs[num].y2 := _y1;
  clipBoxs[num].z1 := _z2;
  clipBoxs[num].z2 := _z1;
  clipBoxs[num].id := id;
end;

Function GetId(a: TComboBox; b: TCheckBox): Integer;
begin
  If b.enabled Then
    GetId := a.ItemIndex + 1
  else
    GetId := 0;
end;

procedure TForm4.SaveClipboxes(sini: Boolean);
Var
  a: TIniFile;
begin
  a := nil;

  if sini = True then
    begin
      a := TIniFile.Create(programDir + '\clipboxes.ini');
      a.WriteString('enabled', 'box1', BoolToStr(CheckBox2.Checked));
      a.WriteString('enabled', 'box2', BoolToStr(CheckBox3.Checked));
      a.WriteString('enabled', 'box3', BoolToStr(CheckBox4.Checked));
      a.WriteString('enabled', 'box4', BoolToStr(CheckBox5.Checked));
      a.WriteString('enabled', 'box5', BoolToStr(CheckBox1.Checked));
    end;

  SetClipbox(a, 1, GetId(ComboBox1, CheckBox2), CheckBox2.Checked, Edit1.Text,
      Edit2.Text, Edit3.Text, Edit4.Text, Edit5.Text, Edit6.Text);
  SetClipbox(a, 2, GetId(ComboBox2, CheckBox3), CheckBox3.Checked, Edit7.Text,
      Edit8.Text, Edit9.Text, Edit10.Text, Edit11.Text, Edit12.Text);
  SetClipbox(a, 3, GetId(ComboBox3, CheckBox4), CheckBox4.Checked, Edit13.Text,
      Edit14.Text, Edit15.Text, Edit16.Text, Edit17.Text, Edit18.Text);
  SetClipbox(a, 4, GetId(ComboBox4, CheckBox5), CheckBox5.Checked, Edit19.Text,
      Edit20.Text, Edit21.Text, Edit22.Text, Edit23.Text, Edit24.Text);
  SetClipbox(a, 5, GetId(ComboBox5, CheckBox1), CheckBox1.Checked, Edit25.Text,
      Edit26.Text, Edit27.Text, Edit28.Text, Edit29.Text, Edit30.Text);

  if sini = True then
    begin
      a.free;
    end;
end;

procedure TForm4.Button1Click(Sender: TObject);
begin
  SaveClipboxes(true);
end;

Procedure GetClipbox(var a: TIniFile; num: Integer; var id: Integer;
  enabled: Boolean; var x1, x2, y1, y2, z1, z2: String);
Var
  n: String;
begin
  n := 'box' + IntToStr(num);
  id := StrToInt(a.ReadString(n, 'id', '1'));
  x1 := a.ReadString(n, 'x1', '');
  x2 := a.ReadString(n, 'x2', '');
  y1 := a.ReadString(n, 'y1', '');
  y2 := a.ReadString(n, 'y2', '');
  z1 := a.ReadString(n, 'z1', '');
  z2 := a.ReadString(n, 'z2', '');

  clipBoxs[num].enabled := enabled;
  if x1 = '' then
    clipBoxs[num].x2 := 10000
  else
    clipBoxs[num].x2 := StrToFloat(x1);
  if x2 = '' then
    clipBoxs[num].x1 := -10000
  else
    clipBoxs[num].x1 := StrToFloat(x2);
  if y1 = '' then
    clipBoxs[num].y2 := 10000
  else
    clipBoxs[num].y2 := StrToFloat(y1);
  if y2 = '' then
    clipBoxs[num].y1 := -10000
  else
    clipBoxs[num].y1 := StrToFloat(y2);
  if z1 = '' then
    clipBoxs[num].z2 := 10000
  else
    clipBoxs[num].z2 := StrToFloat(z1);
  if z2 = '' then
    clipBoxs[num].z1 := -10000
  else
    clipBoxs[num].z1 := StrToFloat(z2);
  clipBoxs[num].id := id;
end;

Procedure TForm4.UpdateClipboxs;
begin
  clipboxs[1].id := Form4.ComboBox1.ItemIndex + 1;
  clipboxs[2].id := Form4.ComboBox2.ItemIndex + 1;
  clipboxs[3].id := Form4.ComboBox3.ItemIndex + 1;
  clipboxs[4].id := Form4.ComboBox4.ItemIndex + 1;
  clipboxs[5].id := Form4.ComboBox5.ItemIndex + 1;
end;

Procedure LoadClipboxs;
Var
  a: TIniFile;
  ii: Integer;
  x1, x2, y1, y2, z1, z2: String;
begin
  a := TIniFile.Create(programDir + '\clipboxes.ini');

  if a.ReadString('enabled', 'box1', '') = '' then
    begin
      a.free;
      Exit;
    end;
  
  Form4.CheckBox2.Checked := a.ReadString('enabled', 'box1', '') = '-1';
  Form4.CheckBox3.Checked := a.ReadString('enabled', 'box2', '') = '-1';
  Form4.CheckBox4.Checked := a.ReadString('enabled', 'box3', '') = '-1';
  Form4.CheckBox5.Checked := a.ReadString('enabled', 'box4', '') = '-1';
  Form4.CheckBox1.Checked := a.ReadString('enabled', 'box5', '') = '-1';

  GetClipbox(a, 1, ii, Form4.CheckBox2.Checked, x1, x2, y1, y2, z1, z2);
  clipboxs[1].id := Form4.ComboBox1.ItemIndex + 1;
//  Form4.ComboBox1.ItemIndex := ii - 1;
  Form4.Edit1.Text := x1;
  Form4.Edit2.Text := x2;
  Form4.Edit3.Text := y1;
  Form4.Edit4.Text := y2;
  Form4.Edit5.Text := z1;
  Form4.Edit6.Text := z2;

  GetClipbox(a, 2, ii, Form4.CheckBox3.Checked, x1, x2, y1, y2, z1, z2);
  clipboxs[2].id := Form4.ComboBox2.ItemIndex + 1;
//  Form4.ComboBox2.ItemIndex := ii - 1;
  Form4.Edit7.Text := x1;
  Form4.Edit8.Text := x2;
  Form4.Edit9.Text := y1;
  Form4.Edit10.Text := y2;
  Form4.Edit11.Text := z1;
  Form4.Edit12.Text := z2;

  GetClipbox(a, 3, ii, Form4.CheckBox4.Checked, x1, x2, y1, y2, z1, z2);
  clipboxs[3].id := Form4.ComboBox3.ItemIndex + 1;
//  Form4.ComboBox3.ItemIndex := ii - 1;
  Form4.Edit13.Text := x1;
  Form4.Edit14.Text := x2;
  Form4.Edit15.Text := y1;
  Form4.Edit16.Text := y2;
  Form4.Edit17.Text := z1;
  Form4.Edit18.Text := z2;

  GetClipbox(a, 4, ii, Form4.CheckBox5.Checked, x1, x2, y1, y2, z1, z2);
  clipboxs[4].id := Form4.ComboBox4.ItemIndex + 1;
//  Form4.ComboBox4.ItemIndex := ii - 1;
  Form4.Edit19.Text := x1;
  Form4.Edit20.Text := x2;
  Form4.Edit21.Text := y1;
  Form4.Edit22.Text := y2;
  Form4.Edit23.Text := z1;
  Form4.Edit24.Text := z2;

  GetClipbox(a, 5, ii, Form4.CheckBox1.Checked, x1, x2, y1, y2, z1, z2);
//  Form4.ComboBox5.ItemIndex := ii - 1;
  clipboxs[5].id := Form4.ComboBox5.ItemIndex + 1;
  Form4.Edit25.Text := x1;
  Form4.Edit26.Text := x2;
  Form4.Edit27.Text := y1;
  Form4.Edit28.Text := y2;
  Form4.Edit29.Text := z1;
  Form4.Edit30.Text := z2;

  a.free;
end;

Function IsRowOk(b: Boolean;
  x1, x2, y1, y2, z1, z2: String; var err: Integer): Boolean;
Var
  cnt: Longint;
  t1, t2: Real;
begin
  err := 0;
  If b = false Then
    IsRowOk := true
  else
    begin
      cnt := 0;
      
      if (x1 <> '') And (x1 <> '-') And (IsNumeric(x1)) And
        (x2 <> '') And (x2 <> '-') And (IsNumeric(x2)) Then
        begin
          t1 := StrToFloat(x1);
          t2 := StrToFloat(x2);

          if t1 < t2 then
            begin
              err := 1;
              IsRowOk := false;
              Exit;
            end;

          Inc(cnt);
        end;

      if (y1 <> '') And (y1 <> '-') And (IsNumeric(y1)) And
        (y2 <> '') And (y2 <> '-') And (IsNumeric(y2)) Then
        begin
          t1 := StrToFloat(y1);
          t2 := StrToFloat(y2);

          if t1 < t2 then
            begin
              err := 2;
              IsRowOk := false;
              Exit;
            end;

          Inc(cnt);
        end;

      if (z1 <> '') And (z1 <> '-') And (IsNumeric(z1)) And
        (z2 <> '') And (z2 <> '-') And (IsNumeric(z2)) Then
        begin
          t1 := StrToFloat(z1);
          t2 := StrToFloat(z2);

          if t1 < t2 then
            begin
              err := 3;            
              IsRowOk := false;
              Exit;
            end;

          Inc(cnt);
        end;

      if (x1 <> '') And (x1 <> '-') And (IsNumeric(x1)) And ((x2 = '')
        Or (x2 = '-') Or (IsNumeric(x2) = false)) then
        begin
          IsRowOk := false;
          Exit;
        end;

      if (x2 <> '') And (x2 <> '-') And (IsNumeric(x2)) And ((x1 = '')
        Or (x1 = '-') Or (IsNumeric(x1) = false)) then
        begin
          IsRowOk := false;
          Exit;
        end;

      if (y1 <> '') And (y1 <> '-') And (IsNumeric(y1)) And ((y2 = '')
        Or (y2 = '-') Or (IsNumeric(y2) = false)) then
        begin
          IsRowOk := false;
          Exit;
        end;

      if (y2 <> '') And (y2 <> '-') And (IsNumeric(y2)) And ((y1 = '')
        Or (y1 = '-') Or (IsNumeric(y1) = false)) then
        begin
          IsRowOk := false;
          Exit;
        end;

      if (z1 <> '') And (z1 <> '-') And (IsNumeric(z1)) And ((z2 = '')
        Or (z2 = '-') Or (IsNumeric(z2) = false)) then
        begin
          IsRowOk := false;
          Exit;
        end;

      if (z2 <> '') And (z2 <> '-') And (IsNumeric(z2)) And ((z1 = '')
        Or (z1 = '-') Or (IsNumeric(z1) = false)) then
        begin
          IsRowOk := false;
          Exit;
        end;

      IsRowOk := (cnt >= 1);
    end;
end;

Procedure MarkColor(var e1, e2: TEdit; clr: Longint);
begin
  e1.Font.Color := clr;
  e2.Font.Color := clr;  
end;

Procedure CheckClipboxs;
Var
  err: Integer;
begin
  With Form4 Do
    begin
      If IsRowOk(CheckBox2.Checked, Edit1.Text, Edit2.Text, Edit3.Text,
        Edit4.Text, Edit5.Text, Edit6.Text, err) Then
        begin
          Image2.Visible := true;
          Image3.Visible := false;
          MarkColor(Edit1, Edit2, clBlack);
          MarkColor(Edit3, Edit4, clBlack);
          MarkColor(Edit5, Edit6, clBlack);
        end
      else
        begin
          Image3.Visible := true;
          Image2.Visible := false;

          If err = 1 Then
            MarkColor(Edit1, Edit2, clRed);
          If err = 2 Then
            MarkColor(Edit3, Edit4, clRed);
          If err = 3 Then
            MarkColor(Edit6, Edit5, clRed);
        end;

      If IsRowOk(CheckBox3.Checked, Edit7.Text, Edit8.Text, Edit9.Text,
        Edit10.Text, Edit11.Text, Edit12.Text, err) Then
        begin
          Image4.Visible := true;
          Image5.Visible := false;
          MarkColor(Edit7, Edit8, clBlack);
          MarkColor(Edit9, Edit10, clBlack);
          MarkColor(Edit11, Edit12, clBlack);
        end
      else
        begin
          Image5.Visible := true;
          Image4.Visible := false;
          If err = 1 Then
            MarkColor(Edit7, Edit8, clRed);
          If err = 2 Then
            MarkColor(Edit9, Edit10, clRed);
          If err = 3 Then
            MarkColor(Edit11, Edit12, clRed);
        end;

      If IsRowOk(CheckBox4.Checked, Edit13.Text, Edit14.Text, Edit15.Text,
        Edit16.Text, Edit17.Text, Edit18.Text, err) Then
        begin
          Image6.Visible := true;
          Image7.Visible := false;
          MarkColor(Edit13, Edit14, clBlack);
          MarkColor(Edit15, Edit16, clBlack);
          MarkColor(Edit17, Edit18, clBlack);
        end
      else
        begin
          Image7.Visible := true;
          Image6.Visible := false;
          If err = 1 Then
            MarkColor(Edit13, Edit14, clRed);
          If err = 2 Then
            MarkColor(Edit15, Edit16, clRed);
          If err = 3 Then
            MarkColor(Edit17, Edit18, clRed);
        end;

      If IsRowOk(CheckBox5.Checked, Edit19.Text, Edit20.Text, Edit21.Text,
        Edit22.Text, Edit23.Text, Edit24.Text, err) Then
        begin
          Image8.Visible := true;
          Image9.Visible := false;
          MarkColor(Edit19, Edit20, clBlack);
          MarkColor(Edit21, Edit22, clBlack);
          MarkColor(Edit23, Edit24, clBlack);
        end
      else
        begin
          Image9.Visible := true;
          Image8.Visible := false;
          If err = 1 Then
            MarkColor(Edit19, Edit20, clRed);
          If err = 2 Then
            MarkColor(Edit21, Edit22, clRed);
          If err = 3 Then
            MarkColor(Edit23, Edit24, clRed);
        end;

      If IsRowOk(CheckBox1.Checked, Edit25.Text, Edit26.Text, Edit27.Text,
        Edit28.Text, Edit29.Text, Edit30.Text, err) Then
        begin
          Image10.Visible := true;
          Image11.Visible := false;
          MarkColor(Edit25, Edit26, clBlack);
          MarkColor(Edit27, Edit28, clBlack);
          MarkColor(Edit29, Edit30, clBlack);
        end
      else
        begin
          Image11.Visible := true;
          Image10.Visible := false;
          If err = 1 Then
            MarkColor(Edit25, Edit26, clRed);
          If err = 2 Then
            MarkColor(Edit27, Edit28, clRed);
          If err = 3 Then
            MarkColor(Edit29, Edit30, clRed);
        end;
    end;
end;

Procedure ReloadBoxesInClient;
Var
  h: HWND;
  k, id, x1, x2, y1, y2, z1, z2, cnt: Integer;
  fn: String;
  f: TextFile;
begin
  CheckClipboxs;

  Form4.SaveClipboxes(false);

  fn := Form1.ClientPathEdit.Text;
  If fn[Length(fn)] <> '\' Then
    fn := fn + '\';
  fn := fn + 'clipboxs.txt';

  AssignFile(f, fn);
  {$I-}
  Rewrite(f);
  {$I+}
  If IOResult <> 0 Then Exit;

  for k := 1 To 5 do
    begin
      id := clipboxs[k].id;
      x1 := Round(clipboxs[k].x1);
      x2 := Round(clipboxs[k].x2);
      y1 := Round(clipboxs[k].y1);
      y2 := Round(clipboxs[k].y2);
      z1 := Round(clipboxs[k].z1);
      z2 := Round(clipboxs[k].z2);

      cnt := 0;
      if (x1 <> -10000) And (x2 <> 10000) then
        Inc(cnt);
      if (y1 <> -10000) And (y2 <> 10000) then
        Inc(cnt);
      if (z1 <> -10000) And (z2 <> 10000) then
        Inc(cnt);

      if (not clipboxs[k].enabled) Or (cnt < 1) Then
        begin
          x1 := 0;
          x2 := 0;
          y1 := 0;
          y2 := 0;
          z1 := 0;
          z2 := 0;
        end;
      WriteLn(f, id, ' ', x1, ' ', x2, ' ', y1, ' ', y2, ' ', z1, ' ', z2);
    end;

  CloseFile(f);

  h := FindWindow('VSpaces virtual worlds player', nil);
  if h <> 0 then
    begin
      SendMessage(h, WM_KEYDOWN, Ord('B'), Ord('L'));
    end;
end;

procedure TForm4.CheckBox2Click(Sender: TObject);
begin
  ReloadBoxesInClient;
end;

procedure TForm4.CheckBox3Click(Sender: TObject);
begin
  ReloadBoxesInClient;
end;

procedure TForm4.CheckBox4Click(Sender: TObject);
begin
  ReloadBoxesInClient;
end;

procedure TForm4.CheckBox5Click(Sender: TObject);
begin
  ReloadBoxesInClient;
end;

procedure TForm4.CheckBox6Click(Sender: TObject);
Var
  h: HWND;
begin
  h := FindWindow('VSpaces virtual worlds player', nil);
  if h <> 0 then
    begin
      if CheckBox6.Checked then
        SendMessage(h, WM_KEYDOWN, Ord('B'), Ord('L') + 2)
      else
        SendMessage(h, WM_KEYDOWN, Ord('B'), Ord('L') + 3);
    end;
end;

procedure TForm4.ComboBox1Change(Sender: TObject);
begin
  ReloadBoxesInClient;
end;

procedure TForm4.Edit1Change(Sender: TObject);
begin
  ReloadBoxesInClient;
end;

procedure TForm4.Edit2Change(Sender: TObject);
begin
  ReloadBoxesInClient;
end;

Function GetPartName: String;
begin
  If Form1.HairRadioButton.Checked Then
    GetPartName := 'hair'
  else
    begin
      if Form1.ClothesType.ItemIndex <> -1 then
        GetPartName := parts[Form1.ClothesType.ItemIndex]
      else
        GetPartName := '';
    end;
end;

Procedure EnableClipbox(i: Integer; b: Boolean);
begin
  if i = 1 then
    begin
      Form4.CheckBox2.Enabled := b;
      Form4.Edit1.Enabled := b;
      Form4.Edit2.Enabled := b;
      Form4.Edit3.Enabled := b;
      Form4.Edit4.Enabled := b;
      Form4.Edit5.Enabled := b;
      Form4.Edit6.Enabled := b;
      Form4.Label1.Enabled := b;
      Form4.Label2.Enabled := b;
      Form4.Label3.Enabled := b;
      Form4.Label4.Enabled := b;
      if b = false then clipBoxs[1].id := 0;
    end
  else if i = 2 then
    begin
      Form4.CheckBox3.Enabled := b;
      Form4.Edit7.Enabled := b;
      Form4.Edit8.Enabled := b;
      Form4.Edit9.Enabled := b;
      Form4.Edit10.Enabled := b;
      Form4.Edit11.Enabled := b;
      Form4.Edit12.Enabled := b;
      Form4.Label5.Enabled := b;
      Form4.Label6.Enabled := b;
      Form4.Label7.Enabled := b;
      Form4.Label8.Enabled := b;
      if b = false then clipBoxs[2].id := 0;
    end
  else if i = 3 then
    begin
      Form4.CheckBox4.Enabled := b;
      Form4.Edit13.Enabled := b;
      Form4.Edit14.Enabled := b;
      Form4.Edit15.Enabled := b;
      Form4.Edit16.Enabled := b;
      Form4.Edit17.Enabled := b;
      Form4.Edit18.Enabled := b;
      Form4.Label9.Enabled := b;
      Form4.Label10.Enabled := b;
      Form4.Label11.Enabled := b;
      Form4.Label12.Enabled := b;
      if b = false then clipBoxs[3].id := 0;
    end
  else if i = 4 then
    begin
      Form4.CheckBox5.Enabled := b;
      Form4.Edit19.Enabled := b;
      Form4.Edit20.Enabled := b;
      Form4.Edit21.Enabled := b;
      Form4.Edit22.Enabled := b;
      Form4.Edit23.Enabled := b;
      Form4.Edit24.Enabled := b;
      Form4.Label13.Enabled := b;
      Form4.Label14.Enabled := b;
      Form4.Label15.Enabled := b;
      Form4.Label16.Enabled := b;
      if b = false then clipBoxs[4].id := 0;
    end
  else if i = 5 then
    begin
      Form4.CheckBox1.Enabled := b;
      Form4.Edit25.Enabled := b;
      Form4.Edit26.Enabled := b;
      Form4.Edit27.Enabled := b;
      Form4.Edit28.Enabled := b;
      Form4.Edit29.Enabled := b;
      Form4.Edit30.Enabled := b;
      Form4.Label21.Enabled := b;
      Form4.Label22.Enabled := b;
      Form4.Label23.Enabled := b;
      Form4.Label24.Enabled := b;
      if b = false then clipBoxs[5].id := 0;
    end;
end;

procedure TForm4.UpdatePermittedClipboxs;
Var
  a: TClipboxIDsArray;
  i, cnt: Integer;
begin
  a := GetPermittedClipboxs(GetPartName);

  cnt := 1;
  While a[cnt] <> 0 Do
    Inc(cnt);
  Dec(cnt);

  For i := 1 To cnt Do
    begin
      if i = 1 then
        ComboBox1.ItemIndex := a[i] - 1;

      if i = 2 then
        ComboBox2.ItemIndex := a[i] - 1;

      if i = 3 then
        ComboBox3.ItemIndex := a[i] - 1;

      if i = 4 then
        ComboBox4.ItemIndex := a[i] - 1;

      if i = 5 then
        ComboBox5.ItemIndex := a[i] - 1;

      EnableClipbox(i, true);
    end;

  if cnt + 1 <= 5 then
  For i := cnt + 1 To 5 Do
    begin
      EnableClipbox(i, false);
    end;
end;

procedure TForm4.FormCanResize(Sender: TObject; var NewWidth,
  NewHeight: Integer; var Resize: Boolean);
begin
  resize := false;
end;

procedure TForm4.FormShow(Sender: TObject);
begin
  ComboBox1.Enabled := false;
  ComboBox2.Enabled := false;
  ComboBox3.Enabled := false;
  ComboBox4.Enabled := false;
  ComboBox5.Enabled := false;

  UpdatePermittedClipboxs;

  LoadClipboxs;

  Edit2.TabOrder := 1;
  Edit1.TabOrder := 2;
  Edit4.TabOrder := 3;
  Edit3.TabOrder := 4;
  Edit6.TabOrder := 5;
  Edit5.TabOrder := 6;
  
  Edit8.TabOrder := 7;
  Edit7.TabOrder := 8;
  Edit10.TabOrder := 9;
  Edit9.TabOrder := 10;
  Edit12.TabOrder := 11;
  Edit11.TabOrder := 12;

  Edit14.TabOrder := 13;
  Edit13.TabOrder := 14;
  Edit16.TabOrder := 15;
  Edit15.TabOrder := 16;
  Edit18.TabOrder := 17;
  Edit17.TabOrder := 18;

  Edit20.TabOrder := 19;
  Edit19.TabOrder := 20;
  Edit22.TabOrder := 21;
  Edit21.TabOrder := 22;
  Edit24.TabOrder := 23;
  Edit23.TabOrder := 24;

  Edit26.TabOrder := 25;
  Edit25.TabOrder := 26;
  Edit28.TabOrder := 27;
  Edit27.TabOrder := 28;
  Edit30.TabOrder := 29;
  Edit29.TabOrder := 30;
end;

end.
