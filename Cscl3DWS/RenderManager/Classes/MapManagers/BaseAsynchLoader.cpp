
#include "StdAfx.h"
#include "MapManager.h"
#include "iasynchresource.h"
#include "BaseAsynchLoader.h"
#include "iasynchresourcemanager.h"
#include "RMContext.h"

CBaseAsynchLoader::CBaseAsynchLoader():
	MP_MAP_INIT(m_loadedObjects),
	MP_MAP_INIT(m_loadingMap),
	MP_MAP_INIT(m_loadingObjects)
{
}

bool CBaseAsynchLoader::IsObjectLoaded(std::string& name)
{
	MP_MAP<MP_STRING, int>::iterator iter = m_loadedObjects.find(MAKE_MP_STRING(name));

	if (iter == m_loadedObjects.end())
	{
		return false;
	}

	return ((*iter).second == 1);
}

bool CBaseAsynchLoader::IsObjectNotExists(std::string& name)
{
	MP_MAP<MP_STRING, int>::iterator iter = m_loadedObjects.find(MAKE_MP_STRING(name));

	if (iter == m_loadedObjects.end())
	{
		return false;
	}

	return ((*iter).second == CLASS_NOT_EXISTS);
}

bool CBaseAsynchLoader::IsObjectLoading(std::string& name)
{
	MP_MAP<MP_STRING, int>::iterator iter = m_loadingMap.find(MAKE_MP_STRING(name));
	return (iter != m_loadingMap.end());
}

void CBaseAsynchLoader::AddLoadingObjectIfNeeded(IAsynchResource* pAsynchResource, std::string& name)
{
	MP_MAP<IAsynchResource*, MP_STRING>::iterator it = m_loadingObjects.find(pAsynchResource);
	if (it == m_loadingObjects.end())
	{
		m_loadingObjects.insert(MP_MAP<IAsynchResource*, MP_STRING>::value_type(pAsynchResource, MAKE_MP_STRING(name)));
		m_loadingMap.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(name), 1));
	}
}

void CBaseAsynchLoader::AddLoadedObject(std::string& name)
{
	m_loadedObjects.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(name), 1));
}

IAsynchResource*  CBaseAsynchLoader::LoadObject(std::string& fileName)
{
	IAsynchResource* pAsynchResource = gRM->resLib->GetAsynchResMan()->GetAsynchResource(fileName.c_str(), false, RES_TYPE_BMF, (IAsynchResourceHandler*)this, gRM->resLib);
	if (pAsynchResource)
	{
		pAsynchResource->SetScriptPriority(ELP_XML);
		gRM->mapMan->UpdateResourceLoadingPriority(pAsynchResource);
	}

	return pAsynchResource;
}

std::string CBaseAsynchLoader::PopLoadingObject(IAsynchResource* asynch)
{
	MP_MAP<IAsynchResource*, MP_STRING>::iterator it = m_loadingObjects.find(asynch);

	if (it == m_loadingObjects.end())
	{
		return "";
	}
	
	std::string name = (*it).second;
	MP_MAP<MP_STRING, int>::iterator iter = m_loadingMap.find((*it).second);
	assert(iter != m_loadingMap.end());
	m_loadingObjects.erase(it);
	m_loadingMap.erase(iter);

	return name;
}

void CBaseAsynchLoader::OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, byte /*percent*/)
{
}

void CBaseAsynchLoader::OnAsynchResourceError(IAsynchResource* asynch)
{
	MP_MAP<IAsynchResource*, MP_STRING>::iterator it = m_loadingObjects.find(asynch);

	if (it != m_loadingObjects.end())
	{
		m_loadedObjects.insert(MP_MAP<MP_STRING, int>::value_type((*it).second, CLASS_NOT_EXISTS));
	}
}

void CBaseAsynchLoader::ClearLoadingErrors()
{
	bool isFound = true;

	while (isFound)
	{
		isFound = false;

		MP_MAP<MP_STRING, int>::iterator it = m_loadedObjects.begin();
		MP_MAP<MP_STRING, int>::iterator itEnd = m_loadedObjects.end();

		for ( ; it != itEnd; it++)
		if ((*it).second == CLASS_NOT_EXISTS)
		{
			m_loadedObjects.erase(it);
			isFound = true;
			break;
		}
	}	
}

CBaseAsynchLoader::~CBaseAsynchLoader()
{
}