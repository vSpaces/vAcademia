#include "stdafx.h"
#include "ComMan.h"
#include "ComManager.h"
#include "ResMan.h"
#include "ClientSession.h"
#include "UrlParser.h"
#include "shlobj.h"
#include "asyncmoduleimpl.h"
#include "ZLIBENGN.H"
#include "CrashHandlerInThread.h"

using namespace res;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CResourceNotify :	public IResourceNotify
{
	friend	class	resResourceManagerWrapper;
public:
	CResourceNotify() : pAsynchResource( NULL)
	{
	}
	virtual	~CResourceNotify()
	{
	}

public:	
	long onResourceNotify( IResource* apIResource, DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t* aNotifyServerText)
	{
		if (pAsynchResource != NULL)
		{
			switch(aNotifyCode)
			{
				case RN_ASYNCH_DOWNLOADED:
					pAsynchResource->OnDownloaded();
					break;
				case RN_ASYNCH_CACHED:
					pAsynchResource->OnCached();
					break;
				case RN_ASYNCH_UNPACKED:
					pAsynchResource->OnUnpacked();
					break;
				case RN_ASYNCH_UPLOADED:
					pAsynchResource->OnUploaded(aNotifyServerText);
					break;
				case RN_ASYNCH_REGISTERED:
					pAsynchResource->OnRegistered(aNotifyServerText);
					break;
				default:
					pAsynchResource->OnError(aNotifyCode, aNotifyServerCode, aNotifyServerText);
			}
		}
		if( apIResource == NULL)
			return S_OK;
		return S_OK;
	}

	void destroy()
	{
		MP_DELETE_THIS;
	}

	void setAsynchResource( void* aAsynch)
	{
		pAsynchResource = (resIAsynchResource*)aAsynch;
	}
	bool isNeedDestroyByComman(){return true;};
private:
	resIAsynchResource* pAsynchResource;
};

/*class resAuthResource: public resIResource
{
	istream* mpIStream;
	iostream* mpIOStream;
	res_streambuf* mpStreamBuf;
	std::wstring mwsURL;
	unsigned char* mpData;
	long mlSize;
	long mlPos;
public:
	resAuthResource(void* apData, int aiSize, const wchar_t* apwcURL){
		mpStreamBuf = NULL;
		mpIStream = NULL;
		mpIOStream = NULL;
		if(apData == NULL || aiSize < 0){
			mpData = NULL;
			mlSize = 0;
		}else{
			mlSize = (unsigned) aiSize;
			mpData = MP_NEW_ARR( unsigned char, mlSize);
			rtl_memcpy(mpData, mlSize, apData, mlSize);
		}
		if(apwcURL != NULL)
			mwsURL = apwcURL;
		mlPos = 0;
	}
	virtual ~resAuthResource(){
		if(mpIStream)
			MP_DELETE( mpIStream);
		if(mpIOStream!=NULL)
			MP_DELETE( mpIOStream);
		if( mpStreamBuf)
			MP_DELETE( mpStreamBuf);
		if(mpData != NULL)
			MP_DELETE_ARR( mpData); 
	}
	omsresult GetIStream(istream* &apistrm){
		MP_NEW_V( mpStreamBuf, res_streambuf, this);
		MP_NEW_V( mpIStream, istream, mpStreamBuf);
		apistrm=mpIStream;
		return OMS_OK;
	}
	omsresult GetStream(oms::Stream* &apOMSStream){
		return OMS_ERROR_NOT_IMPLEMENTED;
	}
	unsigned int Read(unsigned char* aBuffer, unsigned int aBufferSize){
		unsigned int uRead = aBufferSize;
		if((mlSize - mlPos) < uRead) uRead = mlSize - mlPos;
		if(uRead == 0)
			return uRead;
		rtl_memcpy(aBuffer, aBufferSize, mpData+mlPos, uRead);
		mlPos += uRead;
		return uRead;
	}
	unsigned int Write( unsigned char* aBuffer, unsigned int aBufferSize){ return 0; }
	bool Seek( long aPos, unsigned char aFrom){
		// ??
		return false;
	}
	unsigned long GetPos(){ return mlPos; }
	unsigned long GetSize(){ return (long)mlSize; }
	void Close(){
		MP_DELETE_THIS;
	}
	const wchar_t* GetURL(){
		return mwsURL.c_str();
	}


	bool GetFileNameRefineLoaded(wchar_t **wsName)
	{
		*wsName = (wchar_t*)mwsURL.c_str();
		return false;
	}

	//void Download( res::resIAsynchResource* aAsynch){}
	void DownloadToCache( res::resIAsynchResource* aAsynch){}
	bool Unpack(resIAsynchResource* aAsynch){ return true; }
	bool UnpackToCache(resIAsynchResource* aAsynch){ return true; }
	unsigned long GetDownloadedSize(){ return 0; }
	unsigned int GetState(){ return 0; }
	unsigned int GetUnpackingProgress(){ return 0; }
	bool Upload(resIResource* apSrcRes, resIAsynchResource* aAsynch=NULL){ return false; }
	unsigned long GetUploadedSize(){ return 0; }
};*/

bool wstr2local( const wchar_t* apwcStr, unsigned char** apBuffer, unsigned int* auiBufSize)
{
	//*auiBufSize = wcstombs( NULL, apwcStr, 0) + 1;
	wcstombs_s( auiBufSize, NULL, 0, apwcStr, wcslen(apwcStr));
	*apBuffer = MP_NEW_ARR( unsigned char, *auiBufSize + 1);
	//*auiBufSize = wcstombs( (char*)*apBuffer, apwcStr, *auiBufSize);
	wcstombs_s( auiBufSize, (char*)*apBuffer, *auiBufSize, apwcStr, wcslen(apwcStr));
	return true;
}

void CComManager::PackResource(wchar_t* path, wchar_t* newPath)
{
	unsigned char* src = NULL;
	unsigned char* newSrc = NULL;
	unsigned int strSize = 0;
	wstr2local(path, &src, &strSize);
	wstr2local(newPath, &newSrc, &strSize);
			
	ZlibEngine zip;
	zip.compress((const char *)src, (const char *)newSrc, Z_BEST_COMPRESSION);
	
	MP_DELETE_ARR( src);
	MP_DELETE_ARR( newSrc);
}

