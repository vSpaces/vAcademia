// FileResource.h: interface for the CFileResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILERESOURCE_H__71A78E19_52BF_4B2F_AA91_C4C60855513E__INCLUDED_)
#define AFX_FILERESOURCE_H__71A78E19_52BF_4B2F_AA91_C4C60855513E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResourceImpl.h"

// Класс реурсов для отладки CResMan
class CFileResource : public CResourceImpl
{
public:
	CFileResource();
	virtual ~CFileResource();

	// IResource
	virtual long close();
	long read(BYTE * aBuffer, DWORD aBufferSize, DWORD * aReadSize, DWORD *aErrorRead = 0);
	long write(const BYTE* aBuffer, DWORD aBufferSize, DWORD * aWroteSize, DWORD aTotalSize = 0);
	long seek( DWORD aPos, BYTE aFrom);
	long getPos( DWORD* aPos);
	long getSize( DWORD* aSize);
	long setSize( DWORD aSize);
	//long getLastModifyDateTime(DWORD* aDateTime);

	virtual void open( const wchar_t* aResPath, DWORD aFlags, IResourceNotify *aNotify = NULL, const wchar_t* aResRefinePath = NULL);
	virtual void unpackRes( DWORD aCachingBlock, bool* aEnabled);
	virtual void getTime( FILETIME& aTime);

protected:
	HANDLE hFile;
};

#endif // !defined(AFX_FILERESOURCE_H__71A78E19_52BF_4B2F_AA91_C4C60855513E__INCLUDED_)
