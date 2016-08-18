
#include "StdAfx.h"
#include <XMLList.h>
#include "MotionManager.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"

_SAsynchMotionDesc::_SAsynchMotionDesc(CMotion* const _motion, const std::string& _path, CLoadingMotionTask* const _task):
	MP_STRING_INIT(path)
{
	motion = _motion;
	task = _task;
	path = _path;
}

CMotionManager::CMotionManager():
	MP_MAP_INIT(m_animationMap),
	MP_STRING_INIT(m_motionsBasePath),
	MP_VECTOR_INIT(m_asynchMotionDescs)
{
	InitializeCriticalSection(&m_animationMapAccessCS);
}

void CMotionManager::AddCalCoreAnimation(const std::string& name, CalCoreAnimation* const animation)
{
	EnterCriticalSection(&m_animationMapAccessCS);

	std::map<MP_STRING, SCalCoreAnimation*>::iterator it = m_animationMap.find(MAKE_MP_STRING(name));

	if (it == m_animationMap.end())
	{
		SCalCoreAnimation* info = MP_NEW(SCalCoreAnimation);//new SCalCoreAnimation();
		info->refCount = 1;
		info->animation = animation;
		m_animationMap.insert(std::map<MP_STRING, SCalCoreAnimation*>::value_type(MAKE_MP_STRING(name), info));
	}

	LeaveCriticalSection(&m_animationMapAccessCS);
}

CalCoreAnimation* CMotionManager::GetCalCoreAnimation(const std::string& name)
{
	EnterCriticalSection(&m_animationMapAccessCS);
	std::map<MP_STRING, SCalCoreAnimation*>::iterator it = m_animationMap.find(MAKE_MP_STRING(name));

	if (it != m_animationMap.end())
	{
		((*it).second)->refCount++;
		CalCoreAnimation* anim = ((*it).second)->animation; 
		LeaveCriticalSection(&m_animationMapAccessCS);

		return anim;
	}
	else
	{
		LeaveCriticalSection(&m_animationMapAccessCS);

		return NULL;
	}
}

bool CMotionManager::ReleaseCalCoreAnimation(const std::string& name)
{
	EnterCriticalSection(&m_animationMapAccessCS);
	std::map<MP_STRING, SCalCoreAnimation*>::iterator it = m_animationMap.find(MAKE_MP_STRING(name));

	if (it != m_animationMap.end())
	{
		((*it).second)->refCount--;

		bool canDelete = (((*it).second)->refCount == 0);
		if (canDelete)
		{
			MP_DELETE((*it).second);//delete (*it).second;
			m_animationMap.erase(it);			
		}

		LeaveCriticalSection(&m_animationMapAccessCS);

		return canDelete;
	}
	else
	{
		LeaveCriticalSection(&m_animationMapAccessCS);
		return false;
	}
}

bool CMotionManager::IsMotionsPreloaded()
{
#ifdef _DEBUG
	return true;
#else
	return m_preloadMotionsTask.IsPerformed();
#endif
}

void CMotionManager::AsyncPreloadMotions()
{
	g->taskm.AddTask(&m_preloadMotionsTask, MAY_BE_MULTITHREADED, PRIORITY_LOW, true);
}

void CMotionManager::PreloadMotions()
{
	std::string xmlPath, motionsPath;
	std::vector<std::string> optionalFields;
	optionalFields.push_back("motionspath");
	optionalFields.push_back("motionsbasepath");
	CXMLList avatarList(GetApplicationRootDirectory() + L"avatars.xml", optionalFields, true);

	m_motionsBasePath = "";
	avatarList.GetCommonOptionalFieldValue("motionsbasepath", m_motionsBasePath);
	
	while (avatarList.GetNextValue(xmlPath))
	{
		if (avatarList.GetOptionalFieldValue("motionspath", motionsPath))
		{
			PreloadMotions(motionsPath, "*.caf");
			PreloadMotions(motionsPath, "*.bvh");
			PreloadMotions(motionsPath + "poses\\stand_idles\\", "*.caf");
			PreloadMotions(motionsPath + "poses\\stand_idles\\", "*.bvh");
		}
	}
}

void CMotionManager::PreloadMotions(const std::string path, const std::string ext)
{
	USES_CONVERSION;
	std::wstring globalPath = GetApplicationRootDirectory() + A2W(m_motionsBasePath.c_str());
	globalPath += A2W(path.c_str());

	WIN32_FIND_DATAW findData;
	HANDLE hSearch = FindFirstFileW((globalPath + A2W(ext.c_str())).c_str(), &findData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((wcscmp(findData.cFileName, L".") != 0) && (wcscmp(findData.cFileName, L"..") != 0))
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	
			{
			}
			else
			{
				std::string fileName = WCharStringToString(findData.cFileName);
				fileName = StringToLower(fileName);
				PreloadMotion(fileName, path);
			}
		} while (FindNextFileW(hSearch, &findData));
		FindClose(hSearch);		
	}
}

void CMotionManager::PreloadMotion(const std::string& fileName, const std::string& path)
{
	std::string newPath = ":vu.common:\\" + path;
	newPath += fileName;
	USES_CONVERSION;
	int motionID = g->motm.AddObject( newPath.c_str());
	CMotion* motion = g->motm.GetObjectPointer(motionID);
	motion->LoadFromFile(newPath);
	motion->RegisterForPath(newPath);
}

void CMotionManager::CorrectPath(std::string& path)
{
	StringReplace(path, ":\\", ":");
	StringReplace(path, "/", "\\");
	path = StringToLower(path);
}

void CMotionManager::AddAsynchMotion(CMotion* const motion, const std::string& path)
{
	CLoadingMotionTask* task = MP_NEW(CLoadingMotionTask);
	MP_NEW_P3(asynchMotionDesc, SAsynchMotionDesc, motion, path, task);//asynchMotionDesc = new SAsynchMotionDesc(motion, path,new CLoadingMotionTask() );
	asynchMotionDesc->task->SetParams(motion, path);
	g->taskm.AddTask(asynchMotionDesc->task, MAY_BE_MULTITHREADED, PRIORITY_LOW);
	m_asynchMotionDescs.push_back(asynchMotionDesc);
}

void CMotionManager::Update()
{
	std::vector<SAsynchMotionDesc *>::iterator it = m_asynchMotionDescs.begin();
	while (it != m_asynchMotionDescs.end())
	if ((*it)->task->IsPerformed())
	{
		(*it)->motion->OnChanged(0);
		/*delete (*it)->task;
		delete (*it);*/
		it = m_asynchMotionDescs.erase(it);
	}
	else
	{
		it++;
	}
}

CMotionManager::~CMotionManager()
{
	std::map<MP_STRING, SCalCoreAnimation*>::iterator it = m_animationMap.begin();
	std::map<MP_STRING, SCalCoreAnimation*>::iterator itEnd = m_animationMap.end();

	for ( ; it != itEnd; it++)
	{
		CalCoreAnimation* pCalCoreAnimation = ((*it).second)->animation;
		if( pCalCoreAnimation != NULL)
		{
			pCalCoreAnimation->destroy();
			MP_DELETE(pCalCoreAnimation);//delete pCalCoreAnimation;
		}
		MP_DELETE((*it).second);//delete (*it).second;
}

	m_animationMap.clear();

	DeleteCriticalSection(&m_animationMapAccessCS);
}
