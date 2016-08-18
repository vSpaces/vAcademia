
#pragma once

#include "../CommonRenderManagerHeader.h"

#include "BaseAsynchLoader.h"
#include "iasynchresourcehandler.h"

class CPrimObjectsLoader : public CBaseAsynchLoader
{
public:
	CPrimObjectsLoader();
	~CPrimObjectsLoader();

	bool LoadPrimObject(std::string& fileName, std::string& objName, IAsynchResource** ppAsynchResource);

	void OnAsynchResourceLoaded(IAsynchResource* asynch);

private:
	MP_MAP<IAsynchResource*, MP_STRING> m_objectNameByAsynch;
};