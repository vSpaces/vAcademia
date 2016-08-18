// n3dfile.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IN3DFILE_H__2EA8A433_3318_4E3E_A71D_E986DA9A4673__INCLUDED_)
#define AFX_IN3DFILE_H__2EA8A433_3318_4E3E_A71D_E986DA9A4673__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace	natura3d
{
	#define FILEEX_ERR_NOERROR					0x00
	#define FILEEX_ERR_INVALID_OPCLOSEDFILE		0x01
	#define FILEEX_ERR_PARAM_OUTOFRANGE			0x02
	#define FILEEX_ERR_OTHERIO_CURRENTLY_RUNNG	0x03
	#define FILEEX_ERR_INVALIDPARAM				0x04

	struct ifileexception
	{
		virtual	DWORD		get_custom_error() = 0;
		virtual	DWORD		get_win_error_code() = 0;
		virtual	LPCTSTR		get_custom_error_msg() = 0;
		virtual	LPCTSTR		get_win_error_msg() = 0;
	};
	
	struct ifile
	{
		virtual	~ifile(){}

		virtual	void*		get_file_data(LPVOID *data, DWORD* size) = 0;
		virtual	void		free_file_data(LPVOID data) = 0;
		virtual	BOOL		open(	LPCTSTR lpFile, DWORD dwCreateDisp = 0, 
							DWORD dwAccess = GENERIC_READ | GENERIC_WRITE, DWORD dwShare = 0, 
							LPSECURITY_ATTRIBUTES lpSec = NULL ) throw( ifileexception ) = 0;
		virtual	void		close() = 0;
		virtual	UINT		read( TCHAR* pszBuffer, UINT uCount, UINT* puCount ) = 0;
		virtual	DWORD		read( BYTE* pBuffer, DWORD dwSize ) throw( ifileexception ) = 0;
		virtual	void		write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount ) = 0;
		virtual	DWORD		seek( DWORD lToMove, DWORD dwMoveFrom = FILE_BEGIN ) throw( ifileexception ) = 0;
		virtual	DWORD		seek_to_begin() throw( ifileexception ) = 0;
		virtual	DWORD		seek_to_end() throw( ifileexception ) = 0;
		virtual	DWORD		get_file_size() = 0;
		virtual	bool		attach(void* data, DWORD size) = 0;
		virtual	void*		detach() = 0;
		virtual	LPCTSTR		get_file_path() = 0;
	};
};

#endif // !defined(AFX_IN3DFILE_H__2EA8A433_3318_4E3E_A71D_E986DA9A4673__INCLUDED_)