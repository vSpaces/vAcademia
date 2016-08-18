; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAnimationTimePage
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "cal3d_max_exporter.h"
LastPage=0

ClassCount=16
Class1=CMaxExporterApp
Class2=CAnimationExportSheet
Class3=CAnimationTimePage
Class4=CBoneAssignmentPage
Class5=CLevelOfDetailPage
Class6=CMaterialExportSheet
Class7=CMaterialMapsPage
Class8=CMaterialSelectionPage
Class9=CMeshExportSheet
Class10=CSkeletonExportSheet
Class11=CSkeletonFilePage
Class12=CSkeletonHierarchyPage
Class13=CSpringSystemPage

ResourceCount=23
Resource1=IDD_MATERIAL_MAPS_PAGE
Resource2=IDD_ANIMATION_TIME_PAGE
Resource3=IDD_SKELETON_HIERARCHY_PAGE
Resource4=IDD_LOD_PAGE
Resource5=IDD_ABOUT
Resource6=IDD_BONE_ASSIGNMENT_PAGE
Resource7=IDD_SKELETON_FILE_PAGE
Resource8=IDD_MATERIAL_SELECTION_PAGE
Resource9=IDD_SPRINGSYSTEM_PAGE
Resource10=IDD_LOD_PAGE (German (Switzerland))
Resource11=IDD_MATERIAL_SELECTION_PAGE (German (Switzerland))
Resource12=IDD_FLARES (English (U.S.))
Resource13=IDD_MATERIALPROPSDLG (Russian)
Resource14=IDD_SKELETON_FILE_PAGE (German (Switzerland))
Resource15=IDD_SKELETON_HIERARCHY_PAGE (German (Switzerland))
Resource16=IDD_MATERIALPROPSDLG
Resource17=IDD_BONE_ASSIGNMENT_PAGE (German (Switzerland))
Resource18=IDD_MATERIAL_MAPS_PAGE (German (Switzerland))
Resource19=IDD_ABOUT (German (Switzerland))
Class14=CMtrlPropsPage
Resource20=IDD_ANIMATION_TIME_PAGE (German (Switzerland))
Resource21=IDD_DIALOGBAR (English (U.S.))
Class15=CSceneLightsDlg
Resource22=IDD_LIGHTS
Class16=CSceneFlaresDlg
Resource23=IDD_SPRINGSYSTEM_PAGE (German (Switzerland))

[CLS:CMaxExporterApp]
Type=0
BaseClass=CWinApp
HeaderFile=cal3d_max_exporter.h
ImplementationFile=cal3d_max_exporter.cpp

[CLS:CAnimationExportSheet]
Type=0
HeaderFile=\USER\Tandy\NaturaTools\cal3d_exporter\src\win32\animationexportsheet.h
ImplementationFile=\USER\Tandy\NaturaTools\cal3d_exporter\src\win32\animationexportsheet.cpp
BaseClass=CPropertySheet
LastObject=CAnimationExportSheet

[CLS:CBoneAssignmentPage]
Type=0
HeaderFile=\user\alex\programming\cal3d_exporter\src\win32\boneassignmentpage.h
ImplementationFile=\user\alex\programming\cal3d_exporter\src\win32\boneassignmentpage.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=IDC_SAVECONFIG

[CLS:CLevelOfDetailPage]
Type=0
HeaderFile=\User\Alex\Programming\cal3d_exporter\src\win32\levelofdetailpage.h
ImplementationFile=\user\alex\programming\cal3d_exporter\src\win32\levelofdetailpage.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC

