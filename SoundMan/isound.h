using namespace audiere;

// enums
namespace sm
{
	enum SOUNDS_ACT {SOUNDS_DISABLED, SOUNDS_ENABLED, SOUNDS_PLAY, SOUNDS_STOP};	

	#define WARNING_RES_IS_NOT_UPDATED			11
	#define WARNING_RES_IS_OPENING				12
	#define WARNING_RES_IS_NOT_NEED_TO_UPDATE	13

	#define ERROR_RES_IS_NOT_OPENED		1
	#define ERROR_RES_INVALID			2
	#define RMML_ERROR_FILE_INVALID			3
	#define ERROR_FILE_OPENING			4
	#define ERROR_FILE_STREAM_READ		5
}

// functions
#define SAFE_DELETE(p)       MP_DELETE( p)
#define SAFE_DELETE_ARRAY(p) MP_DELETE_ARR( p)
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }

#define SOUND_MAN_RELEASE_H 1

// interfaces
//#include "ComManRes.h"
#include "sm/sm.h"

using namespace sm;

struct ISndFileLoaderPlayer
{
	virtual BOOL load( AudioDevicePtr device, File *resFile) = 0;	
};

struct ISoundPtrBase
{
	ISoundPtrBase(){};
	virtual ~ISoundPtrBase(){};

	virtual void init() = 0;
	virtual void set_notify( sm::smISoundNotify *apISoundNotify) = 0;
	virtual sm::smISoundNotify* get_notify() = 0;
	virtual void set_id( int id) = 0;
	virtual int get_id() = 0;	
	virtual void set_name( const wchar_t *fileName) = 0;	
	virtual const wchar_t *get_name() = 0;

	virtual void lock() = 0;
	virtual void un_lock() = 0;	
	virtual DWORD get_pos( int flag) = 0;	
	virtual void set_pos( DWORD pos) = 0;
	virtual float get_volume() = 0;
	virtual void set_volume( float volume) = 0;
	virtual float get_pan()	= 0;
	virtual void set_pan( float pan) = 0;	
	virtual BOOL play()	= 0;
	virtual int is_play() = 0;
	virtual int is_playing() = 0;
	virtual BOOL is_stopped() = 0;
	virtual void stop() = 0;
#if AUDIERE_RELEASE_P
	virtual void stop_and_wait() = 0;
#endif
	virtual void set_repeat() = 0;
	virtual void set_loop( BOOL bLoop) = 0;
	virtual BOOL is_loop() = 0;
	virtual BOOL is_seekable() = 0;
	virtual DWORD get_frequency() = 0;
	virtual DWORD get_length_in_ms() = 0;	
	virtual DWORD get_length() = 0;	
#if AUDIERE_BUILD_H
	virtual DWORD get_size() = 0;
#endif

#if SOUND_MAN_RELEASE_H
	virtual DWORD get_current_size() = 0;
#endif

	virtual DWORD get_buffer_size()	= 0;
	virtual LPSTR get_buffer( DWORD *size) = 0;
	virtual void set_pitch_shift( float shift) = 0;
	virtual float get_pitch_shift( float shift) = 0;

	virtual BOOL open( AudioDevicePtr device, File *resFile, const wchar_t *fileName) = 0;
	virtual BOOL open_async( AudioDevicePtr device, File *resFile, const wchar_t *fileName) = 0;	

// события
	virtual BOOL on_loaded( int iError) = 0;
	virtual BOOL on_play_starting() = 0;
	virtual BOOL on_play_started( int iError) = 0;
	virtual void on_stopped( StopEvent::Reason reason) = 0;

//
	virtual void set_loading( BOOL bLoading) = 0;
	virtual BOOL is_loading() = 0;	
	virtual BOOL is_loaded() = 0;

//
	virtual void close() = 0;
	virtual void clear() = 0;
	virtual void enabled() = 0;	
	virtual void disabled() = 0;	
	virtual BOOL is_enabled() = 0;
	virtual void params_saver() = 0;
	virtual BOOL apply_params() = 0;
	virtual BOOL is_params_saver() = 0;
	
	virtual void set_play( int iPlay) = 0;

	virtual void release() = 0;

	virtual ISndFileLoaderPlayer *get_snd_file_loader_player() = 0;

#if AUDIERE_BUILD_H
	virtual int update_file_info( int loaded_bytes, bool bLoaded) = 0;
#endif

#if AUDIERE_RELEASE_P
	virtual BOOL is_res_attached() = 0;
	virtual ifile *get_file() = 0;
	virtual void freeze() = 0;
	virtual void unfreeze() = 0;
	virtual BOOL is_freeze() = 0;
	virtual void prepare_for_delete() = 0;
	virtual BOOL is_prepare_for_delete() = 0;
#endif
};

//////////////////////////////////////////////////////////////////////////

#include <map>

template <class objectType>
class CObjectsMap
{
public: 
	CObjectsMap():
	MP_MAP_INIT(objMap)
	{};

	virtual ~CObjectsMap()
	{ remove_all_objects();};

public:
	// проверяет наличие идентификатора в списке
	virtual BOOL has_object( int ID)
	{ return objMap.find( ID) != objMap.end();};

	// вставить объект с идентификатором ID
	virtual  BOOL insert( int ID, objectType obj)
	{ objMap.insert( std::pair<int, objectType>(ID, obj)); return TRUE;};

	// возврашает объект по идентификатору
	virtual objectType get_obj_by_id( int ID)
	{ if ( has_object( ID)) return objMap[ ID]; return NULL;};

	// удалить объект с идентификатором
	virtual BOOL remove( int ID, BOOL bRemove = FALSE)
	{ if ( has_object( ID)) { objMap.erase( ID);} return FALSE;};

	// удалить все объекты
	virtual void remove_all_objects()
	{ objMap.clear();};

	// возвратить размер списка
	int get_size()
	{ return objMap.size();};

protected:
	typedef MP_MAP<int, objectType> objectMap;
	objectMap objMap;
};

