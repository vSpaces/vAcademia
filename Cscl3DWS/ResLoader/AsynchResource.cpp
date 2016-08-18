
#include "StdAfx.h"
#include "AsynchResource.h"
#include "crmResLibrary.h"
#include "ObjStorage.h"
#include <Assert.h>
#include "consts.h"

using namespace res;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILE_SIZE_IS_INCORRECT(x) (x == 0xFFFFFFFF || x == 0)

_ASYNCHRESOURCE::_ASYNCHRESOURCE():
	MP_VECTOR_INIT(handlers)
{
	async = NULL;
}

CAsynchResource::CAsynchResource(IResLibrary* resLibrary, bool abNotSaveCache):	
	m_isImportantSlide(false),
	m_mustDestroySelf(false),
	m_scriptPriority(255),
	m_visibleObject(NULL),
	m_exactObject(NULL),
	m_preopenreaded(0),
	m_readBytes(0),
	m_priority(0),
	m_zoneNum(0),
	m_res(NULL),
	m_time(0),
	m_size(0xFFFFFFFF),
	m_lod(0),
	errorRead(qeNoError),
	MP_STRING_INIT(m_fileName),
	MP_VECTOR_INIT(m_objects),
	MP_VECTOR_INIT(m_koefs),
	MP_VECTOR_INIT(m_buffer)
{
	MP_NEW_V( m_file, crmMemResourceFile, (crmResLibrary *)resLibrary);
	m_preopenbuffer = NULL;
	bErrorAndRepeat = false;

	InitializeCriticalSection(&m_stateCS);

	m_loadingState = resources::RS_LOADING;
	bNotSaveCache = abNotSaveCache;
}

void CAsynchResource::MustDestroySelf()
{
	m_mustDestroySelf = true;
}

bool CAsynchResource::IsMustDestroySelf()
{
	return m_mustDestroySelf;
}

void CAsynchResource::SetScriptPriority(unsigned char priority)
{
	m_scriptPriority = priority;
}

unsigned char CAsynchResource::GetScriptPriority()
{
	return m_scriptPriority;
}

void CAsynchResource::SetLOD(unsigned int lod)
{
	m_lod = lod;
}

unsigned int CAsynchResource::GetLOD()
{
	return m_lod;
}

void CAsynchResource::SetExactObject(void* obj)
{
	m_exactObject = obj;
}

void* CAsynchResource::GetExactObject()
{
	return m_exactObject;
}

void CAsynchResource::SetZoneNum(unsigned int zoneNum)
{
	m_zoneNum = zoneNum;
}

unsigned int CAsynchResource::GetZoneNum()
{
	return m_zoneNum;
}

void CAsynchResource::Download()	
{	
	if (m_res == NULL)
	{
		OpenAsynch((char*)m_fileName.data());
	}

	if (m_res != NULL)
	{
		m_res->DownloadToCache();
	}
}

void CAsynchResource::Unpack()
{
	if (m_res != NULL)
	{
		m_res->UnpackToCache();
		m_readBytes = m_size;

		UpdateLoadingState();
		CloseIResource();
	}
}

void CAsynchResource::OpenAsynchAndReadSome(LPCSTR fileName)
{
	m_readBytes = 0;
	assert(!m_res);
	int res = 0;
	if( !m_res)
	{
		DWORD dwFlags = bNotSaveCache ? (RES_REMOTE | RES_NOT_SAVE_TO_CACHE) : (RES_REMOTE | RES_GRAPHIC);
		res = CObjStorage::GetInstance()->GetResMan()->OpenResource(cLPWCSTR(fileName), m_res, dwFlags);
	}

	if (m_res == NULL)
	{
		if(res == OMS_ERROR_RESOURCE_FOUND_TIMEOUT)
		{
			bErrorAndRepeat = true;
			//iErrorAndRepeat++;
		}
		else
			bErrorAndRepeat = false;
		m_size = 0;
		
		UpdateLoadingState();

		CComString sResPath = (char*)fileName;
		if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
		{
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ERROR] OpenAsynchAndReadSome::Not open asynch resource for reading: ", sResPath.GetBuffer(0)).GetData());
		}
	}
	else
	{
		/*int status = */m_res->GetState();
		m_time = m_res->GetTime();

		bErrorAndRepeat = false;

	#ifdef RESLOADER_LOG
		CComString sResPath = (char*)fileName;
		if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
		{
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ResLoader to Comman] OpenAsynchAndReadSome::GetRecommendedPacketSize for: ", sResPath.GetBuffer(0)).GetData());
		}
	#endif
		errorRead = qeNoError;
		long packetSize = CObjStorage::GetInstance()->GetComMan()->GetRecommendedPacketSize(/*MAX_DOWNLOAD_THREAD_COUNT*/);
		m_preopenbuffer = MP_NEW_ARR(BYTE, packetSize);

