
#include "StdAfx.h"
#ifdef USE_CONAITO_SDK_LIB
#include "ConaitoSDK/EvoWrapper.h"
#endif

#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif
#include <math.h>

#include "include/voipManagerPipeEVO.h"
#include "include/PipeProtocol/InitVoipSystemIn.h"
#include "include/PipeProtocol/SetVoiceGainLevelIn.h"
#include "include/PipeProtocol/SetVoiceActivationLevelIn.h"
#include "include/PipeProtocol/HandleTransmissionStateChangedIn.h"
#include "include/PipeProtocol/SetAutoGaneControlEnabledIn.h"
#include "include/PipeProtocol/SetEnableDenoisingIn.h"
#include "include/PipeProtocol/GetDevicesIn.h"
#include "include/PipeProtocol/GetInputDevicesIn.h"
#include "include/PipeProtocol/SetUserGainLevelIn.h"
#include "include/PipeProtocol/ConnectToEVoipIn.h"
#include "include/PipeProtocol/AcceptingIn.h"
#include "include/PipeProtocol/DisconnectIn.h"
#include "include/PipeProtocol/DoLoginIn.h"
//#include "include/PipeProtocol/GetUserIn.h"
#include "include/PipeProtocol/SetUserVolumeIn.h"
#include "include/PipeProtocol/GetOutputDevicesIn.h"
#include "include/PipeProtocol/InitVoipSystemResultOut.h"
//#include "include/PipeProtocol/SetVoiceGainLevelOut.h"
#include "include/PipeProtocol/GetMyUserIDOut.h"
#include "include/PipeProtocol/GetDevicesResultOut.h"
#include "include/PipeProtocol/GetInputDevicesResultOut.h"
#include "include/PipeProtocol/ConnectToEVoipResultOut.h"
#include "include/PipeProtocol/GetOutputDevicesResultOut.h"
#include "include/PipeProtocol/ConfirmationOut.h"

#define DEF_MESSAGE_START	void CVoipManagerPipeEVO::ProcessCommandById(int cmdID, CDataStorage* commandData) { if (cmdID != 0 && cmdID != 1)	{		switch(cmdID)		{
#define DEF_MESSAGE_HANDLER(x, y) case CMD_##x: { ##y(commandData); } break;
#define DEF_MESSAGE_END default: break; } } }

	DEF_MESSAGE_START
		DEF_MESSAGE_HANDLER(IDLE, OnIdle);
		DEF_MESSAGE_HANDLER(INITVOIPSYSTEM, OnInitVoipSystem);
		DEF_MESSAGE_HANDLER(SETVOICEGAINLEVEL, OnSetVoiceGainLevel);
		DEF_MESSAGE_HANDLER(TRANSMISSIONSTATE, OnHandleTransmissionStateChanged);
		//DEF_MESSAGE_HANDLER( GETMYUSERID, OnGetMyUserID);
		DEF_MESSAGE_HANDLER( SETVOICEACTIVATIONLEVEL, OnSetVoiceActivationLevel)
		DEF_MESSAGE_HANDLER( SETAUTOGANECONTROL, OnSetAutoGaneControlEnabled);
		DEF_MESSAGE_HANDLER( SETDENOISING, OnSetEnableDenoising);
		DEF_MESSAGE_HANDLER( GETDEVICES, OnGetDevices);
		DEF_MESSAGE_HANDLER( SETUSERGAINLEVEL, OnSetUserGainLevel);	
		DEF_MESSAGE_HANDLER( CONNECTTOEVOIP, OnConnectToEVoip);
		DEF_MESSAGE_HANDLER( JOINCHANNEL, OnJoinChannel);
		DEF_MESSAGE_HANDLER( DISCONNECTEVOIP, OnDisconnectEVoip);
		DEF_MESSAGE_HANDLER( DOLOGIN, OnDoLogin);
		//DEF_MESSAGE_HANDLER( GETUSER, OnGetUser);
		DEF_MESSAGE_HANDLER( SETUSERVOLUME, OnSetUserVolume);
		//DEF_MESSAGE_HANDLER( CONNECTIONSTATUS, OnConnectionStatus);
		//DEF_MESSAGE_HANDLER( CHANNELJOINED, OnChannelJoined);
		//DEF_MESSAGE_HANDLER( CHANNELLEFT, OnChannelLeft);
		//DEF_MESSAGE_HANDLER( USERTALKINGSTATUS, OnUserTalking);
		//DEF_MESSAGE_HANDLER( KICKED, OnKicked);
		//DEF_MESSAGE_HANDLER( ADDUSER, OnAddUser);
		//DEF_MESSAGE_HANDLER( REMOVEUSER, OnRemoveUser);
		//DEF_MESSAGE_HANDLER( CURRENTINPUTLEVEL, OnSetCurrentInputLevel);
		DEF_MESSAGE_HANDLER( GETINPUTDEVICES, OnGetInputDevices);
		DEF_MESSAGE_HANDLER( GETOUTPUTDEVICES, OnGetOutputDevices);
		DEF_MESSAGE_HANDLER( CLOSEVOIPMANSERVER, OnCloseVoipManServer);
		DEF_MESSAGE_HANDLER ( SETMASTERVOLUME, OnSetMasterVolume);
		DEF_MESSAGE_HANDLER ( SETECHOCANCELLATION, OnSetEchoCancellation);
		DEF_MESSAGE_HANDLER ( SETMIXERECHOCANCELLATION, OnSetMixerEchoCancellation);

		DEF_MESSAGE_HANDLER ( STARTFINDMICDEVICE, OnStartFindMicDevice);
		DEF_MESSAGE_HANDLER ( ENDFINDMIVDEVICE, OnEndFindMicDevice);
		DEF_MESSAGE_HANDLER ( CONFIRMATION, OnAliveMsg);
	DEF_MESSAGE_END
		
