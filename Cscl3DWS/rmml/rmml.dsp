# Microsoft Developer Studio Project File - Name="rmml" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=rmml - Win32 Debug MT
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rmml.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rmml.mak" CFG="rmml - Win32 Debug MT"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rmml - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rmml - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rmml - Win32 Debug MT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rmml - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMML_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\res\\" /I "..\mme\\" /I "..\js\\" /I ".\\" /I "..\oms\dontdelme" /I "../libxml2/include" /I "../libiconv/include" /I "..\..\libxml2\include" /I "..\..\libiconv\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMML_EXPORTS" /D "XP_WIN" /D "RMML_BUILD" /D "CSCL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 js32.lib /nologo /dll /debug /machine:I386 /out:"../../Dlls/Release/rmml.dll" /libpath:"..\lib\\"

!ELSEIF  "$(CFG)" == "rmml - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rmml___Win32_Debug0"
# PROP BASE Intermediate_Dir "rmml___Win32_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMML_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /I "..\res\\" /I "..\mme\\" /I "..\js\\" /I ".\\" /I "..\oms\dontdelme" /I "../libxml2/include" /I "../libiconv/include" /I "..\..\libxml2\include" /I "..\..\libiconv\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMML_EXPORTS" /D "XP_WIN" /D "RMML_BUILD" /D "CSCL" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 js32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"wsdlpull.lib" /out:"../../Dlls/Debug/rmml.dll" /pdbtype:sept /libpath:"..\lib\\"
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "rmml - Win32 Debug MT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rmml___Win32_Debug_MT"
# PROP BASE Intermediate_Dir "rmml___Win32_Debug_MT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "rmml___Win32_Debug_MT"
# PROP Intermediate_Dir "rmml___Win32_Debug_MT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\res\\" /I "..\mme\\" /I "..\js\\" /I ".\\" /I "..\oms\dontdelme" /I "../libxml2/include" /I "../libiconv/include" /I "..\..\libxml2\include" /I "..\..\libiconv\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMML_EXPORTS" /D "XP_WIN" /D "RMML_BUILD" /D "CSCL" /D "JS_THREADSAFE" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\res\\" /I "..\mme\\" /I "..\js\\" /I ".\\" /I "..\oms\dontdelme" /I "../libxml2/include" /I "../libiconv/include" /I "..\..\libxml2\include" /I "..\..\libiconv\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMML_EXPORTS" /D "XP_WIN" /D "RMML_BUILD" /D "CSCL" /D "JS_THREADSAFE" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 js32.lib /nologo /dll /debug /machine:I386 /out:"../../Dlls/Debug/rmml.dll" /pdbtype:sept /libpath:"..\lib\\"
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 js32mt.lib /nologo /dll /debug /machine:I386 /out:"../../Dlls/Debug/rmml.dll" /pdbtype:sept /libpath:"..\lib\\"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "rmml - Win32 Release"
# Name "rmml - Win32 Debug"
# Name "rmml - Win32 Debug MT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "2D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\2D\mlPointJSO.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRectangleJSO.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLActiveX.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLButton.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLFlash.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLImage.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLInput.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLQT.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLStyles.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLText.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLVideo.cpp
# End Source File
# End Group
# Begin Group "3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3D\mlRMML3DObject.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLCloud.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLHint.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLIdle.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLIdles.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLLight.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMap.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMaterials.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMotion.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMotionAOs.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMovement.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLObject.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLPath3D.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLPosition3D.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLRotation3D.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLScale3D.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLScene3D.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLShadow3D.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLShadows3D.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLSpeech.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLViewport.cpp
# End Source File
# End Group
# Begin Group "XML"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XML\mlDict.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlHashTable.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXML.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLAttr.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLCDATA.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLComment.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLElement.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLNamedNodeMap.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLNode.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLNodeList.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLText.cpp
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLXPath.cpp
# End Source File
# End Group
# Begin Group "gd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gd\gd_HTMLArchive.cpp
# End Source File
# Begin Source File

SOURCE=.\gd\gd_String.cpp
# End Source File
# Begin Source File

SOURCE=.\gd\gd_Strings.cpp
# End Source File
# Begin Source File

SOURCE=.\gd\gd_UTF8String.cpp
# End Source File
# Begin Source File

SOURCE=.\gd\gd_XMLArchive.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\mlBuildEvalCtxt.cpp
# End Source File
# Begin Source File

SOURCE=.\mlColorJSO.cpp
# End Source File
# Begin Source File

SOURCE=.\mldRMMLDebugger.cpp
# End Source File
# Begin Source File

SOURCE=.\mlErrors.cpp
# End Source File
# Begin Source File

SOURCE=.\mlJSClass.cpp
# End Source File
# Begin Source File

SOURCE=.\mlJSClassProto.cpp
# End Source File
# Begin Source File

