// NRManager.h: interface for the CNRManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRMANAGER_H__3487CB92_6C81_463E_9E3B_1D4AC27BFC9F__INCLUDED_)
#define AFX_NRMANAGER_H__3487CB92_6C81_463E_9E3B_1D4AC27BFC9F__INCLUDED_

#include "../CommonRenderManagerHeader.h"

#pragma warning (disable:4786)

#include "IDynamicTexture.h"
#include "..\oms\oms_context.h"
#include "LinkCollector.h"
#include "rm.h"
#ifndef csclRM_API
#include "nrm.h"
#endif
#include <map>
#include "..\nrmDrawManager.h"
#include "CameraController.h"
//#include "Clouds.h"
#include "HDRPostEffect.h"
//#include "UpdateCallback.h"
#include "IMouseController.h"
#include "UserSettings.h"
#include "GraphicQualityController.h"
#include "../ClothEditorSupporter.h"
#include "../GraphicsDebugSupporter.h"
#include "AudioEventsManager.h"
#include "VideoCaptureSupport.h"
#include "CodeEditor.h"
#include "Cave.h"

class CRMCompLib;
class crmResLibrary;
class nrmViewport;
class nrmQuery;
class nrmMath3D;
class nrmProjection;
class nrm3DScene;
struct iframefork;
class nrm3DSoundManager;
class CWorldEditor;

namespace desktop
{
	struct IDesktopSharingDestination;
}

namespace voip{
	class IVoipSoundManager;
}

#pragma warning (disable : 4275)


using namespace rmml;

namespace res
{
	struct resIAsynchResource;
};

#define LOW_QUALITY_MODE_ID			0
#define MEDIUM_QUALITY_MODE_ID		1
#define HIGH_QUALITY_MODE_ID		2
#define VERY_HIGH_QUALITY_MODE_ID	3

class csclRM_API CNRManager : public rm::rmIRenderManager, public rm::rmIRenderManagerAX, public CLinkCollector
{
public:
	CNRManager( oms::omsContext* aContext);
	virtual ~CNRManager();
	
	// реализация rm::rmIRenderManager
	std::vector<unsigned int> GetEngineWarningIDs();
	virtual unsigned int GetCurrentFPS();
	unsigned char GetMaxAntialiasingPower();
	virtual void* GetTaskManager();
	bool ToogleRenderState(int stateID);
	bool GetUserSetting(wchar_t* _name, char* apwcValue, unsigned short aSizeValue);
	int SetUserSetting(wchar_t* _name, wchar_t* _value);
	void GetEncodingPassword(std::string& value);
	void GetEncodingPassword(std::wstring& value);
	void ReloadUserSetting();
	virtual omsresult CreateMO(mlMedia* apMLMedia);
	virtual omsresult DestroyMO(mlMedia* apMLMedia);
	virtual omsresult DestroyMO(moMedia* apMOMedia);
	virtual bool StencilBufferExists();
	virtual ml3DRotation multiplyRotations(ml3DRotation &rot1, ml3DRotation &rot2);
	virtual rm::IMath3DRMML*	 GetMath3D() { return (rm::IMath3DRMML*)m_math3D;}
	virtual oms::omsresult SaveMedia(const mlMedia* apMLMedia, const wchar_t* apwcPath);
	virtual rm::rmIRenderManagerAX*	GetExtendedRM(){ return this;}
	virtual void MLMediaDeleting(mlMedia* apMLMedia);
	// возвращаем версию ОС
	void GetOSVersion(unsigned int* apuMajor,  unsigned int* apuMinor,  unsigned int* apuBuild);
	// уничтожить плагин, отвязанный ранее от mlMeida
	virtual oms::omsresult DestroyPlugin(moMedia* apPlugin);

	bool IsCaveMode();
	//
	void SaveBitmap(LPCTSTR fileName, HBITMAP hBitmap);
	bool SetControlWindow( HWND ahWnd, const RECT& arcRect);
	bool SetScreenMode(unsigned long adwWidth, unsigned long adwHeight, unsigned long adwBPP, bool abFullScreen);
	void SetScreenModeForOculus(unsigned long adwWidth, unsigned long adwHeight, unsigned long x, unsigned long y);
	void SetSoundManager(rm::rmISoundManager* asmISoundManager);
	void SetResourceManager(res::resIResourceManager* apResourceManager);
	void SetCommunicationManager(cm::cmICommunicationManager* apComManager);
	void SetSceneManager( mlISceneManager* apmlISceneManager);

