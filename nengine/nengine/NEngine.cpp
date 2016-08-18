
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"
#include "XMLPropertiesList.h"
#include "RenderingCallback.h"
#include "HelperFunctions.h"
#include "TextureManager.h"
#include "TimeProcessor.h"
#include "CameraManager.h"
#include "ModelManager.h"
#include "NEngine.h"
#include "XMLList.h"
#include "wglew.h"
#include "Math.h"
#include "cal3d/memory_leak.h"

#define FIRE_LOAD_EVENT(x) if (loadInfoListener) { loadInfoListener->##x##(); }

CNEngine::CNEngine():
	m_workMode(WORK_MODE_NORMALRENDER),
	MP_STRING_INIT(m_titleString),
	m_mainThreadAffinityMask(1),
	m_isNewFrameStarted(false),
	m_isShowFPSNeeded(false),
	m_mainThreadCoreID(0),
	m_isFullScreen(false),
	m_isNeedResize(false),	
	m_isMouseHide(true),
	m_backgroundR(0.0f),
	m_backgroundG(0.0f),
	m_backgroundB(0.0f),
	m_backgroundA(0.0f),
	m_isDrawing(false),
	m_isWorking(true),
	m_isLoaded(false),
	m_frameCount(0),
	m_currentFPS(0),
	m_clockCount(0), 
	m_isUsingShaders(false),
	isIconicAndIntel(false),
	helperWnd(NULL),
	ghWnd(NULL)
{	
	for (int i = 0; i < FPS_SECONDS; i++)
	{
		m_fpsData[i] = -1;
	}
}

int CNEngine::GetWorkMode()const
{
	return m_workMode;
}

void CNEngine::SetWorkMode(int mode)
{
	m_workMode = mode;
}

bool InitMultisample(HWND hWnd, PIXELFORMATDESCRIPTOR /*pfd*/, int& arbMultisampleFormat)
{ 
	// Возьмем наш формат пикселя
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (!wglChoosePixelFormatARB)
	{
		return false;
	}

	HDC hDC = GetDC(hWnd);

	int pixelFormat;
	bool valid;
	UINT numFormats;

	float fAttributes[] = {0, 0};
	int iAttributes[] = {WGL_DRAW_TO_WINDOW_ARB, GL_TRUE, 
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE, 
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, 
		WGL_COLOR_BITS_ARB, 24,          
		WGL_ALPHA_BITS_ARB, 8,           
		WGL_DEPTH_BITS_ARB, 24,          
		WGL_STENCIL_BITS_ARB, 8,         
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,  
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE, 
		WGL_SAMPLES_ARB, g->rs.GetInt(MSAA),            
		0, 0};

	valid = (wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats) > 0);

	if ((valid) && (numFormats >= 1))
	{
		arbMultisampleFormat  = pixelFormat; 
		return true;
	}

	return false;
}

void CNEngine::SetAppHandle(HMODULE hModule)
{
	m_hModule = hModule;
	g->rp.SetAppHandle(m_hModule);
}

HMODULE CNEngine::GetAppHangle()
{
	return m_hModule;
}

/*void CNEngine::AddAlphaPolygon(float x1, float y1, float z1, float u1, float v1,
					 float x2, float y2, float z2, float u2, float v2,
					 float x3, float y3, float z3, float u3, float v3,
					 int textureNumber)
{
	m_alphaPolygons[m_alphaPolygonsCount][0] = x1;
	m_alphaPolygons[m_alphaPolygonsCount][1] = y1;
	m_alphaPolygons[m_alphaPolygonsCount][2] = z1;
	m_alphaPolygons[m_alphaPolygonsCount][3] = u1;
	m_alphaPolygons[m_alphaPolygonsCount][4] = v1;
	m_alphaPolygons[m_alphaPolygonsCount][5] = x2;
	m_alphaPolygons[m_alphaPolygonsCount][6] = y2;
	m_alphaPolygons[m_alphaPolygonsCount][7] = z2;
	m_alphaPolygons[m_alphaPolygonsCount][8] = u2;
	m_alphaPolygons[m_alphaPolygonsCount][9] = v2;
	m_alphaPolygons[m_alphaPolygonsCount][10] = x3;
	m_alphaPolygons[m_alphaPolygonsCount][11] = y3;
	m_alphaPolygons[m_alphaPolygonsCount][12] = z3;
	m_alphaPolygons[m_alphaPolygonsCount][13] = u3;
	m_alphaPolygons[m_alphaPolygonsCount][14] = v3;

	m_alphaTextures[m_alphaPolygonsCount] = textureNumber;

	m_alphaPolygonsCount++;
}*/

