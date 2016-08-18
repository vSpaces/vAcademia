#include "StdAfx.h"

//#ifdef USE_CONAITO_SDK_LIB

#include "..\..\include\ConaitoSDK\VoipManagerConaitoServer.h"
#include "..\..\include\ConaitoSDK\VoipWindow.h"

CVoipWindow::CVoipWindow(IVoipManWndEventHandlerEVO* aManager)
{
	manager = aManager;
}

CVoipWindow::~CVoipWindow(void)
{
	
}

LRESULT CVoipWindow::OnConnectionRequest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	manager->HandleOnConnectRequest();
	return 0;
}

LRESULT CVoipWindow::OnConnectFailed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnConnectFailed();
}

LRESULT CVoipWindow::OnConnectSuccess(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnConnectSuccess();
}

LRESULT CVoipWindow::OnConnectionLost(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnConnectionLost();
}

LRESULT CVoipWindow::OnAccepted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnAcceptedFromVoip();
}

LRESULT CVoipWindow::OnLoggedOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnLoggedOut();
}

LRESULT CVoipWindow::OnKicked(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnKicked();
}

LRESULT CVoipWindow::OnServerUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnServerUpdate();
}

LRESULT CVoipWindow::OnCommandError(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnCommandError( wParam);
}

LRESULT CVoipWindow::OnMyUserIDRecieved(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnMyUserIDRecieved( wParam);
}

LRESULT CVoipWindow::OnAddUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnAddUser( wParam);
}

LRESULT CVoipWindow::OnUpdateUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUpdateUser( wParam);
}

LRESULT CVoipWindow::OnRemoveUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnRemoveUser( wParam);
}

LRESULT CVoipWindow::OnAddChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnAddChannel( wParam);
}

LRESULT CVoipWindow::OnUpdateChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUpdateChannel( wParam);
}

LRESULT CVoipWindow::OnRemoveChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnRemoveChannel( wParam);
}

LRESULT CVoipWindow::OnChannelJoined(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnChannelJoined( wParam);
}

LRESULT CVoipWindow::OnChannelLeft(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnChannelLeft( wParam);
}

LRESULT CVoipWindow::OnAddFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnAddFile( lParam);
}

LRESULT CVoipWindow::OnRemoveFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnRemoveFile( lParam);
}

LRESULT CVoipWindow::OnFileTransferBegin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnFileTransferBegin( wParam);
}

LRESULT CVoipWindow::OnFileTransferCompleted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnFileTransferCompleted( wParam);
}

LRESULT CVoipWindow::OnFileTransferFailed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnFileTransferFailed( lParam);
}

LRESULT CVoipWindow::OnUserMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUserMessage( wParam, lParam);
}

LRESULT CVoipWindow::OnChannelMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnChannelMessage( wParam, lParam);
}

LRESULT CVoipWindow::OnUserTalking(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUserTalking( wParam);
}

LRESULT CVoipWindow::OnUserStoppedTalking(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnUserStoppedTalking( wParam);
}

LRESULT CVoipWindow::OnInvalideChannelName(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnInvalideChannelName( wParam);
}

LRESULT CVoipWindow::OnLogMumbleErrorCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->OnLogMumbleErrorCode( wParam, lParam);
}

/*LRESULT CVoipWindow::OnCommandToEVOIP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return manager->ProcessCommandById( wParam, lParam);
}*/
