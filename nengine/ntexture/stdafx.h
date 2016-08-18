// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include "std_define.h"

#include <windows.h>
#include <stdio.h>

#pragma  warning (disable: 4239)
#pragma  warning (disable: 4505)

// TODO: reference additional headers your program requires here
#define NTEXTURE_BUILD
#include "..\..\common\memprofiling.h"
#undef NTEXTURE_BUILD