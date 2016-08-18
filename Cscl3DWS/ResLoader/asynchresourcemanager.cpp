
#include "StdAfx.h"
#include "AsynchResourceManager.h"
#include "IAsynchResource.h"
#include "ThreadAffinity.h"
#include "HelpFunctions.h"
#include "ObjStorage.h"
#include <Algorithm>
#include <Assert.h>

//#define MEASURE_TIME(t)	unsigned int t; if (m_isTimeLoggingEnabled) { t = (unsigned int)m_tp.GetMicroTickCount(); }
#define MAX_NORMAL_UPDATE_TIME				30	// 30 mcs
#define MAX_NORMAL_PROCESS_RESOURCE_TIME	10	// 10 mcs

#define MAX_MODERATE_PROCESS_LOADING_RESOURCES_TIME_IN_MS	25		// 25 ms
#define MAX_AGRESSIVE_PROCESS_LOADING_RESOURCES_TIME_IN_MS	1000	// 1000 ms

static CAsynchResourceManager* gpAsynchResourceManager = NULL;

#include "CrashHandlerInThread.h"

//static unsigned int gsl03 = 1;

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	int num = (int)lpParam;

	//CoInitialize(NULL);

	CAsynchResourceManager::GetInstance()->Download(num);

	//CoUninitialize();

	return 0;
}

CAsynchResourceManager::CAsynchResourceManager():
	m_mode(DOWNLOAD_MODE_AGRESSIVE_LOADING),	
	m_isTimeLoggingEnabled(false),
	m_isLocalScripts(false),
	MP_VECTOR_INIT(m_blackList),
	m_loadingMesh(NULL),
	m_lastZoneNum(0),
	m_isWorking(true),
	MP_VECTOR_INIT(m_loadedResources),
	MP_MAP_INIT(m_localResources),
	MP_MAP_INIT(m_objectsBySRC),
	MP_MAP_INIT(m_map),
	MP_VECTOR_INIT(m_resources),
	MP_VECTOR_INIT(m_pendingResources),
	MP_VECTOR_INIT(m_pausingResources),
	MP_VECTOR_INIT(m_NotAcceptedResources)
{
	m_resources.clear();
	m_loadingMesh = NULL;
	m_lw = NULL;
	m_seriousChangesListener = NULL;
	
	InitDownloadSchemes();
	InitializeCriticalSection(&m_csBlackList);
	InitializeCriticalSection(&m_csWorking);
	InitializeCriticalSection(&m_csListResource);
	InitializeCriticalSection(&m_csListPending);
	InitializeCriticalSection(&m_csListPausing);
	InitializeCriticalSection(&m_csListNotAcceptedResource);
	InitializeCriticalSection(&m_csLoadedResource);

	m_tp.Init();
	m_tp.Start();
	m_priorityUpdater = NULL;	
}

bool CAsynchResourceManager::IsLocalScripts()
{
	return m_isLocalScripts;
}

void CAsynchResourceManager::SetLocalScripts(bool isLocalScripts)
{
	m_isLocalScripts = isLocalScripts;
}

void CAsynchResourceManager::SetPriorityUpdater(IPriorityUpdater* updater)
{
	m_priorityUpdater = updater;
}

void CAsynchResourceManager::EnableTimeLogging(bool isEnabled)
{
	m_isTimeLoggingEnabled = isEnabled;
}

bool CAsynchResourceManager::GetModifyTime(LPCWSTR fileName, __int64& modifyTime, bool abLocal)
{
	// Ф-ия должна работать следующим образом (если abLocal = false):
	// 1. Если ресурс есть в списке от сервера, 
	// то записать дату его модификации и вернуть true.
	// 2. Если ресурс в кеше есть, но он неактуальный (т.е. на сервере есть
	// его новая версия), то вернуть false.
	// 3. Если ресурса в кеше нет, то вернуть false.

	if (!CObjStorage::GetInstance()->GetResMan())
	{
		return false;
	}

	if(abLocal)
	{
		res::resIResource* res = NULL;
		/*int rs = */CObjStorage::GetInstance()->GetResMan()->OpenResource( fileName, res, RES_LOCAL);
		if (res == NULL)
		{
			return false;
		}
		modifyTime = res->GetTime();
		res->Close();
		return true;
	}
	res::resIResource* res = NULL;
	// функция дает данные файла из списка со сервера, актуальность кеша ресурсов не проверяет. 
	/*int rs = */CObjStorage::GetInstance()->GetResMan()->OpenResource( fileName, res, RES_REMOTE | RES_ONLY_CACHE | RES_NOT_SAVE_TO_CACHE);
	if (res == NULL)
	{
		return false;
	}

	modifyTime = res->GetTime();
	//DWORD d = res->GetCachedSize();
    
	res->Close();

	/*if (d == 0)
	{
		return false;
	}*/

	return true;
}

IAsynchResource* CAsynchResourceManager::FindAsynchResource(LPCSTR fileName)
{
	CComString _fileName = fileName;

	// ищем в массиве ресурсов
	EnterCriticalSection(&m_csListResource);
	std::vector<ASYNCHRESOURCE>::iterator it = m_resources.begin(),
		end = m_resources.end();
	for (; it != end; it++)
		if ((it->async) && (_fileName == it->async->GetFileName()))
		{
			LeaveCriticalSection(&m_csListResource);
			return it->async;
		}
	LeaveCriticalSection(&m_csListResource);

	// ищем в массиве ресурсов, которые будут добавлены в общий массив на апдейте
	
	EnterCriticalSection(&m_csListPending);
	it = m_pendingResources.begin();
	end = m_pendingResources.end();
	for (; it!=end; it++)
	if ((it->async) && (_fileName == it->async->GetFileName()))
	{
		LeaveCriticalSection(&m_csListPending);
		return it->async;
	}
	LeaveCriticalSection(&m_csListPending);

	EnterCriticalSection(&m_csListNotAcceptedResource);
	it = m_NotAcceptedResources.begin();
	end = m_NotAcceptedResources.end();
	for (; it!=end; it++)
		if ((it->async) && (_fileName == it->async->GetFileName()))
		{
			LeaveCriticalSection(&m_csListNotAcceptedResource);
			return it->async;
		}
	LeaveCriticalSection(&m_csListNotAcceptedResource);

	return NULL;
}

void CAsynchResourceManager::DeleteAsyncResourse( IAsynchResource* apIAsynchResource, IAsynchResourceHandler* handler/*, IResLibrary* resLibrary*/)
{	
	vecResourcesIt it = m_resources.begin();
	vecResourcesIt end = m_resources.end();

	for ( ; it != end; it++)
	if (it->async == apIAsynchResource)
	{
		int k = 0;
		for ( ; k < MAX_DOWNLOAD_THREAD_COUNT; k++)
		if (m_downloads[k].asynch == apIAsynchResource)
		{
			break;
		}

		if (k != MAX_DOWNLOAD_THREAD_COUNT)
		{			
			EnterCriticalSection(&m_csListResource);
			if (handler != NULL)
			{
				RemoveLoadHandler( apIAsynchResource, handler/*, resLibrary*/);
			}
			else
			{
				std::vector<IAsynchResourceHandler*>::iterator it2 = it->handlers.begin();
				std::vector<IAsynchResourceHandler*>::iterator it2End = it->handlers.end();

				for ( ; it2 != it2End; it2++)
				{
					RemoveLoadHandler( apIAsynchResource, *it2/*, resLibrary*/);
				}
			}
			it->async = NULL;			

			IAsynchResource* asynch = m_downloads[k].asynch;

			if (asynch)
			if (asynch == apIAsynchResource)
			{
				asynch->MustDestroySelf();				
			}

			ClearResourceLinks(*it);
			m_resources.erase(it);			
			LeaveCriticalSection(&m_csListResource);			
		}
		else
		{
			EnterCriticalSection(&m_csListResource);
			RemoveLoadHandler( apIAsynchResource, handler/*, resLibrary*/);
			FreeResource( it->async);
			it->async = NULL;
			ClearResourceLinks(*it);
			m_resources.erase( it);
			LeaveCriticalSection(&m_csListResource);
		}
		return;
	}
}

