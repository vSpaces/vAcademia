// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0500
#endif*/

// Windows Header Files:
#include <windows.h>
#include <wtypes.h>
#include <assert.h>
#include <atldef.h>
#include <atltrace.h>
#include "rtl.h"

#define VOIPMANCLIENT_BUILD
#include "../../common/memprofiling.h"
#undef VOIPMANCLIENT_BUILD

#include "include\VoipManClient.h"

#define USE_MUMBLE_SDK

/*#ifndef USE_MUMBLE_SDK
#define	USE_CONAITO_SDK_LIB
#endif

#ifdef USE_CONAITO_SDK_LIB
	#define	CONAITO_SDK_LIB_VERSION 3
#endif*/

// functions
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }

#define BUILD_H 1
#define HAS_AEC 1
//////////////////////////////////////////////////////////////////////////
// �� ��������� ����������� ������ � ����� ����� ��� ������ bResult = SetUserPosition( nUserID, userPos.x, userPos.y, userPos.z);
//#define VOIPMAN_BUILD_L 1


using namespace voip;

#include "include\errors.h"

//////////////////////////////////////////////////////////////////////////
// OpenH323 project
#ifdef USE_OPENH323_LIB
#include "AvSoundLib.h"
#include "VoIPEndPoint.h"
//#include "include\OpenH323\VoipManagerH323.h"
#endif


// TODO: reference additional headers your program requires here
