
#include "StdAfx.h"
#include <math.h>
#ifdef USE_CONAITO_SDK_LIB
#include "ConaitoSDK/EvoWrapper.h"
#endif

#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif
#include "include/voipManagerPipe.h"
#include "include/PipeProtocol/InitVoipSystemResultIn.h"
#include "include/PipeProtocol/GetMyUserIDIn.h"
#include "include/PipeProtocol/GetDevicesResultIn.h"
#include "include/PipeProtocol/GetInputDevicesResultIn.h"
#include "include/PipeProtocol/GetOutputDevicesResultIn.h"
#include "include/PipeProtocol/ConnectToEVoipResultIn.h"
//#include "include/PipeProtocol/GetUserResultIn.h"

#include "include/PipeProtocol/ConnectionStatusIn.h"
#include "include/PipeProtocol/ChannelJoinedIn.h"
//#include "include/PipeProtocol/ChannelLeftIn.h"
#include "include/PipeProtocol/UserTalkingIn.h"
//#include "include/PipeProtocol/KickedIn.h"
#include "include/PipeProtocol/AddUserIn.h"
#include "include/PipeProtocol/RemoveUserIn.h"
#include "include/PipeProtocol/CurrentInputLevelIn.h"
#include "include/PipeProtocol/LogIn.h"
#include "include/PipeProtocol/WizardFindedDeviceIn.h"


#define DEF_MESSAGE_START	void CVoipManagerPipe::ProcessCommandById(int cmdID, CDataStorage* commandData) { if (cmdID != 0 && cmdID != 1)	{		switch(cmdID)		{
#define DEF_MESSAGE_HANDLER(x, y) case CMD_##x: { ##y(commandData); } break;
#define DEF_MESSAGE_END default: break; } } }

	DEF_MESSAGE_START
		DEF_MESSAGE_HANDLER( INITVOIPSYSTEM, OnSendInitVoipSystem);
		//DEF_MESSAGE_HANDLER( SETVOICEGAINLEVEL, OnSetVoiceGainLevel);
		//DEF_MESSAGE_HANDLER( TRANSMISSIONSTATE, OnHandleTransmissionStateChanged);		
		DEF_MESSAGE_HANDLER( GETMYUSERID, OnGetMyUserID);
		//DEF_MESSAGE_HANDLER( SETVOICEACTIVATIONLEVEL, OnSetVoiceActivationLevel);
		//DEF_MESSAGE_HANDLER( SCMD_SetAutoGaneControlEnabled);
		//DEF_MESSAGE_HANDLER( SETDENOISING, OnEnableDenoising);
		DEF_MESSAGE_HANDLER( GETDEVICES, OnGetDevices);
		//DEF_MESSAGE_HANDLER( SETUSERGANELEVEL, OnGetInputDevices);
		DEF_MESSAGE_HANDLER( CONNECTTOEVOIP, OnConnectToEVoipResult);
		//DEF_MESSAGE_HANDLER( JOINCHANNEL, OnAccept);
		//DEF_MESSAGE_HANDLER( DISCONNECTEVOIP, OnDisconnectEVoip);
		//DEF_MESSAGE_HANDLER( DOLOGIN, OnDoLogin);
		//DEF_MESSAGE_HANDLER( GETUSER, OnGetUserResult);
		//DEF_MESSAGE_HANDLER( SETUSERVOLUME, OnSetUserVolume);
		DEF_MESSAGE_HANDLER( CONNECTIONSTATUS, OnConnectionStatus);
		DEF_MESSAGE_HANDLER( CHANNELJOINED, OnChannelJoined);
		DEF_MESSAGE_HANDLER( CHANNELLEFT, OnChannelLeft);
		DEF_MESSAGE_HANDLER( USERTALKINGSTATUS, OnUserTalking);
		DEF_MESSAGE_HANDLER( KICKED, OnKicked);
		DEF_MESSAGE_HANDLER( ADDUSER, OnAddUser);
		DEF_MESSAGE_HANDLER( REMOVEUSER, OnRemoveUser);
		DEF_MESSAGE_HANDLER( CURRENTINPUTLEVEL, OnSetCurrentInputLevel);
		DEF_MESSAGE_HANDLER( GETINPUTDEVICES, OnGetInputDevices);
		DEF_MESSAGE_HANDLER( LOG, OnServerLog);
		DEF_MESSAGE_HANDLER( GETOUTPUTDEVICES, OnGetOutputDevices);
		DEF_MESSAGE_HANDLER( CONFIRMATION, OnVoipManAliveMsg);
		DEF_MESSAGE_HANDLER( DEVICE_ERROR_CODE, OnDeviceErrorMsg);
		DEF_MESSAGE_HANDLER( AUTOFINDEDMIC, OnAutoFindedMic);
	DEF_MESSAGE_END

	
	void CVoipManagerPipe::OnAutoFindedMic(CDataStorage* data)
	{
		CWizardFindedDeviceIn packetIn( data->GetData(), data->GetSize());
		if(!packetIn.Analyse())
		{
			if( context->mpLogWriter != NULL)
				context->mpLogWriter->WriteLn("[VOIP] OnAutoFindedMic analyse is false");
			return;
		}
		std::wstring findedMicGuid = packetIn.GetDeviceGuid();
		std::wstring findedMicName = packetIn.GetDeviceName();
		std::wstring findedMicLineName = packetIn.GetDeviceLineName();
		pVoipManagerConaito->OnFindedMicName(findedMicGuid.c_str(), findedMicName.c_str(), findedMicLineName.c_str());
	}

