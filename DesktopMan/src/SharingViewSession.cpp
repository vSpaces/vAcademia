#include "StdAfx.h"
#include "..\include\SharingViewSession.h"
#include "..\include\DynamicTextureImpl.h"
#include "..\include\SharingMan.h"
#include "..\..\Cscl3DWS\rm\rm.h"
#include "..\protocol\SessionStartOut.h"
#include "..\protocol\FrameIn.h"
#include "..\protocol\RemoteDesktopIn.h"
#include "..\protocol\StartSendFrameOut.h"
#include "..\..\VASEncoder\VASE.h"
#include "..\..\VASEncoder\VASEFramesDifPack.h"

#define RESIZE 64

CSharingViewSession::CSharingViewSession( SHARING_CONTEXT* apContext) : CSharingSessionBase( apContext)
{
	numbersOfTryStartFileSession = 0;
	destination = NULL;
	MP_NEW_V( dynamicTexture, CDynamicTextureImpl, apContext);
	dynamicTextureWasSet = false;
	m_fps = -1;
	prevKeyFrameWidth = 0;
	prevKeyFrameHeight = 0;
	useNewCodec = false;
	viewWindow = NULL;
	dynamicTextureWasDeleted = false;
	dynamicTextureWasNeverSet = true;
	m_isResizeApply = false;
	m_isFirstFramePackReceived = true;
}

CSharingViewSession::~CSharingViewSession()
{
 	dynamicTexture->SetViewWindow(NULL);
// 	SAFE_DELETE(viewWindow);
	if (context->gcontext->mpLogWriter)
		context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]VS deleting "+CComString(GetSessionName()));
	if (!dynamicTextureWasNeverSet)
	{
		if (context->gcontext->mpRM)
			context->gcontext->mpRM->DeleteDynamicTexture(dynamicTexture);
		dynamicTextureWasDeleted = true;
		if (dynamicTexture)
			dynamicTexture->DesktopManDestroy();
		dynamicTexture = NULL;
	}
 	else
 		SAFE_DELETE(dynamicTexture);
}

void CSharingViewSession::AddVASEPack( VASEFramesDifPack* pack, bool abKeyFrame)
{
	ATLASSERT( pack);
	if( !pack)
		return;
	if (dynamicTextureWasDeleted)
	{
		SAFE_DELETE( pack);
		return;
	}

	if( !dynamicTextureWasSet && m_fps != 0)
	{
		context->sharingMan->SendDestinationRequest( GetSessionName());
	}

	if( dynamicTexture)
	{
		if (useNewCodec)
			dynamicTexture->UseNewCodec();
		else
			dynamicTexture->NotUseNewCodec();
		dynamicTexture->AddVASEPack( pack, abKeyFrame);
	}
	else
		SAFE_DELETE( pack);
}

void	CSharingViewSession::SetDestinationPointer( IDesktopSharingDestination* apIDesktopSharingDestination)
{
	// ќтпустим DynamicTexture, если она была кому то назначена
	if( destination != apIDesktopSharingDestination && dynamicTextureWasSet)
	{
		dynamicTextureWasSet = false;
		dynamicTexture = NULL;
	}

	// ≈сли мы на кого рисовали, то забудем об этом
	if( destination != apIDesktopSharingDestination)
	{
		destination = NULL;
		dynamicTextureWasSet = false;
	}

	// ≈сли нужно создать новую динамическую текстуру
	if( !dynamicTextureWasSet && apIDesktopSharingDestination)
	{
		destination = apIDesktopSharingDestination;
		if( !dynamicTexture)
			MP_NEW_V( dynamicTexture, CDynamicTextureImpl, context);
		if( destination->SetDynamicTexture( dynamicTexture))
		{
 			MP_NEW_V2( viewWindow, CRemoteDesktopWindow, GetSessionName(), context);
// 			viewWindow->Show();
			dynamicTexture->SetTarget( GetSessionName());
			dynamicTexture->SetViewWindow(viewWindow);
			dynamicTexture->EnableNormalMode();
			dynamicTextureWasSet = true;
			dynamicTextureWasNeverSet = false;
		}
	}
}
void CSharingViewSession::ResetResize()
{
	if (destination)
 		destination->ResetResize();	

	dynamicTexture->ReturnRealSize();
}

void CSharingViewSession::ResizeImage()
{
	if (destination)
		m_isResizeApply = destination->ResizeImage();
}

bool CSharingViewSession::IsResizeApply()
{
	return m_isResizeApply;
}

bool CSharingViewSession::IsDynamicTextureLive()
{
	return dynamicTexture->IsLive();
}

void CSharingViewSession::SendStartSendFrames()
{
	SharingManProtocol::StartSendFrameOut	queryOut( GetSessionName());
	Send(ST_StartSendFrames, queryOut.GetData(), queryOut.GetDataSize());
}

void CSharingViewSession::OnSessionStarted()
{
	// ??
	SendStartSessionRequest();
}

void CSharingViewSession::OnSessionStopped()
{

}

void CSharingViewSession::SendStartSessionRequest()
{
	ATLASSERT( GetSessionName() != "");

	if (!IsFileSharingSession())
	{
		SharingManProtocol::CSessionStartOut	queryOut( GetSessionName(), false);
		SendQuery(ST_StartViewSession, queryOut.GetData(), queryOut.GetDataSize());	
	}
	else
	{
		SharingManProtocol::CSessionStartOut	queryOut( GetSessionName(), GetFileName(), GetRecordID(), IsPaused(), GetSeekFileTime(), GetServerVersion());
		SendQuery(ST_StartFilePlay, queryOut.GetData(), queryOut.GetDataSize());	
	}
}

