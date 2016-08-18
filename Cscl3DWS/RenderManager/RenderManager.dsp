# Microsoft Developer Studio Project File - Name="RenderManager" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=RenderManager - Win32 Debug N3DRelease
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RenderManager.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RenderManager.mak" CFG="RenderManager - Win32 Debug N3DRelease"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RenderManager - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RenderManager - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RenderManager - Win32 Debug N3DRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RenderManager - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDERMANAGER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\res\\" /I "..\sm\\" /I "classes\\" /I "..\\" /I "..\..\natura3d\plugs" /I "..\mme" /I "..\rm" /I "..\rmml" /I "..\js" /I "..\..\Natura3d\engine" /I "..\..\natura3d\common" /I "..\..\natura3d\natura" /I "..\..\natura3d\natura\anim" /I "..\..\natura3d\d3d" /I "..\..\natura3d\cal3d_dx8\src" /I "Classes\Natura3D\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDERMANAGER_EXPORTS" /D "RM_BUILD" /D "OMS_SM_BUILD" /D "OMS_RM_BUILD" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib natura3d.lib cal3d_dx8_r.lib rmml.lib winmm.lib /nologo /dll /machine:I386 /out:"../../Dlls/Release/RenderManager.dll" /libpath:"..\..\natura3d\release" /libpath:"..\..\natura3d\cal3d_dx8\release" /libpath:"..\rmml\release"

!ELSEIF  "$(CFG)" == "RenderManager - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDERMANAGER_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /w /W0 /Gm /GX /ZI /Od /I "..\resloader" /I "..\res\\" /I "..\sm\\" /I "classes\\" /I "..\\" /I "..\..\natura3d\plugs" /I "..\..\natura3d\include" /I "..\mme" /I "..\rm" /I "..\rmml" /I "..\js" /I "..\..\Natura3d\engine" /I "..\..\natura3d\common" /I "..\..\natura3d\natura" /I "..\..\natura3d\natura\anim" /I "..\..\natura3d\d3d" /I "..\..\natura3d\cal3d_dx8\src" /I "Classes\Natura3D\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDERMANAGER_EXPORTS" /D "XP_WIN" /D "RM_BUILD" /D "OMS_SM_BUILD" /D "OMS_RM_BUILD" /D "CSCL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 resloader.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib natura3d_dbg.lib rmml.lib winmm.lib d3dx.lib cal3d_dx8_dbg.lib Dxerr8.lib /nologo /dll /debug /machine:I386 /out:"../../Dlls/Debug/RenderManager.dll" /pdbtype:sept /libpath:"..\..\natura3d\debug" /libpath:"..\..\natura3d\cal3d_dx8\debug" /libpath:"..\rmml\debug\\" /libpath:"..\lib\\" /libpath:"..\ResLoader\Debug"

!ELSEIF  "$(CFG)" == "RenderManager - Win32 Debug N3DRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RenderManager___Win32_Debug_N3DRelease"
# PROP BASE Intermediate_Dir "RenderManager___Win32_Debug_N3DRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "N3DRelease"
# PROP Intermediate_Dir "N3DRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\res\\" /I "..\sm\\" /I "classes\\" /I "..\\" /I "..\..\natura3d\plugs" /I "..\mme" /I "..\rm" /I "..\rmml" /I "..\js" /I "..\..\Natura3d\engine" /I "..\..\natura3d\common" /I "..\..\natura3d\natura" /I "..\..\natura3d\natura\anim" /I "..\..\natura3d\d3d" /I "..\..\natura3d\cal3d_dx8\src" /I "Classes\Natura3D\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDERMANAGER_EXPORTS" /D "XP_WIN" /D "RM_BUILD" /D "OMS_SM_BUILD" /D "OMS_RM_BUILD" /D "CSCL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\res\\" /I "..\sm\\" /I "classes\\" /I "..\\" /I "..\..\natura3d\plugs" /I "..\..\natura3d\include" /I "..\mme" /I "..\rm" /I "..\rmml" /I "..\js" /I "..\..\Natura3d\engine" /I "..\..\natura3d\common" /I "..\..\natura3d\natura" /I "..\..\natura3d\natura\anim" /I "..\..\natura3d\d3d" /I "..\..\natura3d\cal3d_dx8\src" /I "Classes\Natura3D\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDERMANAGER_EXPORTS" /D "XP_WIN" /D "RM_BUILD" /D "OMS_SM_BUILD" /D "OMS_RM_BUILD" /D "CSCL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib natura3d.lib cal3d_dx8.lib rmml.lib winmm.lib d3dx.lib /nologo /dll /debug /machine:I386 /out:"../../Dlls/Debug/RenderManager.dll" /pdbtype:sept /libpath:"..\..\natura3d\debug" /libpath:"..\..\natura3d\cal3d_dx8\debug" /libpath:"..\rmml\debug"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib natura3d_r.lib cal3d_dx8_r.lib rmml.lib winmm.lib d3dx.lib /nologo /dll /debug /machine:I386 /out:"../../Dlls/Release/RenderManager_r.dll" /pdbtype:sept /libpath:"..\..\natura3d\release" /libpath:"..\..\natura3d\cal3d_dx8\release" /libpath:"..\rmml\debug"

!ENDIF 

# Begin Target

# Name "RenderManager - Win32 Release"
# Name "RenderManager - Win32 Debug"
# Name "RenderManager - Win32 Debug N3DRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\natura3D\plugs\CameraController.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\Natura3D\n3d3DObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrm3DGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrm3DObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrm3DScene.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmActiveX.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\NRManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmAnim.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmExtraObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmImage.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmLight.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmMath3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmMotion.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmPath3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmPlugin.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmProjection.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmText.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmVideo.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmVisemes.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\RenderManager.rc
# End Source File
# Begin Source File

SOURCE=.\Classes\RMComp.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\Natura3D\rmn3dVideo.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\SMEmul.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\natura3D\plugs\CameraController.h
# End Source File
# Begin Source File

SOURCE=.\Classes\Natura3D\n3d3DObject.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrm3DGroup.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrm3DObject.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrm3DScene.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmActiveX.h
# End Source File
# Begin Source File

SOURCE=.\Classes\NRManager.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmAnim.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmAudio.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmBrowser.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmCamera.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmCharacter.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmCommon.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmEdit.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmExtraObjects.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmImage.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmLight.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmMath3D.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmMotion.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmObject.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmPath3D.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmPlugin.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmProjection.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmText.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmVideo.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmViewport.h
# End Source File
# Begin Source File

SOURCE=.\Classes\nrmVisemes.h
# End Source File
# Begin Source File

SOURCE=.\Classes\resource.h
# End Source File
# Begin Source File

SOURCE=..\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Classes\RMComp.h
# End Source File
# Begin Source File

SOURCE=.\Classes\Natura3D\rmn3dVideo.h
# End Source File
# Begin Source File

SOURCE=.\Classes\SMEmul.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Classes\transls.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Classes\cursor1.cur
# End Source File
# End Group
# Begin Source File

SOURCE=.\nrmMaterial
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
