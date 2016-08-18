
#include "StdAfx.h"
#include "GLApplication.h"
#include <windowsx.h>

static LRESULT fy_OnChar(HWND hwnd, TCHAR ch, int cRepeat)
{
	switch (ch)
	{
	case VK_ESCAPE:
		PostMessage(hwnd, WM_CLOSE, 0,0);
		break;
	}

	return 0;
}
 
LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	LONG lRet = 1;

    switch (uMsg) 
	{ 
	HANDLE_MSG(hWnd, WM_CHAR, fy_OnChar);
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			break;

		case VK_DOWN:
			break;

		case VK_LEFT:
			break;

		case VK_RIGHT:
			break;
		}
		break;
 
    case WM_SIZE: 
        break; 
 
    case WM_DESTROY: 
        PostQuitMessage (0); 
        break; 
     
    default: 
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet; 
} 

CGLApplication::CGLApplication(std::string name, unsigned int width, unsigned int height, bool isVSyncEnabled):
	m_name(name),
	m_width(width),
	m_height(height),
	m_isVSyncEnabled(isVSyncEnabled)
{
}

int CGLApplication::WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{ 
	CHAR szAppName[] = "Win OpenGL"; 

	MSG        msg; 
	WNDCLASS   wndclass; 
    wndclass.style         = 0; 
    wndclass.lpfnWndProc   = (WNDPROC)MainWndProc; 
    wndclass.cbClsExtra    = 0; 
    wndclass.cbWndExtra    = 0; 
    wndclass.hInstance     = hInstance; 
    wndclass.hIcon         = LoadIcon (hInstance, szAppName); 
    wndclass.hCursor       = LoadCursor (NULL,IDC_ARROW); 
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); 
    wndclass.lpszMenuName  = szAppName; 
    wndclass.lpszClassName = szAppName; 
    if (!RegisterClass (&wndclass)) 
	{
        return FALSE; 
	}
 
    m_hwnd = CreateWindowEx(0, szAppName, m_name.c_str(), WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, m_width, m_height,
		NULL, NULL, hInstance, NULL);

	if (!m_hwnd) 
	{
		MessageBox(NULL, "Create window failed", "Error", MB_OK); 
        return FALSE; 
	}
 
    ShowWindow(m_hwnd, nCmdShow); 
    UpdateWindow(m_hwnd);

	if (!SetupPixelFormat())
	{
		return FALSE;
	}

	InitGL();
	OnLoading();

	SetWindowPos(m_hwnd, NULL, -200, -200, 0, 0, SW_SHOW);

	while (true) 
	{ 
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
        { 
            if (GetMessage(&msg, NULL, 0, 0) ) 
            { 
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            } 
			else
			{
                return TRUE; 
			}
        }
		
		if (!DrawScene())
		{
			return 0;
		}
		SwapBuffers(m_hdc);
    }
	
	return 0;
}

bool CGLApplication::SetupPixelFormat() 
{
	m_hdc = GetDC(m_hwnd);

    PIXELFORMATDESCRIPTOR pfd, *ppfd; 
    int pixelformat; 
 
    ppfd = &pfd; 
 
    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR); 
    ppfd->nVersion = 1; 
    ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |  
                        PFD_DOUBLEBUFFER; 
    ppfd->dwLayerMask = PFD_MAIN_PLANE; 
    ppfd->iPixelType = PFD_TYPE_RGBA; 
    ppfd->cColorBits = 8; 
    ppfd->cDepthBits = 16; 
    ppfd->cAccumBits = 0; 
    ppfd->cStencilBits = 0; 
 
    pixelformat = ChoosePixelFormat(m_hdc, ppfd); 
 
    if ((pixelformat = ChoosePixelFormat(m_hdc, ppfd)) == 0) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return false; 
    } 
 
    if (SetPixelFormat(m_hdc, pixelformat, ppfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return false; 
    } 
 
    return true; 
} 

void CGLApplication::InitGL()
{
    m_glRC = wglCreateContext(m_hdc); 
    wglMakeCurrent(m_hdc, m_glRC); 

//	glewInit();

	glEnable(GL_TEXTURE_2D);
}

HDC CGLApplication::GetHDC()const
{
	return m_hdc;
}

HWND CGLApplication::GetHWND()const
{
	return m_hwnd;
}

HGLRC CGLApplication::GetHGLRC()const
{
	return m_glRC;
}

CGLApplication::~CGLApplication()
{
}