void CAsynchResourceManager::SetDownloadItem(int num, IAsynchResource* asynch)
{
	m_downloads[num].Clear();
	m_downloads[num].asynch = asynch;
	if (asynch)
	{
		m_downloads[num].isImportantSlide = asynch->IsImportantSlide();
	}
}

void CAsynchResourceManager::PauseDownloadAsyncResourse( IAsynchResource* apIAsynchResource)
{
	for ( int k = 0; k < MAX_DOWNLOAD_THREAD_COUNT; k++)
	{
		IAsynchResource* pAsynchResource = m_downloads[ k].asynch;
		if ( pAsynchResource == apIAsynchResource)
		{
			if( LockIResource(k, 2))	// Vladimir
			{
				apIAsynchResource->Pause();
				SetDownloadItem(k, NULL);			
				UnlockIResource( k);
				return;	
			}
			break;
		}
	}

	EnterCriticalSection(&m_csListPausing);
	vecPAResourcesIt	it_pa = find(m_pausingResources.begin(), m_pausingResources.end(), apIAsynchResource);
	if (it_pa != m_pausingResources.end())
	{
		LeaveCriticalSection(&m_csListPausing);
		return;
	}

	m_pausingResources.push_back( apIAsynchResource);
	LeaveCriticalSection(&m_csListPausing);
	return;
}

bool CAsynchResourceManager::LockIResource(int resID, DWORD timeout)
{
	assert((resID >= 0) && (resID < MAX_DOWNLOAD_THREAD_COUNT));
	if (timeout != INFINITE)
	{
		if (m_cslocked[resID] == true)
		{
			return false;
		}
	}

	EnterCriticalSection(&m_cs[resID]);
	assert(!m_cslocked[resID]);
	m_cslocked[resID] = true;

	return true;
}

void CAsynchResourceManager::UnlockIResource(int resID)
{
	assert(resID>=0 && resID<MAX_DOWNLOAD_THREAD_COUNT);
	assert(m_cslocked[resID]);
	m_cslocked[resID] = false;
	LeaveCriticalSection(&m_cs[resID]);
}

void CAsynchResourceManager::AddAsynchResourceHandler(IAsynchResource* apIAsynchResource, IAsynchResourceHandler* handler)
{
	LookupList::iterator iter;
	std::vector<ASYNCHRESOURCE>::iterator	it = m_resources.begin(),
											end = m_resources.end();
	for ( ; it != end; it++)
	{
		if (it->async == apIAsynchResource)
		{
			it->AddHandler(handler);
			LeaveCriticalSection(&m_csListResource);
			return;
		}
	}

	EnterCriticalSection(&m_csListPending);
	it = m_pendingResources.begin();
	end = m_pendingResources.end();
	for ( ; it != end; it++)
	{
		if (it->async == apIAsynchResource)
		{
			it->AddHandler(handler);
			LeaveCriticalSection(&m_csListPending);
			return;
		}
	}
	LeaveCriticalSection(&m_csListPending);

	EnterCriticalSection(&m_csListNotAcceptedResource);
	it = m_NotAcceptedResources.begin();
	end = m_NotAcceptedResources.end();
	for ( ; it != end; it++)
	{
		if (it->async == apIAsynchResource)
		{
			it->AddHandler(handler);
			LeaveCriticalSection(&m_csListNotAcceptedResource);
			return;
		}
	}
	LeaveCriticalSection(&m_csListNotAcceptedResource);
}

static int localResCount = 0;

void CAsynchResourceManager::PatchFileNameIfNeeded(std::string&  sCorrectedFileName)
{
	if (m_isLocalScripts)
	if ((sCorrectedFileName.find(":scripts\\") != -1) || (sCorrectedFileName.find(":scripts/") != -1))
	{
		sCorrectedFileName.erase(sCorrectedFileName.begin(), sCorrectedFileName.begin() + 9);
		sCorrectedFileName = ":rmcommon:/scripts/server/scripts/" + sCorrectedFileName;
	}
}

IAsynchResource* CAsynchResourceManager::GetAsynchResource(LPCSTR fileName, bool isInMainThread, DWORD mainPriority, IAsynchResourceHandler* handler, IResLibrary* resLibrary, std::vector<void*>* owners, void* exactObject, char scriptPriority)
{
	// Проверим на расположение в BlackList
	std::string sCorrectedFileName = CorrectFilename(fileName);
	PatchFileNameIfNeeded(sCorrectedFileName);

	if (ResourceIsInBlackList(sCorrectedFileName))	
	{
		return NULL;
	}

#ifdef RESLOADER_LOG
	if( m_lw!= NULL)
		m_lw->WriteLnLPCSTR(CLogValue("Getting Asynch resource:  ", sCorrectedFileName.c_str()).GetData());
#endif	

	// Проверим, а может ресурс уже грузится
	IAsynchResource* checkRes = FindAsynchResource(sCorrectedFileName.c_str());
	if (checkRes != NULL)
	{

#if RESLOADER_BUILD_H
		if ( checkRes->IsPause())
			checkRes->Continue();
#endif
		AddLoadHandler(checkRes, handler);
		return checkRes;
	}

	ASYNCHRESOURCE tmp;
	tmp.AddHandler(handler);
	tmp.async = resLibrary->GetAsynchResource();
	// Ресурс можно загрузить?
	if (!tmp.async)	
	{
		m_lw->WriteLnLPCSTR(CLogValue("Result: !tmp.async", sCorrectedFileName.c_str()).GetData());
		return NULL;
	}

	tmp.async->SetFileName(sCorrectedFileName.c_str());
	tmp.async->SetMainPriority(mainPriority);
	tmp.async->SetPriority(1.0f);
	tmp.async->SetExactObject(exactObject);
	
	// А может быть ресурс локальный?
	bool bLocal = false;
	MP_MAP<MP_STRING, bool>::iterator mi = m_localResources.find(MAKE_MP_STRING(sCorrectedFileName));
	if(mi == m_localResources.end()){
		bLocal = tmp.async->IsLocalFileExists();
		if(bLocal){
			m_localResources.insert(MP_MAP<MP_STRING, bool>::value_type(MAKE_MP_STRING(sCorrectedFileName), true));
		}
	}else{
		bLocal = true;
	}
	if (!bLocal /*&& tmp.async->IsRemoteFileExists()*/)
	{
		if (owners)
		{
			std::vector<void *>::iterator it = owners->begin();
			std::vector<void *>::iterator itEnd = owners->end();

			for ( ; it != itEnd; it++)
			{
				tmp.async->AddObject(*it);
			}
		}

		MP_MAP<MP_STRING, vecPSObjectInfo*>::iterator it = m_objectsBySRC.find(MAKE_MP_STRING(fileName));
		if (it != m_objectsBySRC.end())
		{
			vecPSObjectInfo* a = (*it).second;
			vecPSObjectInfo::iterator iter = a->begin();
			vecPSObjectInfo::iterator iterEnd = a->end();

			for ( ; iter != iterEnd; iter++)
			{
				tmp.async->AddObject((*iter)->obj, (*iter)->koef);
			}
		}

		if (scriptPriority != -1)
		{
			tmp.async->SetScriptPriority(scriptPriority);
		}	
		
		if ((isInMainThread) && (m_priorityUpdater))
		{
			m_priorityUpdater->UpdateResourceLoadingPriority(tmp.async);
		}

		EnterCriticalSection(&m_csListPending);
		m_pendingResources.push_back(tmp);
		LeaveCriticalSection(&m_csListPending);
		m_map.insert(LookupList::value_type(MAKE_MP_STRING(fileName), m_resources.size() + m_pendingResources.size() - 1));

		return tmp.async;
	}
	else
	{		
		resLibrary->DeleteAsynchResource(tmp.async);
		return NULL;
	}
}

