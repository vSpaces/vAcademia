// SoundWrapper.cpp: implementation of the CSoundWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoundWrapper.h"
#include "Common.h"
#include "..\res\res.h"
#include "ifile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundWrapper::CSoundWrapper():
	MP_MAP_INIT( m_mapAsynchResources)
{	
	mpOMSContext = NULL;
	/*CComObject< CResourceNotify>*	pNotify;
	CComObject< CResourceNotify>::CreateInstance( &pNotify);
	m_pIResourceNotify = pNotify;*/
	mpResMan = NULL;
	mpSoundMan = NULL;	
	m_pIResourceNotify.setSoundWrapper(this);
}

CSoundWrapper::~CSoundWrapper()
{
}

void CSoundWrapper::setOMSContext( oms::omsContext* apOMSContext)
{
	mpOMSContext = apOMSContext;
	//mpResMan = mpOMSContext->mpResM;
}

void CSoundWrapper::setSoundManager( sm::smISoundManager *aSoundMan)
{
	mpSoundMan = aSoundMan;
}

void CSoundWrapper::SetResourceMan( IResMan *aResMan)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::SetResourceMan)\n");
		return;
	}

	mpResMan = aResMan;
	mpSoundMan->setResManager( mpResMan);
}

void CSoundWrapper::Trace( const char* pszFormat, ...)
{
	if ( mpOMSContext == NULL)
		return;
	if ( mpOMSContext->mpTracer == NULL)
		return;

	va_list ptr;
	va_start(ptr, pszFormat);
	char sErrorMessage[ 2500] = "SndM: ";
	vsprintf_s( &sErrorMessage[strlen(sErrorMessage)], 2500-strlen(sErrorMessage), pszFormat, ptr);
	mpOMSContext->mpTracer->trace(sErrorMessage);
	va_end(ptr);
}

void CSoundWrapper::Trace( HRESULT aHR, const char* apszFun)
{
	if(SUCCEEDED(aHR)) return;
	char* pszError="Unknown error";
	switch(aHR)
	{
		case E_FAIL:
			pszError="Error (E_FAIL)";
			break;
		case E_INVALIDARG:
			pszError="An invalid parameter was passed (E_INVALIDARG)";
			break;
		case E_OUTOFMEMORY:
			pszError="Cannot allocate sufficient memory (E_OUTOFMEMORY)";
			break;
		case E_POINTER:
			pszError="An invalid pointer, usually NULL, was passed as a parameter (E_POINTER)";
			break;
		case E_NOTIMPL:
			pszError="(E_NOTIMPL)";
			break;
		case E_NOINTERFACE:
			pszError="(E_NOINTERFACE)";
			break;
		case SNDM_E_RESMANNOTSET:
			pszError="Resource manager is not set (SNDM_E_RESMANNOTSET)";
			break;
		case SNDM_E_COMMANNOTSET:
			pszError="Communication manager is not set (SNDM_E_COMMANNOTSET)";
			break;
	}
	if ( apszFun == NULL)
	{
		Trace((const char*)"%s", pszError);
	}
	else
		Trace("%s (%s)",pszError, apszFun);
}

/**
 * rmISoundManager
 */
int	CSoundWrapper::GetLexema( int aiPerson, int& aoiLexema)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::GetLexema)\n");
		return -1;
	}
	int	lex;
	mpSoundMan->GetLexema( aiPerson, &lex);
	aoiLexema = lex;
	return aoiLexema;
}

//struct resIResource;

int	CSoundWrapper::OpenSound( ifile *pfile, sm::smISoundNotify *apISoundNotify /* = NULL */, bool bAttached /* = FALSE */)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::OpenSound)\n");
		return -2;
	}	
	int sID = -1;
	
	ATLASSERT( pfile);
	if ( !pfile)
	{
		Trace("pFile is NULL (CSoundWrapper::OpenSound)\n");
		return -1;
	}
	HRESULT hr = mpSoundMan->OpenSoundFile( pfile, TRUE, &sID, apISoundNotify, bAttached);	
	Trace( hr, "CSoundWrapper::OpenSound");
	return sID;
}

