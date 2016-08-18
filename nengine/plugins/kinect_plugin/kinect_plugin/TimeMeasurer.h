
#pragma once

#define BLOCK_ID_CAPTURE_SKELETON			1
#define BLOCK_ID_CAPTURE_VIDEO_AND_DEPTH	2
#define BLOCK_ID_FIND_PIXELS				3

#include <string>
#include "../../../ncommon/TimeProcessor.h"
#include "SimpleLogWriter.h"

class CTimeMeasurer
{
public:
	CTimeMeasurer(int blockID, CTimeProcessor* timeProcessor, CSimpleLogWriter* log);
	~CTimeMeasurer();

private:
	std::string GetBlockName();

	int m_blockID;
	CTimeProcessor* m_timeProcessor;
	CSimpleLogWriter* m_log;
	__int64 m_startTime;
};