// nrmPlugin.h: interface for the nrmPlugin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmPlugin_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmPlugin_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "../CommonRenderManagerHeader.h"

#include "nrmObject.h"
#include "../include/rmIPlugin.h"

//////////////////////////////////////////////////////////////////////////
class nrmCharacter;
class nrmObject;

//////////////////////////////////////////////////////////////////////////
class nrmPlugin :	public nrmObject,
					public moIPlugin,
					public IScriptCaller
{
	friend class moMedia;
public:
	nrmPlugin(mlMedia* apMLMedia);
	virtual ~nrmPlugin();

// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }
	moIPlugin* GetIPlugin(){ return this; }

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

public:
	void InitializeDll();
	void UninitializeDll();
	void SetOwner(moMedia* ap_Owner);
	HMODULE GetHMODULE();
	DWORD GetSupportedObjTypes();
	bool LoadLibrary(const wchar_t* pwcSrc);
	void SetUID(LPCTSTR asUID);

	omsresult Call(char* apProcName, int aiArgC, va_list aArgsV, unsigned long &aRes);

	bool Call(char* apProcName, char* aArgTypes, ...);
	bool Call(char* apProcName, char* aArgTypes, va_list ptr);

	bool			IsLoaded();
	IRMBasePlugin*	GetPluginInterface()
					{	return m_rmbaseplugin;	}

protected:
	moMedia*		m_pOwnerMedia;
	IRMBasePlugin*	m_rmbaseplugin;	
	HMODULE			hDll;
	MP_STRING		sUID;
	bool			m_isNotDLL;
};

#endif // !defined(AFX_nrmPlugin_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
