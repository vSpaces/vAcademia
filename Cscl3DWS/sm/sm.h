// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: sm.h,v $
// $Revision: 1.10 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2009/08/28 11:47:05 $ $Author: Vladimir $
//
//////////////////////////////////////////////////////////////////////

#ifndef __sm_h___
#define __sm_h___

#include "../rmml/rmml.h"
#include "../res/res.h"
//#include "../common/3DSoundFactor.h"
#include <string>
//#include <wtypes.h>

/*
// Для импорта/экспорта классов и процедур
#if defined (OMS_SM_BUILD)
#  define SM_DECL  __declspec(dllexport)
#else                 // defined (OMS_SM_BUILD)
# define SM_DECL   __declspec(dllimport)
#endif                // defined (OMS_SM_BUILD)

#ifndef WIN32
#define SM_NO_VTABLE
#else
#define SM_NO_VTABLE __declspec(novtable)
//#define SM_NO_VTABLE
#endif
*/

#define FACILITY_SOUNDMAN 0x9D0
#define SNDM_ERRBASE 0x1000
#ifndef MAKE_HRESULT
#define MAKE_HRESULT(sev,fac,code) ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#endif
#define MAKE_SNDMHRESULTSUCCESS(code)     MAKE_HRESULT(0, FACILITY_SOUNDMAN, (SNDM_ERRBASE + (code)))
#define MAKE_SNDMHRESULTERROR(code)       MAKE_HRESULT(1, FACILITY_SOUNDMAN, (SNDM_ERRBASE + (code)))
#define SNDM_E_RESMANNOTSET	MAKE_SNDMHRESULTERROR(0x2)
#define SNDM_E_COMMANNOTSET	MAKE_SNDMHRESULTERROR(0x3)

#ifndef _TCHAR_DEFINED
	#pragma warning( disable : 4267 )
	#include <cwchar>
	#include <memory>
	#include <string>
	#include <cassert>
	#define _ASSERT assert
	typedef unsigned long       DWORD;
	typedef int                 BOOL;
	typedef unsigned char       BYTE;
	typedef unsigned short      WORD;
	typedef float               FLOAT;
	typedef void	            *LPVOID;
	typedef int                 INT;
	typedef unsigned int        UINT;
	typedef const char *		LPCSTR;
	typedef char *				LPSTR;
	typedef long HRESULT;
	#define S_OK                                   ((HRESULT)0x00000000L)
	#define S_FALSE                                ((HRESULT)0x00000001L)
	#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
	#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)
	#define FALSE   0
	#define TRUE    1
	#define NULL    0
	#define _T(x)	L##x
#endif

//struct IUnknown;

namespace res{
	struct IResource;
	struct IResMan;
}

struct ISoundPtrBase;
struct ifile;
class C3DSoundFactor;

//using boost::shared_ptr;
/*using std::wstring;

namespace oms{
  class  omsContext;
}*/

namespace sm
{
	using namespace rmml;
	using namespace res;

	#define SND_NONE 0
	#define SND_BACK_MUSIC 1
	#define SND_POINT 2
	#define SND_SPLINE 4

	#define SOUNDS_DISABLED		0
	#define SOUNDS_ENABLED		1
	#define SOUNDS_PLAY			2
	#define SOUNDS_STOP			3

	#define WARNING_RES_IS_NOT_UPDATED			11
	#define WARNING_RES_IS_OPENING				12
	#define WARNING_RES_IS_NOT_NEED_TO_UPDATE	13

	#define ERROR_RES_IS_NOT_OPENED		1
	#define ERROR_RES_INVALID			2
	#define RMML_ERROR_FILE_INVALID			3
	#define ERROR_FILE_OPENING			4
	#define ERROR_FILE_STREAM_READ		5

	/**
	* Интерфейс уведомления о событиях: открытие, начало (окончания) проигрывание и так далее
	*/
	struct smISoundNotify
	{
		virtual void OnLoaded( int iError) = 0;	
		virtual void OnStopped( int iReason) = 0;
	};

	/**
	* Интерфейс, возвращаемый при создании менеджера звука
	*/
	struct smISoundManager
	{
		// создать медиа-объект исходя из типа RMML-элемента
		virtual oms::omsresult CreateMO( mlMedia* apMLMedia) = 0;