std::wstring CComManager::RefineURL(const wchar_t* apwcURL, IResModule** aModule, DWORD dwFlags, unsigned int &aCodeError){
	aCodeError = 0;
	if(apwcURL == NULL || *apwcURL==L'\0')
		return L"";
	if(*apwcURL == L':') apwcURL++;
	wstring wsURL=apwcURL;

	if( (dwFlags & RES_LOCAL) != 0 && (dwFlags & RES_REMOTE) == 0)
	{
		int pos = wsURL.find(L"geometry");
		if( pos >-1)
		{
			wsURL.insert( pos + 8, L":");
		}
	}

	int	idx;
	if (aModule != NULL)
		*aModule = NULL;
	// »щем алиас
	if ((idx = wsURL.rfind(L":")) != wstring::npos){
		wstring wsAlias = wsURL.substr(0, idx);
		std::vector<resAliasDesc>::iterator	mvit;
		mutex.lock();
		for (mvit=mvResAliases.begin(); mvit!=mvResAliases.end(); mvit++){
			if(mvit->msAlias == wsAlias){
				//				if (mvit->msOwnerModuleID != wsOwnerModuleID) continue;
				if (wsURL[ idx+1] != L'\\' && wsURL[ idx+1] != L'/')
					wsURL.replace(idx, 1, L"\\");
				else
					idx++;
				bool bError = false;
				if (mvit->spModule != NULL)
				{
					if (aModule == NULL)
					{
						CWComString bsURL;
						//CComBSTR bsURL;
						bError = FAILED( mvit->spModule->getFullPath( (BSTR)&(wsURL.data()[ idx]), 0, bsURL));
						if (!bError)
							wsURL = bsURL.GetBuffer();
					}
					else
					{
						// ќпредел€ем путь дл€ работы через IResModule
						wsURL.erase(0, idx);
						*aModule = mvit->spModule;
						//(*aModule)->AddRef();
					}
				}
				if (bError)
				{
					// ќпредел€ем путь дл€ работы через IResMan напр€мую
					wsURL.replace(0, idx, mvit->msPath);
					wsURL = msLocalRepBase + MODULESBASE + wsURL;
				}
				mutex.unlock();
				return	wsURL;
			}
		}
		mutex.unlock();
		int	idx2;
		if ((idx2=wsURL.rfind(L'.',idx)) != wstring::npos 
			|| wsURL.rfind(L"ui:",0) != wstring::npos 
			|| wsURL.find(L"geometry:", 0) != wstring::npos) 
		{
			if (aModule != NULL)
			{
				// ќпредел€ем путь дл€ работы через IResModule
				wstring wsResModuleID = wsURL.substr(0, idx);
				if (wsResModuleID == wsModuleID)
					*aModule = mspModule;
				else
				{
					if( (dwFlags & RES_ONLY_CACHE) != RES_ONLY_CACHE)
					{
						aCodeError = AddResourceAliasAndPath( wsResModuleID.data(), L"", wsResModuleID.data(), dwFlags);
						mutex.lock();
						std::vector<resAliasDesc>::reverse_iterator	mvit = mvResAliases.rbegin();
						if (mvit != mvResAliases.rend() && mvit->msAlias == wsResModuleID)
							*aModule = mvit->spModule;
						if(aCodeError == OMS_ERROR_RESOURCE_FOUND_TIMEOUT)
						{
							mutex.unlock();
							return wsURL;
						}
						mutex.unlock();
					}
				}
				if (*aModule != NULL)
				{
					//(*aModule)->AddRef();
					wsURL.replace(idx, 1, L"\\");
					wsURL.erase(0, idx);
				}
			}
			if (aModule == NULL || *aModule == NULL)
			{
				// ќпредел€ем путь дл€ работы через IResMan напр€мую
				wsURL.replace(idx, 1, L"\\");
				while((idx2=wsURL.find(L".")) != wstring::npos){
					if(idx2 > idx) break;
					wsURL.replace(idx2, 1, L"\\");
				}
				wsURL = msLocalRepBase + MODULESBASE + wsURL;
			}
			return	wsURL;
		}
	}
	if(wsURL[1]!=L':'){
		if(wsURL[0]==L'u' && wsURL.find(L"usr:")==0){
			if(!msUserBase.empty()){
				// ??
			}
		}else if(wsURL[0]==L'r' && wsURL.find(L"remote:")==0)
		{
			wsURL.erase(0,7);
				CComBSTR bsServer;
				CComBSTR bsComManServer;
				CComBSTR bsRemoteBase;
				this->getServer( &bsComManServer);
				this->getRemoteBase( &bsRemoteBase);
				if( bsComManServer.Length() > 0 
					&& bsComManServer[bsComManServer.Length()-1] != L'/'
					&& bsRemoteBase[0] != L'/'
				)
					bsComManServer += L"/";
				bsComManServer += bsRemoteBase;
				if(  wcsstr(bsComManServer.m_str, L"http://") != bsComManServer.m_str)
				{
					bsServer = L"http://";
					bsServer +=  bsComManServer;
				}
				else
				{
					bsServer = bsComManServer;
				}
				if( bsServer.Length() > 0 && bsServer[bsServer.Length()-1] != L'/')
					bsServer += L'/';
				if(wsURL[0] == L'/')
					wsURL.erase(0,1);
				wsURL.insert(0, bsServer);
		}else if(wsURL[0]==L'm' && wsURL.find(L"module:")==0){
			wsURL.erase(0,7);
			wsURL.insert(0,msLocalRepBase);
			wsURL.append( L".pack");
		}else if(wsURL[0]==L'l' && wsURL.find(L"lr:")==0){
			ATLASSERT(FALSE);
			wsURL.erase(0,3);
			wsURL.insert(0,msLocalRepBase);
		}else if(wsURL.find(L"http://")==0 || wsURL.find(L"ftp://")==0){			
		}
		else if (wsURL[0] == L'\\' && wsURL[1] == L'\\')
		{
			return wsURL;
		}
		else if (aModule != NULL && mspModule != NULL)
		{
			*aModule = mspModule;
			wsURL.insert( 0, L"\\");
		}
		else if (!msModuleBase.empty())
			wsURL.insert(0,msModuleBase);
	}
	return wsURL;
}

inline bool IsEqual(const wchar_t* apch1, const wchar_t* apch2, unsigned int auLen){
	for(; auLen>0; auLen--){
		if(*apch1!=*apch2) return false;
		apch1++, apch2++;
	}
	return true;
}

////////////////////////////////////////////////////
// IResMan

void CComManager::SetConnectType( byte iType )
{
	oResMan.SetTransferType(iType);//1-HTTP, 2-Socket, default value 1
}

long CComManager::openResource(BSTR aResPath, DWORD aFlags, IResource** aRes)
{	
	return oResMan.openResource( aResPath, aFlags, aRes);
}

long CComManager::openAsynchResource(BSTR aResPath, DWORD aFlags, IResourceNotify *aNotify, IResource **aRes)
{
	if( (aFlags & RES_ASYNCH) == 0)
	{
		aFlags |= RES_ASYNCH;
	}
	DWORD cache = (RES_FILE_CACHE|RES_MEM_CACHE);
	if( ((aFlags & cache) == 0) ||
		((aFlags & cache) == cache))
	{
		aFlags |= RES_MEM_CACHE;
	}
	return oResMan.openResource( CWComString( aResPath), aFlags, aRes, aNotify);
}

