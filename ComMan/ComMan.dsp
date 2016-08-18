# Microsoft Developer Studio Project File - Name="ComMan" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ComMan - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ComMan.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ComMan.mak" CFG="ComMan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ComMan - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ComMan - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ComMan - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /I ".\md5" /I ".\ComMan" /I "..\UnZip\lib" /I "..\libxml2\include" /I "..\libiconv\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /I "..\Common" /I "..\common"
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib Winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib Winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib Winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib Winmm.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib Winmm.lib ws2_32.lib unzip32d.lib libxml2.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Dlls/Debug/ComMan.dll" /pdbtype:sept /libpath:"..\UnZip\lib\\" /libpath:"..\libxml2\win32\bin.msvc\\"
# SUBTRACT LINK32 /map
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=\User\Alex\programming\Dlls\Debug\ComMan.dll
InputPath=\User\Alex\programming\Dlls\Debug\ComMan.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ComMan - Win32 Release MinDependency"

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
# ADD CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD MTL /I "..\Common"
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib ..\UnZip\lib\unzip32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Debug/ComMan.dll"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\Debug\ComMan.dll
InputPath=.\Debug\ComMan.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ComMan - Win32 Debug"
# Name "ComMan - Win32 Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AsyncConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\AsyncMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Client_Data.cpp
# End Source File
# Begin Source File

SOURCE=.\ComMan.cpp
# End Source File
# Begin Source File

SOURCE=.\ComMan.def
# End Source File
# Begin Source File

SOURCE=.\ComMan.idl
# ADD MTL /tlb ".\ComMan.tlb" /h "ComMan.h" /iid "ComMan_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\ComMan.rc
# End Source File
# Begin Source File

SOURCE=.\ComManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ComManIntr.idl
# End Source File
# Begin Source File

SOURCE=..\Common\ComString.cpp
# End Source File
# Begin Source File

SOURCE=.\Connection.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\DataBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\dlldatax.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\FileFind.cpp
# End Source File
# Begin Source File

SOURCE=.\FileResource.cpp
# End Source File
# Begin Source File

SOURCE=.\HTTP.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalRepository.cpp
# End Source File
# Begin Source File

SOURCE=.\LogConnect.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\LogEventIntr.idl
# End Source File
# Begin Source File

SOURCE=..\Common\LogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\LogSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\md5\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoteResource.cpp
# End Source File
# Begin Source File

SOURCE=.\ResMan.cpp
# End Source File
# Begin Source File

SOURCE=.\ResManCreation.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ResManIntr.idl
# End Source File
# Begin Source File

SOURCE=.\ResModule.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AsyncConnect.h
# End Source File
# Begin Source File

SOURCE=.\AsyncMan.h
# End Source File
# Begin Source File

SOURCE=.\Client_Data.h
# End Source File
# Begin Source File

SOURCE=.\ComManager.h
# End Source File
# Begin Source File

SOURCE=.\ComManCP.h
# End Source File
# Begin Source File

SOURCE=.\common_globals.h
# End Source File
# Begin Source File

SOURCE=..\Common\ComString.h
# End Source File
# Begin Source File

SOURCE=.\Connection.h
# End Source File
# Begin Source File

SOURCE=..\Common\consts.h
# End Source File
# Begin Source File

SOURCE=..\Common\DataBuffer.h
# End Source File
# Begin Source File

SOURCE=.\dlldatax.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\FileFind.h
# End Source File
# Begin Source File

SOURCE=.\FileResource.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\HTTP.h
# End Source File
# Begin Source File

SOURCE=.\LocalRepository.h
# End Source File
# Begin Source File

SOURCE=.\LogConnect.h
# End Source File
# Begin Source File

SOURCE=..\Common\LogFile.h
# End Source File
# Begin Source File

SOURCE=.\LogSocket.h
# End Source File
# Begin Source File

SOURCE=.\md5\md5.h
# End Source File
# Begin Source File

SOURCE=.\RemoteResource.h
# End Source File
# Begin Source File

SOURCE=.\ResMan.h
# End Source File
# Begin Source File

SOURCE=.\ResManCreation.h
# End Source File
# Begin Source File

SOURCE=.\ResModule.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceImpl.h
# End Source File
# Begin Source File

SOURCE=.\SoundConnect.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\Unzip\lib\unzip32.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\commanag.bmp
# End Source File
# Begin Source File

SOURCE=.\ComManager.rgs
# End Source File
# End Group
# End Target
# End Project
