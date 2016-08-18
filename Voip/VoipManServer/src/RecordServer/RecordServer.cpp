// VoipMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "serverInfo.h"
#include "../../include/RecordServer/RecordServer.h"
#include "../../include/RecordServer/Protocol/StartRecordingQueryOut.h"
#include "../../include/RecordServer/Protocol/FileCreatedQueryIn.h"
#include "../../include/RecordServer/Protocol/DeleteRecordQueryOut.h"
#include "../../include/RecordServer/Protocol/CopyAudioFileQueryOut.h"
#include "../../include/RecordServer/Protocol/DeleteRecordsQueryOut.h"
#include "ILogWriter.h"
#include "../messagesStorage.h"
#include "../../include/AsyncConnection.h"

/************************************************************************/
// Функции записи звука
/************************************************************************/
CRecordServerConnection::CRecordServerConnection(oms::omsContext* aContext, cs::imessagehandler* apHandler, ivoiceconnectionlistender* apConnectionListener)
		: CRecordServerConnectionBase( aContext, VSST_RECORDING, apHandler, apConnectionListener)
{
	recordingInProgress = false;
	recordListeningInProgress = false;
	recordID = 0;
	listenRecordID = 0;
}

CRecordServerConnection::~CRecordServerConnection()
{
	//??
}

//! Подключается к записывающему серверу
void CRecordServerConnection::OnAfterConnected()
{
	if( recordListeningInProgress)
	{
		unsigned int temp = listenRecordID;
		StopListenRecording( listenRecordID);
		StartListenRecording( temp);
	}
}

//! Пишется ли сейчас звук
bool CRecordServerConnection::IsRecordingInProgress()
{
	return recordingInProgress;
}

bool CRecordServerConnection::IsRecordListeningInProgress()
{
	return recordListeningInProgress;
}

//! Начинает получать событие начала и окончания записи звука
bool CRecordServerConnection::StartListenRecording( unsigned int aRecordID)
{
	ATLASSERT( listenRecordID == 0);
	listenRecordID = aRecordID;
	recordListeningInProgress = true;

	CRecordIDQueryOut	queryOut( listenRecordID);
	serverConnection.Send( ST_JoinToRecord, queryOut.GetData(), queryOut.GetDataSize());

	return true;
}

//! Заканчивает получать событие начала и окончания записи звука
bool CRecordServerConnection::StopListenRecording( unsigned int aRecordID)
{
	ATLASSERT( listenRecordID != 0);
	if( aRecordID == listenRecordID)
	{
		listenRecordID = 0;
		recordListeningInProgress = false;
	}
	return true;
}

//! 
bool CRecordServerConnection::StartRecordingBase( LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName, LPCSTR alpcUserName, unsigned int aRecordID)
{
	if( !serverConnection.IsConnected())
		return false;

	voipServerIP = alpcVoipServerAddress;
	roomName = alpcRoomName;
	recordID = aRecordID;
	userName = alpcUserName;

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: StartRecordingBase()::Record ID=%u", recordID);
		context->mpLogWriter->WriteLPCSTR( str.GetBuffer(), ltDefault);
	}

	recordingInProgress = true;
}

//! Начинает запись общения для текущей группы
bool CRecordServerConnection::StartRecording( LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName, LPCSTR alpcUserName, unsigned int aRecordID)
{
	if( !StartRecordingBase(alpcVoipServerAddress, alpcRoomName, alpcUserName, aRecordID))
		return false;

	USES_CONVERSION;
	CStartRecordingQueryOut	queryOut( alpcVoipServerAddress, A2W(alpcRoomName), aRecordID);
	serverConnection.Send( ST_RecordStart, queryOut.GetData(), queryOut.GetDataSize());

	CSetUserNameQueryOut	nameQueryOut( alpcUserName);
	serverConnection.Send( ST_SetUserName, nameQueryOut.GetData(), nameQueryOut.GetDataSize());

	return true;
}

bool CRecordServerConnection::PauseRecording()
{
	if( !serverConnection.IsConnected())
		return false;

	CRecordIDQueryOut	queryOut( recordID);
	serverConnection.Send( ST_RecordPause, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

bool CRecordServerConnection::ResumeRecording(LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName)
{
	if( !serverConnection.IsConnected())
		return false;

	voipServerIP = alpcVoipServerAddress;
	roomName = alpcRoomName;

	USES_CONVERSION;
	CChangeRecordServerQueryOut	queryOut( alpcVoipServerAddress, A2W(alpcRoomName), recordID);
	serverConnection.Send( ST_ChangeServer, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

//! Посылает событие синхронизации
bool CRecordServerConnection::ForceSynchronization( int uiUserID)
{
	unsigned int queryRecordID = 0;
	if( IsRecordingInProgress())
		queryRecordID = recordID;
	else	if( IsRecordListeningInProgress())
		queryRecordID = listenRecordID;
	else
		return false;

	CForceSynchronizationQueryOut	queryOut( queryRecordID, uiUserID);
	serverConnection.Send( ST_ForceSynchronization, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

//! Заканчивает запись общения для текущей группы
bool CRecordServerConnection::StopRecording()
{
	recordingInProgress = false;
	if( !serverConnection.IsConnected())
		return false;

	ATLTRACE(" ST_RecordStop WAS SEND FOR: %u \n", recordID);
	CRecordIDQueryOut	queryOut( recordID);
	voipServerIP = "";
	roomName = "";
	recordID = 0;
	serverConnection.Send( ST_RecordStop, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

bool CRecordServerConnection::DeleteRecord(unsigned int aRecordID)
{
	if( !serverConnection.IsConnected())
		return false;

	CDeleteRecordQueryOut	queryOut( aRecordID);
	serverConnection.Send( ST_RecordDelete, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

bool CRecordServerConnection::DeleteRecords( unsigned int *pRecIDs, unsigned int count)
{
	if( !serverConnection.IsConnected())
		return false;

	CDeleteRecordsQueryOut deleteRecords( pRecIDs, count);
	serverConnection.Send( ST_RecordDeletes, deleteRecords.GetData(), deleteRecords.GetDataSize());
	return true;
}

bool CRecordServerConnection::CopyAudioFile( LPCTSTR alpcFileName, unsigned int auSourceRecordID)
{
	if( !serverConnection.IsConnected())
		return false;

	CCopyAudioFileQueryOut	queryOut( alpcFileName, auSourceRecordID, GetRecordID());
	serverConnection.Send( ST_CopyAudioFile, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

void CRecordServerConnection::OnConnectLost()
{
	if( IsRecordingInProgress())
	{	
		if( connectionListener)
			connectionListener->OnRecordConnectionLost(ERROR_CONNECTION_FAILED);
	}
}

void CRecordServerConnection::OnConnectRestored()
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: CRecordServerConnection::OnConnectRestored()");
		context->mpLogWriter->WriteLPCSTR( str.GetBuffer(), ltDefault);
	}

	if( IsRecordingInProgress())
	{
		StartRecording( voipServerIP.GetBuffer(), roomName.GetBuffer(), userName.GetBuffer(), recordID);
	}
	if( connectionListener)
		connectionListener->OnRecordConnectionRestored();
}
