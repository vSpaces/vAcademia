unit materialform;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, ShlObj,Graphics, Controls, Forms,
  Dialogs, StdCtrls, MainForm;

type
  TForm2 = class(TForm)
    ListBox1: TListBox;
    Button1: TButton;
    setMtrlPathBtn: TButton;
    OpenDialog1: TOpenDialog;
    clearPathBtn: TButton;
    procedure FormShow(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure FormCanResize(Sender: TObject; var NewWidth, NewHeight: Integer;
      var Resize: Boolean);
    procedure setMtrlPathBtnClick(Sender: TObject);
    procedure clearPathBtnClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form2: TForm2;
  texturesPath: Array [0..100] Of String;

Function GetTexture(num: Longint): String;

implementation

uses editmaterialform;

{$R *.dfm}

Function GetTexture(num: Longint): String;
Var
  f: File;
  p: Char;
  i: Integer;
begin
  result := '';

  AssignFile(f, programDir + '\mtrl' + IntToStr(num) + '.crf');
  {$I-}
  Reset(f, 1);
  {$I+}
  If IOResult <> 0 Then
    begin
      GetTexture := textures[num];
      Exit;
    end;
  {$I-}
  Seek(f, 36);
  {$I+}
  If IOResult <> 0 Then
    begin
      GetTexture := textures[num];
      Exit;
    end;
  {$I-}
  BlockRead(f, p, 1);
  {$I+}
  If IOResult <> 0 Then
    begin
      GetTexture := textures[num];
      Exit;
    end;
  While p <> #0 Do
    begin
      result := result + p;
      {$I-}
      BlockRead(f, p, 1);
      {$I+}
      If IOResult <> 0 Then
        begin
          GetTexture := textures[num];
          Exit;
        end;
    end;
  CloseFile(f);
end;

procedure TForm2.Button1Click(Sender: TObject);
Var
  i: Longint;
begin
  For i := 1 To matCount Do
  If Form2.ListBox1.Selected[i - 1] Then
    begin
      Form3.Tag := i;
      Form3.Caption := 'Редактирование материала ' + GetTexture(i);
      Form3.ShowModal;
    end;
end;

procedure TForm2.FormCanResize(Sender: TObject; var NewWidth,
  NewHeight: Integer; var Resize: Boolean);
begin
  resize := false;
end;

procedure TForm2.FormShow(Sender: TObject);
Var
  i: Longint;
  path : String;
begin
  Form2.ListBox1.Clear;
  For i := 1 To matCount Do
    begin
        Form2.ListBox1.AddItem(GetTexture(i)+' ('+ texturesPath[i-1] +')', nil);
    end;
  Form2.ListBox1.ItemIndex := 0;
end;

procedure TForm2.setMtrlPathBtnClick(Sender: TObject);
var
  TitleName : string;
  lpItemID : PItemIDList;
  BrowseInfo : TBrowseInfo;
  DisplayName : array[0..MAX_PATH] of char;
  TempPath : array[0..MAX_PATH] of char;

  itemPos, p, tempPos: Longint;
  itemStr, modelPath, resultRelativePath, newPath, path: String;
  i, count, size :Integer;
begin
  FillChar(BrowseInfo, sizeof(TBrowseInfo), #0);
  BrowseInfo.hwndOwner := Form2.Handle;
  BrowseInfo.pszDisplayName := @DisplayName;
  TitleName := 'Укажите папку';
  BrowseInfo.lpszTitle := PChar(TitleName);
  BrowseInfo.ulFlags := BIF_RETURNONLYFSDIRS;
  lpItemID := SHBrowseForFolder(BrowseInfo);
  if lpItemId <> nil then begin
    SHGetPathFromIDList(lpItemID, TempPath);

  itemPos := ListBox1.ItemIndex;
  itemStr := ListBox1.Items[itemPos];

  p := pos('(',itemStr);
  delete (itemStr, p, length(itemStr));

  i :=0;

  while(TempPath[i]<> #0) Do
  begin
    newPath := newPath + TempPath[i];
    i := i+1;
  end;

  if (newPath[length(newPath)] <> '\')  then
    newPath := newPath + '\';

  if not FileExists(newPath +itemStr) then
    begin
      MessageDlg('В папке нет такой текстуры', mtError, [mbOk], 0);
      Exit;
    end;

  count :=0;

  modelPath:= Form1.GetModelPath;
  size :=  length(newPath);
  //  считаем сколько символов совпадает в путях
  i:=1;
  while((modelPath[i] <> #0) AND (newPath[i] <> #0)) do
  begin
    if (modelPath[i] = newPath[i]) then
    begin
        count := count+1;
    end;
    i:=i+1;
  end;

  delete(modelPath,1,count);

  // берем папку до которой совпадает путь
  for  i :=0  To count-1 Do
  begin
      tempPos := pos('\',newPath);
      if ( (tempPos <>0) AND (tempPos <= count) ) then
      begin
        p:= pos('\',newPath);
        delete(newPath,p,1);
        count := count-1;
      end;
  end;

  delete(newPath,1,p-1);

  count := length(modelPath)-1;

  // сколько папок настолько надо и подняться вверх
  for  i :=0  To count Do
  begin
   p := pos ('\',modelPath);

   if (p<>0) then
    begin
        resultRelativePath := resultRelativePath + '..\';
        delete(modelPath,p,1);
        count := count-1;
    end;
  end;

  texturesPath[itemPos] := resultRelativePath + newPath;

  ListBox1.Items[itemPos] := itemStr + '(' + texturesPath[itemPos] + ')';
  GlobalFreePtr(lpItemID);
  end;
end;

procedure TForm2.clearPathBtnClick(Sender: TObject);
var
     itemPos: Longint;
begin
      itemPos := ListBox1.ItemIndex;
      ListBox1.Items[itemPos] := GetTexture(itemPos+1)+ '()';
end;

end.
