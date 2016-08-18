// VoipMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "serverInfo.h"
#include "../../include/RecordServer/PlayServer.h"
#include "../../include/RecordServer/Protocol/StartPlayingQueryOut.h"
#include "../../include/RecordServer/Protocol/PlayAudioFileQueryOut.h"
#include "../../include/RecordServer/Protocol/StartRecordingQueryOut.h"
#include "../../include/RecordServer/Protocol/RemoveUserAudioQueryOut.h"
#include <MMSystem.h>

#define INCORRECT_RECORD_ID	0xFFFFFFFF
/************************************************************************/
// Функции записи звука  
/************************************************************************/
CPlayServerConnection::CPlayServerConnection(oms::omsContext* aContext, cs::imessagehandler* apHandler, ivoiceconnectionlistender* apConnectionListener)
		: serverConnection( VSST_PLAYING/*play mode*/, aContext, apHandler, this)
		, context( aContext)
{
	realityID = INCORRECT_RECORD_ID;
	recordID = INCORRECT_RECORD_ID;
	playingInProgress = false;
	connectionListener = apConnectionListener;
}

CPlayServerConnection::~CPlayServerConnection()
{
	
}

//! Запоминает параметры последней записи
bool CPlayServerConnection::StartPlayingRecord(unsigned int aRecordID, unsigned int aRealityID, const wchar_t* alpcRoomName)
{
	if( playingInProgress)
	{
		// Повторное обращение к StartPlayingRecord без StopPlayingRecord
		ATLASSERT( FALSE);
		return false;
	}
	if( recordID != aRecordID)
	{
		mapCurrentAudioPlayings.clear();
	}

	realityID = aRealityID;
	recordID = aRecordID;
	roomName = alpcRoomName;

	playingInProgress = true;
	return true;
}

//! Останавливает воспроизведение
bool CPlayServerConnection::StopPlayingRecord()
{
	if( !playingInProgress)
	{
		// Нечего останавливать
		ATLASSERT( FALSE);
		return false;
	}

	//! используется класс CStartPlayingQueryOut - так как он подходит
	CStartPlayingQueryOut	queryOut( recordID, realityID);
	serverConnection.Send( ST_PlaySessionStop, queryOut.GetData(), queryOut.GetDataSize());
	
	playingInProgress = false;
	return true;
}

//! Подключается к воспроизводящему серверу
voipError CPlayServerConnection::Connect( LPCTSTR alpcServerIP, unsigned int auiServerPort)
{
	bool savedPlayingInProgress = playingInProgress;

	ATLASSERT( context);
	ATLASSERT( context->mpComMan);

	if( !serverConnection.IsAlreadyConnectedTo(alpcServerIP, auiServerPort))
		serverConnection.AsyncConnect( alpcServerIP, auiServerPort);

	CStartPlayingQueryOut	queryOut( recordID, realityID);
	serverConnection.Send( ST_PlaySessionStart, queryOut.GetData(), queryOut.GetDataSize());

	//! если переподключились в серверу и соединение было закрыто, то восстанавливаем его
	if( !playingInProgress && savedPlayingInProgress)
	{
		StartPlayingRecord( recordID, realityID, roomName.c_str());
		SetEvoVOIPServer( voipServerIP);
	}

	return ERROR_NOERROR;
}

//! Отключается от сервера
voipError CPlayServerConnection::Disconnect()
{
	if( playingInProgress)
		StopPlayingRecord();

	serverConnection.Disconnect();
	return ERROR_NOERROR;
}

