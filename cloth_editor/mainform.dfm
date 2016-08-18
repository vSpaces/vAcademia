object Form1: TForm1
  Left = 199
  Top = 129
  Width = 416
  Height = 615
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Cloth Editor v. 1.61'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  Scaled = False
  OnCanResize = FormCanResize
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object OptionsGroupBox: TGroupBox
    Left = 13
    Top = 9
    Width = 372
    Height = 400
    Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080
    TabOrder = 0
    object PathToClientLabel: TLabel
      Left = 16
      Top = 21
      Width = 79
      Height = 13
      Caption = #1055#1091#1090#1100' '#1082' '#1082#1083#1080#1077#1085#1090#1091
    end
    object ThisIsLabel: TLabel
      Left = 16
      Top = 72
      Width = 19
      Height = 13
      Caption = #1069#1090#1086
    end
    object ModelNameLabel: TLabel
      Left = 8
      Top = 291
      Width = 49
      Height = 13
      Caption = #1048#1084#1103' '#1074#1077#1097#1080
    end
    object ForLabel: TLabel
      Left = 252
      Top = 290
      Width = 19
      Height = 13
      Caption = #1076#1083#1103
    end
    object BlendForLabel: TLabel
      Left = 223
      Top = 117
      Width = 19
      Height = 13
      Caption = #1076#1083#1103
    end
    object HeadLabel: TLabel
      Left = 295
      Top = 116
      Width = 37
      Height = 13
      Caption = #1075#1086#1083#1086#1074#1099
    end
    object FromLabel: TLabel
      Left = 264
      Top = 316
      Width = 11
      Height = 13
      Caption = #1080#1079
    end
    object Label1: TLabel
      Left = 88
      Top = 196
      Width = 109
      Height = 13
      Caption = #1053#1072#1079#1074#1072#1085#1080#1077' '#1087#1072#1088#1072#1084#1077#1090#1088#1072':'
    end
    object Label2: TLabel
      Left = 88
      Top = 228
      Width = 177
      Height = 13
      Caption = #1053#1072#1079#1074#1072#1085#1080#1077' '#1087#1072#1088#1072#1084#1077#1090#1088#1072' '#1084#1080#1085#1080#1084#1080#1079#1072#1094#1080#1080':'
    end
    object Label3: TLabel
      Left = 88
      Top = 260
      Width = 182
      Height = 13
      Caption = #1053#1072#1079#1074#1072#1085#1080#1077' '#1087#1072#1088#1072#1084#1077#1090#1088#1072' '#1084#1072#1082#1089#1080#1084#1080#1079#1072#1094#1080#1080':'
    end
    object ClientPathEdit: TEdit
      Left = 16
      Top = 40
      Width = 257
      Height = 21
      TabOrder = 0
      Text = 'C:\Program Files\Virtual Academia'
      OnChange = ClientPathEditChange
      OnKeyDown = FormKeyDown
    end
    object ChooseClientPath: TButton
      Left = 280
      Top = 32
      Width = 81
      Height = 29
      Caption = #1086#1073#1079#1086#1088
      TabOrder = 1
      OnClick = ChooseClientPathClick
      OnKeyDown = FormKeyDown
    end
    object ClothesRadioButton: TRadioButton
      Left = 72
      Top = 67
      Width = 84
      Height = 25
      Caption = #1086#1076#1077#1078#1076#1072' '#1090#1080#1087#1072
      TabOrder = 2
      OnClick = BlendRadioButtonClick
      OnKeyDown = FormKeyDown
    end
    object HairRadioButton: TRadioButton
      Left = 72
      Top = 89
      Width = 81
      Height = 25
      Caption = #1087#1088#1080#1095#1077#1089#1082#1072
      TabOrder = 3
      OnClick = BlendRadioButtonClick
      OnKeyDown = FormKeyDown
    end
    object ModelName: TEdit
      Left = 72
      Top = 289
      Width = 174
      Height = 21
      TabOrder = 4
      OnKeyDown = FormKeyDown
    end
    object AvatarComboBox: TComboBox
      Left = 277
      Top = 288
      Width = 81
      Height = 21
      ItemHeight = 13
      TabOrder = 5
      Text = 'women8'
      OnChange = AvatarComboBoxChange
      OnKeyDown = FormKeyDown
      Items.Strings = (
        'man'
        'man2'
        'women1'
        'women2'
        'women8'
        'man9'
        'man11'
        'women12')
    end
    object BlendRadioButton: TRadioButton
      Left = 72
      Top = 112
      Width = 81
      Height = 25
      Caption = #1073#1083#1077#1085#1076
      TabOrder = 6
      OnClick = BlendRadioButtonClick
      OnKeyDown = FormKeyDown
    end
    object BlendTypeComboBox: TComboBox
      Left = 128
      Top = 113
      Width = 89
      Height = 21
      Enabled = False
      ItemHeight = 13
      TabOrder = 7
      Text = #1075#1083#1072#1079#1072' '#1074#1083#1077#1074#1086
      OnKeyDown = FormKeyDown
      Items.Strings = (
        #1075#1083#1072#1079#1072' '#1074#1083#1077#1074#1086
        #1075#1083#1072#1079#1072' '#1074#1087#1088#1072#1074#1086
        #1079#1072#1082#1088#1099#1090#1099#1077' '#1075#1083#1072#1079#1072
        #1091#1083#1099#1073#1082#1072' '#1087#1086#1083#1085#1072#1103
        #1091#1083#1099#1073#1082#1072' '#1089#1082#1088#1086#1084#1085#1072#1103
        #1092#1086#1085#1077#1084#1072' 1'
        #1092#1086#1085#1077#1084#1072' 2'
        #1092#1086#1085#1077#1084#1072' 3'
        #1092#1086#1085#1077#1084#1072' 4'
        #1092#1086#1085#1077#1084#1072' 5'
        #1092#1086#1088#1084#1072' '#1085#1086#1089#1072
        #1076#1083#1080#1085#1072' '#1085#1086#1089#1072
        #1091#1096#1080
        #1083#1086#1073
        #1097#1077#1082#1080
        'AU0'
        'AU1'
        'AU2'
        'AU3'
        'AU4'
        'AU5')
    end
    object HeadIDComboBox: TComboBox
      Left = 248
      Top = 113
      Width = 41
      Height = 21
      Enabled = False
      ItemHeight = 13
      TabOrder = 8
      Text = '1'
      OnKeyDown = FormKeyDown
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9')
    end
    object ClothesType: TComboBox
      Left = 162
      Top = 70
      Width = 97
      Height = 21
      ItemHeight = 13
      TabOrder = 9
      Text = #1057
      OnChange = ClothesTypeChange
      OnKeyDown = FormKeyDown
    end
    object UseTexturesFromCheckBox: TCheckBox
      Left = 16
      Top = 316
      Width = 161
      Height = 17
      Caption = #1080#1089#1087#1086#1083#1100#1079#1086#1074#1072#1090#1100' '#1090#1077#1082#1089#1090#1091#1088#1099' '#1086#1090
      TabOrder = 10
      OnClick = UseTexturesFromCheckBoxClick
    end
    object AvatarForTexturesComboBox: TComboBox
      Left = 176
      Top = 313
      Width = 81
      Height = 21
      ItemHeight = 13
      TabOrder = 11
      Text = 'women8'
      OnChange = AvatarForTexturesComboBoxChange
      OnKeyDown = FormKeyDown
      Items.Strings = (
        'man11'
        'man2'
        'women1'
        'women2'
        'women8'
        'man9')
    end
    object ModelForTexturesNameComboBox: TComboBox
      Left = 280
      Top = 313
      Width = 78
      Height = 21
      ItemHeight = 13
      TabOrder = 12
      Text = 'ModelForTexturesNameComboBox'
    end
    object UseServerTexturesCheckBox: TCheckBox
      Left = 16
      Top = 334
      Width = 185
      Height = 25
      Caption = #1076#1077#1088#1078#1072#1090#1100' '#1090#1077#1082#1089#1090#1091#1088#1099' '#1085#1072' '#1089#1077#1088#1074#1077#1088#1077
      TabOrder = 13
      OnClick = UseServerTexturesCheckBoxClick
    end
    object BlendForClothesOrHairdressRadioButton: TRadioButton
      Left = 72
      Top = 139
      Width = 113
      Height = 17
      Caption = #1073#1083#1077#1085#1076' '#1076#1083#1103
      TabOrder = 14
      OnClick = BlendForClothesOrHairdressRadioButtonClick
    end
    object ClothesAndHairdressesComboBox: TComboBox
      Left = 168
      Top = 136
      Width = 145
      Height = 21
      Enabled = False
      ItemHeight = 13
      TabOrder = 15
    end
    object BlendParamNameEdit: TEdit
      Left = 208
      Top = 196
      Width = 145
      Height = 21
      Enabled = False
      TabOrder = 16
    end
    object BlendParamMinNameEdit: TEdit
      Left = 272
      Top = 228
      Width = 81
      Height = 21
      Enabled = False
      TabOrder = 17
    end
    object BlendParamMaxNameEdit: TEdit
      Left = 272
      Top = 260
      Width = 81
      Height = 21
      Enabled = False
      TabOrder = 18
    end
    object DeleteBlendsCheckBox: TCheckBox
      Left = 16
      Top = 361
      Width = 201
      Height = 14
      Caption = #1091#1076#1072#1083#1103#1090#1100' '#1073#1083#1077#1085#1076#1099' '#1087#1088#1080' '#1074#1089#1090#1072#1074#1082#1077
      TabOrder = 19
    end
    object AvatarNumEdit: TEdit
      Left = 223
      Top = 160
      Width = 81
      Height = 21
      Enabled = False
      TabOrder = 20
    end
  end
  object ActionsGroupBox: TGroupBox
    Left = 13
    Top = 418
    Width = 371
    Height = 152
    Caption = #1044#1077#1081#1089#1090#1074#1080#1103
    TabOrder = 1
    object CopyMeshButton: TButton
      Left = 16
      Top = 24
      Width = 337
      Height = 25
      Caption = #1050#1086#1087#1080#1088#1086#1074#1072#1090#1100' '#1084#1077#1096' '#1080#1079' '#1090#1077#1082#1091#1097#1077#1081' '#1087#1072#1087#1082#1080
      TabOrder = 0
      OnClick = CopyMeshButtonClick
      OnKeyDown = FormKeyDown
    end
    object ConfigureMaterialsButton: TButton
      Left = 16
      Top = 55
      Width = 337
      Height = 25
      Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1072' '#1084#1072#1090#1077#1088#1080#1072#1083#1086#1074
      TabOrder = 1
      OnClick = ConfigureMaterialsButtonClick
      OnKeyDown = FormKeyDown
    end
    object ConfigureClipboxesButton: TButton
      Left = 16
      Top = 86
      Width = 337
      Height = 25
      Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1072' '#1073#1086#1082#1089#1086#1074
      TabOrder = 2
      OnClick = ConfigureClipboxesButtonClick
      OnKeyDown = FormKeyDown
    end
    object RunClientButton: TButton
      Left = 16
      Top = 117
      Width = 337
      Height = 25
      Caption = #1047#1072#1087#1091#1089#1090#1080#1090#1100' '#1082#1083#1080#1077#1085#1090
      TabOrder = 3
      OnClick = RunClientButtonClick
      OnKeyDown = FormKeyDown
    end
  end
  object AvatarRadioButton: TRadioButton
    Left = 85
    Top = 171
    Width = 148
    Height = 17
    Caption = #1085#1086#1074#1099#1081' '#1072#1074#1072#1090#1072#1088' '#1089' '#1085#1086#1084#1077#1088#1086#1084
    TabOrder = 2
    OnClick = AvatarRadioButtonClick
  end
  object OpenDialog1: TOpenDialog
    Left = 328
    Top = 16
  end
  object XMLDocument1: TXMLDocument
    Left = 288
    Top = 16
    DOMVendorDesc = 'MSXML'
  end
end
