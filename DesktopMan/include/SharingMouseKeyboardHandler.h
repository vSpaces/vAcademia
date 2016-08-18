#pragma once
#include "SessionFactory.h"

class CSharingMouseKeyboardHandler : public IMouseKeyboardHandler
{
public:
	CSharingMouseKeyboardHandler(CSharingSessionFactory* aSessionsFactory);
	~CSharingMouseKeyboardHandler(void);
	
	void SendCommand(unsigned char aType, unsigned short aFirstParam, UINT aSecondParam, unsigned char aCode);
	

public:
	virtual void SetActiveTarget(LPCSTR alpTarget);
	virtual void SetRealityId(unsigned int aRealityID);
	virtual void OnLeftMouseDown(UINT aFirstParam, UINT aSecondParam);
	virtual void OnLeftMouseUp(UINT aFirstParam, UINT aSecondParam);
	virtual void OnLeftMouseDblClk(UINT aFirstParam, UINT aSecondParam);
	virtual void OnRightMouseDown(UINT aFirstParam, UINT aSecondParam);
	virtual void OnRightMouseUp(UINT aFirstParam, UINT aSecondParam);
	virtual void OnRightMouseDblClk(UINT aFirstParam, UINT aSecondParam);
	virtual void OnMouseMove(UINT aFirstParam, UINT aSecondParam);
	virtual void OnKeyUp(UINT aFirstParam, UINT aSecondParam);
	virtual void OnKeyDown(UINT aFirstParam, UINT aSecondParam);

private: 
	CSharingSessionFactory*		sessionsFactory;
	LPCSTR sessionName;
	UINT lastMouseX;
	UINT lastMouseY;
	unsigned int realityID;
};
