
#pragma once

//#define RESLOADER_LOG	1

#include "IAsynchResource.h"

#define		PRE_OPEN_MAX_BUFFER_SIZE	65536
#define		BUFFER_SIZE				65536

typedef struct _ASYNCHRESOURCE
{
	IAsynchResource* async;	
	MP_VECTOR<IAsynchResourceHandler*>	handlers;

	_ASYNCHRESOURCE();

	void AddHandler(IAsynchResourceHandler* handler)
	{
		for (unsigned int i = 0; i < handlers.size(); i++)
		{
			if( handlers[i] == handler)	return;
		}
		handlers.push_back( handler);
	}

	bool IsContainsHandler(IAsynchResourceHandler* handler)
	{
		for (unsigned int i = 0; i < handlers.size(); i++)
		{
			if( handlers[i] == handler)
			{
				return true;
			}
		}
		return false;
	}

	void RemoveHandler(IAsynchResourceHandler* handler)
	{
		for (unsigned int i = 0; i < handlers.size(); i++)
		{
			if( handlers[i] == handler)
			{
				handlers.erase( handlers.begin() + i);
				break;
			}
		}
	}
} ASYNCHRESOURCE;

class crmMemResourceFile;

class CAsynchResource : public IAsynchResource
{
public:
	CAsynchResource(IResLibrary* resLibrary, bool abNotSaveCache = false);
	~CAsynchResource();

	void OpenAsynch(char* fileName);
	void OpenAsynchAndReadSome(LPCSTR fileName);

	//bool IsRemoteFileExists();

	int	ReadNext(int nBytes);

	resources::RESOURCE_STATE GetLoadingState();

	void SetPriority(float afPriority);
	float GetPriority();

	void SetMainPriority(unsigned int priority);
	unsigned int GetMainPriority();

	void SetObject(void* vs);
	void* GetObject();

	void SetZoneNum(unsigned int priority);
	unsigned int GetZoneNum();

	void SetScriptPriority(unsigned char priority);
	unsigned char GetScriptPriority();

	void SetLOD(unsigned int lod);
	unsigned int GetLOD();

	void SetExactObject(void* obj);
	void* GetExactObject();

	void SetLoadedState();
	
	float GetLoadedPercent();

	unsigned int GetBytesLoaded();

	void UpdateLoadingState();

	ifile* GetIFile();

	void SetFileName(const char* fileName);
	char* GetFileName();

	void AddObject(void* vs, float koef);
	void GetObject(int num, void** obj, float* koef);
	int	GetObjectCount();

	bool IsLocalFileExists();

	unsigned int	GetSize()
	{ return m_size;	}

	void Pause();
	bool IsPause();
	void Continue();

	void Download();
	void Unpack();
	void Release();

	void	SetImportantSlideStatus(bool isImportant);
	bool	IsImportantSlide();

	void	MustDestroySelf();
	bool	IsMustDestroySelf();
	
protected:
	//void	Close();
	void	CloseIResource();
	void	RefineFileName();
	void	UpdateFileSizeAndAlloc();
	void	CheckBufferCapacity(unsigned int auCapacity);

private:
	unsigned int m_size;
	unsigned int m_zoneNum;
	unsigned int m_readBytes;
	unsigned int m_mainPriority;
	unsigned int m_lod;
	void* m_exactObject;
	unsigned int errorRead;

	__int64 m_time;
	
	float m_priority;

	MP_STRING m_fileName;

	crmMemResourceFile* m_file;
	//res::resIResourceManager* m_resMan;
	
	res::resIResource* m_res;

	void* m_visibleObject;

	MP_VECTOR<void *> m_objects;
	MP_VECTOR<float> m_koefs;

	MP_VECTOR<unsigned char> m_buffer;

	BYTE* m_preopenbuffer;
	unsigned int m_preopenreaded;

	bool bErrorAndRepeat;
	unsigned char m_scriptPriority;
	CRITICAL_SECTION m_stateCS;
	resources::RESOURCE_STATE m_loadingState;

	time_t openedTime;
	time_t loadedTime;
	bool bNotSaveCache;
	bool m_isImportantSlide;
	bool m_mustDestroySelf;
};