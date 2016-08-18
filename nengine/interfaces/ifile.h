#pragma once

#define FILEEX_ERR_NOERROR			0x00
#define FILEEX_ERR_INVALID_OPCLOSEDFILE		0x01
#define FILEEX_ERR_PARAM_OUTOFRANGE		0x02
#define FILEEX_ERR_OTHERIO_CURRENTLY_RUNNG	0x03
#define FILEEX_ERR_INVALIDPARAM			0x04

struct ifileexception
{
	virtual	DWORD		get_custom_error() = 0;
	virtual	DWORD		get_win_error_code() = 0;
	virtual	LPCTSTR		get_custom_error_msg() = 0;
	virtual	LPCTSTR		get_win_error_msg() = 0;
};

#pragma  warning (disable: 4290)
	
struct ifile
{
	virtual	~ifile(){}

	virtual	const void*	get_file_data( DWORD* size) = 0;
	virtual	void		free_file_data() = 0;
	virtual	BOOL		open(const wchar_t* lpFile, DWORD dwCreateDisp = 0 ) throw( ifileexception ) = 0;
	virtual	void		close() = 0;
	virtual	UINT		read( TCHAR* pszBuffer, UINT uCount, UINT* puCount ) = 0;
	virtual	DWORD		read( BYTE* pBuffer, DWORD dwSize ) throw( ifileexception ) = 0;
	virtual	void		write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount ) = 0;
	virtual	DWORD		seek( DWORD lToMove, DWORD dwMoveFrom = FILE_BEGIN ) throw( ifileexception ) = 0;
	virtual	DWORD		seek_to_begin() throw( ifileexception ) = 0;
	virtual	DWORD		seek_to_end() throw( ifileexception ) = 0;
	virtual	DWORD		get_pos() throw( ifileexception ) = 0;
	virtual	DWORD		get_file_size() = 0;
	virtual	__int64		get_modify_time() = 0;
	virtual void		set_modify_time(__int64 time) = 0;
	virtual	bool		attach(void* data, DWORD size) = 0;
	virtual	void*		detach() = 0;
	virtual	LPCWSTR		get_file_path() = 0;
	virtual void		set_file_path( LPCWSTR alpFilePath) = 0;
	virtual ifile*		duplicate() = 0;
	virtual void		release() = 0;
};


