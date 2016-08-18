// FileResource.h: interface for the CBigFileResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIGFILERESOURCE_H)
#define AFX_BIGFILERESOURCE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResourceImpl.h"
#include "BigFileman.h"

// Класс реурсов для отладки CResMan
class CBigFileResource : public CResourceImpl
{
public:
	CBigFileResource();
	virtual ~CBigFileResource();

	// IResource
	virtual long close();
	long read(BYTE * aBuffer, DWORD aBufferSize, DWORD * aReadSize, DWORD *aErrorRead = 0);
	long write(const BYTE* aBuffer, DWORD aBufferSize, DWORD * aWroteSize, DWORD aTotalSize = 0);
	long seek( DWORD aPos, BYTE aFrom);
	long getPos( DWORD* aPos);
	long getSize( DWORD* aSize);
	long setSize( DWORD aSize);
	long getStatus(DWORD* aSize);
	//long getLastModifyDateTime(DWORD* aDateTime);

	void setBigFileManagerReader(bigfile::IBigFileManagerReader *apBigFileManagerReader);

	virtual void open( const wchar_t* aResPath, DWORD aFlags, IResourceNotify *aNotify = NULL, const wchar_t* aResRefinePath = NULL);
	virtual void unpackRes( DWORD aCachingBlock, bool* aEnabled);
	virtual void getTime( FILETIME& aTime);

protected:
	bigfile::IBigFileManagerReader *pBigFileManagerReader;
	unsigned int realOffset;
	unsigned int offset;
	__int64 time;
};

#endif // !defined(AFX_BIGFILERESOURCE_H)
