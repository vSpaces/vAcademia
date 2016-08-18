#pragma once

#include "atlbase.h"
#include "atlwin.h"

//#include "AudioCapture.h"

struct IVoipManWndEventHandler;

#define WM_VOIP_BASE	(WM_USER + 111)

#define WM_VOIP_CONNECT_REQUEST			(WM_VOIP_BASE + 1)
#define WM_VOIP_DISCONNECT_REQUEST		(WM_VOIP_BASE + 2)

class CVoipWindow : public CWindowImpl<CVoipWindow>
{
private:
	IVoipManWndEventHandler* manager;

public:
	CVoipWindow(IVoipManWndEventHandler* aManager);
	~CVoipWindow(void);

	BEGIN_MSG_MAP(CVoipWindow)
		MESSAGE_HANDLER(WM_VOIP_CONNECT_REQUEST, OnConnectionRequest)
		//The WM_EVOVOIP_* message handlers
		MESSAGE_HANDLER(WM_EVOVOIP_CONNECTFAILED, OnConnectFailed)
		MESSAGE_HANDLER(WM_EVOVOIP_CONNECTSUCCESS, OnConnectSuccess)
		MESSAGE_HANDLER(WM_EVOVOIP_CONNECTIONLOST, OnConnectionLost)
		MESSAGE_HANDLER(WM_EVOVOIP_ACCEPTED, OnAccepted)
		MESSAGE_HANDLER(WM_EVOVOIP_LOGGEDOUT, OnLoggedOut)
		/*session tree */
		//MESSAGE_HANDLER(WM_EVOVOIP_ADDCHANNEL, OnAddChannel)
		//MESSAGE_HANDLER(WM_EVOVOIP_UPDATECHANNEL, OnUpdateChannel)
		//MESSAGE_HANDLER(WM_EVOVOIP_REMOVECHANNEL, OnRemoveChannel)
		MESSAGE_HANDLER(WM_EVOVOIP_JOINEDCHANNEL, OnChannelJoined)
		MESSAGE_HANDLER(WM_EVOVOIP_LEFTCHANNEL, OnChannelLeft)

		MESSAGE_HANDLER(WM_EVOVOIP_ADDUSER, OnAddUser)
		//MESSAGE_HANDLER(WM_EVOVOIP_UPDATEUSER, OnUpdateUser)
		MESSAGE_HANDLER(WM_EVOVOIP_REMOVEUSER, OnRemoveUser)

		MESSAGE_HANDLER(WM_EVOVOIP_ADDFILE, OnAddFile)
		MESSAGE_HANDLER(WM_EVOVOIP_REMOVEFILE, OnRemoveFile)
		MESSAGE_HANDLER(WM_EVOVOIP_TRANSFER_FAILED, OnFileTransferFailed)
		MESSAGE_HANDLER(WM_EVOVOIP_TRANSFER_BEGIN, OnFileTransferBegin)
		MESSAGE_HANDLER(WM_EVOVOIP_TRANSFER_COMPLETED, OnFileTransferCompleted)

		MESSAGE_HANDLER(WM_EVOVOIP_USERMESSAGE, OnUserMessage)
		MESSAGE_HANDLER(WM_EVOVOIP_CHANNELMESSAGE, OnChannelMessage)

		MESSAGE_HANDLER(WM_EVOVOIP_USERTALKING, OnUserTalking)
		MESSAGE_HANDLER(WM_EVOVOIP_USERSTOPPEDTALKING, OnUserStoppedTalking)

		MESSAGE_HANDLER(WM_EVOVOIP_KICKED, OnKicked)
		MESSAGE_HANDLER(WM_EVOVOIP_SERVERUPDATE, OnServerUpdate)
		MESSAGE_HANDLER(WM_EVOVOIP_ERROR, OnCommandError)
	END_MSG_MAP()

protected:
	LRESULT OnConnectionRequest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//the WM_TEAMTALK_* message handlers
	LRESULT OnConnectFailed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnConnectSuccess(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnConnectionLost(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAccepted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLoggedOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKicked(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnServerUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommandError(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnAddUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUpdateUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRemoveUser(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnAddChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUpdateChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRemoveChannel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChannelJoined(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChannelLeft(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnAddFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRemoveFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFileTransferBegin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFileTransferCompleted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFileTransferFailed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


	LRESULT OnUserMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChannelMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnUserTalking(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUserStoppedTalking(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