omsresult CComManager::OpenAsynchResource(const wchar_t* apwcURL, resIResource* &ares, IResourceNotify* apNotifier, unsigned int dwFlags)
{
	//if( !spResourceMan)	return OMS_ERROR_NOT_INITIALIZED;
	if( !apNotifier)	return OMS_ERROR_INVALID_ARG;
	mbUsed=true;
	//

	unsigned int codeError = 0;
	IResModule* spResModule = NULL;
	wstring wsURL=RefineURL(apwcURL, &spResModule, dwFlags, codeError);
	if( codeError == OMS_ERROR_RESOURCE_FOUND_TIMEOUT)
	{
		if( m_pLW != NULL)
			m_pLW->WriteLn(CLogValue("[WARNING] CComManager::OpenAsynchResource::server not answer for file: ", CWComString(wsURL.c_str()).GetBuffer(), " dwFlags=="+dwFlags));
		return OMS_ERROR_RESOURCE_FOUND_TIMEOUT;
	}

	IResource* spResource = NULL;

	if (spResModule != NULL ?
		SUCCEEDED(spResModule->openAsynchResource((BSTR)wsURL.data(), dwFlags, apNotifier, &spResource)) :
	    SUCCEEDED(this->openAsynchResource((BSTR)wsURL.data(), dwFlags, apNotifier, &spResource)))
	{
		// убрана проверка на статус, чтобы не устанавливалс€ размер файла в -1 при отсуствии его в списке файлов со сервера.
		/*DWORD dwStatus;
		spResource->getStatus( &dwStatus);
		if ((dwStatus & RES_EXISTS) == 0){
			//			TRACE("Cannot open XML-file (%S)\n",apwcSrc);
			return OMS_ERROR_RESOURCE_NOT_FOUND;
		}*/
		MP_NEW_P3( pRes, resResource, spResource, wsURL.c_str(),  m_pLW);
		ares = pRes;
		return OMS_OK;
	}
	if(spResource!=NULL)
	{
		spResource->destroy();
	}

	return OMS_ERROR_RESOURCE_NOT_FOUND;
}

omsresult CComManager::OpenResource(const wchar_t* apwcURL, resIResource* &ares, unsigned int dwFlags)
{
	CWComString sResPath = (BSTR)apwcURL;

	mbUsed=true;
	//
#if ( defined LOG_REMOTE_RESOURCE | defined LOG_LOCAL_RESOURCE)	
	if (m_pLW != NULL)
	{
		m_pLW->WriteLn(CLogValue("CComManager::OpenResource path = ", sResPath.GetBuffer()));
	}
#endif

	IResModule* spResModule = NULL;	

	unsigned int codeError = 0;
	wstring wsURL=RefineURL(apwcURL, &spResModule, dwFlags, codeError);
	if( codeError == OMS_ERROR_RESOURCE_FOUND_TIMEOUT)
	{
		if( m_pLW != NULL && apwcURL != NULL)
		{
			CComString s;
			s.Format("[WARNING] CComManager::OpenResource::server not answer for file: %s,  dwFlags==%d", WideToChar(apwcURL), dwFlags);
			m_pLW->WriteLnLPCSTR( s.GetBuffer());
		}
		return OMS_ERROR_RESOURCE_FOUND_TIMEOUT;
	}

	IResource* spResource = NULL;
	
	if(wcsstr( wsURL.data(), L"player1.xml") != NULL || wcsstr( wsURL.data(), L"control1.xml"))
		dwFlags &= ~RES_FILE_CACHE;

#if LOG_LOCAL_RESOURCE	
	if (m_pLW != NULL)
	{
		CWComString s = (BSTR)wsURL.data();
		m_pLW->WriteLn(CLogValue("CComManager::OpenResource afterRefine path = ", s.GetBuffer()));
	}
#endif

	if (spResModule != NULL ?
		SUCCEEDED(spResModule->openResource( (wchar_t*)wsURL.c_str(), dwFlags, &spResource)) :
	SUCCEEDED(this->openResource( (BSTR)wsURL.c_str(), dwFlags, &spResource)))
	{
		DWORD dwStatus;
		spResource->getStatus( &dwStatus);
		if ((dwStatus & RES_EXISTS) == 0){
			spResource->destroy();
			//CWComString s = (BSTR)wsURL.c_str();
			if((dwStatus & RES_SERVER_ERROR_TIMEOUT) == RES_SERVER_ERROR_TIMEOUT && (dwFlags&RES_LOCAL)==0)
			{
				/*if( m_pLW != NULL)
					m_pLW->WriteLn(CLogValue("[ERROR] server not answer for file: ", s.GetBuffer()));*/
				return OMS_ERROR_RESOURCE_FOUND_TIMEOUT;
			}
			/*if( m_pLW != NULL)
				m_pLW->WriteLn(CLogValue("[ERROR] OMS_ERROR_RESOURCE_NOT_FOUND for file: ", s.GetBuffer()));*/
			return OMS_ERROR_RESOURCE_NOT_FOUND;
		}
		MP_NEW_P3( pRes, resResource, spResource, wsURL.c_str(), m_pLW);
		ares = pRes;
		return OMS_OK;
	}
	spResource->destroy();
	return OMS_ERROR_RESOURCE_NOT_FOUND;
}

void CComManager::GetIResourceAndClose(resIResource* ares, LPVOID *appRes){
	if(ares == NULL)
		return;
	resResource* pRes = (resResource*)ares;
	pRes->GetIResourceAndClose( (IResource**)appRes);
}

omsresult CComManager::OpenToWrite(const wchar_t* apwcURL, resIWrite* &ares, unsigned int adwFlags){
	//if( !spResourceMan)	return OMS_ERROR_NOT_INITIALIZED;
	mbUsed=true;
	//
	IResModule* spResModule = NULL;
	unsigned int codeError = 0;
	wstring wsURL=RefineURL(apwcURL, &spResModule, 0, codeError);
	if( codeError == OMS_ERROR_RESOURCE_FOUND_TIMEOUT)
		return OMS_ERROR_RESOURCE_FOUND_TIMEOUT;
	IResource* spResource = NULL;
	if(adwFlags == 0){
		adwFlags = RES_LOCAL | RES_CREATE | RES_UPLOAD;
	}else{
		adwFlags |= RES_CREATE | RES_TO_WRITE | RES_UPLOAD;
	}
	if (spResModule != NULL ?
		SUCCEEDED(spResModule->openResource((BSTR)wsURL.data(), adwFlags, &spResource)) :
	SUCCEEDED(this->openResource((BSTR)wsURL.data(), adwFlags, &spResource)))
	{
		DWORD dwStatus;
		spResource->getStatus( &dwStatus);
		if ((dwStatus & RES_EXISTS) == 0){
			spResource->destroy();
			//			TRACE("Cannot open XML-file (%S)\n",apwcSrc);
			return OMS_ERROR_RESOURCE_NOT_FOUND;
		}
		MP_NEW_P3( pRes, resResource, spResource, NULL, pContext->mpLogWriter);
		ares = pRes;
		return OMS_OK;
	}

	return OMS_ERROR_RESOURCE_NOT_FOUND;
}

