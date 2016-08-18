// ResFile.cpp: implementation of the CResourceFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResFile.h"
#include "res_consts.h"
#include "ifile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResourceFile::CResourceFile( res::IResMan *spResMan, const wchar_t *nameFile, res::IResource* res): 
	m_pResMan(spResMan), m_sName(nameFile)
{
	m_dwFlags = 0;
	m_dwStatus = 0;
	if ( res)
	{
		m_pResource = res;
	}
}

void CResourceFile::destroy()
{
	MP_DELETE_THIS;
}

CResourceFile::~CResourceFile() 
{
	CloseFile();
}

int ADR_CALL CResourceFile::read(void* buffer, int size)
{
	unsigned long readed;
#ifdef DEBUG_MODE
		ATLTRACE("size:  %d \n",size);
#endif
	if ( m_pResource == NULL) 
		return 0;

	m_pResource->read((unsigned char *)buffer, size, &readed);
	if ( size != readed)
	{	
#ifdef DEBUG_MODE
		ATLTRACE("End of file:  need=%d readed=%d\n",size, readed);
#endif
	}	

	return readed;
}

int ADR_CALL CResourceFile::write(const void* buffer, int size) 
{
	unsigned long wroten;
	if ( m_pResource == NULL) 
		return 0;

	m_pResource->write( ( unsigned char *) buffer, size, &wroten);
	if ( size != wroten)
	{	
		ATLTRACE("Error writing buffer \n");
		return 0;
	}	

	return size;
}

bool ADR_CALL CResourceFile::seek(int position, SeekMode mode) 
{
	if ( m_pResource == NULL) 
		return 0;

	switch (mode) 
	{
		case BEGIN:    
			m_pResource->seek(position, RES_BEGIN);
				break;
		case CURRENT:  
			m_pResource->seek(position, RES_CURRENT);
				break;
		case END:      
			m_pResource->seek(position, RES_END);
				break;
		default:      
			return false;
	}

	return true;
}

int ADR_CALL CResourceFile::tell() 
{
	DWORD position;
	if ( m_pResource == NULL) 
		return 0;

	m_pResource->getPos( &position);
	return position;
}

#if AUDIERE_BUILD_H
File* ADR_CALL CResourceFile::duplicate()
{	
	return NULL;
}

int ADR_CALL CResourceFile::readNextNewDataFromFile(void* buffer, int size)
{	
	unsigned long readed;
#ifdef DEBUG_MODE
	ATLTRACE("size:  %d \n",size);
#endif
	if ( m_pResource == NULL) 
		return 0;

	m_pResource->read((unsigned char *)buffer, size, &readed);
	if ( size != readed)
	{	
#ifdef DEBUG_MODE
		ATLTRACE("End of file:  need=%d readed=%d\n",size, readed);
#endif
	}	

	return readed;
}

#endif

/*int CResourceFile::InitWriteMode()
{
	if( FAILED(pResMan->openResource(CComBSTR(name) , RES_REMOTE|RES_CREATE|RES_TO_WRITE, &pResource))) 
		return FALSE;

	return TRUE;
}*/

int CResourceFile::Init( ISoundPtrBase *soundPtr, AudioDevicePtr device)
{
	this->m_pSoundPtr = soundPtr;
	this->m_device = device;
	if ( m_pResource != NULL) 
		return TRUE;

	if ( !ResourceExists())
		return ERROR;	
		  
	if (( m_dwStatus & RES_EXISTS) == 0)
		return FALSE;
	else 
	{
		if (( m_dwStatus & RES_FILE_CACHE) == 0 && (m_dwStatus & RES_LOCAL) == 0)
		{
			// открываем поток
			if ( !m_handle)
			{
				if ( !create())
					return FALSE;				
			}
			m_pSoundPtr->lock();
			m_pSoundPtr->set_loading( TRUE);
			m_pSoundPtr->un_lock();
			resume();			
			return 2;
		}
		else
			return OpenFile();
	}
}

BOOL CResourceFile::ResourceExists()
{
	m_dwFlags = 0;
	m_dwStatus = 0;	
	if ( m_pResMan == NULL) return FALSE;

	if( FAILED( m_pResMan->resourceExists( CComBSTR( m_sName), m_dwFlags, &m_dwStatus)) )
		return ERROR;
	return TRUE;
}

