#pragma once

#include "WinThread.h"
#include "Mutex.h"

using namespace audiere;

class CResourceFile;

class CSndFileLoaderPlayer : public CWinThread, public ISndFileLoaderPlayer
{
public:
	CSndFileLoaderPlayer( ISoundPtrBase *pSoundPtr);
	~CSndFileLoaderPlayer();

public:
	BOOL load( AudioDevicePtr device, File *resFile);
	void main();
	void createBuffer();
	void releaseBuffer();
	void releaseSound();

private:
	ISoundPtrBase *m_pSoundPtr;
	AudioDevicePtr m_pDevice;
	File *m_pResFile;
	BOOL bCreateBufferNeed;
	BOOL bReleaseBufferNeed;
	BOOL bReleaseSoundNeed;
};
