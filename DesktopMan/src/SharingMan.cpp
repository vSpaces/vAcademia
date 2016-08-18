#include "StdAfx.h"
#include "..\include\SharingMan.h"
#include "..\include\SharingViewSession.h"
#include "..\include\Selectors\DesktopRectSelector.h"
#include "..\include\Selectors\WndSelector.h"
#include "..\include\Selectors\VideoInputSelector.h"
#include "..\include\Selectors\VideoFileSelector.h"
#include "..\include\SessionFactory.h"
#include "..\include\view\DrawWindow.h"
#include "../protocol/RecordOut.h"
#include "../protocol/RemoteDesktopOut.h"
#include "./protocol/StartSendFrameOut.h"
#include "./protocol/deleterecord.h"
#include "..\include\SharingSession.h"
#include "../WebCamGrabber/DevicesCtrl.h"
#include "rm\rm.h"
#include <algorithm>

#include "../include/MemLeaks.h"

#define SelectBitmap(hdc, hbm)  ((HBITMAP)SelectObject((hdc), (HGDIOBJ)(HBITMAP)(hbm)))

CSharingManager::CSharingManager(oms::omsContext* aContext) 
		: sessionsFactory( &context)
		, connectionsFactory( &context)
		, destinationsFactory( &context)
		, MP_VECTOR_INIT(vecCallbacks)
		, MP_VECTOR_INIT(m_hookMouseEvents)
{
	previewWindow = NULL;
	currentSelector = NULL;
	webCameraSelector = NULL;
	context.gcontext = aContext;
	context.sharingMan = this;
	context.sessionsFactory = &sessionsFactory;
	context.connectionsFactory = &connectionsFactory;
	context.destinationsFactory = &destinationsFactory;
	csVecCallbacks.Init();
	m_recording = false;
	m_playing = false;
	IsNewCodec = false;
	m_enteredInRecord = false;
	MP_NEW_V( sharingMouseKeyboardHandler, CSharingMouseKeyboardHandler, &sessionsFactory);
	potentialTargetGroupNameInRecord = "";
	m_playOnSharingRealityID = 0;
	m_playOnSharingRecordID = 0;
	m_recordLocation = "";
	m_isRecordAuthor = false;
	m_recordID = 0;
	m_mapPlayRecIDByLocation.clear();

	m_deviceController = MP_NEW( CDevicesCtrl);

	m_hookMouse = CHookMouse::Instance();
	m_hookMouse->Subscribe(this);
}

CSharingManager::~CSharingManager()
{
	SAFE_RELEASE(currentSelector);
	SAFE_RELEASE( previewWindow);
	sessionsFactory.DestroyAllSessions();
	connectionsFactory.DestroyAllConnections();
	selectorFactory.DestroyDeletedSelectors();
	csVecCallbacks.Term();
	m_hookMouse->Unsubscribe(this);
	m_hookMouse->FreeInst();
}

//! Удаляет объект
void CSharingManager::Release()
{
	MP_DELETE_THIS;
}

void CSharingManager::FreeCaptureBoardByPreview(LPCSTR alpNewTarget)
{
	if (previewWindow) {
		if (previewWindow->IsWindowVisible()){
			//окно предпросмотра сейчас видимо и сменлась доска для которой показываем предпросмотр
			// то освободим захват предыдущей доски
			if (CComString(alpNewTarget) != potentialTarget) {
				OnPreviewWindowCancel();
			}
		}
	}
}

int CSharingManager::GetInteractiveBoardMonitorID()
{
	int monitorID = -1;
	int size = 10;
	char* sMonitorNum = MP_NEW_ARR(char, size);
	//safe for reload settings - call in main thread 
	bool res = context.gcontext->mpRM->GetUserSetting(L"monitor_for_interactive_board", sMonitorNum, size);
	if (res)
	{
		monitorID = rtl_atoi(sMonitorNum);
	}
	if (context.gcontext->mpLogWriter)
	{
		std::string strLog = std::string("[SharingMAN]GetInteractiveBoardMonitorID ") + std::string(sMonitorNum);
		context.gcontext->mpLogWriter->WriteLnLPCSTR(strLog.c_str());
	}

	MP_DELETE_ARR(sMonitorNum);

	return monitorID-1;
}

// Начинает Sharing с интерактивной доски
bool CSharingManager::StartCapturingInteractiveBoard( LPCSTR alpTarget, LPCSTR alpGroupName)
{
	BeforeStartNewSelector(alpTarget, alpGroupName);

	int monitorID = GetInteractiveBoardMonitorID();

	MP_NEW_P( rectSelector, CDesktopRectSelector, context.gcontext);
	rectSelector->SetInteractiveBoardCapture(monitorID);
	rectSelector->Subscribe( this);
	rectSelector->StartSourceSelection();
	currentSelector = rectSelector;

	m_hookMouse->StartHookMouse();

	CreateAndStartCapturingSession(45);
	return false;
}

void CSharingManager::BeforeStartNewSelector(LPCSTR alpTarget, LPCSTR alpGroupName)
{
	//почистим селекторы устройств
	selectorFactory.DestroyDeletedSelectors();
	// почистим объекты-cессии, которые можно
	sessionsFactory.DestroyDeletedSessions();

	if( currentSelector)
		currentSelector->CancelSourceSelection();
	if (currentSelector != NULL)
	{
		if (!currentSelector->Release()) // веб-камера еще деинициализируется
			selectorFactory.AddSelector(currentSelector);
		currentSelector = NULL;
	}
	webCameraSelector = NULL;
	FreeCaptureBoardByPreview(alpTarget);

	potentialTarget = alpTarget;
	potentialTargetGroupName = alpGroupName;
}

//! Начинает Sharing области
bool CSharingManager::StartCapturingRectRequest( LPCSTR alpTarget, LPCSTR alpGroupName)
{
	BeforeStartNewSelector(alpTarget, alpGroupName);

	MP_NEW_P( rectSelector, CDesktopRectSelector, context.gcontext);
	
	rectSelector->Subscribe( this);
	rectSelector->StartSourceSelection();
	currentSelector = rectSelector;
	return false;
}

bool CSharingManager::EmulateRectRequest( LPCSTR alpTarget, LPCSTR alpGroupName)
{
	BeforeStartNewSelector(alpTarget, alpGroupName);

	MP_NEW_P( rectSelector, CDesktopRectSelector, context.gcontext);

	rectSelector->Subscribe( this);
	currentSelector = rectSelector;
	currentSelector->EmulateSelection( 100, 500, 100, 400);
	
	IsNewCodec = true;

	// Create server connection
	CSharingConnection*		connection = connectionsFactory.GetConnectionByLocationName( potentialTargetGroupName);
	ATLASSERT( connection);
	connection->SetMessageHandler( &sessionsFactory);

	unsigned int quality = 50;

	if( sessionsFactory.StartCapturingSession( ConvertToUnicName(potentialTarget), currentSelector, connection, quality, false))
	{
		currentSelector = NULL;
		webCameraSelector = NULL;
	}

	return true;
}

