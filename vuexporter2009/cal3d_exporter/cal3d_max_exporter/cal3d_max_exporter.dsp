# Microsoft Developer Studio Project File - Name="cal3d_max_exporter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cal3d_max_exporter - Win32 Debug_Max5
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cal3d_max_exporter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cal3d_max_exporter.mak" CFG="cal3d_max_exporter - Win32 Debug_Max5"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cal3d_max_exporter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cal3d_max_exporter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cal3d_max_exporter - Win32 Release_Max5" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cal3d_max_exporter - Win32 Debug_Max5" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cal3d_max_exporter - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\cal3d_max_exporter" /I "..\..\cal3d_dx8\src\\" /I "common\\" /I "..\src" /I "..\src\win32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x807 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x807 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 cal3d_dx8.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib bmm.lib /nologo /subsystem:windows /dll /machine:I386 /out:"c:\123\cal3d_max_exporter_dx8.dle" /libpath:"..\..\cal3d_dx8\release" /libpath:"F:\3dsmax4\maxsdk\lib\\"

!ELSEIF  "$(CFG)" == "cal3d_max_exporter - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\cal3d_max_exporter" /I "..\..\cal3d_dx8\src\\" /I "common\\" /I "..\src" /I "..\src\win32" /I "J:\3dsmax4\maxsdk\include\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x807 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x807 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib cal3d_dx8.lib bmm.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"J:\3dsmax4\plugins\cal3d_max_exporter_dx8.dle" /pdbtype:sept /libpath:"..\..\cal3d_dx8\Debug" /libpath:"J:\3dsmax4\maxsdk\lib\\"

!ELSEIF  "$(CFG)" == "cal3d_max_exporter - Win32 Release_Max5"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cal3d_max_exporter___Win32_Release_Max5"
# PROP BASE Intermediate_Dir "cal3d_max_exporter___Win32_Release_Max5"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "cal3d_max_exporter___Win32_Release_Max5"
# PROP Intermediate_Dir "cal3d_max_exporter___Win32_Release_Max5"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\cal3d_max_exporter" /I "..\..\cal3d_dx8\src\\" /I "common\\" /I "..\src" /I "..\src\win32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\cal3d_max_exporter" /I "..\..\cal3d_dx8\src\\" /I "common\\" /I "..\src" /I "..\src\win32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x807 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x807 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cal3d_dx8.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib bmm.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release\cal3d_max_exporter_dx8.dle" /libpath:"..\..\cal3d_dx8\release" /libpath:"F:\3dsmax4\maxsdk\lib\\"
# ADD LINK32 cal3d_dx8.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib bmm.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release\cal3d_max_exporter_dx8.dle" /libpath:"..\..\cal3d_dx8\release" /libpath:"F:\3dsmax4\maxsdk\lib\\"

!ELSEIF  "$(CFG)" == "cal3d_max_exporter - Win32 Debug_Max5"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cal3d_max_exporter___Win32_Debug_Max5"
# PROP BASE Intermediate_Dir "cal3d_max_exporter___Win32_Debug_Max5"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "cal3d_max_exporter___Win32_Debug_Max5"
# PROP Intermediate_Dir "cal3d_max_exporter___Win32_Debug_Max5"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\cal3d_max_exporter" /I "..\..\cal3d_dx8\src\\" /I "common\\" /I "..\src" /I "..\src\win32" /I "E:\3dsmax5\maxsdk\include\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "E:\3dsmax5\CStudio\SDK\\" /I "..\cal3d_max_exporter" /I "..\..\cal3d_dx8\src\\" /I "common\\" /I "..\src" /I "..\src\win32" /I "E:\3dsmax5\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x807 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x807 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib cal3d_dx8.lib bmm.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"C:\3dsmax4\plugins\cal3d_max_exporter_dx8.dle" /pdbtype:sept /libpath:"..\..\cal3d_dx8\Debug" /libpath:"E:\3dsmax5\maxsdk\lib\\"
# ADD LINK32 core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib cal3d_dx8.lib bmm.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"E:\3dsmax5\plugins\cal3d_max_exporter_dx8.dle" /pdbtype:sept /libpath:"E:\3dsmax5\maxsdk\lib\\" /libpath:"..\..\cal3d_dx8\Debug"

!ENDIF 

# Begin Target

# Name "cal3d_max_exporter - Win32 Release"
# Name "cal3d_max_exporter - Win32 Debug"
# Name "cal3d_max_exporter - Win32 Release_Max5"
# Name "cal3d_max_exporter - Win32 Debug_Max5"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\win32\AnimationExportSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\AnimationTimePage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BaseInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BaseLight.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BaseMaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BaseMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BaseNode.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\BoneAssignmentPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\BoneCandidate.cpp
# End Source File
# Begin Source File

SOURCE=.\cal3d_max_exporter.cpp
# End Source File
# Begin Source File

SOURCE=.\cal3d_max_exporter.def
# End Source File
# Begin Source File

SOURCE=.\Cal3d_max_exporter.idl
# ADD MTL /h "Cal3d_max_exporter_i.h" /iid "Cal3d_max_exporter_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\cal3d_max_exporter.rc
# End Source File
# Begin Source File