int	CSoundWrapper::OpenSound( const wchar_t* apwcURL, sm::smISoundNotify *apISoundNotify)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::OpenSound)\n");
		return -2;
	}
	// Проверка на то, что ресурс уже есть в кеше
	bool localAlready = true;
	/*if ( FAILED( localAlready = mpOMSContext->mpResM->ResourceExists( apwcURL, RES_FILE_CACHE|RES_LOCAL|RES_CDROM|RES_MEM_CACHE)))
	{
		return -1;
	}*/
	unsigned int codeError = 0;
	if ( FAILED( localAlready = mpOMSContext->mpResM->ResourceExists(apwcURL, RES_FILE_CACHE, codeError)))
	{
		return -1;
	}
	
	int sID = -1;
	res::resIResource* pOMSRes = NULL;

	if ( localAlready)
	{
		IResource *spRes = NULL;

		if ( OMS_FAILED( mpOMSContext->mpResM->OpenResource( apwcURL, pOMSRes, RES_FILE_CACHE /*| RES_LOCAL*/)))		
			return -1;
		
		
		mpOMSContext->mpResM->GetIResourceAndClose( pOMSRes, (LPVOID*)&spRes);

		DWORD dwStatus = 0;
		spRes->getStatus( &dwStatus);

		if ( (dwStatus & RES_ERROR ) != 0 || ((dwStatus & RES_LOCAL) == 0 && (dwStatus & RES_FILE_CACHE) == 0
			&& (dwStatus & RES_MEM_CACHE) == 0))
			return -1;

		HRESULT hr = mpSoundMan->OpenSoundRes( spRes, apwcURL, TRUE, &sID, apISoundNotify);
		Trace( hr, "CSoundWrapper::OpenSound");
		//if ( apIAsynchResource)
		//	apIAsynchResource->OnLoaded();
		return sID;
	}
	else
	{
		/*CComPtr< IResource> spRes;
		if ( OMS_FAILED( mpOMSContext->mpResM->OpenAsynchResource( apwcURL, pOMSRes, , RES_REMOTE | RES_ASYNCH | RES_FILE_CACHE)))
			return -1;		
		mpOMSContext->mpResM->GetIResourceAndClose( pOMSRes, (LPVOID*)&spRes);
		spRes.p->AddRef();
		char* pszURL = WC2MB(apwcURL); 
		HRESULT hr = mpSoundMan->OpenSoundRes( spRes, pszURL, FALSE, &sID);
		Trace( hr,"CSoundWrapper::AsynchOpenSound");
		delete pszURL;

		// Добавим скачиваемый ресурс в мапку
		swAsynchMapIterator	it = m_mapAsynchResources.find( spRes);
		if ( it == m_mapAsynchResources.end())
		{
			m_mapAsynchResources.insert( swAsynchMap::value_type( spRes, apIAsynchResource));
		}
		return sID;*/
	}
	return -1;
}

int CSoundWrapper::UpdateFileInfo( int	aiSound, unsigned long adwLoadedBytes, bool bLoaded)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::UpdateFileInfo)\n");
		return WARNING_RES_IS_NOT_UPDATED;
	}
	return mpSoundMan->UpdateFileInfo( aiSound, adwLoadedBytes, bLoaded);
}

bool CSoundWrapper::CloseSound( int aiSound)
{
	if ( !mpSoundMan)
	{
		Trace( "Sound manager is not set (CSoundWrapper::CloseSound)\n");
		return false;
	}
	return mpSoundMan->closeSound( aiSound)!=0;
}

void CSoundWrapper::StopSound( int aiSound)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::StopSound)\n");		
		return;
	}

	mpSoundMan->stopSound( aiSound);
}

bool CSoundWrapper::PlaySound( int aiSound, int aiPerson, bool loop)
{
	if ( aiSound < 0) 
		return false;

	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::PlaySound)\n");
		return false;
	}
	//HRESULT hr=mpSoundMan->PlaySound(aiSound, aiPerson,loop);
	HRESULT hr = mpSoundMan->playSound( aiSound, aiPerson,loop);
	Trace( hr,"CSoundWrapper::PlaySound");
	return true;
}

void CSoundWrapper::SoundOn( int id, char** ppGUID)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::SoundOn)\n");
		return;
	}
	//mpSoundMan->SoundOn( id, ppGUID);
}

void CSoundWrapper::SoundOff()
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::SoundOff)\n");
		return;
	}
	//mpSoundMan->SoundOff();
}

bool CSoundWrapper::IsPlaying( int aiSound)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::IsPlaying)\n");
		return false;
	}

	return mpSoundMan->IsPlaying( aiSound)!=0;
}

bool	CSoundWrapper::SetPosition( int	aiSound, unsigned long adwPos)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::SetPosition)\n");
		return false;
	}
	return mpSoundMan->SetPosition( aiSound, adwPos)!=0;
}

bool	CSoundWrapper::GetPosition( int	aiSound, unsigned long &adwPos)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::GetPosition)\n");
		return false;
	}
	return mpSoundMan->GetPosition( aiSound, &adwPos)!=0;
}