bool CComManager::ResourceExists(const wchar_t* apwcURL, unsigned int dwFlags, bool abCheckOnlyExistInList, unsigned int &aCodeError){
	//if(!spResourceMan)	return false;
	mbUsed=true;
	if(apwcURL == NULL) return false;
	if( dwFlags==0)
	{
		dwFlags = RES_LOCAL;
	}
	IResModule* spResModule = NULL;
	wstring wsURL=RefineURL(apwcURL, &spResModule, dwFlags, aCodeError);
	if( aCodeError == OMS_ERROR_RESOURCE_FOUND_TIMEOUT)
	{
		if( m_pLW != NULL)
			m_pLW->WriteLn(CLogValue("[WARNING] CComManager::ResourceExists::server not answer for file: ", CWComString(wsURL.c_str()).GetBuffer(), " dwFlags=="+dwFlags));
		return false;
	}
	DWORD dwStatus;
	if (spResModule != NULL){
		if(FAILED(spResModule->resourceExists((BSTR)wsURL.data(), dwFlags, &dwStatus, abCheckOnlyExistInList)))
			return false;
	}else
		if(FAILED(this->resourceExists((BSTR)wsURL.data(), dwFlags, &dwStatus, abCheckOnlyExistInList)))
			return false;
	return ((dwStatus & RES_EXISTS)?true:false);
}

bool CComManager::ResourceExists(const wchar_t* apwcURL, unsigned int &aCodeError)
{
	return ResourceExists( apwcURL, 0, false, aCodeError);
}

bool CComManager::ResourceExists(const wchar_t* apwcURL, unsigned int dwFlags, unsigned int &aCodeError)
{
	return ResourceExists( apwcURL, dwFlags, false, aCodeError);
}

DWORD WINAPI CComManager::CheckExistThread_( LPVOID aQuery) {
	CAsyncCheckExist* pQuery = (CAsyncCheckExist*)aQuery;
	if (pQuery != NULL && pQuery->pHost != NULL)
		pQuery->pHost->CheckExistThread( pQuery);
	return 0;
}

DWORD CComManager::CheckExistThread( CAsyncCheckExist* pAsyncCheckExist) 
{
	unsigned int codeError = 0;
	bool bRes = ResourceExists( pAsyncCheckExist->wsFilePath.c_str(), pAsyncCheckExist->dwFlags, pAsyncCheckExist->bCheckOnlyExistInList, codeError);
	while( codeError == OMS_ERROR_RESOURCE_FOUND_TIMEOUT)
		bRes = ResourceExists( pAsyncCheckExist->wsFilePath.c_str(), pAsyncCheckExist->dwFlags, pAsyncCheckExist->bCheckOnlyExistInList, codeError);
	pAsyncCheckExist->mutex.lock();
	if( pAsyncCheckExist->getHandler() != NULL)
	{
		if(bRes)
			pAsyncCheckExist->getHandler()->OnExisted( true);
		else
			pAsyncCheckExist->getHandler()->OnExisted( false);
	}
	pAsyncCheckExist->mutex.unlock();
	CloseHandle( pAsyncCheckExist->hQueryThread);
	pAsyncCheckExist->hQueryThread = NULL;
	return 0;
}

IAsyncCheckExist *CComManager::ResourceExistsAsync( resIAsynchResource* aAsynch, const wchar_t* apwcURL, unsigned int dwFlags, bool abCheckOnlyExistInList){

	if( apwcURL == NULL)
		return NULL;
	if( aAsynch == NULL)
		return NULL;

	CAsyncCheckExist *pNotify = MP_NEW( CAsyncCheckExist);
	pNotify->setHandler( aAsynch);
	pNotify->pHost = this;
	pNotify->wsFilePath = apwcURL;
	pNotify->dwFlags = dwFlags;
	pNotify->bCheckOnlyExistInList = abCheckOnlyExistInList;

	CSimpleThreadStarter queryThreadStarter(__FUNCTION__);	
	queryThreadStarter.SetRoutine(CheckExistThread_);
	queryThreadStarter.SetParameter(pNotify);
	pNotify->hQueryThread  = queryThreadStarter.Start();

	return (IAsyncCheckExist*)pNotify;
}

bool CComManager::GetModuleExists(const wchar_t* apwcModuleID, unsigned int dwFlags, void **ppModule, unsigned int *aStatus)
{
	if(/*!spResourceMan ||*/ apwcModuleID == NULL || *apwcModuleID == L'\0')	return false;
	mbUsed=true;

	wstring wsResModuleID = apwcModuleID;
	DWORD dwStatus;
	if ((dwFlags & RES_ANYWHERE) == 0)
		dwFlags |= RES_REMOTE | RES_LOCAL | RES_MODULE_READY;
	else
		dwFlags |= RES_MODULE_READY;

	long res = this->getModuleExists((BSTR)wsResModuleID.data(), dwFlags, &dwStatus, (IResModule**)ppModule);
	if( res != S_OK)
	{
		*aStatus = dwStatus;
		return false;
	}
	*aStatus = dwStatus;
	if(RES_IS_ERROR(dwStatus)) 
		return false;
	bool bExist = ((dwStatus & RES_EXISTS) != 0);
	return bExist;
}

void CComManager::DestroyModule( void **ppModule)
{
	 IResModule *spModule = (IResModule *)*ppModule;
	 spModule->destroy();
	 spModule = NULL;
}

wchar_t* CComManager::GetFullPath(const wchar_t* apwcURL, DWORD aFlags){
	//if(!spResourceMan)	return NULL;
	mbUsed=true;	
	IResModule *spResModule = NULL;
	unsigned int codeError = 0;
	wstring wsURL=RefineURL(apwcURL, &spResModule, 0, codeError);
	if (spResModule != NULL){
		if(FAILED(spResModule->getFullPath((BSTR)wsURL.data(), aFlags, mbstrFullPath)))
			return L"";
	}else
		if(FAILED(this->getFullPath((BSTR)wsURL.data(), aFlags,mbstrFullPath)))
			return L"";
	return mbstrFullPath.GetBuffer();
}

omsresult CComManager::AddResourceAliasAndPath(const wchar_t* apwcAlias,  const wchar_t* apwcOwnerModuleID, 
															 const wchar_t* apwcResModuleID, unsigned int dwFlags)
{
	if( !apwcAlias || !apwcResModuleID){
		/*mutex.lock();
		mvResAliases.clear();
		mutex.unlock();*/
		return OMS_OK;
	}
	IResModule *spModule = NULL;

	resAliasDesc	desc;
	desc.msAlias = (*apwcAlias != L'\0' ? apwcAlias : apwcResModuleID);
	desc.msOwnerModuleID = apwcOwnerModuleID;
	desc.msModuleID = apwcResModuleID;
	desc.msPath = apwcResModuleID;

	int	idx;
	while((idx=desc.msPath.find(L".")) != wstring::npos)
		desc.msPath.replace(idx, 1, L"\\");

	mutex.lock();
	std::vector<resAliasDesc>::iterator	mvit;
	for (mvit=mvResAliases.begin(); mvit!=mvResAliases.end(); mvit++)
	{
		if(mvit->msAlias == desc.msAlias && 
			mvit->msOwnerModuleID == desc.msOwnerModuleID && 
			mvit->msPath != desc.msPath)
		{
			mutex.unlock();	// alex. ¬ параллельном потоке может идти добавление (см. ниже) и итератор сломаетс€
			return OMS_ERROR_ILLEGAL_VALUE;
		}
		/*if(mvit->msModuleID == apwcResModuleID)
		{
			spModule = mvit->spModule;
			mutex.unlock();
			break;
		}*/
	}
	mutex.unlock();	
	if ( spModule == NULL)
	{
		unsigned int dwCodeError = 0;	
		if ( !GetModuleExists(apwcResModuleID, dwFlags, (void **)&spModule, &dwCodeError))
		{					
			if(spModule != NULL)
			{
				spModule->getStatus( (DWORD*)&dwCodeError);
				spModule->destroy();
				spModule = NULL;
			}
			if((dwCodeError & RES_SERVER_ERROR_TIMEOUT) == RES_SERVER_ERROR_TIMEOUT)
			{
				return OMS_ERROR_RESOURCE_FOUND_TIMEOUT;
			}			
			return OMS_ERROR_RESOURCE_NOT_FOUND;
		}
	}

	desc.spModule = spModule;	
	DWORD dwStatus;
	desc.spModule->getStatus( &dwStatus);
	if ((dwStatus & RES_EXISTS) == 0)
		desc.spModule = NULL;
	mutex.lock();
	mvResAliases.push_back(desc);
	mutex.unlock();

	return OMS_OK;
}

