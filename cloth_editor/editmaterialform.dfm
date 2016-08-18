object Form3: TForm3
  Left = 418
  Top = 185
  Width = 382
  Height = 685
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Form3'
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
  object Label1: TLabel
    Left = 24
    Top = 11
    Width = 50
    Height = 13
    Caption = #1052#1072#1090#1077#1088#1080#1072#1083
  end
  object Label2: TLabel
    Left = 24
    Top = 35
    Width = 42
    Height = 13
    Caption = #1071#1088#1082#1086#1089#1090#1100
  end
  object Label4: TLabel
    Left = 24
    Top = 152
    Width = 106
    Height = 13
    Caption = #1056#1072#1079#1084#1077#1088' '#1087#1103#1090#1085#1072' '#1073#1083#1077#1089#1082#1072
  end
  object Label3: TLabel
    Left = 24
    Top = 504
    Width = 82
    Height = 13
    Caption = #1050#1072#1088#1090#1072' '#1085#1086#1088#1084#1072#1083#1077#1081
  end
  object Label5: TLabel
    Left = 24
    Top = 563
    Width = 69
    Height = 13
    Caption = #1050#1072#1088#1090#1072' '#1073#1083#1077#1089#1082#1072
  end
  object Label6: TLabel
    Left = 24
    Top = 96
    Width = 81
    Height = 13
    Caption = #1059#1088#1086#1074#1077#1085#1100' '#1073#1083#1077#1089#1082#1072
  end
  object Label7: TLabel
    Left = 306
    Top = 35
    Width = 3
    Height = 13
  end
  object Label8: TLabel
    Left = 304
    Top = 96
    Width = 3
    Height = 13
  end
  object Label9: TLabel
    Left = 310
    Top = 152
    Width = 31
    Height = 13
    Caption = 'Label9'
  end
  object Label10: TLabel
    Left = 310
    Top = 216
    Width = 31
    Height = 13
    Caption = 'Label9'
  end
  object Shape1: TShape
    Left = 24
    Top = 464
    Width = 57
    Height = 33
    OnMouseDown = Shape1MouseDown
  end
  object Label11: TLabel
    Left = 88
    Top = 464
    Width = 110
    Height = 13
    Caption = #1055#1088#1080#1084#1077#1096#1080#1074#1072#1077#1084#1099#1081' '#1094#1074#1077#1090
  end
  object Label12: TLabel
    Left = 24
    Top = 432
    Width = 114
    Height = 13
    Caption = #1060#1080#1083#1100#1090#1088#1072#1094#1080#1103' '#1090#1077#1082#1089#1090#1091#1088#1099
  end
  object MaterialTypeComboBox: TComboBox
    Left = 76
    Top = 8
    Width = 237
    Height = 19
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -9
    Font.Name = 'Tahoma'
    Font.Style = []
    ItemHeight = 11
    ParentFont = False
    TabOrder = 0
    Text = #1053#1077#1090' '#1084#1072#1090#1077#1088#1080#1072#1083#1072
    OnChange = MaterialTypeComboBoxChange
    Items.Strings = (
      #1053#1077#1090' '#1084#1072#1090#1077#1088#1080#1072#1083#1072
      #1054#1073#1099#1095#1085#1086#1077' '#1086#1089#1074#1077#1097#1077#1085#1080#1077
      #1054#1089#1074#1077#1097#1077#1085#1080#1077' '#1089' '#1073#1072#1084#1087#1086#1084
      #1054#1089#1074#1077#1097#1077#1085#1080#1077' '#1089' '#1073#1072#1084#1087#1086#1084' '#1080' '#1082#1072#1088#1090#1086#1081' '#1073#1083#1077#1089#1082#1072
      #1054#1073#1099#1095#1085#1086#1077' '#1086#1089#1074#1077#1097#1077#1085#1080#1077' '#1089' '#1085#1072#1096#1083#1077#1087#1082#1086#1081
      #1054#1089#1074#1077#1097#1077#1085#1080#1077' '#1089' '#1073#1072#1084#1087#1086#1084' '#1089' '#1085#1072#1096#1083#1077#1087#1082#1086#1081
      #1054#1073#1099#1095#1085#1086#1077' '#1086#1089#1074#1077#1097#1077#1085#1080#1077' '#1089' '#1072#1083#1100#1092#1072'-'#1084#1072#1089#1082#1086#1081' '#1085#1072' '#1094#1074#1077#1090
      #1054#1089#1074#1077#1097#1077#1085#1080#1077' '#1089' '#1073#1072#1084#1087#1086#1084' '#1089' '#1072#1083#1100#1092#1072'-'#1084#1072#1089#1082#1086#1081' '#1085#1072' '#1094#1074#1077#1090)
  end
  object TrackBar1: TTrackBar
    Left = 24
    Top = 54
    Width = 321
    Height = 27
    Max = 200
    Position = 100
    TabOrder = 1
    OnChange = TrackBar1Change
  end
  object TrackBar2: TTrackBar
    Left = 16
    Top = 235
    Width = 321
    Height = 25
    Max = 255
    TabOrder = 2
    OnChange = TrackBar2Change
  end
  object TrackBar3: TTrackBar
    Left = 20
    Top = 171
    Width = 321
    Height = 25
    Max = 100
    Min = 10
    Position = 10
    TabOrder = 3
    OnChange = TrackBar3Change
  end
  object CheckBox1: TCheckBox
    Left = 24
    Top = 218
    Width = 73
    Height = 17
    Caption = #1040#1083#1100#1092#1072#1090#1077#1089#1090
    TabOrder = 4
    OnClick = CheckBox1Click
  end
  object CheckBox2: TCheckBox
    Left = 24
    Top = 280
    Width = 161
    Height = 17
    Caption = #1044#1074#1091#1093#1089#1090#1086#1088#1086#1085#1085#1080#1081' '#1084#1072#1090#1077#1088#1080#1072#1083
    TabOrder = 5
    OnClick = CheckBox2Click
  end
  object Edit1: TEdit
    Left = 24
    Top = 520
    Width = 233
    Height = 21
    TabOrder = 6
  end
  object Button1: TButton
    Left = 264
    Top = 518
    Width = 81
    Height = 25
    Caption = #1054#1073#1079#1086#1088
    TabOrder = 7
    OnClick = Button1Click
  end
  object Edit2: TEdit
    Left = 24
    Top = 579
    Width = 233
    Height = 21
    TabOrder = 8
  end
  object Button2: TButton
    Left = 263
    Top = 575
    Width = 81
    Height = 25
    Caption = #1054#1073#1079#1086#1088
    TabOrder = 9
    OnClick = Button2Click
  end
  object TrackBar4: TTrackBar
    Left = 20
    Top = 115
    Width = 321
    Height = 25
    Max = 100
    Position = 10
    TabOrder = 10
    OnChange = TrackBar4Change
  end
  object Button3: TButton
    Left = 104
    Top = 613
    Width = 153
    Height = 25
    Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100
    TabOrder = 11
    OnClick = Button3Click
  end
  object CheckBox3: TCheckBox
    Left = 24
    Top = 312
    Width = 232
    Height = 17
    Caption = #1042#1082#1083#1102#1095#1080#1090#1100' '#1087#1088#1086#1079#1088#1072#1095#1085#1086#1089#1090#1100' '#1087#1088#1080#1085#1091#1076#1080#1090#1077#1083#1100#1085#1086
    TabOrder = 12
    OnClick = CheckBox3Click
  end
  object CheckBox4: TCheckBox
    Left = 24
    Top = 335
    Width = 232
    Height = 17
    Caption = #1042#1099#1082#1083#1102#1095#1080#1090#1100' '#1087#1088#1086#1079#1088#1072#1095#1085#1086#1089#1090#1100' '#1087#1088#1080#1085#1091#1076#1080#1090#1077#1083#1100#1085#1086
    TabOrder = 13
    OnClick = CheckBox4Click
  end
  object CheckBox5: TCheckBox
    Left = 24
    Top = 407
    Width = 232
    Height = 17
    Caption = #1054#1090#1082#1083#1102#1095#1080#1090#1100' '#1085#1072#1083#1086#1078#1077#1085#1080#1077' '#1090#1077#1085#1077#1081' '#1085#1072' '#1086#1076#1077#1078#1076#1091
    TabOrder = 14
    OnClick = CheckBox5Click
  end
  object Button4: TButton
    Left = 315
    Top = 8
    Width = 25
    Height = 25
    Caption = '?'
    TabOrder = 15
    OnClick = Button4Click
  end
  object TextureFiltrationComboBox: TComboBox
    Left = 143
    Top = 430
    Width = 186
    Height = 21
    ItemHeight = 13
    TabOrder = 16
    Text = #1083#1080#1085#1077#1081#1085#1072#1103
    Items.Strings = (
      #1083#1080#1085#1077#1081#1085#1072#1103
      #1087#1086#1087#1080#1082#1089#1077#1083#1100#1085#1072#1103
      #1089' '#1084#1080#1087#1084#1072#1087#1072#1084#1080)
  end
  object TreatAsAlphaTestCheckBox: TCheckBox
    Left = 24
    Top = 358
    Width = 265
    Height = 17
    Caption = #1045#1089#1083#1080' '#1085#1077' '#1088#1072#1073#1086#1090#1072#1077#1090' '#1087#1088#1086#1079#1088#1072#1095#1085#1086#1089#1090#1100', '#1046#1052#1048' '#1057#1070#1044#1040'!!!!'
    TabOrder = 17
  end
  object OpenDialog1: TOpenDialog
    Left = 264
    Top = 480
  end
  object ColorDialog1: TColorDialog
    Left = 304
    Top = 456
  end
end
