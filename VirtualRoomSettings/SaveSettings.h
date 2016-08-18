#pragma once
#include "SingletonTemplate.h"
#include "../Cscl3DWS/RenderManager/Classes/UserSettings.h"

class CSaveSettings : public Singleton<CSaveSettings>
{
protected:
	CSaveSettings(){}
	friend class Singleton<CSaveSettings>;

public:
	CUserSettings* GetUserSettings();
	void NotifyAboutChangeSettings();
	void NotifyPlayer(HANDLE ahProcess, HWND ahWnd);

private:
	void InformExternalPlayer();
	

	CUserSettings	m_userSettings;
};
