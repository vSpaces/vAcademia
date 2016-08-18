// crmResourceFile.h: interface for the crmResourceFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRMRESOURCEFILE_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_)
#define AFX_CRMRESOURCEFILE_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
class resIResource;

//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

#ifdef RESLOADER_EXPORTS
#define RESLOADER_API __declspec(dllexport)
#else
#define RESLOADER_API __declspec(dllimport)
#endif

#pragma warning (disable : 4251)
#pragma warning (disable : 4275)

#include "ifile.h"

//////////////////////////////////////////////////////////////////////////
//class crmResourceFile  : public CFileEx
class RESLOADER_API crmResourceFile  : public ifile
{
	friend class crmResLibrary;
	crmResLibrary*		m_pResLibrary;
	MP_WSTRING			m_pFilePath;
	res::resIResource*	pResource;
	__int64 m_modifyTime;
public:
	crmResourceFile(crmResLibrary*	a_pResLibrary);
	crmResourceFile(crmResLibrary*	a_pResLibrary, res::resIResource*	apResource);
	virtual ~crmResourceFile();

	// реализация natura3d::ifile
public:
	const void*	get_file_data( DWORD* size);
	void		free_file_data();
	BOOL		open(	const wchar_t* lpFile, DWORD dwCreateDisp = 0 ) throw( ifileexception );
	void		close();
	UINT		read( TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	DWORD		read( BYTE* pBuffer, DWORD dwSize ) throw( ifileexception );
	void		write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	DWORD		seek( DWORD lToMove, DWORD dwMoveFrom = FILE_BEGIN ) throw( ifileexception );
	DWORD		seek_to_begin() throw( ifileexception );
	DWORD		seek_to_end() throw( ifileexception );
	DWORD		get_pos() throw( ifileexception );
	DWORD		get_file_size()
				{	return GetFileSize();	}
	bool		attach(void* /*data*/, DWORD /*size*/){ return false;}
	void*		detach(){ return NULL;}
	LPCWSTR		get_file_path();
	__int64		get_modify_time();
	void set_modify_time(__int64 time) { m_modifyTime = time; }

	void	set_file_path( LPCWSTR alpFilePath)
	{
		ATLASSERT( alpFilePath);
		if( alpFilePath)
			m_pFilePath = alpFilePath;
		else
			m_pFilePath = L"";
	}

//////////////////////////////////////////////////////////////////////////
public:
	// CFileEx	overrides
	BOOL		Open(	const wchar_t* lpFile, DWORD dwCreateDisp = 0 );
	void		Close();
	UINT		Read( TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	DWORD		Read( BYTE* pBuffer, DWORD dwSize );
	void		Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	DWORD		Seek( DWORD lToMove, DWORD dwMoveFrom = FILE_BEGIN );
	DWORD		SeekToBegin();
	DWORD		SeekToEnd();
	DWORD		GetPos();
	DWORD		GetFileSize();
	ifile*		duplicate();
	void		release();

private:
	LPVOID		m_pGetFileDataPtr;

};

#endif // !defined(AFX_CRMRESOURCEFILE_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_)
