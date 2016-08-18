#include "StdAfx.h"
#include "KinectPointer.h"

#define VERY_BIG_COORD			20000

CKinectPointer::CKinectPointer():
	m_ppMan(NULL),
	m_displayWidth(0),
	m_displayHeight(0),
	m_displayOffsetX(0)
{
	SetInteractiveBoardMonitorID(g->rs.GetInt(INTERACTIVE_BOARD_MONITOR_ID));
}

unsigned int CKinectPointer::GetLParam(short x, short y)
{
	unsigned int lParam;
	*((short*)&lParam) = x;
	*(((short*)&lParam) + 1) = y;

	return lParam;
}

void CKinectPointer::SetPowerPointManipulator(CKinectPowerPointManipulator* ppMan)
{
	assert(ppMan);
	m_ppMan = ppMan;
}

void CKinectPointer::StopShow()
{	
	g->lw.WriteLn("StopShow");
	if (m_ppMan)
	if (m_ppMan->GetControlAppWindow())
	{
		SendMessage(m_ppMan->GetControlAppWindow(), WM_LBUTTONDOWN, MK_LBUTTON, GetLParam(VERY_BIG_COORD, VERY_BIG_COORD));
	}
}

void CKinectPointer::ShowPointer(float x, float y)
{
	g->lw.WriteLn("Show pointer: ", x, " ", y);
	if (m_ppMan)
	if (m_ppMan->GetControlAppWindow())
	{
		short _x = (short)(x * m_displayWidth + m_displayOffsetX);
		short _y = (short)(y * m_displayHeight);
		SendMessage(m_ppMan->GetControlAppWindow(), WM_LBUTTONDOWN, MK_LBUTTON, GetLParam(_x, _y));
		g->lw.WriteLn("Show pointer end");
	}
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC /*hdcMonitor*/, LPRECT /*lprcMonitor*/, LPARAM dwData)
{
	std::vector<MONITORINFOEX> * monitors = (std::vector<MONITORINFOEX>*)dwData;
	MONITORINFOEX info;
	info.cbSize = sizeof( info);
	if( GetMonitorInfo( hMonitor, &info))
		monitors->push_back( info);
	return TRUE;
}

std::vector<MONITORINFOEX> CKinectPointer::GetMonitors()
{
	std::vector<MONITORINFOEX> monitorsInfo;
	EnumDisplayMonitors( 0, NULL, MonitorEnumProc, (LPARAM)&monitorsInfo);
	
	return monitorsInfo;
	
}

void CKinectPointer::SetInteractiveBoardMonitorID(unsigned int _displayID)
{
	int displayID = -1;
	std::vector<MONITORINFOEX> monitors = GetMonitors();	
	for (unsigned int i = 0; i < monitors.size(); i++)
	{
		std::string nameDevice = monitors[i].szDevice;
		if (nameDevice.size() > 0)
		if (nameDevice[nameDevice.size() - 1] == (char)(_displayID + '0'))
		{
			displayID = i;
			break;
		}
	}

	if (-1 == displayID)
	{
		g->lw.WriteLn("could not find display id for kinect pointer");
		displayID = 0;
	}
	
	m_displayWidth = monitors[displayID].rcMonitor.right - monitors[displayID].rcMonitor.left;
	m_displayHeight = monitors[displayID].rcMonitor.bottom - monitors[displayID].rcMonitor.top;
	m_displayOffsetX = monitors[displayID].rcMonitor.left;

	g->lw.WriteLn("interactive board monitor: ", displayID);
	g->lw.WriteLn("m_displayWidth: ", m_displayWidth);
	g->lw.WriteLn("m_displayHeight: ", m_displayHeight);
	g->lw.WriteLn("m_displayOffsetX: ", m_displayOffsetX);
}

CKinectPointer::~CKinectPointer()
{
}