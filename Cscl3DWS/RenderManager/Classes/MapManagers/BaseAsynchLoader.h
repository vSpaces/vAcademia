
#pragma once

#include "CommonRenderManagerHeader.h"

class CMapManager;

class CBaseAsynchLoader: public IAsynchResourceHandler
{
public:
	CBaseAsynchLoader();
	~CBaseAsynchLoader();

	IAsynchResource* LoadObject(std::string& fileName);

	std::string PopLoadingObject(IAsynchResource* asynch);
	void AddLoadingObjectIfNeeded(IAsynchResource* pAsynchResource, std::string& name);
	bool IsObjectLoading(std::string& name);

	void AddLoadedObject(std::string& name);
	bool IsObjectLoaded(std::string& name);
	bool IsObjectNotExists(std::string& name);

	void OnAsynchResourceLoadedPersent(IAsynchResource* asynch, byte percent);
	void OnAsynchResourceError(IAsynchResource* asynch);

	void ClearLoadingErrors();

private:
	MP_MAP<MP_STRING, int> m_loadedObjects;
	MP_MAP<MP_STRING, int> m_loadingMap;
	MP_MAP<IAsynchResource*, MP_STRING> m_loadingObjects;
};