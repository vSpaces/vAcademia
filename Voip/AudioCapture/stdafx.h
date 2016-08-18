// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"

// Windows Header Files:
#include <windows.h>
#include <atlbase.h>

#define AUDIOCAPTURE_BUILD
#include "../../common/memprofiling.h"

// TODO: reference additional headers your program requires here
#define VOIPAUDIOCAPTURE_EXPORTS

#define SAFE_DELETE(p)  { if(p) { MP_DELETE (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

