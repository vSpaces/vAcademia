unit advice;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, jpeg, ExtCtrls;

type
  TForm5 = class(TForm)
    Image1: TImage;
    Label1: TLabel;
    CheckBox1: TCheckBox;
    CheckBox2: TCheckBox;
    CheckBox3: TCheckBox;
    Button1: TButton;
    CheckBox4: TCheckBox;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form5: TForm5;

implementation

uses editmaterialform;

{$R *.dfm}

procedure TForm5.Button1Click(Sender: TObject);
begin
  If (CheckBox1.Checked = true) And (CheckBox2.Checked = true)
    And (CheckBox3.Checked = true) And (CheckBox4.Checked = false) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 5;
      Form5.Close;
      Exit;
    end;

  If (CheckBox1.Checked = false) And (CheckBox2.Checked = true)
    And (CheckBox3.Checked = true) And (CheckBox4.Checked = false) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 4;
      Form5.Close;
      Exit;
    end;

  If (CheckBox1.Checked = true) And (CheckBox2.Checked = false)
    And (CheckBox3.Checked = true) And (CheckBox4.Checked = false) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 7;
      Form5.Close;
      Exit;
    end;

  If (CheckBox1.Checked = false) And (CheckBox2.Checked = false)
    And (CheckBox3.Checked = true) And (CheckBox4.Checked = false) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 6;
      Form5.Close;
      Exit;
    end;

  If (CheckBox1.Checked = true) And (CheckBox2.Checked = true)
    And (CheckBox3.Checked = false) And (CheckBox4.Checked = false) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 2;
      Form5.Close;
      Exit;
    end;

  If (CheckBox1.Checked = false) And (CheckBox2.Checked = true)
    And (CheckBox3.Checked = false) And (CheckBox4.Checked = false) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 1;
      Form5.Close;
      Exit;
    end;

  If (CheckBox1.Checked = true) And (CheckBox2.Checked = false)
    And (CheckBox3.Checked = false) And (CheckBox4.Checked = false) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 2;
      Form5.Close;
      Exit;
    end;

  If (CheckBox1.Checked = false) And (CheckBox2.Checked = false)
    And (CheckBox3.Checked = false) And (CheckBox4.Checked = false) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 1;
      Form5.Close;
      Exit;
    end;

  If (CheckBox1.Checked = true)
    And (CheckBox3.Checked = false) And (CheckBox4.Checked = true) Then
    begin
      Form3.MaterialTypeComboBox.ItemIndex := 3;
      Form5.Close;
      Exit;
    end;

  ShowMessage('Да, пожалуй без живого Напа не разберешься!');  
end;

end.
