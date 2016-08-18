#pragma once
//#include <windows.h>
#include "../mumble/Global.h"


#include "MumbleAPI.h"

struct FindedDeviceInfo{
	QString deviceGuid;
	QString deviceName;
	QString deviceLineName;
};

class AudioMainThread : public QThread {
private:
//	Q_OBJECT
//	Q_DISABLE_COPY(AudioMainThread)
public:
	AudioMainThread();
	~AudioMainThread();
	void run();
};

class CSDKInterface
{
public:
	CSDKInterface(void);
	~CSDKInterface(void);
	static CSDKInterface* sharedSDKInterface();

	void SetUpdServerPort(unsigned short anUdpPort);
	unsigned short GetUpdServerPort();
	void SetServerHost(QString anServerHost);
	QString GetServerHost();
	void SetConnectingChannelName(QString aChannelName);
	void SetInitialized(bool anInit);
	bool GetInitialized();
	AudioMainThread* GetAudioThread();

	void onAddChannel(int id, Channel *p, const QString &name);
	void onConnectedByDoLogin();
	void onDisconnected();

	bool IsConnectedNow();


	void AddFindingMicDev(AudioInput* anAudioIn);
	FindedDeviceInfo GetFindedMicInd();
	void StopAndClearFindingDev();
private:
	unsigned short m_udpPort;
	QString m_serverHost;
	bool m_isInitialized;
	bool m_isConnected;
	QString m_connectChannelName;
	AudioMainThread m_audioMainThread;

	CRITICAL_SECTION m_csConnectChanged;

	std::vector<AudioInput*> m_allStartedDevices;
};



