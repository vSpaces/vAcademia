# Microsoft Developer Studio Project File - Name="Cscl3DWS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Cscl3DWS - Win32 Natura3D_Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cscl3DWS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cscl3DWS.mak" CFG="Cscl3DWS - Win32 Natura3D_Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cscl3DWS - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Cscl3DWS - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Cscl3DWS - Win32 Natura3D_Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cscl3DWS - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "Common" /I "rm" /I "..\natura3d\plugs" /I "..\natura3d\engine" /I "..\natura3d\cal3d_dx8\src" /I "..\natura3d\common" /I "..\natura3d\natura\anim" /I "..\natura3d\d3d" /I "R:\USER\BDima\Programming\cscl\Common\\" /I "media\\" /I "classes\\" /I "mme\\" /I "rmml\\" /I "RenderManager\classes" /I "RenderManager" /I "..\..\natura3d\plugs" /I "..\..\natura3d\engine" /I "..\..\natura3d\common" /I "..\..\natura3d\natura" /I "..\..\natura3d\natura\anim" /I "..\..\natura3d\d3d" /I "..\..\natura3d\cal3d_dx8\src" /D "_DEBUG" /D "_MBCS" /D "OMS_RESM_BUILD" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CSCL" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /I "..\Common" /I "Common"
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Rendermanager.lib rmml.lib Winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Dlls/Debug/Cscl3DWS.dll" /pdbtype:sept /libpath:"..\natura3d\debug" /libpath:"..\natura3d\cal3d_dx8\debug" /libpath:"rmml\debug" /libpath:"rendermanager\Debug"
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=\Projects\mm\Dlls\Debug\Cscl3DWS.dll
InputPath=\Projects\mm\Dlls\Debug\Cscl3DWS.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Cscl3DWS - Win32 Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /O1 /I "..\Common" /I "Common" /I "rm" /I "..\natura3d\plugs" /I "..\natura3d\engine" /I "..\natura3d\cal3d_dx8\src" /I "..\natura3d\common" /I "..\natura3d\natura\anim" /I "..\natura3d\d3d" /I "R:\USER\BDima\Programming\cscl\Common\\" /I "media\\" /I "classes\\" /I "mme\\" /I "rmml\\" /I "RenderManager\classes" /I "RenderManager" /I "..\..\natura3d\plugs" /I "..\..\natura3d\engine" /I "..\..\natura3d\common" /I "..\..\natura3d\natura" /I "..\..\natura3d\natura\anim" /I "..\..\natura3d\d3d" /I "..\..\natura3d\cal3d_dx8\src" /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "OMS_RESM_BUILD" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD MTL /I "..\Common" /I "Common"
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib natura3d.lib cal3d_dx8.lib rendermanager.lib rmml.lib Winmm.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../Dlls/Release/Cscl3DWS.dll" /libpath:"..\natura3d\release" /libpath:"..\natura3d\cal3d_dx8\release" /libpath:"rmml\debug" /libpath:"rendermanager\release"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=\Projects\mm\Dlls\Release\Cscl3DWS.dll
InputPath=\Projects\mm\Dlls\Release\Cscl3DWS.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Cscl3DWS - Win32 Natura3D_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Cscl3DWS___Win32_Natura3D_Release"
# PROP BASE Intermediate_Dir "Cscl3DWS___Win32_Natura3D_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Cscl3DWS___Win32_Natura3D_Release"
# PROP Intermediate_Dir "Cscl3DWS___Win32_Natura3D_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "Common" /I "rm" /I "..\natura3d\plugs" /I "..\natura3d\engine" /I "..\natura3d\cal3d_dx8\src" /I "..\natura3d\common" /I "..\natura3d\natura\anim" /I "..\natura3d\d3d" /I "R:\USER\BDima\Programming\cscl\Common\\" /I "media\\" /I "classes\\" /I "mme\\" /I "rmml\\" /I "RenderManager\classes" /I "RenderManager" /I "..\..\natura3d\plugs" /I "..\..\natura3d\engine" /I "..\..\natura3d\common" /I "..\..\natura3d\natura" /I "..\..\natura3d\natura\anim" /I "..\..\natura3d\d3d" /I "..\..\natura3d\cal3d_dx8\src" /D "_DEBUG" /D "_MBCS" /D "OMS_RESM_BUILD" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CSCL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "Common" /I "rm" /I "..\natura3d\plugs" /I "..\natura3d\engine" /I "..\natura3d\cal3d_dx8\src" /I "..\natura3d\common" /I "..\natura3d\natura\anim" /I "..\natura3d\d3d" /I "R:\USER\BDima\Programming\cscl\Common\\" /I "media\\" /I "classes\\" /I "mme\\" /I "rmml\\" /I "RenderManager\classes" /I "RenderManager" /I "..\..\natura3d\plugs" /I "..\..\natura3d\engine" /I "..\..\natura3d\common" /I "..\..\natura3d\natura" /I "..\..\natura3d\natura\anim" /I "..\..\natura3d\d3d" /I "..\..\natura3d\cal3d_dx8\src" /D "_DEBUG" /D "_MBCS" /D "OMS_RESM_BUILD" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CSCL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /I "..\Common" /I "Common"
# ADD MTL /I "..\Common" /I "Common"
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 rendermanager.lib rmml.lib Winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Dlls/Debug/Cscl3DWS.dll" /pdbtype:sept /libpath:"..\natura3d\debug" /libpath:"..\natura3d\cal3d_dx8\debug" /libpath:"rmml\debug" /libpath:"rendermanager\Debug"
# ADD LINK32 rendermanager.lib rmml.lib Winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Dlls/Release/Cscl3DWS.dll" /pdbtype:sept /libpath:"rmml\release" /libpath:"rendermanager\N3DRelease"
# Begin Custom Build - Performing registration
OutDir=.\Cscl3DWS___Win32_Natura3D_Release
TargetPath=\Projects\mm\Dlls\Release\Cscl3DWS.dll
InputPath=\Projects\mm\Dlls\Release\Cscl3DWS.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Cscl3DWS - Win32 Debug"
# Name "Cscl3DWS - Win32 Release MinDependency"
# Name "Cscl3DWS - Win32 Natura3D_Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Common\ComManIntr.idl
# End Source File
# Begin Source File

