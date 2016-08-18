#pragma once

#include <windows.h>
#include "IFlash.h"
#include "IDynamicTexture.h"
#include <vector>
#include "iasynchresourcemanager.h"
#include <string>
#include "npapi.h"
#include "npfunctions.h"
#include "npruntime_impl.h"

// natscape-плагин
// (создается и иницаилизируется при загрузке этой dll-ки)
class CFlashNP
{
friend class CFlash;
	HMODULE m_module;
	int miMajorFlashVersion;
	NPP_ShutdownProcPtr m_NPP_Shutdown;
	NPPluginFuncs m_pluginFuncs;
	NPNetscapeFuncs m_browserFuncs;

public:
	CFlashNP();
	~CFlashNP();

public:
	IFlash* CreateInstance(IFlashCallBack* apCallBack, HWND ahwndParentWindow, IResLibrary* apResLibrary);
	void DestroyInstance(IFlash* apInstance);

private:
	int LoadFlashPlugin();
};

extern CFlashNP gFlashNP;

extern const char* const errorStrings[];

#define LOG_NPERROR(err) if (err != NPERR_NO_ERROR) OutputDebugString(errorStrings[err]); OutputDebugString("\n");
