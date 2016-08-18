// SoundPtr.h: interface for the CSoundPtr and CSoundPtrMap classes
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SOUNDPTR_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_)
#define AFX_SOUNDPTR_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_

#include "ResFile.h"

using namespace audiere;

struct smISoundNotify;
class CSndFileLoaderPlayer;
class CStopCallback;

template <class TSound>
class CSoundPtrBase : public ISoundPtrBase
{ 
public:	
	CSoundPtrBase();	

	virtual ~CSoundPtrBase();	

public:
	virtual void init();

	void set_notify( sm::smISoundNotify *apISoundNotify);
	sm::smISoundNotify* get_notify();

	void set_id( int id);

	int get_id();

	void set_name( const wchar_t *fileName);

	const wchar_t *get_name();

public:
	void lock();

	void un_lock();

	virtual DWORD get_pos( int flag);

	virtual void set_pos( DWORD pos);

	virtual float get_volume();

	virtual void set_volume( float volume);

	virtual float get_pan();

	virtual void set_pan( float pan);

	virtual BOOL play();

	virtual BOOL is_play();

	virtual BOOL is_playing();

	virtual BOOL is_stopped(){ return !is_playing();};

	virtual void stop();

#if AUDIERE_RELEASE_P
	virtual void stop_and_wait();
#endif

	virtual void set_repeat();

	virtual void set_loop( BOOL bLoop);

	virtual BOOL is_loop();

	virtual BOOL is_seekable();

	virtual DWORD get_frequency();

	virtual DWORD get_length_in_ms();

	virtual DWORD get_length();

#if AUDIERE_BUILD_H
	virtual DWORD get_size();	
#endif

#if SOUND_MAN_RELEASE_H
	virtual DWORD get_current_size();
#endif

	virtual DWORD get_buffer_size();

	virtual LPSTR get_buffer( DWORD *size);

	virtual void set_pitch_shift( float shift);

	virtual float get_pitch_shift( float shift);

public:	
	virtual BOOL open( AudioDevicePtr device, File *resFile, const wchar_t *fileName) = 0;
	virtual BOOL open_async( AudioDevicePtr device, File *resFile, const wchar_t *fileName);

	virtual BOOL on_loaded( int iError);
	virtual BOOL on_play_starting();
	virtual BOOL on_play_started( int iError);
	virtual void on_stopped( StopEvent::Reason reason);

	virtual void set_loading( BOOL bLoading);

	virtual BOOL is_loading();

	virtual BOOL is_loaded();

	virtual void close();

	virtual void clear();

public:
	// устанавливает параметры, что звук начнется воспроизводится
	virtual void set_play( int iPlay);

	virtual void enabled();

	virtual void disabled();

	virtual BOOL is_enabled();

	virtual void params_saver();

	virtual BOOL apply_params();

	virtual BOOL is_params_saver();

	virtual ISndFileLoaderPlayer *get_snd_file_loader_player();

#if AUDIERE_BUILD_H
	int update_file_info( int loaded_bytes, bool bLoaded);
#endif

#if AUDIERE_RELEASE_P
	virtual BOOL is_res_attached(){ return FALSE;};
	virtual ifile *get_file(){ return NULL;};
	virtual void freeze(){};
	virtual void unfreeze(){};
	virtual BOOL is_freeze(){ return FALSE;};
	virtual void prepare_for_delete(){};
	virtual BOOL is_prepare_for_delete(){return FALSE;};
#endif

protected:
	void _apply_params();

protected:
	// ссылка на звуковой ресурс
	//CResourceFile *m_pResFile;
	// идентификатор звука
	int m_iId;
	// TRUE идет проигрывание
	int  m_iPlay;
	// TRUE - файл загружен
	BOOL m_bLoadedFile;
	// TRUE - идет процесс загрузки файла
	BOOL m_bLoading;
	// TRUE - указанаие на циклическое воспроизведение звука
	BOOL m_bLoop;
	// имя звука
	//char m_sName[ MAX_PATH];
	CWComString m_sName;
	//
	CMutex m_mutex;
	//
	CMutex m_mutexSoundNotify;
	// уровень громкости звука
	float m_fVolume;
	// соотношение уровней громкости левого и правого канала
	float m_fPanVolume;	
	// позиция проигрывания звука
	DWORD m_dwPosition;
	// звук
	TSound m_sound;
	// TRUE - указывает, что звук начнется воспроизводиться (команда уже послана)
	BOOL m_bStartPlay;
	// TRUE - указывает на то, что параметры звука можно менять
	BOOL m_bEnabled;
	// TRUE - указывает на то, что параметры звука можно менять, но не будут применяться для объекта звука audiere, 
	// пока не будет пересоздан буфер
	BOOL m_bFreezzed;
	// указывает, что звук audiere удален
	BOOL m_bASndRemoved;
	// TRUE - дает возможность сохранить параметры и применить их, когда будет возможно
	// Параметры будут применяться только тогда когда будет вызвана функция _apply_params
	BOOL m_bParamsSaver;
	// осуществляет асинхронную загрузка файла и декодировку
	CSndFileLoaderPlayer *m_pSndFileLoaderPlayer;
	// уведомление о завершении работы звукового объекта и его псоледующего удаления
	BOOL m_bTerminating;
	// уведомление о событиях
	sm::smISoundNotify *m_pISoundNotify;
	//
	BOOL m_bStream;
	//
	File *m_pResFile;
#if AUDIERE_RELEASE_O
#else
	// обработчик, вызывающий после остановки воспроизведения звука
	RefPtr<CStopCallback> m_pStopCallBack;
#endif
	// Значение TRUE указывает, что проигрывание звук остановлен и обработчик m_pStopCallBack выполнил свое действие
	BOOL m_bStopped;
	// Сслыка на абстрактное аудио-устройство
	AudioDevicePtr m_pDevice;

