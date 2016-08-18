unit mainform;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, xmldom, XMLIntf, msxmldom, XMLDoc, StdCtrls, ShlObj, ShellApi, IniFiles;

   Function IsFileExists(fn: String): Boolean;

type
  _SBlends = record
      name: String;
      param: String;
      part: String;
      min: String;
      max: String;
      mintext: String;
      maxtext: String;
    end;
  TForm1 = class(TForm)
    OptionsGroupBox: TGroupBox;
    ClientPathEdit: TEdit;
    PathToClientLabel: TLabel;
    ChooseClientPath: TButton;
    ThisIsLabel: TLabel;
    ClothesRadioButton: TRadioButton;
    HairRadioButton: TRadioButton;
    ModelNameLabel: TLabel;
    ModelName: TEdit;
    ActionsGroupBox: TGroupBox;
    CopyMeshButton: TButton;
    ConfigureMaterialsButton: TButton;
    OpenDialog1: TOpenDialog;
    XMLDocument1: TXMLDocument;
    AvatarComboBox: TComboBox;
    ForLabel: TLabel;
    ConfigureClipboxesButton: TButton;
    RunClientButton: TButton;
    BlendRadioButton: TRadioButton;
    BlendTypeComboBox: TComboBox;
    BlendForLabel: TLabel;
    HeadIDComboBox: TComboBox;
    HeadLabel: TLabel;
    ClothesType: TComboBox;
    UseTexturesFromCheckBox: TCheckBox;
    AvatarForTexturesComboBox: TComboBox;
    FromLabel: TLabel;
    ModelForTexturesNameComboBox: TComboBox;
    UseServerTexturesCheckBox: TCheckBox;
    BlendForClothesOrHairdressRadioButton: TRadioButton;
    ClothesAndHairdressesComboBox: TComboBox;
    Label1: TLabel;
    BlendParamNameEdit: TEdit;
    Label2: TLabel;
    BlendParamMinNameEdit: TEdit;
    Label3: TLabel;
    BlendParamMaxNameEdit: TEdit;
    DeleteBlendsCheckBox: TCheckBox;
    AvatarRadioButton: TRadioButton;
    AvatarNumEdit: TEdit;
    procedure ChooseClientPathClick(Sender: TObject);
    procedure CopyMeshButtonClick(Sender: TObject);
    procedure ConfigureMaterialsButtonClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure RunClientButtonClick(Sender: TObject);
    procedure BlendRadioButtonClick(Sender: TObject);
    procedure AvatarComboBoxChange(Sender: TObject);
    procedure ClientPathEditChange(Sender: TObject);
    procedure ConfigureClipboxesButtonClick(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure ClothesTypeChange(Sender: TObject);
    procedure AvatarForTexturesComboBoxChange(Sender: TObject);
    procedure UseTexturesFromCheckBoxClick(Sender: TObject);
    procedure FormCanResize(Sender: TObject; var NewWidth, NewHeight: Integer;
      var Resize: Boolean);
    procedure UseServerTexturesCheckBoxClick(Sender: TObject);
    Function GetModelPath: String;
    Procedure CopyModelTextures(s: String);
    procedure BlendForClothesOrHairdressRadioButtonClick(Sender: TObject);
    procedure AvatarRadioButtonClick(Sender: TObject);
    Procedure ReloadAvatarsList(path: String);
  private
    { Private declarations }
  public
    { Public declarations }
  end;
  TAvatar = Record
             id: Integer;
             folder: String;
             compatID: Integer;
             isWoman: Boolean;
           end;  

var
  Form1: TForm1;
  matCount: Longint;
  materialsNode, materialNode: IXMLNode;
  materials: Array [1..100] Of String;
  textures: Array [1..100] Of String;
  programDir: String;
  parts: TStringList;
  hairOrClothesCorrespondenceIndex: Array [0..100] Of String;
  avatars: Array [1..1000] of TAvatar;
  avatarsCount: Integer;

const
  SERVER1_PATH = '\\FSERVER\WebServersCD\home\virtuniver.loc\www\vu\root\files\vu2008-02-20\geometry\avatars\editor\';
  SERVER2_PATH = '\\TERA\WebServersCD\home\virtuniver.loc\www\vu\root\files\vu2008-02-20\geometry\avatars\editor\';

implementation

uses materialform, clipboxform, editmaterialform;

{$R *.dfm}

Function GetAvatarIDByString(s: String): Integer;
Var
  i: Integer;
begin
  result := 0;

  For i := 1 To avatarsCount Do
  If (avatars[i].folder = s) Then
    result := avatars[i].id;
end;

// На вход данные типа women8, suit1
// На выходе suit01 - папка
Function GetThingFolder(avatarStringID: String;
    thingStringID: String): String;
Var
  avatarID: Longint;
  av, editparam, templates, template, parts, part: IXMLNode;
  editparamID, templateID: Longint;
  src, s: String;
begin
  avatarID := GetAvatarIDByString(avatarStringID);

  s := Form1.ClientPathEdit.Text;
  If s[Length(s)] <> '\' Then
    s := s + '\';

  Form1.XMLDocument1.LoadFromFile(s + 'ui\avatareditorpack' + IntToStr(avatarID) +
        '.xml');
  try
    Form1.XMLDocument1.Active := true;
  except
    Exit;
  end;
  If Form1.XMLDocument1.IsEmptyDoc Then
    begin
      MessageDlg('В папке тела нет описания меша', mtError, [mbOk], 0);
      Exit;
    end;
  av := Form1.XMLDocument1.DocumentElement;
  if av = nil then
    begin
      MessageDlg('Описание аватара пустое', mtError, [mbOk], 0);
      Exit;
    end;

  For editparamID := 0 To av.ChildNodes.Count - 1 Do
    begin
      editparam := av.ChildNodes[editparamID];
      templates := editparam.ChildNodes.FindNode('templates');
      if (templates <> nil) Then
        begin
          For templateID := 0 To templates.ChildNodes.Count - 1 Do
            begin
              template := templates.ChildNodes[templateID];
              If (not VarIsNull(templates.ChildNodes[templateID].Attributes['id'])) And
                (templates.ChildNodes[templateID].Attributes['id'] = thingStringID) Then
                begin
                  parts := template.ChildNodes.FindNode('parts');
                  if (parts <> nil) Then
                    begin
                      part := parts.ChildNodes.FindNode('part');
                      if part <> nil Then
                        begin
                          src := part.Attributes['src'];
                          While src[1] <> '\' Do
                            Delete(src, 1, 1);
                          Delete(src, 1, 1);
                          While src[Length(src)] <> '\' Do
                            Delete(src, Length(src), 1);
                          Delete(src, Length(src), 1);
                          GetThingFolder := src;
                          Exit;
                        end;
                    end;
                end;
            end;
        end;
    end;

  GetThingFolder := '';  
end;

procedure TForm1.ChooseClientPathClick(Sender: TObject);
var
  TitleName : string;
  lpItemID : PItemIDList;
  BrowseInfo : TBrowseInfo;
  DisplayName : array[0..MAX_PATH] of char;
  TempPath : array[0..MAX_PATH] of char;
begin
  FillChar(BrowseInfo, sizeof(TBrowseInfo), #0);
  BrowseInfo.hwndOwner := Form1.Handle;
  BrowseInfo.pszDisplayName := @DisplayName;
  TitleName := 'Укажите папку';
  BrowseInfo.lpszTitle := PChar(TitleName);
  BrowseInfo.ulFlags := BIF_RETURNONLYFSDIRS;
  lpItemID := SHBrowseForFolder(BrowseInfo);
  if lpItemId <> nil then begin
    SHGetPathFromIDList(lpItemID, TempPath);
  ClientPathEdit.Text := TempPath;
  GlobalFreePtr(lpItemID);
  end;
end;

Procedure ParseXML;
Var
  i: Longint;
begin
  if (FileExists(programDir + '\mesh.xml') = false) Then
    begin
      MessageDlg('В папке нет меша', mtError, [mbOk], 0);
      Exit;
    end;

  Form1.XMLDocument1.LoadFromFile(programDir + '\mesh.xml');
  Form1.XMLDocument1.Active := true;
  If Form1.XMLDocument1.IsEmptyDoc Then
    begin
      MessageDlg('В папке нет меша', mtError, [mbOk], 0);
      Exit;
    end;
  materialsNode := Form1.XMLDocument1.DocumentElement.ChildNodes.FindNode(WideString('materials'));
  if materialsNode = nil then
    begin
      MessageDlg('В меше нет материалов', mtError, [mbOk], 0);
      Exit;
    end;
  matCount := materialsNode.ChildNodes.Count;
  For i := 0 To matCount - 1 Do
    begin
      materialNode := materialsNode.ChildNodes[i];
      If (not VarIsNull(materialsNode.ChildNodes[i].Attributes['texture'])) Then
        textures[i + 1] := materialsNode.ChildNodes[i].Attributes['texture'];
    end;
end;

Function GetAvatarID: Integer;
begin
  result := GetAvatarIDByString(Form1.AvatarComboBox.Text);
end;

Function GetAvatarIDCopy: Integer;
Var
  i: Integer;
begin
  For i := 1 To avatarsCount Do
  If (avatars[i].folder = Form1.AvatarForTexturesComboBox.Text) Then
    result := avatars[i].id;
end;

Function GetAvatarCompatabilityID: Integer;
Var
  i: Integer;
begin
  For i := 1 To avatarsCount Do
  If (avatars[i].folder = Form1.AvatarComboBox.Text) Then
    result := avatars[i].compatID;
end;

function WindowsCopyFile(FromFile, ToDir : string) : boolean;
var
  F: TShFileOpStruct;
begin
  F.Wnd := 0; F.wFunc := FO_COPY;
  FromFile:=FromFile+#0; F.pFrom:=pchar(FromFile);
  ToDir:=ToDir+#0; F.pTo:=pchar(ToDir);
  F.fFlags := FOF_ALLOWUNDO or FOF_NOCONFIRMATION or FOF_NOERRORUI;
  result:=ShFileOperation(F) = 0;
end;

Procedure ReloadResServerCache;
Var
  s, tmpPath, exePath: String;
  err: Longint;

begin
  SetLength(tmpPath, MAX_PATH);
	SHGetSpecialFolderPath(Form1.Handle, PChar(tmpPath), CSIDL_APPDATA, true);

  WindowsCopyFile('R:\VU2008\cloth_editor\77.bat', tmpPath);
  WindowsCopyFile('R:\VU2008\cloth_editor\ServerCommon.dll', tmpPath);
  WindowsCopyFile('R:\VU2008\cloth_editor\ControlServers_work.exe', tmpPath);

  GetDir(0, s);
  ChDir(tmpPath);
  exePath := '77.bat';
  err := WinExec(PChar(exePath), SW_SHOW);
  Sleep(1000);
  ChDir(s);
end;

Procedure GetTextures(i: Integer; var s1: String; var s2: String);
Var
  s: String;
  f: TextFile;
begin
  If Form3.Visible = true Then
    begin
      s1 := '';
      s2 := '';
      If Form3.Edit1.Enabled Then
        s1 := Form3.Edit1.Text;
      If Form3.Edit2.Enabled Then
        s2 := Form3.Edit2.Text;
      Exit;
    end;
  AssignFile(f, programDir + '\mat' + IntToStr(i) + '.cfg');
  {$I-}
  Reset(f);
  {$I+}
  If IOResult <> 0 Then
    begin
      s1 := '';
      s2 := '';
      Exit;
    end;
  ReadLn(f, s);
  ReadLn(f, s);
  ReadLn(f, s);
  ReadLn(f, s);
  ReadLn(f, s);
  ReadLn(f, s);
  s1 := s;
  ReadLn(f, s);
  s2 := s;
  CloseFile(f);
end;

Function IsMaterialExists(i: Integer): Boolean;
Var
  f: textFile;
begin
  AssignFile(f, programDir + '\mat' + IntToStr(i) + '.cfg');
  {$I-}
  Reset(f);
  {$I+}
  If IOResult <> 0 Then
    begin
      IsMaterialExists := false;
      Exit;
    end;

  CloseFile(f);

  IsMaterialExists := true;
end;

Function GetBlendSrc(blendID: Integer): String;
begin
  Case blendID Of
        0: GetBlendSrc := 'eye\left\';
        1: GetBlendSrc := 'eye\right\';
        2: GetBlendSrc := 'eye\close\';
        3: GetBlendSrc := 'smile\';
        4: GetBlendSrc := 'smile\start\';
        5: GetBlendSrc := 'lips\1\';
        6: GetBlendSrc := 'lips\2\';
        7: GetBlendSrc := 'lips\3\';
        8: GetBlendSrc := 'lips\4\';
        9: GetBlendSrc := 'lips\5\';
        10: GetBlendSrc := 'nose_form\';
        11: GetBlendSrc := 'nose_length\';
        12: GetBlendSrc := 'ears\';
        13: GetBlendSrc := 'forehead\';
        14: GetBlendSrc := 'cheeks\';
        15: GetBlendSrc := 'au0\';
        16: GetBlendSrc := 'au1\';
        17: GetBlendSrc := 'au2\';
        18: GetBlendSrc := 'au3\';
        19: GetBlendSrc := 'au4\';
        20: GetBlendSrc := 'au5\';
      End;
end;

Function GetMinTextForBlend(blendID: Integer): String;
begin
  Case blendID Of
        10: GetMinTextForBlend := 'Уже';
        11: GetMinTextForBlend := 'Короче';
        12: GetMinTextForBlend := 'Меньше';
        13: GetMinTextForBlend := 'Уже';
        14: GetMinTextForBlend := 'Впалые';
      End;
end;

Function GetMaxTextForBlend(blendID: Integer): String;
begin
  Case blendID Of
        10: GetMaxTextForBlend := 'Шире';
        11: GetMaxTextForBlend := 'Длиннее';
        12: GetMaxTextForBlend := 'Больше';
        13: GetMaxTextForBlend := 'Шире';
        14: GetMaxTextForBlend := 'Пухлые';
      End;
end;

Function GetBlendTarget(blendID: Integer): String;
begin
  Case blendID Of
        0: GetBlendTarget := 'eye_left';
        1: GetBlendTarget := 'eye_right';
        2: GetBlendTarget := 'eye_close';
        3: GetBlendTarget := 'smile';
        4: GetBlendTarget := 'smile_start';
        5: GetBlendTarget := 'lip_sync1';
        6: GetBlendTarget := 'lip_sync2';
        7: GetBlendTarget := 'lip_sync3';
        8: GetBlendTarget := 'lip_sync4';
        9: GetBlendTarget := 'lip_sync5';
        10: GetBlendTarget := 'nose_form';
        11: GetBlendTarget := 'nose_length';
        12: GetBlendTarget := 'ears';
        13: GetBlendTarget := 'forehead';
        14: GetBlendTarget := 'cheeks';
        15: GetBlendTarget := 'au0';
        16: GetBlendTarget := 'au1';
        17: GetBlendTarget := 'au2';
        18: GetBlendTarget := 'au3';
        19: GetBlendTarget := 'au4';
        20: GetBlendTarget := 'au5';
      End;
end;

Function GetPackFile: String;
begin
  GetPackFile := 'avatareditorpack' + IntToStr(GetAvatarID()) + '.xml';
end;

Function GetPackFileCopy: String;
begin
  GetPackFileCopy := 'avatareditorpack' + IntToStr(GetAvatarIDCopy()) + '.xml';
end;

Function IsClothesOrHairID(s: string): Boolean;
begin
 IsClothesOrHairID := (s = 'tors') OR (s = 'super_dress') OR (s = 'dress') OR (s = 'hair')
    OR (s = 'legs') OR (s = 'boots') OR (s = 'glasses') OR (s = 'socks') OR (s = 'hands');
end;

Function IsClothesID(s: string): Boolean;
begin
 IsClothesID := (s = 'tors') OR (s = 'super_dress') OR (s = 'dress')
    OR (s = 'legs') OR (s = 'boots') OR (s = 'glasses') OR (s = 'socks') OR (s = 'hands');
end;

Procedure InsertMeshIntoPack(name: String; isClothes: Boolean);
Var
  av, ts, t, tn, parts, part, params, param, clips, clip: IXMLNode;
  i, num, k, ii, j: Longint;
  found: Boolean;
  s: String;
  blends: array [0..100] of _SBlends;

begin
  s := Form1.ClientPathEdit.Text;
  If s[Length(s)] <> '\' Then
    s := s + '\';

  Form1.XMLDocument1.LoadFromFile(s + 'ui\' + GetPackFile);
  try
    Form1.XMLDocument1.Active := true;
  except
    Exit;
  end;
  If Form1.XMLDocument1.IsEmptyDoc Then
    begin
      MessageDlg('В папке тела нет описания меша', mtError, [mbOk], 0);
      Exit;
    end;
  av := Form1.XMLDocument1.DocumentElement;
  if av = nil then
    begin
      MessageDlg('Описание аватара пустое', mtError, [mbOk], 0);
      Exit;
    end;

  num := 0;

  if (not Form1.BlendForClothesOrHairdressRadioButton.Checked) then
  begin
  if isClothes then
    begin
      for i := 0 To av.ChildNodes.Count - 1 Do
       if (not VarIsNull(av.ChildNodes[i].Attributes['name'])) And (IsClothesID(av.ChildNodes[i].Attributes['id'])) then
        begin
          If num = Form1.ClothesType.ItemIndex Then
            begin
              ts := av.ChildNodes[i].ChildNodes.FindNode('templates');
            end;
          Inc(num);
        end
    end
  else
    for i := 0 To av.ChildNodes.Count - 1 Do
    if (not VarIsNull(av.ChildNodes[i].Attributes['name'])) then
      if (av.ChildNodes[i].Attributes['id'] = 'hair') AND (not Form1.BlendRadioButton.Checked) then
        begin
          ts := av.ChildNodes[i].ChildNodes.FindNode('templates');
       end
      else if (av.ChildNodes[i].Attributes['id'] = 'heads') AND (Form1.BlendRadioButton.Checked)  then
        begin
           ts := av.ChildNodes[i].ChildNodes.FindNode('templates');
        end
   end
  else
  begin
       for i := 0 To av.ChildNodes.Count - 1 Do
        if (not VarIsNull(av.ChildNodes[i].Attributes['name'])) And (IsClothesOrHairID(av.ChildNodes[i].Attributes['id'])) then
         begin
          for j := 0 To av.ChildNodes[i].ChildNodes.Count - 1 Do
           for k := 0 To av.ChildNodes[i].ChildNodes[j].ChildNodes.Count - 1 Do
            if (not VarIsNull(av.ChildNodes[i].ChildNodes[j].ChildNodes[k].Attributes['id'])) And (av.ChildNodes[i].ChildNodes[j].ChildNodes[k].Attributes['id'] = name) then
             begin
                ts := av.ChildNodes[i].ChildNodes.FindNode('templates');
              end
          end
  end;

  if ts <> nil then
    begin
      found := false;
        tn := nil;

        for k := 0 To ts.ChildNodes.Count - 1 Do
        if (not VarIsNull(ts.ChildNodes[k].Attributes['id'])) Then
        if ts.ChildNodes[k].Attributes['id'] = name then
          begin
            found := true;
            tn := ts.ChildNodes[k];
            break;
          end;

        if (not Form1.BlendRadioButton.Checked) then
        begin
          if found = false then
           tn := ts.AddChild('template');

          if isClothes then
            tn.Attributes['name'] := ts.ParentNode.Attributes['name']
          else
           tn.Attributes['name'] := 'Прическа';
          tn.Attributes['id'] := name;
          if isClothes then
            tn.Attributes['imgsrc'] := 'clothes\' + name + '\prev.png'
          else
           begin
             tn.Attributes['imgsrc'] := 'hair\' + name + '\prev.png';
              tn.Attributes['colorID'] := 'hair';
            end;

        i:= 0;
        for j:=0 To tn.ChildNodes.Count - 1 Do
        for k := 0 To tn.ChildNodes[j].ChildNodes.Count - 1 Do
          if (not VarIsNull(tn.ChildNodes[j].ChildNodes[k].Attributes['command'])) Then
            if tn.ChildNodes[j].ChildNodes[k].Attributes['command'] = 'set_blend_value' then
             begin
                 blends[i].name := tn.ChildNodes[j].ChildNodes[k].Attributes['name'];
                 blends[i].part := tn.ChildNodes[j].ChildNodes[k].Attributes['part'];
                 blends[i].param := tn.ChildNodes[j].ChildNodes[k].Attributes['param'];
                 blends[i].min := tn.ChildNodes[j].ChildNodes[k].Attributes['min'];
                 blends[i].max := tn.ChildNodes[j].ChildNodes[k].Attributes['max'];
                 blends[i].mintext := tn.ChildNodes[j].ChildNodes[k].Attributes['mintext'];
                 blends[i].maxtext := tn.ChildNodes[j].ChildNodes[k].Attributes['maxtext'];
                 i := i+1;
            end;

          tn.ChildNodes.Clear;

          parts := tn.AddChild('parts');
          part := parts.AddChild('part');
          part.Attributes['id'] := 'part01';
          if isClothes then
             part.Attributes['src'] := 'clothes\' + name + '\mesh.xml'
          else
            part.Attributes['src'] := 'hair\' + name + '\mesh.xml';

          params := tn.AddChild('params');
          param := params.AddChild('param');
          param.Attributes['name'] := 'Цвет';
          param.Attributes['command'] := 'set_rgbv_color';
          param.Attributes['part'] := 'part01';
          param.Attributes['param'] := 'RGBV';
          param.Attributes['min'] := 10;
          param.Attributes['max'] := 90;
          param.Attributes['mintext'] := 'Тусклый оттенок';
          param.Attributes['maxtext'] := 'Яркий оттенок';

          for k:=0 To i-1 Do
          begin
          if ((Form1.BlendForClothesOrHairdressRadioButton.Checked)  AND (blends[k].name <> Form1.BlendParamNameEdit.Text)) then
             begin
              param := params.AddChild('param');
              param.Attributes['name'] := blends[k].name;
              param.Attributes['command'] := 'set_blend_value';
              param.Attributes['part'] := blends[k].part;
              param.Attributes['param'] := blends[k].param;
              param.Attributes['min'] := blends[k].min;
              param.Attributes['max'] := blends[k].max;
              param.Attributes['mintext'] := blends[k].mintext;
              param.Attributes['maxtext'] := blends[k].maxtext;
             end; 
          end;

          if (Form1.BlendForClothesOrHairdressRadioButton.Checked) then
            begin
             param := params.AddChild('param');
             param.Attributes['name'] := Form1.BlendParamNameEdit.Text;
             param.Attributes['command'] := 'set_blend_value';
             param.Attributes['part'] := 'part01';
             param.Attributes['param'] := '\'+ Form1.ModelName.Text+ '/mesh.xml';
             param.Attributes['min'] := 0;
             param.Attributes['max'] := 100;
             param.Attributes['mintext'] := Form1.BlendParamMinNameEdit.Text;
             param.Attributes['maxtext'] := Form1.BlendParamMaxNameEdit.Text;
            end
        end
      else if (Form1.BlendRadioButton.Checked) AND (found = true) then
        begin
          found := false;
          params := tn.ChildNodes.FindNode('params');

          if (params = nil) then
            params := tn.AddChild('params')
          else
           begin
              for i:=0 To params.ChildNodes.Count-1 Do
                if (params.ChildNodes[i].Attributes['name'] = Form1.BlendTypeComboBox.Text) then
                    found := true;
           end;

           if (found = false) then
           begin
             param := params.AddChild('param');
             param.Attributes['name'] := Form1.BlendTypeComboBox.Text;
             param.Attributes['command'] := 'set_blend_value';
             param.Attributes['part'] := 'body';
             param.Attributes['param'] := '\'+ GetBlendTarget(Form1.BlendTypeComboBox.ItemIndex)+ '/mesh.xml';
             param.Attributes['min'] := 0;
             param.Attributes['max'] := 100;
             param.Attributes['mintext'] := GetMinTextForBlend(Form1.BlendTypeComboBox.ItemIndex);
             param.Attributes['maxtext'] := GetMaxTextForBlend(Form1.BlendTypeComboBox.ItemIndex);
           end
        end
    end;

  Form1.XMLDocument1.SaveToFile(s + 'ui\' + GetPackFile);
end;

Procedure DeleteDir(file_Path:String);
var
  sr:TSearchRec;
  ScanDir:String;
  ISearch:Integer;
begin
  while (file_Path[Length(file_Path)] <> '/')  AND (file_Path[Length(file_Path)] <> '\') do
    Delete(file_Path, Length(file_Path), 1);

  ScanDir:= file_Path;

  ISearch:=FindFirst(ScanDir+'*.*', faAnyfile, sr);

  while ISearch=0 do
    begin
      if ((sr.Attr and FaDirectory) = Fadirectory) and (sr.Name[1]<>'.') then
      DeleteDir(ScanDir+sr.Name+'')
    else
      DeleteFile(scandir+sr.Name);
      ISearch:=FindNext(sr);
    end;

  FindClose(sr);
  RemoveDirectory(Pchar(file_Path));{Удаляем папку}
end;

Procedure InsertBlendInMesh(s: string; src: string; target: String);
Var
  a, b: IXMLNode;
  found: Boolean;
  i, j: Integer;
  path: String;

begin
  For i := 1 To Length(src) Do
  If src[i] = '\' Then src[i] := '/';

  Form1.XMLDocument1.LoadFromFile(s + 'mesh.xml');
  Form1.XMLDocument1.Active := true;
  If Form1.XMLDocument1.IsEmptyDoc Then
    begin
      MessageDlg('В папке тела нет описания меша', mtError, [mbOk], 0);
      Exit;
    end;
  a := Form1.XMLDocument1.DocumentElement.ChildNodes.FindNode(WideString('blends'));

  if a = nil then
    begin
      Form1.XMLDocument1.DocumentElement.AddChild('blends');
      a := Form1.XMLDocument1.DocumentElement.ChildNodes.FindNode(WideString('blends'));
    end;

  found := false;

  for i := 0 To a.ChildNodes.Count - 1 Do
  if (a.ChildNodes[i].Attributes['src'] = src) And
     (a.ChildNodes[i].Attributes['target'] = target) Then
        found := true;

  if not found then
    begin
      b := a.AddChild('blend');
      b.Attributes['src'] := src;
      b.Attributes['target'] := target;
    end;

  Form1.XMLDocument1.SaveToFile(s + 'mesh.xml');
end;

Procedure TForm1.CopyModelTextures(s: String);
Var
  i: Longint;
  s1, s2, addPath: String;
begin
  addPath := Form1.AvatarComboBox.Text;
  if (not Form1.BlendRadioButton.Checked) And (not Form1.BlendForClothesOrHairdressRadioButton.Checked) then
    begin
      if Form1.ClothesRadioButton.Checked then
        addPath := addPath + '\clothes\'
      else
        addPath := addPath + '\hair\';
      addPath := addPath + Form1.ModelName.Text + '\';
    end
  else
    Exit;


  For i := 1 To matCount Do
    begin
      {if (WindowsCopyFile(programDir + '\mtrl' + IntToStr(i) + '.crf', s) = false) Then
        begin
          If FileExists(programDir + '\mtrl' + IntToStr(i) + '.crf') = false Then
            MessageDlg('В папке нет файла материала mtrl' + IntToStr(i) + '.crf или он не может быть скопирован в папку клиента!', mtError, [mbOk], 0);
        end;}
      If not Form1.BlendRadioButton.Checked Then
        begin
          if IsMaterialExists(i) then
            WindowsCopyFile(programDir + '\mat' + IntToStr(i) + '.xml', s);
          GetTextures(i, s1, s2);
          if s1 <> '' then
            begin
              If Not Form1.UseTexturesFromCheckBox.Checked Then
              If Not Form1.UseServerTexturesCheckBox.Checked Then
                WindowsCopyFile(s1, s)
              else
                begin
                  ForceDirectories(PChar(SERVER1_PATH + addPath));
                  ForceDirectories(PChar(SERVER2_PATH + addPath));
                  WindowsCopyFile(s1, SERVER1_PATH + addPath);
                  WindowsCopyFile(s1, SERVER2_PATH + addPath);
                end;
            end;
          if s2 <> '' then
            begin
              If Not Form1.UseTexturesFromCheckBox.Checked Then
              If Not Form1.UseServerTexturesCheckBox.Checked Then
                WindowsCopyFile(s2, s)
              else
                begin
                  ForceDirectories(PChar(SERVER1_PATH + addPath));
                  ForceDirectories(PChar(SERVER2_PATH + addPath));
                  WindowsCopyFile(s2, SERVER1_PATH + addPath);
                  WindowsCopyFile(s2, SERVER2_PATH + addPath);
                end;
            end;
          s1 := GetTexture(i);
          if s1 <> '' then
            begin
              If Not Form1.UseTexturesFromCheckBox.Checked Then
              If Not Form1.UseServerTexturesCheckBox.Checked Then
                WindowsCopyFile(s1, s)
              else
                begin
                  ForceDirectories(PChar(SERVER1_PATH + addPath));
                  ForceDirectories(PChar(SERVER2_PATH + addPath));
                  WindowsCopyFile(s1, SERVER1_PATH + addPath);
                  WindowsCopyFile(s1, SERVER2_PATH + addPath);
                end;
            end;
        end;
    end;

  If Form1.UseServerTexturesCheckBox.Checked Then
    ReloadResServerCache;
end;

Function TForm1.GetModelPath: String;
Var
  s: string;
begin
  s := Form1.ClientPathEdit.Text;
  if s[Length(s)] <> '\' then
    s := s + '\';

  s := s + 'ui\resources\avatars\editor\' + Form1.AvatarComboBox.Text;
  if (not Form1.BlendRadioButton.Checked) AND (not Form1.BlendForClothesOrHairdressRadioButton.Checked)  then
    begin
      if Form1.ClothesRadioButton.Checked then
        s := s + '\clothes\'
      else
        s := s + '\hair\';
      s := s + Form1.ModelName.Text + '\';
    end;

  GetModelPath := s;
end;

Procedure MakeXML;
Var
  s, s1, s2, texturesPathString, changePath, tmPath: String;
  f: TextFile;
  i, ii, blendID: Longint;
  isClips: Boolean;
  blends : IXMLNode;
begin
  s := Form1.GetModelPath;

  if Form1.BlendRadioButton.Checked then
    begin
      s := s + '\body';
      if (Form1.HeadIDComboBox.ItemIndex <> 0) AND (Form1.HeadIDComboBox.ItemIndex <> -1) then
        s := s + IntToStr(Form1.HeadIDComboBox.ItemIndex + 1);
      s := s + '\';

      blendID := Form1.BlendTypeComboBox.ItemIndex;

      InsertBlendInMesh(s, GetBlendSrc(blendID) + 'mesh.xml', GetBlendTarget(blendID));
      changePath := s + GetBlendSrc(blendID) + 'ischange';

      s := s + GetBlendSrc(blendID);
    end;

    if Form1.BlendForClothesOrHairdressRadioButton.Checked then
    begin
      if (Form1.ClothesAndHairdressesComboBox.ItemIndex <> -1) then //And (Form1.ClothesAndHairdressesComboBox.ItemIndex <> -1) then
        s := s + '\' + hairOrClothesCorrespondenceIndex[Form1.ClothesAndHairdressesComboBox.ItemIndex] + '\' + GetThingFolder(Form1.AvatarComboBox.Text, Form1.ClothesAndHairdressesComboBox.Text)
      else
        begin
           MessageDlg('Выберите нужную вещь или прическу, для которой создаете бленд!', mtError, [mbOk], 0);
           Exit;
        end;
      s := s + '\';

      If (Form1.ModelName.Text = '') Then
        begin
           MessageDlg('Имя вещи не может быть пустым!', mtError, [mbOk], 0);
           Exit;
        end;

      If (Form1.BlendParamNameEdit.Text = '') OR (Form1.BlendParamMinNameEdit.Text = '') OR (Form1.BlendParamMaxNameEdit.Text = '') Then
        begin
           MessageDlg('Укажите название всех параметров!', mtError, [mbOk], 0);
           Exit;
        end;

      InsertBlendInMesh(s, Form1.ModelName.Text +'\mesh.xml', Form1.ClothesAndHairdressesComboBox.Text);
      changePath := s + Form1.ModelName.Text +'\'+ 'ischange';

      s :=  s  + Form1.ModelName.Text + '\';
    end;

  if (not Form1.BlendRadioButton.Checked) AND (not Form1.BlendForClothesOrHairdressRadioButton.Checked)  then
    begin
      InsertMeshIntoPack(Form1.ModelName.Text, Form1.ClothesRadioButton.Checked);
    end;

  if (Form1.BlendRadioButton.Checked) AND (Form1.BlendTypeComboBox.ItemIndex > 9) then
    begin
      InsertMeshIntoPack('head'+Form1.HeadIDComboBox.Text, false);
    end;

  if (Form1.BlendForClothesOrHairdressRadioButton.Checked) AND (Form1.BlendParamNameEdit.Text <> '') AND (Form1.BlendParamMinNameEdit.Text <> '') AND (Form1.BlendParamMaxNameEdit.Text <> '')  then
      begin
        if (hairOrClothesCorrespondenceIndex[Form1.ClothesAndHairdressesComboBox.ItemIndex] = 'clothes') then
          InsertMeshIntoPack(Form1.ClothesAndHairdressesComboBox.Text, true)
        else
         InsertMeshIntoPack(Form1.ClothesAndHairdressesComboBox.Text, false);
        end;

  ForceDirectories(PAnsiChar(s));

  If (Length(changePath) > 0) Then
    begin
      AssignFile(f, changePath);
      Rewrite(f);
      CloseFile(f);
    end;

  if (Form1.ClothesRadioButton.Checked) OR (Form1.HairRadioButton.Checked) then
    begin
      If FileExists(s + 'mesh.xml') Then
        begin
          Form1.XMLDocument1.LoadFromFile(s + 'mesh.xml');
          try
            Form1.XMLDocument1.Active := true;
          except
            MessageDlg('Невозможно открыть старый mesh.xml', mtError, [mbOk], 0);
            Exit;
          end;

          if Form1.XMLDocument1.IsEmptyDoc Then
            begin
              MessageDlg('В папке тела нет описания меша', mtError, [mbOk], 0);
              Exit;
            end;

          blends := Form1.XMLDocument1.DocumentElement.ChildNodes.FindNode(WideString('blends'));
        end;
    end;

  AssignFile(f, s + 'mesh.xml');
  Rewrite(f);
  texturesPathString := '';
  if (not Form1.BlendRadioButton.Checked) AND (not Form1.BlendForClothesOrHairdressRadioButton.Checked) then
  If Form1.UseTexturesFromCheckBox.Checked Then
    begin
      texturesPathString := ' texturespath="..\..\..\'+Form1.AvatarForTexturesComboBox.Text+'\';
      if Form1.ClothesRadioButton.Checked then
        texturesPathString := texturesPathString + 'clothes'
      else if Form1.HairRadioButton.Checked then
        texturesPathString := texturesPathString + 'hair';
      texturesPathString := texturesPathString + '\'+ GetThingFolder(Form1.AvatarForTexturesComboBox.Text, Form1.ModelForTexturesNameComboBox.Text)+'\"';
    end;

  WriteLn(f, '<object scale="1.000000" id="'+IntToStr(GetAvatarCompatabilityID)+'"'+texturesPathString+'>');
  If (not Form1.BlendRadioButton.Checked) AND (not Form1.BlendForClothesOrHairdressRadioButton.Checked) then
    WriteLn(f, '  <skeleton src="..\..\body\skel.csf"/>')
  else
    begin
      if (Form1.BlendForClothesOrHairdressRadioButton.Checked) then
           WriteLn(f, '  <skeleton src="..\..\..\body\skel.csf"/>')
      else
      begin
        blendID := Form1.BlendTypeComboBox.ItemIndex;
        if (blendID <> 3) AND (blendID < 10) then
          WriteLn(f, '  <skeleton src="..\..\..\body\skel.csf"/>')
        else
          WriteLn(f, '  <skeleton src="..\..\body\skel.csf"/>');
        end;
    end;
  WriteLn(f, '  <mesh src="mesh.cmf"/>');
  WriteLn(f, '  <materials>');
  For i := 1 To matCount Do
    begin
      tmPath := GetTexture(i);
      If Length(tmPath) = 0 Then
        ShowMessage('Предупреждение. В материале номер ' + IntToStr(i) + ' отсутствует текстура!');
      Write(f, '    <material texture="' + tmPath + '" ');
      If (not Form1.BlendRadioButton.Checked) AND (not Form1.BlendForClothesOrHairdressRadioButton.Checked) then
        begin
          if (texturesPath[i-1] <>'') then
            begin
              Write(f, 'path="' + texturesPath[i-1] + '" ');
            end;
          if IsMaterialExists(i) then
           begin
             Write(f, 'base="mat' + IntToStr(i) + '.xml"');
           end;
        end;
      WriteLn(f, '/>');
    end;
  WriteLn(f, '  </materials>');

  isClips := false;
  For ii := 1 To 5 Do
    if clipboxs[ii].enabled then
          begin
            if not isClips then
              begin
                WriteLn(f, '  <clipboxs>');
                isClips := true;
              end;
            Write(f, '      <clipbox ');
            Write(f, ' id="' + IntToStr(clipboxs[ii].id)+'"');
            Write(f, ' ort="1"');
            Write(f, ' start="' + FloatToStr(clipboxs[ii].x1)+'"');
            Write(f, ' end="' + FloatToStr(clipboxs[ii].x2)+'"');
            Write(f, ' ort2="2"');
            Write(f, ' start2="' + FloatToStr(clipboxs[ii].y1)+'"');
            Write(f, ' end2="' + FloatToStr(clipboxs[ii].y2)+'"');
            Write(f, ' ort3="3"');
            Write(f, ' start3="' + FloatToStr(clipboxs[ii].z1)+'"');
            Write(f, ' end3="' + FloatToStr(clipboxs[ii].z2)+'"');
            WriteLn(f, '/>');
          end;
  if isClips then
    begin
      WriteLn(f, '  </clipboxs>');
    end;

  if (blends <> nil)  then
  if  (Form1.DeleteBlendsCheckBox.Enabled) AND  (not Form1.DeleteBlendsCheckBox.Checked) then
          for i:=0 to blends.ChildNodes.Count-1 Do
            begin
                if (i = 0) then
                     WriteLn (f, '  <blends>');
                WriteLn(f, '    <blend src ="' + blends.ChildNodes[i].Attributes['src'] +'" target ="'+ blends.ChildNodes[i].Attributes['target']+'" />');
                if (i = blends.ChildNodes.Count-1) then
                     WriteLn (f, '  </blends>');
            end
  else if (Form1.DeleteBlendsCheckBox.Enabled) AND  (Form1.DeleteBlendsCheckBox.Checked) then
       for i:=0 to  blends.ChildNodes.Count -1 Do
        DeleteDir(s + blends.ChildNodes[i].Attributes['src']);

  WriteLn(f, '</object>');
  CloseFile(f);

  ChDir(programDir);
  If (WindowsCopyFile('mesh.cmf', s) = false) Then
    begin
      MessageDlg('В папке нет файла меша mesh.cmf или он не может быть скопирован в папку клиента!', mtError, [mbOk], 0);
    end;

  If (not Form1.BlendRadioButton.Checked) AND (not Form1.BlendForClothesOrHairdressRadioButton.Checked) then
    begin
      if (WindowsCopyFile('prev.png', s) = false) Then
        begin
          MessageDlg('В папке нет файла превью prev.png или он не может быть скопирован в папку клиента!', mtError, [mbOk], 0);
        end;
    end;

  Form1.CopyModelTextures(s);

  MessageDlg('Успешно!', mtInformation, [mbOk], 0);
end;

procedure TForm1.CopyMeshButtonClick(Sender: TObject);
begin
  If (ClientPathEdit.Text = '') Then
    begin
      MessageDlg('Неверно задана папка клиента', mtError, [mbOk], 0);
      Exit;
    end;

  If (Length(ModelName.Text) = 0) And (not BlendRadioButton.Checked) Then
    begin
      MessageDlg('Имя одежды/прически неверно задано', mtError, [mbOk], 0);
      Exit;
    end;

  ParseXML;

  MakeXML;
end;

procedure TForm1.ConfigureMaterialsButtonClick(Sender: TObject);
begin
  ParseXML;
  
  Form2.ShowModal;
end;

procedure TForm1.ConfigureClipboxesButtonClick(Sender: TObject);
begin
  Form4.ShowModal;
end;

procedure TForm1.RunClientButtonClick(Sender: TObject);
Var
  s, dir: String;
  ini: TIniFile;
begin
  s := ClientPathEdit.Text;
  If s[Length(s)] <> '\' Then
    s := s + '\';
  dir := s;

  ini := TIniFile.Create(s + 'player.ini');
  ini.WriteString('run', 'fast', 'true');
  ini.free;

  s := s + 'player.exe';
  ChDir(dir);
  WinExec(PAnsiChar(s), SW_SHOW);
  Sleep(500);
end;

Function IsFileExists(fn: String): Boolean;
Var
  f: TextFile;
begin
  AssignFile(f, fn);
  {$I-}
  Reset(f);
  {$I+}
  If IOResult <> 0 Then
    IsFileExists := false
  else
    begin
      CloseFile(f);
      IsFileExists := true;
    end;
end;


procedure TForm1.UseTexturesFromCheckBoxClick(Sender: TObject);
begin
  AvatarForTexturesComboBox.Enabled := UseTexturesFromCheckBox.Checked;
  ModelForTexturesNameComboBox.Enabled := UseTexturesFromCheckBox.Checked;
  UseServerTexturesCheckBox.Enabled := not UseTexturesFromCheckBox.Checked;
end;

procedure TForm1.UseServerTexturesCheckBoxClick(Sender: TObject);
begin
  UseTexturesFromCheckBox.Enabled := not UseServerTexturesCheckBox.Checked;
end;

procedure TForm1.AvatarComboBoxChange(Sender: TObject);
Var
  s: String;
  av: IXMLNode;
  hcl:  IXMLNodeList;
  i, j, li, k, z: Longint;
  it, tmp: String;
begin
  s := ClientPathEdit.Text;

  If Length(s) = 0 Then
    Exit;

  If s[Length(s)] <> '\' Then
    s := s + '\';

  if not IsFileExists(s + 'ui\' + GetPackFile) then
    Exit;

  Form1.XMLDocument1.LoadFromFile(s + 'ui\' + GetPackFile);
  try
    Form1.XMLDocument1.Active := true;
  except
    Exit;
  end;
  If Form1.XMLDocument1.IsEmptyDoc Then
    begin
      MessageDlg('В папке тела нет описания меша', mtError, [mbOk], 0);
      Exit;
    end;
  av := Form1.XMLDocument1.DocumentElement;
  if av = nil then
    begin
      MessageDlg('В меше нет материалов', mtError, [mbOk], 0);
      Exit;
    end;

  li := ClothesType.ItemIndex;
  it := ClothesType.Items[li];
  ClothesType.Clear;
  parts := TStringList.Create;

  ClothesAndHairdressesComboBox.Clear;
  z := 0;
  for i := 0 To av.ChildNodes.Count - 1 Do
  begin
  if (not VarIsNull(av.ChildNodes[i].Attributes['name'])) And (IsClothesID(av.ChildNodes[i].Attributes['id'])) then
    begin
      ClothesType.AddItem(av.ChildNodes[i].Attributes['name'], nil);
      parts.Add(av.ChildNodes[i].Attributes['id']);
      tmp := av.ChildNodes[i].Attributes['name'];

      if tmp = it then
        ClothesType.ItemIndex := ClothesType.Items.Count - 1;
    end;
   if (not VarIsNull(av.ChildNodes[i].Attributes['name'])) And (IsClothesOrHairID(av.ChildNodes[i].Attributes['id'])) then
    begin
       for j:= 0 To av.ChildNodes[i].ChildNodes.Count-1 Do
       for k:= 0 To av.ChildNodes[i].ChildNodes[j].ChildNodes.Count-1 Do
        if (not VarIsNull(av.ChildNodes[i].ChildNodes[j].ChildNodes[k].Attributes['id'])) then
          if (av.ChildNodes[i].ChildNodes[j].ChildNodes[k].Attributes['id'] <> 'zero') then
          begin
            ClothesAndHairdressesComboBox.AddItem(av.ChildNodes[i].ChildNodes[j].ChildNodes[k].Attributes['id'], nil);
            if (IsClothesID(av.ChildNodes[i].Attributes['id'])) then
              hairOrClothesCorrespondenceIndex[z] := 'clothes'
             else
             hairOrClothesCorrespondenceIndex[z] := 'hair';
            z := z+1;
           end;
      end;
  end;

  if (z <> 0) then
    ClothesAndHairdressesComboBox.ItemIndex := 0;
{  ComboBox4.AddItem('Новая одежда', nil);
  if 'Новая одежда' = it then
    ComboBox4.ItemIndex := ComboBox4.Items.Count - 1;}

  if ClothesType.ItemIndex = -1 then
    ClothesType.ItemIndex := 0;
end;

procedure TForm1.ClothesTypeChange(Sender: TObject);
begin
  Form4.UpdatePermittedClipboxs;
  Form4.UpdateClipboxs;
end;

procedure TForm1.AvatarForTexturesComboBoxChange(Sender: TObject);
Var
  s: String;
  av: IXMLNode;
  i, li, k: Longint;
  it, tmp: String;
begin
  s := ClientPathEdit.Text;
  If s[Length(s)] <> '\' Then
    s := s + '\';

  if not IsFileExists(s + 'ui\' + GetPackFileCopy) then
    Exit;

  Form1.XMLDocument1.LoadFromFile(s + 'ui\' + GetPackFileCopy);
  try
    Form1.XMLDocument1.Active := true;
  except
    Exit;
  end;
  If Form1.XMLDocument1.IsEmptyDoc Then
    begin
      MessageDlg('Ошибка КТ01', mtError, [mbOk], 0);
      Exit;
    end;
  av := Form1.XMLDocument1.DocumentElement;
  if av = nil then
    begin
      MessageDlg('Ошибка КТ02', mtError, [mbOk], 0);
      Exit;
    end;

  li := ModelForTexturesNameComboBox.ItemIndex;
  it := ModelForTexturesNameComboBox.Items[li];
  ModelForTexturesNameComboBox.Clear;

  for i := 0 To av.ChildNodes.Count - 1 Do
  if (not VarIsNull(av.ChildNodes[i].Attributes['name'])) And (IsClothesOrHairID(av.ChildNodes[i].Attributes['id'])) then
    begin
      for k := 0 to av.ChildNodes[i].ChildNodes[0].ChildNodes.Count - 1 do
      if (not VarIsNull(av.ChildNodes[i].ChildNodes[0].ChildNodes[k].Attributes['id'])) Then
        begin
          tmp := av.ChildNodes[i].ChildNodes[0].ChildNodes[k].Attributes['id'];
          if (tmp <> 'zero') then
            ModelForTexturesNameComboBox.AddItem(tmp, nil);

          if tmp = it then
            ModelForTexturesNameComboBox.ItemIndex := ModelForTexturesNameComboBox.Items.Count - 1;
        end;
    end;

  if ModelForTexturesNameComboBox.ItemIndex = -1 then
    ModelForTexturesNameComboBox.ItemIndex := 0;
end;

procedure TForm1.ClientPathEditChange(Sender: TObject);
begin
  AvatarComboBoxChange(sender);
  ReloadAvatarsList(ClientPathEdit.Text);
  Form4.UpdatePermittedClipboxs;
  Form4.UpdateClipboxs;
end;

procedure TForm1.FormCanResize(Sender: TObject; var NewWidth,
  NewHeight: Integer; var Resize: Boolean);
begin
  resize := false;
end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
Var
  a: TIniFile;
  f: TextFile;
  i: Integer;
begin
  a := TIniFile.Create(programDir + '\thing.ini');
  a.WriteString('client', 'path', ClientPathEdit.Text);
  a.WriteString('thing', 'name', ModelName.Text);
  a.WriteString('thing', 'dest', IntToStr(AvatarComboBox.ItemIndex));
  if ClothesRadioButton.Checked Then
    a.WriteString('thing', 'type', '1');
  if HairRadioButton.Checked Then
    a.WriteString('thing', 'type', '2');
  if BlendRadioButton.Checked Then
    a.WriteString('thing', 'type', '3');
  a.WriteString('thing', 'blend_type', IntToStr(BlendTypeComboBox.ItemIndex));
  a.WriteString('thing', 'blend_head', IntToStr(HeadIDComboBox.ItemIndex));
  a.WriteString('thing', 'cloth_type', IntToStr(ClothesType.ItemIndex));
  a.WriteString('thing', 'get_texture_from', IntToStr(AvatarForTexturesComboBox.ItemIndex));
  a.WriteString('thing', 'get_texture_from_cloth', ModelForTexturesNameComboBox.Text);
  if  BlendForClothesOrHairdressRadioButton.Checked  then
     a.WriteString('thing', 'type', '4');
  a.WriteString('thing', 'blend_clothes_or_head', IntToStr(ClothesAndHairdressesComboBox.ItemIndex));
  a.WriteString('thing', 'blend_param_name', BlendParamNameEdit.Text);
  a.WriteString('thing', 'blend_param_min_name', BlendParamMinNameEdit.Text);
  a.WriteString('thing', 'blend_param_max_name', BlendParamMaxNameEdit.Text);
  if DeleteBlendsCheckBox.Checked = True then
    a.WriteString('thing', 'is_delete_blends', '1')
  else
  a.WriteString('thing', 'is_delete_blends', '0');
  if UseTexturesFromCheckBox.Checked = True then
    a.WriteString('thing', 'is_get_texture_from', '1')
  else
    a.WriteString('thing', 'is_get_texture_from', '0');
  if UseServerTexturesCheckBox.Checked = True then
    a.WriteString('thing', 'is_copy_textures_to_server', '1')
  else
    a.WriteString('thing', 'is_copy_textures_to_server', '0');

  for  i:=0 To 100 Do
  begin
      if (texturesPath[i] <> '') then
        a.WriteString('thing', 'texturesPath[' + IntToStr(i) +']', texturesPath[i]);
  end;

  a.Free;

  AssignFile(f, 'update.bat');
  {$I-}
  Erase(f);
  {$I+}
  IF IOResult <> 0 Then
    Exit;
  {$I-}
  CloseFile(f);
  {$I+}
  IF IOResult <> 0 Then
    Exit;
end;

Function WinDir: String;
Var
  IntLen: Integer;
  StrBuffer: String;
Begin
  SetLength(StrBuffer, 1000);
  IntLen := GetWindowsDirectory(PChar(StrBuffer), 1000);
  WinDir := Trim(Copy(StrBuffer, 1, IntLen));
End;

procedure TForm1.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
Var
  dsk: String;
begin
  dsk := WinDir;
  Delete(dsk, 4, Length(dsk) - 3);

  If key = VK_F1 Then
    begin
      ClientPathEdit.Text := 'D:\vacademia\VU2008\';
    end;

  If key = VK_F2 Then
    begin
      ClientPathEdit.Text := dsk + 'Program Files\Virtual Academia\';
    end;

  If key = VK_F10 Then
    Application.Terminate;
end;

function Are2FilesEqual(const File1, File2: TFileName): Boolean;
var 
  ms1, ms2: TMemoryStream; 
begin 
  Result := False; 
  ms1 := TMemoryStream.Create;
  try 
    ms1.LoadFromFile(File1); 
    ms2 := TMemoryStream.Create; 
    try 
      ms2.LoadFromFile(File2); 
      if ms1.Size = ms2.Size then
        Result := CompareMem(ms1.Memory, ms2.memory, ms1.Size)
      else
        Result := false;   
    finally 
      ms2.Free; 
    end; 
  finally 
    ms1.Free; 
  end 
end;

Procedure AddToAvatarsList(id: Integer; folder: String; compatID: Integer;
    isWoman: Boolean);
begin
  Inc(avatarsCount);
  avatars[avatarsCount].id := id;
  avatars[avatarsCount].folder := folder;
  avatars[avatarsCount].compatID := compatID;
  avatars[avatarsCount].isWoman := isWoman;
end;

Procedure TForm1.ReloadAvatarsList(path: String);
Var
  avatarList, avatar, av: IXMLNode;
  tmpID, tmpFolder, tmpCID, tmpIsWoman, currentAvatar: String;
  avatarID, currentAvatarIndex: Integer;
begin
  avatarsCount := 0;
  currentAvatarIndex := 0;
  currentAvatar := AvatarComboBox.Text;
  AvatarComboBox.Items.Clear();
  AvatarComboBox.Text := '';

  Form1.XMLDocument1.LoadFromFile(path + 'ui\avatarlist.xml');
  try
    Form1.XMLDocument1.Active := true;
  except
    Exit;
  end;
  If Form1.XMLDocument1.IsEmptyDoc Then
    begin
      Exit;
    end;
  avatarList := Form1.XMLDocument1.DocumentElement;
  if avatarList = nil then
    begin
      Exit;
    end;

      if (avatarList <> nil) Then
      For avatarID := 0 To avatarList.ChildNodes.Count - 1 Do
        begin
          avatar := avatarList.ChildNodes[avatarID];
          tmpID := '';
          tmpFolder := '';
          tmpCID := '';
          tmpIsWoman := '';
          If (not VarIsNull(avatar.Attributes['id'])) Then
            begin
              tmpID := avatar.Attributes['id'];
            end;

          If (not VarIsNull(avatar.Attributes['folder'])) Then
            begin
              tmpFolder := avatar.Attributes['folder'];
            end;

          If (not VarIsNull(avatar.Attributes['cid'])) Then
            begin
              tmpCID := avatar.Attributes['cid'];
            end;

          If (not VarIsNull(avatar.Attributes['woman'])) Then
            begin
              tmpIsWoman := avatar.Attributes['woman'];
            end;

           Inc(avatarsCount);
           avatars[avatarsCount].id := StrToInt(tmpID);
           avatars[avatarsCount].folder := tmpFolder;
           avatars[avatarsCount].compatID := StrToInt(tmpCID);
           avatars[avatarsCount].isWoman := (tmpIsWoman = 'true');

           AvatarComboBox.Items.Add(tmpFolder);

           if (currentAvatar = tmpFolder) Then
              currentAvatarIndex := avatarsCount;
        end;
   if (currentAvatarIndex > 0) Then
     begin
       AvatarComboBox.Text := currentAvatar;
     end;
end;

procedure TForm1.FormShow(Sender: TObject);
Var
  a: TIniFile;
  dsk, exePath, batPath: String;
  defThingName, getTextureFromCloth, texturePath: String;
  i: Integer;
  f: TextFile;
  isUpdatePermitted: Boolean;
Const
  newEditorPath = 'R:\VU2008\cloth_editor\cloth_editor.exe';
begin
  GetDir(0, programDir);

  exePath := ParamStr(0);
  isUpdatePermitted := false;

  if Not FileExists(programDir + '\cloth_editor.dpr') then
  if FileExists(newEditorPath) then
  if Not Are2FilesEqual(newEditorPath, exePath) then
    begin
      isUpdatePermitted := true;

      AssignFile(f, programDir + '\try_updating');
      {$I-}
      Reset(f);
      {$I+}
      If IOResult = 0 Then
        begin
          isUpdatePermitted := false;
          CloseFile(f);
        end;

      If (isUpdatePermitted) Then
        begin
          AssignFile(f, programDir + '\try_updating');
          {$I-}
          Rewrite(f);
          {$I+}
          If IOResult = 0 Then
            begin
              CloseFile(f);
            end;

          AssignFile(f, programDir + '\update.bat');
          Rewrite(f);
          WriteLn(f, 'cd ' + programDir);
          WriteLn(f, 'taskkill /im cloth_editor.exe');
          WriteLn(f, 'copy ' + newEditorPath + ' ' + Application.ExeName);
          WriteLn(f, 'cd ' + programDir);          
          WriteLn(f, 'cloth_editor.exe');
          CloseFile(f);
          batPath := programDir + '\update.bat';
          WinExec(PChar(batPath), 0);
          Application.Terminate;     
        end;
    end;

  If isUpdatePermitted = false Then
  if FileExists(programDir + '\try_updating') then
    begin
      AssignFile(f, programDir + '\try_updating');
      {$I-}
      Erase(f);
      {$I+}
      If IOResult <> 0 Then;
    end;

  dsk := WinDir;
  Delete(dsk, 4, Length(dsk) - 3);

  i := Length(programDir);
  while (i <> 0) And (programDir[i] <> '\') do
    begin
      defThingName := programDir[i] + defThingName;
      Dec(i);
    end;

  if i = 0 then
    defThingName := '';

  a := TIniFile.Create(programDir + '\thing.ini');
  ClientPathEdit.Text := a.ReadString('client', 'path', dsk + 'Program Files\Virtual Academia\');
  ModelName.Text := a.ReadString('thing', 'name', defThingName);
  AvatarComboBox.ItemIndex := StrToInt(a.ReadString('thing', 'dest', '4'));
  AvatarComboBox.OnChange(sender);
  BlendTypeComboBox.ItemIndex := StrToInt(a.ReadString('thing', 'blend_type', '0'));
  HeadIDComboBox.ItemIndex := StrToInt(a.ReadString('thing', 'blend_head', '0'));
  ClothesType.ItemIndex := StrToInt(a.ReadString('thing', 'cloth_type', '0'));
  ClothesType.OnChange(sender);
  ClothesRadioButton.Checked := (a.ReadString('thing', 'type', '') = '1');
  HairRadioButton.Checked := (a.ReadString('thing', 'type', '') = '2');
  BlendRadioButton.Checked := (a.ReadString('thing', 'type', '') = '3');
  AvatarForTexturesComboBox.ItemIndex := StrToInt(a.ReadString('thing', 'get_texture_from', '0'));
  AvatarForTexturesComboBox.OnChange(sender);
  getTextureFromCloth := a.ReadString('thing', 'get_texture_from_cloth', '');
  for I := 0 to ModelForTexturesNameComboBox.Items.Count - 1 do
  If ModelForTexturesNameComboBox.Items[i] = getTextureFromCloth Then
    begin
      ModelForTexturesNameComboBox.ItemIndex := i;
    end;

  BlendForClothesOrHairdressRadioButton.Checked :=  (a.ReadString('thing', 'type', '') = '4');
  ClothesAndHairdressesComboBox.ItemIndex := StrToInt(a.ReadString('thing', 'blend_clothes_or_head', '0'));
  BlendParamNameEdit.Text := a.ReadString('thing', 'blend_param_name', '');
  BlendParamMinNameEdit.Text := a.ReadString('thing', 'blend_param_min_name', '');
  BlendParamMaxNameEdit.Text := a.ReadString('thing', 'blend_param_max_name', '');

  DeleteBlendsCheckBox.Checked := a.ReadString('thing', 'is_delete_blends', '0') = '1';
  UseTexturesFromCheckBox.Checked := a.ReadString('thing', 'is_get_texture_from', '0') = '1';
  UseServerTexturesCheckBox.Checked := a.ReadString('thing', 'is_copy_textures_to_server', '0') = '1';

  for i := 0 to 100 do
  begin
    texturePath := a.ReadString('thing', 'texturesPath['+IntToStr(i)+']', '');
    texturesPath[i] := texturePath;
  end;

  a.Free;

  UseTexturesFromCheckBoxClick(sender);
  UseServerTexturesCheckBoxClick(sender);

  Form4.Visible := true;
  Sleep(1);
  Form4.Visible := false;

  ReloadAvatarsList(ClientPathEdit.Text);
end;

procedure TForm1.BlendRadioButtonClick(Sender: TObject);
begin
  AvatarRadioButton.Checked := false;
  UseTexturesFromCheckBox.Enabled := not BlendRadioButton.Checked;
  UseServerTexturesCheckBox.Enabled := not BlendRadioButton.Checked;
  UseTexturesFromCheckBoxClick(sender);
  UseServerTexturesCheckBoxClick(sender);
  BlendTypeComboBox.Enabled := BlendRadioButton.Checked;
  HeadIDComboBox.Enabled := BlendRadioButton.Checked;
  HeadLabel.Enabled := BlendRadioButton.Checked;
  BlendForLabel.Enabled := BlendRadioButton.Checked;
  ConfigureMaterialsButton.Enabled := not BlendRadioButton.Checked;
  ConfigureClipboxesButton.Enabled := not BlendRadioButton.Checked;
  ModelName.Enabled := not BlendRadioButton.Checked;
  ClothesType.Enabled := ClothesRadioButton.Checked;
  ClothesAndHairdressesComboBox.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
  BlendParamNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
  BlendParamMinNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
  BlendParamMaxNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
  DeleteBlendsCheckBox.Enabled :=  HairRadioButton.Checked or ClothesRadioButton.Checked;
  ModelName.Enabled := true;
  AvatarComboBox.Enabled := true;
  AvatarNumEdit.Enabled := false;  
end;

procedure TForm1.BlendForClothesOrHairdressRadioButtonClick(Sender: TObject);
begin
    AvatarRadioButton.Checked := false;
    BlendTypeComboBox.Enabled := not BlendForClothesOrHairdressRadioButton.Checked;
    HeadIDComboBox.Enabled := not BlendForClothesOrHairdressRadioButton.Checked;
    UseTexturesFromCheckBox.Enabled := not BlendForClothesOrHairdressRadioButton.Checked;
    UseServerTexturesCheckBox.Enabled := not BlendForClothesOrHairdressRadioButton.Checked;
    UseTexturesFromCheckBoxClick(sender);
    UseServerTexturesCheckBoxClick(sender);
    ClothesAndHairdressesComboBox.Enabled:= BlendForClothesOrHairdressRadioButton.Checked;
    BlendParamNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
    BlendParamMinNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
    BlendParamMaxNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
    ModelName.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
    ConfigureMaterialsButton.Enabled := not BlendForClothesOrHairdressRadioButton.Checked;
    ConfigureClipboxesButton.Enabled := not BlendForClothesOrHairdressRadioButton.Checked;
    DeleteBlendsCheckBox.Enabled :=  HairRadioButton.Checked or ClothesRadioButton.Checked;
  ModelName.Enabled := true;
  AvatarComboBox.Enabled := true;
    AvatarNumEdit.Enabled := false;
end;

procedure TForm1.AvatarRadioButtonClick(Sender: TObject);
begin
  BlendRadioButton.Checked := false;
  ClothesRadioButton.Checked := false;
  HairRadioButton.Checked := false;
  BlendForClothesOrHairdressRadioButton.Checked := false;
  UseTexturesFromCheckBox.Enabled := false;
  UseServerTexturesCheckBox.Enabled := false;
  BlendTypeComboBox.Enabled := BlendRadioButton.Checked;
  HeadIDComboBox.Enabled := BlendRadioButton.Checked;
  HeadLabel.Enabled := BlendRadioButton.Checked;
  BlendForLabel.Enabled := BlendRadioButton.Checked;
  ConfigureMaterialsButton.Enabled := not BlendRadioButton.Checked;
  ConfigureClipboxesButton.Enabled := not BlendRadioButton.Checked;
  ModelName.Enabled := not BlendRadioButton.Checked;
  ClothesType.Enabled := ClothesRadioButton.Checked;
  ClothesAndHairdressesComboBox.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
  BlendParamNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
  BlendParamMinNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
  BlendParamMaxNameEdit.Enabled := BlendForClothesOrHairdressRadioButton.Checked;
  DeleteBlendsCheckBox.Enabled :=  HairRadioButton.Checked or ClothesRadioButton.Checked;
  ModelName.Enabled := false;
  AvatarComboBox.Enabled := false;
  AvatarNumEdit.Enabled := true;
end;

end.


















