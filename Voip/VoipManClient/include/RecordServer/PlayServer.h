#pragma once

#include "imessagehandler.h"
#include "iclientsession.h"
#include "../ConnectionListener.h"
#include "serverInfo.h"
#include "../messagetypes.h"
#include "../AsyncConnection.h"
#include "Protocol/PlaySessionConnectedQueryIn.h"
#include "Protocol/AudioFilesInfoQueryIn.h"
#include <map>

class CPlayServerConnection : cs::isessionlistener
{
public:
	CPlayServerConnection(oms::omsContext* aContext, cs::imessagehandler* apHandler, ivoiceconnectionlistender* apConnectionListener);
	~CPlayServerConnection();

	//! Реализация isessionlistener
public:
	void OnConnectLost();
	void OnConnectRestored();

public:
	//! Запоминает параметры последней записи
	virtual bool StartPlayingRecord(unsigned int aRecordID, unsigned int aRealityID, const wchar_t* alpcRoomName);

	//! Останавливает воспроизведение
	virtual bool StopPlayingRecord();

	//! Подключиться к серверу
	voipError	Connect( LPCTSTR alpcServerIP, unsigned int auiServerPort);

	//! Отключается от сервера
	voipError	Disconnect();

	//! Производит переподключение к новому голосовому серверу
	bool SetEvoVOIPServer(LPCSTR alpcVoipServerAddress);

	//! Запускает файл на воспроизведение
	bool PlayAudioFile(LPCSTR alpcUserName, LPCSTR alpcAudioFile, unsigned int aiStartPosition, unsigned int auSyncVersion, LPCSTR alpcCommunicationAreaName);

	//! Выключает все файлы для указанного пользователя
	bool RemoveUserAudio(LPCSTR alpcUserName);

	//! Посылает серверу команду на приостановку воспроизведения речи пользователя
	virtual bool PauseUserAudio( LPCSTR alpUserName);

	//! Посылает серверу команду на продолжение воспроизведения речи пользователя
	virtual bool ResumeUserAudio( LPCSTR alpUserName);

	//! Приостанавливает воспроизведение определенной записи
	void PauseServerAudio();

	//! Продолжает воспроизведение определенной записи
	void ResumeServerAudio();

	//! Перематывает воспроизведение определенной записи
	void RewindServerAudio();

	//! Проигрываем сейчас запись или нет
	bool IsPlayingInProgress()
	{
		return playingInProgress;
	}

	//! Устанавливаем режим коспроизведения звуков: на сервере или на клиенте
	void SetPlayingMode( EPLAYSESIONMODE aePlayMode);

	//! Устанавливает список файлов для воспроизведения
	void SetAudioFilesInfo( CAudioFilesInfoQueryIn::VecAudioFrames& aAudioFrames, unsigned int auRecordID);

	//! Номер записи, которую воспроизводим
	unsigned int GetRecordID()
	{
		return recordID;
	}
	
	//! Номер реальности, в которую воспроизводим
	unsigned int GetRealityID()
	{
		return realityID;
	}

	//! Возвращает список файлов, которые были запущены не ранее чем CHECKTIME_TOOLONG_DURATION секунд назад
	std::vector<std::string>	GetPlayingFiles();

protected:
	void UnregisterUserPlayEvent(LPCSTR alpcUserName);
	void RegisterUserPlayEvent(LPCSTR alpcUserName, LPCSTR alpcFileName, unsigned long auTime);

protected:
	CAsyncConnection* serverConnection;

	//! Адрес текущего голосового сервера воспроизведения
	CComString		voipServerIP;
	//! Имя комнаты, в которую воспроизводим
	MP_WSTRING	roomName;
	//! Номер записи, которую воспроизводим
	unsigned int	recordID;
	//! Номер реальности, в которую воспроизводим
	unsigned int	realityID;
	//! Флаг состояния
	bool	playingInProgress;
	//! Мапа воспроизведения
	//typedef std::map<std::string/*user name*/, std::string/*file name*/>	CMapCurrentAudioPlayings;
	typedef MP_MAP<MP_STRING,MP_STRING> CMapCurrentAudioPlayings;
	typedef CMapCurrentAudioPlayings::iterator			CMapCurrentAudioPlayingsIt;
	CMapCurrentAudioPlayings	mapCurrentAudioPlayings;

	//! Контекст, что тут еще сказать :)
	oms::omsContext* context;

	//! Обработка потери связи
	ivoiceconnectionlistender* connectionListener;

	//! Устанавливаем режим коспроизведения звуков: на сервере или на клиенте
	EPLAYSESIONMODE playMode;

	//typedef std::map<unsigned int, CAudioFilesInfoQueryIn::VecAudioFrames>	MapAudioFrames;
	typedef MP_MAP<int,CAudioFilesInfoQueryIn::VecAudioFrames> MapAudioFrames;
	typedef MapAudioFrames::iterator MapAudioFramesIt;
	MapAudioFrames	mapAudioFrames;
};