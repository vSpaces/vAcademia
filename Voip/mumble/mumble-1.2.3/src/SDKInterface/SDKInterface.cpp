#include "mumble_pch.hpp"

#include "SDKInterface.h"
#include "DirectSound.h"
#include "MainWindow.h"
#include "ServerHandler.h"
#include "Channel.h"
#include "log.h"
#include "NetworkConfig.h"
#include "SocketRPC.h"
#include "Database.h"
#include "Plugins.h"

#ifdef ADDITIONAL_WINDOWS
#include "Overlay.h"
#include "LCD.h"
#include "FileEngine.h"
#include "AudioWizard.h"
#endif

#include "AudioInFromStream/AudioInStream.h"

#include "QAppMumble.h"
#include <mmdeviceapi.h>

extern void os_init();
extern char *os_lang;

static CSDKInterface* m_sharedSDKInterface = NULL;

const unsigned char PT_UNKNOWN		= 0;
const unsigned char PROXY_TYPE_HTTP		= 1;
const unsigned char PROXY_TYPE_SOCKS4	= 3;
const unsigned char PROXY_TYPE_SOCKS5	= 4;
const unsigned char PROXY_TYPE_HTTPS	= 2;

CSDKInterface* CSDKInterface::sharedSDKInterface()
{
	if (!m_sharedSDKInterface)
	{
		m_sharedSDKInterface = new CSDKInterface();
	}
	return m_sharedSDKInterface;
}

CSDKInterface::CSDKInterface(void)
{
	m_udpPort = 0;
	m_isInitialized = false;
	m_isConnected = false;

	InitializeCriticalSection(&m_csConnectChanged);
}

CSDKInterface::~CSDKInterface(void)
{
	DeleteCriticalSection(&m_csConnectChanged);
}

void CSDKInterface::SetInitialized(bool anInit) {
	m_isInitialized = anInit;
}

bool CSDKInterface::GetInitialized() {
	return m_isInitialized;
}

void CSDKInterface::SetUpdServerPort(unsigned short anUdpPort){
	m_udpPort = anUdpPort;
}

unsigned short CSDKInterface::GetUpdServerPort(){
	return m_udpPort;
}

void CSDKInterface::SetServerHost(QString anServerHost){
	m_serverHost = anServerHost;
}

QString CSDKInterface::GetServerHost(){
	return m_serverHost;
}

void CSDKInterface::SetConnectingChannelName(QString aChannelName) {
	m_connectChannelName = aChannelName;
}

void CSDKInterface::onAddChannel(int id, Channel *p, const QString &name) 
{
	// callback на создание канала на сервере - если создается канал с тем же именем то сервер вернет ошибку
	// и сюда вызов не придет
	if (m_connectChannelName == name)
	{
		// за время ожидания отклика сервера имя канала не сменилось - подключаемся
		g.sh->joinChannel(id);	
	}
}

void CSDKInterface::onConnectedByDoLogin() 
{
	EnterCriticalSection(&m_csConnectChanged);
	m_isConnected = true;
	LeaveCriticalSection(&m_csConnectChanged);
}

void CSDKInterface::onDisconnected() 
{
	EnterCriticalSection(&m_csConnectChanged);
	m_isConnected = false;
	LeaveCriticalSection(&m_csConnectChanged);
}

bool CSDKInterface::IsConnectedNow()
{
	EnterCriticalSection(&m_csConnectChanged);
	bool isConnected = m_isConnected;
	LeaveCriticalSection(&m_csConnectChanged);
	return isConnected;
}


void CSDKInterface::AddFindingMicDev(AudioInput* anAudioIn)
{
	m_allStartedDevices.push_back(anAudioIn);
}

FindedDeviceInfo CSDKInterface::GetFindedMicInd()
{
	FindedDeviceInfo result;
	qWarning("GetFindedMicInd");
	double maxPeakLevel = 0;
	int indexOfMaxInputLevel = -1;
	QString findedDevGuid = QLatin1String("not found");
	QString findedDevName = QLatin1String("not found");
	QString findedDevLineName = QLatin1String("not found");
	for (unsigned int i=0; i< m_allStartedDevices.size(); i++) {
		if (m_allStartedDevices[i]->getPeakInputLevel() > maxPeakLevel ) {
			maxPeakLevel = m_allStartedDevices[i]->getPeakInputLevel();
			indexOfMaxInputLevel = i;
			if (m_allStartedDevices[i]->getDeviceFormFator() == Microphone && m_allStartedDevices[i]->getDeviceName() != QLatin1String("")
				&& m_allStartedDevices[i]->getDeviceGuid() != QLatin1String(""))
			{
				findedDevGuid = m_allStartedDevices[i]->getDeviceGuid();
				findedDevName = m_allStartedDevices[i]->getDeviceName();
				findedDevLineName = m_allStartedDevices[i]->getDeviceLineName();
			}
		}
	}
	qWarning(findedDevName.toStdString().c_str());
	result.deviceGuid = findedDevGuid;
	result.deviceName = findedDevName;
	result.deviceLineName = findedDevLineName;
	return result;
}

void CSDKInterface::StopAndClearFindingDev()
{
	for (unsigned int i=0; i< m_allStartedDevices.size(); i++) {
		delete m_allStartedDevices[i];
	}
	m_allStartedDevices.clear();
}

AudioMainThread* CSDKInterface::GetAudioThread()
{
	return &m_audioMainThread;
}

AudioMainThread::AudioMainThread()
{
	QThread::setStackSize(64*1024);
	CVolumeTalkingController::GetVolumeController();
}

AudioMainThread::~AudioMainThread()
{
	CVolumeTalkingController::Destroy();
}