void CAsynchResourceManager::Download(int num)
{
	int failCount = 0;
	IAsynchResource* aplast_asynchresource = NULL;

	HANDLE hndl = GetCurrentThread();
	SetSecondaryThreadAffinity( hndl);

	while (TRUE)
	{
		EnterCriticalSection(&m_csWorking);
		if (!m_isWorking)
		{
			LeaveCriticalSection(&m_csWorking);
			Sleep(SLEEP_TIME);
			SetSecondaryThreadAffinity( hndl);
			continue;
		}
		else
		{
			LeaveCriticalSection(&m_csWorking);
		}

		if(!LockIResource(num))
			continue;

		IAsynchResource* apIAsynchResource = m_downloads[num].asynch;
		if (aplast_asynchresource != apIAsynchResource)
		{
			failCount = 0;
			aplast_asynchresource = apIAsynchResource;
		}

		if (apIAsynchResource != NULL)
		if (apIAsynchResource->IsPause())
		{
			UnlockIResource(num);
			Sleep(SLEEP_TIME);
			SetSecondaryThreadAffinity( hndl);
			continue;
		}

		// Если данный ресурс стоит в очереди на паузу
		if( PopPausedResourse( apIAsynchResource))
		{
			apIAsynchResource->Pause();
			SetDownloadItem(num, NULL);			
			apIAsynchResource = NULL;
		}

		// Если нечего грузить немного поспим
		if (apIAsynchResource == NULL)
		{
			UnlockIResource(num);
			Sleep(SLEEP_TIME);
			SetSecondaryThreadAffinity( hndl);
			continue;
		}

		resources::RESOURCE_STATE loadingState = apIAsynchResource->GetLoadingState();
							// если еще не загружен
		bool shouldRead = (loadingState != resources::RS_LOADED &&
							// .. и существует
							loadingState != resources::RS_NOTEXISTS &&
							// .. и доступен
                            //loadingState != resources::RS_NOTACCEPTED &&
							// .. и число ошибое < 3
							failCount < 3);
							// .. то будем его читать

		if (shouldRead)
		{
			// если checkState==true, то m_downloads[num]!=NULL
			assert(m_resources.size() != 0);

			float currentLoadedPercent = apIAsynchResource->GetLoadedPercent();
			
			//bool wasError = false;
			if(apIAsynchResource->ReadNext(BUFFER_SIZE) == -1)
			{
				//wasError = true;
			}
			/*if (wasError == true)
			{
				assert(m_downloads[num] != NULL);
			}*/

			assert(m_downloads[num].asynch != NULL);
			//apIAsynchResource->CheckSizeReading();
			if ( fabs(currentLoadedPercent - apIAsynchResource->GetLoadedPercent()) == 0)
			{
				loadingState = apIAsynchResource->GetLoadingState();
				//if(loadingState != resources::RS_NOTACCEPTED)
					failCount++;
			}
			else
			{
				failCount = 0;
			}

			if (m_downloads[num].asynch)
			if (m_downloads[num].asynch->IsMustDestroySelf())
			{
				FreeResource(m_downloads[num].asynch);				
				m_downloads[num].Clear();							
			}

			UnlockIResource(num);			
		}
		else
		{
			if (m_downloads[num].asynch)
			if (m_downloads[num].asynch->IsMustDestroySelf())
			{
				FreeResource(m_downloads[num].asynch);
				m_downloads[num].Clear();							
			}

			UnlockIResource(num);
			Sleep(SLEEP_TIME);
			SetSecondaryThreadAffinity( hndl);
		}
	}
}

void	CAsynchResourceManager::SetDownloadMode(int mode)
{
	m_mode = mode;
}

void CAsynchResourceManager::Update()
{
	//MEASURE_TIME(t1);
	
	EnterCriticalSection(&m_csWorking);
	if (!m_isWorking)
	{
		LeaveCriticalSection(&m_csWorking);
		return;
	}
	else
	{
		LeaveCriticalSection(&m_csWorking);
	}

	//MEASURE_TIME(t2);

	// Запишем все затребованные в этот кадр ресурсы в общую очередь
	PushPendingResources();

	//MEASURE_TIME(t3);

	// Запишем все ресурсы, которые были не доступны, в общую очередь
	PushNotAcceptedResources();

	//MEASURE_TIME(t4);
	
	// Пройдем по всем потокам и посмотрим, кто загрузился а кого пора выкинуть
	ProcessLoadingResources();

	//MEASURE_TIME(t5);

	// определим количество одновременно загружаемых ресурсов
	int cnt = (m_resources.size() < MAX_DOWNLOAD_THREAD_COUNT) ? m_resources.size() : MAX_DOWNLOAD_THREAD_COUNT;
	
	// массив ресурсов по убыванию приоритетов на данный момент
	memset(m_topDownloadsByZone, 0, sizeof(m_topDownloadsByZone));

	// формирование списка кандидатов на загрузку
	for (int k = 0; k < cnt; k++)
	{
		int num[ZONES_COUNT][RESOURCE_TYPE_COUNT];
		memset(num, -1, sizeof(num));
		
		// определим номер ресурса с максимальным приоритетом в каждой зоне
		for (unsigned int i = 0; i < m_resources.size(); i++)
		{
			IAsynchResource* pIAsynchResource = m_resources[i].async;
			assert(pIAsynchResource);
			if (!pIAsynchResource)	
			{
				continue;	
			}

			int zoneID = pIAsynchResource->GetZoneNum();			
			assert(zoneID < ZONES_COUNT);
			int resType = pIAsynchResource->GetMainPriority();
			assert(resType < RESOURCE_TYPE_COUNT);

			bool isCandidate = true;
			for (int j = 0; isCandidate && (j < k); j++)
			// если ресурс уже в числе кандидатов
			if (pIAsynchResource == m_topDownloadsByZone[j][zoneID][resType])
			{
				isCandidate = false;
				break;
			}

			if (isCandidate)
			for (int i = 0; i < MAX_DOWNLOAD_THREAD_COUNT; i++)
			// если ресурс уже грузится
			if (m_downloads[i].asynch == pIAsynchResource)
			{
				isCandidate = false;
				break;
			}
			
			if (isCandidate)
			if ((num[zoneID][resType] == -1) || (ComparePriority(pIAsynchResource, m_resources[num[zoneID][resType]].async)))
			{
				num[zoneID][resType] = i;
			}
		}

		for (int l = 0; l < RESOURCE_TYPE_COUNT; l++)
		for (int j = 0; j < ZONES_COUNT; j++)
		{
			if (num[j][l] == -1)	
			{
				continue;
			}
			assert((num[j][l] == -1) || ((num[j][l] != -1) && ((unsigned int)num[j][l] < m_resources.size())));

			// запишем этот ресурс в массив загрузок
			m_topDownloadsByZone[k][j][l] = m_resources[num[j][l]].async;
		}
	}

	//MEASURE_TIME(t6);

	// Теперь в массиве topDownloadsByZone[k] содержатся ресурсы в порядке убывания приоритета
	for (int k = 0; k < MAX_DOWNLOAD_THREAD_COUNT; k++)
	{
		IAsynchResource* pAsynchResource = NULL;

		int resType = 0;
		while ((!pAsynchResource) && (resType < RESOURCE_TYPE_COUNT))
		{
			pAsynchResource = GetNextObjectForLoading(m_threadDest[DOWNLOAD_SCHEME_MESH_MAINLY][k][resType++]);
		}

		if (pAsynchResource)
		{
			if( m_downloads[k].asynch != NULL)
			{
				resources::RESOURCE_STATE resourceLoadingState = m_downloads[k].asynch->GetLoadingState();
				if (resourceLoadingState == resources::RS_LOADED ||
					resourceLoadingState == resources::RS_NOTEXISTS || 
					resourceLoadingState == resources::RS_NOTACCEPTED || 
					resourceLoadingState == resources::RS_NOTREAD)
				{
					if (LockIResource(k, 2))
					{
						std::vector<ASYNCHRESOURCE>::iterator it1 = m_resources.begin();
						for( ; it1!=m_resources.end(); it1++)
						{
							if (m_downloads[k].asynch == it1->async)
							{
								CallResourceLoadStateHandlers( it1);
								if(resourceLoadingState != resources::RS_NOTACCEPTED)
								{
									EnterCriticalSection(&m_csListResource);
									FreeResource(it1->async);
									it1->async = NULL;
								}
								else
									EnterCriticalSection(&m_csListResource);
								ClearResourceLinks(*it1);
								m_resources.erase(it1);								
								LeaveCriticalSection(&m_csListResource);
								break;
							}
						}
						SetDownloadItem(k, pAsynchResource);
						
						UnlockIResource(k);
					}
				}
			}
			else
			{
#if RESLOADER_BUILD_H
				if ( !pAsynchResource->IsPause())
				{
					SetDownloadItem(k, pAsynchResource);
				}
#else
				SetDownloadItem(k, pAsynchResource);
#endif
			}
		}
	}

	bool isPauseAll = false;

	for (int k = 0; k < MAX_DOWNLOAD_THREAD_COUNT; k++)
	if (m_downloads[k].asynch != NULL)
	if (m_downloads[k].isImportantSlide)
	{
		isPauseAll = true;
	}

	for (int k = 0; k < MAX_DOWNLOAD_THREAD_COUNT; k++)
	if (m_downloads[k].asynch != NULL)
	{
		if (isPauseAll)
		{
			if (!m_downloads[k].isImportantSlide)
			{
				if (!m_downloads[k].isPaused)
				{
					if (LockIResource(k, 2))
					{
						m_downloads[k].isPaused = true;
						m_downloads[k].asynch->Pause();
						UnlockIResource( k);
					}
				}
			}
			else if (m_downloads[k].isPaused)
			{
				if (LockIResource(k, 2))
				{
					m_downloads[k].asynch->Continue();
					m_downloads[k].isPaused = false;
					UnlockIResource( k);
				}
			}			
		}
		else if (m_downloads[k].isPaused)
		{
			if (LockIResource(k, 2))
			{
				m_downloads[k].asynch->Continue();
				m_downloads[k].isPaused = false;
				UnlockIResource( k);
			}
		}
	}

	/*MEASURE_TIME(t7);

	if ((m_isTimeLoggingEnabled) && (t7 - t1 > MAX_NORMAL_UPDATE_TIME))
	{
		m_lw->WriteLn("RL UPDATE: CS: ", t2 - t1);
		m_lw->WriteLn("RL UPDATE: PushPendingResources: ", t3 - t2);
		m_lw->WriteLn("RL UPDATE: PushNotAcceptedResources: ", t4 - t3);
		m_lw->WriteLn("RL UPDATE: ProcessLoadingResources: ", t5 - t4);
		m_lw->WriteLn("RL UPDATE: FindCandidatesForLoading: ", t6 - t5);
		m_lw->WriteLn("RL UPDATE: ChangeResources: ", t7 - t6);
	}*/
}

