// ResFile.h: interface for the CResourceFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESFILE_H__1CE248A4_135E_4F2A_B02D_700727261777__INCLUDED_)
#define AFX_RESFILE_H__1CE248A4_135E_4F2A_B02D_700727261777__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "audiere.h"
using namespace audiere;

#include <fstream>
#include "WinThread.h"
#include "ComManRes.h"
#include "Mutex.h"
using namespace res;
struct ISoundPtrBase;

class CResourceFile: public RefImplementation<File>, public CWinThread
{
public:
	CResourceFile( IResMan *spResMan, const wchar_t *nameFile, IResource* res = NULL);
	~CResourceFile();

	void destroy();

public:
	int  ADR_CALL read(void* buffer, int size);
	int  ADR_CALL write(const void* buffer, int size);
	bool ADR_CALL seek(int position, SeekMode mode);
	int  ADR_CALL tell();
#if AUDIERE_BUILD_H
	File* ADR_CALL duplicate();
	int  ADR_CALL readNextNewDataFromFile(void* buffer, int size);
#endif

public:
	//int InitWriteMode();
	int Init( ISoundPtrBase *soundPtr, AudioDevicePtr device);
	BOOL ResourceExists();
	BOOL OpenFile();        
	void CloseFile();

public:
	void main();

private:
	res::IResource *m_pResource;
	res::IResMan *m_pResMan;
	const wchar_t *m_sName;
	WORD m_dwFlags;
	DWORD m_dwStatus;
	ISoundPtrBase *m_pSoundPtr;
	AudioDevicePtr m_device;
};


class CMemFile: public RefImplementation<File>, public CWinThread
{	
public:
	CMemFile( const wchar_t *nameFile, ifile *pfile);
	~CMemFile();

	void destroy();

public:
	int  ADR_CALL read(void* buffer, int size);
	int  ADR_CALL write(const void* buffer, int size);
	bool ADR_CALL seek(int position, SeekMode mode);
	int  ADR_CALL tell();
#if AUDIERE_BUILD_H
	File* ADR_CALL duplicate();
	int  ADR_CALL readNextNewDataFromFile(void* buffer, int size);
	int  ADR_CALL getTotalReadBytes();
#endif

public:
	//int InitWriteMode();
	int Init( ISoundPtrBase *soundPtr, AudioDevicePtr device, BOOL bAttached);
	BOOL ResourceExists();
	BOOL OpenFile();        
	void CloseFile();
	BOOL CopyFilePtr( BOOL bDeleted = FALSE);
	DWORD GetSize();
	BOOL UpdateState();
	BOOL IsAttached();
	ifile *GetFile();

public:
	void main();

private:
	ifile *m_pFile;	
	const wchar_t *m_sName;
	DWORD m_dwFlags, m_dwStatus;
	ISoundPtrBase *m_pSoundPtr;
	AudioDevicePtr m_device;
	CMutex m_mutex;
	DWORD m_dwCurrentPos;
	DWORD m_dwSize;
	BOOL m_bAttached;
};

#endif // !defined(AFX_RESFILE_H__1CE248A4_135E_4F2A_B02D_700727261777__INCLUDED_)
