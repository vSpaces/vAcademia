// stdafx.h

#pragma once

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif

#include <malloc.h>

#define AUDIERE_BUILD 
#include "..\..\common\memprofiling.h"
#undef AUDIERE_BUILD