void CAsynchResourceManager::ClearResourceLinks(ASYNCHRESOURCE& res)
{
	for (int k = 0; k < MAX_DOWNLOAD_THREAD_COUNT; k++)
	for (int l = 0; l < RESOURCE_TYPE_COUNT; l++)
	for (int j = 0; j < ZONES_COUNT; j++)
	if (m_topDownloadsByZone[k][j][l] == res.async)
	{	
		m_topDownloadsByZone[k][j][l] = NULL;	
	}
}

// Выбирает из списка наиболее приоритетных ресурс определенного типа,
// по возможности чередуя зоны (среди объектов с одинакомым приоритетом)
IAsynchResource* CAsynchResourceManager::GetNextObjectForLoading(int resType)
{
	IAsynchResource* topResources[ZONES_COUNT];
	memset(topResources, 0, sizeof(topResources));

	// найдем наиболее приоритетный ресурс каждой зоны заданного типа
	for (int k = 0; k < ZONES_COUNT; k++)
	{
		for (int i = 0; i < MAX_DOWNLOAD_THREAD_COUNT; i++)
		if (m_topDownloadsByZone[i][k][resType] != NULL)
		{
			topResources[k] = m_topDownloadsByZone[i][k][resType];
			break;
		}
	}
	
	// найдем максимальный приоритет среди приоритетов выбранных объектов
	int maxPriority = 0;
	
	for (int k = 0; k < ZONES_COUNT; k++)
	if (topResources[k])
	if (topResources[k]->GetPriority() > maxPriority)
	{
		maxPriority = (int)topResources[k]->GetPriority();
	}

	// заNULLим те объекты, которые имеют приоритет ниже максимального
	for (int k = 0; k < ZONES_COUNT; k++)
	if (topResources[k])
	if ( fabs(topResources[k]->GetPriority() - maxPriority) != 0.0f)
	{
		topResources[k] = NULL;
	}

	int sumCount = 1;
	//m_lastZoneNum = (m_lastZoneNum + 1)%ZONES_COUNT;
	m_lastZoneNum = 0;

	while (sumCount <= ZONES_COUNT)
	{
		if (topResources[m_lastZoneNum] != NULL)
		{
			for (int j = 0; j < MAX_DOWNLOAD_THREAD_COUNT; j++)
			if (m_topDownloadsByZone[j][m_lastZoneNum][resType] == topResources[m_lastZoneNum])
			{
				m_topDownloadsByZone[j][m_lastZoneNum][resType] = NULL;
				break;
			}

			return topResources[m_lastZoneNum];
		}

		sumCount++;
		m_lastZoneNum = (m_lastZoneNum + 1)%ZONES_COUNT;
	}

	return NULL;
}

int x_count = 0;

void WriteResourceErrorLog( IAsynchResource* apIAsynchResource, resources::RESOURCE_STATE	resourceLoadingState, int callPlace)
{
	if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
	{
		if( apIAsynchResource->GetFileName())
			CObjStorage::GetInstance()->GetILogWriter()->WriteLn(CLogValue("[ERROR] Resource not exists: "), apIAsynchResource->GetFileName(), ", state = ", (int)resourceLoadingState, ", callplace = ", callPlace);
		else
			CObjStorage::GetInstance()->GetILogWriter()->WriteLn(CLogValue("[ERROR] Resource not exists: file name is unknown, state = "), (int)resourceLoadingState, ", callplace = ", callPlace);
	}
}

