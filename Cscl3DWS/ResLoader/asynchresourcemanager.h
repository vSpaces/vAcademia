#pragma once

#include "IPriorityUpdater.h"
#include "IAsynchResource.h"
#include "IAsynchResourceHandler.h"
#include "IAsynchResourcemanager.h"
#include "IChangesListener.h"
#include "AsynchResource.h"
#include "TimeProcessor.h"
#include "LogWriter.h"
#include "thread.h"
#include <string>
#include <vector>
#include <map>

typedef struct SDownloadItem
{
	IAsynchResource* asynch;
	// some props to remove unnecessary locks
	bool isPaused;
	bool isImportantSlide;

	SDownloadItem()
	{
		Clear();
	}

	void Clear()
	{
		asynch = NULL;
		isPaused = false;
		isImportantSlide = false;
	}
} _SDownloadItem;

typedef MP_MAP<MP_STRING, int> LookupList;
typedef MP_VECTOR<ASYNCHRESOURCE>	vecResources;
typedef vecResources::iterator		vecResourcesIt;

#define RESOURCE_TYPE_COUNT		3
#define SLEEP_TIME				1

#define DOWNLOAD_SCHEMES_START	void InitDownloadSchemes() { int cnt, mode;
#define DOWNLOAD_SCHEMES_END }
#define DOWNLOAD_SCHEME_START(num) ATLASSERT(num < DOWNLOAD_SCHEME_COUNT); cnt = 0; mode = num;
#define DOWNLOAD_SCHEME_END ATLASSERT(cnt == MAX_DOWNLOAD_THREAD_COUNT);
#define THREAD_DEST(num, x, y, z) m_threadDest[mode][num][0] = x; m_threadDest[mode][num][1] = y; m_threadDest[mode][num][2] = z; cnt++;

#ifdef RESLOADER_EXPORTS
#define RESLOADER_API __declspec(dllexport)
#else
#define RESLOADER_API __declspec(dllimport)
#endif

typedef struct _SObjectInfo
{
	void* obj;
	float koef;

	_SObjectInfo()
	{
		obj = NULL;
		koef = 1.0f;
	}
} SObjectInfo;

class RESLOADER_API CAsynchResourceManager : public IAsynchResourceManager, public IAsyncResourcesLoadingStat
{
public:
	static CAsynchResourceManager* GetInstance();
	static CAsynchResourceManager* GetInstanceNoCreation();
	static bool ReleaseInstance();

	// реализация IAsyncResourcesLoadingStat
public:
	int GetResourcesLoadInfo(resourceLoadStruct** pResourceLoadStruct);

	// реализация IAsynchResourcemanager
public:
	IAsynchResource* GetAsynchResource(LPCSTR fileName, bool isInMainThread, DWORD mainPriority,	IAsynchResourceHandler* handler, IResLibrary* resLibrary, std::vector<void*>* owners = NULL, void* exactObject = NULL, char scriptPriority = -1);
	
	void	AddLoadHandler(IAsynchResource* async, IAsynchResourceHandler* handler);

	void	RemoveLoadHandler(IAsynchResource* async, IAsynchResourceHandler* handler/*, IResLibrary* resLibrary*/);
	void	RemoveLoadHandler(IAsynchResourceHandler* handler/*, IResLibrary* resLibrary*/);

	void	SetDownloadMode(int mode);

	void	AddPotentionalTexture(void* obj, char* src, float koef);
	void	DeletePotentionalTexture(void* object, char* src);

	IAsynchResource* FindAsynchResource(LPCSTR fileName);

	void	DeleteAsyncResourse( IAsynchResource* apIAsynchResource, IAsynchResourceHandler* handler/*, IResLibrary* resLibrary*/);
	
	void	PauseDownloadAsyncResourse( IAsynchResource* apIAsynchResource);	
		
	void	Update();

	void	SetLoadingMesh(void* mesh);
	void*	GetLoadingMesh();

	bool	IsLocalScripts();
	void	SetLocalScripts(bool isLocalScripts);

	void	GetAllAsynchResources(std::vector<IAsynchResource *>& resources);
	int		GetAllAsynchResourcesCount();

	void	SetILogWriter(ILogWriter* lw);

	void	Download(int num);

	//void	SetResManager(res::resIResourceManager* resMan);

	void	stop();
	void	start();

	void	SetSeriousChangesListener(IChangesListener* seriousChangesListener);
	bool	GetModifyTime(LPCWSTR fileName, __int64& modifyTime, bool abLocal = false);
	void	setNotSaveToCache( bool abNotSaveCache = false);
	
	void	EnableTimeLogging(bool isEnabled);

	bool	IsLocalFileExists(LPCSTR fileName);

	void	DeleteAsyncResourses();	

	void	SetPriorityUpdater(IPriorityUpdater* updater);

protected:
	void	AddAsynchResourceHandler(IAsynchResource* apIAsynchResource, IAsynchResourceHandler* handler);
	void	RegisterListNotAcceptedResource(ASYNCHRESOURCE* asResDesc);
	void	RegisterBlackListResource(IAsynchResource* apIAsynchResource);
	bool	ResourceIsInBlackList(const std::string& asResName);

	void	CreateThreads();
	void	CallResourceLoadStateHandlers( std::vector<ASYNCHRESOURCE>::iterator& it);

