// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: rmext.h,v $
// $Revision: 1.6 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2008/04/21 11:39:52 $ $Author: tandy $
//
//////////////////////////////////////////////////////////////////////

#ifndef __rmext_h___
#define __rmext_h___

#include <windows.h>
#include <string>

// Для импорта/экспорта классов и процедур
#if defined (OMS_RM_BUILD)
#  define RM_DECL  __declspec(dllexport)
#else                 // defined (OMS_RM_BUILD)
# define RM_DECL   __declspec(dllimport)
#endif                // defined (OMS_RM_BUILD)

#ifndef WIN32
#define RM_NO_VTABLE
#else
#define RM_NO_VTABLE __declspec(novtable)
//#define RM_NO_VTABLE
#endif

#include "../res/res.h"

namespace rm
{

struct RM_NO_VTABLE rmISoundManager;
struct RM_NO_VTABLE syncISyncManager;

struct RM_NO_VTABLE rmIRenderManagerAX
{
	virtual void SetSceneManager(rmml::mlISceneManager* amlISceneManager) = 0;
	virtual void SetResourceManager(res::resIResourceManager* aresIResourceManager) = 0;
	virtual void SetSoundManager(rm::rmISoundManager* asmISoundManager) = 0;
	virtual bool SetScreenMode(unsigned long adwWidth, unsigned long adwHeight, unsigned long adwBPP, bool abFullScreen) = 0;
	virtual void SetScreenModeForOculus(unsigned long adwWidth, unsigned long adwHeight, unsigned long x, unsigned long y) = 0;
	virtual bool SetControlWindow(HWND ahWnd, const RECT& arcRect) = 0;
	virtual int  GetFilesFromCFG(int aiType, char* apszCFGFileName, char* appszFileNames, int bufCount) = 0;
	virtual void UpdateAndRender() = 0;
	virtual void SaveBitmap(LPCTSTR fileName, HBITMAP hBitmap) = 0;
	virtual void SetCommunicationManager(cm::cmICommunicationManager* apComManager) = 0;
	virtual void StartLoadingResource() = 0;
};

}

#endif
