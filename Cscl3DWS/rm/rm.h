// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: rm.h,v $
// $Revision: 1.49 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2010/04/14 09:51:45 $ $Author: tandy $
//
//////////////////////////////////////////////////////////////////////

#ifndef __rm_h___
#define __rm_h___

#include "../../nengine/interfaces/IDynamicTexture.h"
#include "../rmml/rmml.h"
#include "../rm/rm.h"
#include "../res/res.h"
#include "../rmml/rmmld.h"
#include "../sm/sm.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
//World Editor
class IWorldEditor;
class IMouseController;

typedef struct _SResourceConsumingApplication
{
	std::string name;
	float cpuPercent;
	__int64 memorySize;

	_SResourceConsumingApplication(std::string _name, float _cpuPercent, __int64 _memorySize)
	{
		name = _name;
		cpuPercent = _cpuPercent;
		memorySize = _memorySize;
	}

	_SResourceConsumingApplication(const _SResourceConsumingApplication& other)
	{
		name = other.name;
		cpuPercent = other.cpuPercent;
		memorySize = other.memorySize;
	}
} SResourceConsumingApplication;

//////////////////////////////////////////////////////////////////////////

// Для импорта/экспорта классов и процедур
#if defined (OMS_RM_BUILD)
#  define RM_DECL  __declspec(dllexport)
#else                 // defined (OMS_RM_BUILD)
# define RM_DECL   __declspec(dllimport)
#endif                // defined (OMS_RM_BUILD)

#ifndef WIN32
#define RM_NO_VTABLE
#else
#define RM_NO_VTABLE __declspec(novtable)
//#define RM_NO_VTABLE
#endif


//using boost::shared_ptr;
using std::wstring;
class IMath3DRMML;

namespace mme
{
  struct DeviceMode;
//  class RenderDevice;
//  class  mmeContext;
}

namespace oms{
  class  omsContext;
}

namespace desktop
{
	struct IDesktopSharingDestination;
}

namespace voip{
	struct IVoip3DSoundCalculator;
}

struct resourceLoadStruct;

namespace rm
{
	using namespace rmml;

/**
 * Интерфейс математических операций с 3D объектами
 */
struct  IMath3DRMML
{
	virtual void	quatMult(ml3DRotation &rot1, ml3DRotation &rot2, ml3DRotation &rotRes)=0;
	virtual void	quatBlend(ml3DRotation &rot1, ml3DRotation &rot2, double k, ml3DRotation &rotRes)=0;
	virtual void	vec3MultQuat(ml3DPosition &pos, ml3DRotation &rot, ml3DPosition &posRes)=0;
};

/*enum	rmRenderMode{
	rmrmNormal,
	rmrmDefault,
	rmrmProjection
};*/

/*enum	rmProjectionType{
	rmptDefault,
	rmptFront,
	rmptBack,
	rmptLeft,
	rmptRight,
	rmptTop,
	rmptBottom
};*/

enum rmDebugRenderItems{
	rmdriAABB,
	rmdriOOB,
	rmdriWorldBounds
};

struct RM_NO_VTABLE rmISoundManager
{
	virtual int		GetLexema( int aiPerson, int& aoiLexema) = 0;
	virtual int		OpenSound( ifile *pfile, sm::smISoundNotify *apISoundNotify = NULL, bool bAttached = FALSE) = 0;
	virtual int		OpenSound( const wchar_t* apwcURL, sm::smISoundNotify *apISoundNotify = NULL) = 0;
	virtual int 	UpdateFileInfo( int	aiSound, unsigned long adwLoadedBytes, bool bLoaded) = 0;
	virtual bool	CloseSound( int aiSound) = 0;
	virtual bool	PlaySound( int aiSound, int aiPerson, bool loop) = 0;
	virtual void	StopSound(int aiSound) = 0;
	virtual void	SoundOn( int id, char** ppGUID) = 0;
	virtual void	SoundOff() = 0;
	virtual bool	IsPlaying( int	aiSound) = 0;
	virtual bool	SetPosition( int	aiSound, unsigned long adwPos) = 0;
	virtual bool	GetPosition( int	aiSound, unsigned long &adwPos) = 0;
	virtual bool	GetFrequency(int aiSound, unsigned long &adwFrequency) = 0;
	virtual bool	GetDuration( int	aiSound, unsigned long &adwDur) = 0;
	virtual bool	GetLength( int	aiSound, unsigned long &adwLength) = 0;
	virtual bool	GetSize( int	aiSound, unsigned long &adwSize) = 0;
	virtual bool	GetCurrentSize( int	aiSound, unsigned long &adwSize) = 0;
	virtual bool	GetVolume( int id, float &fVolume) = 0;
	virtual bool	SetCooperativeLevel( unsigned long hWnd, unsigned long level) = 0;
	virtual bool	Set3DSoundParams( int id, rmml::ml3DPosition dist, rmml::ml3DPosition dir, rmml::ml3DPosition vp_dir) = 0;
	virtual bool	Set3DSoundParams( int id, float coef, float left_coef, float right_coef) = 0;
	virtual bool	SetVolume( int id, float volume) = 0;
	virtual bool	SetPitchShift( int id,  float shift) = 0;
};

struct RM_NO_VTABLE rmIRenderManagerAX;

#define ENGINE_WARNING_OPTIMUS_INTEL_USED		1
#define ENGINE_WARNING_BETA_DRIVER				2
#define ENGINE_WARNING_OLD_DRIVER				3		
#define ENGINE_WARNING_VENDOR_UNSUPPORTED		4	
#define ENGINE_WARNING_DRIVER_INVALID			5	
#define ENGINE_WARNING_DRIVERS_NOT_INSTALLED	6
#define ENGINE_WARNING_RAM_NOT_ENOUGH			7
#define ENGINE_WARNING_VIDEOCARD_TOO_WEAK		8
#define ENGINE_WARNING_CPU_FREQ_NOT_ENOUGH		9
#define ENGINE_WARNING_OS_OUTDATED				10
#define ENGINE_WARNING_VIDEOMEM_NOT_ENOUGH		11
#define ENGINE_WARNING_NEED_TO_ENLARGE_VIDEOMEM	12

#define VIDEO_CAPTURE_UNKNOWN_ERROR						0
#define VIDEO_CAPTURE_SETTINGS_ERROR					1
#define VIDEO_CAPTURE_VIDEO_FILTER_NOT_EXISTS_ERROR		2
#define VIDEO_CAPTURE_VIDEO_FILTER_NOT_REGISTERED_ERROR	3
#define VIDEO_CAPTURE_NO_EVENT_ID_ERROR					4
#define VIDEO_CAPTURE_FFMPEG_NOT_FOUND_ERROR			5
#define VIDEO_CAPTURE_FFMPEG_NOT_EXECUTED_ERROR			6
#define VIDEO_CAPTURE_AUDIO_FILTER_NOT_EXISTS_ERROR		7
#define VIDEO_CAPTURE_AUDIO_FILTER_NOT_REGISTERED_ERROR	8
#define VIDEO_CAPTURE_PERFORMANCE_IS_LOW				9

class IVideoCaptureSupport
{
public:
	virtual bool StartCapture(unsigned int eventID) = 0;
	virtual bool EndCapture() = 0;