#ifdef RESLOADER_LOG
		CComString sResPath = (char*)fileName;
		if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
		{
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ResLoader to Comman] OpenAsynchAndReadSome::open asynch resource for reading: ", sResPath.GetBuffer(0)).GetData());
		}
#endif

		m_preopenreaded = m_res->Read((unsigned char*)m_preopenbuffer, packetSize, &errorRead);
		if( m_preopenreaded > 0)
			errorRead = qeNoError;

		// Если размер не установили
		if( FILE_SIZE_IS_INCORRECT (m_size))
		{
			UpdateFileSizeAndAlloc();
		}

		if( m_preopenreaded > 0)
		{
			// Если удалось что-то прочитать, то размер должен быть
			ATLASSERT( FILE_SIZE_IS_INCORRECT (m_size) == false);
			ATLASSERT( m_preopenreaded <= m_buffer.size());

			CheckBufferCapacity( m_readBytes + m_preopenreaded);
			rtl_memcpy( &m_buffer[m_readBytes], m_buffer.size() - m_readBytes, &m_preopenbuffer[m_readBytes], m_preopenreaded);
			m_readBytes+=m_preopenreaded;
		}

		MP_DELETE_ARR(m_preopenbuffer);

		UpdateLoadingState();
		if( m_loadingState == resources::RS_NOTACCEPTED)
		{
			CloseIResource();
		}
	}
}

void CAsynchResource::OpenAsynch(char* fileName)
{
	m_readBytes = 0;
	assert(!m_res);
	
#ifdef RESLOADER_LOG
	if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
	{
		CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(
			CLogValue("[BD-RES] Asynch Resource is being tried to open: ", fileName).GetData());
	}
#endif

	int res = 0;
	if( !m_res)
	{
		DWORD dwFlags = bNotSaveCache ? (RES_REMOTE | RES_NOT_SAVE_TO_CACHE) : (RES_REMOTE | RES_GRAPHIC);
		res = CObjStorage::GetInstance()->GetResMan()->OpenResource(cLPWCSTR(fileName), m_res, dwFlags);
	}

	if (m_res == NULL)
	{
		if(res == OMS_ERROR_RESOURCE_FOUND_TIMEOUT)
		{
			bErrorAndRepeat = true;
		}
		else
		{
			bErrorAndRepeat = false;
		}
		m_size = 0;

		UpdateLoadingState();
		CComString sResPath = (char*)fileName;
		CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ERROR] Not open asynch resource: ", sResPath.GetBuffer(0)).GetData());
	}
	else
	{
#ifdef RESLOADER_LOG
		if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
		{
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(
				CLogValue("[BD-RES] Asynch Resource has been opened: ", fileName).GetData());
		}
#endif

		/*int status = */m_res->GetState();
		m_size = m_res->GetSize();
		m_time = m_res->GetTime();
		m_buffer.resize(m_size + 100);

		UpdateLoadingState();
	}
}

bool CAsynchResource::IsLocalFileExists()
{
	CloseIResource();
	unsigned int codeError = 0;
	return CObjStorage::GetInstance()->GetResMan()->ResourceExists(cLPWCSTR(m_fileName.data()), RES_LOCAL | RES_FILE_CACHE, codeError);
}

void CAsynchResource::SetFileName(const char* fileName)
{
	m_fileName = fileName;
}

void CAsynchResource::RefineFileName()
{
	wchar_t* psName = NULL;
	USES_CONVERSION;
	if(m_res->GetFileNameRefineLoaded(&psName, A2W(m_fileName.c_str())) && psName!=NULL)
	{
		m_fileName = W2A(psName);
		SysFreeString(psName);
	}
}

void	CAsynchResource::UpdateFileSizeAndAlloc()
{
	ATLASSERT( FILE_SIZE_IS_INCORRECT( m_size));

	m_size = m_res->GetSize();
	CheckBufferCapacity( m_size);
}

void	CAsynchResource::CheckBufferCapacity(unsigned int auCapacity)
{
	if( !FILE_SIZE_IS_INCORRECT( auCapacity) && m_buffer.size() < auCapacity)
		m_buffer.resize( auCapacity);
}

