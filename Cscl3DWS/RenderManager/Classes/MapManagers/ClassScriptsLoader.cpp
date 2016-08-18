
#include "StdAfx.h"
#include "XMLList.h"
#include "RMContext.h"
#include "iasynchresource.h"
#include "PlatformDependent.h"
#include "ClassScriptsLoader.h"
#include "MapManagerPriority.h"
#include <algorithm>

CClassScriptsLoader::CClassScriptsLoader():
	MP_MAP_INIT(m_localClassesMap)
	, MP_MAP_INIT(m_includedFilesLoadingStateVec)
	, MP_MAP_INIT(m_includesPerFileMap)
{
	CXMLList localClassesList(GetApplicationRootDirectory() + L"local_classes.xml", true);
	std::string className;
	while (localClassesList.GetNextValue(className))
	{
		m_localClassesMap.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(className), 1)); 
	}
}

std::string CClassScriptsLoader::GetClassFileName(std::string& className)
{
	if( className.find(".xml") != std::string::npos)
		return className;
	return ":scripts\\\\" + className + ".xml";
}

bool CClassScriptsLoader::IsLocalClass(std::string& className)const
{
	MP_MAP<MP_STRING, int>::const_iterator it = m_localClassesMap.find(MAKE_MP_STRING(className));
	return (it != m_localClassesMap.end());
}

/*
ƒобавл€ем такой функционал:
1. ѕосле загрузки файла с классом - открываем все его внутренние инклуды;
2. ƒл€ каждого инклуда создаем асинхронный ресурс
3. ѕосле загрузки файла с инклудом парсим его рекурсивно
4. ѕровер€ем что все инклуды загрузились
*/

int CClassScriptsLoader::LoadClass(std::string& className)
{
	//  ласс уже скомпилен
	if (gRM->mapMan->IsJSObjClassDefined(className.c_str()))
	{
		return CLASS_LOADED;
	}

	//  ласс локальный - загрузитс€
	if (IsLocalClass(className))
	{
		return CLASS_LOADING;
	}

	//  ласс не был загружен - уже ничто не поможет
	if (IsObjectNotExists(className))
	{
		return CLASS_NOT_EXISTS;
	}

	// если загружен сам и все инклуды - класс готов к созданию
	if ( IsObjectLoaded(className))
	{
		std::string classFileName = GetClassFileName(className);
		int auIncludesLoadedState = GetIncludesLoadedState( classFileName);

		switch ( auIncludesLoadedState)
		{
			case INCLUDES_NOT_EXISTS:
				return CLASS_NOT_EXISTS;

			case INCLUDES_LOADING:
				return CLASS_LOADING;

			case INCLUDES_LOADED:
			{
				std::string jsClassLoad = "Player.module.load(\"" + classFileName + "\");";
				gRM->mapMan->ExecJS(jsClassLoad.c_str());
				
				return CLASS_LOADED;
			}
		}
	}

	// если еще не загружаетс€ - добавим на загрузку
	if (!IsObjectLoading(className))
	{
		std::string classFileName = GetClassFileName(className);

		IAsynchResource* pAsynchResource = LoadObject(classFileName);
		if (pAsynchResource == NULL)
		{
			if (g->rl->GetAsynchResMan()->IsLocalScripts())
			{
				classFileName.erase(classFileName.begin(), classFileName.begin() + 9);
				classFileName = ":rmcommon:/scripts/server/scripts/" + classFileName;

				std::string jsClassLoad = "Player.module.load(\"" + classFileName + "\");";
				gRM->mapMan->ExecJS(jsClassLoad.c_str());			
			}

			return CLASS_LOADED;
		}

		pAsynchResource->SetPriority(PRIORITY_EXTREMELY_BIG);

		AddLoadingObjectIfNeeded(pAsynchResource, className);
	}

	return CLASS_LOADING;
}

void CClassScriptsLoader::OnAsynchResourceLoaded(IAsynchResource* asynch)
{
	std::string className = PopLoadingObject(asynch);
	
	if (className.size() == 0)
	{
		ATLASSERT(FALSE);
	}
	else
	{
		AddLoadedObject(className);

		// ?? если ресурс скачивалс€ с сервера, на не лежал уже в кэше, 
		// то стоит его прежде в другом потоке распарсить как XML, 
		// извлечь include-ы и так же загрузить их асинхронно
		std::string classFileName = GetClassFileName(className);

		ExtractAndLoadIncludes( classFileName, asynch);
	}
}

