// crmResourceFile.cpp: implementation of the crmResourceFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "res.h"
#include <Assert.h>
#include "crmResourceFile.h"
#include "ObjStorage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

crmResourceFile::crmResourceFile(crmResLibrary*	a_pResLibrary):
	MP_WSTRING_INIT(m_pFilePath)
{
	pResource = NULL;
	m_pResLibrary = a_pResLibrary;
	m_modifyTime = 0;
	m_pGetFileDataPtr = 0;
}

crmResourceFile::crmResourceFile(crmResLibrary*	a_pResLibrary, res::resIResource*	apResource):
	MP_WSTRING_INIT(m_pFilePath)
{
	pResource = apResource;
	m_pResLibrary = a_pResLibrary;
	m_modifyTime = apResource->GetTime();
}

crmResourceFile::~crmResourceFile()
{
	Close();
}

const void*		crmResourceFile::get_file_data( DWORD* size)
{
	ATLASSERT(m_pResLibrary);
	if( !m_pResLibrary)	return NULL;

	free_file_data();

	*size = 0;
	UINT readed;
	if( m_pResLibrary->mpIResourceManager && pResource)
	{
		UINT fs = GetFileSize();
		*size=GetFileSize();
		m_pGetFileDataPtr = MP_NEW_ARR(unsigned char, *size);
		Read((TCHAR*)m_pGetFileDataPtr, fs=GetFileSize(), &readed);
		if( readed != fs)
		{
		  return NULL;
		}
	}
	return m_pGetFileDataPtr;
}

void crmResourceFile::free_file_data()
{
	if( m_pGetFileDataPtr != NULL) 
		MP_DELETE_ARR(m_pGetFileDataPtr);
}

BOOL		crmResourceFile::Open(	const wchar_t* lpFile, DWORD dwCreateDisp)
{
	ATLASSERT(m_pResLibrary);
	ATLASSERT(m_pResLibrary->mpIResourceManager);
	if( !m_pResLibrary)	return false;
	if( !m_pResLibrary->mpIResourceManager)	return	false;
	Close();
	dwCreateDisp &= (RES_TO_WRITE | RES_CREATE);
	if( m_pResLibrary->mpIResourceManager->OpenResource( lpFile, pResource, dwCreateDisp/*RES_LOCAL*/) == OMS_OK)
	{
		m_pFilePath = lpFile;
		m_modifyTime = pResource->GetTime();
		return true;
	}
	//CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ERROR] Cannot open resource: ", lpFile).GetData());
	return	false;
}

__int64		crmResourceFile::get_modify_time()
{
	return m_modifyTime;
}

void		crmResourceFile::Close()
{
	if( m_pResLibrary->mpIResourceManager && pResource)
		pResource->Close();
	pResource = NULL;
	free_file_data();
}

UINT		crmResourceFile::Read( TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	ATLASSERT(m_pResLibrary);
	if( !m_pResLibrary)	return 0;
	*puCount = 0;
	unsigned int errorRead = 0;
	if( m_pResLibrary->mpIResourceManager && pResource)
		*puCount = pResource->Read((BYTE*)pszBuffer, uCount, &errorRead);
	if (*puCount==0)
	{
		CComString sResPath="";
		if(pResource)
			sResPath = (BSTR)pResource->GetURL();
		if( errorRead == 7)
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ERROR ACCEPT TO RESSERVER] Not reading resource: ", sResPath.GetBuffer()).GetData());
		else
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ERROR] Not reading resource: ", sResPath.GetBuffer()).GetData());

	}
	return *puCount;
}