void CVoipManagerPipeEVO::OnIdle(CDataStorage* data)
{
	int ff = 0;
}

void CVoipManagerPipeEVO::OnAliveMsg( CDataStorage* data)
{
	MP_NEW_P( packetOut, CConfirmationOut, CMD_CONFIRMATION);
	m_commandsData.AddOutCommand( packetOut->GetDataStorage());
}

void CVoipManagerPipeEVO::OnInitVoipSystem( CDataStorage* data)
{
	//
	CInitVoipSystemIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] CInitVoipSystemIn analyse is false");
		return;
	}
	unsigned int aErrorCode = pVoipManagerConaitoServer->InitializeVoipSystem( packetIn.GetDeviceGuid(), packetIn.GetOutDeviceGuid());
	bool bInitVoipSystem = pVoipManagerConaitoServer->IsVoipSystemInitialized();
	std::wstring currentEvoVoipDeviceGuid = pVoipManagerConaitoServer->GetCurrentEvoVoipDeviceGuid();	
	if( currentEvoVoipDeviceGuid.size() == 0)
		currentEvoVoipDeviceGuid = packetIn.GetDeviceGuid();
	std::wstring currentEvoVoipOutDeviceGuid = pVoipManagerConaitoServer->GetCurrentEvoVoipOutDeviceGuid();
	if( currentEvoVoipOutDeviceGuid.size() == 0)
		currentEvoVoipOutDeviceGuid = packetIn.GetOutDeviceGuid();
	CInitVoipSystemResultOut packetOut( CMD_INITVOIPSYSTEM, aErrorCode, bInitVoipSystem, packetIn.GetVasDevice(), packetIn.GetToIncludeSoundDevice(), currentEvoVoipDeviceGuid.c_str(), currentEvoVoipOutDeviceGuid.c_str());
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: InitializeVoipSystem  bInitVoipSystem = %d, vasDevice = %d",  bInitVoipSystem, packetIn.GetVasDevice());
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( str.GetBuffer());
	}
}

