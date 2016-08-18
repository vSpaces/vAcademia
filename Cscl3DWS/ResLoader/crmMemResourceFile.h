// crmMemResourceFile.h: interface for the crmMemResourceFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_crmMemResourceFile_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_)
#define AFX_crmMemResourceFile_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef RESLOADER_EXPORTS
#define RESLOADER_API __declspec(dllexport)
#else
#define RESLOADER_API __declspec(dllimport)
#endif

class resIResource;

#include "ifile.h"

//class crmMemResourceFile  : public CMemFileEx
class RESLOADER_API crmMemResourceFile  : public ifile
{
	friend class crmResLibrary;
	MP_WSTRING	m_pFilePath;
	res::resIResource*	pResource;
	DWORD	dwLocation;
	void*	lpData;
	DWORD	dwSize;
	__int64 modifyTime;
	crmResLibrary*	m_pResLibrary;
	bool	bAttached;	// память, на которую указывает lpData приаттачена методом attach,
						// поэтому ее освобождать на free_file_data не надо
public:
	crmMemResourceFile(crmResLibrary*	a_pResLibrary);
	crmMemResourceFile(crmResLibrary*	a_pResLibrary, res::resIResource*	apResource);
	virtual ~crmMemResourceFile();

	// реализация natura3d::ifile
public:
	// CFileEx	overrides
	BOOL		open(	const wchar_t* lpFile, DWORD dwCreateDisp = 0 ) throw( ifileexception );
	const void*	get_file_data( DWORD* size);
	void		free_file_data();
	void		close();
	UINT		read( TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	DWORD		read( BYTE* apBuffer, DWORD adwSize ) throw( ifileexception );
	void		write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	DWORD		seek( DWORD lToMove, DWORD dwMoveFrom = FILE_BEGIN ) throw( ifileexception );
	DWORD		seek_to_begin() throw( ifileexception );
	DWORD		seek_to_end() throw( ifileexception );
	DWORD		get_pos() throw( ifileexception );
	DWORD		get_file_size();
	bool		attach(void* data, DWORD size);
	void*		detach();
	LPCWSTR		get_file_path();
	void		set_file_path( LPCWSTR alpFilePath);
	ifile*		duplicate();
	void		release();
	__int64		get_modify_time();
	void set_modify_time(__int64 time) { modifyTime = time; }


protected:
	// CFileEx	overrides
	BOOL	Open(	const wchar_t* lpFile, DWORD dwCreateDisp = OPEN_EXISTING ) throw( ifileexception );
	DWORD	SeekToEnd();
	DWORD	SeekToBegin();
	DWORD	Seek( DWORD lToMove, DWORD dwMoveFrom);
	DWORD	GetPos();
	LPVOID	Detach();
	DWORD	GetFileSize();
	void	Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	DWORD	Read( BYTE* pBuffer, DWORD dwSize );
	UINT	Read( TCHAR* pszBuffer, UINT uCount, UINT* puCount );

};

#endif // !defined(AFX_crmMemResourceFile_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_)
