object Form2: TForm2
  Left = 558
  Top = 264
  Width = 522
  Height = 236
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = #1052#1072#1090#1077#1088#1080#1072#1083#1099
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnCanResize = FormCanResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ListBox1: TListBox
    Left = 8
    Top = 8
    Width = 489
    Height = 97
    ItemHeight = 13
    TabOrder = 0
  end
  object Button1: TButton
    Left = 152
    Top = 120
    Width = 201
    Height = 25
    Caption = #1056#1077#1076#1072#1082#1090#1080#1088#1086#1074#1072#1090#1100' '#1084#1072#1090#1077#1088#1080#1072#1083
    TabOrder = 1
    OnClick = Button1Click
  end
  object setMtrlPathBtn: TButton
    Left = 8
    Top = 160
    Width = 185
    Height = 25
    Caption = #1047#1072#1076#1072#1090#1100' '#1087#1091#1090#1100' '#1076#1083#1103' '#1084#1072#1090#1077#1088#1080#1072#1083#1072
    TabOrder = 2
    OnClick = setMtrlPathBtnClick
  end
  object clearPathBtn: TButton
    Left = 312
    Top = 160
    Width = 185
    Height = 25
    Caption = #1054#1095#1080#1089#1090#1080#1090#1100' '#1087#1091#1090#1100
    TabOrder = 3
    OnClick = clearPathBtnClick
  end
  object OpenDialog1: TOpenDialog
    Left = 368
    Top = 160
  end
end