	enum{ STOP_BY_USER = 1, STOP_BY_ENDFILE};
};

class CSoundStreamPtr : public CSoundPtrBase<OutputStreamPtr>
{
public:
	CSoundStreamPtr();
	~CSoundStreamPtr();

public:
	BOOL open( AudioDevicePtr device, File *resFile, const wchar_t *fileName);
	BOOL open( AudioDevicePtr device, File *resFile, const wchar_t *fileName, bool streaming);	
	void close();
	void clear();

public:
	void release();
	DWORD get_pos( int flag);
	void set_pos( DWORD pos);
	BOOL is_play();
	BOOL is_playing();
	BOOL is_stopped();
#if AUDIERE_RELEASE_P
	void stop_and_wait();
#endif
	void set_repeat();
	BOOL is_seekable();	
	DWORD get_frequency();
	DWORD get_length_in_ms();	
	DWORD get_length();	
#if AUDIERE_BUILD_H
	DWORD get_size();
#endif
#if SOUND_MAN_RELEASE_H
	DWORD get_current_size();
#endif
	DWORD get_buffer_size();	
	LPSTR get_buffer( DWORD *size);
#if AUDIERE_BUILD_H
	int update_file_info( int loaded_bytes, bool bLoaded);
#endif
#if AUDIERE_RELEASE_P
	BOOL is_res_attached();
	ifile *get_file();
	void freeze();
	void unfreeze();
	BOOL is_freeze();
	void create_buffer();
	void release_buffer();
	void prepare_for_delete();
	BOOL is_prepare_for_delete();
	void release_sound();
#endif

public:	
	void save_params();
	void restore_params();
	BOOL is_params_saved();

private:	
	float m_fVolumePrev;
	float m_fPanVolumePrev;	
};

class CSoundEffectPtr : public CSoundPtrBase<SoundEffectPtr>
{
public:
	CSoundEffectPtr();
	~CSoundEffectPtr();

public:
	BOOL open( AudioDevicePtr device, File *resFile, const wchar_t *fileName);
	void close();

public:	
	BOOL is_play();
	BOOL is_playing();		
	DWORD get_buffer_size();	
	LPSTR get_buffer( DWORD *size);	
	void set_pitch_shift( float shift);
	float get_pitch_shift( float shift);
#if AUDIERE_BUILD_H
	int update_file_info( int loaded_bytes, bool bLoaded);
#endif
};


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

class CSoundManager;

class CSoundPtrMap : public CObjectsMap<ISoundPtrBase *>, public CWinThread
{
public:
	CSoundPtrMap();
	virtual ~CSoundPtrMap();	

public:
	BOOL remove( int ID, CSoundManager *pSoundManager, BOOL bAsync, BOOL bRemove = FALSE);
	BOOL remove( ISoundPtrBase *soundPtr, CSoundManager *pSoundManager, BOOL bAsync, BOOL bRemove = FALSE);
	int get_new_id();
	void remove_all_objects();
	void remove_all_objects( CSoundManager *pSound);
	BOOL update_all_sound( int act, CSoundManager *pSound);
	// остановить проигрывание всех звуков
	void stop_all_sounds();

protected:
	// запретить изменять параметры звука
	void set_all_sounds_disabled();
	// разрешить изменять параметры звука
	void set_all_sounds_enabled();	

protected:
	CMutex mutex;
};

#endif // !defined(AFX_SOUNDPTR_H__54B0AECF_085B_4AC2_9BEE_FD00DC0B82A4__INCLUDED_)