int	CAsynchResource::ReadNext(int nBytes)
{	
	// пересчитаем nBytes - надо расчет в вызов функции вынести
	nBytes = CObjStorage::GetInstance()->GetComMan()->GetRecommendedPacketSize(/*MAX_DOWNLOAD_THREAD_COUNT*/);

#ifdef RESLOADER_LOG
	if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
	{
		CObjStorage::GetInstance()->GetILogWriter()->WriteLn(CLogValue("ReadNext path = ", m_fileName.data()));
	}
#endif
	if (!m_res)
	{
		OpenAsynchAndReadSome(m_fileName.data());
		return -1;
	}

	if( FILE_SIZE_IS_INCORRECT (m_size))
	{
		UpdateFileSizeAndAlloc();

		//////////////////////////////
		// нельзя выходить без вызова UpdateLoadingState (установка статуса ошибки)
		// закоментировано, потому что ошибку для прекращения чтения файла должен выдать непосредственно сервер.
		// ошибки связи не должны влиять, при этом запросе на чтение сервер всегда посылает размер файла
		/*if( FILE_SIZE_IS_INCORRECT (m_size))
			return -1;*/
		/////////////////////////////

		if( m_res->GetPos() == 0)
			m_readBytes = 0;
	}
	
	unsigned int readedCount = 0;

	unsigned int wantCount = (m_readBytes + nBytes < m_size) ? nBytes : (m_size - m_readBytes);

	if( m_size > 0 && wantCount > 0)
	{
		errorRead = qeNoError;

		CheckBufferCapacity( m_readBytes + wantCount);

		ATLASSERT( m_buffer.size() >= m_readBytes + wantCount);
#ifdef RESLOADER_LOG
		CComString sResPath = (char*)fileName;
		if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
		{
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ResLoader to Comman] ReadNext::open asynch resource for reading: ", sResPath.GetBuffer(0)).GetData());
		}
#endif
		readedCount = m_res->Read((unsigned char*)(&m_buffer[m_readBytes]), wantCount, &errorRead);

		if( readedCount > 0)
		{
			errorRead = qeNoError;
		}
	}

	m_readBytes += readedCount;

	UpdateLoadingState();

	return m_readBytes;
}

void CAsynchResource::UpdateLoadingState()
{
	EnterCriticalSection(&m_stateCS);

	if (m_size != 0)
	{
		// Обработчик нужен для перезапуска чтения файла для файла, у которых изменился размер перед или в момент чтением файлом.
		if(errorRead == qeReadError && m_res != NULL)
		{
			unsigned int fileSize = m_res->GetSize();
			if( fileSize>0 && fileSize != m_size)
			{
				m_size = fileSize;
				errorRead = qeNoError;
			}
		}
		switch(errorRead)
		{
		case qeReadError:
			{
				m_loadingState = resources::RS_NOTREAD;
				if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
					CObjStorage::GetInstance()->GetILogWriter()->WriteLn( CLogValue("[ERROR] Resource not read, errorRead = 'qeReadError', m_loadingState = RS_NOTREAD (7), m_fileName = ", m_fileName.c_str()));
				break;
			}
		case qeNotExistFileReadError:
			{
				m_loadingState = resources::RS_NOTEXISTS;
				if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
					CObjStorage::GetInstance()->GetILogWriter()->WriteLn(CLogValue("[ERROR] Resource not read, errorRead = 'qeNotExistFileReadError', m_loadingState = RS_NOTEXISTS (4), m_fileName = ", m_fileName.c_str()));
				break;
			}
		case qeTimeOut:
			{
				if(m_size == -1 || m_size > m_readBytes)
				{					
					m_loadingState = resources::RS_NOTACCEPTED;
					if ( CObjStorage::GetInstance()->GetILogWriter() != NULL)
						CObjStorage::GetInstance()->GetILogWriter()->WriteLn(CLogValue("[ERROR] Resource not accepted, m_size = -1, errorRead = 'qeTimeOut', m_loadingState = RS_NOTACCEPTED (5) m_fileName", m_fileName.c_str()));
					break;
				}
			}
		default:
			{
				if(m_size == -1)
				{					
					m_loadingState = resources::RS_NOTACCEPTED;
					if ( CObjStorage::GetInstance()->GetILogWriter() != NULL)
						CObjStorage::GetInstance()->GetILogWriter()->WriteLn(CLogValue("[ERROR] Resource not accepted, m_size = -1, m_loadingState = RS_NOTACCEPTED (5), errorRead = "), (int)errorRead, " m_fileName = ", m_fileName.c_str());
				}
				else
					m_loadingState = (m_size > m_readBytes) ? resources::RS_LOADING : resources::RS_LOADED;
			}
		}		
			
		if(m_fileName.size() != 0 && m_loadingState == resources::RS_LOADED)
		{
		#ifdef RESLOADER_LOG
			if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
			{
				CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(
					CLogValue("[BD-RESLOADER] Asynch Resource has been loaded: ", m_fileName.c_str()).GetData());
			}
		#endif
			RefineFileName();
			CloseIResource();
		}
	}
	else
	{
		if( errorRead == qeReadError)
		{
			m_loadingState = resources::RS_NOTREAD;
		}
		else if (bErrorAndRepeat)
		{			
			m_loadingState = resources::RS_NOTACCEPTED;
			if ( CObjStorage::GetInstance()->GetILogWriter() != NULL)
				CObjStorage::GetInstance()->GetILogWriter()->WriteLn(CLogValue("[ERROR] Resource not accepted, m_size = 0, m_loadingState = RS_NOTACCEPTED (5) m_fileName = ", m_fileName.c_str()));
		}
		else
		{
			m_loadingState = resources::RS_NOTEXISTS;
			if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
				CObjStorage::GetInstance()->GetILogWriter()->WriteLn(CLogValue("[ERROR] Resource not read, m_size = 0, m_loadingState = RS_NOTEXISTS (6) m_fileName = ", m_fileName.c_str()));
		}
	}
	LeaveCriticalSection(&m_stateCS);
}