void	CClassScriptsLoader::ExtractAndLoadIncludes( std::string fileName, IAsynchResource* asynch)
{
	// уже был разобран
	if( m_includesPerFileMap.find(MAKE_MP_STRING(fileName)) != m_includesPerFileMap.end())
		return;

	// вытащим все инклуды верхнего уровн€
	std::vector<std::string>	includeFilesVector;
	gRM->mapMan->ExtractIncludes( asynch, includeFilesVector);

	// сопоставим все инклуды верхнего уровн€ текущему файлу
	m_includesPerFileMap.insert( MP_MAP<MP_STRING, TIncludesMapValue>::value_type(MAKE_MP_STRING(fileName), includeFilesVector));

	// запомним, что этот файл загружаетс€ (так как инклуды неизвестны)
	m_includedFilesLoadingStateVec.insert( MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(fileName), INCLUDES_LOADING));

	// пройдем по всем инклудам и поставим их на загрузку
	std::vector<std::string>::iterator it = includeFilesVector.begin(), end = includeFilesVector.end();
	for ( ; it!=end; it++)
	{
		MP_STRING includeFileName = MAKE_MP_STRING(*it);

		// если статус загрузки уже определен, пропустим
		if( m_includedFilesLoadingStateVec.find( includeFileName) != m_includedFilesLoadingStateVec.end())
			continue;

		if ( IsObjectNotExists(includeFileName))
		{
			m_includedFilesLoadingStateVec.insert( MP_MAP<MP_STRING, int>::value_type(includeFileName, INCLUDES_NOT_EXISTS));
			continue;
		}

		IAsynchResource* pAsynchResource = LoadObject(includeFileName);
		if (pAsynchResource == NULL)
		{
			m_includedFilesLoadingStateVec.insert( MP_MAP<MP_STRING, int>::value_type(includeFileName, INCLUDES_LOADED));
			continue;
		}

		pAsynchResource->SetPriority(PRIORITY_EXTREMELY_BIG);

		AddLoadingObjectIfNeeded(pAsynchResource, includeFileName);

		m_includedFilesLoadingStateVec.insert( MP_MAP<MP_STRING, int>::value_type(includeFileName, INCLUDES_LOADING));
	}
}

int		CClassScriptsLoader::GetIncludesLoadedState(std::string& fileName)
{
	TIncludesMapValue recursiveIncludesFilter;
	recursiveIncludesFilter.push_back( fileName);

	return GetIncludesLoadedStateImpl( fileName, recursiveIncludesFilter);
}

int		CClassScriptsLoader::GetIncludesLoadedStateImpl(std::string& fileName, TIncludesMapValue& recursiveIncludesFilter)
{
	MP_MAP<MP_STRING, int>::iterator loadedStateIt = m_includedFilesLoadingStateVec.find( MAKE_MP_STRING(fileName));
	if( loadedStateIt == m_includedFilesLoadingStateVec.end())
		return INCLUDES_LOADING;

	if( loadedStateIt->second != INCLUDES_LOADING)
		return loadedStateIt->second;

	MP_MAP<MP_STRING, TIncludesMapValue>::iterator it = m_includesPerFileMap.find( MAKE_MP_STRING(fileName));
	// не нашли инклуд в очереди на загрузку
	if( it == m_includesPerFileMap.end())
	{
		if( IsObjectNotExists( fileName))
			return INCLUDES_NOT_EXISTS;

		return INCLUDES_LOADING;
	}

	// если сюда пришло, то он loaded
	int recursiveLoadedResult = INCLUDES_LOADED;

	TIncludesMapValue::iterator itbegin = it->second.begin(), itend = it->second.end();
	for( ; itbegin != itend; itbegin++)
	{
		std::string includeName = *itbegin;

		TIncludesMapValue::iterator recursiveIt = std::find( recursiveIncludesFilter.begin(), recursiveIncludesFilter.end(), includeName);
		if( recursiveIt != recursiveIncludesFilter.end())
			continue;

		recursiveIncludesFilter.push_back( includeName);
		int oneIncludeLoadedResult = GetIncludesLoadedStateImpl( includeName, recursiveIncludesFilter);
		recursiveIncludesFilter.erase( recursiveIncludesFilter.begin() + recursiveIncludesFilter.size() - 1);

		// если хот€ бы один имеет статус INCLUDES_NOT_EXISTS - не беда, может просто лишний
		if( oneIncludeLoadedResult == INCLUDES_NOT_EXISTS)
		{
			//??
		}
		// если хот€ бы один имеет статус INCLUDES_LOADING - то € тоже гружусь
		if( oneIncludeLoadedResult == INCLUDES_LOADING)
		{
			recursiveLoadedResult = INCLUDES_LOADING;
		}
	}

	loadedStateIt->second = recursiveLoadedResult;

	return recursiveLoadedResult;
}

CClassScriptsLoader::~CClassScriptsLoader()
{
}