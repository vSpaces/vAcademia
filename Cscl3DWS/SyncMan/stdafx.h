// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "std_define.h"
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define SYNCMAN_BUILD
#include "..\..\common\memprofiling.h"
#undef SYNCMAN_BUILD

#include <atlbase.h>
#include "..\..\common/CriticalSectionGuard.h"

#include <crtdbg.h>
#include <vector>
#include <string>

#include "rtl.h"

#include "DataBuffer.h"
#include "mlSynchState.h"
#include "ProtocolConsts.h"
#include "CommonPacketOut.h"
#include "CommonPacketIn.h"
#include "SyncMan.h"

using namespace Protocol;
using namespace rmml;
using namespace std;
using namespace sync;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif

// TODO: reference additional headers your program requires here