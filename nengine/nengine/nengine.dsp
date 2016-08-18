# Microsoft Developer Studio Project File - Name="nengine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=nengine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nengine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nengine.mak" CFG="nengine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nengine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "nengine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nengine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "../nengine/" /I "../nmath/" /I "../ncommon/" /I "../nengine/ode/include/ode/" /I "../nengine/ode/include/" /I "../nanimation/" /I "../nsound/" /I "../nphysics/" /I "../ngui/" /I "../nscript/" /I "../nscript/include/" /I "include/" /I "../nengine/FTGL/" /I "../nengine/freetype/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "nengine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../nengine/" /I "../nmath/" /I "../ncommon/" /I "../nengine/ode/include/ode/" /I "../nengine/ode/include/" /I "../nanimation/" /I "../nsound/" /I "../nphysics/" /I "../ngui/" /I "../nscript/" /I "../nscript/include/" /I "include/" /I "FTGL/" /I "freetype/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "nengine - Win32 Release"
# Name "nengine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\2DScene.cpp
# End Source File
# Begin Source File

SOURCE=.\2DSceneManager.cpp
# End Source File
# Begin Source File

SOURCE=.\3DObject.cpp
# End Source File
# Begin Source File

SOURCE=.\3DObjectManager.cpp
# End Source File
# Begin Source File

SOURCE=.\AccumVertexStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\AuxilaryProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\BasePostEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\BillboardEffectsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Clouds.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonObject.cpp
# End Source File
# Begin Source File

SOURCE=.\CompoundDepthStencil.cpp
# End Source File
# Begin Source File

SOURCE=.\CompoundTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\CubeMap.cpp
# End Source File
# Begin Source File

SOURCE=.\CubeMapManager.cpp
# End Source File
# Begin Source File

SOURCE=.\data.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayList.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayListManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayListRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawingArea.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicClouds.cpp
# End Source File
# Begin Source File

SOURCE=.\FastVertexStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\Font.cpp
# End Source File
# Begin Source File

SOURCE=.\FontManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalInterfaceStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalSingletonStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\HDRPostEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\Heightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\HeightmapManager.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoTextConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\libtexture3d.cpp
# End Source File
# Begin Source File

SOURCE=.\LODGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\LODGroupManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MainRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\Material.cpp
# End Source File
# Begin Source File

SOURCE=.\MaterialManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Model.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelClouds.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelManager.cpp
# End Source File
# Begin Source File

SOURCE=.\NEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\noise3D.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemManager.cpp
# End Source File
# Begin Source File

SOURCE=.\PBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderingCallback.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\RotationPack.cpp
# End Source File
# Begin Source File

SOURCE=.\Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Shader.cpp
# End Source File
# Begin Source File

SOURCE=.\ShaderManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadowManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadowObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\StandardRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\StatesProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture3d.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture3DManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeOfDayController.cpp
# End Source File
# Begin Source File

SOURCE=.\VARenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\VBOObject.cpp
# End Source File
# Begin Source File

SOURCE=.\VertexStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterSurface.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\2DScene.h
# End Source File
# Begin Source File

SOURCE=.\2DSceneManager.h
# End Source File
# Begin Source File

SOURCE=.\3DObject.h
# End Source File
# Begin Source File

SOURCE=.\3DObjectManager.h
# End Source File
# Begin Source File

SOURCE=.\AccumVertexStorage.h
# End Source File
# Begin Source File

SOURCE=.\AuxilaryProcessor.h
# End Source File
# Begin Source File

SOURCE=.\BasePostEffect.h
# End Source File
# Begin Source File

SOURCE=.\BillboardEffectsManager.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\CameraManager.h
# End Source File
# Begin Source File

SOURCE=.\CameraManagerInterface.h
# End Source File
# Begin Source File

SOURCE=.\Clouds.h
# End Source File
# Begin Source File

SOURCE=.\CommonDefines.h
# End Source File
# Begin Source File

SOURCE=.\CommonObject.h
# End Source File
# Begin Source File

SOURCE=.\CompoundDepthStencil.h
# End Source File
# Begin Source File

SOURCE=.\CompoundTexture.h
# End Source File
# Begin Source File

SOURCE=.\CubeMap.h
# End Source File
# Begin Source File

SOURCE=.\CubeMapManager.h
# End Source File
# Begin Source File