//! Устанавливает Fps Sharing сессии
bool CSharingManager::SetSharingViewFps( LPCSTR alpTarget, LPCSTR alpGroupName, unsigned int auFps)
{
	ATLASSERT( alpTarget);
	ATLASSERT( alpGroupName);

	if( !alpTarget)
		return false;
	if( !alpGroupName)
		return false;

	sessionsFactory.SetSharingViewFps(ConvertToUnicName(alpTarget), auFps);

	return true;
}

bool CSharingManager::SetBoardOwnerChanged( LPCSTR alpTarget, LPCSTR alpGroupName, unsigned int auState)
{
	if( !alpTarget)
		return false;
	if( !alpGroupName)
		return false;

	if (previewWindow) {
		if (previewWindow->IsWindowVisible()){
			// если показывается окошко предпросмотра шаринга и меняется владелец доски
			if (auState == BOARD_NOT_MY_OWN) {
				OnPreviewWindowCancel();
				return true;
			}
		}
	}

	return false;
}

//! Начинает просмотр Sharing сессии
bool CSharingManager::StartSharingView( LPCSTR alpTarget, LPCSTR alpGroupName, LPCSTR alpFileName)
{
	ATLASSERT( alpTarget);
	ATLASSERT( alpGroupName);
	ATLASSERT( alpFileName);

	if( !alpTarget)
		return false;
	if( !alpGroupName)
		return false;
	if( !alpFileName)
		return false;

	// Create server connection
	CSharingConnection*		connection = connectionsFactory.GetConnectionByLocationName( alpGroupName);
	ATLASSERT( connection);
	connection->SetMessageHandler( &sessionsFactory);

	sessionsFactory.StartViewSession( ConvertToUnicName(alpTarget), alpFileName, connection);

	return true;

}
//! Заканчивает шаринг или просмотр шаринга
bool CSharingManager::StopSharing( LPCSTR alpTarget)
{
	ATLASSERT( alpTarget);

	if( !alpTarget)
		return false;

	CComString baseSessionName;
	baseSessionName = GetNameInZeroReality(alpTarget);
	if (context.gcontext->mpLogWriter)
		context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]delete VS on  user StopSharing");
	if (!baseSessionName.IsEmpty())
	{
		sessionsFactory.DestroyViewSession( baseSessionName, true);
		sessionsFactory.DestroyCapturingSession( baseSessionName, true);
	}

	// Destroy view session
	sessionsFactory.DestroyViewSession( ConvertToUnicName(alpTarget), true);

	// Destroy sharing session
	sessionsFactory.DestroyCapturingSession( ConvertToUnicName(alpTarget), true);	
	return true;
}
void CSharingManager::StopWaitWebCameraResponce(LPCSTR alpTarget)
{
	if (NULL != currentSelector && !currentSelector->CanShowCaptureRect())
		currentSelector->CancelSourceSelection();
}

void CSharingManager::OnSetResolutionCompleted(int resultCode)
{
	if (previewWindow)
	{
		previewWindow->OnSetResolutionCompleted(currentSelector, resultCode);
	}
}
//! Начинает Sharing c вебкамеры
bool CSharingManager::StartCapturingVideoInputRequest( LPCSTR alpTarget, LPCSTR alpGroupName)
{
//	if( currentSelector)
//		currentSelector->CancelSourceSelection();
	
// идет инициализация устройства, игнорируем запрос
	if (webCameraSelector != NULL)
	{
		OnSelectionCompleted(WEBCAMERA_NOTHING);
		potentialTarget = alpTarget;
		potentialTargetGroupName = alpGroupName;
		return false;
	}
	//почистим селекторы устройств
	selectorFactory.DestroyDeletedSelectors();
	// почистим объекты-cессии, которые можно
	sessionsFactory.DestroyDeletedSessions();
	FreeCaptureBoardByPreview(alpTarget);

	potentialTarget = alpTarget;
	potentialTargetGroupName = alpGroupName;

	if (currentSelector != NULL)
	{
		if (!currentSelector->Release()) // веб-камера еще деинициализируется
			selectorFactory.AddSelector(currentSelector);
		currentSelector = NULL;
	}

	MP_NEW_P2( videoSelector, CVideoInputSelector, context.gcontext, m_deviceController);
	videoSelector->CEventSource<IDesktopSelectorEvents>::Subscribe( this);
	currentSelector = videoSelector;
	if (previewWindow)
		videoSelector->SetResolution(previewWindow->m_lastResolution, 0, false);
	videoSelector->StartSourceSelection();
	
	webCameraSelector = videoSelector;

	return false;
}

//! Начинает Sharing видео файла
bool CSharingManager::StartCapturingVideoFileRequest( LPCSTR alpTarget, LPCSTR alpGroupName, LPCWSTR alwFileName)
{	
	BeforeStartNewSelector(alpTarget, alpGroupName);
	MP_NEW_P2( videoFileSelector, CVideoFileSelector, context.gcontext, alwFileName);
	videoFileSelector->CEventSource<IDesktopSelectorEvents>::Subscribe( this);
	currentSelector = videoFileSelector;
	videoFileSelector->SetBoardName( ConvertFromUnicName(alpTarget));
	videoFileSelector->StartSourceSelection();
	return false;
}

IDesktopSelector *CSharingManager::GetSelector( LPCSTR alpTarget)
{
	CSharingSession* session = sessionsFactory.GetSharingSessionByName( ConvertToUnicName(alpTarget));
	if( session == NULL)
		return false;
	return session->GetSelector();
}

bool CSharingManager::Seek( LPCSTR alpTarget, double aSecond)
{	
	ATLASSERT( alpTarget);
	context.gcontext->mpLogWriter->WriteLnLPCSTR( "CSharingManager::Seek");
	if( !alpTarget)
	{
		context.gcontext->mpLogWriter->WriteLnLPCSTR( "CSharingManager::Pause: alpTarget==NULL");
		return false;
	}

	IDesktopSelector *pSelector = GetSelector( alpTarget);
	if( pSelector == NULL)
	{
		CComString s = "CSharingManager::Seek alpTarget=="; s += alpTarget;	
		context.gcontext->mpLogWriter->WriteLnLPCSTR( s.GetBuffer());
		return false;
	}
	pSelector->Seek( aSecond);
	return true;
}

bool CSharingManager::Play( LPCSTR alpTarget)
{	
	ATLASSERT( alpTarget);
	context.gcontext->mpLogWriter->WriteLnLPCSTR( "CSharingManager::Play");
	if( !alpTarget)
	{
		context.gcontext->mpLogWriter->WriteLnLPCSTR( "CSharingManager::Pause: alpTarget==NULL");
		return false;
	}
	IDesktopSelector *pSelector = GetSelector( alpTarget);
	if( pSelector == NULL)
	{
		CComString s = "CSharingManager::Play alpTarget=="; s += alpTarget;	
		context.gcontext->mpLogWriter->WriteLnLPCSTR( s.GetBuffer());
		return false;
	}
	pSelector->Play();
	return true;
}