SOURCE=.\Common\Common.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ComString.cpp
# End Source File
# Begin Source File

SOURCE=.\Cscl3DWS.cpp
# End Source File
# Begin Source File

SOURCE=.\Cscl3DWS.def
# End Source File
# Begin Source File

SOURCE=.\Cscl3DWS.idl
# ADD MTL /tlb ".\Cscl3DWS.tlb" /h "Cscl3DWS.h" /iid "Cscl3DWS_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\Cscl3DWS.rc
# End Source File
# Begin Source File

SOURCE=.\Common\cSynchData.cpp
# End Source File
# Begin Source File

SOURCE=.\dlldatax.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Common\LogManIntr.idl
# End Source File
# Begin Source File

SOURCE=..\Common\ResManIntr.idl
# End Source File
# Begin Source File

SOURCE=.\ResManWrap.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\SoundManIntr.idl
# End Source File
# Begin Source File

SOURCE=.\Classes\SoundWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\synch.cpp
# End Source File
# Begin Source File

SOURCE=.\tracer.cpp
# End Source File
# Begin Source File

SOURCE=.\WS3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Classes\WSThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Cscl3DWSCP.h
# End Source File
# Begin Source File

SOURCE=.\Common\cSynchData.h
# End Source File
# Begin Source File

SOURCE=.\dlldatax.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ResManWrap.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Classes\SoundWrapper.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\synch.h
# End Source File
# Begin Source File

SOURCE=.\WS3D.h
# End Source File
# Begin Source File

SOURCE=.\Classes\WSThread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\arrow.cur
# End Source File
# Begin Source File

SOURCE=.\arrow_l.cur
# End Source File
# Begin Source File

SOURCE=.\beam_i.cur
# End Source File
# Begin Source File

SOURCE=.\beam_il.cur
# End Source File
# Begin Source File

SOURCE=.\busy_l.cur
# End Source File
# Begin Source File

SOURCE=.\harrow.cur
# End Source File
# Begin Source File

SOURCE=.\hmove.cur
# End Source File
# Begin Source File

SOURCE=.\size1_i.cur
# End Source File
# Begin Source File

SOURCE=.\size2_i.cur
# End Source File
# Begin Source File

SOURCE=.\size3_i.cur
# End Source File
# Begin Source File

SOURCE=.\size4_i.cur
# End Source File
# Begin Source File

SOURCE=.\WS3D.rgs
# End Source File
# End Group
# End Target
# End Project