SOURCE=.\mlMapJSS2JSO.cpp
# End Source File
# Begin Source File

SOURCE=.\mlMapWCS2Int.cpp
# End Source File
# Begin Source File

SOURCE=.\mlNextUpdateDTimeList.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMML.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLComposition.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLContinuous.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLElement.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSKey.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSMouse.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSPlAuth.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSPlMap.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSPlMap.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSPlModule.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSPlServer.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSShellFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLLoadable.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLMath3D.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLModule.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLPButton.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLPlugin.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLResource.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLScene.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLScript.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLSequencer.cpp
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLVisemes.cpp
# End Source File
# Begin Source File

SOURCE=.\mlRMMLVisible.cpp
# End Source File
# Begin Source File

SOURCE=.\mlSceneManager.cpp
# End Source File
# Begin Source File

SOURCE=.\mlSoapJSO.cpp
# End Source File
# Begin Source File

SOURCE=.\mlSPParser.cpp
# End Source File
# Begin Source File

SOURCE=.\mlSynch.cpp
# End Source File
# Begin Source File

SOURCE=.\mlSynchData.cpp
# End Source File
# Begin Source File

SOURCE=.\mlUIDefault.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "2Dh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\2D\mlPointJSO.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRectangleJSO.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLActiveX.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLAnimation.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLBrowser.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLButton.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLFlash.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLImage.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLInput.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLIQT.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLQT.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLStyles.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLText.h
# End Source File
# Begin Source File

SOURCE=.\2D\mlRMMLVideo.h
# End Source File
# End Group
# Begin Group "3Dh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3D\mlRMML3DObject.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLCamera.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLCharacter.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLCloud.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLGroup.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLHint.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLIdle.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLIdles.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLLight.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMap.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMaterial.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMaterials.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMotion.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMotionAOs.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLMovement.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLObject.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLPath3D.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLPosition3D.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLRotation3D.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLScale3D.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLScene3D.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLShadow3D.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLShadows3D.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLSpeech.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLViewport.h
# End Source File
# Begin Source File

SOURCE=.\3D\mlRMMLVisemes.h
# End Source File
# End Group
# Begin Group "XMLh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XML\mlDict.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlHashTable.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXML.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLAttr.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLCDATA.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLComment.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLDocument.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLElement.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLNamedNodeMap.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLNode.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLNodeList.h
# End Source File
# Begin Source File

SOURCE=.\XML\mlRMMLXMLText.h
# End Source File
# End Group
# Begin Group "gdh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gd\gd_HTMLArchive.h
# End Source File
# Begin Source File

SOURCE=.\gd\gd_listtmpl.h
# End Source File
# Begin Source File

SOURCE=.\gd\gd_String.h
# End Source File
# Begin Source File

SOURCE=.\gd\gd_Strings.h
# End Source File
# Begin Source File

SOURCE=.\gd\gd_UTF8String.h
# End Source File
# Begin Source File

SOURCE=.\gd\gd_XMLArchive.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\oms\mi.h
# End Source File
# Begin Source File

SOURCE=.\mlBuildEvalCtxt.h
# End Source File
# Begin Source File

SOURCE=.\mlColorJSO.h
# End Source File
# Begin Source File

SOURCE=.\mldRMMLDebugger.h
# End Source File
# Begin Source File

SOURCE=.\mlErrors.h
# End Source File
# Begin Source File

SOURCE=.\mlJSClass.h
# End Source File
# Begin Source File

SOURCE=.\mlJSClassProto.h
# End Source File
# Begin Source File

SOURCE=.\mlMapJSS2JSO.h
# End Source File
# Begin Source File

SOURCE=.\mlMapWCS2Int.h
# End Source File
# Begin Source File

SOURCE=.\mlNextUpdateDTimeList.h
# End Source File
# Begin Source File

SOURCE=.\mlRMML.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLAudio.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLBuilder.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLComposition.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLContinuous.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLElement.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLIterator.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLJSPlServer.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLLoadable.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLModule.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLPButPLHA.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLPButton.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLPlugin.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLResource.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLScene.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLScript.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLSequencer.h
# End Source File
# Begin Source File

SOURCE=.\mlRMMLVisible.h
# End Source File
# Begin Source File

SOURCE=.\mlSceneManager.h
# End Source File
# Begin Source File

SOURCE=.\mlSoapJSO.h
# End Source File
# Begin Source File

SOURCE=.\mlSPParser.h
# End Source File
# Begin Source File

SOURCE=.\mlSynch.h
# End Source File
# Begin Source File

SOURCE=.\mlSynchData.h
# End Source File
# Begin Source File

SOURCE=..\Resource.h
# End Source File
# Begin Source File

SOURCE=..\..\OMSPlayer\resource.h
# End Source File
# Begin Source File

SOURCE=.\rmml.h
# End Source File
# Begin Source File

SOURCE=.\rmmld.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