void CVoipManagerPipeEVO::OnSetVoiceGainLevel( CDataStorage* data)
{	
	CSetVoiceGainLevelIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] CSetVoiceGainLevelIn analyse is false");
		return;
	}
	pVoipManagerConaitoServer->SetVoiceGainLevel( packetIn.GetValue());
	SendCommit();
}

void CVoipManagerPipeEVO::OnHandleTransmissionStateChanged(CDataStorage* data)
{
	CHandleTransmissionStateChangedIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] CHandleTransmissionStateChangedIn analyse is false");
		return SendCommit();
	}
	bool bSendUserID = pVoipManagerConaitoServer->OnHandleTransmissionStateChanged( packetIn.GetMicrophoneEnabled(), packetIn.GetVoiceActivate());

	if( bSendUserID)
	{
		bool bInitVoipSystem = pVoipManagerConaitoServer->IsVoipSystemInitialized();
		int userID = pVoipManagerConaitoServer->GetMyUserID();

		if( pVoipManagerConaitoServer != NULL) {
			CComString sLog;
			sLog.Format("[VOIP_SERVER] OnHandleTransmissionStateChanged. Userid = %u", userID);
			pVoipManagerConaitoServer->WriteLogLine( sLog.GetBuffer());
		}

		//userID == 0 в мамбл - это значит что userID не известен или мы запрашиваем его когда отключены от сервера
		if (userID != 0){
			CGetMyUserIDOut packetOut( CMD_GETMYUSERID, userID, bInitVoipSystem);
			m_commandsData.AddOutCommand( packetOut.GetDataStorage());
		}
	}
	else
	{
		SendCommit();
	}
}

void CVoipManagerPipeEVO::OnSetVoiceActivationLevel( CDataStorage* data)
{	
	CSetVoiceActivationLevelIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] CSetVoiceActivationLevelIn analyse is false");
		return SendCommit();
	}
	pVoipManagerConaitoServer->SetVoiceActivationLevel( packetIn.GetValue());
	SendCommit();
}

void CVoipManagerPipeEVO::OnSetAutoGaneControlEnabled( CDataStorage* data)
{	
	CSetAutoGaneControlEnabledIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] CSetAutoGaneControlEnabledIn analyse is false");
		return;
	}
	pVoipManagerConaitoServer->SetAutoGaneControlEnabled( packetIn.GetEnabled());
	SendCommit();
}

void CVoipManagerPipeEVO::OnSetEnableDenoising( CDataStorage* data)
{	
	CSetEnableDenoisingIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] CSetEnableDenoisingIn analyse is false");
		return;
	}
	pVoipManagerConaitoServer->SetDenoisingEnabled( packetIn.GetEnabled());
	SendCommit();
}

void CVoipManagerPipeEVO::OnGetDevices( CDataStorage* data)
{
	CGetDevicesIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] CGetDevicesIn analyse is false");
		return;
	}
	vecDeviceInfo *pInputDevicesInfo = NULL;
	if( packetIn.GetForce())
		pVoipManagerConaitoServer->GrabDevicesInfo( true);
	pVoipManagerConaitoServer->GetInputDevicesInfo( &pInputDevicesInfo);
	vecDeviceInfo *pOutputDevicesInfo = NULL;
	pVoipManagerConaitoServer->GetOutputDevicesInfo( &pOutputDevicesInfo);
	//bool bInitVoipSystem = pVoipManagerConaitoServer->IsVoipSystemInitialized();
	CGetDevicesResultOut packetOut( CMD_GETDEVICES, pInputDevicesInfo, pOutputDevicesInfo, packetIn.GetNeedAnswerToRmml());
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
	if( pVoipManagerConaitoServer != NULL)
	{
		CComString s;
		s.Format("[VOIPSERVER] OnGetDevices pInputDevicesInfo.size() = %u,  pOutputDevicesInfo->size() = %u", pInputDevicesInfo->size(), pOutputDevicesInfo->size());
		pVoipManagerConaitoServer->WriteLogLine( s.GetBuffer());
	}
}