bool CSharingManager::Pause( LPCSTR alpTarget)
{	
	ATLASSERT( alpTarget);
	context.gcontext->mpLogWriter->WriteLnLPCSTR( "CSharingManager::Pause");
	if( !alpTarget)
	{
		context.gcontext->mpLogWriter->WriteLnLPCSTR( "CSharingManager::Pause: alpTarget==NULL");
		return false;
	}
	IDesktopSelector *pSelector = GetSelector( alpTarget);
	if( pSelector == NULL)
	{
		CComString s = "CSharingManager::Pause alpTarget=="; s += alpTarget;
		context.gcontext->mpLogWriter->WriteLnLPCSTR( s.GetBuffer());
		return false;
	}
	pSelector->Pause();
	return true;
}

double CSharingManager::GetDuration( LPCSTR alpTarget)
{	
	ATLASSERT( alpTarget);

	if( !alpTarget)
		return 0;
	IDesktopSelector *pSelector = GetSelector( alpTarget);
	CComString s = "CSharingManager::GetDuration: alpTarget=="; s += alpTarget;
	context.gcontext->mpLogWriter->WriteLnLPCSTR( s.GetBuffer());
	if( pSelector == NULL || pSelector->GetType() != VIDEOFILE_SELECTOR)
		return 0;
	s = "CSharingManager::GetDuration video alpTarget=="; s += alpTarget;
	context.gcontext->mpLogWriter->WriteLnLPCSTR( s.GetBuffer());
	return pSelector->GetDuration();
}

bool CSharingManager::StartCapturingWndRequest( LPCSTR alpTarget, LPCSTR alpGroupName)
{
	return StartCapturingWnd( alpTarget, alpGroupName, false);
}
bool CSharingManager::StartCapturingNotepadWndRequest( LPCSTR alpTarget, LPCSTR alpGroupName)
{
	return StartCapturingWnd( alpTarget, alpGroupName, true);
}

//! Начинает Sharing окна
bool CSharingManager::StartCapturingWnd( LPCSTR alpTarget, LPCSTR alpGroupName, bool NPad)
{
	BeforeStartNewSelector(alpTarget, alpGroupName);
    
	MP_NEW_P2( wndSelector, CWndSelector, context.gcontext, NPad);
	wndSelector->SetMainWindowHandle( (HWND)(context.gcontext->mpWindow->GetMainWindowHandle()));
	wndSelector->Subscribe( this);
	wndSelector->StartSourceSelection();//
	currentSelector = wndSelector;

	return false;
}

//! Останавливает Sharing
void CSharingManager::StopCapturing( LPCSTR alpTarget)
{
//	sessionsFactory.DestroyViewSession( alpTarget, true);
}

void CSharingManager::StopCapturingAll()
{
	sessionsFactory.DestroyAllSessions();
}

void CSharingManager::OnSelectionCompleted(int resultCode)
{
	//resultCode == 1 - успешное завершение
	//resultCode == 0 - шаринг области или окна
	//resultCode < 0 - не сообщать в скрипт
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onVideoInputDeviceInitializedEvent(potentialTarget, resultCode);
	}
	csVecCallbacks.Unlock();
	if (resultCode > 1 && resultCode != WEBCAMERA_NOTHING)
		webCameraSelector = NULL;
	if (resultCode > 0 && resultCode != SUCCESSFULL && resultCode != USER_CHOSE_WEBCAMERA)
		RaisePreviewCancelEvent( potentialTarget);
}

void	CSharingManager::OnSelectionEscape()
{
	RaisePreviewCancelEvent( potentialTarget);
}

void CSharingManager::OnSetBarPosition( LPCSTR aTarget, std::vector<double> v1)
{
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		double second = (double)v1.at(0);
		double duration = (double)v1.at(1);
		(*it)->onSetBarPositionEvent( aTarget, second, duration);
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::OnVideoEnd( LPCSTR aTarget)
{
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onVideoEndEvent( aTarget);
	}
	csVecCallbacks.Unlock();
}

bool	CSharingManager::SetSharingServerAddress(LPCSTR alpcGroupName
										, LPCSTR alpcServerAddress, unsigned int aiRecordPort)
{
	return connectionsFactory.SetSharingServerAddress(alpcGroupName, alpcServerAddress, aiRecordPort);
}

bool	CSharingManager::ShowPreviewWindow(int resultCode)
{
	if( !currentSelector)
		return false;

	RECT rcCapture = currentSelector->GetCurrentSelectingRect();
	if (resultCode != USER_CHOSE_WEBCAMERA)
	{
		if( rcCapture.right<=rcCapture.left)
			return false;
		if( rcCapture.bottom<=rcCapture.top)
			return false;
	}

	if( !previewWindow)
	{
		CDialogTemplate::SetResourceInstance( (HINSTANCE)_Module.m_hInstResource);
		MP_NEW_V( previewWindow, CDrawWindow, context.gcontext);
		previewWindow->Subscribe(this);
	}
	return previewWindow->ShowPreview( currentSelector, webCameraSelector);
}

void	CSharingManager::OnPreviewWindowCancel()
{
	if( currentSelector)
		currentSelector->CancelSourceSelection();
	if (currentSelector != NULL)
	{
		if (!currentSelector->Release()) // веб-камера еще деинициализируется
			selectorFactory.AddSelector(currentSelector);
		currentSelector = NULL;
	}
	webCameraSelector = NULL;
	RaisePreviewCancelEvent( potentialTarget);
	previewWindow->ShowWindow( SW_HIDE);
}

void	CSharingManager::OnPreviewWindowOK(int aScaleMode)
{
	previewWindow->ShowWindow( SW_HIDE);

	unsigned int quality = previewWindow->GetQuality();
	CreateAndStartCapturingSession(quality);
}

void	CSharingManager::CreateAndStartCapturingSession(int aQuality)
{
	if (previewWindow)
		IsNewCodec = previewWindow->useNewCodec;
	else
		IsNewCodec = true;

	// реально сессию создаем тут
	if( !currentSelector)
		return;

	// Create server connection
	CSharingConnection*		connection = connectionsFactory.GetConnectionByLocationName( potentialTargetGroupName);
	ATLASSERT( connection);
	connection->SetMessageHandler( &sessionsFactory);

	RaisePreviewOKEvent( potentialTarget);

	if( sessionsFactory.StartCapturingSession( ConvertToUnicName(potentialTarget), currentSelector, connection, aQuality, false))
	{
		currentSelector = NULL;
		webCameraSelector = NULL;
	}

}

void	CSharingManager::SetDestinationPointer( LPCSTR alpcTarget, IDesktopSharingDestination* apIDesktopSharingDestination)
{
	sessionsFactory.SetViewSessionDestination( ConvertToUnicName(alpcTarget), apIDesktopSharingDestination);
}

