
#include "stdafx.h"
#include <windows.h>
#include "TimeProcessor.h"

CTimeProcessor::CTimeProcessor()
{
}

void CTimeProcessor::Init()
{
	LARGE_INTEGER s;
	QueryPerformanceFrequency(&s);
	m_freq = s.QuadPart;
}

__int64 CTimeProcessor::GetTime()
{
	LARGE_INTEGER s;
	QueryPerformanceCounter(&s);

	return s.QuadPart;
}

void CTimeProcessor::Start()
{
	m_startTime = GetTime();
}

__int64 CTimeProcessor::GetTickCount()
{
	if (m_freq == 0)
	{
		m_freq = 1;
	}

	return ((GetTime() - m_startTime) * 1000 / m_freq);
}

__int64 CTimeProcessor::GetMicroTickCount()
{
	if (m_freq == 0)
	{
		m_freq = 1;
	}

	return ((GetTime() - m_startTime) * 1000000 / m_freq);
}

CTimeProcessor::~CTimeProcessor()
{
}