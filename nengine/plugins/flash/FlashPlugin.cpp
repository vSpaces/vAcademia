
#include "StdAfx.h"
#include "flash.h"
#include "FlashPlugin.h"
#include "FlashNP.h"

IFlash* CreateFlashObject(IFlashCallBack* apCallBack, HWND ahwndParentWindow, IResLibrary* apResLibrary)
{
	return gFlashNP.CreateInstance(apCallBack, ahwndParentWindow, apResLibrary);
//	CFlash* pFlash = new CFlash(apCallBack);

//	return pFlash;
}

void DeleteFlashObject(IFlash* apFlash)
{
	gFlashNP.DestroyInstance(apFlash);
//	if (apFlash != NULL){
//		CFlash* pFlash = (CFlash*)apFlash;
//		delete pFlash;
//	}
}