[CLS:CMaterialExportSheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=\Projects\cal3d_exporter\src\win32\MaterialExportSheet.h
ImplementationFile=\Projects\cal3d_exporter\src\win32\MaterialExportSheet.cpp

[CLS:CMaterialMapsPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=\Projects\cal3d_exporter\src\win32\MaterialMapsPage.h
ImplementationFile=\Projects\cal3d_exporter\src\win32\MaterialMapsPage.cpp

[CLS:CMaterialSelectionPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=\Projects\cal3d_exporter\src\win32\MaterialSelectionPage.h
ImplementationFile=\Projects\cal3d_exporter\src\win32\MaterialSelectionPage.cpp

[CLS:CMeshExportSheet]
Type=0
HeaderFile=\User\Alex\programming\Cal3D\cal3d_exporter\src\win32\meshexportsheet.h
ImplementationFile=\User\Alex\programming\Cal3D\cal3d_exporter\src\win32\meshexportsheet.cpp
BaseClass=CPropertySheet
LastObject=CMeshExportSheet

[CLS:CSkeletonExportSheet]
Type=0
HeaderFile=\user\alex\programming\cal3d\cal3d_exporter\src\win32\skeletonexportsheet.h
ImplementationFile=\user\alex\programming\cal3d\cal3d_exporter\src\win32\skeletonexportsheet.cpp
BaseClass=CPropertySheet
LastObject=CSkeletonExportSheet

[CLS:CSkeletonFilePage]
Type=0
HeaderFile=\user\alex\programming\terrain\cal3d_exporter\src\win32\skeletonfilepage.h
ImplementationFile=\user\alex\programming\terrain\cal3d_exporter\src\win32\skeletonfilepage.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CSkeletonFilePage

[CLS:CSpringSystemPage]
Type=0
HeaderFile=\user\alex\programming\cal3d\cal3d_exporter\src\win32\springsystempage.h
ImplementationFile=\user\alex\programming\cal3d\cal3d_exporter\src\win32\springsystempage.cpp
BaseClass=CPropertyPage
LastObject=CSpringSystemPage

[DLG:IDD_ANIMATION_TIME_PAGE]
Type=1
Class=CAnimationTimePage
ControlCount=18
Control1=IDC_START_FRAME,edit,1350639744
Control2=IDC_END_FRAME,edit,1350639744
Control3=IDC_DISPLACEMENT,edit,1350639744
Control4=IDC_FPS,edit,1350639744
Control5=ID_APP_ABOUT,button,1342242816
Control6=IDC_STATIC,static,1342177294
Control7=IDC_STATIC,static,1342177296
Control8=IDC_STEP,static,1342308352
Control9=IDC_DESCRIPTION,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,button,1342177287
Control17=IDC_STATIC,button,1342177287
Control18=IDC_STATIC,button,1342177287

[DLG:IDD_BONE_ASSIGNMENT_PAGE]
Type=1
Class=CBoneAssignmentPage
ControlCount=12
Control1=IDC_MAX_BONE_COUNT,edit,1350639744
Control2=IDC_WEIGHT_THRESHOLD,edit,1350631552
Control3=ID_APP_ABOUT,button,1342242816
Control4=IDC_STATIC,static,1342177294
Control5=IDC_STATIC,static,1342177296
Control6=IDC_STEP,static,1342308352
Control7=IDC_DESCRIPTION,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,button,1342177287
Control12=IDC_STATIC,static,1342308352

[DLG:IDD_LOD_PAGE]
Type=1
Class=CLevelOfDetailPage
ControlCount=7
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342177294
Control3=IDC_STATIC,static,1342177296
Control4=IDC_STEP,static,1342308352
Control5=IDC_DESCRIPTION,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_ENABLE_LOD,button,1342244867

[DLG:IDD_MATERIAL_MAPS_PAGE]
Type=1
Class=CMaterialMapsPage
ControlCount=7
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342177294
Control4=IDC_STATIC,static,1342177296
Control5=IDC_STEP,static,1342308352
Control6=IDC_DESCRIPTION,static,1342308352
Control7=IDC_MAPS,SysListView32,1350664197

[DLG:IDD_MATERIAL_SELECTION_PAGE]
Type=1
Class=CMaterialSelectionPage
ControlCount=7
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342177294
Control4=IDC_STATIC,static,1342177296
Control5=IDC_STEP,static,1342308352
Control6=IDC_DESCRIPTION,static,1342308352
Control7=IDC_MATERIAL,listbox,1352728835

[DLG:IDD_SKELETON_FILE_PAGE]
Type=1
Class=CSkeletonFilePage
ControlCount=8
Control1=IDC_LRU,combobox,1344339969
Control2=IDC_BROWSE,button,1342242816
Control3=ID_APP_ABOUT,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342177294
Control6=IDC_STATIC,static,1342177296
Control7=IDC_STEP,static,1342308352
Control8=IDC_DESCRIPTION,static,1342308352

[DLG:IDD_SKELETON_HIERARCHY_PAGE]
Type=1
Class=CSkeletonHierarchyPage
ControlCount=7
Control1=IDC_HIERARCHY,SysTreeView32,1350631703
Control2=ID_APP_ABOUT,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342177294
Control5=IDC_STATIC,static,1342177296
Control6=IDC_STEP,static,1342308352
Control7=IDC_DESCRIPTION,static,1342308352

[DLG:IDD_SPRINGSYSTEM_PAGE]
Type=1
Class=CSpringSystemPage
ControlCount=7
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342177294
Control3=IDC_STATIC,static,1342177296
Control4=IDC_STEP,static,1342308352
Control5=IDC_DESCRIPTION,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_ENABLE_SPRINGSYSTEM,button,1342244867

[DLG:IDD_ABOUT]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342179342
Control3=IDC_STATIC,static,1342308353
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308353
Control6=IDC_URL,button,1476493312

[DLG:IDD_ABOUT (German (Switzerland))]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342179342
Control3=IDC_STATIC,static,1342308353
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308353
Control6=IDC_URL,button,1476493312

[DLG:IDD_ANIMATION_TIME_PAGE (German (Switzerland))]
Type=1
Class=CAnimationTimePage
ControlCount=29
Control1=IDC_START_FRAME,edit,1350639744
Control2=IDC_END_FRAME,edit,1350639744
Control3=IDC_DISPLACEMENT,edit,1350639744
Control4=IDC_FPS,edit,1350639744
Control5=ID_APP_ABOUT,button,1342242816
Control6=IDC_STATIC,static,1342177294
Control7=IDC_STATIC,static,1342177296
Control8=IDC_STEP,static,1342308352
Control9=IDC_DESCRIPTION,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,button,1342177287
Control17=IDC_STATIC,button,1342177287
Control18=IDC_STATIC,button,1342177287
Control19=IDC_RELANIM,button,1342242819
Control20=IDC_SAVEFIRST,button,1342242819
Control21=IDC_SAVELAST,button,1342242819
Control22=IDC_SAVEECF,button,1073807360
Control23=IDC_COMMON,button,1342242819
Control24=IDC_STAY_ATLAST,button,1342242819
Control25=IDC_WALKANIMATION,button,1342242819
Control26=IDC_STATIC,button,1342177287
Control27=IDC_ZPOSITION,edit,1350631552
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352

[DLG:IDD_BONE_ASSIGNMENT_PAGE (German (Switzerland))]
Type=1
Class=CBoneAssignmentPage
ControlCount=17
Control1=IDC_MAX_BONE_COUNT,edit,1350639744
Control2=IDC_WEIGHT_THRESHOLD,edit,1350631552
Control3=ID_APP_ABOUT,button,1342242816
Control4=IDC_STATIC,static,1342177294
Control5=IDC_STATIC,static,1342177296
Control6=IDC_STEP,static,1342308352
Control7=IDC_DESCRIPTION,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,button,1342177287
Control12=IDC_STATIC,static,1342308352
Control13=IDC_ENABLE_FLIP,button,1342244867
Control14=IDC_SAVECONFIG,button,1342244867
Control15=IDC_SHADOWDATA,button,1342244867
Control16=IDC_LOCALCOORDINATES,button,1342244867
Control17=IDC_NORMALIZEWEIGHTS,button,1342244867

[DLG:IDD_LOD_PAGE (German (Switzerland))]
Type=1
Class=CLevelOfDetailPage
ControlCount=7
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342177294
Control3=IDC_STATIC,static,1342177296
Control4=IDC_STEP,static,1342308352
Control5=IDC_DESCRIPTION,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_ENABLE_LOD,button,1342244867

[DLG:IDD_MATERIAL_MAPS_PAGE (German (Switzerland))]
Type=1
Class=CMaterialMapsPage
ControlCount=7
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342177294
Control4=IDC_STATIC,static,1342177296
Control5=IDC_STEP,static,1342308352
Control6=IDC_DESCRIPTION,static,1342308352
Control7=IDC_MAPS,SysListView32,1350664197

[DLG:IDD_MATERIAL_SELECTION_PAGE (German (Switzerland))]
Type=1
Class=CMaterialSelectionPage
ControlCount=7
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342177294
Control4=IDC_STATIC,static,1342177296
Control5=IDC_STEP,static,1342308352
Control6=IDC_DESCRIPTION,static,1342308352
Control7=IDC_MATERIAL,listbox,1352728835

[DLG:IDD_SKELETON_FILE_PAGE (German (Switzerland))]
Type=1
Class=CSkeletonFilePage
ControlCount=17
Control1=IDC_LRU,combobox,1344339970
Control2=IDC_BROWSE,button,1342242816
Control3=ID_APP_ABOUT,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342177294
Control6=IDC_STATIC,static,1342177296
Control7=IDC_STEP,static,1342308352
Control8=IDC_DESCRIPTION,static,1342308352
Control9=IDC_STATIC2,static,1342308352
Control10=IDC_FACEEX,combobox,1344339970
Control11=IDC_BROWSEF,button,1342242816
Control12=IDC_EXPCONFIGFILE,combobox,1344339970
Control13=IDC_BROWSEECF,button,1342242816
Control14=IDC_STATICECF,static,1342308352
Control15=IDC_EXPCONFIGNAME,combobox,1344339970
Control16=IDC_STATICCONF,static,1342308352
Control17=IDC_FDP,button,1342242819

[DLG:IDD_SKELETON_HIERARCHY_PAGE (German (Switzerland))]
Type=1
Class=CSkeletonHierarchyPage
ControlCount=6
Control1=IDC_HIERARCHY,SysTreeView32,1350631703
Control2=ID_APP_ABOUT,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342177296
Control5=IDC_STEP,static,1342308352
Control6=IDC_DESCRIPTION,static,1342308352

[DLG:IDD_SPRINGSYSTEM_PAGE (German (Switzerland))]
Type=1
Class=CSpringSystemPage
ControlCount=7
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342177294
Control3=IDC_STATIC,static,1342177296
Control4=IDC_STEP,static,1342308352
Control5=IDC_DESCRIPTION,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_ENABLE_SPRINGSYSTEM,button,1342244867

[DLG:IDD_DIALOGBAR (English (U.S.))]
Type=1
Class=?
ControlCount=0

[DLG:IDD_MATERIALPROPSDLG]
Type=1
Class=CMtrlPropsPage
ControlCount=13
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342177294
Control3=IDC_STEP,static,1342308352
Control4=IDC_DESCRIPTION,static,1342308352
Control5=IDC_TEXTPATH,edit,1350631552
Control6=IDC_MATERIALS,button,1342242819
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,button,1342177287
Control9=IDC_CHECKAMB,button,1342242819
Control10=IDC_CHECKDIF,button,1342242819
Control11=IDC_BUTTON1,button,1342275595
Control12=IDC_BUTTON2,button,1342275595
Control13=IDC_KEEPCFG,button,1342242819

[CLS:CMtrlPropsPage]
Type=0
HeaderFile=..\src\win32\mtrlpropspage.h
ImplementationFile=..\src\win32\mtrlpropspage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CMtrlPropsPage
VirtualFilter=idWC

[DLG:IDD_MATERIALPROPSDLG (Russian)]
Type=1
ControlCount=12
Control1=ID_APP_ABOUT,button,1342242816
Control2=IDC_STATIC,static,1342177294
Control3=IDC_STEP,static,1342308352
Control4=IDC_DESCRIPTION,static,1342308352
Control5=IDC_TEXTPATH,edit,1350631552
Control6=IDC_MATERIALS,button,1342242819
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,button,1342177287
Control9=IDC_CHECKAMB,button,1342242819
Control10=IDC_CHECKDIF,button,1342242819
Control11=IDC_BUTTON1,button,1342275595
Control12=IDC_BUTTON2,button,1342275595
Class=CMtrlPropsPage

[DLG:IDD_LIGHTS]
Type=1
Class=CSceneLightsDlg
ControlCount=19
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST1,listbox,1352730883
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT1,edit,1350631552
Control8=IDC_BUTTON1,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT2,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT3,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_BUTTON2,button,1342275595
Control16=IDC_BUTTON3,button,1342275595
Control17=IDC_STATIC,static,1342308352
Control18=IDC_EDIT4,edit,1350631552
Control19=IDC_BUTTON4,button,1342242816

[CLS:CSceneLightsDlg]
Type=0
HeaderFile=..\src\scenelightsdlg.h
ImplementationFile=..\src\SceneLightsDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CSceneLightsDlg
VirtualFilter=dWC

[DLG:IDD_FLARES (English (U.S.))]
Type=1
Class=CSceneFlaresDlg
ControlCount=11
Control1=IDSet,button,1342242816
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT1,edit,1484849280
Control6=IDC_BUTTON1,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT2,edit,1484849280
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT3,edit,1484849280
Control11=IDC_LIST1,listbox,1352730883

[CLS:CSceneFlaresDlg]
Type=0
HeaderFile=\user\alex\programming\Natura3D\cal3d_exporter\src\flaresdlg.h
ImplementationFile=\user\alex\programming\Natura3D\cal3d_exporter\src\flaresdlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDSet

[CLS:CAnimationTimePage]
Type=0
HeaderFile=\user\alex\programming\Natura3D\cal3d_exporter\src\win32\animationtimepage.h
ImplementationFile=\user\alex\programming\Natura3D\cal3d_exporter\src\win32\animationtimepage.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CAnimationTimePage