SOURCE=..\src\ComString.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\ExportConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Exporter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Cal3D_DX8\src\FaceDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FileEx.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Filesys.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FlaresDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\LevelOfDetailPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Lodder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MaterialCandidate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\MaterialExportSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MaterialLibraryCandidate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\MaterialMapsPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\MaterialSelectionPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxAnimationExport.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxAnimationExportDesc.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxLight.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMaterialExport.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMaterialExportDesc.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMeshExport.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMeshExportDesc.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxNode.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxNullView.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxSkeletonExport.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxSkeletonExportDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MeshCandidate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\MeshExportSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\MRU.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\mtrlpropspage.cpp
# ADD CPP /I "..\..\cal3d_max_exporter"
# End Source File
# Begin Source File

SOURCE=..\src\Quadric.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SceneLightsDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SkeletonCandidate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\SkeletonExportSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\SkeletonFilePage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\SkeletonHierarchyPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32\SpringSystemPage.cpp
# ADD CPP /I "..\..\cal3d_max_exporter"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\src\SubmeshCandidate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\VertexCandidate.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\win32\AnimationExportSheet.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\AnimationTimePage.h
# End Source File
# Begin Source File

SOURCE=..\src\BaseInterface.h
# End Source File
# Begin Source File

SOURCE=..\src\BaseLight.h
# End Source File
# Begin Source File

SOURCE=..\src\BaseMaterial.h
# End Source File
# Begin Source File

SOURCE=..\src\BaseMesh.h
# End Source File
# Begin Source File

SOURCE=..\src\BaseNode.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\BoneAssignmentPage.h
# End Source File
# Begin Source File

SOURCE=..\src\BoneCandidate.h
# End Source File
# Begin Source File

SOURCE=.\cal3d_max_exporter.h
# End Source File
# Begin Source File

SOURCE=..\src\ComString.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\ExportConfig.h
# End Source File
# Begin Source File

SOURCE=..\src\Exporter.h
# End Source File
# Begin Source File

SOURCE=..\..\Cal3D_DX8\src\FaceDesc.h
# End Source File
# Begin Source File

SOURCE=..\src\FileEx.h
# End Source File
# Begin Source File

SOURCE=..\src\Filesys.h
# End Source File
# Begin Source File

SOURCE=..\src\FlaresDlg.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\LevelOfDetailPage.h
# End Source File
# Begin Source File

SOURCE=..\src\Lodder.h
# End Source File
# Begin Source File

SOURCE=..\src\MaterialCandidate.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\MaterialExportSheet.h
# End Source File
# Begin Source File

SOURCE=..\src\MaterialLibraryCandidate.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\MaterialMapsPage.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\MaterialSelectionPage.h
# End Source File
# Begin Source File

SOURCE=.\MaxAnimationExport.h
# End Source File
# Begin Source File

SOURCE=.\MaxAnimationExportDesc.h
# End Source File
# Begin Source File

SOURCE=.\MaxFlare.h
# End Source File
# Begin Source File

SOURCE=.\MaxInterface.h
# End Source File
# Begin Source File

SOURCE=.\MaxLight.h
# End Source File
# Begin Source File

SOURCE=.\MaxMaterial.h
# End Source File
# Begin Source File

SOURCE=.\MaxMaterialExport.h
# End Source File
# Begin Source File

SOURCE=.\MaxMaterialExportDesc.h
# End Source File
# Begin Source File

SOURCE=.\MaxMesh.h
# End Source File
# Begin Source File

SOURCE=.\MaxMeshExport.h
# End Source File
# Begin Source File

SOURCE=.\MaxMeshExportDesc.h
# End Source File
# Begin Source File

SOURCE=.\MaxNode.h
# End Source File
# Begin Source File

SOURCE=.\MaxNullView.h
# End Source File
# Begin Source File

SOURCE=.\MaxSkeletonExport.h
# End Source File
# Begin Source File

SOURCE=.\MaxSkeletonExportDesc.h
# End Source File
# Begin Source File

SOURCE=..\src\MeshCandidate.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\MeshExportSheet.h
# End Source File
# Begin Source File

SOURCE=..\..\Cal3D_DX8\src\Mpeg4FDP.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\MRU.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\mtrlpropspage.h
# End Source File
# Begin Source File

SOURCE=..\src\Quadric.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\src\scenelightsdlg.h
# End Source File
# Begin Source File

SOURCE=..\src\SkeletonCandidate.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\SkeletonExportSheet.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\SkeletonFilePage.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\SkeletonHierarchyPage.h
# End Source File
# Begin Source File

SOURCE=..\src\win32\SpringSystemPage.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\src\SubmeshCandidate.h
# End Source File
# Begin Source File

SOURCE=..\src\VertexCandidate.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cal3d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cal3d_bar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cal3d_max_exporter.rc2
# End Source File
# Begin Source File

SOURCE=.\Cal3d_max_exporter.rgs
# End Source File
# Begin Source File

SOURCE=.\res\type_bone.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_dummy.ico
# End Source File
# Begin Source File

SOURCE=.\res\type_other.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\AUTHORS
# End Source File
# Begin Source File

SOURCE=.\ChangeLog
# End Source File
# Begin Source File

SOURCE=.\COPYING
# End Source File
# Begin Source File

SOURCE=.\INSTALL
# End Source File
# Begin Source File

SOURCE=.\README
# End Source File
# Begin Source File

SOURCE=.\TODO
# End Source File
# End Target
# End Project
# Section cal3d_max_exporter : {00730020-0079-0073-7400-65006D002000}
# 	1:22:IDR_CAL3D_MAX_EXPORTER:4000
# End Section
