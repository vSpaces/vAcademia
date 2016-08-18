// SoundManager.h: interface for the CSoundManager class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SOUND_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_)
#define AFX_SOUND_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_


#include "res/res.h"

#ifndef REC_H
#include "recognize.h"
#define REC_H
#endif
#include "ResFile.h"

#include <dsound.h>
#include <iostream>


#include "SoundPtr.h"
#include "Listener.h"
#include "WinThread.h"
#include "3DSoundFactor.h"

using namespace oms;
using namespace sm;
using namespace res;

// General sound manager class
class CSoundManager : public sm::smISoundManager
{
public:
	CSoundManager( omsContext* aContext);
	virtual ~CSoundManager();

public:		
	// Открытие звукового потока на воспроизведение
	int OpenAudiereOutput();	

protected:
	BOOL LoadSoundFile( ISoundPtrBase *soundPtr, LPCWSTR name, ifile *pfile, BOOL bImmediate, BOOL bAttached = FALSE);
	BOOL LoadSound( ISoundPtrBase *soundPtr, LPCWSTR name, res::IResource* res, BOOL bImmediate);

	// реализация интерфейса sm::smISoundManager
public:	
	//
	oms::omsresult CreateMO( mlMedia* apMLMedia);

	//
	oms::omsresult DestroyMO( mlMedia* apMLMedia);

	// (0 - звук выключен, 1 - звук включен)
	oms::omsresult SetMode(int /*aiMode*/){ return OMS_ERROR_NOT_IMPLEMENTED; };

	// получить режим менеджера звука 
	int GetMode(){ return 1; }

	// установить уровень громкости звука
	oms::omsresult SetVolume(double adVol);//{	return OMS_ERROR_NOT_IMPLEMENTED; };

	// получить уровень громкости звука
	double GetVolume(){ return 1.0; };

	// реализация интерфейса sm::smISoundManager
public:
	omsresult setResManager( IResMan *aResMan);
	omsresult setCooperativeLevel( UINT hWnd, UINT level);
	omsresult openSoundFile( ifile* pfile, BOOL bImmediate, sm::smISoundNotify *apISoundNotify = NULL, BOOL bAttached = FALSE);
	omsresult openSound( LPCWSTR name, IResource* res, BOOL bImmediate, sm::smISoundNotify *apISoundNotify = NULL);
	omsresult OpenSoundFile( ifile *pfile, BOOL immediate, int *id, sm::smISoundNotify *apISoundNotify = NULL, bool bAttached = FALSE);
	omsresult openSoundFile( int aiSoundIndex, ifile* pfile, BOOL bImmediate, sm::smISoundNotify *apISoundNotify = NULL, BOOL bAttached = FALSE);
	omsresult OpenSoundRes( IResource* res, LPCWSTR name, BOOL immediate, int *id, sm::smISoundNotify *apISoundNotify = NULL);
	omsresult playSound( int index, int num_person, BOOL loop);
	omsresult stopSound( int index);	
	BOOL closeSound( int id);
	void removeSound( ISoundPtrBase *soundPtr);
	
	// реализация интерфейса sm::smISoundManager
public:
	BOOL IsPlaying( int index);
	BOOL IsLoaded( int index);
	DWORD GetPosition( int index);
	omsresult GetPosition( int id, DWORD *time);
	DWORD GetFrequency( int index);
	omsresult GetFrequency( int index, DWORD *time);
	DWORD GetDuration( int index);
	omsresult GetDuration( int index, DWORD *time);	
	DWORD GetLength( int index);
	omsresult GetLength( int index, DWORD *length);
	DWORD GetSize( int index);
	omsresult GetSize( int index, DWORD *size);
	DWORD GetCurrentSize( int index);
	omsresult GetCurrentSize( int index, DWORD *size);
	omsresult GetVolume( int id, float *volume);
	void SetVolume( float volume);
	BOOL SetPosition( int index, DWORD time);
	BOOL Set3DSoundCoef( int id, float coef, float left_coef, float right_coef);
	BOOL Set3DSoundParams( int id, float distX, float distY, float distZ, float dirX, float dirY, float dirZ, float vp_dirX, float vp_dirY, float vp_dirZ);
	BOOL Set3DSoundParams( int id, C3DSoundFactor *p3DSoundFactor);
	BOOL SetSoundType( int id, int type);
	BOOL SetVolume( int id,  float volume);
	BOOL SetPitchShift( int id,  float shift);
	BOOL UpdateAllSound( int act);
	omsresult UpdateFileInfo( int id, int loaded_bytes, bool bLoaded);
	//omsresult GetSoundOutputDevices( std::wstring& soundOutputDevices);
	omsresult ReplaceDevice(std::wstring &sDeviceName);

public:
	int FindSound( int id);
	void DeleteElementInSoundArray( int index);
	

public:
	void FreezeAll();
	void UnFreezeAll();
	void Silence( BOOL mode);

// Lipsync
public:
	omsresult GetLexema( int num_person, int *lexema);
	int RecognizeLexemaAudiereSound( int position);
	int RecognizeLexema();
	int RecognizeLexemaBeforeSend( LPSTR buffer, WORD buflen);
	
//	IAsyncConnect *spSConnect;
//	WORD id;								// идентификатор от рабочего пространства

public:
	void setSoundBase( BSTR newVal);	
	AudioDevicePtr GetDevice();
	void setResMan( res::IResMan *resMan);
	void Lock();
	void UnLock();
	
protected:
	int num_sound_handler;
	DWORD NewPlaySoundPosition, PreviousPlaySoundPosition;
	int PreviousLexema;

public:
	BOOL BSoundThread, BPausa;
	int iSndRemoving;

protected:
	DWORD prevTime;
	DWORD prevRcvTime;

	res::IResMan *mpResMan;
	omsContext* m_pContext;
	int IndexThread;
	
	int PositionPlaySoundThread;
	long start;
	long end;
	
	AudioDevicePtr device;
	DWORD interval;
	
	std::ofstream fLog;
	CComString sSoundBase;

	int num_avatar;	

protected:
	CMutex mutex;
	CMutex mutexOpenClose;
	CMutex mutexPosition;
	CMutex mutexUpdateData;
	BOOL bDeviceChanging;
	// Array of threads
	//CPlaySoundThread PlaySoundThreads[ MAX_SOUNDS];
	// Array for playing different sounds	
	CSoundPtrMap soundPtrMap;
	//
	CListener listener;
	//
	C3DSoundFactor t3dListenerSoundParams;
	// Lipsync 
	CRecognize recognize;
	// список потоков, ожидающих загрузку звука, чтобы начать проигрывание звука
	//std::vector<CPlaySoundThread*> plSndThreadQueue;
	// общее состояние для всех звуков в процессе воспроизведения
	int playingState;
};



#endif // !defined(AFX_SOUND_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_)
