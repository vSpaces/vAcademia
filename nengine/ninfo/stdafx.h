
#pragma once
// Windows Header Files:
#include <windows.h>

#include "std_define.h"
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN 

#pragma  warning (disable: 4702)

#include "GlobalSingletonStorage.h"
#include "..\..\rtl\rtl.h"

#define NINFO_BUILD
#include "..\..\common\memprofiling.h"
#undef NINFO_BUILD


#pragma warning(disable : 4505)