SOURCE=.\data.h
# End Source File
# Begin Source File

SOURCE=.\DisplayList.h
# End Source File
# Begin Source File

SOURCE=.\DisplayListManager.h
# End Source File
# Begin Source File

SOURCE=.\DisplayListRenderer.h
# End Source File
# Begin Source File

SOURCE=.\DrawingArea.h
# End Source File
# Begin Source File

SOURCE=.\DynamicClouds.h
# End Source File
# Begin Source File

SOURCE=.\FastVertexStorage.h
# End Source File
# Begin Source File

SOURCE=.\Font.h
# End Source File
# Begin Source File

SOURCE=.\FontManager.h
# End Source File
# Begin Source File

SOURCE=.\FrameBuffer.h
# End Source File
# Begin Source File

SOURCE=.\GlobalInterfaceStorage.h
# End Source File
# Begin Source File

SOURCE=.\GlobalSingletonStorage.h
# End Source File
# Begin Source File

SOURCE=.\HDRPostEffect.h
# End Source File
# Begin Source File

SOURCE=.\Heightmap.h
# End Source File
# Begin Source File

SOURCE=.\HeightmapManager.h
# End Source File
# Begin Source File

SOURCE=.\InfoTextConsole.h
# End Source File
# Begin Source File

SOURCE=.\libimg.h
# End Source File
# Begin Source File

SOURCE=.\libtexture3d.h
# End Source File
# Begin Source File

SOURCE=.\LODGroup.h
# End Source File
# Begin Source File

SOURCE=.\LODGroupManager.h
# End Source File
# Begin Source File

SOURCE=.\MainRenderer.h
# End Source File
# Begin Source File

SOURCE=.\Manager.h
# End Source File
# Begin Source File

SOURCE=.\Material.h
# End Source File
# Begin Source File

SOURCE=.\MaterialManager.h
# End Source File
# Begin Source File

SOURCE=.\Model.h
# End Source File
# Begin Source File

SOURCE=.\ModelClouds.h
# End Source File
# Begin Source File

SOURCE=.\ModelManager.h
# End Source File
# Begin Source File

SOURCE=.\NEngine.h
# End Source File
# Begin Source File

SOURCE=.\noise3D.h
# End Source File
# Begin Source File

SOURCE=.\NvTriStrip.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemManager.h
# End Source File
# Begin Source File

SOURCE=.\PBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Renderer.h
# End Source File
# Begin Source File

SOURCE=.\RenderingCallback.h
# End Source File
# Begin Source File

SOURCE=.\RenderTarget.h
# End Source File
# Begin Source File

SOURCE=.\RotationPack.h
# End Source File
# Begin Source File

SOURCE=.\Scene.h
# End Source File
# Begin Source File

SOURCE=.\SceneManager.h
# End Source File
# Begin Source File

SOURCE=.\SceneObject.h
# End Source File
# Begin Source File

SOURCE=.\Shader.h
# End Source File
# Begin Source File

SOURCE=.\ShaderManager.h
# End Source File
# Begin Source File

SOURCE=.\ShadowManager.h
# End Source File
# Begin Source File

SOURCE=.\ShadowObject.h
# End Source File
# Begin Source File

SOURCE=.\Sprite.h
# End Source File
# Begin Source File

SOURCE=.\SpriteManager.h
# End Source File
# Begin Source File

SOURCE=.\SpriteProcessor.h
# End Source File
# Begin Source File

SOURCE=.\StandardRenderer.h
# End Source File
# Begin Source File

SOURCE=.\StatesProcessor.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Terrain.h
# End Source File
# Begin Source File

SOURCE=.\TerrainManager.h
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\Texture3d.h
# End Source File
# Begin Source File

SOURCE=.\Texture3DManager.h
# End Source File
# Begin Source File

SOURCE=.\TextureLoader.h
# End Source File
# Begin Source File

SOURCE=.\TextureManager.h
# End Source File
# Begin Source File

SOURCE=.\TimeOfDayController.h
# End Source File
# Begin Source File

SOURCE=.\tinystr.h
# End Source File
# Begin Source File

SOURCE=.\tinyxml.h
# End Source File
# Begin Source File

SOURCE=.\VARenderer.h
# End Source File
# Begin Source File

SOURCE=.\VBOObject.h
# End Source File
# Begin Source File

SOURCE=.\VertexStorage.h
# End Source File
# Begin Source File

SOURCE=.\WaterSurface.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