void CAsynchResourceManager::ProcessLoadingResources()
{
	__int64 startProcessingTime = m_tp.GetTickCount();

	for (int k = 0; k < MAX_DOWNLOAD_THREAD_COUNT; k++)
	{
		IAsynchResource* apIAsynchResource = m_downloads[k].asynch;
		if (apIAsynchResource == NULL)	
		{
			continue;
		}

		resources::RESOURCE_STATE	resourceLoadingState = apIAsynchResource->GetLoadingState();

		if (resourceLoadingState == resources::RS_LOADING)
		{
			float count = apIAsynchResource->GetLoadedPercent();

			if ( fabs(count) > 0.0f && fabs(count - 1.0f) > 0.0f)
			{				
				std::vector<ASYNCHRESOURCE>::iterator it1 = m_resources.begin();				
				for ( ;it1 != m_resources.end(); it1++)
					if ( it1->async == apIAsynchResource)
						break;
				if ( it1 == m_resources.end())	
					continue;
				CallPersentLoadListeners(&(*it1));
			}
		}
		else	if ((resourceLoadingState == resources::RS_LOADED) || 
			(resourceLoadingState == resources::RS_NOTEXISTS) || 
			(resourceLoadingState == resources::RS_NOTACCEPTED)
			|| resourceLoadingState == resources::RS_NOTREAD)
		{
			//MEASURE_TIME(t1);

			// Получим итератор на загруженный или не существующий ресурс
			std::vector<ASYNCHRESOURCE>::iterator it1 = m_resources.begin();
			for(; it1!=m_resources.end(); it1++)
			{
				if (it1->async == apIAsynchResource)
				{
					break;
				}
			}	

			if (it1 == m_resources.end())	
			{
				continue;
			}

			const char *pFile = it1->async->GetFileName();
			std::string name = "";
			if (pFile)
			{
				name = pFile;
			}
			
			if( !LockIResource(k, 2))	// Alex
				continue;

			bool isResourceSaved = false;
			if (resourceLoadingState == resources::RS_LOADED)
			{
				// Ресурс загрузился
				isResourceSaved = true;
				EnterCriticalSection(&m_csLoadedResource);
				m_loadedResources.push_back(*it1);
				LeaveCriticalSection(&m_csLoadedResource);
				it1->async = NULL;
			}
			else if(resourceLoadingState == resources::RS_NOTACCEPTED)
			{
				// Сообщим о том что ресурс временно недоступен
				RegisterListNotAcceptedResource(&(*it1));
			}
			else if(resourceLoadingState == resources::RS_NOTEXISTS)
			{
				// Сообщим о том что ресурс не существует
				RegisterBlackListResource(apIAsynchResource);
				WriteResourceErrorLog( apIAsynchResource, resourceLoadingState, 1);
				CallErrorListeners(&(*it1));
			}
			else if(resourceLoadingState == resources::RS_NOTREAD)
			{
				// Сообщим о том что пользовательский ресурс не может быть прочитан (и сообщим об ошибке)
				// и прекратим запрашивать до следующего пользовательского запроса 
				WriteResourceErrorLog( apIAsynchResource, resourceLoadingState, 1);
				CallErrorListeners(&(*it1));
			}

			EnterCriticalSection(&m_csListResource);
			if(resourceLoadingState != resources::RS_NOTACCEPTED)
			{
				if (!isResourceSaved)
				{					
					FreeResource(it1->async);
					it1->async = NULL;
				}
			}
			
			ClearResourceLinks(*it1);
			m_resources.erase(it1);			
			LeaveCriticalSection(&m_csListResource);

			SetDownloadItem(k, NULL);			
			UnlockIResource(k);

			//MEASURE_TIME(t2);

			/*if ((m_isTimeLoggingEnabled) && (t2 - t1 > MAX_NORMAL_PROCESS_RESOURCE_TIME))
			{
				m_lw->WriteLn("RL: PROCESS: ", name, " in ", t2 - t1, " mcs");
			}*/
		}
	}	

	int maxProcessTime = (DOWNLOAD_MODE_MODERATE_LOADING == m_mode) ? MAX_MODERATE_PROCESS_LOADING_RESOURCES_TIME_IN_MS : MAX_AGRESSIVE_PROCESS_LOADING_RESOURCES_TIME_IN_MS;

	EnterCriticalSection(&m_csLoadedResource);
	std::vector<ASYNCHRESOURCE>::iterator it = m_loadedResources.begin();
	std::vector<ASYNCHRESOURCE>::iterator itEnd = m_loadedResources.end();

	for ( ; it != itEnd; it++)
	{
		CallLoadedListeners(&(*it));
		FreeResource(it->async);
		it->async = NULL;

		if (m_tp.GetTickCount() - startProcessingTime > maxProcessTime)
		{
			it++;
			break;
		}
	}

	m_loadedResources.erase(m_loadedResources.begin(), it);
	LeaveCriticalSection(&m_csLoadedResource);
}

void CAsynchResourceManager::GetAllAsynchResources(std::vector<IAsynchResource *>& resources)
{
	for (unsigned int i = 0; i < m_resources.size(); i++)
	{
		ASYNCHRESOURCE&	resource = m_resources[i];

		resources.push_back(resource.async);
	}
}

int CAsynchResourceManager::GetAllAsynchResourcesCount()
{
	return m_resources.size();
}

bool CAsynchResourceManager::IsLocalFileExists(LPCSTR fileName)
{
	unsigned int codeError = 0;
	return CObjStorage::GetInstance()->GetResMan()->ResourceExists(cLPWCSTR(fileName), RES_LOCAL, codeError);
}

bool CAsynchResourceManager::IsLocalFileExists(LPCSTR fileName, IResLibrary* resLibrary)
{
	ifile* file = resLibrary->GetResFile(fileName);
	if (file != NULL)
	{
		file->close();
		return true;
	}

	return false;
}

// ***********************************************************
// Compare priority stuff
// ***********************************************************

bool CAsynchResourceManager::ComparePriority(IAsynchResource* async1, IAsynchResource* async2)
{
	if (async1->GetMainPriority() > async2->GetMainPriority())
	{
		return true;
	}

	if (async1->GetMainPriority() < async2->GetMainPriority())
	{
		return false;
	}

	if (async1->GetPriority() > async2->GetPriority())
	{
		return true;
	}

	if (async1->GetPriority() < async2->GetPriority())
	{
		return false;
	}

	return false;
}

// ***********************************************************
// Working with resources stuff
// ***********************************************************

vecResourcesIt	CAsynchResourceManager::GetResourceIterator(IAsynchResource* apIAsynchResource)
{
	vecResourcesIt	it = m_resources.begin(),
		end = m_resources.end();
	for (;it!=end;it++)
		if(it->async == apIAsynchResource)
			return it;
	return end;
}

void	CAsynchResourceManager::FreeResource(IAsynchResource* apIAsynchResource)
{
	assert(apIAsynchResource);
	if(apIAsynchResource)
	{
		//apIAsynchResource->Release();
		// ?? надо все переделать так, чтобы ресурсы освобождали через crmResLibrary::delete_asynch_resource те, 
		// ?? кто их запрашивал через crmResLibrary::get_asynch_resource
		// пока просто удаляем
		CAsynchResource* pAsynchResource = (CAsynchResource*)apIAsynchResource;
		MP_DELETE( pAsynchResource);
		apIAsynchResource = NULL;
	}
}

// ***********************************************************
// Init download threads
// ***********************************************************

void CAsynchResourceManager::CreateThreads()
{
	InitSecondaryThreadAffinityRoutine();

	for (int i = 0; i < MAX_DOWNLOAD_THREAD_COUNT; i++)
	{
		InitializeCriticalSection(&m_cs[i]);
		m_cslocked[i] = false;
		SetDownloadItem(i, NULL);		
		m_threads[i].SetThreadProcedure(ThreadProc);
		m_threads[i].SetParameter((void *)i);
		m_threads[i].Start();
		SetSecondaryThreadAffinity(m_threads[i].GetHandle());
	}
}

// ***********************************************************
// Current loading mesh (for loading textures)
// ***********************************************************

void CAsynchResourceManager::SetLoadingMesh(void* mesh)
{
	m_loadingMesh = mesh;
}

void* CAsynchResourceManager::GetLoadingMesh()
{
	return m_loadingMesh;
}

// ***********************************************************
// Operations with load handlers
// ***********************************************************