void CNEngine::SetMouseSettings(bool isHide)
{
	m_isMouseHide = isHide;
}

bool CNEngine::IsWorking()const
{
	return m_isWorking;
}

void CNEngine::SetWorkingState(bool state)
{
	m_isWorking = state;
}

bool CNEngine::IsDrawing()const
{
	return m_isDrawing;
}

void CNEngine::SetDrawingState(bool state)
{
	m_isDrawing = state;	
}

bool CNEngine::IsLoaded()const
{
	return m_isLoaded;
}

void CNEngine::SetLoadedState(bool state)
{
	if (state)
	{
		m_clock = g->tp.GetTickCount();
		m_oldClock = g->tp.GetTickCount();
		m_clockCount = 0;
	}

	m_isLoaded = state;
}

void CNEngine::LinkTexturesAndModels()
{
	int i;
	for (i = 0; i < g->mm.GetCount(); i++)
	{
		CModel* model = g->mm.GetObjectPointer(i);

		int kTextures = model->GetTextureCount() - 1;

		for (int k = 0; k < kTextures; k++)
		{
			std::string tmp = model->GetTextureName(k);
			int num = g->tm.GetObjectNumber(tmp.data());
			assert(num != -1);
			
			model->ChangeTexture(k, num);
		}
	}	
}

void CNEngine::LoadSettings()
{
	g->lw.WriteLn("Loading engine configuration...");

	CXMLPropertiesList settings(GetApplicationRootDirectory() + L"settings.xml", WARNING_IF_NOT_FOUND);

	if (settings.IsLoaded())
	{
		int width = settings.GetInteger("screen_width");
		int height = settings.GetInteger("screen_height");
		m_isFullScreen = settings.GetBool("fullscreen");
		
		g->stp.SetSize(0, 0, width, height);
	}

	g->gi.Analyze();
	g->rs.LoadAll();
}

HINSTANCE GetAppHinstance(HWND hWnd)
{
     char lpClassName[1024]="";
     if (!GetClassName(hWnd,lpClassName,sizeof(lpClassName))) return NULL;
     WNDCLASSEX wndcx;
     memset(&wndcx,0,sizeof(wndcx));
     if (!GetClassInfoEx(NULL, lpClassName,&wndcx)) return NULL; //Fails here
     return wndcx.hInstance;
}

LONG WINAPI MainWndProc(HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/) 
{ 
	return 1;
}