	bool	IsTermitatedThreads();

private:
	DOWNLOAD_SCHEMES_START
		DOWNLOAD_SCHEME_START(DOWNLOAD_SCHEME_MESH_MAINLY)
			THREAD_DEST(0, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(1, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(2, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(3, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(4, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(5, RES_TYPE_SOUND, RES_TYPE_BMF, RES_TYPE_TEXTURE)
			THREAD_DEST(6, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(7, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(8, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(9, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(10, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(11, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(12, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(13, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(14, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(15, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
		DOWNLOAD_SCHEME_END
		DOWNLOAD_SCHEME_START(DOWNLOAD_SCHEME_NORMAL)
			THREAD_DEST(0, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(1, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(2, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(3, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(4, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(5, RES_TYPE_SOUND, RES_TYPE_BMF, RES_TYPE_TEXTURE)
			THREAD_DEST(6, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(7, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(8, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(9, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(10, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(11, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(12, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(13, RES_TYPE_BMF, RES_TYPE_TEXTURE, RES_TYPE_SOUND)
			THREAD_DEST(14, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
			THREAD_DEST(15, RES_TYPE_TEXTURE, RES_TYPE_BMF, RES_TYPE_SOUND)
		DOWNLOAD_SCHEME_END
	DOWNLOAD_SCHEMES_END

	void SetDownloadItem(int num, IAsynchResource* asynch);

	void ClearResourceLinks(ASYNCHRESOURCE& res);

public:
	CAsynchResourceManager();
	virtual	~CAsynchResourceManager();
private:
	CAsynchResourceManager(const CAsynchResourceManager&);
	CAsynchResourceManager& operator=(const CAsynchResourceManager&);

	void PatchFileNameIfNeeded(std::string&  sCorrectedFileName);

	typedef MP_VECTOR<ASYNCHRESOURCE>	vecResources;
	typedef MP_VECTOR<IAsynchResource*>	vecPAResources;
	typedef vecResources::iterator		vecResourcesIt;
	typedef vecPAResources::iterator	vecPAResourcesIt;
	typedef MP_VECTOR<SObjectInfo *>	vecPSObjectInfo;

	bool	LockIResource(int resID, DWORD timeout = INFINITE);
	void	UnlockIResource(int resID);

	bool	ComparePriority(IAsynchResource* async1, IAsynchResource* async2);

	void	CallPersentLoadListeners( ASYNCHRESOURCE* asResDesc);
	void	CallLoadedListeners( ASYNCHRESOURCE* asResDesc);
	void	CallErrorListeners( ASYNCHRESOURCE* asResDesc);
	void	CallErrorListeners( IAsynchResource* apIAsynchResource);

	void	ProcessLoadingResources();
	void	PushPendingResources();
	void	PushNotAcceptedResources();
	bool	PopPausedResourse( IAsynchResource* apIAsynchResource);

	bool	IsLocalFileExists(LPCSTR fileName, IResLibrary* resLibrary);

	IAsynchResource* GetNextObjectForLoading(int resType);

	void	FreeResource(IAsynchResource* apIAsynchResource);
	vecResourcesIt	GetResourceIterator(IAsynchResource* apIAsynchResource);

	vecResources		m_resources;
	vecResources		m_pendingResources;
	vecPAResources		m_pausingResources;
	vecResources		m_NotAcceptedResources;

	void*				m_loadingMesh;

	resourceLoadStruct	m_resourcesStat[MAX_DOWNLOAD_THREAD_COUNT];
	SDownloadItem	m_downloads[MAX_DOWNLOAD_THREAD_COUNT];
	IAsynchResource*	m_topDownloadsByZone[MAX_DOWNLOAD_THREAD_COUNT][ZONES_COUNT][RESOURCE_TYPE_COUNT];	

	LookupList m_map;

	CThread m_threads[MAX_DOWNLOAD_THREAD_COUNT];

	int m_threadNum[MAX_DOWNLOAD_THREAD_COUNT];

	//bool m_paused;
	bool m_isWorking;
	bool m_isTimeLoggingEnabled;

	CRITICAL_SECTION	m_cs[MAX_DOWNLOAD_THREAD_COUNT];
	bool				m_cslocked[MAX_DOWNLOAD_THREAD_COUNT];
	CRITICAL_SECTION	m_csBlackList;
	CRITICAL_SECTION	m_csWorking;
	CRITICAL_SECTION	m_csListResource;
	CRITICAL_SECTION    m_csListPending;
	CRITICAL_SECTION    m_csListPausing;
	CRITICAL_SECTION    m_csListNotAcceptedResource;
	CRITICAL_SECTION	m_csLoadedResource;

	int	m_threadDest[DOWNLOAD_SCHEME_COUNT][MAX_DOWNLOAD_THREAD_COUNT][RESOURCE_TYPE_COUNT];

	int m_mode;
	int m_lastZoneNum;

	ILogWriter* m_lw;

	IChangesListener* m_seriousChangesListener;

	MP_MAP<MP_STRING, vecPSObjectInfo*> m_objectsBySRC;

	// список уже загружавшихся локальных ресурсов, 
	// чтобы не проверять повторно, есть ли ресурс на жестком диске
	MP_MAP<MP_STRING, bool> m_localResources;

	//res::resIResourceManager* m_resMan;

	MP_VECTOR<MP_STRING> m_blackList;

	CTimeProcessor m_tp;

	MP_VECTOR<ASYNCHRESOURCE> m_loadedResources;

	IPriorityUpdater* m_priorityUpdater;

	bool m_isLocalScripts;
};