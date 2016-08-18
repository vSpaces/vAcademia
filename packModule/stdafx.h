// stdafx.h

#pragma once

/*#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;*/

#define COMMAN_EXPORTS
#define _SECURE_SCL 0
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <atlbase.h>
#include "../rtl/rtl.h"
//#include "Common.h"
//#include "globals.h"
//#include "common_globals.h"
//#include "time.h"
//#include "assert.h"
//#include <shellapi.h>
//#include <mmsystem.h>
//#include <WINDOWSX.H>

#include <crtdbg.h>

#define MAX_PATH2          3120

/*#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif*/

/*void correctPath( CComString& aPath);
void correctRemotePath( CComString& aPath);
HMODULE LoadDLL( LPCSTR alpcRelPath, LPCSTR alpcName);*/

// TODO: reference additional headers your program requires here