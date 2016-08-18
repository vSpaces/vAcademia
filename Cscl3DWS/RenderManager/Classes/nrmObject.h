// nrmObject.h: interface for the nrmObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmObject_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmObject_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "../CommonRenderManagerHeader.h"

#include "nrmanager.h"
#include "../include/rmIPlGetProps.h"

class		nrmPlugin;
interface	IPluginNotifications;

//////////////////////////////////////////////////////////////////////////
class nrmObject :	public moMedia,
					public moILoadable,
					public IPropertiesContainer,
					public IAnyDataContainer,
					public IVisibilityProvider
{
public:
	nrmObject(mlMedia* apMLMedia);
	virtual ~nrmObject();

public:
	bool				IsVisible();

	bool				IsSet(const char*);
	int					GetPropType(const char*);
	int					GetIntProp(const char*);
	bool				GetBooleanProp(const char*);
	double				GetDoubleProp(const char*);
	wchar_t*			GetStringProp(const char*);
	IAnyDataContainer*	GetRefProp(const char*);
	void				SetProp(const char*, int);
	void				SetProp(const char*, double);
	void				SetProp(const char*, const char*);
	void				SetProp(const char*, const wchar_t*);

// реализация  ianydatacontainer
	virtual	void* GetMediaPointer();
	virtual void* GetData(const char* /*lpcTag*/){ return NULL;}
	virtual long  GetData(const char* /*lpcTag*/, void* /*buffer*/, long /*count*/){ return NULL;}

// реализация moMedia
public:
	virtual	moILoadable* GetILoadable(){ return this; }
	virtual	void PropIsSet();
	virtual void FrozenChanged()
	{	
	}

// реализация Plugins
public:
	void ScanPlugins();
	void FreePlugins();
	bool InitializePlugin(nrmPlugin* plugin);
	virtual bool InitializePlugins();

// реализация  moILoadable
public:
	void SetCompLib(CRMCompLib* cl);
	virtual	bool SrcChanged(){ return true;} // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src
	void clear();
	virtual	void on_clear(){};
	virtual	void LoadingPriorityChanged( ELoadingPriority /*aPriority*/){}

public:
	virtual	BOOL	SaveMedia(const wchar_t* /*apwcPath*/){ return FALSE;}
	virtual	void	SetRenderManager(CNRManager* apManager);
	virtual	void	OnSetRenderManager(){}
	CNRManager*		GetRenderManager(){ return m_pRenderManager; }

protected:
	CRMCompLib*								__pCompLib;
	nrmPlugin*								pExternalLoader;
	MP_VECTOR<nrmPlugin*>					vPlugins;
	CNRManager*								m_pRenderManager;
};

#endif // !defined(AFX_nrmObject_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