void CVoipManagerPipe::OnDeviceErrorMsg(CDataStorage* data)
{
	CConnectionStatusIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] OnDeviceErrorMsg analyse is false");
		return;
	}
	int codeError = packetIn.GetCode();
	pVoipManagerConaito->OnDeviceErrorCode(codeError);
}

void CVoipManagerPipe::OnVoipManAliveMsg(CDataStorage* data)
{
	lastCommandReceivedTime = GetTickCount()/1000;
}

void CVoipManagerPipe::OnSendInitVoipSystem(CDataStorage* data)
{
	CInitVoipSystemResultIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CInitVoipSystemResultIn analyse is false");
		return;
	}
	int codeError = packetIn.GetErrorCode();
	pVoipManagerConaito->OnSendInitVoipSystem( codeError, packetIn.IsInitVoipSystem(), packetIn.GetVasDevice(), packetIn.GetToIncludeSoundDevice(), packetIn.GetDeviceName(), packetIn.GetOutDeviceName());	
}

void CVoipManagerPipe::OnSetVoiceGainLevel(CDataStorage* data)
{
}

void CVoipManagerPipe::OnStartTransmitting(CDataStorage* data)
{

}

void CVoipManagerPipe::OnStopTransmitting(CDataStorage* data)
{

}

void CVoipManagerPipe::OnEnableVoiceActivation(CDataStorage* data)
{

}

void CVoipManagerPipe::OnGetMyUserID( CDataStorage* data)
{
	CGetMyUserIDIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CGetMyUserIDIn analyse is false");
		return;
	}
	int userID = packetIn.GetUserID();
	pVoipManagerConaito->OnGetMyUserID( userID);
	if( packetIn.IsVoipSystemInitialized())
		pVoipManagerConaito->SetVoipSystemInitialized( packetIn.IsVoipSystemInitialized());
}

void CVoipManagerPipe::OnGetDevices( CDataStorage* data)
{
	CGetDevicesResultIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CGetDevicesIn analyse is false");
		return;
	}
	//std::wstring str = packetIn.GetDevicesInfo();
	vecDeviceInfo *pInputDevicesInfo = packetIn.GetInputDevicesInfo();
	pVoipManagerConaito->SetInputDevices( pInputDevicesInfo);
	vecDeviceInfo *pOutputDevicesInfo = packetIn.GetOutputDevicesInfo();
	pVoipManagerConaito->SetOutputDevices( pOutputDevicesInfo);
	//pVoipManagerConaito->CorrectOldSettings();
	if( packetIn.GetNeedAnswerToRmml())
	{
		pVoipManagerConaito->OnGetInputDevices();
		pVoipManagerConaito->OnGetOutputDevices();
	}
	if( context->mpLogWriter)
	{		
		context->mpLogWriter->WriteLn( "[VOIP] OnGetDevices pInputDevicesInfo.size() = ", (int)pInputDevicesInfo->size(), ",  pOutputDevicesInfo->size() = ", (int)pOutputDevicesInfo->size());		
	}
}

void CVoipManagerPipe::OnGetInputDevices( CDataStorage* data)
{
	CGetInputDevicesResultIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CGetInputDevicesIn analyse is false");
		return;
	}
	//std::wstring str = packetIn.GetDevicesInfo();
	vecDeviceInfo *pInputDevicesInfo = packetIn.GetInputDevicesInfo();
	pVoipManagerConaito->SetInputDevices( pInputDevicesInfo);
	if( packetIn.GetNeedAnswerToRmml())
		pVoipManagerConaito->OnGetInputDevices();
	if( context->mpLogWriter)
	{		
		context->mpLogWriter->WriteLn( "[VOIP] OnGetInputDevices pInputDevicesInfo.size() = ", (int)pInputDevicesInfo->size());		
	}
}

void CVoipManagerPipe::OnGetOutputDevices( CDataStorage* data)
{
	CGetOutputDevicesResultIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CGetOutputDevicesIn analyse is false");
		return;
	}
	//std::wstring str = packetIn.GetDevicesInfo();
	vecDeviceInfo *pOutputDevicesInfo = packetIn.GetOutputDevicesInfo();
	pVoipManagerConaito->SetOutputDevices( pOutputDevicesInfo);
	if( packetIn.GetNeedAnswerToRmml())
		pVoipManagerConaito->OnGetOutputDevices();
	if( context->mpLogWriter)
	{		
		context->mpLogWriter->WriteLn( "[VOIP] OnGetInputDevices pOutputDevicesInfo.size() = ", (int)pOutputDevicesInfo->size());		
	}
}