resources::RESOURCE_STATE CAsynchResource::GetLoadingState()
{
	EnterCriticalSection(&m_stateCS);

	resources::RESOURCE_STATE state = m_loadingState;

	LeaveCriticalSection(&m_stateCS);

	return state;
}

void CAsynchResource::SetLoadedState()
{
	m_readBytes = m_size;

	UpdateLoadingState();
}

void CAsynchResource::SetPriority(float afPriority)
{
	m_priority = afPriority;
}

float CAsynchResource::GetPriority()
{
	return m_priority;
}

void CAsynchResource::SetMainPriority(unsigned int priority)
{
	m_mainPriority = priority;
}

unsigned int CAsynchResource::GetMainPriority()
{
	return m_mainPriority;
}

float CAsynchResource::GetLoadedPercent()
{
	if(m_size == 0)	return 0.0f;
	return (float)(m_readBytes) / (float)(m_size);
}

unsigned int CAsynchResource::GetBytesLoaded()
{
	return m_readBytes;
}

ifile* CAsynchResource::GetIFile()
{
	if(m_file)
	{
		if( m_readBytes > 0)
			m_file->attach((unsigned char*)&m_buffer[0], m_readBytes);
		USES_CONVERSION;
		m_file->set_file_path( A2W(m_fileName.data()));
		m_file->set_modify_time(m_time);
	}
	return m_file;
}

void CAsynchResource::AddObject(void* vs, float koef)
{
	if (vs != NULL)
	{
		m_objects.push_back(vs);
		m_koefs.push_back(koef);
	}
}

void CAsynchResource::GetObject(int num, void** obj, float* koef)
{
	*koef = m_koefs[num];
	*obj = m_objects[num];
}

int	CAsynchResource::GetObjectCount()
{
	return m_objects.size();
}

char* CAsynchResource::GetFileName()
{
	return (char*)m_fileName.c_str();
}

void CAsynchResource::Pause()
{
	m_loadingState = resources::RS_PAUSED;
}

bool CAsynchResource::IsPause()
{
	return m_loadingState == resources::RS_PAUSED;
}

void CAsynchResource::Continue()
{
	m_loadingState = resources::RS_LOADING;
}

void CAsynchResource::CloseIResource()
{
	if (m_res)
	{
		m_res->Close();
		m_res = NULL;
	}
}

void CAsynchResource::Release()
{
	if (m_file)
	{
		m_file->close();
		MP_DELETE( m_file);
		m_file = NULL;
	}

	CloseIResource();
}

void	CAsynchResource::SetImportantSlideStatus(bool isImportant)
{
	m_isImportantSlide = isImportant;
}

bool	CAsynchResource::IsImportantSlide()
{
	return m_isImportantSlide;
}

CAsynchResource::~CAsynchResource()
{
	Release();
}