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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx.lib winmm.lib /nologo /dll /machine:I386 /out:"R:\Town2006\MaxExporter\new_alex\cal3d_dx8.dll"

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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"C:\Program Files\3dsmax\cal3d_dx8_dbg.dll "
# SUBTRACT LINK32 /profile

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

SOURCE=.\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\animation_action.cpp
# End Source File
# Begin Source File

SOURCE=.\animation_cycle.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimSequencer.cpp
# End Source File
# Begin Source File

SOURCE=.\bone.cpp
# End Source File
# Begin Source File

SOURCE=.\bvhFigure.cpp
# End Source File
# Begin Source File

SOURCE=.\bvhloader.cpp
# End Source File
# Begin Source File

SOURCE=.\bvhParser.cpp
# End Source File
# Begin Source File

SOURCE=.\CalAnimationMorph.cpp
# End Source File
# Begin Source File

SOURCE=.\CalMorpher.cpp
# End Source File
# Begin Source File

SOURCE=.\CalShadowData.cpp
# End Source File
# Begin Source File

SOURCE=.\char_mixer.cpp
# End Source File
# Begin Source File

SOURCE=.\ComString.cpp
# End Source File
# Begin Source File

SOURCE=.\coreanimation.cpp
# End Source File
# Begin Source File

SOURCE=.\corebone.cpp
# End Source File
# Begin Source File

SOURCE=.\corebvhanimation.cpp
# End Source File
# Begin Source File

SOURCE=.\corebvhanimation.h
# End Source File
# Begin Source File

SOURCE=.\corekeyframe.cpp
# End Source File
# Begin Source File

SOURCE=.\corematerial.cpp
# End Source File
# Begin Source File

SOURCE=.\coremesh.cpp
# End Source File
# Begin Source File

SOURCE=.\coremodel.cpp
# End Source File
# Begin Source File

SOURCE=.\coreskeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\coresubmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\coretrack.cpp
# End Source File
# Begin Source File

SOURCE=.\error.cpp
# End Source File
# Begin Source File

SOURCE=.\FaceDesc.cpp
# End Source File
# Begin Source File

SOURCE=.\global.cpp
# End Source File
# Begin Source File

SOURCE=.\loader.cpp
# End Source File
# Begin Source File

SOURCE=.\logfile.cpp
# End Source File
# Begin Source File

SOURCE=.\LookController.cpp
# End Source File
# Begin Source File

SOURCE=.\memfile.cpp
# End Source File
# Begin Source File

SOURCE=.\mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\mixer.cpp
# End Source File
# Begin Source File

SOURCE=.\model.cpp
# End Source File
# Begin Source File

SOURCE=.\Mpeg4FDP.cpp
# End Source File
# Begin Source File

SOURCE=.\Mpeg4FDP_cp.cpp
# End Source File
# Begin Source File

SOURCE=.\physique.cpp
# End Source File
# Begin Source File

SOURCE=.\plane.cpp
# End Source File
# Begin Source File

SOURCE=.\platform.cpp
# End Source File
# Begin Source File

SOURCE=.\quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\saver.cpp
# End Source File
# Begin Source File

SOURCE=.\skeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\springsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\StepCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\submesh.cpp
# End Source File
# Begin Source File

SOURCE=.\vector.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\animation.h
# End Source File
# Begin Source File

SOURCE=.\animation_action.h
# End Source File
# Begin Source File

SOURCE=.\animation_cycle.h
# End Source File
# Begin Source File

SOURCE=.\AnimSequencer.h
# End Source File
# Begin Source File

SOURCE=.\bone.h
# End Source File
# Begin Source File

SOURCE=.\bvhFigure.h
# End Source File
# Begin Source File

SOURCE=.\bvhloader.h
# End Source File
# Begin Source File

SOURCE=.\bvhParams.h
# End Source File
# Begin Source File

SOURCE=.\bvhParser.h
# End Source File
# Begin Source File

SOURCE=.\cal3d.h
# End Source File
# Begin Source File

SOURCE=.\CalAnimationMorph.h
# End Source File
# Begin Source File

SOURCE=.\CalMorpher.h
# End Source File
# Begin Source File

SOURCE=.\CalShadowData.h
# End Source File
# Begin Source File

SOURCE=.\char_mixer.h
# End Source File
# Begin Source File

SOURCE=.\ComString.h
# End Source File
# Begin Source File

SOURCE=.\coreanimation.h
# End Source File
# Begin Source File

SOURCE=.\corebone.h
# End Source File
# Begin Source File

SOURCE=.\corekeyframe.h
# End Source File
# Begin Source File

SOURCE=.\corematerial.h
# End Source File
# Begin Source File

SOURCE=.\coremesh.h
# End Source File
# Begin Source File

SOURCE=.\coremodel.h
# End Source File
# Begin Source File

SOURCE=.\coreskeleton.h
# End Source File
# Begin Source File

SOURCE=.\coresubmesh.h
# End Source File
# Begin Source File

SOURCE=.\coretrack.h
# End Source File
# Begin Source File

SOURCE=.\error.h
# End Source File
# Begin Source File

SOURCE=.\FaceDesc.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\hitverts.h
# End Source File
# Begin Source File

SOURCE=.\loader.h
# End Source File
# Begin Source File

SOURCE=.\logfile.h
# End Source File
# Begin Source File

SOURCE=.\LookController.h
# End Source File
# Begin Source File

SOURCE=.\memfile.h
# End Source File
# Begin Source File

SOURCE=.\mesh.h
# End Source File
# Begin Source File

SOURCE=.\mixer.h
# End Source File
# Begin Source File

SOURCE=.\model.h
# End Source File
# Begin Source File

SOURCE=.\Mpeg4FDP.h
# End Source File
# Begin Source File

SOURCE=.\physique.h
# End Source File
# Begin Source File

SOURCE=.\plane.h
# End Source File
# Begin Source File

SOURCE=.\platform.h
# End Source File
# Begin Source File

SOURCE=.\quaternion.h
# End Source File
# Begin Source File

SOURCE=.\renderer.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\saver.h
# End Source File
# Begin Source File

SOURCE=.\skeleton.h
# End Source File
# Begin Source File

SOURCE=.\springsystem.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StepCtrl.h
# End Source File
# Begin Source File

SOURCE=.\submesh.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