void AudioMainThread::run()
{
	HANDLE hEvent; 

	hEvent = g.hEvent;
	bool eventSeted = false;	

	int res = 0;
	int argc = 0;
	char** argv = NULL;
	QT_REQUIRE_VERSION(argc, argv, "4.4.0");

#if defined(Q_OS_WIN)
	SetDllDirectory(L"");
#endif

	// Initialize application object.
	QAppMumble a(argc, argv);
	a.setApplicationName(QLatin1String("Vacademia-voip"));
	a.setOrganizationName(QLatin1String("Vacademia"));
	a.setOrganizationDomain(QLatin1String("vacademia.com"));
	a.setQuitOnLastWindowClosed(false);

	Q_INIT_RESOURCE(mumble);


	qsrand(QDateTime::currentDateTime().toTime_t());

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
	os_init();
#endif

	bool bAllowMultiple = false;
	QUrl url;
	// Load preferences
	g.s.load();
//set position audio
	g.s.bTransmitPosition = true;
	g.s.bPositionalAudio = true;

	g.s.bReconnect = false;
	g.s.bPositionalHeadphone = false;
	g.s.iJitterBufferSize = 180;
	g.s.fVADmax = 0.11f;
	g.s.fVADmin = 0.1f;
	g.s.iMinLoudness = 30000;
	g.s.iNoiseSuppress = -30;
	g.s.iFramesPerPacket = 4;

	g.s.fVolume = 1.0f;

	g.s.fAudioMaxListenDistance = 300.0f;
	g.s.fAudioMinDistance = 10.0f;
	g.s.fAudioMaxDistance = 280.0f;
	g.s.fAudioMaxDistVolume = 1.0f;

	//отключаем нотификации, обновление и оверлей
	g.s.bStateInTray = false;
	g.s.os.bEnable = false;
	g.s.os.bFps = false;
	g.s.bTTS = false;
	g.s.bUpdateCheck = false;
	g.s.bPluginOverlayCheck = false;
	g.s.bUsage = false;
	g.s.iMaxImageSize = 0;
	g.s.bMute = true;
	g.s.iQuality = 29000;
	g.s.bAttenuateOthers = false;

	g.s.qmMessages[Log::DebugInfo] = Settings::LogConsole;
	g.s.qmMessages[Log::CriticalError] = Settings::LogConsole;
	g.s.qmMessages[Log::Warning] = Settings::LogConsole;
	g.s.qmMessages[Log::Information] = Settings::LogConsole;
	g.s.qmMessages[Log::ServerConnected] = Settings::LogConsole;
	g.s.qmMessages[Log::ServerDisconnected] = Settings::LogConsole;
	g.s.qmMessages[Log::UserJoin] = Settings::LogConsole;
	g.s.qmMessages[Log::UserLeave] = Settings::LogConsole;
	g.s.qmMessages[Log::Recording] = Settings::LogConsole;
	g.s.qmMessages[Log::UserKicked] = Settings::LogConsole;
	g.s.qmMessages[Log::YouKicked] = Settings::LogConsole;
	g.s.qmMessages[Log::SelfMute] = Settings::LogConsole;
	g.s.qmMessages[Log::OtherSelfMute] = Settings::LogConsole;
	g.s.qmMessages[Log::YouMuted] = Settings::LogConsole;
	g.s.qmMessages[Log::YouMutedOther] = Settings::LogConsole;
	g.s.qmMessages[Log::OtherMutedOther] = Settings::LogConsole;
	g.s.qmMessages[Log::ChannelJoin] = Settings::LogConsole;
	g.s.qmMessages[Log::ChannelLeave] = Settings::LogConsole;
	g.s.qmMessages[Log::PermissionDenied] = Settings::LogConsole;
	g.s.qmMessages[Log::TextMessage] = Settings::LogConsole;

	//для оверлея 
	g.s.os.qslBlacklist.push_back(QLatin1String("player.exe"));

	DeferInit::run_initializers();

	a.setStyleSheet(MainWindow::defaultStyleSheet);
	QDir::addSearchPath(QLatin1String("skin"),QLatin1String(":/"));
	QDir::addSearchPath(QLatin1String("translation"), QLatin1String(":/"));

	QString qsSystemLocale = QLocale::system().name();

	QString locale = g.s.qsLanguage.isEmpty() ? qsSystemLocale : g.s.qsLanguage;

	g.qsRegionalHost = qsSystemLocale;
	g.qsRegionalHost = g.qsRegionalHost.remove(QRegExp(QLatin1String("^.+_"))).toLower() + QLatin1String(".mumble.info");

	// Initialize proxy settings
	NetworkConfig::SetupProxy();

	g.nam = new QNetworkAccessManager();

	//g.pStreamSoundBuffer = new StreamSound();
	// Initialize logger
	g.l = new Log();
#ifdef USE_SQL
	// Initialize database
	g.db = new Database();
#endif
#ifdef USE_OVERLAY
	g.o = new Overlay();
	//g.o->setActive(g.s.os.bEnable);
	g.o->setActive(false);
#endif
		
#ifdef ADDITIONAL_WINDOWS
	g.lcd = new LCD();
#endif
	// Process any waiting events before initializing our MainWindow.
	// The mumble:// URL support for Mac OS X happens through AppleEvents,
	// so we need to loop a little before we begin.
	a.processEvents();

	// Main Window
	g.s.aotbAlwaysOnTop = Settings::OnTopNever;
	g.mw=new MainWindow(NULL);
	g.mw->hide();
	//	g.mw->show();

	SocketRPC *srpc = new SocketRPC(QLatin1String("VacademiaVoip"));
	// Plugins
	g.p = new Plugins(NULL);
	g.p->rescanPlugins();

	//MumbleFileEngineHandler *mfeh = new MumbleFileEngineHandler();

//	Audio::start();

	a.setQuitOnLastWindowClosed(false);
	g.s.uiUpdateCounter = 1;
	SetEvent(hEvent);
	eventSeted = true;

	res=a.exec();
		
	Audio::stop();
	for (unsigned int i=0; i<g.pStreamSoundBuffers.size(); i++)
	{
		delete g.pStreamSoundBuffers[i];
	}
	g.pStreamSoundBuffers.clear();
		

	if (g.sh)
		g.sh->disconnect();

//		delete mfeh;

	delete srpc;

	g.sh.reset();
	while (g.sh && ! g.sh.unique())
		QThread::yieldCurrentThread();
	g.sh.reset();

	delete g.mw;

	delete g.nam;
#ifdef ADDITIONAL_WINDOWS
	delete g.lcd;
#endif
#ifdef USE_SQL
	delete g.db;
#endif
	delete g.p;
	delete g.l;

#ifdef USE_BONJOUR
	delete g.bc;
#endif
#ifdef USE_OVERLAY
	delete g.o;
#endif
	DeferInit::run_destroyers();

	delete Global::g_global_struct;
	Global::g_global_struct = NULL;

#ifndef QT_NO_DEBUG
	// Hide Qt memory leak.
#ifdef SSL_SOCKET
	QSslSocket::setDefaultCaCertificates(QList<QSslCertificate>());
#endif
	// Release global protobuf memory allocations.
#if (GOOGLE_PROTOBUF_VERSION >= 2001000)
	google::protobuf::ShutdownProtobufLibrary();
#endif
#endif

}