//! Производит переподключение к новому голосовому серверу
bool CPlayServerConnection::SetEvoVOIPServer(LPCSTR alpcVoipServerAddress)
{
	voipServerIP = alpcVoipServerAddress;

	if( voipServerIP.IsEmpty())
		return false;

	if( roomName.empty())
		return false;

	USES_CONVERSION;

	CComString sLocationName;
	sLocationName.Format( "%S%u", roomName.c_str(), realityID);
	
	CChangePlayServerQueryOut	queryOut( alpcVoipServerAddress, A2W(sLocationName.GetBuffer()), recordID, realityID);
	serverConnection.Send( ST_ChangePlayServer, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

//! Запускает файл на воспроизведение
bool CPlayServerConnection::PlayAudioFile(LPCSTR alpcUserName, LPCSTR alpcAudioFile, unsigned int aiStartPosition)
{
	CPlayAudioFileQueryOut	queryOut( alpcUserName, alpcAudioFile, aiStartPosition, recordID, realityID);
	serverConnection.Send( ST_PlayUserAudioFile, queryOut.GetData(), queryOut.GetDataSize());

	RegisterUserPlayEvent( alpcUserName, alpcAudioFile, timeGetTime());

	return true;
}

void CPlayServerConnection::UnregisterUserPlayEvent(LPCSTR alpcUserName)
{
	CMapCurrentAudioPlayingsIt it = mapCurrentAudioPlayings.find( alpcUserName);
	if( it != mapCurrentAudioPlayings.end())
		mapCurrentAudioPlayings.erase( it);
}

void CPlayServerConnection::RegisterUserPlayEvent(LPCSTR alpcUserName, LPCSTR alpcFileName, unsigned long auTime)
{
	UnregisterUserPlayEvent( alpcUserName);

	mapCurrentAudioPlayings.insert( CMapCurrentAudioPlayingsIt::value_type( alpcUserName, alpcFileName));
}

//! Возвращает список файлов, которые были запущены не ранее чем CHECKTIME_TOOLONG_DURATION секунд назад
std::vector<std::string>	CPlayServerConnection::GetPlayingFiles()
{
	std::vector<std::string>	playingFiles;

	CMapCurrentAudioPlayingsIt it = mapCurrentAudioPlayings.begin();
	for( ; it != mapCurrentAudioPlayings.end(); it++)
	{
		voip::IVoipCharacter* mpIVoipCharacter = context->mpMapMan->GetIVoipAvatarByName( it->first.c_str());
		if( mpIVoipCharacter != NULL)
		{
			if( mpIVoipCharacter->isSayingAudioFile(it->second.c_str()))
			{
				playingFiles.push_back(it->second);
			}
		}
		it++;
	}

	return playingFiles;
}

//! Выключает все файлы для указанного пользователя
bool CPlayServerConnection::RemoveUserAudio(LPCSTR alpcUserName)
{
	CRemoveUserAudioQueryOut queryOut( alpcUserName, recordID, realityID);
	serverConnection.Send( ST_RemoveUserAudio, queryOut.GetData(), queryOut.GetDataSize());

	UnregisterUserPlayEvent( alpcUserName);

	return true;
}

//! Посылает серверу команду на приостановку воспроизведения речи пользователя
bool CPlayServerConnection::PauseUserAudio( LPCSTR alpUserName)
{
	CPauseResumeUserAudioQueryOut	queryOut( alpUserName, realityID);
	serverConnection.Send( ST_PauseUserAudio, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

//! Посылает серверу команду на продолжение воспроизведения речи пользователя
bool CPlayServerConnection::ResumeUserAudio( LPCSTR alpUserName)
{
	CPauseResumeUserAudioQueryOut	queryOut( alpUserName, realityID);
	serverConnection.Send( ST_ResumeUserAudio, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

//! Приостанавливает воспроизведение определенной записи
void CPlayServerConnection::PauseServerAudio()
{
	CServerAudioCommandQueryOut	queryOut( recordID, realityID, CServerAudioCommandQueryOut::SAS_PAUSE);
	serverConnection.Send( ST_ServerAudioCommand, queryOut.GetData(), queryOut.GetDataSize());
}

//! Продолжает воспроизведение определенной записи
void CPlayServerConnection::ResumeServerAudio()
{
	CServerAudioCommandQueryOut	queryOut( recordID, realityID, CServerAudioCommandQueryOut::SAS_RESUME);
	serverConnection.Send( ST_ServerAudioCommand, queryOut.GetData(), queryOut.GetDataSize());
}

//! Перематывает воспроизведение определенной записи
void CPlayServerConnection::RewindServerAudio()
{
	CServerAudioCommandQueryOut	queryOut( recordID, realityID, CServerAudioCommandQueryOut::SAS_REWIND);
	serverConnection.Send( ST_ServerAudioCommand, queryOut.GetData(), queryOut.GetDataSize());
}

void CPlayServerConnection::OnConnectLost()
{
	if( IsPlayingInProgress())
	{
		if( connectionListener)
			connectionListener->OnPlayConnectionLost();
	}
}

void CPlayServerConnection::OnConnectRestored()
{
	if( IsPlayingInProgress())
	{
		CStartPlayingQueryOut	queryOut( recordID, realityID);
		serverConnection.Send( ST_PlaySessionStart, queryOut.GetData(), queryOut.GetDataSize());

		// Сбросим флаг, чтобы не обработалось двойное воспроизведение
		playingInProgress = false;
		StartPlayingRecord( recordID, realityID, roomName.c_str());
		SetEvoVOIPServer( voipServerIP);
	}
}

void CPlayServerConnection::SetPlayingMode( EPLAYSESIONMODE aePlayMode)
{
	playMode = aePlayMode;
}

//! Устанавливает список файлов для воспроизведения
void CPlayServerConnection::SetAudioFilesInfo( CAudioFilesInfoQueryIn::VecAudioFrames& aAudioFrames, unsigned int auRecordID)
{
	MapAudioFramesIt it = mapAudioFrames.find( auRecordID);
	if( it == mapAudioFrames.end())
	{
		mapAudioFrames.insert( MapAudioFrames::value_type(auRecordID, aAudioFrames));
	}
	if( playingInProgress && playMode == EPM_ONCLIENT && recordID == auRecordID)
	{
		// downloaderAudio.StartDownload( aAudioFrames, auRecordID);
	}
}