void CNEngine::OnDisplayChange(HWND hwnd, UINT /*bitsPerPixel*/, UINT cxScreen, UINT cyScreen)
{
	g->lw.WriteLn("Applying settings...");

	DEVMODE dm;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

	if (((int(m_dm.dmPelsWidth) == g->stp.GetWidth()) && (int(m_dm.dmPelsHeight) == g->stp.GetHeight())) || (!m_isFullScreen))
	{
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));

		CHAR szAppName[] = "Win OpenGL"; 

		HINSTANCE hInst = GetAppHinstance(hwnd);
		
		WNDCLASS   wndclass; 
		wndclass.style         = 0; 
		wndclass.lpfnWndProc   =(WNDPROC)MainWndProc; 
		wndclass.cbClsExtra    = 0; 
		wndclass.cbWndExtra    = 0; 
		wndclass.hInstance     = hInst; 
		wndclass.hIcon         = LoadIcon (hInst, szAppName); 
		wndclass.hCursor       = LoadCursor (NULL,IDC_ARROW); 
		wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); 
		wndclass.lpszMenuName  = szAppName; 
		wndclass.lpszClassName = szAppName; 
		if (!RegisterClass (&wndclass)) 
		{
			return; 
		}
 
		g->ne.helperWnd = CreateWindowEx(0, szAppName, szAppName, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 512, 512,
			NULL, NULL, hInst, NULL);

		if (!helperWnd) 
		{
			MessageBox(NULL, "Create window failed", "Error", MB_OK); 
			return; 
		}

		ghHelperDC = GetDC(g->ne.helperWnd); 

		std::string deviceID, vendorID;
		g->gi.GetDeviceAndVendorIDs(vendorID, deviceID);
		g->lw.WriteLn("GPU Vendor ID: ", vendorID);
		g->lw.WriteLn("GPU Device ID: ", deviceID);

		/*if (g->ne.helperWnd)
		{
			ghDC = GetDC(g->ne.helperWnd);

			if (!SetupPixelFormat(ghDC, pfd)) 
			{
				PostQuitMessage(0); 
			}
	 
			ghRC = wglCreateContext(ghDC); 
			wglMakeCurrent(ghDC, ghRC); 
		}*/		
		
		/*int pixelformat;
		if ((g->ne.helperWnd) && (g->rs.GetInt(MSAA) != 0) && (InitMultisample(hwnd, pfd, pixelformat)))
		{
			g->lw.WriteLn("Try to set MSAA");
			wglMakeCurrent(ghDC, NULL); 
			wglDeleteContext(ghRC);
			ghDC = GetDC(hwnd); 

			if (ChoosePixelFormat(ghDC, &pfd) == 0)
			{
				return;
			}

			if (SetPixelFormat(ghDC, pixelformat, &pfd) == FALSE) 
			{                
				if (!SetupPixelFormat(ghDC, pfd)) 
				{
					PostQuitMessage(0); 
				}				
			}

			ghRC = wglCreateContext(ghDC); 
			wglMakeCurrent(ghDC, ghRC);
			GetClientRect(hwnd, &rect);
		}
		else*/
		{
			g->lw.WriteLn("No MSAA support detected!");

			//wglMakeCurrent(ghDC, NULL);
			/*if (g->ne.helperWnd)
			{
				wglDeleteContext(ghRC);
			}*/
			ghDC = GetDC(hwnd); 

			g->lw.WriteLn("SetupPixelFormat start");

			//PIXELFORMATDESCRIPTOR pfd;
			if (!SetupPixelFormat(ghDC, pfd)) 
			{
				PostQuitMessage(0); 
			}

			g->lw.WriteLn("SetupPixelFormat end");
			
			ghRC = wglCreateContext(ghDC); 
			wglMakeCurrent(ghDC, ghRC);			

			GetClientRect(hwnd, &rect);

			g->lw.WriteLn("context created");
		}

		g->lw.WriteLn("try to InitializeGL");
		if (!InitializeGL(cxScreen, cyScreen))
		{
			g->lw.WriteLn("InitializeGL failed");
			PostQuitMessage(0);
			g->ne.SetWorkingState(false);
		}
		else
		{
			g->lw.WriteLn("InitializeGL ok");
			g->cg.LogAll(g->lw.GetILogWriter());
			g->ne.SetDrawingState(true);
		}
	}
}

void CNEngine::SetSettings()
{
	if (m_isMouseHide)
	{
		ShowCursor(FALSE);
	}

	ZeroMemory(&m_dm, sizeof(m_dm));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &m_dm);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &m_dm0);

	if (m_isFullScreen)
	{
		if (int(m_dm.dmPelsWidth) != g->stp.GetWidth())
		{
			m_dm.dmPelsWidth = g->stp.GetWidth();
			m_dm.dmPelsHeight = g->stp.GetHeight();
			m_dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
			ChangeDisplaySettingsEx(NULL, &m_dm, NULL, CDS_FULLSCREEN, NULL);
		}
		else
		{
			OnDisplayChange(ghWnd, 32, g->stp.GetWidth(), g->stp.GetHeight());
		}	
	}
	else
	{
		OnDisplayChange(ghWnd, 32, g->stp.GetWidth(), g->stp.GetHeight());
	}	
}

int CNEngine::GetModelsResourceFileID()
{
	return m_resModelsFileID;
}

int CNEngine::GetTexturesResourceFileID()
{
	return m_resTexturesFileID;
}