void CAsynchResourceManager::AddLoadHandler(IAsynchResource* apIAsynchResource, IAsynchResourceHandler* handler)
{
	LookupList::iterator iter;
	std::vector<ASYNCHRESOURCE>::iterator	it = m_resources.begin(),
		end = m_resources.end();
	for (;it!=end;it++)
	{
		if(it->async == apIAsynchResource)
		{
			it->AddHandler(handler);
			return;
		}
	}

	EnterCriticalSection(&m_csListPending);
	it = m_pendingResources.begin();
	end = m_pendingResources.end();
	for (;it!=end;it++)
	{
		if(it->async == apIAsynchResource)
		{
			it->AddHandler(handler);
			LeaveCriticalSection(&m_csListPending);
			return;
		}
	}
	LeaveCriticalSection(&m_csListPending);
}

void	CAsynchResourceManager::RemoveLoadHandler(IAsynchResource* async, IAsynchResourceHandler* handler/*, IResLibrary* resLibrary*/)
{
	std::vector<ASYNCHRESOURCE>::iterator it = m_resources.begin(),
												end = m_resources.end();
	for (; it!=end;++it)
	{
		if(it->async == async)
		{
			EnterCriticalSection( &m_csListResource);
			it->RemoveHandler(handler);
			if(it->handlers.size() == 0)
				it->async->SetPriority(0);
			LeaveCriticalSection( &m_csListResource);
			break;
		}
	}

	EnterCriticalSection(&m_csListPending);
	it = m_pendingResources.begin();
	end = m_pendingResources.end();
	for (; it!=end;++it)
	{
		if(it->async == async)
		{
			it->RemoveHandler(handler);
			if(it->handlers.size() == 0)
			{
				// ASYNCHRESOURCE& tmp = *it; 
				// resLibrary->delete_asynch_resource(tmp.async); // Tandy 15.05.08
				m_pendingResources.erase(it);
				break;
			}
		}
	}
	LeaveCriticalSection(&m_csListPending);

	EnterCriticalSection(&m_csListNotAcceptedResource);
	it = m_NotAcceptedResources.begin();
	end = m_NotAcceptedResources.end();
	for (; it!=end;++it)
	{
		if(it->async == async)
		{
			it->RemoveHandler(handler);
			if(it->handlers.size() == 0)
			{
				// ASYNCHRESOURCE& tmp = *it; 
				// resLibrary->delete_asynch_resource(tmp.async); // Tandy 15.05.08
				m_NotAcceptedResources.erase(it);
				break;
			}
		}
	}
	LeaveCriticalSection(&m_csListNotAcceptedResource);

	EnterCriticalSection(&m_csLoadedResource);
	it = m_loadedResources.begin();
	end = m_loadedResources.end();
	for (; it!=end;++it)
	{
		if(it->async == async)
		{
			it->RemoveHandler(handler);
			if(it->handlers.size() == 0)
			{
				// ASYNCHRESOURCE& tmp = *it; 
				// resLibrary->delete_asynch_resource(tmp.async); // Tandy 15.05.08
				m_loadedResources.erase(it);
				break;
			}
		}
	}
	LeaveCriticalSection(&m_csLoadedResource);
}

void	CAsynchResourceManager::RemoveLoadHandler(IAsynchResourceHandler* handler/*, IResLibrary* resLibrary*/)
{
	std::vector<ASYNCHRESOURCE>::iterator it = m_resources.begin(),
											end = m_resources.end();
	for (; it!=end;++it)
	{
		EnterCriticalSection( &m_csListResource);
		it->RemoveHandler(handler);
		if(it->handlers.size() == 0)
			it->async->SetPriority(0);
		LeaveCriticalSection( &m_csListResource);
	}

	EnterCriticalSection(&m_csListPending);
	it = m_pendingResources.begin();
    end = m_pendingResources.end();
	for (; it!=end;++it)
	{
		if(it->IsContainsHandler(handler))
		{
			it->RemoveHandler(handler);
			if(it->handlers.size() == 0)
			{
				// ASYNCHRESOURCE& tmp = *it; 
				// resLibrary->delete_asynch_resource(tmp.async); // Tandy 15.05.08
				m_pendingResources.erase(it);
				//LeaveCriticalSection(&m_csListPending);
				break;
			}
		}
	}
	LeaveCriticalSection(&m_csListPending);

	EnterCriticalSection(&m_csListNotAcceptedResource);
	it = m_NotAcceptedResources.begin();
	end = m_NotAcceptedResources.end();
	for (; it!=end;++it)
	{
		if(it->IsContainsHandler(handler))
		{
			it->RemoveHandler(handler);
			if(it->handlers.size() == 0)
			{
				m_NotAcceptedResources.erase(it);
				break;
			}
		}
	}
	LeaveCriticalSection(&m_csListNotAcceptedResource);

	EnterCriticalSection(&m_csLoadedResource);
	it = m_loadedResources.begin();
	end = m_loadedResources.end();
	for (; it!=end;++it)
	{
		if(it->IsContainsHandler(handler))
		{
			it->RemoveHandler(handler);
			if(it->handlers.size() == 0)
			{
				m_loadedResources.erase(it);
				break;
			}
		}
	}
	LeaveCriticalSection(&m_csLoadedResource);
}

void CAsynchResourceManager::DeletePotentionalTexture(void* object, char* src)
{
	MP_MAP<MP_STRING, vecPSObjectInfo*>::iterator it = m_objectsBySRC.find(MAKE_MP_STRING(src));

	if (m_objectsBySRC.end() != it)
	{
		vecPSObjectInfo* a = (*it).second;

		vecPSObjectInfo::iterator iter = a->begin();
		vecPSObjectInfo::iterator iterEnd = a->end();

		for ( ; iter != iterEnd; iter++)
		if ((*iter)->obj == object)
		{
			MP_DELETE( *iter);
			a->erase(iter);
			break;
		}

		if (a->size() == 0)
		{
			m_objectsBySRC.erase(it);
		}
	}
	
}

void CAsynchResourceManager::AddPotentionalTexture(void* obj, char* src, float koef)
{
	if( !src)
	{
		return;
	}

	MP_MAP<MP_STRING, vecPSObjectInfo*>::iterator it = m_objectsBySRC.find(MAKE_MP_STRING(src));

	if (m_objectsBySRC.end() == it)
	{
		MP_NEW_VECTOR(a, vecPSObjectInfo);		

		SObjectInfo* b = MP_NEW( SObjectInfo);
		b->obj = obj;
		b->koef = koef;

		a->push_back(b);
		m_objectsBySRC.insert(MP_MAP<MP_STRING, vecPSObjectInfo*>::value_type(MAKE_MP_STRING(src), a));
	}
	else
	{
		std::vector<SObjectInfo *>* a = (*it).second;
		bool isFound = false;

		std::vector<SObjectInfo *>::iterator iter = a->begin();
		std::vector<SObjectInfo *>::iterator iterEnd = a->end();

		for ( ; iter != iterEnd; iter++)
		if ((*iter)->obj == obj)
		{
			isFound = true;
			break;
		}

		if (!isFound)
		{
			SObjectInfo* b = MP_NEW( SObjectInfo);
			b->obj = obj;
			b->koef = koef;

			a->push_back(b);
		}
	}

	CComString _src = CorrectFilename(src);
	IAsynchResource* apIAsynchResource = FindAsynchResource( _src.GetBuffer());
	if( apIAsynchResource != NULL)
	{
		apIAsynchResource->AddObject(obj);
	}
}

// ***********************************************************
// Singleton stuff
// ***********************************************************

CAsynchResourceManager* CAsynchResourceManager::GetInstance()
{
	if (!gpAsynchResourceManager)
	{
		gpAsynchResourceManager = MP_NEW( CAsynchResourceManager);
		gpAsynchResourceManager->CreateThreads();
	}
	return gpAsynchResourceManager;
}

CAsynchResourceManager* CAsynchResourceManager::GetInstanceNoCreation()
{
	return gpAsynchResourceManager;
};