//! Добавляет обработчик нотификационных событий
bool CSharingManager::AddCallback( desktopIDesktopManCallbacks* apCallback)
{
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = std::find( vecCallbacks.begin(), vecCallbacks.end(), apCallback);
	if( it == vecCallbacks.end())
	{
		vecCallbacks.push_back( apCallback);
	}
	csVecCallbacks.Unlock();
	return true;
}

bool CSharingManager::Update()
{
	//почистим селекторы устройств
	selectorFactory.DestroyDeletedSelectors();
	// почистим объекты-cессии, которые можно
	sessionsFactory.DestroyDeletedSessions();

	SendHookMouseEvent();

	return true;
}

//! Удаляет обработчик нотификационных событий
bool CSharingManager::RemoveCallback( desktopIDesktopManCallbacks* apCallback)
{
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = find( vecCallbacks.begin(), vecCallbacks.end(), apCallback);
	if( it != vecCallbacks.end())
	{		
		vecCallbacks.erase( it);
		csVecCallbacks.Unlock();
		return true;
	}
	csVecCallbacks.Unlock();
	return false;
}

void CSharingManager::SendDestinationRequest( LPCSTR alpcTarget)
{
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onDesktopDestinationRequest( ConvertFromUnicName(alpcTarget));
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::RaiseSessionStartedEvent( LPCSTR alpcTarget, LPCSTR alpcSessionKey, LPCSTR alpcFileName)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	ATLASSERT( alpcSessionKey);
	ATLASSERT( *alpcSessionKey);

	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onRaiseSessionStartedEvent( ConvertFromUnicName(alpcTarget), alpcSessionKey, alpcFileName);
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::RaisePreviewCancelEvent( LPCSTR alpcTarget)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);	

	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onRaisePreviewCancelEvent( ConvertFromUnicName(alpcTarget));
	}
	csVecCallbacks.Unlock();
}