void CNEngine::Initialize(int resModelsFileID, int resTexturesFileID, ILoadInfoListener* loadInfoListener)
{
	g->lw.WriteLn("Init (dot)Nap Engine...");

	m_resModelsFileID = resModelsFileID;
	m_resTexturesFileID = resTexturesFileID;

	if (g->rs.GetBool(SHADERS_USING))
	{
		g->sm.Init();
	}

#ifdef USE_SCRIPT
	// init dotnapscript&
	//g->sc.Set_3dom(&g->o3dm);
	g->sc.Set_frmm((ScriptCallbackListenerInterface *)&g->frmm);
	g->sc.Set_form((ScriptCallbackListenerInterface *)&g->frmm);
#endif

	g->tm.Initialize();
	if (loadInfoListener)
	{
		g->tm.SetLoadInfoListener(loadInfoListener);
#ifdef USE_SOUND
		g->snd.SetLoadInfoListener(loadInfoListener);
#endif
	}
	
	g->cmm.LoadAll();
	g->tm.LoadAll();

	g->matm.LoadAll();
	FIRE_LOAD_EVENT(OnMaterialsLoaded);

	g->mm.LoadAll();
	FIRE_LOAD_EVENT(OnModelsLoaded);

#ifdef USE_SKINNED_GUI
	g->fm.LoadAll();
	FIRE_LOAD_EVENT(OnGUILoaded);
#endif

	LinkTexturesAndModels();
	g->t3dm.LoadAll();
	FIRE_LOAD_EVENT(OnTextures3DLoaded);
	
#ifdef USE_SOUND
	g->snd.LoadAll();
#endif

	g->lod.LoadAll();
	FIRE_LOAD_EVENT(OnLODInfoLoaded);
	
	g->sprm.LoadAll();
	FIRE_LOAD_EVENT(OnSpritesInfoLoaded);
	
	g->sc2d.LoadAll();
	FIRE_LOAD_EVENT(OnScenes2DLoaded);
	
//	g->hmm.LoadAll();
//	FIRE_LOAD_EVENT(OnHeightmapsLoaded);
	
//	g->term.LoadAll();
//	FIRE_LOAD_EVENT(OnTerrainsLoaded);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	g->stp.PushAttrib(GL_TEXTURE_BIT);
	
	GLFUNC(glHint)(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	GLFUNC(glHint)(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	GLFUNC(glBlendFunc)(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLFUNC(glShadeModel)(GL_SMOOTH);
	g->stp.SetState(ST_BLEND, false);
	g->stp.SetState(ST_ZTEST, true);
}

GLvoid CNEngine::Resize(int width, int height) 
{ 
	RECT rct;
	GetClientRect(g->ne.ghWnd, &rct);

	if ((rct.bottom - rct.top != height) || (rct.right - rct.left != width))
	{
		m_isNeedResize = true;
		m_resizeWidth = width;
		m_resizeHeight = height;
	}
	else
	{
		m_isNeedResize = false;
	}

    GLFUNC(glViewport)(0, 0, width,height); 
 
    float aspect = (float)width / (float)height; 
 
    GLFUNC(glMatrixMode)(GL_PROJECTION); 

    GLFUNC(glLoadIdentity)(); 
    GLFUNC(gluPerspective)(45.0f, aspect, 0.1f, 10.0f); 

    GLFUNC(glMatrixMode)(GL_MODELVIEW); 
}

void CNEngine::UpdateVSync()
{
	static int lastVSync = -1;

	if (lastVSync != (int)g->rs.GetBool(VSYNC_ENABLED))
	{
		PFNWGLSWAPINTERVALEXTPROC pSwapInterwal;
		pSwapInterwal = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if (pSwapInterwal)
		if (g->rs.GetBool(VSYNC_ENABLED))
		{
			GLFUNC(pSwapInterwal)(1);
		}
		else
		{
			GLFUNC(pSwapInterwal)(0);
		}
		lastVSync = g->rs.GetBool(VSYNC_ENABLED);
	}
}
 
bool CNEngine::InitializeGL(int width, int height) 
{ 
    float aspect; 
 
    GLFUNC(glClearIndex)((GLfloat)BLACK_INDEX); 
    GLFUNC(glClearDepth)(1.0f); 

	g->stp.SetState(ST_ZTEST, true);
 
    GLFUNC(glMatrixMode)(GL_PROJECTION); 

    aspect = (float)width / (float)height; 
    GLFUNC(gluPerspective)(45.0f, aspect, 0.1f, 10.0f); 

    GLFUNC(glMatrixMode)(GL_MODELVIEW); 

	if (glewInit() != GLEW_OK)
	{
		return false;
	}

	return true;
} 

bool CNEngine::SetupPixelFormat(HDC hdc, PIXELFORMATDESCRIPTOR& pfd) 
{ 
	memset(&pfd, 0, sizeof(pfd));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); 
    pfd.nVersion = 1; 
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |  
                        PFD_DOUBLEBUFFER; 

	/*#define PFD_SUPPORT_COMPOSITION 0x00008000
	pfd.dwFlags += PFD_SUPPORT_COMPOSITION;*/

    pfd.dwLayerMask = PFD_MAIN_PLANE; 
    pfd.iPixelType = PFD_TYPE_RGBA; 
	pfd.cAlphaBits = 8;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;	
    pfd.cColorBits = 32; 
    pfd.cDepthBits = 24; 
    pfd.cAccumBits = 0; 
    pfd.cStencilBits = 8;
	pfd.cAuxBuffers = 0;

	g->lw.WriteLn("ChoosePixelFormat start");
	
	int pixelformat;
    if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == 0) 
    { 
		g->lw.WriteLn("ChoosePixelFormat failed");
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 

	g->lw.WriteLn("SetPixelFormat start");
 
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
		g->lw.WriteLn("SetPixelFormat failed");
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 

	g->lw.WriteLn("SetPixelFormat end");
 
    return TRUE; 
} 

void CNEngine::SetMainThreadCoreID(int coreID)
{
	g->taskm.SetMainThreadCoreID(coreID);
	m_mainThreadCoreID = coreID;
	m_mainThreadAffinityMask = 1;
	for (unsigned int i = 0; i < m_mainThreadCoreID; i++)
	{
		m_mainThreadAffinityMask *= 2;
	}
}

void CNEngine::ZeroProgramTime()
{
	/*time = 0;
	m_clock = g->tp.GetTickCount();*/
}

void CNEngine::UpdateMinimizeState()
{
	bool lastValue = isIconicAndIntel;
	isIconicAndIntel = false;
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	if (IsIconic(ghWnd))
	{
		isIconicAndIntel = true;			
		if (lastValue == false)
		{
			g->lw.WriteLn("Save using shaders: ", m_isUsingShaders ? 1 : 0);
			m_isUsingShaders = g->rs.GetBool(SHADERS_USING);
			g->rs.SetBool(SHADERS_USING, false);
		}
	}
	else if (lastValue == true)
	{
		g->lw.WriteLn("Restore using shaders: ", m_isUsingShaders ? 1 : 0);
		g->rs.SetBool(SHADERS_USING, m_isUsingShaders);
	}
}

void CNEngine::BeginFrame(bool isClearNeeded)
{
	UpdateMinimizeState();	

	HDC hDC = ghDC;
	if (g->gi.GetVendorID() == VENDOR_INTEL || ghDC == 0)
	{
		hDC = GetDC(ghWnd);
		if (( hDC != ghDC) || (g->gi.GetVendorID() == VENDOR_INTEL))
		{			
			ghDC = hDC;
			if (wglMakeCurrent(ghDC, ghRC) == FALSE)
			{
				g->lw.WriteLn("OpenGL context was lost");
			}				
		}		
	}

	stat_motionCount = 0;
	g->stat.OnNewFrame();
	g->sm.OnNewFrame();

	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE);

	HANDLE hndl = GetCurrentThread();
	SetThreadAffinityMask(hndl, m_mainThreadAffinityMask);

	if (m_isNeedResize)
	{
		Resize(m_resizeWidth, m_resizeHeight);
	}

	g->shdw.NewFrame();

	GLFUNC(glClearColor)(m_backgroundR, m_backgroundG, m_backgroundB, m_backgroundA);
	GLFUNC(glClearStencil)(127);
	if (isClearNeeded)
	{
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	} 

	int glErr = glGetError();
	if (glErr != 0)
	{
		g->lw.WriteLn("glClear error!");
	}

	glLoadIdentity();

	g->stp.PushMatrix();

	time = (g->tp.GetTickCount() - m_clock);
	m_clockCount += (time - m_oldClock);
	m_oldClock = time;

	g->stp.SetColor(255, 255, 255);
	GLFUNC(glColor3ub)(255, 255, 255);
	GLFUNC(glEnable)(GL_TEXTURE_2D);

	g->stp.SetDefaultStates();

	g->stp.SetState(ST_FRONT_FACE, GL_CCW);

	g->tm.BeforeNewFrame();
	g->mr.SetMode(MODE_RENDER_ALL);

	m_isNewFrameStarted = true;
}

void CNEngine::EndFrame()
{
	if (!m_isNewFrameStarted)
	{
		return;
	}

	m_isNewFrameStarted = false;

	g->tm.AfterNewFrame();

	g->stp.PopMatrix();
	
	DWORD time = timeGetTime();
	GLFUNC(SwapBuffers)(ghDC);
	DWORD time2 = timeGetTime();

	if ((time2 - time) > 100)
			g->lw.WriteLn("SwapBuffers execution time > 100 and = ", time2- time);

	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		ReleaseDC( ghWnd, ghDC);
	}


	if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
	{
		g->tm.Update();
		g->mm.Update();
	
		g->motm.Update();
		g->skom.Update();
		g->dc.Update();
	}

	ShowFPSIfNeeded();
}

