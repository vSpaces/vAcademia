#pragma once
#include "SingletonTemplate.h"
#include "MonitorIdentify.h"

class CScreenDrawWindow;

class CMonitorDrawer : public Singleton<CMonitorDrawer>
{
protected:
	CMonitorDrawer(){ m_currWnd = NULL;}
	friend class Singleton<CMonitorDrawer>;

public:
	void ShowMonitorImageByCode(int aCode, int aTimeStep);
	void StopShow();
	CScreenDrawWindow* GetActiveShowWindow();
	void SetActiveShowWindow(CScreenDrawWindow* aWnd);

private:
	void ShowImage(std::wstring imgSrc, int showTime, int monitorID, vecMonitorsInfo monitorsInfo);

	int GetInteractiveBoardMonitorID();
	vecMonitorsInfo GetMonitors();

	CScreenDrawWindow* m_currWnd;
};