namespace api {

MUMBLE_API BOOL MUMBLE_IsVoiceActivated(IN LPVOID pClientInstance) {
	if (g.s.atTransmit == Settings::VAD)
		return TRUE;
	return FALSE;
}

MUMBLE_API BOOL MUMBLE_EnableVoiceActivation(IN LPVOID pClientInstance, BOOL bEnable) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	if (bEnable)
		g.s.atTransmit = Settings::VAD;
	else
		g.s.atTransmit = Settings::Continous;
	return TRUE;
}

MUMBLE_API BOOL MUMBLE_SetVoiceActivationLevel(IN LPVOID pClientInstance, INT32 nLevel){
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	if (nLevel < 0 || nLevel > 32767)
		return FALSE;
	
	g.s.fVADmin = static_cast<float>(nLevel) / 32767.0f;
	g.s.fVADmax = g.s.fVADmin + 0.01f;
	
	return TRUE;
}

MUMBLE_API BOOL MUMBLE_ShutdownSoundSystem(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	Audio::stop();
	return TRUE;
}

MUMBLE_API BOOL MUMBLE_LaunchSoundSystem(IN LPVOID pClientInstance, 
										 IN SoundSystem nSoundSystem, 
										 IN INT32 nInputDeviceID, 
										 IN INT32 nOutputDeviceID,
										 IN INT32 nRecorderFreq,
										 IN INT32 nQuality,
										 IN BOOL bStereoPlayback) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	g.s.iQuality = nQuality*8000 - 5000;
	if (g.s.iQuality < 8000)
		g.s.iQuality = 8000;
	QString soundSystem;
	if (nSoundSystem == SOUNDSYSTEM_WASAPI) {
		soundSystem = QLatin1String("WASAPI");
	}
	else if (nSoundSystem == SOUNDSYSTEM_DSOUND) {
		soundSystem = QLatin1String("DirectSound");
	}
	else if (nSoundSystem == SOUNDSYSTEM_STREAM) {
		soundSystem = QLatin1String("SoundStream");
	}
	else {
		soundSystem = QLatin1String("");
		qWarning("MUMBLE_LaunchSoundSystem - error unknown soundSystem");
		return FALSE;
	}
	g.s.qsAudioInput = soundSystem;
	g.s.qsAudioOutput = soundSystem;
//запрашиваем список устройств работающих на прием по выбранной системе
	QList<audioDevice> ql;
	if (AudioInputRegistrar::qmNew) {
		QString str = g.s.qsAudioInput;
		AudioInputRegistrar *air = AudioInputRegistrar::qmNew->value(g.s.qsAudioInput);
		if (!air) {
			qWarning("MUMBLE_LaunchSoundSystem - error AudioInputRegistrar not found");
			return FALSE;
		}
		ql = air->getDeviceChoices( false);

		int idx = 0;
		bool findDev = false;
//ищем устройство по выбранному индексу
		foreach(audioDevice d, ql) {
			LPGUID lpguid = reinterpret_cast<LPGUID>(((QVariant)d.first).toByteArray().data());
			lpguid = reinterpret_cast<LPGUID>(((QVariant)d.second).toByteArray().data());
			if (idx == nInputDeviceID) {
				air->setDeviceChoice(d.second, g.s);
				g.s.qsHumanInput = d.first;
				findDev = true;
				break;
			}
			++idx;
		}
		if (!findDev) {
			qWarning("MUMBLE_LaunchSoundSystem - error input not found");
		//	return FALSE;
		}
	}
//запрашиваем список устройств работающих на выход по выбранной системе
	if (AudioOutputRegistrar::qmNew) {
		AudioOutputRegistrar *aor = AudioOutputRegistrar::qmNew->value(g.s.qsAudioOutput);
		if (!aor) {
			qWarning("MUMBLE_LaunchSoundSystem - error AudioOutputRegistrar not found");
			return FALSE;
		}
		ql = aor->getDeviceChoices( false);

		int idx = 0;
		bool findDev = false;
//ищем устройство по выбранному индексу
		foreach(audioDevice d, ql) {
			if (idx == nOutputDeviceID) {
				aor->setDeviceChoice(d.second, g.s);
				g.s.qsHumanOutput = d.first;
				findDev = true;
				break;
			}
			++idx;
		}
		if (!findDev) {
			qWarning("MUMBLE_LaunchSoundSystem - error output not found");
		//	return FALSE;
		}
	}
	
//запуск потоков ввода/вывода аудио
	Audio::stop();
	Audio::start();
	return TRUE;	
}

MUMBLE_API BOOL MUMBLE_SetMasterVolume(IN LPVOID pClientInstance, IN INT32 nVolume) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	if (nVolume < 0 || nVolume > 255)
		return FALSE;

	float decVolume = nVolume;

	//mumble 0..4
	float volume = static_cast<float>(decVolume) / 64.0f;
	
	g.s.fVolume = volume;
	
	return TRUE;
}

MUMBLE_API BOOL MUMBLE_SetEchoCancellation(IN LPVOID pClientInstance, IN BOOL anEnabled) {
	if (g.s.bEcho == (bool)anEnabled)
		return TRUE;
	g.s.bEcho = anEnabled;
	//запуск потоков ввода/вывода аудио
	Audio::stop();
	Audio::start();
	return TRUE;
}

MUMBLE_API BOOL MUMBLE_SetMixerEchoCancellation(IN LPVOID pClientInstance, IN BOOL anEnabled, IN SoundSystem nSoundSystem) {
	if (nSoundSystem != SOUNDSYSTEM_WASAPI)
		return TRUE; //for xp no echo cancellation 
	if (g.s.bEchoForSystemMixer == (bool)anEnabled)
		return TRUE;
	g.s.bEchoForSystemMixer = anEnabled;
	//запуск потоков ввода/вывода аудио
	Audio::stop();
	Audio::start();
	return TRUE;
}

MUMBLE_API BOOL MUMBLE_SetUserVolume(IN LPVOID pClientInstance,
									 IN INT32 nUserID, IN INT32 nVolume) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
//должна быть простановка громкости для каждого человека	
//	MUMBLE_SetMasterVolume(pClientInstance, nVolume);

	ClientUser* clUser = ClientUser::get(nUserID);

	if (!clUser)
		return FALSE;

	QString logstr = QLatin1String("MUMBLE_SetUserVolume - nUserID==") + QString::number(nUserID) + QLatin1String("  volume==") + QString::number(nVolume);
	qWarning(logstr.toStdString().c_str());

	if (nVolume == 0)
		clUser->bLocalMute = true;
	else
		clUser->bLocalMute = false;

	clUser->removeRef();

	return TRUE;
}

MUMBLE_API INT32 MUMBLE_GetMyUserID(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	return (INT32)g.uiSession;
}