BOOL CResourceFile::OpenFile()
{	
	m_pResource = NULL;
	if( FAILED( m_pResMan->openResource( CComBSTR( m_sName), m_dwStatus | RES_FILE_CACHE, &m_pResource))) 
		return ERROR;
	return TRUE;
}

void CResourceFile::CloseFile() 
{
	if ( m_pResource != NULL)
	{
		m_pResource->destroy();
		m_pResource = NULL;
	}
	m_dwFlags = 0;
	m_dwStatus = 0;
} 

void CResourceFile::main()
{
	while( true)
	{
		// опрос состояние загрузки файла
		/*if ( FALSE)
		{
			soundPtr->set_loading( FALSE);
			soundPtr->lock();
			soundPtr->open( device, this, name);
			soundPtr->un_lock();
		}*/
		if ( m_bTerminating)
			break;
		Sleep( 10);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemFile::CMemFile( const wchar_t *nameFile, ifile* res): 
m_sName(nameFile)
{
	m_dwFlags = 0;
	m_dwStatus = 0;
	m_dwSize = 0;
	if ( res)
	{
		m_pFile = res;
		m_dwSize = m_pFile->get_file_size();
	}
	m_dwCurrentPos = 0;	
	m_bAttached = FALSE;
}

CMemFile::~CMemFile() 
{
	CloseFile();
}

int ADR_CALL CMemFile::read( void* buffer, int size)
{
	CWaitAndSignal waitAndSignal( m_mutex);
	unsigned int readed;
#ifdef DEBUG_MODE
	ATLTRACE("size:  %d \n",size);
#endif
	if ( m_pFile == NULL) 
		return 0;

	if ( m_dwCurrentPos != m_pFile->get_pos())
		m_pFile->seek( m_dwCurrentPos);

	m_pFile->read((TCHAR *)buffer, size, &readed);
	if ( size != readed)
	{	
#ifdef DEBUG_MODE
		ATLTRACE("End of file:  need=%d readed=%d\n", size, readed);
#endif
	}
	m_dwCurrentPos += readed;

	return readed;
}

int ADR_CALL CMemFile::write(const void* buffer, int size) 
{
	CWaitAndSignal waitAndSignal( m_mutex);
	unsigned int wroten;
	if ( m_pFile == NULL) 
		return 0;

	m_pFile->write( ( const TCHAR *) buffer, size, &wroten);
	if ( size != wroten)
	{	
		ATLTRACE("Error writing buffer \n");
		return 0;
	}	

	return size;
}

bool ADR_CALL CMemFile::seek( int position, SeekMode mode) 
{
	CWaitAndSignal waitAndSignal( m_mutex);
	if ( m_pFile == NULL) 
		return 0;

	switch ( mode) 
	{
	case BEGIN: 
		{
			m_pFile->seek( position, RES_BEGIN);
			m_dwCurrentPos = position;
			break;
		}
	case CURRENT:
		{
			m_pFile->seek( position, m_pFile->get_pos());
			m_dwCurrentPos = m_pFile->get_pos();
			break;
		}
	case END:      
		{
			int dwPos = m_pFile->get_file_size();
			if ( position < dwPos)
				dwPos -= position;			
			m_pFile->seek( dwPos, 0);
			m_dwCurrentPos = dwPos;
			break;
		}
	default:      
		return false;
	}

	return true;
}

int ADR_CALL CMemFile::tell() 
{
	CWaitAndSignal waitAndSignal( m_mutex);
	DWORD position;
	if ( m_pFile == NULL) 
		return 0;

	position = m_pFile->get_pos();

	if ( position != m_dwCurrentPos)
	{
		return m_dwCurrentPos;
	}

	return position;
}

#if AUDIERE_BUILD_H

File* ADR_CALL CMemFile::duplicate()
{
	CWaitAndSignal waitAndSignal( m_mutex);
	if ( m_pFile == NULL) 
		return 0;
	ifile *p_mem_file = m_pFile->duplicate();
	ATLASSERT( p_mem_file);
	if ( !p_mem_file)
		return NULL;
	MP_NEW_P2( resFile, CMemFile, m_sName, p_mem_file);
	resFile->m_dwCurrentPos	= m_dwCurrentPos;
	resFile->m_dwSize		= m_dwSize;
	resFile->m_bAttached	= m_bAttached;
	resFile->m_pSoundPtr	= m_pSoundPtr;
	resFile->m_device		= m_device;
	return resFile;
}

int ADR_CALL CMemFile::readNextNewDataFromFile(void* buffer, int size)
{
	CWaitAndSignal waitAndSignal( m_mutex);
	unsigned int readed;
#ifdef DEBUG_MODE
	ATLTRACE("size:  %d \n",size);
#endif
	if ( m_pFile == NULL) 
		return 0;

	if ( m_dwSize != m_pFile->get_pos())
		m_pFile->seek( m_dwSize);

	m_pFile->read((TCHAR *)buffer, size, &readed);
	if ( size != readed)
	{	
#ifdef DEBUG_MODE
		ATLTRACE("End of file:  need=%d readed=%d\n", size, readed);
#endif
	}

	m_dwSize += readed;

	return readed;
}

void CMemFile::destroy()
{
	MP_DELETE_THIS;
}

int ADR_CALL CMemFile::getTotalReadBytes()
{
	return m_dwSize;
}

#endif

int CMemFile::Init( ISoundPtrBase *soundPtr, AudioDevicePtr device, BOOL bAttached)
{
	this->m_pSoundPtr = soundPtr;
	this->m_device = device;
	this->m_bAttached = bAttached;
	if ( m_pFile != NULL)
	{
		if ( !this->m_bAttached)
			CopyFilePtr();
		return TRUE;
	}

	if ( !ResourceExists())
		return ERROR;	

	if (( m_dwStatus & RES_EXISTS) == 0)
		return FALSE;
	else 
	{
		if (( m_dwStatus & RES_FILE_CACHE) == 0 && (m_dwStatus & RES_LOCAL) == 0)
		{
			// открываем поток
			if ( !m_handle)
			{
				if ( !create())
					return FALSE;				
			}
			m_pSoundPtr->lock();
			m_pSoundPtr->set_loading( TRUE);
			m_pSoundPtr->un_lock();
			resume();			
			return 2;
		}
		else
			return OpenFile();
	}
}

BOOL CMemFile::ResourceExists()
{
	m_dwFlags = 0;
	m_dwStatus = 0;	
	//if ( m_pResMan == NULL) return FALSE;

	//if( FAILED( m_pResMan->resourceExists( CComBSTR( m_sName), m_dwFlags, &m_dwStatus)) )
	//	return ERROR;
	return TRUE;
}

BOOL CMemFile::OpenFile()
{	
	//m_pFile = NULL;
	//if( FAILED( m_pResMan->openResource( CComBSTR( m_sName), m_dwStatus | RES_FILE_CACHE, &m_pResource))) 
	//	return ERROR;
	return TRUE;
}

void CMemFile::CloseFile() 
{
	CWaitAndSignal waitAndSignal( m_mutex);
	if ( !m_bAttached && m_pFile != NULL)
	{
		m_pFile->close();
		m_pFile->release();	
		m_pFile = NULL;
	}
	m_dwFlags = 0;
	m_dwStatus = 0;
}

BOOL CMemFile::CopyFilePtr( BOOL bDeleted /* = FALSE */)
{
	CWaitAndSignal waitAndSignal( m_mutex);
	if ( !m_pFile)
		return FALSE;
	ifile *p_mem_file = m_pFile->duplicate();
	ATLASSERT( p_mem_file);
	if ( !p_mem_file)
		return FALSE;

	if ( bDeleted)
	{
		m_pFile->close();
		m_pFile->release();
		m_pFile = NULL;
	}
	m_pFile = p_mem_file;
	if ( m_pFile->get_pos() != m_dwCurrentPos)
		m_pFile->seek( m_dwCurrentPos);
	m_bAttached = FALSE;
	return TRUE;
}

DWORD CMemFile::GetSize()
{
	CWaitAndSignal waitAndSignal( m_mutex);
	if ( !m_pFile)
		return 0xFFFFFFFF;
	return m_pFile->get_file_size();
}

BOOL CMemFile::UpdateState()
{
	CWaitAndSignal waitAndSignal( m_mutex);
	if ( !m_pFile)
		return FALSE;
	m_dwSize = m_pFile->get_file_size();
	return TRUE;
}

BOOL CMemFile::IsAttached()
{
	return m_bAttached;
}

ifile *CMemFile::GetFile()
{
	return m_pFile;
}

void CMemFile::main()
{
	while( true)
	{
		// опрос состояние загрузки файла
		/*if ( FALSE)
		{
			soundPtr->set_loading( FALSE);
			soundPtr->lock();
			soundPtr->open( device, this, name);
			soundPtr->un_lock();
		}*/
		if ( m_bTerminating)
			break;
		Sleep( 10);
	}
}