void CSharingManager::RaisePreviewOKEvent( LPCSTR alpcTarget)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);

	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onRaisePreviewOKEvent( ConvertFromUnicName(alpcTarget));
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::RecordStartEvent(LPCSTR alpcTarget, LPCSTR alpcFileName)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	ATLASSERT( alpcFileName);
	ATLASSERT( *alpcFileName);
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for (; it!=end; ++it)
	{
		(*it)->onSharingServerFileSaved( ConvertFromUnicName(alpcTarget), alpcFileName);
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::RaiseViewSessionStartedEvent(LPCSTR alpcTarget, LPCSTR alpcSessionKey, bool canRemoteAccess)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	ATLASSERT( alpcSessionKey);
	ATLASSERT( *alpcSessionKey);

	if (m_recording && m_isRecordAuthor)
	{
		StartRecording(m_recordID, alpcTarget, m_recordLocation);
	}

	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onRaiseViewSessionStartedEvent( ConvertFromUnicName(alpcTarget), alpcSessionKey, canRemoteAccess);
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::RaiseSharingEventWithCode(LPCSTR alpcTarget, int aEventCode)
{
	if (!alpcTarget)
		return;
	if (0 == *alpcTarget)
		return;
	
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onSharingEventByCode( ConvertFromUnicName(alpcTarget), aEventCode);
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::SendNotificationToServerForFileSession(unsigned int idMessage)
{
	CObjectMapByName<CSharingSession> mapSharingSessions = sessionsFactory.GetAllSharingSession();
	CSharingSession* session = NULL;
	if( (session = mapSharingSessions.GetFirst()) != NULL)
	{
		do 
		{
			if (session->IsFileSharingSession())
			{
				SharingManProtocol::RecordOut StartRecPacket(session->GetSessionID()); 
				session->Send(idMessage, StartRecPacket.GetData(), StartRecPacket.GetDataSize());
			}
		} while ( (session = mapSharingSessions.GetNext()) != NULL);
	}
}

bool CSharingManager::StartPlayingSharing(unsigned int aRecordID, unsigned int aRealityID)
{
	m_enteredInRecord = true;
	m_playOnSharingRecordID = aRecordID;
	m_playOnSharingRealityID = aRealityID;
//	sharingMouseKeyboardHandler->SetRealityId(m_playOnSharingRealityID);
	return true;
}
bool CSharingManager::StartSharingPlay( LPCSTR alpTarget, LPCSTR alpGroupName, LPCSTR alpFileName, bool isPaused, unsigned int aRecordID, unsigned int aShiftTime, unsigned int aServerVersion)
{
	//	if (m_playing)
	{
		if( !alpTarget)
			return false;
		if ( !alpFileName)
			return false;

		// Create server connection
		CSharingConnection*		connection = connectionsFactory.GetConnectionByLocationName( alpGroupName);
		potentialTargetGroupNameInRecord = alpGroupName;

		connection->SetMessageHandler( &sessionsFactory);

		unsigned int objReality = GetObjectRealityIDByTarget(alpTarget);
		if (aRecordID == 4294967295) // после очередного слома в скриптах делаю проверку на -1 тут и использую движковый RecordID
		{
			m_mapPlayRecIDByLocation[alpGroupName] = m_playOnSharingRecordID;
			sessionsFactory.StartFilePlay( alpTarget, alpFileName, m_playOnSharingRecordID, objReality, isPaused, connection, aShiftTime, aServerVersion);
		}
		else // похоже срабатывает только для циклических
		{
			m_mapPlayRecIDByLocation[alpGroupName] = aRecordID;
			sessionsFactory.StartFilePlay( alpTarget, alpFileName, aRecordID, objReality, isPaused, connection, aShiftTime, aServerVersion);
		}
	}
	//raiseViewSessionStartedEvent для уже играемой сессии не вызовется, команду начала записи необходимо делать тут
	//в первый приход для сессии сюда команда записи не сработает тк id этой сессии еще не получен с сервера и запись ее начнется по raiseViewSessionStartedEvent
	if (m_recording && m_isRecordAuthor)
	{
		StartRecording(m_recordID, ConvertToUnicName(alpTarget), m_recordLocation);
	}
	
	return true;
}

void CSharingManager::PauseServerSharing()
{
//	SendNotificationToServerForFileSession(ST_PauseRecord);
/*
	if (m_recording)
	{
		m_playing = false;
		StartRecording(m_recordID, m_recordLocation);
	}
	*/
}
void CSharingManager::ResumeServerSharingInternal()
{
	SendNotificationToServerForFileSession(ST_ResumeRecord);
}
void CSharingManager::ResumeServerSharing()
{
/*	context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]start playing record");
	m_playing = true;
	if (m_recording)
	{
		StopRecordingInFileSession();
		StartRecording(m_recordID, m_recordLocation);
	}
	*/
	//ResumeServerSharingInternal();
}
void CSharingManager::RewindServerSharing()
{
	m_playing = false;
	SendNotificationToServerForFileSession(ST_RewindRecord);
}
bool CSharingManager::PauseUserSharing( LPCSTR alpSessionName)
{
	CSharingSession* sharingSession = sessionsFactory.GetSharingSessionByName(ConvertToUnicName(alpSessionName));
	if (sharingSession)
	{
		if (sharingSession->IsFileSharingSession())
		{
			SharingManProtocol::RecordOut StartRecPacket(sharingSession->GetSessionID()); 
			sharingSession->Send(ST_PauseRecord, StartRecPacket.GetData(), StartRecPacket.GetDataSize());
		}
	}
	CSharingViewSession* viewSession = sessionsFactory.GetViewSessionByName(ConvertToUnicName(alpSessionName));
	if (viewSession)
		viewSession->SetIsPaused(true);

	return false;
}

bool CSharingManager::ResumeUserSharing( LPCSTR alpSessionName)
{
	CSharingSession* sharingSession = sessionsFactory.GetSharingSessionByName(ConvertToUnicName(alpSessionName));
	if (sharingSession)
	{
		if (sharingSession->IsFileSharingSession())
		{
			SharingManProtocol::RecordOut StartRecPacket(sharingSession->GetSessionID()); 
			sharingSession->Send(ST_ResumeRecord, StartRecPacket.GetData(), StartRecPacket.GetDataSize());
		}
	}
	CSharingViewSession* viewSession = sessionsFactory.GetViewSessionByName(ConvertToUnicName(alpSessionName));
	if (viewSession)
		viewSession->SetIsPaused(false);
	return false;
}

bool CSharingManager::StopPlayingSharing()
{
	if (!m_enteredInRecord)
		return false;
	m_playOnSharingRealityID = 0;
//	sharingMouseKeyboardHandler->SetRealityId(m_playOnSharingRealityID);
	m_playing = false;
	m_enteredInRecord = false;
	CObjectMapByName<CSharingViewSession> mapViewSessions = sessionsFactory.GetAllViewSession();
	CSharingViewSession* session = NULL;
	if( (session = mapViewSessions.GetFirst()) != NULL)
	{
		do 
		{
			if (session->GetFileName() != NULL && GetRealityIDFromUnicName(session->GetSessionName()) != 0)
			{
				ViewSessionClosedEvent(session->GetSessionName());
				// Destroy view session
				if (context.gcontext->mpLogWriter)
					context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]delete VS on  exit from viewing record");
				sessionsFactory.DestroyViewSession( session->GetSessionName(), true);
			}
		} while ( (session = mapViewSessions.GetNext()) != NULL);
	}

	CObjectMapByName<CSharingSession> mapSharingSessions = sessionsFactory.GetAllSharingSession();
	CSharingSession* sharingSession = NULL;
	if( (sharingSession = mapSharingSessions.GetFirst()) != NULL)
	{
		do 
		{
			if (sharingSession->IsFileSharingSession() && GetRealityIDFromUnicName(sharingSession->GetSessionName()) != 0)
			{
				// Destroy sharing session
				sessionsFactory.DestroyCapturingSession( sharingSession->GetSessionName(), false);
			}
		} while ( (sharingSession = mapSharingSessions.GetNext()) != NULL);
	}
	return true;
}
//! удалить запись (отсылает запрос на сервер)
bool CSharingManager::DeleteRecord(unsigned int aRecordID)
{
	CSharingConnection* serverConnection = NULL;
	if (m_recordID == aRecordID)
	{
		// удаляется текущая запись 
		CObjectMapByName<CSharingViewSession> mapViewSessions = sessionsFactory.GetAllViewSession();
		CSharingViewSession* session = NULL;
		if( (session = mapViewSessions.GetFirst()) != NULL)
		{
			do 
			{
				// локальность session не совпадает с локальностью записи
				if (strcmp(session->GetConnection()->GetConnectionName(), m_recordLocation.GetString()))
					continue;
				//берем Connection session, чтобы команда на остановку гарантировано пришла раньше команды на удаление
				serverConnection = session->GetConnection(); 
				break;
			} while ( (session = mapViewSessions.GetNext()) != NULL);
		}
		// останавливаем запись
		StopRecording(aRecordID);
	}
	if (serverConnection == NULL)
		serverConnection = connectionsFactory.GetConnectionByLocationName("serverConnection");
	if (serverConnection)
	{
		SharingManProtocol::CDeleteRecord deleteRecPacket(aRecordID); 
		serverConnection->Send(CSharingSessionBase::INCORRECT_SESSION_ID, ST_DeleteRecord, deleteRecPacket.GetData(), deleteRecPacket.GetDataSize());
	}
	return true;
}
void CSharingManager::SessionClosedEvent(LPCSTR alpcTarget)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onSessionClosedEvent(ConvertFromUnicName(alpcTarget));
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::ViewSessionClosedEvent(LPCSTR alpcTarget)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onViewSessionClosedEvent(ConvertFromUnicName(alpcTarget));
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::WaitOwnerEvent(LPCSTR alpcTarget)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onWaitOwnerEvent(ConvertFromUnicName(alpcTarget));
	}
	csVecCallbacks.Unlock();
}
void CSharingManager::NotifyOnWaitOwnerEventInternal(LPCSTR alpcTarget)
{
	sessionsFactory.TryingStartFileSession(alpcTarget);
}

void CSharingManager::TextureSizeChangedEvent(LPCSTR alpcTarget, int width, int height)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onTextureSizeChangedEvent(ConvertFromUnicName(alpcTarget), width, height);
	}
	csVecCallbacks.Unlock();
}

void CSharingManager::RemoteAccessModeChangedEvent( LPCSTR alpcTarget, bool canRemoteAccess)
{
	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
	for ( ; it!=end; it++)
	{
		(*it)->onRemoteAccessModeChangedEvent(ConvertFromUnicName(alpcTarget), canRemoteAccess);
	}
	csVecCallbacks.Unlock();
}