void CVoipManagerPipe::OnConnectToEVoipResult( CDataStorage* data)
{
	CConnectToEVoipResultIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CConnectToEVoipResultIn analyse is false");
		pVoipManagerConaito->OnConnectImpl( -1);
		return;
	}
	pVoipManagerConaito->OnConnectImpl( packetIn.GetCode());
	//if( packetIn.GetCode() == ERROR_NOERROR)
		maxDelayBetweenCommands = 20;
}

void CVoipManagerPipe::OnConnectionStatus( CDataStorage* data)
{
	CConnectionStatusIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CConnectionStatusIn analyse is false");
		return;
	}
	pVoipManagerConaito->OnConnectionStatus( packetIn.GetCode());
}

void CVoipManagerPipe::OnChannelJoined( CDataStorage* data)
{
	CChannelJoinedIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CChannelJoinedIn analyse is false");
		return;
	}
	pVoipManagerConaito->SendMessageToWnd( WM_EVOVOIP_JOINEDCHANNEL, 0, packetIn.GetChannelPath().c_str());
	//pVoipManagerConaito->OnChannelJoined( (wchar_t*)packetIn.GetChannelPath().c_str());
}

void CVoipManagerPipe::OnChannelLeft( CDataStorage* data)
{
	/*MP_NEW_P2( packetIn, CChannelLeftIn, data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CChannelLeftIn analyse is false");
		return;
	}*/
	pVoipManagerConaito->SendMessageToWnd( WM_EVOVOIP_LEFTCHANNEL);
	//pVoipManagerConaito->OnChannelLeft();
}

void CVoipManagerPipe::OnAddUser( CDataStorage* data)
{
	CAddUserIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CAddUserIn analyse is false");
		pVoipManagerConaito->OnConnectImpl( -1);
		return;
	}
	//CComString sUserName = packetIn.GetUserNick().c_str();
	pVoipManagerConaito->SendMessageToWnd( WM_EVOVOIP_ADDUSER, packetIn.GetUserID(), packetIn.GetUserNick().c_str());
	//pVoipManagerConaito->OnAddUser( packetIn.GetUserID(), sUserName.GetBuffer());
}

void CVoipManagerPipe::OnRemoveUser( CDataStorage* data)
{
	CRemoveUserIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CRemoveUserIn analyse is false");
		return;
	}
	pVoipManagerConaito->SendMessageToWnd( WM_EVOVOIP_REMOVEUSER, packetIn.GetUserID());
	//pVoipManagerConaito->OnRemoveUser( packetIn.GetUserID());
}

void CVoipManagerPipe::OnUserTalking( CDataStorage* data)
{
	CUserTalkingIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CUserTalkingIn analyse is false");
		return;
	}
	int iTalking = (int)packetIn.GetIsTalking();
	//CComString sUserName = packetIn.GetUserNick().c_str();
	switch (iTalking)
	{
	case 1:
		pVoipManagerConaito->SendMessageToWnd( WM_EVOVOIP_USERTALKING, packetIn.GetUserID(), packetIn.GetUserNick().c_str());
		//pVoipManagerConaito->OnUserTalking( packetIn.GetUserID(), packetIn.GetUserNick().c_str());
		break;
	case 0:
		pVoipManagerConaito->SendMessageToWnd( WM_EVOVOIP_USERSTOPPEDTALKING, packetIn.GetUserID(), packetIn.GetUserNick().c_str());
		//pVoipManagerConaito->OnUserStoppedTalking( packetIn.GetUserID(), packetIn.GetUserNick().c_str());
		break;
	}
}

void CVoipManagerPipe::OnKicked( CDataStorage* data)
{
	/*MP_NEW_P2( packetIn, CKickedIn, data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CKickedIn analyse is false");
		return;
	}*/
	pVoipManagerConaito->SendMessageToWnd( WM_EVOVOIP_KICKED);
	//pVoipManagerConaito->OnKicked();
}

void CVoipManagerPipe::OnSetCurrentInputLevel( CDataStorage* data)
{
	CCurrentInputLevelIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CCurrentInputLevelIn analyse is false");
		return;
	}
	if(bRunning)
		pVoipManagerConaito->SetCurrentInputLevel( packetIn.GetCurrentInputLevel());
}

void CVoipManagerPipe::OnServerLog( CDataStorage* data)
{
	CLogIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIP] CLogIn analyse is false");
		return;
	}
	if( context->mpLogWriter != NULL)
	{
		context->mpLogWriter->WriteLnLPCSTR( packetIn.GetLog().c_str());
	}
}