bool CComManager::GetModuleIDByAlias(const wchar_t* apwcAlias, const wchar_t* apwcOwnerModuleID, wchar_t **apModuleID, DWORD aSize)
{
	std::vector<resAliasDesc>::iterator	mvit;
	mutex.lock();
	for (mvit=mvResAliases.begin(); mvit!=mvResAliases.end(); mvit++)
	{
		if(mvit->msAlias == apwcAlias && mvit->msOwnerModuleID == apwcOwnerModuleID)
		{
			mutex.unlock();
			rtl_wcscpy(*apModuleID, aSize, mvit->msModuleID.c_str());
			return true;
			//return mvit->msModuleID.c_str();
		}
	}
	mutex.unlock();
	return false;
	//return NULL;
}

omsresult CComManager::CreateModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool aRemote, bool aAsynchMode, resIModuleNotify* apResIModuleNotify)
{
	if( aAsynchMode)
	{
		MP_NEW_P( moduleTask, CAsyncModuleCreationImpl, this);
		moduleTask->CreateModule( apwcModuleID, apwcAlias, aRemote, apResIModuleNotify);
		return OMS_OK;
	}

	IResModule *spNewModule = NULL;
	DWORD dwStatus;

	if (aRemote)
	{
		if (FAILED( this->openModule( (BSTR)apwcModuleID, RES_REMOTE | RES_CREATE, &spNewModule)))
			return OMS_ERROR_RESOURCE_NOT_FOUND;
	}
	else
	{
		if (FAILED( this->openModule( (BSTR)apwcModuleID, RES_LOCAL | RES_CREATE, &spNewModule)))
			return OMS_ERROR_RESOURCE_NOT_FOUND;
	}
	if (FAILED( spNewModule->getStatus( &dwStatus)) || (dwStatus & RES_EXISTS) == 0)
		return OMS_ERROR_RESOURCE_NOT_FOUND;
	spNewModule->destroy();
	return AddResourceAliasAndPath( apwcAlias, L"", apwcModuleID);
}

omsresult CComManager::OpenModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool aAsynchMode, resIModuleNotify* apResIModuleNotify)
{
	if( aAsynchMode)
	{
		MP_NEW_P( moduleTask, CAsyncModuleOpenImpl, this);
		moduleTask->OpenModule( apwcModuleID, apwcAlias, apResIModuleNotify);
		return OMS_OK;
	}
	return AddResourceAliasAndPath( apwcAlias, L"", apwcModuleID);
}


omsresult CComManager::CloseModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias)
{
	resAliasDesc	desc;
	desc.msAlias = apwcAlias;
	desc.msModuleID = apwcModuleID;

	std::vector<resAliasDesc>::iterator	mvit;
	mutex.lock();
	for (mvit=mvResAliases.begin(); mvit < mvResAliases.end(); mvit++)
	{
		if(mvit->msAlias == desc.msAlias && mvit->msModuleID == desc.msModuleID)
		{
			IResModule *pResModule = mvit->spModule;
			pResModule->destroy();
			mvit = mvResAliases.erase( mvit);
		}
	}
	mutex.unlock();
	return OMS_OK;
}

omsresult CComManager::CopyResource(const wchar_t* apwcSrcURL, const wchar_t* apwcDestURL)
{
	IResModule* spSrcModule = NULL;
	unsigned int codeError = 0;
	wstring wsSrcURL = RefineURL(apwcSrcURL, &spSrcModule, 0, codeError);

	IResModule* spDestModule = NULL;
	wstring wsDestURL = RefineURL(apwcDestURL, &spDestModule, 0, codeError);

	if (spSrcModule != NULL && spDestModule != NULL &&
		SUCCEEDED( spSrcModule->copyResource( (BSTR)wsSrcURL.data(), spDestModule, (BSTR)wsDestURL.data())))
		return OMS_OK;
	else
		return OMS_ERROR_RESOURCE_NOT_FOUND;
}

omsresult CComManager::DownloadModule( const wchar_t* apwcModuleID)
{
	pContext->mpApp->ExternCommand( L"DownloadModule", apwcModuleID);
	return OMS_OK;
}

const wchar_t* CComManager::Extract(const wchar_t* apwcSrc, const wchar_t* apwcSubPath){
	//if(!spResourceMan) return NULL;

	IResModule* spSrcModule = NULL;
	unsigned int codeError = 0;
	wstring wsSrc = RefineURL(apwcSrc, &spSrcModule, 0, codeError);
	wstring wsSubPath = apwcSubPath;

	mbstrFullPathC.Empty();
	if(spSrcModule != NULL && SUCCEEDED(spSrcModule->extract((wchar_t*)wsSrc.data(), (wchar_t*)wsSubPath.data(), mbstrFullPathC)))
		return mbstrFullPathC.AllocSysString();
	return NULL;
}

const wchar_t* CComManager::GetTaxonomy(const wchar_t* apwcModuleID)
{
	mbstrTaxonomy.Empty();
	IResModule* pModule = NULL;
	if (apwcModuleID != NULL)
	{
		std::wstring msModID( apwcModuleID);
		std::vector<resAliasDesc>::iterator	mvit;
		mutex.lock();
		for (mvit=mvResAliases.begin(); mvit < mvResAliases.end(); mvit++)
		{
			if (mvit->msModuleID == msModID)
			{
				pModule = mvit->spModule;
				break;
			}
		}
		mutex.unlock();
	}
	else if (mspModule != NULL)
		pModule = mspModule;
	if (pModule != NULL)
	{
		//CComPtr< IResModuleMetadata> spMetadata;
		IResModuleMetadata *spMetadata = NULL;
		pModule->getMetadata( &spMetadata);
		if (spMetadata != NULL)
			spMetadata->getTaxonomy( mbstrTaxonomy);
		return mbstrTaxonomy.AllocSysString();
	}
	else
		return NULL;
}

