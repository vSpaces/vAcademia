#include "StdAfx.h"
#include "TimeMeasurer.h"

CTimeMeasurer::CTimeMeasurer(int blockID, CTimeProcessor* timeProcessor, CSimpleLogWriter* log):
	m_blockID(blockID),
	m_timeProcessor(timeProcessor),
	m_log(log)
{
	m_startTime = m_timeProcessor->GetTickCount();
}

std::string CTimeMeasurer::GetBlockName()
{
	if (m_blockID == BLOCK_ID_CAPTURE_SKELETON)
	{
		return "Capture skeleton";
	}
	else if (m_blockID == BLOCK_ID_CAPTURE_VIDEO_AND_DEPTH)
	{
		return "Capture video and depth";
	}
	else if (m_blockID == BLOCK_ID_FIND_PIXELS)
	{
		return "Find pixels";
	}

	return "Unknown block";
}

CTimeMeasurer::~CTimeMeasurer()
{
	int allTime = m_timeProcessor->GetTickCount() - m_startTime;
	if (allTime > 0)
	{
		m_log->Write(GetBlockName(), allTime);
	}
}