DWORD		crmResourceFile::Read( BYTE* pBuffer, DWORD dwSize )
{
	ATLASSERT(m_pResLibrary);
	if( !m_pResLibrary)	return 0;
	DWORD ret = 0;
	unsigned int errorRead = 0;
	if( m_pResLibrary->mpIResourceManager && pResource)
		ret = pResource->Read((BYTE*)pBuffer, dwSize, &errorRead);
	if (ret==0)
	{
		CComString sResPath="";
		if(pResource)
			sResPath = (BSTR)pResource->GetURL();
		if( errorRead == 7)
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ERROR ACCEPT TO RESSERVER] Not reading resource: ", sResPath.GetBuffer()).GetData());
		else
			CObjStorage::GetInstance()->GetILogWriter()->WriteLnLPCSTR(CLogValue("[ERROR] Not reading resource: ", sResPath.GetBuffer()).GetData());
	}
	return ret;
}

void		crmResourceFile::Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	ATLASSERT(m_pResLibrary);
	if( puCount)	*puCount = 0;
	if( !m_pResLibrary)	return;
	if( m_pResLibrary->mpIResourceManager && pResource)
	{
		unsigned int wrote = pResource->Write((BYTE*)pszBuffer, uCount);
		if( puCount) *puCount = wrote;
	}
}

DWORD		crmResourceFile::GetFileSize()
{
	ATLASSERT(m_pResLibrary);
	if( !m_pResLibrary)	return 0;
	if( m_pResLibrary->mpIResourceManager && pResource)
		return pResource->GetSize();
	return 0;
}

DWORD	crmResourceFile::Seek( DWORD lToMove, DWORD dwMoveFrom)
{
	ATLASSERT(m_pResLibrary);
	if (!m_pResLibrary)	return (DWORD)-1;
	if ((!m_pResLibrary->mpIResourceManager) || (!pResource) || (lToMove + dwMoveFrom > (unsigned int)pResource->GetSize()))
	{
		return (DWORD)-1;
	}
	pResource->Seek(lToMove, (unsigned char)dwMoveFrom);
	return 0;
}

DWORD	crmResourceFile::SeekToBegin()
{
	ATLASSERT(m_pResLibrary);
	if( !m_pResLibrary)	return (DWORD)-1;
	if( !m_pResLibrary->mpIResourceManager || !pResource)
	{
		return (DWORD)-1;
	}
	pResource->Seek(0,0);
	return 0;
}

DWORD	crmResourceFile::SeekToEnd()
{
	ATLASSERT(m_pResLibrary);
	if( !m_pResLibrary)	return (DWORD)-1;
	if( !m_pResLibrary->mpIResourceManager || !pResource)
	{
		return (DWORD)-1;
	}
	assert(false);
	pResource->Seek(0,0);
	return 0;
}

DWORD crmResourceFile::GetPos()
{
	ATLASSERT(m_pResLibrary);
	if( !m_pResLibrary)	return (DWORD)-1;
	if( !m_pResLibrary->mpIResourceManager || !pResource)
	{
		return (DWORD)-1;
	}
	assert(false);
	return pResource->GetPos();
}


BOOL	crmResourceFile::open(	const wchar_t* lpFile, DWORD dwCreateDisp)
{
	return Open(lpFile, dwCreateDisp);
}

void	crmResourceFile::close()
{
	Close();
}

UINT	crmResourceFile::read( TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	return	Read(pszBuffer, uCount, puCount);
}

DWORD	crmResourceFile::read( BYTE* pBuffer, DWORD dwSize )
{
	return	Read(pBuffer, dwSize);
}

void	crmResourceFile::write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	Write(pszBuffer, uCount, puCount);
}

DWORD	crmResourceFile::seek( DWORD lToMove, DWORD dwMoveFrom)
{
	return	Seek( lToMove, dwMoveFrom);
}

DWORD	crmResourceFile::seek_to_begin()
{
	return	SeekToBegin();
}

DWORD	crmResourceFile::seek_to_end()
{
	return	SeekToEnd();
}

DWORD	crmResourceFile::get_pos()
{
	return	GetPos();
}

ifile *	crmResourceFile::duplicate()
{
	return NULL;
}

void crmResourceFile::release()
{
	MP_DELETE_THIS;
}

LPCWSTR crmResourceFile::get_file_path()
{
	return m_pFilePath.c_str();
}