	virtual bool GetPictureData(void** data, unsigned int& size, unsigned int& width, unsigned int& height) = 0;
	virtual void FreePictureData(void* data) = 0;

	virtual char* GetVideoURL() = 0;

	virtual bool CanVideoBeStarted() = 0;

	virtual void SetVideoSize(int width, int height) = 0;
};

class ICodeEditorSupport
{
public:
	virtual void Start(std::wstring code) = 0;
	virtual void SetNewProgramText() = 0;	
	virtual void Close() = 0;
};

/**
 * Интерфейс, возвращаемый при создании менеджера графики
 */
struct RM_NO_VTABLE rmIRenderManager
{
	// вернуть список ресурсоемких приложений
	virtual std::vector<SResourceConsumingApplication> GetResourceConsumingApplications() = 0;
	// вернуть ID предупреждения от графического движка
	virtual std::vector<unsigned int> GetEngineWarningIDs() = 0;
	// слишком много памяти занято
	virtual void OnMemoryOverflow() = 0;

	// уведомление о закрытии окна
	virtual void OnExit() = 0;
	virtual unsigned int GetCurrentFPS() = 0;
	virtual void* GetTaskManager() = 0;
	// создать медиа-объект исходя из типа RMML-элемента
	virtual bool ToogleRenderState(int stateID) = 0;
	virtual oms::omsresult CreateMO(mlMedia* apMLMedia)=0;
	virtual oms::omsresult DestroyMO(mlMedia* apMLMedia)=0;
	// получилось ли создать stencil-буфер при инициализации
	virtual bool StencilBufferExists()=0;
	// перемножает два 3D поворота
	virtual ml3DRotation multiplyRotations(ml3DRotation &rot1, ml3DRotation &rot2)=0;
	virtual IMath3DRMML*	 GetMath3D()=0;
	virtual oms::omsresult SaveMedia(const mlMedia* apMLMedia, const wchar_t* apwcPath)=0;
	// режимы рендеринга проекций
	//virtual bool SetRenderMode(rmRenderMode aeRenderMode)=0;
	//virtual bool SetProjectionScene(mlMedia* apMLScene)=0;
	//virtual mlMedia* GetProjectedVisibleAt(const int& iX, const int& iY)=0;
	//virtual oms::omsIAuthoring::omsIAuthoring3D*	GetIAuthoring3D() = 0;
	//virtual void SetIAuthoring(rmmld::mlIAuthoring* pAuth) = 0;
	// режими показа дебаговой информации
	virtual rm::rmIRenderManagerAX*	GetExtendedRM() = 0;
	virtual void ReloadUserSetting() = 0;
	virtual bool GetUserSetting( wchar_t* name, char* apwcValue, unsigned short aSizeValue) = 0;
	virtual int SetUserSetting( wchar_t* name, wchar_t* value) = 0;
	virtual void GetEncodingPassword(std::string& value) = 0;
	virtual void GetEncodingPassword(std::wstring& value) = 0;
	virtual void PrepareForDestroy() = 0;
	virtual ILogWriter* GetLogWriter() = 0;
	virtual void CreateMapManager()=0;
	virtual void DeleteDynamicTexture(IDynamicTexture* dynamicTexture) = 0;
	virtual void* GetCSForWindow(void* ahWnd) = 0; // HWND
	virtual IWorldEditor* GetWorldEditor() = 0;
	virtual IMouseController* GetMouseController() = 0;
	virtual void SetLogMode(bool isWorking) = 0;
	virtual void SetAudioMode(wchar_t* sType, int iMode) = 0;
	virtual void SetAudioVolume(wchar_t* sType, float fVolume) = 0;
	virtual bool FindFreePlace(ml3DPosition& pos, ml3DBounds& bounds, ml3DRotation& rot, int maxDist, ml3DPosition& posRes, const wchar_t* location) = 0;
	// возвращает строковый идентификатор локации, в которой находится 3D-объект
	virtual const char* GetObjectLocation(mlMedia* apMLMedia)=0;
	// возвращает строковый идентификатор локации общения (зоны общения), в которой находится 3D-объект
	virtual const char* GetObjectCommunicationArea(mlMedia* apMLMedia)=0;
	virtual voip::IVoip3DSoundCalculator*	GetI3DSoundCalculator() = 0;
	virtual desktop::IDesktopSharingDestination* GetDesktopSharingDestination(char* alpcTarget) = 0;
	// вызывается, когда rmml-элемент удаляется
	virtual void MLMediaDeleting(mlMedia* apMLMedia) = 0;
	// возвращаем версию ОС
	virtual void GetOSVersion(unsigned int* apuMajor,  unsigned int* apuMinor,  unsigned int* apuBuild) = 0;
	// уничтожить плагин, отвязанный ранее от mlMeida
	virtual oms::omsresult DestroyPlugin(moMedia* apPlugin)=0;
	virtual void EnableLastScreen(bool isEnabled) = 0;
	virtual void LoadingModeChanged(bool isEnabled) = 0;
	// Для отладки: включает/выключает режим показа всех скрытых объектов
	virtual void ShowHidden3DObjects(bool abShow) = 0;
	// код ошибки при отмены авторизации
	virtual void OnLogout( int iCode) = 0;
	// асинхронная загрузка motions завершилась?
	virtual bool IsMotionPreloadedEnd() = 0;
	// асинхронная загрузка blends завершилась?
	virtual bool IsBlendsPreloadedEnd() = 0;
	// вернуть максимальное количество семплов для MSAA
	virtual unsigned char GetMaxAntialiasingPower() = 0;
	// Заполняет координаты по загружаемым ресурсам
	virtual void GetLoadingResourcesCoords( resourceLoadStruct* apResLoadStruct, int aResCount, float* pAvatarX, float* pAvatarY, float* pAvatarZ) = 0;