bool	CSoundWrapper::GetFrequency(int aiSound, unsigned long &adwFrequency)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::GetFrequency)\n");
		return false;
	}
	return mpSoundMan->GetFrequency( aiSound, &adwFrequency)!=0;
}

bool	CSoundWrapper::GetDuration( int aiSound, unsigned long &adwDur)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::GetDuration)\n");
		return false;
	}
	return mpSoundMan->GetDuration( aiSound, &adwDur)!=0;
}

bool	CSoundWrapper::GetLength( int aiSound, unsigned long &adwLength)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::GetLength)\n");
		return false;
	}
	return mpSoundMan->GetLength( aiSound, &adwLength)!=0;
}

bool	CSoundWrapper::GetSize( int aiSound, unsigned long &adwSize)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::GetSize)\n");
		return false;
	}
	return mpSoundMan->GetSize( aiSound, &adwSize)!=0;
}

bool	CSoundWrapper::GetCurrentSize( int aiSound, unsigned long &adwSize)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::GetCurrentSize)\n");
		return false;
	}
	return mpSoundMan->GetCurrentSize( aiSound, &adwSize)!=0;
}

bool	CSoundWrapper::GetVolume( int aiSound, float &fVolume)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::GetVolume)\n");
		return false;
	}
	return mpSoundMan->GetVolume( aiSound, &fVolume)!=0;
}


bool	CSoundWrapper::SetCooperativeLevel( unsigned long hWnd, unsigned long level)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::SetCooperativeLevel)\n");
		return false;
	}
	return mpSoundMan->setCooperativeLevel( (UINT) hWnd, (UINT) level)!=0;
}

bool	CSoundWrapper::Set3DSoundParams( int id, rmml::ml3DPosition dist, rmml::ml3DPosition dir, rmml::ml3DPosition vp_dir)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::Set3DSoundParams)\n");
		return false;
	}
	return mpSoundMan->Set3DSoundParams( id, (float)dist.x, (float)dist.y, (float)dist.z, (float)dir.x, (float)dir.y, (float)dir.z, (float)vp_dir.x, (float)vp_dir.y, (float)vp_dir.z)!=0;
}

bool	CSoundWrapper::Set3DSoundParams( int id, float coef, float left_coef, float right_coef)
{
	if ( !mpSoundMan) 
	{
		Trace("Sound manager is not set (CSoundWrapper::Set3DSoundParams)\n");
		return false;
	}
	return mpSoundMan->Set3DSoundCoef( id, coef, left_coef, right_coef)!=0;
}

bool	CSoundWrapper::SetVolume( int id, float volume)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::SetVolume)\n");
		return false; 
	}
	return mpSoundMan->SetVolume( id, volume)!=0;
}

bool	CSoundWrapper::SetPitchShift( int id, float shift)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::SetPitchShift)\n");
		return false;
	}
	return mpSoundMan->SetPitchShift( id, shift)!=0;
}

bool	CSoundWrapper::UpdateAllSound( int playingState)
{
	if ( !mpSoundMan)
	{
		Trace("Sound manager is not set (CSoundWrapper::UpdateAllSound)\n");
		return false;
	}
	if ( playingState == 0)
		mpSoundMan->SetVolume( 0.0f);
	return mpSoundMan->UpdateAllSound( playingState)!=0;
}

void	CSoundWrapper::OnResourceNotify( IResource*	apIResource, unsigned long adwNotifyCode, unsigned int aNotifyServerCode, wchar_t *aErrorText)
{
	// Check for notification
	swAsynchMapIterator	it = m_mapAsynchResources.find(apIResource);
	if( it != m_mapAsynchResources.end())
	{
		res::resIAsynchResource*	pIAsynchResource = it->second;
		m_mapAsynchResources.erase( it);
		if( pIAsynchResource)
		{
			pIAsynchResource->OnLoaded();
		}
		MP_DELETE( apIResource);
		//apIResource->Release();
	}
	
}


/************************************************************************/
/* CResourceNotify class
/************************************************************************/
CResourceNotify::CResourceNotify()
{
}

CResourceNotify::~CResourceNotify()
{
}

HRESULT CResourceNotify::onResourceNotify(IResource *aResource, DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t *aErrorText)
{
	if( m_pSoundWrapper != NULL)
		m_pSoundWrapper->OnResourceNotify(aResource, aNotifyCode, aNotifyServerCode, aErrorText);
	return 0;
}

void CResourceNotify::destroy()
{
	MP_DELETE_THIS;
}

void CResourceNotify::setSoundWrapper( CSoundWrapper* apSoundWrapper)
{
	m_pSoundWrapper = apSoundWrapper;
}