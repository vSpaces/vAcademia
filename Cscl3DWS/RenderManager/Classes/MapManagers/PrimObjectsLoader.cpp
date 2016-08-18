
#include "StdAfx.h"
#include "MapManager.h"
#include "iasynchresource.h"
#include "PrimObjectsLoader.h"

CPrimObjectsLoader::CPrimObjectsLoader():
	MP_MAP_INIT(m_objectNameByAsynch)
{
}

bool CPrimObjectsLoader::LoadPrimObject(std::string& fileName, std::string& objName, IAsynchResource** ppAsynchResource)
{
	assert(ppAsynchResource);
	if (!IsObjectLoaded(fileName))
	{
		IAsynchResource* pAsynchResource = LoadObject(fileName);
		*ppAsynchResource = pAsynchResource;
		if (pAsynchResource == NULL)
		{
			return true;
		}

		// Убрать потом
		pAsynchResource->SetPriority(3000);
 
		AddLoadingObjectIfNeeded(pAsynchResource, fileName);
		m_objectNameByAsynch.insert(MP_MAP<IAsynchResource*, MP_STRING>::value_type(pAsynchResource, MAKE_MP_STRING(objName)));

		return false;
	}
	else
	{
		return true;
	}
}

void CPrimObjectsLoader::OnAsynchResourceLoaded(IAsynchResource* asynch)
{
	std::string fileName = PopLoadingObject(asynch);
	MP_MAP<IAsynchResource*, MP_STRING>::iterator it = m_objectNameByAsynch.find(asynch);

	if ((fileName.size() == 0) || (it == m_objectNameByAsynch.end()))
	{
		assert(FALSE);
	}
	else
	{
		std::string objName = (*it).second;
		AddLoadedObject(fileName);
	}
}

CPrimObjectsLoader::~CPrimObjectsLoader()
{
}