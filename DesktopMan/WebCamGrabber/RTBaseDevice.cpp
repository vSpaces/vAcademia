#include "StdAfx.h"
#include ".\rtbasedevice.h"

CRTBaseDevice::CRTBaseDevice(void):MP_STRING_INIT(m_displayName), MP_STRING_INIT(m_name)
{
	isStoping = false;
	isStarted = false;
	m_displayName = "";
	m_name = "";
}

CRTBaseDevice::~CRTBaseDevice(void)
{
}
void CRTBaseDevice::NeedStoping()
{
	isStoping = true;
}

bool CRTBaseDevice::IsNeedStoping()
{
	return isStoping;
}

bool CRTBaseDevice::IsStarted()
{
	return isStarted;
}

void CRTBaseDevice::Stop() 
{
	isStarted = false;
	isStoping = false;
	StopImpl();
}

int CRTBaseDevice::Start(HWND ahWnd) 
{
	isStarted = true;
	isStoping = false;
	m_hWnd = ahWnd;
	return StartImpl(ahWnd);
}

std::string	CRTBaseDevice::GetName()
{
	return m_name;
}

std::string	CRTBaseDevice::GetDisplayDeviceName()
{
	return m_displayName;
}