bool CAsynchResourceManager::ReleaseInstance()
{
	if (gpAsynchResourceManager)
	{
		MP_DELETE( gpAsynchResourceManager);
		gpAsynchResourceManager = NULL;
		return true;
	}
	return false;
}

// ***********************************************************
// Black list of resources
// ***********************************************************

bool	CAsynchResourceManager::ResourceIsInBlackList(const std::string& asResName)
{
	EnterCriticalSection(&m_csBlackList);
	MP_VECTOR<MP_STRING>::iterator it = find(m_blackList.begin(), m_blackList.end(), asResName);
	if(it != m_blackList.end())
	{
//#ifdef DEBUGfuf
		/*if( m_lw != NULL)
		{
			CComString temp;
			temp.Format("[IN BLACKLIST] Result: file \"%s\" in blacklist", asResName.c_str());
			m_lw->WriteLnLPCSTR(CLogValue(temp.GetBuffer(0)).GetData());
		}*/
//#endif
		LeaveCriticalSection(&m_csBlackList);
		return true;
	}
	LeaveCriticalSection(&m_csBlackList);
	return false;
}

void	CAsynchResourceManager::RegisterListNotAcceptedResource(ASYNCHRESOURCE* asResDesc)
{
	CComString tmp = asResDesc->async->GetFileName();
	EnterCriticalSection(&m_csListNotAcceptedResource);
	m_lw->WriteLnLPCSTR(CLogValue("Repeating to download: ", tmp.GetBuffer(0)).GetData());
	m_NotAcceptedResources.push_back(*asResDesc);
	LeaveCriticalSection(&m_csListNotAcceptedResource);
}

void	CAsynchResourceManager::RegisterBlackListResource(IAsynchResource* apIAsynchResource)
{
	assert(apIAsynchResource);
	if(!apIAsynchResource)	return;

	EnterCriticalSection(&m_csBlackList);
	LPCSTR lpFileName = apIAsynchResource->GetFileName();
	std::string s = lpFileName;
	m_blackList.push_back(MAKE_MP_STRING(s));
	LeaveCriticalSection(&m_csBlackList);
}

// ***********************************************************
// Pending resources
// ***********************************************************

void CAsynchResourceManager::PushPendingResources()
{

	EnterCriticalSection(&m_csListPending);
	EnterCriticalSection(&m_csListResource);

	vecResources::iterator it = m_pendingResources.begin();
	vecResources::iterator itEnd = m_pendingResources.end();

	for (; it != itEnd; it++)
	{
		m_resources.push_back(*it);
	}

	m_pendingResources.clear();
	
	LeaveCriticalSection(&m_csListResource);
	LeaveCriticalSection(&m_csListPending);
}

void CAsynchResourceManager::PushNotAcceptedResources()
{
	if( CObjStorage::GetInstance()->GetResMan() == NULL)
		return;
	if( !CObjStorage::GetInstance()->GetResMan()->IsConnectionResServer())
		return;
	EnterCriticalSection(&m_csListNotAcceptedResource);
	EnterCriticalSection(&m_csListResource);
	vecResources::iterator it = m_NotAcceptedResources.begin();
	vecResources::iterator itEnd = m_NotAcceptedResources.end();

	for (; it != itEnd; it++)
	{
		m_resources.push_back(*it);
	}

	m_NotAcceptedResources.clear();
	
	LeaveCriticalSection(&m_csListResource);
	LeaveCriticalSection(&m_csListNotAcceptedResource);
}

bool CAsynchResourceManager::PopPausedResourse( IAsynchResource* apIAsynchResource)
{
	if ( apIAsynchResource == NULL)
		return false;

	EnterCriticalSection(&m_csListPausing);
	EnterCriticalSection(&m_csListResource);	

	vecPAResourcesIt	it_pa = find(m_pausingResources.begin(), m_pausingResources.end(), apIAsynchResource);
	if (it_pa == m_pausingResources.end())
	{
		LeaveCriticalSection(&m_csListResource);
		LeaveCriticalSection(&m_csListPausing);
		return false;
	}
	m_pausingResources.erase( it_pa);
	LeaveCriticalSection(&m_csListResource);
	LeaveCriticalSection(&m_csListPausing);
	return true;	
}

// ***********************************************************
// Calling listeners
// ***********************************************************
void CAsynchResourceManager::CallPersentLoadListeners(ASYNCHRESOURCE* asResDesc)
{
	assert(asResDesc);
	if(!asResDesc)	return;

	for (unsigned int ih = 0; ih < asResDesc->handlers.size(); ih++)
	{
		asResDesc->handlers[ih]->OnAsynchResourceLoadedPersent(asResDesc->async, (unsigned char)(asResDesc->async->GetLoadedPercent() * 100));
	}
}

void CAsynchResourceManager::CallLoadedListeners(ASYNCHRESOURCE* asResDesc)
{
	assert(asResDesc);
	if (!asResDesc)	
	{
		return;
	}

	if(asResDesc->async->GetMainPriority() == RES_TYPE_SKELETON + 1)
	{
		asResDesc->async->SetMainPriority(RES_TYPE_BMF);
	}

	PopPausedResourse( asResDesc->async);

	// Сообщим о загрузке ресурса
	for (unsigned int ih = 0; ih < asResDesc->handlers.size(); ih++)
	{
		asResDesc->handlers[ih]->OnAsynchResourceLoaded(asResDesc->async);
		if (m_lw)
		{
//			m_lw->WriteLn("RLoaded: ", asResDesc->async->GetFileName(), " ", asResDesc->async->GetMainPriority(), " ", asResDesc->async->GetPriority(), " ", (int)asResDesc->async->GetScriptPriority(), " ", asResDesc->async->GetObjectCount());
		}
	}

	if ((asResDesc->async->GetMainPriority() == RES_TYPE_BMF) || 
		((asResDesc->async->GetMainPriority() == RES_TYPE_TEXTURE) &&
		asResDesc->async->GetObjectCount() != 0))
	{
		if (m_seriousChangesListener)
		{
			m_seriousChangesListener->OnChanged();
		}
	}
}

void CAsynchResourceManager::SetSeriousChangesListener(IChangesListener* seriousChangesListener)
{
	m_seriousChangesListener = seriousChangesListener;
}

void CAsynchResourceManager::CallErrorListeners(IAsynchResource* apIAsynchResource)
{
	assert(apIAsynchResource);
	if(!apIAsynchResource)	return;

	ASYNCHRESOURCE* asResDesc = NULL;
	std::vector<ASYNCHRESOURCE>::iterator	it = GetResourceIterator(apIAsynchResource),
		end = m_resources.end();
	if(it != end)
		asResDesc = &(*it);

	if(asResDesc == NULL)
	{
		EnterCriticalSection(&m_csListPending);
		it = m_pendingResources.begin();
		end = m_pendingResources.end();
		for (;!asResDesc && it!=end;it++)
		{
			if(it->async == apIAsynchResource)
			{
				asResDesc = &(*it);
				LeaveCriticalSection(&m_csListPending);
				break;
			}
		}
		LeaveCriticalSection(&m_csListPending);
	}

	if(asResDesc == NULL)
	{
		EnterCriticalSection(&m_csListNotAcceptedResource);
		it = m_NotAcceptedResources.begin();
		end = m_NotAcceptedResources.end();
		for (;!asResDesc && it!=end;it++)
		{
			if(it->async == apIAsynchResource)
			{
				asResDesc = &(*it);
				LeaveCriticalSection(&m_csListNotAcceptedResource);
				break;
			}
		}
		LeaveCriticalSection(&m_csListNotAcceptedResource);
	}

	if( asResDesc == NULL)
		return;

	PopPausedResourse( asResDesc->async);

	// Сообщим о загрузке ресурса
	for (unsigned int ih = 0; asResDesc && ih < asResDesc->handlers.size(); ih++)
	{
		asResDesc->handlers[ih]->OnAsynchResourceError(asResDesc->async);
	}
}

