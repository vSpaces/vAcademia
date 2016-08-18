
#include "StdAfx.h"
#include "PerformanceMonitor.h"
#include "GlobalSingletonStorage.h"

#define MAX_STORED_FRAMES_COUNT		2000
#define CLEARED_FRAMES_COUNT		250

CPerformanceMonitor::CPerformanceMonitor():
	m_frameEndTime(0),
	m_lastBlockTime(0),
	m_frameStartTime(0),
	m_lastBlockPerformed(0),
	MP_VECTOR_INIT(m_framesInfo)
{
	m_framesInfo.reserve(MAX_STORED_FRAMES_COUNT);
}

void CPerformanceMonitor::StartFrame()
{
	m_frameStartTime = g->tp.GetMicroTickCount();
	m_lastBlockTime = m_frameStartTime;
	m_lastBlockPerformed = 0;

	memset(m_frameBlockTime, 0, sizeof(m_frameBlockTime));
	memset(m_customBlockTime, 0, sizeof(m_customBlockTime));
	memset(m_customMultipleBlockTime, 0, sizeof(m_customMultipleBlockTime));
}

void CPerformanceMonitor::EndFrame()
{
	m_frameEndTime = g->tp.GetMicroTickCount();
	m_frameBlockTime[m_lastBlockPerformed + 1] = m_frameEndTime - m_lastBlockTime;

	SFrameInfo* frameInfo = MP_NEW(SFrameInfo);
	frameInfo->frameTime = (unsigned int)(m_frameEndTime - m_frameStartTime);
	memcpy(&frameInfo->blockTimings[0], m_frameBlockTime, sizeof(m_frameBlockTime));

	for (unsigned int customBlockID = 0; customBlockID < CUSTOM_BLOCK_COUNT; customBlockID++)
	{
		frameInfo->customBlockTimings[customBlockID] = (unsigned int)(m_customBlockTime[customBlockID * 2 + 1] - 
			m_customBlockTime[customBlockID * 2 + 0]);

		frameInfo->customMultipleBlockTimings[customBlockID]	 = m_customMultipleBlockTime[customBlockID * 4 + 2];
		frameInfo->customMultipleBlockIterationCount[customBlockID] = (int)m_customMultipleBlockTime[customBlockID * 4 + 3];
	}

	m_framesInfo.push_back(frameInfo);

	if (m_framesInfo.size() == MAX_STORED_FRAMES_COUNT)
	{
		DeleteFrameInfoRangeFromStart(CLEARED_FRAMES_COUNT);
	}

    int frameCount = 0;
	int frameTimeSum = 0;
	MP_VECTOR<SFrameInfo*>::reverse_iterator rit = m_framesInfo.rbegin();
	MP_VECTOR<SFrameInfo*>::reverse_iterator ritEnd = m_framesInfo.rend();

	for ( ; rit != ritEnd; rit++, frameCount++)
	{
		frameTimeSum += (*rit)->frameTime;
		if (frameTimeSum > 1000000)
		{
			break;
		}
	}

	float fps = (float)(frameCount + 1) / ((float)frameTimeSum / 1000000.0f);

	if (fps < g->rs.GetInt(MINIMAL_NORMAL_FPS))
	{
		g->lw.WriteLn("[Slow frame]");

		int frameID = 0;
		frameTimeSum = 0;

		rit = m_framesInfo.rbegin();
		for ( ; rit != ritEnd; rit++, frameID++)
		{
			frameTimeSum += (*rit)->frameTime;

			if (frameID > 10)
			{
				g->lw.WriteLn("all frames count: ", m_framesInfo.size(), " terminate to minimaze log size");
				break;
			}
			
			g->lw.WriteLn("**frame ¹", frameID);
			g->lw.WriteLn("frame time: ", (*rit)->frameTime);

			g->lw.Write("frame blocks time: ");
			for (int blockID = 0; blockID < FRAME_BLOCK_COUNT; blockID++)
			{
				g->lw.Print(" [", blockID, "] ", (unsigned int)((*rit)->blockTimings[blockID]));
			}
			g->lw.PrintLn(" ");

			g->lw.Write("custom blocks time: ");
			for (int blockID = 0; blockID < CUSTOM_BLOCK_COUNT; blockID++)
			{
				g->lw.Print(" [", blockID, "] ", (unsigned int)((*rit)->customBlockTimings[blockID]));
			}
			g->lw.PrintLn(" ");

			g->lw.Write("custom multiple blocks time: ");
			for (int blockID = 0; blockID < CUSTOM_BLOCK_COUNT; blockID++)
			{
				g->lw.Print(" [", blockID, "] ", (unsigned int)((*rit)->customMultipleBlockTimings[blockID]), "/", (*rit)->customMultipleBlockIterationCount[blockID], " ");
			}
			g->lw.PrintLn(" ");

			if (frameTimeSum > 1000000)
			{
				break;
			}
		}

		DeleteFrameInfoRangeFromStart(m_framesInfo.size());
	}
}

void CPerformanceMonitor::FrameBlockPerformed(const unsigned int blockID)
{
	m_frameBlockTime[blockID] = g->tp.GetMicroTickCount() - m_lastBlockTime;
	m_lastLastBlockTime = m_lastBlockTime;
	m_lastBlockTime = g->tp.GetMicroTickCount();
	m_lastBlockPerformed = blockID;
}
    
void CPerformanceMonitor::CustomBlockStart(const unsigned int customBlockID)
{
	m_customBlockTime[customBlockID * 2 + 0] = g->tp.GetMicroTickCount();
}

void CPerformanceMonitor::CustomBlockEnd(const unsigned int customBlockID)
{
	m_customBlockTime[customBlockID * 2 + 1] = g->tp.GetMicroTickCount();
}

void CPerformanceMonitor::DeleteFrameInfoRangeFromStart(unsigned int frameCount)
{
	MP_VECTOR<SFrameInfo *>::iterator it = m_framesInfo.begin();
	MP_VECTOR<SFrameInfo *>::iterator itEnd = m_framesInfo.begin() + frameCount;

	for ( ; it != itEnd; it++)
	{
		MP_DELETE((*it));
	}

	m_framesInfo.erase(m_framesInfo.begin(), itEnd);
}

void CPerformanceMonitor::CustomMultipleBlockStart(const unsigned int customBlockID)
{
	m_customMultipleBlockTime[customBlockID * 4 + 0] = g->tp.GetMicroTickCount();
}

void CPerformanceMonitor::CustomMultipleBlockEnd(const unsigned int customBlockID)
{
	m_customMultipleBlockTime[customBlockID * 4 + 1] = g->tp.GetMicroTickCount();
	m_customMultipleBlockTime[customBlockID * 4 + 2] += (m_customMultipleBlockTime[customBlockID * 4 + 1] - m_customMultipleBlockTime[customBlockID * 4 + 0]);
	m_customMultipleBlockTime[customBlockID * 4 + 3]++;
}

CPerformanceMonitor::~CPerformanceMonitor()
{
	MP_VECTOR<SFrameInfo *>::iterator it = m_framesInfo.begin();
	MP_VECTOR<SFrameInfo *>::iterator itEnd = m_framesInfo.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE(*it);		
	}

	m_framesInfo.clear();
}