bool CSharingManager::StartRecording(unsigned int aRecordID, LPCSTR alpcLocation)
{
	//	CObjectMapByName<CSharingSession> mapSharingSessions = sessionsFactory.GetAllSharingSession();
	//	CSharingSession* session = NULL;

	CObjectMapByName<CSharingViewSession> mapViewSessions = sessionsFactory.GetAllViewSession();
	CSharingViewSession* session = NULL;

	if( (session = mapViewSessions.GetFirst()) != NULL)
	{
		do 
		{
			// локальность session не совпадает с локальностью записи
			if (strcmp(session->GetConnection()->GetConnectionName(), alpcLocation))
				continue;

			CSharingSession* sharingSession = sessionsFactory.GetSharingSessionByName(session->GetSessionName());
			if (sharingSession != NULL)
			{
				//обновляем время для текущего файла - для записанного объекта не обновится
				RecordStartEvent(sharingSession->GetSessionName(), sharingSession->GetFileName());
				sharingSession->NeedKeyFrame();
			}
			if (session->IsFileSharingSession())
			{
				unsigned int playRecID = m_playOnSharingRecordID;
				if (m_mapPlayRecIDByLocation.find(alpcLocation) != m_mapPlayRecIDByLocation.end())
					playRecID = m_mapPlayRecIDByLocation[alpcLocation];
				SharingManProtocol::RecordOut StartRecPacket(session->GetSessionID(), playRecID, aRecordID); 
				session->Send(ST_StartRecording, StartRecPacket.GetData(), StartRecPacket.GetDataSize());
			}
			else
			{
				SharingManProtocol::RecordOut StartRecPacket(session->GetSessionID(), aRecordID, session->GetFileName()); 
				session->Send(ST_StartRecording, StartRecPacket.GetData(), StartRecPacket.GetDataSize());
			}
		} while ( (session = mapViewSessions.GetNext()) != NULL);
	}
	//StartRecording - вызывается для автора записи, для остальных StartListenRecording
	SetRecordState(aRecordID, alpcLocation, true, true);

	return true;
}

bool CSharingManager::StartRecording(unsigned int aRecordID, LPCSTR alpTarget, LPCSTR alpcLocation)
{
	CSharingViewSession* session = NULL;
	session = sessionsFactory.GetViewSessionByName(alpTarget);
	if(session != NULL)
	{
		// локальность session не совпадает с локальностью записи
		if (strcmp(session->GetConnection()->GetConnectionName(), alpcLocation))
			return false;
		CSharingSession* sharingSession = sessionsFactory.GetSharingSessionByName(session->GetSessionName());
		if (sharingSession != NULL)
		{
			//обновляем время для текущего файла - для записанного объекта не обновится
			RecordStartEvent(sharingSession->GetSessionName(), sharingSession->GetFileName());
		}
		if (session->IsFileSharingSession())
		{
			unsigned int playRecID = m_playOnSharingRecordID;
			if (m_mapPlayRecIDByLocation.find(alpcLocation) != m_mapPlayRecIDByLocation.end())
				playRecID = m_mapPlayRecIDByLocation[alpcLocation];
			SharingManProtocol::RecordOut StartRecPacket(session->GetSessionID(), playRecID, aRecordID); 
			session->Send(ST_StartRecording, StartRecPacket.GetData(), StartRecPacket.GetDataSize());
		}
		else
		{
			SharingManProtocol::RecordOut StartRecPacket(session->GetSessionID(), aRecordID, session->GetFileName()); 
			session->Send(ST_StartRecording, StartRecPacket.GetData(), StartRecPacket.GetDataSize());
		}
//		session->NeedKeyFrame();
	}
	//StartRecording - вызывается для автора записи, для остальных StartListenRecording
	SetRecordState(aRecordID, alpcLocation, true, true);

	return true;
}
bool CSharingManager::StopRecording(unsigned int aRecordID)
{
	// 	CObjectMapByName<CSharingSession> mapSharingSessions = sessionsFactory.GetAllSharingSession();
	// 	CSharingSession* session = NULL;

	if (m_recordID != aRecordID)
	{
		return false;
	}

	CObjectMapByName<CSharingViewSession> mapViewSessions = sessionsFactory.GetAllViewSession();
	CSharingViewSession* session = NULL;
	if( (session = mapViewSessions.GetFirst()) != NULL)
	{
		do 
		{
			// локальность session не совпадает с локальностью записи
			if (strcmp(session->GetConnection()->GetConnectionName(), m_recordLocation.GetString()))
				continue;
			SharingManProtocol::RecordOut StartRecPacket(session->GetSessionID()); 
			session->Send(ST_StopRecording, StartRecPacket.GetData(), StartRecPacket.GetDataSize());
		} while ( (session = mapViewSessions.GetNext()) != NULL);
	}

	SetRecordState(0, "", false, false);

	return true;
}
bool CSharingManager::StopRecordingInFileSession()
{
	SendNotificationToServerForFileSession(ST_StopRecording);
	return true;
}

bool CSharingManager::StartListenRecording( unsigned int aRecordID, LPCSTR alpcLocation)
{
	// мы участвуем в чьей-то записи, для сессий шаринга в локации alpcLocation захватываем ключевой кадр 
	CObjectMapByName<CSharingSession> mapSharingSessions = sessionsFactory.GetAllSharingSession();
	CSharingSession* session = NULL;

	if( (session = mapSharingSessions.GetFirst()) != NULL)
	{
		do 
		{
			if (strcmp(session->GetConnection()->GetConnectionName(), alpcLocation))
				continue;
			session->NeedKeyFrame();
		} while ( (session = mapSharingSessions.GetNext()) != NULL);
	}
	SetRecordState(aRecordID, alpcLocation, true, false);

	return false;
}

bool CSharingManager::StopListenRecording( unsigned int aRecordID)
{
	SetRecordState(0, "", false, false);

	return false;
}

void CSharingManager::SetRecordState(unsigned int aRecordID, LPCSTR alpcLocation, bool aRecording, bool anAuthor)
{
	m_recordID = aRecordID;
	m_recordLocation = alpcLocation;
	m_recording = aRecording;
	m_isRecordAuthor = anAuthor;
}

void CSharingManager::ShowSharingSessionRectangle( LPCSTR alpSessionName)
{
	sessionsFactory.ShowSharingSessionRectangle( ConvertToUnicName(alpSessionName));
}

void CSharingManager::HideSharingSessionRectangle( LPCSTR alpSessionName)
{
	sessionsFactory.HideSharingSessionRectangle( ConvertToUnicName(alpSessionName));
}
void CSharingManager::GetOnlyKeyFrame(LPCSTR alpSessionName)
{
	CObjectMapByName<CSharingViewSession> mapViewSessions = sessionsFactory.GetAllViewSession();
	CSharingViewSession* session = NULL;
	if( (session = mapViewSessions.GetFirst()) != NULL)
	{
		do 
		{
			if (!strcmp(session->GetSessionName(), alpSessionName))
			{
				session->SendGetOnlyKeyFrame();
			}
		} while ( (session = mapViewSessions.GetNext()) != NULL);
	}
}
void CSharingManager::GetAllFrames( LPCSTR alpSessionName)
{
	CObjectMapByName<CSharingViewSession> mapViewSessions = sessionsFactory.GetAllViewSession();
	CSharingViewSession* session = NULL;
	if( (session = mapViewSessions.GetFirst()) != NULL)
	{
		do 
		{
			if (!strcmp(session->GetSessionName(), alpSessionName))
			{
				session->SendGetAllFrames();
			}
		} while ( (session = mapViewSessions.GetNext()) != NULL);
	}
}

void CSharingManager::AllowRemoteControl( LPCSTR alpSessionName)
{
	CSharingSession* session = NULL;
	session = sessionsFactory.GetSharingSessionByName(ConvertToUnicName(alpSessionName));
	if(session != NULL)
	{
		SharingManProtocol::StartSendFrameOut packetOut(ConvertToUnicName(alpSessionName));
		session->Send(ST_AllowRemoteControl, packetOut.GetData(), packetOut.GetDataSize());
	}
}