void CAsynchResourceManager::CallErrorListeners(ASYNCHRESOURCE* asResDesc)
{
	assert(asResDesc);
	if( asResDesc == NULL)	return;

	PopPausedResourse( asResDesc->async);
	// Сообщим о загрузке ресурса
	for (unsigned int ih = 0; ih < asResDesc->handlers.size(); ih++)
		asResDesc->handlers[ih]->OnAsynchResourceError(asResDesc->async);
}

void CAsynchResourceManager::stop()
{
	EnterCriticalSection(&m_csWorking);
	m_isWorking = false;
	LeaveCriticalSection(&m_csWorking);
}

void CAsynchResourceManager::start()
{
	EnterCriticalSection(&m_csWorking);
	m_isWorking = true;
	LeaveCriticalSection(&m_csWorking);
}

void CAsynchResourceManager::DeleteAsyncResourses()
{
	EnterCriticalSection(&m_csListNotAcceptedResource);
	EnterCriticalSection(&m_csListPending);

	{
		std::vector<ASYNCHRESOURCE>::iterator it = m_NotAcceptedResources.begin();
		std::vector<ASYNCHRESOURCE>::iterator it1 = m_pendingResources.begin();

		for (; it != m_NotAcceptedResources.end(); it++)
		for(; it1!=m_pendingResources.end(); it1++)
		if (it->async == it1->async)
		{
			assert(false);
		}
	}

	LeaveCriticalSection(&m_csListPending);
	EnterCriticalSection(&m_csListResource);

	{
		std::vector<ASYNCHRESOURCE>::iterator it = m_NotAcceptedResources.begin();
		std::vector<ASYNCHRESOURCE>::iterator it1 = m_resources.begin();

		for (; it != m_NotAcceptedResources.end(); it++)
		for(; it1!=m_resources.end(); it1++)
		if (it->async == it1->async)
		{
			assert(false);
		}
	}

	LeaveCriticalSection(&m_csListNotAcceptedResource);
	EnterCriticalSection(&m_csListPending);

	{
		std::vector<ASYNCHRESOURCE>::iterator it = m_pendingResources.begin();
		std::vector<ASYNCHRESOURCE>::iterator it1 = m_resources.begin();

		for (; it != m_pendingResources.end(); it++)
		for(; it1!=m_resources.end(); it1++)
		if (it->async == it1->async)
		{
			assert(false);
		}
	}

	LeaveCriticalSection(&m_csListResource);

	std::vector<ASYNCHRESOURCE>::iterator it = m_pendingResources.begin();
	for(; it!=m_pendingResources.end(); it++)
	{
		FreeResource(it->async);
		it->async = NULL;
	}
	LeaveCriticalSection(&m_csListPending);

	EnterCriticalSection(&m_csListNotAcceptedResource);
	it = m_NotAcceptedResources.begin();
	vecResources::iterator itEnd = m_NotAcceptedResources.end();
	for (; it != itEnd; it++)
	{
		FreeResource(it->async);
		it->async = NULL;
	}

	LeaveCriticalSection(&m_csListNotAcceptedResource);

	EnterCriticalSection(&m_csListPausing);
	m_pausingResources.clear();
	LeaveCriticalSection(&m_csListPausing);

	EnterCriticalSection(&m_csListResource);
	it = m_resources.begin();
	for(; it!=m_resources.end(); it++)
	if (it->async)
	{
		FreeResource(it->async);
		it->async = NULL;		
	}
	LeaveCriticalSection(&m_csListResource);
		
	m_NotAcceptedResources.clear();
	m_pendingResources.clear();
	m_resources.clear();
}

bool CAsynchResourceManager::IsTermitatedThreads()
{
	for (int i = 0; i < MAX_DOWNLOAD_THREAD_COUNT; i++)
	{
		if(!m_threads[i].IsThreadTerminated())
			return false;
	}
	return true;
}

CAsynchResourceManager::~CAsynchResourceManager()
{
	ATLTRACE( "CAsynchResourceManager::~CAsynchResourceManager");
	m_loadingMesh = NULL;

	stop();

	for (int i = 0; i < MAX_DOWNLOAD_THREAD_COUNT; i++)
	{
		m_threads[i].TerminateStop();

		//DeleteCriticalSection(&m_cs[i]);
	}

	while (!IsTermitatedThreads())
	{
		Sleep(1);
	}
	
	DeleteAsyncResourses();
	
	for (int i = 0; i < MAX_DOWNLOAD_THREAD_COUNT; i++)
	{
		DeleteCriticalSection(&m_cs[i]);
	}

	DeleteCriticalSection(&m_csLoadedResource);
	DeleteCriticalSection(&m_csBlackList);
	DeleteCriticalSection(&m_csListResource);
	DeleteCriticalSection(&m_csListPending);
	DeleteCriticalSection(&m_csListPausing);
	DeleteCriticalSection(&m_csWorking);
	DeleteCriticalSection(&m_csListNotAcceptedResource);
}

void	CAsynchResourceManager::CallResourceLoadStateHandlers( std::vector<ASYNCHRESOURCE>::iterator& it)
{
	IAsynchResource* apiasynchresource = it->async;
	ATLASSERT(apiasynchresource);
	if(!apiasynchresource)	return;

	resources::RESOURCE_STATE	resourceLoadingState = apiasynchresource->GetLoadingState();

	if (resourceLoadingState == resources::RS_LOADED)
	{
		// Ресурс загрузился
		CallLoadedListeners( &(*it));
	}
	else if(resourceLoadingState == resources::RS_NOTACCEPTED)
	{
		// Сообщим о том что ресурс временно недоступен
		RegisterListNotAcceptedResource(&(*it));
		//CallErrorListeners(&(*it));
	}
	else if( resourceLoadingState == resources::RS_NOTEXISTS)
	{
		// Сообщим о том что ресурс не существует
		RegisterBlackListResource( apiasynchresource);
		WriteResourceErrorLog( apiasynchresource, resourceLoadingState, 2);
		CallErrorListeners( &(*it));
	}
	else if( resourceLoadingState == resources::RS_NOTREAD)
	{
		// Сообщим о том что ресурс не существует
		WriteResourceErrorLog( apiasynchresource, resourceLoadingState, 2);
		CallErrorListeners( &(*it));
	}
}

void CAsynchResourceManager::SetILogWriter(ILogWriter* lw)
{
	m_lw = lw;
	CObjStorage::GetInstance()->SetILogWriter(lw);
}

int CAsynchResourceManager::GetResourcesLoadInfo(resourceLoadStruct** pResourceLoadStruct)
{
	if( !pResourceLoadStruct)
		return 0;

	for (int i = 0; i < MAX_DOWNLOAD_THREAD_COUNT; i++)
	{
		strcpy_s(m_resourcesStat[i].filename, "none");
		m_resourcesStat[i].fileSize = 0;
		m_resourcesStat[i].loadedPercent = 0;
		m_resourcesStat[i].coordsSize = 0;
		m_resourcesStat[i].pIAsynchResource = NULL;

		if( !m_downloads[i].asynch)
			continue;

		char* pFileName = m_downloads[i].asynch->GetFileName();
		if( pFileName != NULL)
			strcpy_s( m_resourcesStat[i].filename, pFileName);
		else
			strcpy_s( m_resourcesStat[i].filename, "file name is NULL");

		m_resourcesStat[i].fileSize = (long) m_downloads[i].asynch->GetSize();
		m_resourcesStat[i].loadedPercent = (long) (100 * m_downloads[i].asynch->GetLoadedPercent());
		m_resourcesStat[i].pIAsynchResource = m_downloads[i].asynch;

	}

	*pResourceLoadStruct = m_resourcesStat;
	return MAX_DOWNLOAD_THREAD_COUNT;} 