void CVoipManagerPipeEVO::OnGetInputDevices( CDataStorage* data)
{
	CGetInputDevicesIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] CGetInputDevicesIn analyse is false");
		return;
	}
	vecDeviceInfo *pInputDevicesInfo = NULL;
	if( packetIn.GetForce())
		pVoipManagerConaitoServer->GrabInputDevicesInfoForce( true);
	pVoipManagerConaitoServer->GetInputDevicesInfo( &pInputDevicesInfo);
	//bool bInitVoipSystem = pVoipManagerConaitoServer->IsVoipSystemInitialized();
	CGetInputDevicesResultOut packetOut( CMD_GETINPUTDEVICES, pInputDevicesInfo, packetIn.GetNeedAnswerToRmml());
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
	if( pVoipManagerConaitoServer != NULL)
	{
		CComString s;
		s.Format("[VOIPSERVER] OnGetInputDevices pInputDevicesInfo.size() = %u", pInputDevicesInfo->size());
		pVoipManagerConaitoServer->WriteLogLine( s.GetBuffer());
	}
}

void CVoipManagerPipeEVO::OnGetOutputDevices( CDataStorage* data)
{
	CGetOutputDevicesIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] CGetOutputDevicesIn analyse is false");
		return;
	}
	vecDeviceInfo *pOutputDevicesInfo = NULL;
	if( packetIn.GetForce())
		pVoipManagerConaitoServer->GrabOutputDevicesInfoForce( true);
	pVoipManagerConaitoServer->GetOutputDevicesInfo( &pOutputDevicesInfo);
	//bool bInitVoipSystem = pVoipManagerConaitoServer->IsVoipSystemInitialized();
	CGetOutputDevicesResultOut packetOut( CMD_GETOUTPUTDEVICES, pOutputDevicesInfo, packetIn.GetNeedAnswerToRmml());
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
	if( pVoipManagerConaitoServer != NULL)
	{
		CComString s;
		s.Format("[VOIPSERVER] OnGetOutputDevices pOutputDevicesInfo.size() = %u", pOutputDevicesInfo->size());
		pVoipManagerConaitoServer->WriteLogLine( s.GetBuffer());
	}
}

void CVoipManagerPipeEVO::OnSetUserGainLevel( CDataStorage* data)
{
	CSetUserGainLevelIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] CSetUserGainLevelIn analyse is false");
		return;
	}
	pVoipManagerConaitoServer->SetUserGainLevel( packetIn.GetUserName().c_str(), packetIn.GetValue());
	SendCommit();
}

void CVoipManagerPipeEVO::OnConnectToEVoip( CDataStorage* data)
{
	CConnectToEVoipIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] CConnectToEVoipIn analyse is false");
		return;
	}

	pVoipManagerConaitoServer->SetServerParams( packetIn.GetServerIP(), packetIn.GetTcpPort(), packetIn.GetUdpPort(), packetIn.GetProxySettings());
	unsigned int errorCode = pVoipManagerConaitoServer->ConnectImpl( packetIn.GetCheckConnecting());
	CConnectToEVoipResultOut packetOut(CMD_CONNECTTOEVOIP, errorCode);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: ConnectImpl errorCode = %d", errorCode == ERROR_NOERROR ? 0 : errorCode);
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( str.GetBuffer());
	}
}

void CVoipManagerPipeEVO::OnJoinChannel( CDataStorage* data)
{
	//SendCommit();
	CJoinChannelIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] CJoinChannelIn analyse is false");
		return;
	}
	//pVoipManagerConaitoServer->SetServerParams( packetIn.GetServerIP(), packetIn.GetTcpPort(), packetIn.GetUdpPort());
	pVoipManagerConaitoServer->JoinChannel( packetIn.GetRoomName());
}

