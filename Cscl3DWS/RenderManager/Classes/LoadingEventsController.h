
#pragma once

class CLoadingEventsController
{
public:
	CLoadingEventsController();
	~CLoadingEventsController();

	void OnLoginSuccess();
	void OnWindowResized();
	void OnNew3DFrame();

private:
	void SendKeyMessage(int key);

	HWND m_windowHandle;
};