// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define SOUNDMAN_EXPORTS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#include "std_define.h"
/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlwin.h>

#define SOUNDMAN_BUILD 
#include "..\common\memprofiling.h"
#undef SOUNDMAN_BUILD 


#include "rtl.h"
#include "audiere.h"
#include "isound.h"

typedef unsigned char UCHAR;
typedef unsigned char* LPUCHAR;
typedef unsigned short USHORT;
typedef unsigned short* LPUSHORT;

// TODO: reference additional headers your program requires here