/*const wchar_t** CComManager::GetRBRList(const wchar_t* apwcModuleID){
	// ??
	return NULL;
}*/

const wchar_t* CComManager::GetCacheBase(){
	// ??
	mbstrCacheBase.Empty();
	this->getCacheBase( mbstrCacheBase);
	return mbstrCacheBase.AllocSysString();
}

/*void CComManager::CloseResource(resResource* apRes){
	if(apRes)
	{
		delete apRes;
	}
}*/

wchar_t* CComManager::GetLastError(){
	return L"Unknown error";
}

bool CComManager::SetModuleBase(const wchar_t* apwcModuleBase){ 
	wstring wsModuleBase;
	if(apwcModuleBase==NULL || *apwcModuleBase==L'\0'){
	}else{
		// если дали ссылку на xml-файл, то...
		std::wstring ws(apwcModuleBase);
		int iLen=ws.length();
		if(iLen>4 && ws[iLen-4]==L'.'){
			if((ws[iLen-3]==L'x' || ws[iLen-3]==L'X') &&
				(ws[iLen-2]==L'm' || ws[iLen-2]==L'M') &&
				(ws[iLen-1]==L'l' || ws[iLen-1]==L'L')
				){
					for(int ii=0; ii<iLen; ii++)
						if(ws[ii]==L'/') ws[ii]=L'\\';
					// ... то ищем \META-INF\manifest.xml
					// using namespace std;
					std::wstring::size_type pos;
					static const std::wstring::size_type npos = std::wstring::npos;
					while((pos=ws.rfind(L'\\'))!=npos){
						ws.erase(pos);
						wstring ws2=ws+MANIFESTXML;
						unsigned int codeError = 0;
						if(ResourceExists(ws2.c_str(), codeError)){
							wsModuleBase=ws;
							break;
						}
					}
					// если не нашли, то...
					if(wsModuleBase.empty()){
						ws=apwcModuleBase;
						if((pos=ws.rfind(L'\\'))!=npos){
							ws.erase(pos);
							wsModuleBase=ws;
						}
					}
				}
		}
		if(wsModuleBase.empty()){
			ws = apwcModuleBase;
			int	idx;
			while((idx=ws.find(L".")) != wstring::npos)
				ws.replace(idx, 1, L"\\");
			wsModuleBase = msLocalRepBase + MODULESBASE + ws;
		}
	}
	setModuleBase((wchar_t*)wsModuleBase.c_str()); 
	return true; 
}

bool CComManager::GetModuleBase(const wchar_t** apwcModuleBase){ 
	getModuleBase((BSTR*)apwcModuleBase); 
	return true; 
}


void CComManager::LoadModuleMetaAndLoadScene(unsigned int aID, wchar_t *asModuleID, wchar_t *asScene)
{
	if( pContext->mpSM != NULL)
		pContext->mpSM->LoadModuleMetaAndLoadScene(aID, asModuleID, asScene);
}

void CComManager::OnLoadedModuleMeta(unsigned int aID, int iError)
{
	if(aID >= vecQueries.size())
		return;

	EnterCriticalSection( &csQuery);
	CAsyncQuery *pQuery = vecQueries[ aID];
	vecQueries[ aID] = NULL;
	LeaveCriticalSection( &csQuery);

	pQuery->pHandler->onQueryComplete(aID, NULL, &iError);
	MP_DELETE( pQuery);
}



bool CComManager::GetModuleIDAndReady(wchar_t* apwcBuffer, unsigned int auBufferSize, unsigned int* auOutSize, bool* abReady)
{
	if( !apwcBuffer)
		return false;

	if( wsModuleID.length() > auBufferSize-1)
		return false;

	if( !abReady)
		return false;

	if( auOutSize)
		*auOutSize = wsModuleID.length();

	rtl_wcscpy( apwcBuffer, auBufferSize, wsModuleID.c_str());

	if( abReady)
	{
		*abReady = (mspModule == NULL) ? false : true;
	}

	return true;
}

int CComManager::SetModuleID(const wchar_t* apwcModuleID, bool aSync, wchar_t *asScene, unsigned int *apErrorCode)
{ 
	EnterCriticalSection( &csQuery);
	if(mspModule!=NULL)
	{
		wsModuleID = L"";
		mspModule->destroy();
		mspModule = NULL;
	}
	wsModuleID = apwcModuleID;
	LeaveCriticalSection( &csQuery);
	
	if(aSync)
	{
		CAsyncQuery* pQuery = CreateAsyncQuery();

		pQuery->pHandler = pCallbacks;
		MP_NEW_P( moduleTask, CAsyncSetModuleImpl, this);
		if (m_pLW != NULL)
		{
			USES_CONVERSION;
			m_pLW->WriteLn("SetModuleID begin async request apwcModuleID==", W2A(apwcModuleID), ", this = ", (unsigned int)this);
		}
		moduleTask->SetModuleID( pQuery->nQueryID, apwcModuleID, asScene);

		return pQuery->nQueryID;
	}

	IResModule* apModule = NULL;

	if (apwcModuleID!=NULL && *apwcModuleID!=L'\0' /*&& spResourceMan != NULL*/ &&
		SUCCEEDED(this->openModule( (BSTR)apwcModuleID, RES_LOCAL | RES_REMOTE, &apModule))) 
	{
		DWORD dwStatus = 0;		
		if (FAILED( apModule->getStatus( &dwStatus)) || (dwStatus & RES_EXISTS) == 0)
		{
			apModule->destroy();
			apModule = NULL;
			wsModuleID = L"";
			if (m_pLW != NULL)
			{
				USES_CONVERSION;
				m_pLW->WriteLn("[ERROR] SetModuleID apwcModuleID==", W2A(apwcModuleID));
			}
			if( apErrorCode != NULL)
				*apErrorCode = dwStatus;
			return -1;
		}
		else
		{
			EnterCriticalSection( &csQuery);
			mspModule = apModule;
			LeaveCriticalSection( &csQuery);
		}
		if( apErrorCode != NULL)
			*apErrorCode = dwStatus;
	}
	if (m_pLW != NULL)
	{
		USES_CONVERSION;
		m_pLW->WriteLn("SetModuleID async request complete apwcModuleID==", W2A(apwcModuleID), ", errorCode = ", (apErrorCode==NULL) ? -1 : *apErrorCode, ", this = ", (unsigned int)this);
	}
	return 1; 	
}

void CComManager::SetUserBase(const wchar_t* sDir){
	msUserBase=sDir;
	oResMan.setUserBase( (wchar_t*)sDir);
}

void CComManager::GetUserBase(const wchar_t** psDir){
	*psDir=msUserBase.data();
}

void CComManager::GetUIBase(const wchar_t**psDir){
	*psDir=msUIBase.data();
};

void CComManager::GetUISkinBase(const wchar_t** psDir){
	*psDir=msUISkinBase.data();
};

void CComManager::SetUISkinBase(const wchar_t* psDir)
{
	setUISkinBase((BSTR)psDir);
}

