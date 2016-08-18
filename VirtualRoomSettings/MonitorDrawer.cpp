#include "StdAfx.h"
#include ".\monitordrawer.h"
#include "screenDraw.h"
#include "showImageCodes.h"
#include "SaveSettings.h"
#include "Time.h"

#include <GdiPlus.h>
using namespace Gdiplus;


const std::wstring IMG_FOLDER = L"Images/";

CScreenDrawWindow* CMonitorDrawer::GetActiveShowWindow()
{
	return m_currWnd;
}

void CMonitorDrawer::SetActiveShowWindow(CScreenDrawWindow* aWnd)
{
	m_currWnd = aWnd;
}


void CMonitorDrawer::StopShow()
{
	if (m_currWnd != NULL)
	{
		if (m_currWnd->IsWindow()) {
			m_currWnd->HideWindow();
			m_currWnd->DestroyWindow();
		}
	}
}

void CMonitorDrawer::ShowImage(std::wstring imgSrc, int showTime, int monitorID, vecMonitorsInfo monitorsInfo)
{
	Bitmap* bmp = new Bitmap(imgSrc.c_str());
	CScreenDrawWindow* drawMonitorWnd = new CScreenDrawWindow();

	MONITORINFOEX selectedMonitor = monitorsInfo[monitorID];

	drawMonitorWnd->CreateOnMonitor(selectedMonitor);
	drawMonitorWnd->SetImgPosition(CENTER_POSITION);
	drawMonitorWnd->SetImageToDraw(bmp);
	drawMonitorWnd->SetTimeToShow(showTime);
	drawMonitorWnd->ShowFullScreen();

	SetActiveShowWindow(drawMonitorWnd);
}

void CMonitorDrawer::ShowMonitorImageByCode(int aCode, int aTimeStep)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	vecMonitorsInfo monitorsInfo = GetMonitors();

	int monitorID = GetInteractiveBoardMonitorID();
	if (monitorID >= monitorsInfo.size() )
	{
		monitorID = 0;
	}

	StopShow();

	switch (aCode)
	{

	case SHOW_IDENTIFY_MONITOR:	
		{
			for (unsigned int i=0; i < monitorsInfo.size(); i++)
			{
				wchar_t buf[50];
				_itow(i+1, buf, 10);
				std::wstring imgSrc = IMG_FOLDER + std::wstring(L"mon") + buf;
				imgSrc += L".png";
				Bitmap* bmp = new Bitmap(imgSrc.c_str());
				CScreenDrawWindow* drawMonitorWnd = new CScreenDrawWindow();

				int x = (monitorsInfo[i].rcMonitor.right - monitorsInfo[i].rcMonitor.left)/2;
				x = x - bmp->GetWidth()/2;
				int y = (monitorsInfo[i].rcMonitor.bottom - monitorsInfo[i].rcMonitor.top)/2;
				y = y - bmp->GetHeight()/2;


				drawMonitorWnd->CreateOnMonitor(monitorsInfo[i]);
				drawMonitorWnd->SetImgPosition(x, y);
				drawMonitorWnd->SetImageToDraw(bmp);
				drawMonitorWnd->SetTimeToShow(STATUS_TIME * 1000);
				drawMonitorWnd->ShowFullScreen();
				//SetActiveShowWindow(drawMonitorWnd);
			}		
		}
		break;

	case SHOW_COUNT_BEFORE:
		{
			wchar_t buf[50];
			//int cntNum = 30;
			std::vector<Bitmap*> bmpArray;
			//for (int i = cntNum; i > 0; i--)
			for (int i = aTimeStep; i > 0; i--)
			{
				_itow(i, buf, 10);
				std::wstring imgSrc = IMG_FOLDER + std::wstring(L"cnt") + buf;
				imgSrc += L".png";
				Bitmap* bmp = new Bitmap(imgSrc.c_str());
				bmpArray.push_back(bmp);
			}
			CScreenDrawWindow* drawMonitorWnd = new CScreenDrawWindow();

			MONITORINFOEX selectedMonitor = monitorsInfo[monitorID];

			drawMonitorWnd->CreateOnMonitor(selectedMonitor);
			drawMonitorWnd->SetImgPosition(CENTER_POSITION);
			drawMonitorWnd->SetImagesToDraw(bmpArray);
			drawMonitorWnd->SetTimeToShow(TIME_PROGRESS * 1000);
			drawMonitorWnd->ShowFullScreen();

			SetActiveShowWindow(drawMonitorWnd);
		}
		break;

	case SHOW_TOUCH_POINTS:
		{
			std::wstring imgSrc = IMG_FOLDER + L"touch.png";
			ShowImage(imgSrc, TIME_TOUCH_POINTS * 1000, monitorID, monitorsInfo);
/*
			Bitmap* bmp = new Bitmap(imgSrc.c_str());
			CScreenDrawWindow* drawMonitorWnd = new CScreenDrawWindow();

			MONITORINFOEX selectedMonitor = monitorsInfo[monitorID];

			drawMonitorWnd->CreateOnMonitor(selectedMonitor);
			drawMonitorWnd->SetImgPosition(CENTER_POSITION);
			drawMonitorWnd->SetImageToDraw(bmp);
			drawMonitorWnd->SetTimeToShow(5000);
			drawMonitorWnd->ShowFullScreen();

			SetActiveShowWindow(drawMonitorWnd);
*/
		}
		break;

	case SHOW_SUCCESS_RECOGNIZE:
		{
			std::wstring imgSrc = IMG_FOLDER + L"recognize_success.png";
			ShowImage(imgSrc, STATUS_TIME * 1000, monitorID, monitorsInfo);
		}
		break;

	case SHOW_FAIL_RECOGNIZE:
		{
			std::wstring imgSrc = IMG_FOLDER + L"recognize_fail.png";
			ShowImage(imgSrc, STATUS_TIME * 1000, monitorID, monitorsInfo);
		}
		break;

	}

}

int CMonitorDrawer::GetInteractiveBoardMonitorID()
{
	int monitorID = -1;
	int size = 10;

	CSaveSettings* sharedSaveSettings =  CSaveSettings::Instance();
	CUserSettings* settings = sharedSaveSettings->GetUserSettings();
	settings->LoadGlobal();
	std::string val = "";
	val = settings->GetSetting("monitor_for_interactive_board");

	if (val.size() != 0)
	{
		monitorID = atoi(val.c_str());
	}

	sharedSaveSettings->FreeInst();
	if (monitorID <= 0)
		return 0;
	return monitorID-1;
}

vecMonitorsInfo CMonitorDrawer::GetMonitors()
{
	vecMonitorsInfo monitorsInfo;
	EnumDisplayMonitors( 0, NULL, MonitorEnumProc, (LPARAM)&monitorsInfo);

	return monitorsInfo;

}
