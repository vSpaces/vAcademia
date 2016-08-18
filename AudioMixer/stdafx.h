// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include "std_define.h"
#define AUIDOMIXER_BUILD
#include "..\common\memprofiling.h"
#undef AUIDOMIXER_BUILD

#include "rtl.h"

//#include "targetver.h"
#include "IAudioMixer.h"


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here
