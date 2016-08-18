
#pragma once

#include "CommonInfoHeader.h"

#include <vector>

#define FRAME_BLOCK_COUNT	9

#define CUSTOM_BLOCK_PHYSICS_UPDATE						0
#define CUSTOM_BLOCK_TEXTURES_LOADING					1
#define CUSTOM_BLOCK_OBJECTS_MANAGEMENT					2
#define CUSTOM_BLOCK_UPDATE_DYNAMIC_TEXTURES			3
#define CUSTOM_BLOCK_TEXTURES_READING_CACHE				4
#define CUSTOM_BLOCK_TEXTURES_LOADING2					5
#define CUSTOM_BLOCK_TEXTURES_LOADING_SAVE_COMPRESSED	6
#define CUSTOM_BLOCK_CAMERA_COLLISION					7
#define CUSTOM_BLOCK_END_RENDERING						8
#define CUSTOM_BLOCK_END_RENDERING_2D					9
#define CUSTOM_BLOCK_END_RENDERING_3D					10
#define CUSTOM_BLOCK_SAO_LOADING						11

#define CUSTOM_MULTIPLE_BLOCK_ANIMATION_PROCESSING		0
#define CUSTOM_MULTIPLE_BLOCK_ANIMATION_VBO_UPDATE		1

#define CUSTOM_BLOCK_COUNT								12

typedef struct _SFrameInfo
{
	unsigned int frameTime;
	__int64 blockTimings[FRAME_BLOCK_COUNT];
	__int64 customBlockTimings[CUSTOM_BLOCK_COUNT];
	__int64 customMultipleBlockTimings[CUSTOM_BLOCK_COUNT];
	int customMultipleBlockIterationCount[CUSTOM_BLOCK_COUNT];
} SFrameInfo;

class CPerformanceMonitor
{
public:
	CPerformanceMonitor();
	~CPerformanceMonitor();

	void StartFrame();
	void EndFrame();

	void FrameBlockPerformed(const unsigned int blockID);
    
    void CustomBlockStart(const unsigned int customBlockID);    
	void CustomBlockEnd(const unsigned int customBlockID);  

	void CustomMultipleBlockStart(const unsigned int customBlockID);
	void CustomMultipleBlockEnd(const unsigned int customBlockID);

private:
	void DeleteFrameInfoRangeFromStart(unsigned int frameCount);

	MP_VECTOR<SFrameInfo *> m_framesInfo;

	__int64 m_frameStartTime;
	__int64 m_frameEndTime;
	
	__int64 m_lastBlockTime;
	__int64 m_lastLastBlockTime;
	__int64 m_lastBlockPerformed;
	
	__int64 m_customMultipleBlockTime[CUSTOM_BLOCK_COUNT * 4];
	__int64 m_customBlockTime[CUSTOM_BLOCK_COUNT * 2];
	__int64 m_frameBlockTime[FRAME_BLOCK_COUNT];
};