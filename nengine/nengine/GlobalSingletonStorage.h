	
#pragma once

#pragma  warning (disable: 4786)

//#include <glew.h>
#include <atlconv.h>
#include "DisplayListManager.h"

#ifdef USE_PHYSICS
	#include "PhysicsObjectManager.h"
#endif

#include "TimeOfDayController.h"

#ifdef USE_3D_ANIMATION
	#include "MotionManager.h"
#endif

#include "SkeletonAnimationObjectManager.h"
#include "BillboardRenderer.h"
#include "ResourceProcessor.h"
#include "AuxilaryProcessor.h"
#include "Texture3DManager.h"
#include "LODGroupManager.h"
#include "SpriteProcessor.h"
#include "MaterialManager.h"
#include "3DObjectManager.h"
#include "StatesProcessor.h"
#include "CubeMapManager.h"
#include "TextureManager.h"
#include "2DSceneManager.h"
#include "RenderSettings.h"
#include "SpriteManager.h"
#include "CameraManager.h"
#include "ShaderManager.h"
#include "ShadowManager.h"
#include "TimeProcessor.h"
#include "BlendsManager.h"
#include "SceneManager.h"
#include "TaskManager.h"
#include "FontManager.h"
#include "IResLibrary.h"
#include "Statistics.h"
#include "DataCache.h"
#include "PathFinder.h"

#ifdef USE_SCRIPT
	#include "ScriptCaller.h"
#endif

#include "ModelManager.h"

#ifdef USE_SOUND
	#include "SoundManager.h"
#endif

#include "MainRenderer.h"

#ifdef USE_SKINNED_GUI
	#include "FormManager.h"
#endif

#include "InternalLogWriter.h"
#include "NEngine.h"

#include "ConfigurationChecker.h"
#include "ConfigurationLogger.h"
#include "PerformanceMonitor.h"
#include "ExtensionsInfo.h"
#include "ComputerInfo.h"
#include "GPUInfo.h"
#include "ProcessesInfo.h"

#include "FilterPalette.h"
#include "FilterSequencePalette.h"

#include "OGLdebug.h"

#include "OcclusionQueryManager.h"

class CGlobalSingletonStorage
{
public:
	CGlobalSingletonStorage();
	~CGlobalSingletonStorage();

	static CGlobalSingletonStorage* GetInstance();
	void Destroy();

	// Creating top-priority objects
	CNEngine ne;
	CSkeletonAnimationObjectManager skom;
	C3DObjectManager o3dm;
	CInternalLogWriter lw;
	CTimeProcessor tp;
	CMainRenderer mr;
	CFontManager fm;
	CDataCache dc;

	// Creating other objects
	CTimeOfDayController dtc;
	CDisplayListManager dlm;
	CTexture3DManager t3dm;
	CAuxilaryProcessor ap;
	CResourceProcessor rp;
	CLODGroupManager lod;
	CMaterialManager matm;
	CMotionManager motm;
	//C3DObjectManager o3dm;
	CStatesProcessor stp;
	C2DSceneManager sc2d;
	CCubeMapManager cmm;
	CSpriteProcessor sp;
	CSpriteManager sprm;
	CShadowManager shdw;
	CRenderSettings rs;
	CTextureManager tm;
	CSceneManager scm;
	CCameraManager cm;
	CShaderManager sm;
	CBlendsManager bm;

	CConfigurationChecker cc;
	CConfigurationLogger cg;
	CExtensionsInfo ei;
	CComputerInfo ci;
	CStatistics stat;
	CPerformanceMonitor pm;
	CGPUInfo gi;
	CProcessesInfo pi;

	CTaskManager taskm;

	CBillboardRenderer br;

//#ifdef USE_PHYSICS
	CPhysicsObjectManager phom;
//#endif	

	COcclusionQueryManager occlm;

	std::wstring sApplicationRootDirectory;

/*#ifdef USE_SCRIPT
	CScriptCaller sc;
#endif

#ifdef USE_SOUND
	CSoundManager snd;
#endif

#ifdef USE_SKINNED_GUI
	CFormManager frmm;
#endif*/

	CModelManager mm;

	CPathFinder pf;

	CFilterPalette fp;
	CFilterSequencePalette fsp;

	IResLibrary* rl;

	void SetResLibrary(IResLibrary* reslib);
	void SetLogWriter(ILogWriter* apLogWriter);
	
private:	
	CGlobalSingletonStorage(const CGlobalSingletonStorage&);
	CGlobalSingletonStorage& operator=(const CGlobalSingletonStorage&);	
};

extern CGlobalSingletonStorage* g;