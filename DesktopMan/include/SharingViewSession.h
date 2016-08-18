#pragma once

#include "SharingSessionBase.h"
#include "SharingSession.h"
#include "../include/view/RemoteDesktopWindow.h"

class VASEFramesDifPack;
class CDynamicTextureImpl;


//////////////////////////////////////////////////////////////////////////
class DESKTOPMAN_API CSharingViewSession : public CSharingSessionBase
{
public:
	CSharingViewSession( SHARING_CONTEXT* apContext);
	virtual ~CSharingViewSession();

	void	AddVASEPack(VASEFramesDifPack* aPack, bool abKeyFrame);
	void	SetDestinationPointer( IDesktopSharingDestination* apIDesktopSharingDestination);
	void	HandleMessage( BYTE aType, BYTE* aData, int aDataSize);

	// overrides CSharingSessionBase
public:
	virtual void OnSessionStarted();
	virtual void OnSessionStopped();
	virtual void SendTerminateSessionRequest();

	// реализация ISharingConnectionEvents
public:
	void OnConnectLost();
	void OnConnectRestored();

public:
	bool						useNewCodec;
	void SetSessionFps(int fps);
    void UpdateTextureSize(int currKeyFrameWidth, int currKeyFrameHeight);
	void SendStartSendFrames();

	void SendGetOnlyKeyFrame();
	void SendGetAllFrames();

	void IncCountsOfTrying()
	{
		numbersOfTryStartFileSession++;
	}
	int GetNumbersOfTryingStartFileSession()
	{
		return numbersOfTryStartFileSession;
	}
	void ResetCountsOfTrying()
	{
		numbersOfTryStartFileSession = 0;
	}

	std::string GetSessionInfo();
	void ShowLastFrame();

	void ResetResize();
	void ResizeImage();
	bool IsResizeApply();

	bool IsDynamicTextureLive();

private:
	void SendStartSessionRequest();
	void OnFramesPackReceived( BYTE* aData, int aDataSize);
	

private:
	CRemoteDesktopWindow*		viewWindow;
	CDynamicTextureImpl*		dynamicTexture;
	IDesktopSharingDestination*	destination;
	bool						dynamicTextureWasSet;
	int							m_fps;
	int							prevKeyFrameWidth;
	int							prevKeyFrameHeight;
	int							numbersOfTryStartFileSession;
	bool						dynamicTextureWasDeleted;
	bool						dynamicTextureWasNeverSet;
	bool						m_isResizeApply;
	bool						m_isFirstFramePackReceived;
};