MUMBLE_API BOOL MUMBLE_SetEncoderComplexity(IN LPVOID pClientInstance, 
											IN INT32 nComplexity){
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	return FALSE;
}

MUMBLE_API BOOL MUMBLE_IsConnecting(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	return FALSE;
}

MUMBLE_API BOOL MUMBLE_Connect(IN LPVOID pClientInstance,
							   IN const TTCHAR* lpszHostAddress, 
							   IN INT32 nTcpPort, 
							   IN INT32 nUdpPort, 
							   IN INT32 nLocalTcpPort, 
							   IN INT32 nLocalUdpPort) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	QString qsServer;
	unsigned short usPort = nUdpPort;
	qsServer = QString::fromWCharArray(lpszHostAddress);
	sdkApi->SetServerHost(qsServer);
	sdkApi->SetUpdServerPort(usPort);
	
	if (g.hCallbackWnd)
		::SendMessage(g.hCallbackWnd, WM_MUMBLE_CONNECTSUCCESS, NULL, NULL);
	if (g.pOnConnectSuccessCallback)
		((EmptyCallback*)g.pOnConnectSuccessCallback)();

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_SetProxySettings(IN LPVOID pClientInstance,
										IN const TTCHAR* lpszProxyHostAddress, 
										IN const TTCHAR* lpszProxyUserName, 
										IN const TTCHAR* lpszProxyPassword, 
										IN INT32 nProxyType)
{
	if (!lpszProxyHostAddress || !lpszProxyUserName || !lpszProxyPassword)
		return FALSE;
	if (nProxyType == 0) {
		g.s.ptProxyType = Settings::NoProxy;	

		g.s.qsProxyHost = QLatin1String("");
		g.s.usProxyPort = 0;
		g.s.qsProxyUsername = QLatin1String("");
		g.s.qsProxyPassword = QLatin1String("");
		NetworkConfig::SetupProxy();
		return TRUE;
	}
	else if (nProxyType == PROXY_TYPE_HTTP || nProxyType == PROXY_TYPE_HTTPS) {
		g.s.ptProxyType = Settings::HttpProxy;
	}
	else if (nProxyType == PROXY_TYPE_SOCKS4 || nProxyType == PROXY_TYPE_SOCKS5) {
		g.s.ptProxyType = Settings::Socks5Proxy;
	}

	QString qsProxyHostAddress, qsProxyUserName, qsProxyPassword;
	qsProxyHostAddress = QString::fromWCharArray(lpszProxyHostAddress);
	qsProxyUserName = QString::fromWCharArray(lpszProxyUserName);
	qsProxyPassword = QString::fromWCharArray(lpszProxyPassword);
	if (qsProxyHostAddress == QLatin1String(""))
	{
		g.s.ptProxyType = Settings::NoProxy;
		return FALSE;
	}
	QString qsProxyIP, qsProxyPort;  

	QChar qCh(':', 0); 
	int ind = qsProxyHostAddress.lastIndexOf(qCh);
	qsProxyIP = qsProxyHostAddress;
	qsProxyIP = qsProxyIP.remove(ind, qsProxyHostAddress.length()-ind);
	qsProxyPort = qsProxyHostAddress;
	qsProxyPort = qsProxyPort.remove(0, ind+1);

	g.s.qsProxyHost = qsProxyIP;
	g.s.usProxyPort = qsProxyPort.toUShort();
	g.s.qsProxyUsername = qsProxyUserName;
	g.s.qsProxyPassword = qsProxyPassword;

	if (g.s.ptProxyType == Settings::Socks5Proxy)
		qWarning("Work in socks proxy");
	else if (g.s.ptProxyType == Settings::HttpProxy)
		qWarning("Work in https proxy");

	NetworkConfig::SetupProxy();

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_DoLogin(IN LPVOID pClientInstance,
							   IN const TTCHAR* lpszNickName, 
							   IN const TTCHAR* lpszServerPassword) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	qWarning("MUMBLE_DoLogin 1");
	if (sdkApi->IsConnectedNow())
		return FALSE;
	QString qsUsername, qsPassword;
	qsUsername = QString::fromWCharArray(lpszNickName);
	qsPassword = QString::fromWCharArray(lpszServerPassword);
	qWarning("MUMBLE_DoLogin 2");
	if (qsUsername == QLatin1String(""))
		return FALSE;
	qWarning("MUMBLE_DoLogin 3");
	g.mw->connectToServer(sdkApi->GetServerHost(), sdkApi->GetUpdServerPort(), qsUsername, qsPassword);

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_StartTransmitting(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	AudioInputPtr ai = g.ai;
	if (ai)
		ai->tIdle.restart();

	g.s.bMute = false;

	if (g.sh) {
		g.sh->setSelfMuteDeafState(g.s.bMute, g.s.bDeaf);
	}

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_StopTransmitting(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	
	AudioInputPtr ai = g.ai;
	if (ai)
		ai->tIdle.restart();

	g.s.bMute = true;

	if (g.sh) {
		g.sh->setSelfMuteDeafState(g.s.bMute, g.s.bDeaf);
	}

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_SetAGCSettings(IN LPVOID pClientInstance, 
									  IN INT32 nGainLevel, 
									  IN INT32 nMaxIncrement,
									  IN INT32 nMaxDecrement,
									  IN INT32 nMaxGain) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	return FALSE;
}

MUMBLE_API BOOL MUMBLE_IsDenoising(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	
	if (g.s.iNoiseSuppress != 0)
		return TRUE; 

	return FALSE;
}

MUMBLE_API BOOL MUMBLE_EnableDenoising(IN LPVOID pClientInstance, 
									   IN BOOL bEnable) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	if (bEnable)
		g.s.iNoiseSuppress = -30;
	else
		g.s.iNoiseSuppress = 0;

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_SetUserPosition(IN LPVOID pClientInstance,
									   IN INT32 nUserID, 
									   IN float x,
									   IN float y, 
									   IN float z) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	return FALSE;
}

MUMBLE_API BOOL MUMBLE_SetVoiceGainLevel(IN LPVOID pClientInstance, IN float nLevel) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	

	g.s.vAcademGain = nLevel;
//для отсутствия треска при молчании
	g.s.iMinLoudness = 30000;

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_IsAuthorized(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	return sdkApi->IsConnectedNow();
	//return TRUE;
}

MUMBLE_API BOOL MUMBLE_DoJoinChannel(IN LPVOID pClientInstance,
									 IN const TTCHAR* lpszChannelPath, 
									 IN const TTCHAR* lpszPassword, 
									 IN const TTCHAR* lpszTopic,
									 IN const TTCHAR* lpszOpPassword) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	Channel* rootChannel = Channel::get(0);
	Channel* joinChannel = NULL;
	if (QString::fromWCharArray(lpszChannelPath) == QLatin1String(""))
		return FALSE;	
	if (rootChannel) {
		foreach(Channel* channel, Channel::c_qhChannels) {
			if (channel->cParent == rootChannel) {
				if (channel->qsName == QString::fromWCharArray(lpszChannelPath)) {
					joinChannel = channel;
					break;
				}
			}
		}
	}
	else
		return FALSE;

	//joinChannel = rootChannel;
	if (joinChannel == NULL) {
		g.sh->createChannel(0, QString::fromWCharArray(lpszChannelPath), QString(), 0, true);
	}
	else {
		g.sh->joinChannel(joinChannel->iId);
	}

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_IsSoundSystemInitialized(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	return sdkApi->GetInitialized();
}

MUMBLE_API BOOL MUMBLE_InitVoIP(IN HWND hWnd, IN BOOL bLLMouseHook, OUT LPVOID* pNewClientInstance, IN SoundSystem nSoundSystem) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	
	if (pNewClientInstance)
		*pNewClientInstance = new char[1];
	if (MUMBLE_IsSoundSystemInitialized(*pNewClientInstance))
		return TRUE;

	Global::g_global_struct = new Global();

	ResetEvent(g.hEvent);
	ResetEvent(g.hInitWASAPIInputEvent);
	ResetEvent(g.hInitWASAPIOutputEvent);

	sdkApi->GetAudioThread()->start(QThread::HighPriority);
	//sdkApi->GetAudioThread()->run();
	//sdkApi->GetAudioThread()->run();
	//ожидаем инициализации
	WaitForSingleObject(g.hEvent, INFINITE);
	Audio::start();
	if (nSoundSystem == SOUNDSYSTEM_WASAPI) {
		WaitForSingleObject(g.hInitWASAPIInputEvent, INFINITE);
		WaitForSingleObject(g.hInitWASAPIOutputEvent, INFINITE);

	}
	
	g.hCallbackWnd = hWnd;

	sdkApi->SetInitialized(true);

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_CloseVoIP(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	if (pClientInstance) {
		delete[] pClientInstance;
		pClientInstance = NULL;
	}
	
	sdkApi->SetInitialized(false);

	g.pTalkingCallback = NULL;
	g.pCallbackUserData = NULL;
	
	return FALSE;
}

MUMBLE_API BOOL MUMBLE_DoLeaveChannel(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	return FALSE;
}
/*
MUMBLE_API INT32 MUMBLE_GetCurrentInputLevel(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	return FALSE;
}*/

MUMBLE_API INT32 MUMBLE_GetInputDevicesCount(IN LPVOID pClientInstance, 
											 IN SoundSystem nSoundSystem) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	QString soundSystem;
	if (nSoundSystem == SOUNDSYSTEM_WASAPI)
		soundSystem = QLatin1String("WASAPI");
	else if (nSoundSystem == SOUNDSYSTEM_DSOUND)
		soundSystem = QLatin1String("DirectSound");
	else {
		return 0;
	}

	QList<audioDevice> ql;
	if (AudioInputRegistrar::qmNew) {
		AudioInputRegistrar *air = AudioInputRegistrar::qmNew->value(soundSystem);
		if (!air)
			return 0;
		ql = air->getDeviceChoices( true);

		int idx = 0;
		bool findDev = false;
		return ql.count();
	}

	return 0;
}