BOOL CComManager::RemoveResource(const wchar_t* aPath)
{
	if( aPath == NULL || wcslen(aPath) < 1)
		return false;
	IResModule* spResModule = NULL;
	unsigned int codeError = 0;
	wstring wsURL=RefineURL(aPath, &spResModule, RES_LOCAL, codeError);
	BOOL bRes = false;
	if (spResModule != NULL)
	{
		spResModule->removeResource( (wchar_t*)wsURL.c_str(), &bRes);
	}
	else
	{
		CWComString sPath = (BSTR)aPath;
		oResMan.removeResource( sPath, &bRes);
	}	
	return bRes;
}

/*bool CComManager::GetValidateInfoCache()
{
	if(oResSocket.GetConnectionStatus() == 0)
	{
		ClearInfoList();
		if(oResSocket.Open( &resServerInfo, 0, m_pLW))
		{
			if (m_pLW != NULL)
			{
				CLogValue log("Connected to ResServer with IP: ", (char*)resServerInfo.sIP.GetString(), " and port=", (int)resServerInfo.tcpPort);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
		}
		else
		{
			if (m_pLW != NULL)
			{
				CLogValue log("ERROR: Cannot connect to ResServer with IP: ", resServerInfo.sIP.GetString(), " and port=", resServerInfo.tcpPort);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
		}
	}
	return oResMan.getValidateInfoCache();
}*/

void CComManager::InitResourceManager()
{
	this->getRepositoryBase(msLocalRepBase);
}

void CComManager::DestroyResourceManager()
{
	this->writeStillOpened();
	oResMan.CloseAllResource();
	//spModule->close();
	if(mspModule!=NULL)
	{
		mspModule->destroy();
		mspModule = NULL;
	}
	CloseAllModule();
}

void CComManager::CloseAllModule()
{
	std::vector<resAliasDesc>::iterator	mvit;
	mutex.lock();
	for (mvit=mvResAliases.begin(); mvit < mvResAliases.end(); mvit++)
	{
		//mvit->spModule->Release();
		//MP_DELETE( mvit->spModule);
		if (mvit->spModule)
		{
			mvit->spModule->destroy();
			mvit->spModule = NULL;
		}
		mvit = mvResAliases.erase( mvit);
	}
	mutex.unlock();
}

void CComManager::ClearInfoList()
{
	oResMan.clearInfoList();
}