	void StartLoadingResource();

	void EnableLastScreen(bool isEnabled);
	void LoadingModeChanged(bool isEnabled);

	bool IsKnownProxyServer(std::string& userName, std::string& password, std::string& host, std::string& port);

	nrmAudioEventsManager&	GetAudioEventsManager(){ return	m_AudioEventsManager;}

	std::vector<SResourceConsumingApplication> GetResourceConsumingApplications();
	
	// Для отладки: включает/выключает режим показа всех скрытых объектов
	void ShowHidden3DObjects(bool abShow);
	//
	// режимы рендеринга проекций
	bool SetProjectionScene(mlMedia* apMLScene);
	mlMedia* GetProjectedVisibleAt(const int& iX, const int& iY);
	// режими показа дебаговой информации
	int	 GetFilesFromCFG(int aiType, char* apszCFGFileName, char* appszFileNames, int bufCount);

	cm::cmICommunicationManager* GetComMan();
	oms::omsContext* GetContext();

	void CreateMapManager();
	void DestroyMapManager();
	void OnCreateWorldScene( nrm3DScene* aWorldScene);
	void OnDestroyWorldScene( nrm3DScene* aWorldScene);

	// Вернуть указатель на интерфейс IDesktopSharingDestination, который реализуется RMML объектом eval(alpcTarget)
	desktop::IDesktopSharingDestination* GetDesktopSharingDestination(char* alpcTarget);

	void PrepareForDestroy();
	void OnExit();

	void DeleteDynamicTexture(IDynamicTexture* dynamicTexture);
	void RemoveDynamicTextures();

	ILogWriter* GetLogWriter();
	IWorldEditor* GetWorldEditor();
	IMouseController* GetMouseController();

	void* GetCSForWindow(void* ahWnd);

	bool SetCSForChildWindows(LPCRITICAL_SECTION alpCS){ m_browsersCSForChildWindows = alpCS; return true; }

	HWND GetMainWindow(){ return m_wnd; }

	virtual bool FindFreePlace(ml3DPosition& pos, ml3DBounds& bounds, ml3DRotation& rot, int maxDist, ml3DPosition& posRes, const wchar_t* location);
	// возвращает строковый идентификатор локации, в которой находится 3D-объект
	virtual const char* GetObjectLocation(mlMedia* apMLMedia);
	// возвращает строковый идентификатор локации общения (зоны общения), в которой находится 3D-объект
	virtual const char* GetObjectCommunicationArea(mlMedia* apMLMedia);

	void SetAudioMode( wchar_t* sType, int iMode);
	void SetAudioVolume( wchar_t* sType, float fVolume);


	void SaveCloudsVisibility();
	void RestoreCloudsVisibility();

	void LoadAndApplyUserSettings();
	// код ошибки при отмены авторизации
	virtual void OnLogout( int iCode);
	virtual bool IsMotionPreloadedEnd();
	virtual bool IsBlendsPreloadedEnd();

	void OnMemoryOverflow();
	bool IsComputerSlow();

	void GetComputerInfo( std::wstring &compName, std::string &processName, unsigned int &processorCount, unsigned int &realCoreCount, bool &hyperthreadingEnabled, unsigned __int64 &physicalMemorySize, unsigned __int64 &virtualMemorySize, std::string &osVersion, unsigned char &osBits);
	void GetGPUInfo( std::string &vendorName, std::string &deviceName, unsigned __int64 &videoMemorySize, int &major, int &minor);

	// Заполняет координаты по загружаемым ресурсам
	void GetLoadingResourcesCoords( resourceLoadStruct* apResLoadStruct, int aResCount, float* pAvatarX, float* pAvatarY, float* pAvatarZ);

	void GetDriverLink(std::string& link, std::string& videoURL, unsigned char& linkType);

	void SetPlayingEnabled( int iType, bool bEnabled);
	bool GetPlayingEnabled( int iType);

