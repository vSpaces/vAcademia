// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif 

#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <shlwapi.h>
#include <atldef.h>
#include "ComString.h"

#define	PROTOCOL_PREFIX_WITHOUT_SLASH _T("vacademia")
#define	PROTOCOL_PREFIX _T("vacademia://")
#define	PROTOCOL_PREFIX_ALT _T("goworld://")

// TODO: reference additional headers your program requires here