bool CComManager::IsConnectionResServer()
{
	if( oResSocket.GetConnectionStatus() == CClientSession::STATUS_CONNECTED
		&& oResXMLSocket.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
		return true;
	return false;
}

int CComManager::PackUpdateClientFilesAndGetInfo( const wchar_t* aVersion, wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int &aFileSize)
{
	int dwStatus = 0;
	oResMan.packUpdateClientFilesAndGetInfo(aVersion, apBufferForFileName, auBufferWideCharsCount, &aFileSize, &dwStatus);
	return dwStatus;
}

int CComManager::PackClientSetup(wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int &aFileSize)
{
	int dwStatus = 0;
	oResMan.packClientSetup(apBufferForFileName, auBufferWideCharsCount, &aFileSize, &dwStatus);
	return dwStatus;
}

void CComManager::WriteUpdaterLog(const char * logValue)
{
	oResMan.writeUpdaterLog(logValue);
}

resResource::~resResource()
{
	lw = NULL;
	if(mpIStream)
		MP_DELETE( mpIStream);
	if(mpIOStream!=NULL)
		MP_DELETE( mpIOStream);
	if( mpStreamBuf)
		MP_DELETE(mpStreamBuf);
	if(spResource)
	{
		spResource->destroy();
		spResource = NULL;
	}
}

void resResource::Close()
{
	MP_DELETE_THIS;
}

unsigned int resResource::Read( unsigned char* aBuffer, unsigned int aBufferSize, unsigned int *aError)
{	
#if LOG_REMOTE_RESOURCE
	if(lw != NULL)
	{
		lw->WriteLn("[READING] resResource::Read aBuffer==", aBuffer, spResource == NULL ? "spResource==NULL" : "spResource != NULL");
	}
#endif
	if( !spResource)	return 0;
	DWORD readed = 0;
	int res = spResource->read(aBuffer, aBufferSize, &readed, (DWORD*)aError);
	if( SUCCEEDED(res))
		return readed;
	return 0;
}

unsigned int resResource::Write( unsigned char* aBuffer, unsigned int aBufferSize)
{
	if( !spResource)	return 0;
	DWORD wrote = 0;
	if( SUCCEEDED(spResource->write(aBuffer, aBufferSize, &wrote)))
		return wrote;
	return 0;
}

bool resResource::Seek( long aPos, unsigned char aFrom)
{
	if( !spResource)	return false;
	if( SUCCEEDED(spResource->seek(aPos, aFrom)))
		return true;
	return false;
}

unsigned long resResource::GetPos()
{
	if( !spResource)	return 0;
	DWORD aPos;
	if( SUCCEEDED(spResource->getPos(&aPos)))
		return aPos;
	return 0;
}

unsigned long resResource::GetSize()
{
	if(!spResource) return 0;
	DWORD aSize;
	if( SUCCEEDED(spResource->getSize(&aSize)))
		return aSize;
	return 0;
}

unsigned long resResource::GetCachedSize()
{
	if(!spResource) return 0;
	DWORD aSize = 0;
	if( SUCCEEDED(spResource->getCachedSize(&aSize)))
		return aSize;
	return 0;
}


__int64 resResource::GetTime()
{
	__int64 result;
	DWORD* pResultWord = (DWORD *)&result;

	FILETIME a;
	spResource->getTime(a);

	*pResultWord = a.dwLowDateTime;
	pResultWord++;
	*pResultWord = a.dwHighDateTime;

	return result;
}

const wchar_t* resResource::GetURL(){
	if(!spResource) return L"";
	return msSrc.c_str();
}


bool resResource::GetFileNameRefineLoaded(wchar_t **psName, const wchar_t *pCurName)
{
	*psName = NULL;
	bool bRes = false;
	if(!spResource) return bRes;

	wchar_t	wsBuffer[MAX_PATH];
	CWComString sFileName;
	bRes = spResource->getRefineLoadedResourcePath(wsBuffer, countof(wsBuffer));
	if(!bRes)
	{		
		return bRes;
	}
	sFileName = wsBuffer;
	sFileName.Replace(L"\\", L"/");

	CWComString sModuleName = L"";
	CWComString sModule = pCurName;
	int pos1 = sModule.Find(':', 0);
	int pos2 = sModule.ReverseFind(':');
	if( pos1 != pos2)
	{
		sModule = sModule.Mid( pos1, pos2 - pos1 + 1);
		sModuleName = sModule.Mid( 1, sModule.GetLength() - 2);
	}
	else if( pos1 == pos2 && pos1>0)
	{
		sModule = sModule.Left( pos1 + 1);
		sModuleName = sModule.Left( sModule.GetLength() - 1);
	}

	int pos = sModuleName.IsEmpty() == true ? -1 : sFileName.Find( sModuleName, 0);
	if(pos>-1)
		sFileName = sModule + sFileName.Right(sFileName.GetLength() - pos - 1 - sModuleName.GetLength());
	*psName = sFileName.AllocSysString();
	return bRes;
}

void resResource::Download( resIAsynchResource* aAsynch, const wchar_t* apwcLocalPath)
{
	if(!spResource) return;
	if (aAsynch == NULL)
		spResource->download( NULL);
	else
	{
		CResourceNotify* pNotifyObj = MP_NEW( CResourceNotify);
		//CComObject< CResourceNotify>::CreateInstance( &pNotifyObj);
		pNotifyObj->setAsynchResource( aAsynch);
		IResourceNotify*	pNotify = pNotifyObj;
		spResource->download( pNotify, apwcLocalPath);
	}
}

void resResource::DownloadToCache( resIAsynchResource* aAsynch)
{
	if(!spResource) return;
	if (aAsynch == NULL)
		spResource->downloadToCache( NULL);
	else
	{
		/*CComObject< CResourceNotify>* pNotifyObj;
		CComObject< CResourceNotify>::CreateInstance( &pNotifyObj);
		pNotifyObj->setAsynchResource( aAsynch);*/
		CResourceNotify* pNotifyObj = MP_NEW( CResourceNotify);
		pNotifyObj->setAsynchResource( aAsynch);
		IResourceNotify*	pNotify = pNotifyObj;
		spResource->downloadToCache( pNotify);
	}
}

bool resResource::Unpack(resIAsynchResource* aAsynch){
	if(!spResource) return false;
	if (aAsynch == NULL)
		spResource->unpack2( NULL, NULL);
	else
	{
		//CComObject< CResourceNotify>* pNotifyObj;
		//CComObject< CResourceNotify>::CreateInstance( &pNotifyObj);
		CResourceNotify* pNotifyObj = MP_NEW( CResourceNotify);
		pNotifyObj->setAsynchResource( aAsynch);
		IResourceNotify*	pNotify = pNotifyObj;
		spResource->unpack2(NULL, pNotify);
	}
	return true;
}

bool resResource::UnpackToCache(resIAsynchResource* aAsynch){
	if(!spResource) return false;
	if (aAsynch == NULL)
		spResource->unpackToCache( NULL, NULL);
	else
	{
		//CComObject< CResourceNotify>* pNotifyObj;
		//CComObject< CResourceNotify>::CreateInstance( &pNotifyObj);
		CResourceNotify* pNotifyObj = MP_NEW( CResourceNotify);
		pNotifyObj->setAsynchResource( aAsynch);
		IResourceNotify*	pNotify = pNotifyObj;
		spResource->unpackToCache(NULL, pNotify);
	}
	return true;
}

bool resResource::Upload(resIResource* apSrcRes, resIAsynchResource* aAsynch){
	if(!spResource) return false;
	if(!apSrcRes) return false;
	//
	resResource* pSrcRes = (resResource*)apSrcRes;
	IResource* pISrcRes = NULL;
	pSrcRes->GetIResourceAndClose(&pISrcRes);
	if(pISrcRes == NULL) return false;
	//
	if (aAsynch == NULL)
		spResource->upload(pISrcRes, NULL);
	else
	{
		//CComObject< CResourceNotify>* pNotifyObj;
		//CComObject< CResourceNotify>::CreateInstance( &pNotifyObj);
		CResourceNotify* pNotifyObj = MP_NEW( CResourceNotify);
		pNotifyObj->setAsynchResource( aAsynch);
		IResourceNotify*	pNotify = pNotifyObj;
		spResource->upload(pISrcRes, pNotify);
	}
	return true;
}

unsigned long resResource::GetUploadedSize(){
	if(!spResource) return 0;
	DWORD aSize;
	if( SUCCEEDED(spResource->getUploadedSize(&aSize)))
		return aSize;
	return 0;
}

unsigned long resResource::GetDownloadedSize()
{
	if(!spResource) return 0;
	DWORD aSize;
	if( SUCCEEDED(spResource->getDownloadedSize(&aSize)))
		return aSize;
	return 0;
}

unsigned int resResource::GetState(){
	if(!spResource) return 0;
	DWORD dwStatus;
	spResource->getStatus(&dwStatus);
	// если ошибка, то вернем ее код
	if(RES_IS_ERROR(dwStatus))
		return (dwStatus & RES_ERROR);
	// если ресурс не существует, то 0
	if(RES_IS_NOT_EXISTS(dwStatus))
		return 0;
	if((dwStatus & RES_LOCAL) != 0 && (dwStatus & RES_FILE_CACHE) != 0){
		// в кэше, то 4
		return 4;
	}
	// если ресурс существует налокальной машине, но не загружен на сервер
	if((dwStatus & RES_UPLOAD) != 0)
	{
		DWORD dwUploadedSize = 0;
		spResource->getUploadedSize(&dwUploadedSize);
		if ( dwUploadedSize == 0)
			return 6; // пауза
		DWORD dwSize = 0;
		spResource->getSize( &dwSize);
		if ( dwSize <= dwUploadedSize)
			return 7; // закачан
		// и закачиваетс€, то 5
		return 5; // идет процесс закачки
	}
	// если ресурс существует на сервере, но не закачан
	if((dwStatus & RES_REMOTE) != 0){
		// и не закачиваетс€, то 1
		DWORD dwDownloadedSize = 0;
		spResource->getDownloadedSize(&dwDownloadedSize);
		if(dwDownloadedSize == 0)
			return 1;
		// и закачиваетс€, то 2
		return 2;
	}	
	// если русурс существует локально или на CD, то 3
	return 3;
}

unsigned int resResource::GetUnpackingProgress(){
	if(!spResource) return 0;
	BYTE btPercent = 0;
	spResource->getUnpackProgress(&btPercent);
	return btPercent;
}

omsresult resResource::GetIStream(istream* &apistrm)
{
	if(!spResource) return OMS_ERROR_NOT_INITIALIZED;
	MP_NEW_V( mpStreamBuf, res_streambuf, this);
	MP_NEW_V( mpIStream, istream, mpStreamBuf);
	apistrm=mpIStream;
	return OMS_OK;
}

omsresult resResource::GetIOStream(iostream* &apiostrm){
	if(!spResource) return OMS_ERROR_NOT_INITIALIZED;
	MP_NEW_V( mpStreamBuf, res_streambuf, this);
	MP_NEW_V( mpIOStream, iostream, mpStreamBuf);
	apiostrm=mpIOStream;
	return OMS_OK;
}

void resResource::GetIResourceAndClose( IResource** appRes){
	*appRes = spResource;
	// ресурс отдаетс€ во вне. ”бивать себ€ тут не надо
	spResource = NULL;
	Close();
}

bool resResource::RegisterAtResServer( resIAsynchResource* aAsynch){
	if(!spResource) return false;
	//			
	//CComObject< CResourceNotify>* pNotifyObj;
	//CComObject< CResourceNotify>::CreateInstance( &pNotifyObj);
	CResourceNotify* pNotifyObj = MP_NEW( CResourceNotify);
	pNotifyObj->setAsynchResource( aAsynch);
	IResourceNotify*	pNotify = pNotifyObj;
	spResource->registerAtResServer( pNotify);
	return true;
}