		//
		virtual oms::omsresult DestroyMO( mlMedia* apMLMedia) = 0;

		// установить режим менеджера звука 
		// (0 - звук выключен, 1 - звук включен)
		virtual oms::omsresult SetMode( int aiMode) = 0;

		// получить режим менеджера звука 
		virtual int GetMode() = 0;

		// установить уровень громкости звука
		virtual oms::omsresult SetVolume( double adVol) = 0;

		// получить уровень громкости звука
		virtual double GetVolume() = 0;

		//
		virtual omsresult setResManager( IResMan *aResMan) = 0;

		//
		virtual omsresult setCooperativeLevel( UINT hWnd, UINT level) = 0;

		//
		//virtual omsresult openSound( LPCSTR name, IResource* res, BOOL bImmediate) = 0;

		virtual omsresult OpenSoundFile( ifile *pfile, BOOL immediate, int *id, smISoundNotify *apISoundNotify = NULL, bool bAttached = FALSE ) = 0;
		//
		virtual omsresult OpenSoundRes( IResource* res, LPCWSTR name, BOOL immediate, int *id, smISoundNotify *apISoundNotify = NULL) = 0;

		//
		virtual omsresult playSound( int index, int num_person, BOOL loop) = 0;

		//
		virtual omsresult stopSound( int index) = 0;

		//
		virtual BOOL closeSound( int id) = 0;

		//
		virtual void removeSound( ISoundPtrBase *soundPtr) = 0;

		//
		virtual BOOL IsPlaying( int index) = 0;

		//
		virtual BOOL IsLoaded( int index) = 0;

		//
		virtual DWORD GetPosition( int index) = 0;

		//
		virtual omsresult GetPosition( int id, DWORD *time) = 0;

		//
		virtual DWORD GetFrequency( int index) = 0;

		//
		virtual omsresult GetFrequency( int index, DWORD *adwFrequency) = 0;

		// возвращает длительность в мс
		virtual DWORD GetDuration( int index) = 0;

		// возвращает длительность в мс
		virtual omsresult GetDuration( int index, DWORD *time) = 0;

		// возвращает размер файла в frames
		virtual DWORD GetLength( int index) = 0;

		// возвращает размер файла в frames
		virtual omsresult GetLength( int index, DWORD *length) = 0;

		// возвращает размер файла
		virtual DWORD GetSize( int index) = 0;

		// возвращает размер файла
		virtual omsresult GetSize( int index, DWORD *size) = 0;

		// возвращает текущий размер файла
		virtual DWORD GetCurrentSize( int index) = 0;

		// возвращает текущий размер файла
		virtual omsresult GetCurrentSize( int index, DWORD *size) = 0;

		virtual omsresult GetVolume( int id, float *volume) = 0;

		//
		virtual void SetVolume( float volume) = 0;

		//
		virtual BOOL SetPosition( int index, DWORD time) = 0;

		//
		virtual BOOL Set3DSoundCoef( int id, float coef, float left_coef, float right_coef) = 0;

		//
		virtual BOOL Set3DSoundParams( int id, float distX, float distY, float distZ, float dirX, float dirY, float dirZ,
										float vp_dirX, float vp_dirY, float vp_dirZ) = 0;

		//
		virtual BOOL Set3DSoundParams( int id, C3DSoundFactor *p3DSoundFactor) = 0;

		//
		virtual BOOL SetSoundType( int id, int type) = 0;

		//
		virtual BOOL SetVolume( int id,  float volume) = 0;

		//
		virtual BOOL SetPitchShift( int id,  float shift) = 0;

		//
		virtual BOOL UpdateAllSound( int act) = 0;

		//
		virtual omsresult GetLexema( int num_person, int *lexema) = 0;

		// обновить информацию о файле
		// id - идентификатор звука
		// loaded_bytes - кол-во байтов - текущий размер загружаемого файла
		// bLoaded - указывает: файл загружен
		virtual omsresult UpdateFileInfo( int id, int loaded_bytes, bool bLoaded) = 0;

		//virtual omsresult GetSoundOutputDevices( std::wstring& soundOutputDevices) = 0;

		virtual omsresult ReplaceDevice(std::wstring &sDeviceName) = 0;
	};	
}

#endif