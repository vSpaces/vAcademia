# Microsoft Developer Studio Project File - Name="cal3d" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cal3d - Win32 3DSMan
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cal3d.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cal3d.mak" CFG="cal3d - Win32 3DSMan"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cal3d - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cal3d - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cal3d - Win32 3DSMan" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cal3d - Win32 Release_Max5" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cal3d - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAL3D_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAL3D_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx.lib winmm.lib /nologo /dll /machine:I386 /out:"C:\3dsmax7\cal3d_dx8.dll" /implib:"../lib/cal3d_dx8.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cal3d - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAL3D_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAL3D_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"C:\Program Files\Autodesk\3ds Max 2009\cal3d_dx8_dbg.dll " /implib:"../lib/cal3d_dx8_dbg.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cal3d - Win32 3DSMan"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cal3d___Win32_3DSMan"
# PROP BASE Intermediate_Dir "cal3d___Win32_3DSMan"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "cal3d___Win32_3DSMan"
# PROP Intermediate_Dir "cal3d___Win32_3DSMan"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAL3D_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAL3D_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"..\..\BSPTree\debug\cal3d_dx8.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"e:\3dsmax5\cal3d_dx8.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "cal3d - Win32 Release_Max5"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cal3d___Win32_Release_Max5"
# PROP BASE Intermediate_Dir "cal3d___Win32_Release_Max5"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "cal3d___Win32_Release_Max5"
# PROP Intermediate_Dir "cal3d___Win32_Release_Max5"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAL3D_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "E:\3dsmax5\maxsdk\include\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAL3D_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx.lib winmm.lib /nologo /dll /machine:I386 /out:"..\dlls\cal3d_dx8.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx.lib winmm.lib /nologo /dll /machine:I386 /out:"Release_DX5\cal3d_dx8_dbg.dll" /libpath:"E:\3dsmax5\maxsdk\lib\\"

!ENDIF 

# Begin Target

# Name "cal3d - Win32 Release"
# Name "cal3d - Win32 Debug"
# Name "cal3d - Win32 3DSMan"
# Name "cal3d - Win32 Release_Max5"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\animation_action.cpp
# End Source File
# Begin Source File

SOURCE=.\src\animation_cycle.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AnimSequencer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bone.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bvhFigure.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bvhloader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bvhParser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cal3dComString.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CalAnimationMorph.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CalMorpher.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CalShadowData.cpp
# End Source File
# Begin Source File

SOURCE=.\src\char_mixer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coreanimation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\corebone.cpp
# End Source File
# Begin Source File

SOURCE=.\src\corebvhanimation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\corebvhanimation.h
# End Source File
# Begin Source File

SOURCE=.\src\corekeyframe.cpp
# End Source File
# Begin Source File

SOURCE=.\src\corematerial.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coremesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coremodel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coreskeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coresubmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coretrack.cpp
# End Source File
# Begin Source File

SOURCE=.\src\error.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FaceDesc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\global.cpp
# End Source File
# Begin Source File

SOURCE=.\src\loader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\logfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LookController.cpp
# End Source File
# Begin Source File

SOURCE=.\src\memfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\mixer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\model.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Mpeg4FDP.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Mpeg4FDP_cp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\physique.cpp
# End Source File
# Begin Source File

SOURCE=.\src\plane.cpp
# End Source File
# Begin Source File

SOURCE=.\src\platform.cpp
# End Source File
# Begin Source File

SOURCE=.\src\quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\src\renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\saver.cpp
# End Source File
# Begin Source File

SOURCE=.\src\skeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\src\springsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StepCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\submesh.cpp
# End Source File
# Begin Source File

SOURCE=.\src\vector.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\animation.h
# End Source File
# Begin Source File

SOURCE=.\src\animation_action.h
# End Source File
# Begin Source File

SOURCE=.\src\animation_cycle.h
# End Source File
# Begin Source File

SOURCE=.\src\AnimSequencer.h
# End Source File
# Begin Source File

SOURCE=.\src\bone.h
# End Source File
# Begin Source File

SOURCE=.\src\bvhFigure.h
# End Source File
# Begin Source File

SOURCE=.\src\bvhloader.h
# End Source File
# Begin Source File

SOURCE=.\src\bvhParams.h
# End Source File
# Begin Source File

SOURCE=.\src\bvhParser.h
# End Source File
# Begin Source File

SOURCE=.\src\cal3d.h
# End Source File
# Begin Source File

SOURCE=.\src\cal3dComString.h
# End Source File
# Begin Source File

SOURCE=.\src\CalAnimationMorph.h
# End Source File
# Begin Source File

SOURCE=.\src\CalMorpher.h
# End Source File
# Begin Source File

SOURCE=.\src\CalShadowData.h
# End Source File
# Begin Source File

SOURCE=.\src\char_mixer.h
# End Source File
# Begin Source File

SOURCE=.\src\coreanimation.h
# End Source File
# Begin Source File

SOURCE=.\src\corebone.h
# End Source File
# Begin Source File

SOURCE=.\src\corekeyframe.h
# End Source File
# Begin Source File

SOURCE=.\src\corematerial.h
# End Source File
# Begin Source File

SOURCE=.\src\coremesh.h
# End Source File
# Begin Source File

SOURCE=.\src\coremodel.h
# End Source File
# Begin Source File

SOURCE=.\src\coreskeleton.h
# End Source File
# Begin Source File

SOURCE=.\src\coresubmesh.h
# End Source File
# Begin Source File

SOURCE=.\src\coretrack.h
# End Source File
# Begin Source File

SOURCE=.\src\error.h
# End Source File
# Begin Source File

SOURCE=.\src\FaceDesc.h
# End Source File
# Begin Source File

SOURCE=.\src\global.h
# End Source File
# Begin Source File

SOURCE=.\src\hitverts.h
# End Source File
# Begin Source File

SOURCE=.\src\loader.h
# End Source File
# Begin Source File

SOURCE=.\src\logfile.h
# End Source File
# Begin Source File

SOURCE=.\src\LookController.h
# End Source File
# Begin Source File

SOURCE=.\src\memfile.h
# End Source File
# Begin Source File

SOURCE=.\src\mesh.h
# End Source File
# Begin Source File

SOURCE=.\src\mixer.h
# End Source File
# Begin Source File

SOURCE=.\src\model.h
# End Source File
# Begin Source File

SOURCE=.\src\Mpeg4FDP.h
# End Source File
# Begin Source File

SOURCE=.\src\physique.h
# End Source File
# Begin Source File

SOURCE=.\src\plane.h
# End Source File
# Begin Source File

SOURCE=.\src\platform.h
# End Source File
# Begin Source File

SOURCE=.\src\quaternion.h
# End Source File
# Begin Source File

SOURCE=.\src\renderer.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\saver.h
# End Source File
# Begin Source File

SOURCE=.\src\skeleton.h
# End Source File
# Begin Source File

SOURCE=.\src\springsystem.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\StepCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\submesh.h
# End Source File
# Begin Source File

SOURCE=.\src\vector.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
