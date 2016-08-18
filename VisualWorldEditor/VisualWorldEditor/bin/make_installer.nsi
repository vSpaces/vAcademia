# Visual World Editor Installer
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Russian.nlf"
Name "Визуального редактора мира"
OutFile "install.exe"
InstallDir "$PROGRAMFILES\VAcademy Editor"
InstallDirRegKey HKLM "Software\VAcademy Editor" "Install_Dir"
RequestExecutionLevel admin
Icon "..\icons\vaedit.ico"
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "Основные файлы"
	SectionIn RO

	CreateDirectory "$INSTDIR\mtemplates"
	SetOutPath "$INSTDIR\mtemplates"
	File "mtemplates\*.*"

	SetOutPath $INSTDIR
	File "VAcademy Editor.exe"
	File "textures_viewer.exe"
	File "qtintf70.dll"
	File "CustomControls.dll"
	File "Jayrock.Json.dll"
	File "MySql.Data.dll"
	File "Nini.dll"
	File "ServerCommon.dll"
	File "ControlServers_work.exe"
	File "settings.ini"
	File "controlservers_work.exe"
	File "ServerCommon.dll"
	
	WriteRegStr HKLM "Software\VAcademy Editor" "Install_Dir" "$INSTDIR"
	
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VAcademy Editor" "DisplayName" "VAcademy Editor"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VAcademy Editor" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VAcademy Editor" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VAcademy Editor" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
SectionEnd

Section "Добавить в Пуск"

  CreateDirectory "$SMPROGRAMS\VAcademy Editor"
  CreateShortCut "$SMPROGRAMS\VAcademy Editor\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\VAcademy Editor\VAcademy Editor.lnk" "$INSTDIR\VAcademy Editor.exe" "" "$INSTDIR\VAcademy Editor.exe" 0
  
SectionEnd

Section "Ярлык на рабочий стол"

	CreateShortCut "$DESKTOP\VAcademy Editor.lnk" "$INSTDIR\VAcademy Editor.exe" "" "$INSTDIR\VAcademy Editor.exe" 0

SectionEnd

Section "Uninstall"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VAcademy Editor"
  DeleteRegKey HKLM "SOFTWARE\VAcademy Editor"

  Delete "$INSTDIR\mtemplates\*.*"
  Delete "$INSTDIR\VAcademy Editor.exe"
  Delete "$INSTDIR\uninstall.exe"

  Delete "$SMPROGRAMS\VAcademy Editor\*.*"

  Delete "$DESKTOP\VAcademy Editor.lnk"
  
  RMDir "$SMPROGRAMS\VAcademy Editor"
  RMDir "$INSTDIR\mtemplates"
  RMDir "$INSTDIR"

SectionEnd