MUMBLE_API BOOL MUMBLE_StartFindRealMic(IN LPVOID pClientInstance, IN SoundSystem nSoundSystem) 
{
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	QString soundSystem;
	if (nSoundSystem == SOUNDSYSTEM_WASAPI)
		soundSystem = QLatin1String("WASAPI");
	else if (nSoundSystem == SOUNDSYSTEM_DSOUND)
		soundSystem = QLatin1String("DirectSound");
	else {
		return FALSE;
	}

	SoundDevice* soundDev = NULL;
	QList<audioDevice> ql;
	if (AudioInputRegistrar::qmNew) {
		AudioInputRegistrar *air = AudioInputRegistrar::qmNew->value(soundSystem);
		if (!air)
			return FALSE;
		ql = air->getDeviceChoices( false);

		int idx = 0;
		bool findDev = false;
		sdkApi->StopAndClearFindingDev();

		Audio::stop();

		std::vector<AudioInput*> allStartedDevices;
		g.s.bMute = true;

		qWarning("MUMBLE_StartFindRealMic");

		foreach(audioDevice d, ql) {
			if (d.first == QLatin1String("Default Device"))
				continue;
			
			g.s.qsHumanInput = d.first;
			
			AudioInput* audioInput = air->create(d.first, d.second);
			if (!audioInput)
				continue;
			if (nSoundSystem == SOUNDSYSTEM_DSOUND) {
				DXAudioInput* dxAudioInput = (DXAudioInput*)audioInput;

				wchar_t* wDeviceName = new wchar_t[d.first.length()+1];
				d.first.toWCharArray(wDeviceName);
				std::wstring wsDeviceName = wDeviceName;
				delete[] wDeviceName;

				std::vector<std::wstring> micLines = dxAudioInput->GetMicLinesOnDevice(wDeviceName);

				/*QString guid = d.second.toString();
				wchar_t* wDeviceGuid = new wchar_t[guid.length()+1];
				guid.toWCharArray(wDeviceGuid);												
				std::wstring wsDeviceGuid = wDeviceGuid;
				delete[] wDeviceGuid;*/

				for (unsigned int i=0; i < micLines.size(); i++) {
					dxAudioInput->SetEnabledInDeviceLine(wsDeviceName, micLines[i], TRUE);
					break; 
				}
				if (micLines.size() > 1)
					qWarning("!!! mic lines not one");
				
			}

			audioInput->setFindMicMode();
			audioInput->start(QThread::HighestPriority);
			sdkApi->AddFindingMicDev(audioInput);

		}
	}

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_EndFindRealMic(IN LPVOID pClientInstance, OUT TTCHAR asFindedDevice[MUMBLE_STRLEN], OUT TTCHAR asFindedDeviceName[MUMBLE_STRLEN], OUT TTCHAR asFindedDeviceLine[MUMBLE_STRLEN])
{
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	FindedDeviceInfo deviceAndLine = sdkApi->GetFindedMicInd();
	QString resultGuid = deviceAndLine.deviceGuid;
	QString resultName = deviceAndLine.deviceName;
	QString resultLine = deviceAndLine.deviceLineName;


	if( resultGuid.size() >= MUMBLE_STRLEN)
		resultGuid.resize( MUMBLE_STRLEN-1);
	resultGuid.toWCharArray(asFindedDevice);
	asFindedDevice[MUMBLE_STRLEN-1] = 0;

	if( resultName.size() >= MUMBLE_STRLEN)
		resultName.resize( MUMBLE_STRLEN-1);
	resultName.toWCharArray(asFindedDeviceName);
	asFindedDeviceName[MUMBLE_STRLEN-1] = 0;

	if( resultLine.size() >= MUMBLE_STRLEN)
		resultLine.resize( MUMBLE_STRLEN-1);
	resultLine.toWCharArray(asFindedDeviceLine);
	asFindedDeviceLine[MUMBLE_STRLEN-1] = 0;

	sdkApi->StopAndClearFindingDev();

	g.s.bMute = false;

	Audio::stop();
	Audio::start();

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_GetInputDevice(IN LPVOID pClientInstance, 
									  IN SoundSystem nSoundSystem, 
									  IN INT32 nIndex, 
									  OUT SoundDevice* pSoundDevice) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	QString soundSystem;
	if (nSoundSystem == SOUNDSYSTEM_WASAPI)
		soundSystem = QLatin1String("WASAPI");
	else if (nSoundSystem == SOUNDSYSTEM_DSOUND)
		soundSystem = QLatin1String("DirectSound");
	else {
		return FALSE;
	}
	SoundDevice* soundDev = NULL;
	QList<audioDevice> ql;
	if (AudioInputRegistrar::qmNew) {
		AudioInputRegistrar *air = AudioInputRegistrar::qmNew->value(soundSystem);
		if (!air)
			return FALSE;
		ql = air->getDeviceChoices( false);

		int idx = 0;
		bool findDev = false;
		//ищем устройство по выбранному индексу
		foreach(audioDevice d, ql) {
			if (idx == nIndex) {
				soundDev = pSoundDevice;
				soundDev->nDeviceID = nIndex;
				soundDev->nSoundSystem = nSoundSystem;
				d.first.toWCharArray( soundDev->szDeviceName);
				soundDev->szDeviceName[d.first.length()] = 0;
				QString qstr = d.second.toString();
				int pos = qstr.lastIndexOf('{');
				if( pos > 0)
				{
					qstr = qstr.mid( pos);
				}
				qstr.toWCharArray( soundDev->sGUID);				
				soundDev->sGUID[qstr.length()] = 0;
				soundDev->bSupports3D = false;
				soundDev->nMaxInputChannels = 1;
				soundDev->nMaxOutputChannels = 1;
				//g.l->log(Log::UserLeave, d.first);
				break;
			}
			++idx;
		}
	}

	return TRUE;
}

MUMBLE_API INT32 MUMBLE_GetOutputDevicesCount(IN LPVOID pClientInstance, 
											  IN SoundSystem nSoundSystem) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	QString soundSystem;
	if (nSoundSystem == SOUNDSYSTEM_WASAPI)
		soundSystem = QLatin1String("WASAPI");
	else if (nSoundSystem == SOUNDSYSTEM_DSOUND)
		soundSystem = QLatin1String("DirectSound");
	else {
		return 0;
	}

	QList<audioDevice> ql;
	if (AudioOutputRegistrar::qmNew) {
		AudioOutputRegistrar *aor = AudioOutputRegistrar::qmNew->value(soundSystem);
		if (!aor)
			return 0;
		ql = aor->getDeviceChoices( true);

		int idx = 0;
		bool findDev = false;
		return ql.count();
	}

	return 0;
}

