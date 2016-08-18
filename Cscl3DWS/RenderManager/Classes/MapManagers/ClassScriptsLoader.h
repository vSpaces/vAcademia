
#pragma once

#include "CommonRenderManagerHeader.h"

#include "MapManagerZone.h"
#include "iasynchresourcehandler.h"
#include "BaseAsynchLoader.h"

#define INCLUDES_LOADING		0
#define INCLUDES_LOADED			1
#define INCLUDES_NOT_EXISTS		2

class CClassScriptsLoader : public CBaseAsynchLoader
{
public:
	CClassScriptsLoader();
	~CClassScriptsLoader();

	int LoadClass(std::string& className);
	bool IsLocalClass(std::string& className)	const;

	void OnAsynchResourceLoaded(IAsynchResource* asynch);

private:
	typedef std::vector<std::string> TIncludesMapValue;

	std::string GetClassFileName(std::string& className);
	int			GetIncludesLoadedState(std::string& fileName);
	int			GetIncludesLoadedStateImpl(std::string& fileName, TIncludesMapValue& recursiveIncludesFilter);
	void		ExtractAndLoadIncludes( std::string fileName, IAsynchResource* asynch);
	void		BuildJSClassIfNeeded(std::string fileName);

	MP_MAP<MP_STRING, int> m_localClassesMap;

	MP_MAP<MP_STRING, int>					m_includedFilesLoadingStateVec;
	MP_MAP<MP_STRING, TIncludesMapValue>	m_includesPerFileMap;
};