// crmMemResourceFile.cpp: implementation of the crmMemResourceFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "res.h"
#include <Assert.h>
#include "crmMemResourceFile.h"
#include "ObjStorage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

crmMemResourceFile::crmMemResourceFile(crmResLibrary*	a_pResLibrary):
	MP_WSTRING_INIT(m_pFilePath)
{
	pResource = NULL;
	lpData = NULL;
	dwSize = NULL;
	dwLocation = 0;
	m_pResLibrary = a_pResLibrary;
	bAttached = false;
	modifyTime = 0;
}

bool crmMemResourceFile::attach(void* data, DWORD size)
{
	dwLocation = 0;
	dwSize = size;
	lpData = data;
	bAttached = true;
	modifyTime = 0;

	return true;
}

crmMemResourceFile::crmMemResourceFile(crmResLibrary*	a_pResLibrary, res::resIResource*	apResource):
	MP_WSTRING_INIT(m_pFilePath)
{
	pResource = apResource;
	m_pResLibrary = a_pResLibrary;
	bAttached = false;
	dwLocation = 0;
	lpData = NULL;
	dwSize = 0;
	modifyTime = 0;
}

crmMemResourceFile::~crmMemResourceFile()
{
	if(bAttached)
	{
		Detach();
	}
	else
	{
		if( lpData != NULL) 
			MP_DELETE_ARR(lpData);
	}
	lpData = NULL;
	pResource = NULL;
}

BOOL crmMemResourceFile::Open(	const wchar_t* lpFile, DWORD dwCreateDisp)
{
	ATLASSERT(m_pResLibrary);
	if( !m_pResLibrary)	return	false;
	if( !m_pResLibrary->mpIResourceManager)	return	false;		
#ifdef RESLOADER_LOG
	if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
	{
		CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR( CLogValue("[BD-RESLOADER] Opening local resource: ", s.GetBuffer()).GetData());
	}
#endif
	if( m_pResLibrary->mpIResourceManager->OpenResource( lpFile, pResource, RES_LOCAL) != OMS_OK
		|| pResource == NULL)
	{
		//ILogWriter *plw = CObjStorage::GetInstance()->GetILogWriter();
		if(dwCreateDisp == 0)
			dwCreateDisp = 1;
		return FALSE;
	}

	modifyTime = pResource->GetTime();

	dwSize = pResource->GetSize();
	bool bRes = (dwSize != 0) && (dwSize != 0xFFFFFFFF);
	if (bRes)
	{
		lpData = MP_NEW_ARR( BYTE, dwSize);
		UINT readed;
		unsigned int errorRead = 0;
		readed = pResource->Read((BYTE*)lpData, dwSize, &errorRead);
		if( readed == 0)
		{
			CComString sResPath="";
			sResPath = (BSTR)pResource->GetURL();
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ERROR] Not reading local resource: ", sResPath.GetBuffer()).GetData());
		}
		else
		{
#ifdef RESLOADER_LOG
			if (CObjStorage::GetInstance()->GetILogWriter() != NULL)
			{
				CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR( CLogValue("[BD-RESLOADER] Loaded local resource: ", s.GetBuffer()).GetData());
			}
#endif
		}

	}
	else
	{	
		ILogWriter *plw = CObjStorage::GetInstance()->GetILogWriter();
		if( plw != NULL)
		{
			CComString s1 = lpFile;
			plw->WriteLn(CLogValue("Get failed mem resource: ", s1.GetBuffer()));
		}
	}
	pResource->Close();
	pResource = NULL;
	if( lpFile)
		m_pFilePath = lpFile;
	else
		m_pFilePath = L"";
	return bRes;
}

//////////////////////////////////////////////////////////////////////////
UINT	crmMemResourceFile::Read( TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	if( !lpData)	return 0;
	UINT readed = uCount;

	if( dwLocation + uCount >= dwSize)
		readed = dwSize - dwLocation;

	memcpy( pszBuffer, (void*)((DWORD) lpData + dwLocation), readed);

	dwLocation += readed;
	if( puCount) *puCount = readed;

	//if( readed == 0)		dwLocation += 1;

	return readed;
}

DWORD		crmMemResourceFile::Read( BYTE* pBuffer, DWORD size )
{
	if( !lpData)
	{
		//ThrowErr( FILEEX_ERR_OTHERIO_CURRENTLY_RUNNG, TRUE );
		return 0;
	}
	UINT readed = size;

	if( dwLocation + size >= dwSize)
		readed = dwSize - dwLocation;

	memcpy( pBuffer, (void*)((DWORD) lpData + dwLocation), readed);

	dwLocation += readed;

	//if( readed == 0) dwLocation += 1;

	return readed;
}