MUMBLE_API BOOL MUMBLE_GetOutputDevice(IN LPVOID pClientInstance, 
									   IN SoundSystem nSoundSystem, 
									   IN INT32 nIndex, 
									   OUT SoundDevice* pSoundDevice) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	
	QString soundSystem;
	if (nSoundSystem == SOUNDSYSTEM_WASAPI)
		soundSystem = QLatin1String("WASAPI");
	else if (nSoundSystem == SOUNDSYSTEM_DSOUND)
		soundSystem = QLatin1String("DirectSound");
	else {
		return FALSE;
	}
	SoundDevice* soundDev = NULL;
	QList<audioDevice> ql;
	if (AudioOutputRegistrar::qmNew) {
		AudioOutputRegistrar *aor = AudioOutputRegistrar::qmNew->value(soundSystem);
		if (!aor)
			return FALSE;
		ql = aor->getDeviceChoices( false);

		int idx = 0;
		bool findDev = false;
		//ищем устройство по выбранному индексу
		foreach(audioDevice d, ql) {
			if (idx == nIndex) {
				soundDev = pSoundDevice;
				soundDev->nDeviceID = nIndex;
				soundDev->nSoundSystem = nSoundSystem;
				d.first.toWCharArray(soundDev->szDeviceName);
				soundDev->szDeviceName[d.first.length()] = 0;
				QString qstr = d.second.toString();
				int pos = qstr.lastIndexOf('{');
				if( pos > 0)
				{
					qstr = qstr.mid( pos);
				}								
				qstr.toWCharArray( soundDev->sGUID);
				soundDev->sGUID[qstr.length()] = 0;
				soundDev->bSupports3D = false;
				soundDev->nMaxInputChannels = 1;
				soundDev->nMaxOutputChannels = 1;

				break;
			}
			++idx;
		}
	}

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_GetChannelPath(IN LPVOID pClientInstance,
									  IN INT32 nChannelID, 
									  OUT TTCHAR szChannelPath[MUMBLE_STRLEN]) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	Channel* rootChannel = Channel::get(0);
	Channel* joinChannel = NULL;
	if (rootChannel) {
		foreach(Channel* channel, Channel::c_qhChannels) {
			if (channel->iId == nChannelID) {
				channel->qsName.toWCharArray(szChannelPath);
				return TRUE;
			}	
		}
	}

	return FALSE;
}

MUMBLE_API BOOL MUMBLE_IsConnected(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	return TRUE;
}