void CNEngine::AddFPS(int fps)
{
	int i = FPS_SECONDS;
	for (i = FPS_SECONDS - 1; i >= 1; i--)
	{
		m_fpsData[i] = m_fpsData[i - 1];
	}

	m_fpsData[i] = fps;
}

unsigned int CNEngine::GetAverageFPS()
{
	unsigned int secondsCount = 0;
	unsigned int fpsSum = 0;

	for (int i = 0; i < FPS_SECONDS; i++)
	if (m_fpsData[i] != -1)
	{
		fpsSum += m_fpsData[i];
		secondsCount++;
	}

	if (secondsCount != 0)
	{
		return (fpsSum / secondsCount);
	}
	else
	{
		return 0;
	}
}

void CNEngine::ShowFPSIfNeeded()
{
	m_frameCount++;

	if (m_clockCount < 0)
	{
		m_clockCount = 0;
	}

	if (m_clockCount >= 1000)
	{
		m_currentFPS = m_frameCount;

		if (m_isShowFPSNeeded)
		{
			if (m_titleString.empty())
			{
				char tmp[501];
				GetWindowText(ghWnd, tmp, 500);
				m_titleString = tmp;
			}

			std::string fpsString = m_titleString;
			fpsString += " [fps: ";
			fpsString += IntToStr((int)m_frameCount);
			fpsString += "]";
			SetWindowText(ghWnd, fpsString.c_str());				
		}
		else if (!m_titleString.empty())
		{
			SetWindowText(ghWnd, m_titleString.c_str());
			m_titleString = "";
		}

		AddFPS(m_frameCount);

		m_clockCount -= 1000;
		m_frameCount = 0;
	}	
}

// returns current fps
unsigned int CNEngine::GetCurrentFPS()
{
	return m_currentFPS;
}

void CNEngine::End()
{
	/*m_dm0.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
	ChangeDisplaySettingsEx(NULL, &m_dm0, NULL, 0, NULL;)*/
}

void CNEngine::SetBackgroundColor(float backgroundR, float backgroundG, float backgroundB, float backgroundA)
{
	m_backgroundR = backgroundR;
	m_backgroundG = backgroundG;
	m_backgroundB = backgroundB;
	m_backgroundA = backgroundA;
}

void CNEngine::PrepareForDestroy()
{	
	g->taskm.Destroy();	
}		

void CNEngine::SetShowFPSNeededStatus(bool isShowFPSNeeded)
{
	m_isShowFPSNeeded = isShowFPSNeeded;
}

bool CNEngine::IsShowFPSNeeded()const
{
	return m_isShowFPSNeeded;
}

CNEngine::~CNEngine()
{
	End();
}
