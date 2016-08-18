// crmMemResourceFile.h: interface for the crmMemResourceFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_crmMemResourceFile_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_)
#define AFX_crmMemResourceFile_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class crmMemResourceFile
{
	DWORD	dwLocation;
	void*	lpData;
	DWORD	dwSize;
public:
	crmMemResourceFile();
	virtual ~crmMemResourceFile();

public:
	void	Attach( void* data, DWORD size);
	DWORD	Seek( DWORD lToMove, DWORD dwMoveFrom);
	LPVOID	Detach();
	DWORD	GetFileSize();
	void	Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount );
	DWORD	Read( BYTE* pBuffer, DWORD dwSize );
	UINT	Read( TCHAR* pszBuffer, UINT uCount, UINT* puCount );
};

#endif // !defined(AFX_crmMemResourceFile_H__26FE0C79_240E_48B4_A652_0AA112168892__INCLUDED_)
