// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _SECURE_SCL 0
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#include <assert.h>
#include <stdio.h>
#include <memory.h>
#include <windows.h>
#include <atlbase.h>
#include "../../common/comstring.h"
#include "Command.h"
#include <string>
#include <vector>
#include "CommandManager.h"
#include "CommonPacketIn.h"
#include "GettingLogRecordsIn.h"
#include "oms_context.h"

using namespace SyncManProtocol;
using namespace Protocol;
using namespace rmml;
using namespace std;
using namespace sync;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif

// TODO: reference additional headers your program requires here
