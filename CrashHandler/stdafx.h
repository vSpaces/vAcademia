// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "std_define.h"
#undef MDUMP_STATIC_LIB

#ifdef WIN32
/*
* Allow use of functions and symbols first defined in Win2k.
*/
#if !defined(WINVER) || (WINVER < 0x0500)
#undef WINVER
#define WINVER 0x0500
#endif
#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0500)
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#endif /* WIN32 */

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include "VacademiaDump.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
