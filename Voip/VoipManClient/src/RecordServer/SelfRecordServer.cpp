// VoipMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "serverInfo.h"
#include "../../include/RecordServer/SelfRecordServer.h"
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
CSelfRecordServerConnection::CSelfRecordServerConnection(oms::omsContext* aContext, cs::imessagehandler* apHandler, ivoiceconnectionlistender* apConnectionListener)
		: CRecordServerConnectionBase( aContext, VSST_SELF_RECORDING, apHandler, apConnectionListener)
{
	recordingInProgress = false;
}

CSelfRecordServerConnection::~CSelfRecordServerConnection()
{
	//??
}

//! Пишется ли сейчас звук
bool CSelfRecordServerConnection::IsRecordingInProgress()
{
	return recordingInProgress;
}

//! 
bool CSelfRecordServerConnection::StartRecordingBase( LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName, LPCSTR alpcUserName, unsigned int aRecordID)
{
	if( !serverConnection->IsConnected())
		return false;

	voipServerIP = alpcVoipServerAddress;
	roomName = alpcRoomName;
	recordID = aRecordID;
	userName = alpcUserName;

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: CSelfRecordServerConnection::StartRecordingBase()::Record ID=%u", recordID);
		context->mpLogWriter->WriteLnLPCSTR( str.GetBuffer(), ltDefault);
	}

	recordingInProgress = true;
	return true;
}

bool CSelfRecordServerConnection::PrepareSelfRecording( LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName, LPCSTR alpcUserName)
{
	if( !StartRecordingBase(alpcVoipServerAddress, alpcRoomName, alpcUserName, -1))
		return false;

	USES_CONVERSION;
	CStartRecordingQueryOut	queryOut( alpcVoipServerAddress, A2W(alpcRoomName), GenerateNextToken());

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: Before send ST_StartSelfRecording");
		context->mpLogWriter->WriteLPCSTR( str.GetBuffer(), ltDefault);
	}
	serverConnection->Send( ST_StartSelfRecording, queryOut.GetData(), queryOut.GetDataSize());
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: After send ST_StartSelfRecording");
		context->mpLogWriter->WriteLPCSTR( str.GetBuffer(), ltDefault);
	}

	return true;
}

bool CSelfRecordServerConnection::ResumeSelfRecording( )
{
	USES_CONVERSION;
	CStartRecordingQueryOut	queryOut( voipServerIP.GetBuffer(), A2W(roomName.GetBuffer()), -1);
	serverConnection->Send( ST_ResumeSelfRecording, queryOut.GetData(), queryOut.GetDataSize());

	return true;
}

//! Останавливает запись of self voice
bool CSelfRecordServerConnection::PauseSelfRecording( )
{
	if( !serverConnection->IsConnected())
		return false;

	if( !IsRecordingInProgress())
		return false;

	CCommonVoipPacketOut	queryOut;
	serverConnection->Send( ST_PauseSelfRecording, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

//! Воспроизводит запись of self voice
bool CSelfRecordServerConnection::PlaySelfRecording( )
{
	if( !serverConnection->IsConnected())
		return false;

	if( !IsRecordingInProgress())
		return false;

	CCommonVoipPacketOut	queryOut;
	serverConnection->Send( ST_PlaySelfRecording, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

//! Заканчивает запись общения для текущей группы
bool CSelfRecordServerConnection::StopSelfRecording()
{
	recordingInProgress = false;
	if( !serverConnection->IsConnected())
		return false;

	ATLTRACE(" ST_RecordStop WAS SEND FOR: %u \n", recordID);
	CRecordIDQueryOut	queryOut( recordID);
	voipServerIP = "";
	roomName = "";
	recordID = 0;
	serverConnection->Send( ST_RecordStop, queryOut.GetData(), queryOut.GetDataSize());
	return true;
}

void CSelfRecordServerConnection::OnConnectLost()
{
	if( connectionListener)
		connectionListener->OnSelfRecordingConnectionLost();
}

void CSelfRecordServerConnection::OnConnectRestored()
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: CSelfRecordServerConnection::OnConnectRestored()");
		context->mpLogWriter->WriteLnLPCSTR( str.GetBuffer(), ltDefault);
	}

	if( IsRecordingInProgress())
	{
		// ?? StartRecording( voipServerIP.GetBuffer(), roomName.GetBuffer(), userName.GetBuffer(), recordID);
	}

	if( connectionListener)
		connectionListener->OnSelfRecordingConnectionRestored();
}

unsigned int CSelfRecordServerConnection::GenerateNextToken()
{
	unsigned int newToken = 0;

	while( newToken == sessionToken)
		newToken = (unsigned int)(65535 * rand() / RAND_MAX);

	sessionToken = newToken;

	return sessionToken;
}