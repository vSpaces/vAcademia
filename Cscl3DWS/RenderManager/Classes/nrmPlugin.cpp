// nrmPlugin.cpp: implementation of the nrmPlugin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmPlugin.h"
#include "avatareditor.h"

static unsigned int g_plugCount = 0;

nrmPlugin::nrmPlugin(mlMedia* apMLMedia): 
	nrmObject(apMLMedia),
	m_rmbaseplugin(NULL),
	m_pOwnerMedia(NULL),
	m_isNotDLL(false),
	hDll(0),
	MP_STRING_INIT(sUID)
{
}

// загрузка нового ресурса
bool nrmPlugin::SrcChanged()
{
	USES_CONVERSION;

	if (!mpMLMedia)
	{
		return false;
	}

	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	if (!pwcSrc) 
	{
		return false;
	}

	mlString sSRC = pwcSrc; 
	if (sSRC.empty()) 
	{
		return false;
	}

	mlIPlugin*	pmlIPlugin = GetMLMedia()->GetIPlugin();
	assert(pmlIPlugin);
	const wchar_t*	wsUID = pmlIPlugin->GetUID();
	sUID = W2A(wsUID);

	return LoadLibrary(pwcSrc);
}

bool nrmPlugin::LoadLibrary(const wchar_t* pwcSrc)
{
	if (pwcSrc == NULL)	
	{
		return false;
	}

	CWComString	fullPath = pwcSrc;
	fullPath.MakeLower();

	if (fullPath.Find(L"avatareditor") != -1)
	{
		m_isNotDLL = true;
		return true;
	}

	/*ATLASSERT(m_pRenderManager);
	if (m_pRenderManager->GetResourceManager())
	{
		wchar_t*	pwcSrcFull = m_pRenderManager->GetResourceManager()->GetFullPath(pwcSrc);
		fullPath = CComString( W2A(pwcSrcFull));
	}*/

	if (!fullPath.IsEmpty())
	{
		hDll = ::LoadLibraryW( fullPath);
		if (hDll == NULL)
		{
//			int ier = GetLastError();
			rm_trace("Library %s not found", cLPCSTR(pwcSrc));
			hDll = 0;
			return false;
		}
		
		return true;
	}

	return false;
}

DWORD nrmPlugin::GetSupportedObjTypes()
{
	return 0;
}

HMODULE nrmPlugin::GetHMODULE()
{
	return hDll;
}

// calls from rmml
omsresult nrmPlugin::Call(char* /*apProcName*/, int aiArgC, va_list aArgsV, unsigned long &aRes)
{
	if (m_rmbaseplugin == 0) 
	{
		return OMS_ERROR_NOT_INITIALIZED;
	}

	aRes = m_rmbaseplugin->ProcessMessage(aiArgC, aArgsV);

	return OMS_OK;
}

// calls from plugin
bool nrmPlugin::Call(char* apProcName, char* aArgTypes, ...)
{
	if (mpMLMedia)
	{
		va_list ptr;
		va_start(ptr, aArgTypes);
		omsresult res = mpMLMedia->GetIPlugin()->Call(apProcName, aArgTypes, ptr);
		va_end(ptr);

		return OMS_SUCCEEDED(res);
	}
	else
	{
		return false;
	}	
}

bool nrmPlugin::Call(char* apProcName, char* aArgTypes, va_list ptr)
{
	if (mpMLMedia)
	{
		omsresult res = mpMLMedia->GetIPlugin()->Call(apProcName, aArgTypes, ptr);
		return OMS_SUCCEEDED(res);
	}
	else
	{
		return false;
	}
}

void nrmPlugin::SetOwner(moMedia* ap_Owner)
{
	m_pOwnerMedia = ap_Owner;
}

void nrmPlugin::InitializeDll()
{
	if ((!m_isNotDLL) && (GetHMODULE() == 0))
	{
		return;
	}

	USES_CONVERSION;

	if (!m_isNotDLL)
	{
		bool (*lpfnCreateInstance)(char*, IScriptCaller*, IRMBasePlugin**);
		(FARPROC&)lpfnCreateInstance = GetProcAddress(GetHMODULE(), "CreateInstance");
		if( lpfnCreateInstance == NULL)
		{
			rm_trace("Plugin doesn`t implement main function [CreateInstance] \n");
			return;
		}
		if( !lpfnCreateInstance((char *)sUID.c_str(), static_cast<IScriptCaller*>(this), &m_rmbaseplugin) || m_rmbaseplugin==NULL)
		{
			rm_trace("Plugin main function [CreateInstance] execution failed\n");
			return;
		}
	}
	else
	{
		CreateAvatarEditorInstance(static_cast<IScriptCaller*>(this), &m_rmbaseplugin);				
	}
	
	m_rmbaseplugin->SetResponder(static_cast<IScriptCaller*>(this));
}

void nrmPlugin::UninitializeDll()
{
	if ((!m_isNotDLL) && (GetHMODULE() == 0))
	{
		return;
	}

	if (!m_isNotDLL)
	{
		void (*lpfnDestroyInstance)(IScriptCaller*);
		(FARPROC&)lpfnDestroyInstance = GetProcAddress(GetHMODULE(), "DestroyInstance");
		if( !lpfnDestroyInstance)
		{
			rm_trace("Plugin doesn`t implement main function [DestroyInstance] \n");
			return;
		}
		lpfnDestroyInstance(static_cast<IScriptCaller*>(this));
	}
	else
	{
		DestroyAvatarEditorInstance(static_cast<IScriptCaller*>(this));
	}
}
			
bool	nrmPlugin::IsLoaded()
{
	return (((hDll != 0) ? true : false) || (m_isNotDLL));
}

void	nrmPlugin::SetUID(LPCTSTR asUID)
{
	sUID = asUID;
}

nrmPlugin::~nrmPlugin()
{
	UninitializeDll();
	if (hDll != 0)
	{
		FreeLibrary(hDll);
	}
}