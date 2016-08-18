
#pragma once

#include "CommonEngineHeader.h"

#define WORK_MODE_NORMALRENDER		1
#define WORK_MODE_SHADOWMAPRENDER	2
#define WORK_MODE_SNAPSHOTRENDER	3
#define WORK_MODE_DESTROY			4

#include "Thread.h"
#include "CommonDefines.h"
#include "ILoadInfoListener.h"
#include "GlobalInterfaceStorage.h"

#define FPS_SECONDS 60

class CNEngine/* : public INEngine*/
{
friend class CGlobalSingletonStorage;

public:
	int GetWorkMode()const;
	void SetWorkMode(int mode);

	// apply global setting
	void SetSettings();
	// load global settings
	void LoadSettings();
	// initialize OpenGL
	void Initialize(int resModelsFileID, int resTexturesFileID, ILoadInfoListener* loadInfoListener);
	// resize screen
	void Resize(int width, int height);
	// starting frame
	void BeginFrame(bool isClearNeeded);
	// ending frame
	void EndFrame();
	// returns current fps
	unsigned int GetCurrentFPS();
	unsigned int GetAverageFPS();

	// hander for WM_DISPLAYCHANGE
	// used for correct applying global settings
	void OnDisplayChange(HWND hwnd, UINT bitsPerPixel, UINT cxScreen, UINT cyScreen);
	
	void SetMouseSettings(bool isHide);

	void UpdateVSync();

	// is program working?
	bool IsWorking()const;
	// is rendering starting?
	bool IsDrawing()const;
	// were all data loaded?
	bool IsLoaded()const;

	void SetWorkingState(bool state);
	void SetDrawingState(bool state);
	void SetLoadedState(bool state);

	void SetBackgroundColor(float backgroundR, float backgroundG, float backgroundB, float backgroundA);

	int GetModelsResourceFileID();
	int GetTexturesResourceFileID();

	void SetAppHandle(HMODULE hModule);
	HMODULE GetAppHangle();

	void SetShowFPSNeededStatus(bool isShowFPSNeeded);
	bool IsShowFPSNeeded()const;

	void PrepareForDestroy();

	void SetMainThreadCoreID(int coreID);

	void ZeroProgramTime();

	void AddFPS(int fps);

	// engine working time
	__int64 time;
	// window handle
	HWND  ghWnd; 
	HWND  helperWnd;
	// device context handle
	HDC   ghDC; 
	HDC   ghHelperDC; 
	// OpenGL rendering context handle
	HGLRC ghRC; 
	HGLRC ghHelperRC; 
	// window rect
	RECT rect;	
	// special flag for Intel
	bool isIconicAndIntel;

private:
	CNEngine();
	CNEngine(const CNEngine&);
	CNEngine& operator=(const CNEngine&);
	~CNEngine();

	void End();
	bool InitializeGL(int width, int height);
	// setup pixel format for device context
	bool SetupPixelFormat(HDC hdc, PIXELFORMATDESCRIPTOR& pfd);
	// making faster texturing for model rendering
	void LinkTexturesAndModels();	

	void ShowFPSIfNeeded();

	void UpdateMinimizeState();

	int m_resModelsFileID; 
	int m_resTexturesFileID;
	HMODULE m_hModule;

	// engine states
	bool m_isWorking, m_isDrawing, m_isLoaded;
	// states of working with texturing
	bool m_isMultiTexturing, m_isTransparent;
	// time variables
	__int64 m_clock, m_oldClock, m_clockCount;
	// fps variables
	unsigned int m_frameCount, m_currentFPS;
	// data structure of display device
	DEVMODE m_dm, m_dm0;

	bool m_isFullScreen;
	bool m_isMouseHide;

	float m_backgroundR, m_backgroundG, m_backgroundB, m_backgroundA;

	int m_minFacesForVBO;

	int m_renderTargetWithStencilSupportMode;

	bool m_isNeedResize;
	bool m_isShowFPSNeeded;
	bool m_isNewFrameStarted;	
	bool m_isUsingShaders;

	unsigned int m_mainThreadCoreID;
	unsigned int m_mainThreadAffinityMask;

	int m_workMode;
	int m_resizeWidth, m_resizeHeight;

	MP_STRING m_titleString;

	CThread* m_helperThread;	

	int m_fpsData[FPS_SECONDS];
};