void CVoipManagerPipeEVO::OnDisconnectEVoip( CDataStorage* data)
{
	CDisconnectIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] CDisconnectIn analyse is false");
		return;
	}
	pVoipManagerConaitoServer->DisconnectImpl();
	SendCommit();
}

void CVoipManagerPipeEVO::OnDoLogin( CDataStorage* data)
{	
	CDoLoginIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] CDoLoginIn analyse is false");
		return;
	}
	//pVoipManagerConaitoServer->SetServerParams( packetIn.GetServerIP(), packetIn.GetTcpPort(), packetIn.GetUdpPort());
	pVoipManagerConaitoServer->DoLogin( packetIn.GetUserName().c_str(), packetIn.GetPassword().c_str(), packetIn.GetRoomName().c_str());
	SendCommit();
}

/*void CVoipManagerPipeEVO::OnGetUser( CDataStorage* data)
{
	MP_NEW_P2( packetIn, CGetUserIn, data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "CGetUserIn analyse is false");
		return;
	}
	//pVoipManagerConaitoServer->SetServerParams( packetIn.GetServerIP(), packetIn.GetTcpPort(), packetIn.GetUdpPort());
	User user = {0};
	pVoipManagerConaitoServer->GetEvoUserImpl( packetIn.GetUserID(), &user);
	std::wstring sNickName;
#if CONAITO_SDK_LIB_VERSION == 3	
	sNickName = user.szNickName;
#else
	USES_CONVERSION;
	sNickName = A2W(user.szNickName);
#endif
	MP_NEW_P5( packetOut, CGetUserResultOut, CMD_GETUSER, packetIn.GetMsgID(), packetIn.GetUserID(), user.nUserID, sNickName);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}*/

void CVoipManagerPipeEVO::OnSetUserVolume( CDataStorage* data)
{	
	CSetUserVolumeIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] CDisconnectIn analyse is false");
		return;
	}
	//pVoipManagerConaitoServer->SetServerParams( packetIn.GetServerIP(), packetIn.GetTcpPort(), packetIn.GetUdpPort());
	pVoipManagerConaitoServer->SetUserVolume( packetIn.GetUserName().c_str(), packetIn.GetVolume());
	SendCommit();
}

void CVoipManagerPipeEVO::OnSetMasterVolume( CDataStorage* data)
{
	CSetVoiceGainLevelIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] OnSetMasterVolume analyse is false");
		return;
	}

	pVoipManagerConaitoServer->SetMasterVolume( packetIn.GetValue());
	SendCommit();
}

void CVoipManagerPipeEVO::OnSetEchoCancellation( CDataStorage* data)
{
	CSetEnableDenoisingIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] OnSetEchoCancellation analyse is false");
		return;
	}

	pVoipManagerConaitoServer->SetEchoCancellation( packetIn.GetEnabled());
	SendCommit();
}

void CVoipManagerPipeEVO::OnSetMixerEchoCancellation( CDataStorage* data)
{
	CSetEnableDenoisingIn packetIn( data->GetData(), data->GetSize());
	if(!packetIn.Analyse())
	{
		if( pVoipManagerConaitoServer != NULL)
			pVoipManagerConaitoServer->WriteLogLine( "[VOIPSERVER] OnSetMixerEchoCancellation analyse is false");
		return;
	}

	pVoipManagerConaitoServer->SetMixerEchoCancellation( packetIn.GetEnabled());
	SendCommit();
}

void CVoipManagerPipeEVO::OnCloseVoipManServer( CDataStorage* data)
{
	if( pVoipManagerConaitoServer != NULL)
		pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] i have got close message from player");
	SendDestroyMsg();
}	

void CVoipManagerPipeEVO::OnStartFindMicDevice(CDataStorage* data)
{
	pVoipManagerConaitoServer->AppStartFindMicDevice();
}

void CVoipManagerPipeEVO::OnEndFindMicDevice(CDataStorage* data)
{
	pVoipManagerConaitoServer->AppEndFindMicDevice();
}