void CSharingViewSession::SendTerminateSessionRequest()
{
	ATLASSERT( GetSessionName() != "");
	SharingManProtocol::CSessionStartOut	queryOut( GetSessionName(), false);
	Send(ST_StopSessionView, queryOut.GetData(), queryOut.GetDataSize());
}

void CSharingViewSession::HandleMessage( BYTE aType, BYTE* aData, int aDataSize)
{
	switch( aType)
	{
	case RT_FramesPackReceived:
		OnFramesPackReceived( aData, aDataSize);
		break;
	default:
		ATLASSERT( false);
	}
}

void CSharingViewSession::OnFramesPackReceived( BYTE* aData, int aDataSize)
{
	if (m_fps == 0) // если сесси€ скрыт
	{
		if (!m_isFirstFramePackReceived)
		{
			//dynamicTexture->SetFreezState(true);
			return;
		}
		else
		{
			//if (destination)
			//	m_isResizeApply = destination->ResizeImage();
			dynamicTexture->SetTextureSizeBeforeResize(dynamicTexture->GetTextureWidth(),dynamicTexture->GetTextureHeight());
			dynamicTexture->SetTextureSize(RESIZE,RESIZE);
			dynamicTexture->SetFreezState(false);
			UpdateTextureSize(RESIZE,RESIZE);
			m_isFirstFramePackReceived = false; //даем выполнить 1 раз, когда мы вдали от доски, а на ней началс€ шаринг
		}
	}

	VASEFramesDifPack* pack = MP_NEW(VASEFramesDifPack);
	CFrameIn	frameIn( aData, aDataSize);
	frameIn.SetFramesPack( pack);
//	if (context->gcontext->mpLogWriter)
//	context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]Frame received");
	if( !frameIn.Analyse())
	{
		SAFE_DELETE( pack);
		ATLASSERT( FALSE);
		return;
	}
	useNewCodec = frameIn.UseNewCodec();

	if (m_fps != 0)
		UpdateTextureSize(frameIn.GetKeyFrameWidth(), frameIn.GetKeyFrameHeight());
	AddVASEPack( pack, frameIn.GetKeyFrameFlag());
}

void CSharingViewSession::UpdateTextureSize(int currKeyFrameWidth, int currKeyFrameHeight)
{
	if (!currKeyFrameWidth || !currKeyFrameHeight)
	{
		return;
	}
	if	( currKeyFrameWidth == prevKeyFrameWidth && 
		currKeyFrameHeight == prevKeyFrameHeight)
	{
		return;
	}

	context->sharingMan->TextureSizeChangedEvent(GetSessionName(), currKeyFrameWidth, currKeyFrameHeight);
	prevKeyFrameWidth = currKeyFrameWidth;
	prevKeyFrameHeight = currKeyFrameHeight;
}

void CSharingViewSession::SetSessionFps(int fps)
{
	m_fps = fps;
	/*if (fps == 0)
	{
		context->gcontext->mpRM->DeleteDynamicTexture(dynamicTexture);
		if (dynamicTexture)
			dynamicTexture->DesktopManDestroy();
		dynamicTexture = NULL;
//		SendTerminateSessionRequest();
	}*/
}
// реализаци€ ISharingConnectionEvents
void CSharingViewSession::OnConnectLost()
{
	m_isFirstStartFileSession = true;
	connectionConnected = false;
}

void CSharingViewSession::OnConnectRestored()
{
	connectionConnected = true;
	SendStartSessionRequest();
}

void CSharingViewSession::SendGetOnlyKeyFrame()
{
	SharingManProtocol::StartSendFrameOut	queryOut( GetSessionName());
	Send(ST_OnlyKeyFrames, queryOut.GetData(), queryOut.GetDataSize());
}
void CSharingViewSession::SendGetAllFrames()
{
	SharingManProtocol::StartSendFrameOut	queryOut( GetSessionName());
	Send(ST_AllFrames, queryOut.GetData(), queryOut.GetDataSize());
}

std::string CSharingViewSession::GetSessionInfo()
{
	std::string str = "";
	char buf[100];
	str += "\r\nSession Info: \r\n\r\n";
	str += "unicSessionName: ";
	str += GetSessionName();
	str += "\r\n"; 
	str += "canRemoteAccess: ";
	_itoa_s(GetRemoteAccessMode(), (char*)buf, 100, 10);
	str += buf;
	str += "\r\n";
	str += "recordID: ";
	_itoa_s(GetRecordID(), (char*)buf, 100, 10);
	str += buf;
	str += "\r\n";
	str += "isFileSharing: ";
	_itoa_s(IsFileSharingSession(), (char*)buf, 100, 10);
	str += buf;
	str += "\r\n";
	str += "fileName: ";
	str += GetFileName();
	str += "\r\n";
	str += "connection active: ";
	_itoa_s(connectionConnected, (char*)buf, 100, 10);
	str += buf;
	str += "\r\n";

	str += "last keyframe width: ";
	_itoa_s(prevKeyFrameWidth, (char*)buf, 100, 10);
	str += buf;
	str += "\r\n";
	str += "last keyframe height: ";
	_itoa_s(prevKeyFrameHeight, (char*)buf, 100, 10);
	str += buf;
	str += "\r\n";
	str += "dynamicTextureWasSet: ";
	_itoa_s(dynamicTextureWasSet, (char*)buf, 100, 10);
	str += buf;
	str += "\r\n";

	if (dynamicTexture)
	{
		str += dynamicTexture->GetTextureInfo();
	}

	return str;
}

void CSharingViewSession::ShowLastFrame()
{
	if (dynamicTexture)
	{
		dynamicTexture->ShowLastFrame();
	}
}
