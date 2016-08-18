#pragma once
#include "SharingContext.h"
#include "selectors/IDesktopSelector.h"

class SharingWM_Handler
{
public:
	SharingWM_Handler(void);
	~SharingWM_Handler(void);

	void SetContext(SHARING_CONTEXT* apContext);
	void SetSharingSelector(IDesktopSelector* aSelector);
	unsigned char GetHardwareScanCode(unsigned int aScanCode);

	void SetRemoteControlMode(int aFullControl);
	void OnRemoteCommandReceived( BYTE* aData, int aDataSize);
	bool NotifyChildWindow(HWND hChildWindow, LPARAM Param, int currentMessage, RECT mainRect);
	HWND FindParentWindow(HWND hChild);

private:
	bool IsSystemKey(unsigned int aScanCode);
private:
	SHARING_CONTEXT* m_context;

	IDesktopSelector* m_selector;

	HWND hLastActiveWindow;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_ctrlPressed;
	int m_fullControlMode;
};
