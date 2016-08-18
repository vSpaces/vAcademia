#pragma once

#include "selectors/IDesktopSelector.h"
#include "view/DrawWindow.h"
#include "SessionFactory.h"
#include "DestinationsFactory.h"
#include "ConnectionFactory.h"
#include "SessionFactory.h"
#include "SelectorFactory.h"
#include "sharingmousekeyboardhandler.h"
#include "stateCodes.h"
#include "HookMouse.h"
#include <map>

//////////////////////////////////////////////////////////////////////////
namespace oms
{
	class omsContext;
}

class CDesktopRectSelector;
class CSharingSessionView;
class CDrawWindow;
class CDevicesCtrl;

//////////////////////////////////////////////////////////////////////////
class DESKTOPMAN_API CSharingManager : public IDesktopSharingManager
						, public IDesktopSelectorEvents
						, public IPreviewWindowEvents
						, public IHookMouseEvents
						
{
public:
	bool IsNewCodec;

	CSharingManager(oms::omsContext* aContext);
	virtual ~CSharingManager();

	void SendDestinationRequest( LPCSTR alpcTarget);
	void RaiseSessionStartedEvent( LPCSTR alpcTarget, LPCSTR alpcSessionKey, LPCSTR alpcFileName);
	void RaisePreviewCancelEvent( LPCSTR alpcTarget);
	void RaisePreviewOKEvent( LPCSTR alpcTarget);
	void RaiseSharingEventWithCode(LPCSTR alpcTarget, int aEventCode);
	void RecordStartEvent( LPCSTR alpcTarget, LPCSTR alpcFileName);
	void RaiseViewSessionStartedEvent( LPCSTR alpcTarget, LPCSTR alpcSessionKey, bool canRemoteAccess);
	void SessionClosedEvent(LPCSTR alpcTarget);
	void TextureSizeChangedEvent(LPCSTR alpcTarget, int width, int height);
	void ViewSessionClosedEvent(LPCSTR alpcTarget);
	void WaitOwnerEvent(LPCSTR alpcTarget);
	void RemoteAccessModeChangedEvent( LPCSTR alpcTarget, bool canRemoteAccess);

	bool RecordPlaying()
	{
		return m_playing;
	}
	LPCSTR GetTargetGroupNameInRecord()
	{
		if (!potentialTargetGroupNameInRecord.IsEmpty())
			return potentialTargetGroupNameInRecord.GetBuffer();
		else
			return NULL;
	}
	bool StartRecording(unsigned int aRecordID, LPCSTR alpTarget, LPCSTR alpcLocation);
	bool StopRecordingInFileSession();
	void ResumeServerSharingInternal();
	CComString GetNameInZeroReality(LPCSTR alpTarget);
	CComString ConvertNameToRecordReality( LPCSTR alpTarget, unsigned int auBornRealityID);
	CComString ConvertFromUnicName( LPCSTR alpTarget);
	CComString ConvertToUnicName( LPCSTR alpTarget, unsigned int auBornRealityID);
	CComString ConvertToUnicName( LPCSTR alpTarget);
	unsigned int GetRealityIDFromUnicName( LPCSTR alpTarget);
	unsigned int GetObjectRealityIDByTarget( LPCSTR alpTarget);
	CComString GetNameCycleSession( LPCSTR alpTarget);
	void SendNotificationToServerForFileSession(unsigned int idMessage);
	void AddVASEPack( void* pack, bool abKeyFrame, CSharingViewSession* viewSession);

	// реализация IDesktopSharingManager
public:
	//обновить имя файла в сессии
	void UpdateFileName( LPCSTR alpTarget, LPCSTR alpFileName);
	//получать все кадры
	void GetAllFrames( LPCSTR alpSessionName);
	//получать только ключевые кадры
	void GetOnlyKeyFrame( LPCSTR alpSessionName);
	//! Удаляет объект
	void Release();
	//! Начинает Sharing области
	bool StartCapturingRectRequest( LPCSTR alpTarget, LPCSTR alpGroupName);
	//! Начинает Sharing окна
	bool StartCapturingWndRequest( LPCSTR alpTarget, LPCSTR alpGroupName);

	bool StartCapturingNotepadWndRequest( LPCSTR alpTarget, LPCSTR alpGroupName);

	bool StartCapturingWnd( LPCSTR alpTarget, LPCSTR alpGroupName, bool NPad);
	//! Начинает Sharing c вебкамеры
	bool StartCapturingVideoInputRequest( LPCSTR alpTarget, LPCSTR alpGroupName);
	// Начинает Sharing с интерактивной доски
	bool StartCapturingInteractiveBoard( LPCSTR alpTarget, LPCSTR alpGroupName);
	//! Начинает Sharing видеофайла
	bool StartCapturingVideoFileRequest( LPCSTR alpTarget, LPCSTR alpGroupName, LPCWSTR alwFileName);
	//! Останавливает Sharing
	void StopCapturing( LPCSTR alpTarget);
	void StopCapturingAll();
	
	//Показывает область захвата по имени сессии
	void ShowSharingSessionRectangle( LPCSTR alpSessionName);
	void HideSharingSessionRectangle( LPCSTR alpSessionName);
	
	//! Начинает просмотр Sharing сессии
	virtual	bool StartSharingView( LPCSTR alpTarget, LPCSTR alpGroupName, LPCSTR alpFileName);
	//! Начинает просмотр Sharing сессии
	virtual	bool SetSharingViewFps( LPCSTR alpTarget, LPCSTR alpGroupName, unsigned int fps);
	//! Заканчивает шаринг или просмотр шаринга
	virtual	bool StopSharing( LPCSTR alpTarget);
	//! Устанавливает требуемые параметры подключения к определенной комнате
	virtual bool	SetSharingServerAddress(LPCSTR alpcGroupName
		, LPCSTR alpcServerAddress, unsigned int aiRecordPort);
	//!
	CSharingSessionFactory& GetSessionsFactory()
	{
		return sessionsFactory;
	}
	void	NotifyOnWaitOwnerEventInternal( LPCSTR alpcTarget);
	//
	void	SetDestinationPointer( LPCSTR alpcTarget, IDesktopSharingDestination* apIDesktopSharingDestination);
	//! Добавляет обработчик нотификационных событий
	bool AddCallback( desktopIDesktopManCallbacks* apCallback);
	//! Удаляет обработчик нотификационных событий
	bool RemoveCallback( desktopIDesktopManCallbacks* apCallback);
	bool Update();

	//! Начинает запись шаринга для текущей группы
	virtual bool StartRecording(unsigned int aRecordID, LPCSTR alpcLocation);
	
	//! Входит в режим просмотра записей
	virtual bool StartListenRecording( unsigned int aRecordID, LPCSTR alpcLocation);

	//! Заканчивает запись шаринга для текущей группы
	virtual bool StopRecording(unsigned int aRecordID);

	//! Выходит из режима просмотра записей
	virtual bool StopListenRecording( unsigned int aRecordID);

	//! Начинает воспроизведение указанной записи для указанной реальности
	virtual bool StartPlayingSharing(unsigned int aRecordID, unsigned int aRealityID);
	//! Заканчивает воспроизведение определенной записи
	virtual bool StopPlayingSharing(); 

	//! Начинает просмотр сессии шаринга
	virtual	bool StartSharingPlay( LPCSTR alpTarget, LPCSTR alpGroupName, LPCSTR alpFileName, bool isPaused, unsigned int aRecordID, unsigned int aShiftTime, unsigned int aServerVersion);
	//! Приостанавливает воспроизведение определенной записи для всех сессий, участвующих в записи
	virtual void PauseServerSharing();

	//! Продолжает воспроизведение определенной записи, участвующих в записи
	virtual void ResumeServerSharing();

	//! Перематывает воспроизведение определенной записи, участвующих в записи
	virtual void RewindServerSharing();

	//! Посылает серверу команду на приостановку воспроизведения определенной сесси
	virtual bool PauseUserSharing( LPCSTR alpSessionName);
	//! Посылает серверу команду на продолжение воспроизведения определенной сесси
	virtual bool ResumeUserSharing( LPCSTR alpSessionName);
	//! удалить запись (отсылает запрос на сервер)
	virtual bool DeleteRecord(unsigned int aRecordID);
	//
	virtual	bool SetBoardOwnerChanged( LPCSTR alpTarget, LPCSTR alpGroupName, unsigned int auState);
	
	virtual void AllowRemoteControl( LPCSTR alpSessionName);
	virtual void DenyRemoteControl( LPCSTR alpSessionName);
	virtual void* GetIMouseKeyboardHandler();
	virtual void TraceAllSession();
	virtual void NotifyVideoDeviceInitialized(int resultCode);
	virtual void StopWaitWebCameraResponce(LPCSTR alpTarget);
	virtual void SetCursorMode(LPCSTR alpTarget, int mode);
	virtual void FullControlChanged(LPCSTR alpTarget, int mode);

	virtual std::string GetAllViewSession();
	virtual std::string GetAllSharingSession();
	virtual std::string GetSessionInfo(LPCSTR alpTarget);
	virtual void ShowLastFrame(LPCSTR alpTarget);

	bool SetupInteractiveBoardSettings();
	bool EmulateRectRequest( LPCSTR alpTarget, LPCSTR alpGroupName);
	
	bool Seek( LPCSTR alpTarget, double aSecond);
	bool Play( LPCSTR alpTarget);
	bool Pause( LPCSTR alpTarget);
	double GetDuration( LPCSTR alpTarget);

	// реализация IDesktopSelectorEvents
public:
	void OnSelectionCompleted(int resultCode);
	void OnSelectionEscape();
	void OnSetBarPosition( LPCSTR aTarget, std::vector<double> v1);
	void OnVideoEnd( LPCSTR aTarget);

	// реализация IPreviewWindowEvents
public:
	void OnPreviewWindowOK(int aScaleMode);
	void OnPreviewWindowCancel();	
	// IHookMouseEvents
public:
	void OnGlobalMouseEvent(MouseEvent aMouseEvent);

public:
	void OnSetResolutionCompleted(int resultCode);

private:
	bool	ShowPreviewWindow(int resultCode);
	void	SetRecordState(unsigned int aRecordID, LPCSTR alpcLocation, bool aRecording, bool anAuthor);
	void	FreeCaptureBoardByPreview(LPCSTR alpNewTarget);
	void	CreateAndStartCapturingSession(int aQuality);
	void	BeforeStartNewSelector(LPCSTR alpTarget, LPCSTR alpGroupName);
	int		GetInteractiveBoardMonitorID();
	CSharingSession* GetInteractiveBoardSession();
	void	SendHookMouseEvent();

	IDesktopSelector *GetSelector( LPCSTR alpTarget);

private:
	bool m_playing;
	bool m_recording;
	bool m_enteredInRecord;
	bool m_isRecordAuthor;
	unsigned int m_recordID;
	CComString	m_recordLocation;
	unsigned int m_playOnSharingRecordID;
	unsigned int m_playOnSharingRealityID;
	SHARING_CONTEXT				context;

	std::map<CComString, unsigned int>	m_mapPlayRecIDByLocation;

	// Desktop selectors
	CComString					potentialTarget;
	CComString					potentialTargetGroupName;
	CComString					potentialTargetGroupNameInRecord;
	IDesktopSelector*			currentSelector;
	IWebCameraSelector*			webCameraSelector;

	// PreviewWindow
	CDrawWindow*				previewWindow;

	CSharingSessionFactory		sessionsFactory;
	CSharingConnectionFactory	connectionsFactory;
	CDestinationsFactory		destinationsFactory;
	SelectorFactory				selectorFactory;

	CSharingMouseKeyboardHandler* sharingMouseKeyboardHandler;

	CDevicesCtrl*				m_deviceController;

	//! Callbacks
	typedef	MP_VECTOR<desktopIDesktopManCallbacks*>	VecCallbacks;
	VecCallbacks	vecCallbacks;

	CCriticalSection csVecCallbacks;

	CHookMouse* m_hookMouse;
	MP_VECTOR<MouseEvent> m_hookMouseEvents;
};