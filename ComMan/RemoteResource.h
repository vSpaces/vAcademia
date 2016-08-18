// RemoteResource.h: interface for the CRemoteResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTERESOURCE_H__30C8B35F_A771_4934_AA97_275FB7F1F8AF__INCLUDED_)
#define AFX_REMOTERESOURCE_H__30C8B35F_A771_4934_AA97_275FB7F1F8AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResourceImpl.h"

#define FIRST_BLOCK_SIZE 65535

class CRemoteResource : public CResourceImpl
{
public:
	CRemoteResource();
	virtual ~CRemoteResource();

	virtual void open( const wchar_t* aResPath, DWORD aFlags, IResourceNotify *aNotify = NULL, const wchar_t* aResRefinePath = NULL);
	virtual void getTime( FILETIME& aTime);

	// IResource
	virtual long close();
	long read(BYTE * aBuffer, DWORD aBufferSize, DWORD *aReadSize, DWORD *aError);
	long write(const BYTE* aBuffer, DWORD aBufferSize, DWORD *aWroteSize, DWORD aTotalSize);
	long seek(DWORD aPos, BYTE aFrom);
	long getPos(DWORD* aPos);
	long getSize(DWORD* aSize);
	long getUploadedSize( DWORD *aSize);
	long getStatus(DWORD* aSize);
	long upload(IResource* aSrcRes, IResourceNotify* aNotify);
	long registerAtResServer( IResourceNotify* aNotify);

protected:
	void	updateInfo(BOOL aAlways=FALSE);
	void	setInfo( DWORD aStatus, DWORD aSize, FILETIME aTime);
	long	getUploadInfo(byte &btStatusServerCode, std::wstring &sErrorText);
	/*// Размер файла
	DWORD dwSize;*/
	// Размер загруженной части файла
	DWORD dwDownloadedSize;
	// Текущая позиция
	DWORD dwPos;
	FILETIME oTime;
	unsigned char* pCacheData;
	DWORD dwCachedSize;
	WORD wFirstBlockSize;
	BYTE btFirstBlock[ FIRST_BLOCK_SIZE];
	BOOL	flag;
	BOOL	bInfoSet;
	BOOL	bTimeSet;
	HANDLE hRegisterAtResServerThread;

protected:
	// реализация uploading-а
	int uploadThread();
	int uploadRes();
	void onUploadEnd();

	int registerAtResServerThread();
	int registerFileAtResServer();

private:
	static DWORD WINAPI cacheThread_(LPVOID param);
	static DWORD WINAPI uploadThread_(LPVOID param);
	static DWORD WINAPI registerAtResServerThread_(LPVOID param);


};

#endif // !defined(AFX_REMOTERESOURCE_H__30C8B35F_A771_4934_AA97_275FB7F1F8AF__INCLUDED_)