void	crmMemResourceFile::Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	if( pszBuffer == NULL || uCount == 0 || puCount == NULL)
		uCount = 1;
	assert(false);
}

DWORD	crmMemResourceFile::GetFileSize()
{
	return dwSize;
}

LPVOID	crmMemResourceFile::Detach()
{
	LPVOID	ret = lpData;
	dwSize = 0;
	lpData = NULL;
	return ret;
}

DWORD	crmMemResourceFile::Seek( DWORD lToMove, DWORD dwMoveFrom)
{
	if( !lpData || lToMove+dwMoveFrom > dwSize)
	{
		//ThrowErr( FILEEX_ERR_OTHERIO_CURRENTLY_RUNNG, TRUE );
		return 0;
	}
	dwLocation = lToMove + dwMoveFrom;
	return dwLocation;
}

DWORD	crmMemResourceFile::SeekToBegin()
{
	if( !lpData)
	{
		//ThrowErr( FILEEX_ERR_OTHERIO_CURRENTLY_RUNNG, TRUE );
		return 0;
	}
	dwLocation = 0;
	return dwLocation;
}

DWORD	crmMemResourceFile::SeekToEnd()
{
	if( !lpData)
	{
		//ThrowErr( FILEEX_ERR_OTHERIO_CURRENTLY_RUNNG, TRUE );
		return 0;
	}
	dwLocation = dwSize;
	return dwLocation;
}

DWORD crmMemResourceFile::GetPos()
{
	return dwLocation;
}


//////////////////////////////////////////////////////////////////////////
const void* crmMemResourceFile::get_file_data(DWORD* size)
{	
	if( size)
		*size=GetFileSize();
	return lpData;
}

BOOL		crmMemResourceFile::open(	const wchar_t* lpFile, DWORD dwCreateDisp)
{
	return Open(lpFile, dwCreateDisp);
}

void		crmMemResourceFile::close()
{
	if( m_pResLibrary->mpIResourceManager && pResource != NULL)		
	{
		pResource->Close();		
		pResource = NULL;
	}
}

UINT		crmMemResourceFile::read( TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	return	Read(pszBuffer, uCount, puCount);
}

DWORD		crmMemResourceFile::read( BYTE* apBuffer, DWORD adwSize )
{
	return	Read(apBuffer, adwSize);
}

void		crmMemResourceFile::write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	Write(pszBuffer, uCount, puCount);
}

DWORD		crmMemResourceFile::seek( DWORD lToMove, DWORD dwMoveFrom)
{
	return	Seek( lToMove, dwMoveFrom);
}

DWORD		crmMemResourceFile::seek_to_begin()
{
	return	SeekToBegin();
}

DWORD		crmMemResourceFile::seek_to_end()
{
	return	SeekToEnd();
}

DWORD		crmMemResourceFile::get_pos()
{
	return	GetPos();
}

DWORD		crmMemResourceFile::get_file_size()
{
	return	GetFileSize();
}

__int64		crmMemResourceFile::get_modify_time()
{
	return modifyTime;
}

void crmMemResourceFile::free_file_data()
{

	}

void*	crmMemResourceFile::detach()
{
	LPVOID	ret = lpData;
	dwSize = 0;
	lpData = NULL;
	m_pFilePath = L"";
	return ret;
}

void	crmMemResourceFile::set_file_path( LPCWSTR alpFilePath)
{
	ATLASSERT( alpFilePath);
	if( alpFilePath)
		m_pFilePath = alpFilePath;
	else
		m_pFilePath = L"";
}

ifile *	crmMemResourceFile::duplicate()
{
	crmMemResourceFile *pMemResFile = NULL;
	if ( pResource != NULL)
	{
		MP_NEW_V2( pMemResFile, crmMemResourceFile, m_pResLibrary, pResource);
	}
	else
	{
		MP_NEW_V( pMemResFile, crmMemResourceFile, m_pResLibrary);
	}

	DWORD size = 0;	
	const void* buffer = get_file_data( &size);

	if( size > 0)
	{
		void* newBuffer = MP_NEW_ARR(BYTE, size);
		//memcpy( newBuffer, buffer, size);
		rtl_memcpy( newBuffer, size, buffer, size);
		pMemResFile->attach( newBuffer, size);
	}
	
	pMemResFile->bAttached = false;
	pMemResFile->dwLocation = dwLocation;
	pMemResFile->modifyTime = modifyTime;
	return pMemResFile;
}

void crmMemResourceFile::release()
{
	MP_DELETE_THIS;
}

LPCWSTR crmMemResourceFile::get_file_path()
{
	return m_pFilePath.c_str();
}