MUMBLE_API BOOL MUMBLE_Disconnect(IN LPVOID pClientInstance) {
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	HANDLE hEvent;

	hEvent = CreateEvent( 
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		L"ServerHandlerThreadEnd"  // object name
		); 

	g.mw->disconnectFromServer();

	//ожидаем обработки потоком завершения
	WaitForSingleObject(hEvent, 10000);

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_SetUserGainLevel(IN LPVOID pClientInstance,
										IN INT32 nUserID, IN INT32 nGainLevel){

	//no support for every user volume
	return FALSE;
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
	//nLevel - 100 .. 8000
	int normLevel = 0;
	if (nGainLevel <= 1000)
		normLevel = ceil((float)(nGainLevel*128)/1000);
	else
		normLevel = ceil((float)(nGainLevel*128)/8000 + 128);
	if (normLevel > 255)
		normLevel = 255;
	if (normLevel <= 0)
		normLevel = 1;

	return MUMBLE_SetMasterVolume(pClientInstance, normLevel);
}

MUMBLE_API BOOL MUMBLE_GetUser(IN LPVOID pClientInstance,
							   IN INT32 nUserID, OUT api::User* lpUser){
	CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();

	ClientUser* clUser = ClientUser::get(nUserID);

	if (!clUser)
		return FALSE;

	api::User* resultUser = lpUser;

	//resultUser->nUserID = clUser->iId;
	resultUser->nUserID = nUserID;
	//qWarning() << "MUMBLE_GetUser::userID==" << nUserID << "clUser->iId==" << clUser->iId;
	resultUser->szNickName[0] = NULL;
	clUser->qsName.toWCharArray(resultUser->szNickName);
//	INT32 nStatusMode;
	resultUser->szStatusMsg[0] = NULL;
	if (clUser->cChannel)
		resultUser->nChannelID = clUser->cChannel->iId;
	
	resultUser->szIPAddress[0] = NULL;
	resultUser->nUdpPort = 0;
	resultUser->szVersion[0] = NULL;
	resultUser->nFrequency = 0;
	resultUser->nPacketsLost = 0;
	resultUser->nPacketsCount = 0;
	resultUser->nPlayingPacket = 0;
	resultUser->bForwarding = false;
	resultUser->bTalking = clUser->tsState == Settings::Talking;
	resultUser->bAdmin = false;

	clUser->removeRef();

	return TRUE;
}

MUMBLE_API BOOL MUMBLE_RegisterTalkingCallback(IN LPVOID pClientInstance,
                                                     IN TalkingCallback* talkCallback, 
													 IN PVOID pUserData) {

	g.pTalkingCallback = talkCallback;
	g.pCallbackUserData = pUserData;
	return TRUE;
}

MUMBLE_API INT32 MUMBLE_GetVoiceGainLevel(IN LPVOID pClientInstance) {
	return g.s.iMinLoudness;
}
MUMBLE_API BOOL MUMBLE_GetAGCSettings(IN LPVOID pClientInstance, 
									  IN PINT32 lpnGainLevel, 
									  OUT PINT32 lpnMaxIncrement,
									  OUT PINT32 lpnMaxDecrement,
									  OUT PINT32 lpnMaxGain){
	return FALSE;
}
MUMBLE_API INT32 MUMBLE_GetMasterVolume(IN LPVOID pClientInstance) {
	return g.s.fVolume * 64.0f;
}
MUMBLE_API INT32 MUMBLE_GetVoiceActivationLevel(IN LPVOID pClientInstance) {
	return g.s.fVADmax*32767.0f;
}
MUMBLE_API INT32 MUMBLE_GetEncoderComplexity(IN LPVOID pClientInstance) {
	return 0;
}
MUMBLE_API BOOL MUMBLE_EnableAGC(IN LPVOID pClientInstance, 
								 IN BOOL bEnable){
	return FALSE;
}

MUMBLE_API BOOL MUMBLE_IsForwardingToAll(IN LPVOID pClientInstance) {
	return FALSE;
}

MUMBLE_API DOUBLE MUMBLE_GetCurrentInputLevel(IN LPVOID pClientInstance, IN CurrentInputLevelCalculation nInputLevelCalculation) {
	AudioInputPtr ai = g.ai;

	if (ai.get() == NULL || ! ai->getSppPreprocess())
		return 0;

	DOUBLE currentValue = 0;
	//amplitude
	if (nInputLevelCalculation == AMPLITUDE)
		currentValue = iroundf((32767.f/96.0f) * (96.0f + ai->dPeakCleanMic) + 0.5f);
	else if (nInputLevelCalculation == SIGNAL_TO_NOISE)
		currentValue = iroundf(ai->fSpeechProb * 32767.0f + 0.5f);
	else if (nInputLevelCalculation == VACADEM)
		currentValue = ai->getVacademInputLevel()*32767.0f;
	return currentValue;
}

MUMBLE_API void MUMBLE_StartInputFromDataStream()
{
	qWarning("MUMBLE_StartInputFromDataStream");
	//начинаем капчить звук с буфера (MUMBLE_SetDataForDataStream - установка данных в буфер извне)
	g.s.iJitterBufferSize = 180;

	Audio::stop();
	Audio::start(QLatin1String("SoundStream"), QLatin1String("SoundStream"));
}

MUMBLE_API BOOL MUMBLE_SetCommandForDataStream(IN INT32 aCommandID, IN const TTCHAR* lpszStreamID)
{
	g.streamSound_lock();
	std::vector<StreamSound*> copyOfStreamSoundBuffers = g.pStreamSoundBuffers;
	g.streamSound_unlock();

	QString streamID = QString::fromWCharArray(lpszStreamID);
	qWarning("MUMBLE_SetCommandForDataStream");
	qWarning() << "my = " << streamID;
	for (unsigned int i=0; i < copyOfStreamSoundBuffers.size(); i++)
	{
		qWarning() << copyOfStreamSoundBuffers[i]->GetStreamID();
		if (copyOfStreamSoundBuffers[i]->GetStreamID() == streamID)
		{
			BOOL res = copyOfStreamSoundBuffers[i]->ProcessCommand(aCommandID);
			return res;
		}
	}
	qWarning("not found sound stream");
	return FALSE;
}

MUMBLE_API BOOL MUMBLE_SetDataForDataStream(IN LPVOID pData, IN INT32 aDataSize, IN const TTCHAR* lpszStreamID)
{
	g.streamSound_lock();
	std::vector<StreamSound*> copyOfStreamSoundBuffers = g.pStreamSoundBuffers;
	g.streamSound_unlock();

	QString streamID = QString::fromWCharArray(lpszStreamID);

	for (unsigned int i=0; i < copyOfStreamSoundBuffers.size(); i++)
	{
		if (copyOfStreamSoundBuffers[i]->GetStreamID() == streamID)
		{
			BOOL res = copyOfStreamSoundBuffers[i]->AddData(pData, aDataSize);
			return res;
		}
	}
	
	return FALSE;
}

MUMBLE_API void MUMBLE_CreateDataStream(IN const TTCHAR* lpszStreamID)
{
	g.streamSound_lock();
	QString streamID = QString::fromWCharArray(lpszStreamID);

	for (unsigned int i=0; i < g.pStreamSoundBuffers.size(); i++)
	{
		if (g.pStreamSoundBuffers[i]->GetStreamID() == streamID)
		{
			g.pStreamSoundBuffers[i]->StartPreload();
			g.streamSound_unlock();
			return;
		}
	}

	qWarning((QString(QLatin1String("MUMBLE_CreateDataStream - create new")+streamID)).toStdString().c_str());
	StreamSound* stream = new StreamSound(streamID);
	g.pStreamSoundBuffers.push_back(stream);
	g.streamSound_unlock();
}

MUMBLE_API void MUMBLE_RemoveDataStream(IN const TTCHAR* lpszStreamID)
{
	g.streamSound_lock();
	std::vector<StreamSound*> copyOfStreamSoundBuffers = g.pStreamSoundBuffers;
	g.streamSound_unlock();

	QString streamID = QString::fromWCharArray(lpszStreamID);
	std::vector<StreamSound*>::iterator it;
	for (it=copyOfStreamSoundBuffers.begin(); it!=copyOfStreamSoundBuffers.end(); it++)
	{
		if (((StreamSound*)(*it))->GetStreamID() == streamID)
		{
			//создает утечку, но утечка памяти под класс незначительна 
			//зато позволяет избежать локов на доступ к стримам
			((StreamSound*)(*it))->Clear(); 
			qWarning("MUMBLE_RemoveDataStream");
			//delete *it;
			//g.pStreamSoundBuffers.erase(it);
			break;
		}
	}
}

MUMBLE_API void MUMBLE_RegisterOnConnectSuccessCallback(IN LPVOID pClientInstance, 
														IN EmptyCallback* aCallback)
{
	g.pOnConnectSuccessCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnConnectFailedCallback(IN LPVOID pClientInstance, 
													   IN EmptyCallback* aCallback)
{
	g.pOnConnectFailedCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnConnectionLostCallback(IN LPVOID pClientInstance, 
														IN EmptyCallback* aCallback)
{
	g.pOnConnectionLostCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnInvalideChannelNameCallback(IN LPVOID pClientInstance, 
															 IN EmptyCallback* aCallback)
{
	g.pOnInvalideChannelNameCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnAcceptedCallback(IN LPVOID pClientInstance, 
												  IN IntParamCallback* aCallback)
{
	g.pOnAcceptedCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnUserTalkingCallback(IN LPVOID pClientInstance, 
													 IN IntParamCallback* aCallback)
{
	g.pOnUserTalkingCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnUserStoppedTalkingCallback(IN LPVOID pClientInstance, 
													 IN IntParamCallback* aCallback)
{
	g.pOnUserStoppedTalkingCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnJoinedChannelCallback(IN LPVOID pClientInstance, 
													   IN IntParamCallback* aCallback)
{
	g.pOnJoinedChannelCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnLeftChannelCallback(IN LPVOID pClientInstance, 
													 IN IntParamCallback* aCallback)
{
	g.pOnLeftChannelCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnAddUserCallback(IN LPVOID pClientInstance, 
												 IN TwoIntParamCallback* aCallback)
{
	g.pOnAddUserCallback = aCallback;
}

MUMBLE_API void MUMBLE_RegisterOnRemoveUserCallback(IN LPVOID pClientInstance, 
												 IN TwoIntParamCallback* aCallback)
{
	g.pOnRemoveUserCallback = aCallback;
}

MUMBLE_API BOOL MUMBLE_RegisterTalkingCallback2(IN LPVOID pClientInstance,
												IN TalkingCallback2* talkCallback, 
												IN PVOID pUserData)
{
	g.pTalkingCallback2 = talkCallback;
	return 1;
}

MUMBLE_API int MUMBLE_GetUserChannelInfo(IN INT32 anUserID, OUT TTCHAR asLog[MUMBLE_LOG_STRLEN], IN INT32 aDataSize, OUT BOOL &aHangAudioThreadDetected)
{
	ClientUser* clUser = ClientUser::get(anUserID);
	QString sLog = QLatin1String("no channel");

	if (!( clUser == NULL || clUser->cChannel == NULL))
	{
		sLog = clUser->cChannel->GetUserChannelInfo();
		clUser->removeRef();
	}

	aHangAudioThreadDetected = false;
	g.sh_stop_lock();
	if(g.ai.get() != NULL && g.ao.get() != NULL)
	{
		QString inputDev, outputDev;
		if (g.s.qsAudioInput == QLatin1String("WASAPI"))
		{
			inputDev = QLatin1String("WASAPI '") + g.s.qsHumanInput;
		}
		else if (g.s.qsAudioInput == QLatin1String("DirectSound"))
		{
			inputDev = QLatin1String("DS '") + g.s.qsHumanInput;
		}

		if (g.s.qsAudioOutput == QLatin1String("WASAPI"))
		{
			outputDev = QLatin1String("WASAPI '") + g.s.qsHumanOutput;
		}
		else if (g.s.qsAudioOutput == QLatin1String("DirectSound"))
		{
			outputDev = QLatin1String("DS '") + g.s.qsHumanOutput;
		}

		bool bNotHangI = g.ai->getNotHungFlag();
		bool bNotHangO = g.ao->getNotHungFlag();
		g.ai->clearNotHungFlag();
		g.ao->clearNotHungFlag();
		if (!bNotHangI || !bNotHangO)
			aHangAudioThreadDetected = true;
		QString sLogI = inputDev + QLatin1String("' AudioIn working is ");
		sLogI += bNotHangI ? QLatin1String("true") : QLatin1String("false");	sLogI += QLatin1String("\n");
		QString sLogO = outputDev + QLatin1String("' AudioOut working is ");
		sLogO += bNotHangO ? QLatin1String("true") : QLatin1String("false");	sLogO += QLatin1String(" ");
		sLogO += QLatin1String("mix is ");
		sLogO += g.ao->getSuccessMixFlag() ? QLatin1String("true") : QLatin1String("false");	sLogO += QLatin1String("\n");
		sLog.insert(0, sLogI);
		sLog.insert(0, sLogO);
	}
	g.sh_stop_unlock();
	if (sLog.size()<1)
		return -1;

	if( sLog.size() > aDataSize)
		sLog.resize( aDataSize-1);

	asLog[MUMBLE_LOG_STRLEN-1] = 0;
	sLog.toWCharArray(asLog);
	return sLog.size();
}

}