	rm::IVideoCaptureSupport* GetVideoCaptureSupport();
	rm::ICodeEditorSupport* GetCodeEditorSupport();

public:
	void RenderScene(bool isPostEffectNeeded = true);
	void UpdateCompLibs(DWORD delta);
	void ShowHint(nrmViewport* vp, BOOL show);
	CRMCompLib* GetCompLib(mlMedia* apScene);
	omsresult CopyToLibrary(mlMedia* apMLMedia);
	omsresult DeleteFromLibrary(mlMedia* apMLMedia);
	__forceinline	rm::rmISoundManager*			GetSoundManager(){ return m_soundManager;}	
	__forceinline	res::resIResourceManager*		GetResourceManager(){ return m_resourceManager;}
	__forceinline	mlISceneManager*				GetSceneManager(){ return m_sceneManager;}
	__forceinline	voip::IVoipManager*				GetVoipMan(){ return m_context->mpVoipMan;}
	__forceinline	voip::IVoip3DSoundCalculator*	GetI3DSoundCalculator(){ return (voip::IVoip3DSoundCalculator*) m_soundMan3D;}
	nrm3DSoundManager*						Get3DSoundManager(){ return m_soundMan3D;};

	void RegisterController(IController* controller);
	void UnregisterController(IController* controller);

	void RegisterViewport(nrmViewport* viewport);
	void UnregisterViewport(nrmViewport* viewport);

	void ProcessClouds();
	void SetLogMode(bool isWorking);

	void UpdateAndRender();
	
	// Rect окна
	RECT m_wndRect;	
	nrmQuery*						m_query;

	int GetQualityMode();

private:
	int SetActiveCameraToScene(void* _scene);
	void LoadUserSettingsIfNeeded();
	void AutoFix16bitDisplays();
	void UpdateMumble();

protected:
	mlISceneManager*				m_sceneManager;
	rm::rmISoundManager*			m_soundManager;
	res::resIResourceManager*		m_resourceManager;
	mlMedia*						rmRenderedComp;
	mlMedia*						rmUIRenderedComp;
	mlMedia*						rmAuthRenderedComp;
	MP_MAP<mlMedia*, CRMCompLib*>	rmCompositions;
	DWORD							m_lastTick;
	DWORD							m_nextTick;
	nrmViewport*					m_hintViewport;
	nrmMath3D*						m_math3D;
	DWORD							m_screenWidth;
	DWORD							m_screenHeight;
	DWORD							m_screenBPP;
	HWND							m_wnd;
	cm::cmICommunicationManager*	m_comManager;
	oms::omsContext*				m_context;
	sync::syncISyncManager*			m_syncMan;
	nrm3DSoundManager*				m_soundMan3D;
	int								m_qualityMode;
	__int64							m_updateTime;
	__int64							m_updateResTime;
	__int64							m_lastUpdateTime;
	bool							m_renderingStarted;
	bool							m_isLogEnabled;
	bool							m_isPhysicsInitialized;

	bool*							m_cloudsVisibility;

	bool							m_isResolutionChanged;
	bool							m_isNeedToLoadUserSettings; 

	bool							m_isNeedCaveSnapshot;

	unsigned int					m_lastScreenTextureID;
 
	CClothEditorSupporter*			m_clothEditorSupporter;

	MP_VECTOR<IDynamicTexture *>	m_dynamicTexturesForDeleting;
	nrmAudioEventsManager			m_AudioEventsManager;

	CVector3D m_lastEyePos;
	CVector3D m_lastLookAt;

	CUserSettings m_userSettings;

	CGraphicQualityController*      m_graphicQualityController;

	MP_VECTOR<IController *>		m_controllers;
	MP_VECTOR<nrmViewport *>		m_viewports;

	CWorldEditor*					m_worldEditor;

	LPCRITICAL_SECTION m_browsersCSForChildWindows;

	CRITICAL_SECTION m_deleteDynamicTexturesCS;
	int m_cloudsVisibilitySize;
	bool m_isCaveSupport;

	CVideoCaptureSupport m_videoCaptureSupport;
	CCodeEditor m_codeEditorSupport;

	CCave m_cave;
};

#endif // !defined(AFX_NRMANAGER_H__3487CB92_6C81_463E_9E3B_1D4AC27BFC9F__INCLUDED_)