void CSharingManager::DenyRemoteControl( LPCSTR alpSessionName)
{
	CSharingSession* session = NULL;
	session = sessionsFactory.GetSharingSessionByName(ConvertToUnicName(alpSessionName));
	if(session != NULL)
	{
		SharingManProtocol::StartSendFrameOut packetOut(ConvertToUnicName(alpSessionName));
		session->Send(ST_DenyRemoteControl, packetOut.GetData(), packetOut.GetDataSize());
	}
}

void CSharingManager::FullControlChanged( LPCSTR alpSessionName, int mode)
{
	CSharingSession* session = NULL;
	session = sessionsFactory.GetSharingSessionByName(ConvertToUnicName(alpSessionName));
	if(session != NULL)
	{
		session->SetRemoteControlMode(mode);
	}
}

void CSharingManager::UpdateFileName( LPCSTR alpTarget, LPCSTR alpFileName)
{
	CSharingSession* sharingSession = sessionsFactory.GetSharingSessionByName(ConvertToUnicName(alpTarget));
	CSharingViewSession* viewSession = sessionsFactory.GetViewSessionByName(ConvertToUnicName(alpTarget));
	if (sharingSession != NULL)
		sharingSession->SetFileName(alpFileName);
	if (viewSession != NULL)
		viewSession->SetFileName(alpFileName);
}

void CSharingManager::NotifyVideoDeviceInitialized(int resultCode)
{
	if (resultCode < 0)
		OnSetResolutionCompleted(-resultCode);
	else
	{		
		if( currentSelector != NULL)
		{			
			if (!ShowPreviewWindow(resultCode))
			{
				// по currentSelector определять???
				/*HWND h = currentSelector->GetCapturedWindow();
				HWND hMainWnd = currentSelector->GetMainWindowHandle();*/
				int resultCode1 = WEBCAMERA_CANNOTCAPTUREFRAME;
				int type = currentSelector->GetType();
				switch (type)
				{
				case RECT_SELECTOR:
					resultCode1 = WEBCAMERA_CANNOTCAPTUREFRAMERECT;
					break;
				case WND_SELECTOR:
					resultCode1 = WEBCAMERA_CANNOTCAPTUREFRAMEWND;
					break;
				case VIDEOFILE_SELECTOR:
					resultCode1 = VIDEO_FILE_IS_NOT_PLAYING;
					break;
				}
				OnSelectionCompleted(resultCode1);
				return;
			}
			if (NULL == previewWindow)
				return;
			//resultCode == 0 - не веб-камера
			if (resultCode == 0)
				previewWindow->SetLayout(NORMAL_PREVIEW_MODE);
			else
				previewWindow->SetLayout(WEBCAM_PREVIEW_MODE);
		}
	}
}

CComString CSharingManager::GetNameInZeroReality( LPCSTR alpTarget)
{
	CComString target = alpTarget;
	CComString baseSessionName;
	for (int i=0; i < target.GetLength(); i++)
	{
		if (target[i] == '_' && target[i+1] == '_')
		{
			baseSessionName.CreateString(alpTarget, i);
			baseSessionName += ".pscreenObj.sharingCtrl.sharingImg";
			break;
		}
	}
	baseSessionName = ConvertToUnicName(baseSessionName.GetBuffer(), 0);
	return baseSessionName;
}

CComString CSharingManager::GetNameCycleSession( LPCSTR alpTarget)
{
	CComString sessionName = ConvertFromUnicName(alpTarget);
	sessionName = ConvertToUnicName(sessionName.GetBuffer(), 0);
	return sessionName;
}

CComString CSharingManager::ConvertNameToRecordReality( LPCSTR alpTarget, unsigned int auBornRealityID)
{
	CComString baseSessionName = alpTarget;
	int pos = baseSessionName.Find("_reality__");

	if (pos == -1)
	{
		pos = baseSessionName.Find(".pscreenObj.sharingCtrl.sharingImg");
		baseSessionName.CreateString(alpTarget, pos);
		char buf[100];
		_itoa_s(auBornRealityID, (char*)buf, 100, 10);
		baseSessionName += "__";
		baseSessionName += buf;
		baseSessionName += "_reality__.pscreenObj.sharingCtrl.sharingImg";

		//scene.objects.sqvu_screen_senter.pscreenObj.sharingCtrl.sharingImg
		//scene.objects.sqvu_screen_senter__2122_reality__.pscreenObj.sharingCtrl.sharingImg
	}
	baseSessionName = ConvertToUnicName(baseSessionName.GetBuffer(), auBornRealityID);
	return baseSessionName;
}

CComString CSharingManager::ConvertToUnicName( LPCSTR alpTarget, unsigned int auBornRealityID)
{
	CComString baseSessionName = alpTarget;
	char buf[100];
	_itoa_s(auBornRealityID, (char*)buf, 100, 10);
	baseSessionName += "__u";
	baseSessionName += buf;
	return baseSessionName;
}

CComString CSharingManager::ConvertToUnicName( LPCSTR alpTarget)
{
	CComString baseSessionName = alpTarget;
	int pos = baseSessionName.Find(".pscreenObj.sharingCtrl.sharingImg");
	if (pos != -1)
	{
		baseSessionName.CreateString(alpTarget, pos);
	}
	char buf[100];

	int iSLen=(int)strlen(baseSessionName.GetBuffer());
	wchar_t* name = MP_NEW_ARR( wchar_t, iSLen*MB_CUR_MAX+1);
	size_t resLen = 0;
	mbstowcs_s(&resLen, name, iSLen*MB_CUR_MAX+1, alpTarget, iSLen);
	name[iSLen]=L'\0';

	unsigned int objReality = context.gcontext->mpSM->GetObjectRealityIDByTarget(name);
	_itoa_s(objReality, (char*)buf, 100, 10);
	baseSessionName = alpTarget;
	baseSessionName += "__u";
	baseSessionName += buf;
	return baseSessionName;
}

CComString CSharingManager::ConvertFromUnicName( LPCSTR alpTarget)
{
	CComString target = alpTarget;
	CComString baseSessionName = alpTarget;
	for (int i=target.GetLength()-1; i > 0; i--)
	{
		if (i-2 >= 0 && target[i] == 'u' && target[i-1] == '_' && target[i-2] == '_')
		{
			baseSessionName.CreateString(alpTarget, i-2);
			break;
		}
	}
	return baseSessionName;
}

unsigned int CSharingManager::GetRealityIDFromUnicName( LPCSTR alpTarget)
{
	CComString target = alpTarget;
	CComString baseSessionName = alpTarget;
	CComString recordID = "";
	for (int i=target.GetLength()-1; i > 0; i--)
	{
		if (i-2 >= 0 && target[i] == 'u' && target[i-1] == '_' && target[i-2] == '_')
		{
			break;
		}
		recordID += target[i];
	}
	if (recordID != "")
		return rtl_atoi(recordID.GetBuffer());
	else
		return -1;
}