	virtual void GetComputerInfo( std::wstring &compName, std::string &processName, unsigned int &processorCount, unsigned int &realCoreCount, bool &hyperthreadingEnabled, unsigned __int64 &physicalMemorySize, unsigned __int64 &virtualMemorySize, std::string &osVersion, unsigned char &osBits) = 0;
	virtual void GetGPUInfo( std::string &vendorName, std::string &deviceName, unsigned __int64 &videoMemorySize, int &major, int &minor) = 0;

	virtual void GetDriverLink(std::string& link, std::string& videoURL, unsigned char& linkType) = 0;
	// компьютер медленный?
	virtual bool IsComputerSlow() = 0;
	virtual void SetPlayingEnabled( int iType, bool bEnabled) = 0;
	virtual bool GetPlayingEnabled( int iType) = 0;
	virtual IVideoCaptureSupport* GetVideoCaptureSupport() = 0;	
	virtual ICodeEditorSupport* GetCodeEditorSupport() = 0;	

	virtual bool IsKnownProxyServer(std::string& userName, std::string& password, std::string& host, std::string& port) = 0;

	virtual bool IsCaveMode() = 0;
};



RM_DECL oms::omsresult CreateRenderManager(oms::omsContext* amcx);
RM_DECL void DestroyRenderManager(oms::omsContext* amcx);

}

#endif
