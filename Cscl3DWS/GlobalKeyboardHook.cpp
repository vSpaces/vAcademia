#include "StdAfx.h"
#include "GlobalKeyboardHook.h"
#include "oms_context.h"

static HHOOK hHook = 0;
static bool kbCtrlPressed = false;
static bool kbQPressed = false;
static bool kbWPressed = false;
static oms::omsContext *pGlobalHookOMSContext = NULL;

#define WH_KEYBOARD_LL     13
#define LLKHF_UP             (KF_UP >> 8)

#if _MSC_VER < 1600
typedef struct tagKBDLLHOOKSTRUCT {
	DWORD   vkCode;
	DWORD   scanCode;
	DWORD   flags;
	DWORD   time;
	ULONG_PTR dwExtraInfo;
} KBDLLHOOKSTRUCT;
#endif

LRESULT CALLBACK HookProc(int code, WORD wParam, long lParam)
{
	KBDLLHOOKSTRUCT* pKBDLLHOOKSTRUCT = (KBDLLHOOKSTRUCT*)lParam;

	bool needCheck = false;
	bool isKeyDown = (pKBDLLHOOKSTRUCT->flags & LLKHF_UP) == 0 ? true : false;
	if( pKBDLLHOOKSTRUCT->vkCode == VK_LCONTROL)
	{
		kbCtrlPressed = isKeyDown;
		needCheck = true;
	}
	if( pKBDLLHOOKSTRUCT->scanCode == 16)
	{
		kbQPressed = isKeyDown;
		needCheck = true;
	}
	else if( pKBDLLHOOKSTRUCT->scanCode == 17)
	{
		kbWPressed = isKeyDown;
		needCheck = true;
	}

	if( needCheck)
	{
		if( kbCtrlPressed && kbQPressed)
		{
			if( pGlobalHookOMSContext && pGlobalHookOMSContext->mpApp)
			{
				pGlobalHookOMSContext->mpApp->HandleDebugBreakKeyboardRequest();
				kbCtrlPressed = false;
				kbQPressed = false;
			}
		}
		else if (kbCtrlPressed && kbWPressed)
		{
			if( pGlobalHookOMSContext && pGlobalHookOMSContext->mpApp)
			{
				//pGlobalHookOMSContext->mpApp->HandleMakeDumpKeyboardRequest();
				kbCtrlPressed = false;
				kbWPressed = false;
			}
		}
	}
	
	return CallNextHookEx( hHook, code, wParam, lParam);
}

CGlobalKeyboardHook::CGlobalKeyboardHook(HINSTANCE ahInstance, oms::omsContext* apOmsContext)
{
	hInstance = ahInstance;
	pGlobalHookOMSContext = apOmsContext;
}

CGlobalKeyboardHook::~CGlobalKeyboardHook()
{
	Stop();
}

DWORD CGlobalKeyboardHook::Run()
{
	hHook = SetWindowsHookEx( WH_KEYBOARD_LL, (HOOKPROC)&HookProc, hInstance, NULL);

	MSG msg;
	HACCEL hAccelTable = 0;
	while ( !ShouldStop()) 
	{
		while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);						
			}
		}
	}

	return 0;
}