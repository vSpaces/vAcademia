#include "StdAfx.h"

//#ifdef USE_CONAITO_SDK_LIB

#include "..\..\include\ConaitoSDK\VoipManagerConaito.h"
#include "..\..\include\ConaitoSDK\VoipWindowClient.h"

CVoipWindowClient::CVoipWindowClient(IVoipManWndEventHandler* aManager)
{
	manager = aManager;
}

CVoipWindowClient::~CVoipWindowClient(void)
{
	
}

LRESULT CVoipWindowClient::OnConnectionRequest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	manager->HandleOnConnectRequest();
	return 0;
}

LRESULT CVoipWindowClient::OnConnectFailed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnConnectFailed();
}

LRESULT CVoipWindowClient::OnConnectSuccess(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnConnectSuccess();
}

LRESULT CVoipWindowClient::OnConnectionLost(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnConnectionLost();
}

LRESULT CVoipWindowClient::OnAccepted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnAccepted();
}

LRESULT CVoipWindowClient::OnLoggedOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnLoggedOut();
}

LRESULT CVoipWindowClient::OnKicked(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnKicked();
}

LRESULT CVoipWindowClient::OnServerUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnServerUpdate();
}

LRESULT CVoipWindowClient::OnCommandError(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnCommandError( wParam);
}

LRESULT CVoipWindowClient::OnAddUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnAddUser( wParam, lParam);
}

LRESULT CVoipWindowClient::OnUpdateUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUpdateUser( wParam);
}

LRESULT CVoipWindowClient::OnRemoveUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnRemoveUser( wParam);
}

LRESULT CVoipWindowClient::OnAddChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnAddChannel( wParam);
}

LRESULT CVoipWindowClient::OnUpdateChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUpdateChannel( wParam);
}

LRESULT CVoipWindowClient::OnRemoveChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnRemoveChannel( wParam);
}

LRESULT CVoipWindowClient::OnChannelJoined(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnChannelJoined( wParam, lParam);
}

LRESULT CVoipWindowClient::OnChannelLeft(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnChannelLeft( wParam);
}

LRESULT CVoipWindowClient::OnAddFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnAddFile( lParam);
}

LRESULT CVoipWindowClient::OnRemoveFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnRemoveFile( lParam);
}

LRESULT CVoipWindowClient::OnFileTransferBegin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnFileTransferBegin( wParam);
}

LRESULT CVoipWindowClient::OnFileTransferCompleted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnFileTransferCompleted( wParam);
}

LRESULT CVoipWindowClient::OnFileTransferFailed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnFileTransferFailed( lParam);
}

LRESULT CVoipWindowClient::OnUserMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUserMessage( wParam, lParam);
}

LRESULT CVoipWindowClient::OnChannelMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnChannelMessage( wParam, lParam);
}

LRESULT CVoipWindowClient::OnUserTalking(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUserTalking( wParam, lParam);
}

LRESULT CVoipWindowClient::OnUserStoppedTalking(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUserStoppedTalking( wParam, lParam);
}

LRESULT CVoipWindowClient::OnInitVoipSystem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnInitVoipSystem( wParam, lParam);
}

//#endif