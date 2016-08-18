#include "StdAfx.h"
#include "../include/sharingmousekeyboardhandler.h"
#include "../protocol/RemoteDesktopOut.h"

CSharingMouseKeyboardHandler::CSharingMouseKeyboardHandler(CSharingSessionFactory* aSessionsFactory)
{
	sessionsFactory = aSessionsFactory;
	sessionName = NULL;
	lastMouseX = 0;
	lastMouseY = 0;
	realityID = 0;
}

CSharingMouseKeyboardHandler::~CSharingMouseKeyboardHandler(void)
{
}
void CSharingMouseKeyboardHandler::SetActiveTarget(LPCSTR alpTarget)
{
	sessionName = alpTarget;
}

void CSharingMouseKeyboardHandler::SetRealityId(unsigned int aRealityID)
{
	realityID = aRealityID;
}

void CSharingMouseKeyboardHandler::SendCommand(unsigned char aType, unsigned short aFirstParam, UINT aSecondParam, unsigned char aCode)
{
	if (!sessionName)
		return;
	//изменилась ли позиция мыши по OnMouseMove
	if (aType == 0 && aCode == 6)
	{
		if (lastMouseX == aFirstParam && lastMouseY == aSecondParam)
			return;
		else
		{
			lastMouseX = aFirstParam;
			lastMouseY = aSecondParam;
		}
	}
	CSharingViewSession* session = NULL;
	if (realityID == 0)
		session = sessionsFactory->GetViewSessionByName(sessionsFactory->ConvertToUnicName(sessionName));	
	else
	{
		session = sessionsFactory->GetViewSessionByName(sessionsFactory->ConvertToUnicName(sessionName, realityID));	
	}

	if (session)
	{
		SharingManProtocol::CRemoteDesktopOut commandPacket(session->GetSessionID(), aType, aFirstParam, aSecondParam, aCode);
		session->Send(ST_RemoteControl, commandPacket.GetData(), commandPacket.GetDataSize());
	}
	else
	{
		SetActiveTarget(NULL);
	}
}

void CSharingMouseKeyboardHandler::OnLeftMouseDown(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(0,aFirstParam, aSecondParam, 0);
}
void CSharingMouseKeyboardHandler::OnLeftMouseUp(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(0,aFirstParam, aSecondParam, 1);
}
void CSharingMouseKeyboardHandler::OnLeftMouseDblClk(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(0,aFirstParam, aSecondParam, 2);
}
void CSharingMouseKeyboardHandler::OnRightMouseDown(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(0,aFirstParam, aSecondParam, 3);
}
void CSharingMouseKeyboardHandler::OnRightMouseUp(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(0,aFirstParam, aSecondParam, 4);
}
void CSharingMouseKeyboardHandler::OnRightMouseDblClk(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(0,aFirstParam, aSecondParam, 5);
}
void CSharingMouseKeyboardHandler::OnMouseMove(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(0,aFirstParam, aSecondParam, 6);
}
void CSharingMouseKeyboardHandler::OnKeyUp(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(1,aFirstParam, aSecondParam, 0);
}
void CSharingMouseKeyboardHandler::OnKeyDown(UINT aFirstParam, UINT aSecondParam)
{
	SendCommand(1,aFirstParam, aSecondParam, 1);
}