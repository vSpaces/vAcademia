// VoipMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "serverInfo.h"
#include "../../include/RecordServer/RecordServerBase.h"
#include "ILogWriter.h"
#include "../messagesStorage.h"
#include "../../include/AsyncConnection.h"

/************************************************************************/
// ������� ������ �����  
/************************************************************************/
CRecordServerConnectionBase::CRecordServerConnectionBase(oms::omsContext* aContext, voip::voipServerSessionType aSessionType, cs::imessagehandler* apHandler, ivoiceconnectionlistender* apConnectionListener)
		: context( aContext)
{
	MP_NEW_V4(serverConnection, CAsyncConnection, aSessionType/*record mode*/, aContext, apHandler, this);
	connectionListener = apConnectionListener;
}

CRecordServerConnectionBase::~CRecordServerConnectionBase()
{
	MP_DELETE(serverConnection);
}

//! ������������ � ������������� �������
voipError CRecordServerConnectionBase::Connect( LPCTSTR alpcServerIP, unsigned int auiServerPort)
{
	if( serverConnection->IsAlreadyConnectedTo( alpcServerIP, auiServerPort))
	{
		OnConnectRestored();
		return ERROR_NOERROR;
	}

	ATLASSERT( context);
	ATLASSERT( context->mpComMan);

	serverConnection->AsyncConnect( alpcServerIP, auiServerPort);

	OnAfterConnected();

	return ERROR_NOERROR;
}

//! ����������� �� �������
voipError CRecordServerConnectionBase::Disconnect()
{
	serverConnection->Disconnect();
	return ERROR_NOERROR;
}

//! ���������� true - ���� ������� ���������� � ��������
bool CRecordServerConnectionBase::IsConnected()
{
	return serverConnection->IsConnected();
}