unsigned int CSharingManager::GetObjectRealityIDByTarget( LPCSTR alpTarget)
{
	CComString baseSessionName = alpTarget;
	int pos = baseSessionName.Find(".pscreenObj.sharingCtrl.sharingImg");
	if (pos != -1)
	{
		baseSessionName.CreateString(alpTarget, pos);
	}
	
	int iSLen=(int)strlen(baseSessionName.GetBuffer());
	wchar_t* name = MP_NEW_ARR( wchar_t, iSLen*MB_CUR_MAX+1);
	size_t resLen = 0;
	mbstowcs_s(&resLen, name, iSLen*MB_CUR_MAX+1, alpTarget, iSLen);
	name[iSLen]=L'\0';
	unsigned int objReality = context.gcontext->mpSM->GetObjectRealityIDByTarget(name);
	return objReality;
}

void* CSharingManager::GetIMouseKeyboardHandler()
{
	return sharingMouseKeyboardHandler;
}

void CSharingManager::TraceAllSession()
{
	context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]All ViewSession:");
	CObjectMapByName<CSharingViewSession> mapViewSessions = sessionsFactory.GetAllViewSession();
	CSharingViewSession* session = NULL;
	if( (session = mapViewSessions.GetFirst()) != NULL)
	{
		do 
		{
			CComString mystr = session->GetSessionName();
			context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]VS name = "+mystr);
			char buf[100];
			_itoa_s(session->GetSessionID(), (char*)buf, 100, 10);
			mystr = buf;
			context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]VS ID = "+mystr);
			mystr = session->GetFileName();
			context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]VS has file name = "+mystr);
			_itoa_s(session->IsFileSharingSession(), (char*)buf, 100, 10);
			mystr = buf;
			context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]VS is session playing file = "+mystr);
			_itoa_s(session->IsPaused(), (char*)buf, 100, 10);
			mystr = buf;
			context.gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]VS can handle playing file = "+mystr);
		} while ( (session = mapViewSessions.GetNext()) != NULL);
	}
	
}

void CSharingManager::SetCursorMode(LPCSTR alpTarget, int mode)
{
	CSharingSession* sharingSession = sessionsFactory.GetSharingSessionByName(ConvertToUnicName(alpTarget));
	if (sharingSession)
	{
		if (sharingSession->GetSelector())
			sharingSession->GetSelector()->SetShowCursorState(mode);
	}
}

void CSharingManager::AddVASEPack( void* pack, bool abKeyFrame, CSharingViewSession* viewSession)
{
	sessionsFactory.AddVASEPack(pack, abKeyFrame, viewSession);
}


std::string CSharingManager::GetAllViewSession()
{
	CObjectMapByName<CSharingViewSession> mapViewSessions = sessionsFactory.GetAllViewSession();
	CSharingViewSession* session = NULL;
	std::string str = "";
	if( (session = mapViewSessions.GetFirst()) != NULL)
	{
		do 
		{
			str += session->GetSessionName();
			str += " ";
		} while ( (session = mapViewSessions.GetNext()) != NULL);
	}
	return str;
}
std::string CSharingManager::GetAllSharingSession()
{
	CObjectMapByName<CSharingSession> mapSharingSessions = sessionsFactory.GetAllSharingSession();
	CSharingSession* session = NULL;
	std::string str = "";
	if( (session = mapSharingSessions.GetFirst()) != NULL)
	{
		do 
		{
			str += session->GetSessionName();
			str += " ";
		} while ( (session = mapSharingSessions.GetNext()) != NULL);
	}
	return str;
}
std::string CSharingManager::GetSessionInfo(LPCSTR alpTarget)
{
	CSharingViewSession* session = NULL;
	std::string str = "";
	if (!alpTarget)
		return str;
	session = sessionsFactory.GetViewSessionByName(alpTarget);
	
	if (session)
	{
		str += session->GetSessionInfo();
	}
	return str;
}
void CSharingManager::ShowLastFrame(LPCSTR alpTarget)
{
	if (!alpTarget)
		return;
	CSharingViewSession* session = NULL;
	session = sessionsFactory.GetViewSessionByName(alpTarget);
	if (session)
	{
		session->ShowLastFrame();
	}
}

CSharingSession* CSharingManager::GetInteractiveBoardSession()
{
	CObjectMapByName<CSharingSession> mapSharingSessions = sessionsFactory.GetAllSharingSession();
	CSharingSession* session = NULL;
	if( (session = mapSharingSessions.GetFirst()) != NULL)
	{
		do 
		{
			if (session->GetSelector())
			{
				if (session->GetSelector()->GetType() == INTERACTIVEBOARD_SELECTOR)
				{
					return session;
				}
			}
		} while ( (session = mapSharingSessions.GetNext()) != NULL);
	}
	return NULL;
}

bool CSharingManager::SetupInteractiveBoardSettings()
{
	CSharingSession* session = GetInteractiveBoardSession();
	
	if (session)
	{
		int monitorID = GetInteractiveBoardMonitorID();
		session->GetSelector()->SetInteractiveBoardCapture(monitorID);
		session->GetSelector()->UpdateCurrentSelectingRect();
	}
	return true;
}

void CSharingManager::OnGlobalMouseEvent(MouseEvent aMouseEvent)
{
	m_hookMouseEvents.push_back(aMouseEvent);
}

void CSharingManager::SendHookMouseEvent()
{
	for (unsigned int i=0; i<m_hookMouseEvents.size(); i++)
	{
		int x = GET_X_LPARAM(m_hookMouseEvents[i].lParam);
		int y = GET_Y_LPARAM(m_hookMouseEvents[i].lParam);

		CSharingSession* session = GetInteractiveBoardSession();
		if (session)
		{
			RECT monitorRect = session->GetSelector()->GetCurrentSelectingRect();
			//отжатие шлем всегда 
			if (m_hookMouseEvents[i].wParam != WM_LBUTTONUP && m_hookMouseEvents[i].wParam != WM_RBUTTONUP)
			{
				if (x < monitorRect.left || x > monitorRect.right)
					continue;
				if (y < monitorRect.top || y > monitorRect.bottom)
					continue;
			}
			

			float textureX = ((float)x - monitorRect.left) / (monitorRect.right - monitorRect.left);
			float textureY = ((float)y - monitorRect.top) / (monitorRect.bottom - monitorRect.top);

			csVecCallbacks.Lock();
			VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
			for ( ; it!=end; it++)
			{
				(*it)->onRaiseGlobalMouseEvent((unsigned int)m_hookMouseEvents[i].wParam, textureX, textureY);
			}
			csVecCallbacks.Unlock();

		}
	}
	m_hookMouseEvents.clear();
}