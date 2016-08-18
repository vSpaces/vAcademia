f// nrmObject.cpp: implementation of the nrmObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmObject.h"
#include "RMComp.h"
#include "nrmPlugin.h"


nrmObject::nrmObject(mlMedia* apMLMedia): 
	moMedia(apMLMedia),
	MP_VECTOR_INIT(vPlugins)
{
	pExternalLoader = NULL;
	__pCompLib = NULL;
	m_pRenderManager = NULL;
}

void nrmObject::clear()
{
	on_clear();
	FreePlugins();
	vPlugins.clear();
	//virmplugnotifications.clear();
}

void nrmObject::ScanPlugins()
{
	if (!GetMLMedia())
	{
		return;
	}

	std::vector<mlMedia*> avPlugRefs;
	avPlugRefs.reserve(255);
	GetMLMedia()->EnumPlugins(avPlugRefs);
	assert(avPlugRefs.size()<255);
	//
	std::vector<mlMedia*>::iterator it;
	for (it=avPlugRefs.begin();it!=avPlugRefs.end();it++) 
	{
		nrmPlugin*	m_pPlugin = (nrmPlugin*)(*it)->GetSafeMO();
		if (!m_pPlugin)	
		{
			continue;
		}

		std::vector<nrmPlugin*>::iterator it2;
		for (it2 = vPlugins.begin(); it2 != vPlugins.end(); it2++)
		if ((*it2) == m_pPlugin)	
		{
			break;
		}

		if (it2 != vPlugins.end())	
		{
			continue;
		}

		m_pPlugin->SetOwner(this);
		m_pPlugin->InitializeDll();
		vPlugins.push_back(m_pPlugin);
	}
}

void nrmObject::FreePlugins()
{
	// UninitializeDll() will be called in ~nrmPlugin
/*	std::vector<nrmPlugin*>::iterator it = vPlugins.begin();
	std::vector<nrmPlugin*>::iterator itEnd = vPlugins.end();
	for ( ; it != itEnd; it++)
	{
		assert(*it);
		if (*it)
		{
			(*it)->UninitializeDll();
		}
	}*/
	vPlugins.clear();
}

bool nrmObject::InitializePlugin(nrmPlugin*	plugin)
{
	if (plugin)
	{
		return (plugin->IsLoaded());
	}
	else
	{
		return false;
	}
}

bool nrmObject::InitializePlugins()
{
	return true;
}

void nrmObject::SetCompLib(CRMCompLib *cl)
{
	__pCompLib = cl;
}

void nrmObject::PropIsSet()
{
}

bool	nrmObject::IsVisible()
{
	if (!mpMLMedia)	
	{
		return false;
	}

	return mpMLMedia->GetIVisible()->GetAbsVisible();	
}

bool	nrmObject::IsSet(const char* param)
{
	if (!mpMLMedia)	
	{
		return false;
	}

	if (mpMLMedia->GetPropType(param) == MLPT_UNKNOWN)
	{
		return false;
	}

	return true;
}

int		nrmObject::GetPropType(const char* param)
{
	if (!mpMLMedia)	
	{
		return false;
	}

	return mpMLMedia->GetPropType(param);
}

int		nrmObject::GetIntProp(const char* param)
{
	if(mpMLMedia)
	{
		return mpMLMedia->GetIntProp(param);
	}

	return 0;
}

bool		nrmObject::GetBooleanProp(const char* param)
{
	if(mpMLMedia)
	{
		return mpMLMedia->GetBooleanProp(param);
	}

	return false;
}

double	nrmObject::GetDoubleProp(const char* param)
{
	if(mpMLMedia)
	{
		return mpMLMedia->GetDoubleProp(param);
	}

	return 0.0;
}

wchar_t*	nrmObject::GetStringProp(const char* param)
{
	if(mpMLMedia)
	{
		return mpMLMedia->GetStringProp(param);
	}

	return NULL;
}

IAnyDataContainer*	nrmObject::GetRefProp(const char* param)
{
	if(!mpMLMedia)	return NULL;
	mlMedia*	pRef = mpMLMedia->GetRefProp(param);
	if(!pRef)	return NULL;
	switch(pRef->GetType())
	{
		case MLMT_ANIMATION:
		case MLMT_CHARACTER:
		case MLMT_IMAGE:
		case MLMT_OBJECT:
		case MLMT_SCENE3D:
		case MLMT_GROUP:
		case MLMT_LIGHT:
		case MLMT_VIDEO:
		case MLMT_VIEWPORT:
		case MLMT_CAMERA:
		case MLMT_COMMINICATION_AREA:
		return (IAnyDataContainer*)((nrmObject*)pRef->GetSafeMO());
	}
	return NULL;
}

void	nrmObject::SetProp(const char* param, int val)
{
	if(mpMLMedia)
	{
		mpMLMedia->SetProp(param, val);
	}
}

void	nrmObject::SetProp(const char* param, double val)
{
	if(mpMLMedia)
	{
		mpMLMedia->SetProp(param, val);
	}
}

void	nrmObject::SetProp(const char* param, const char* val)
{
	if(mpMLMedia)
	{
		mpMLMedia->SetProp(param, val);
	}
}

void	nrmObject::SetProp(const char* param, const wchar_t* val)
{
	if(mpMLMedia)
	{
		mpMLMedia->SetProp(param, val);
	}
}

void*	nrmObject::GetMediaPointer()
{
	return NULL;
}

void	nrmObject::SetRenderManager(CNRManager* apManager)
{
	assert(!m_pRenderManager);
	m_pRenderManager = apManager;
	OnSetRenderManager();
}

nrmObject::~nrmObject()
{
	if(__pCompLib)
		__pCompLib->RemoveObject(GetMLMedia());
	clear();
}