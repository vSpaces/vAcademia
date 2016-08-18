
#pragma once

#include "CommonRenderManagerHeader.h"

#include <vector>

#define PLAYING_OK 1
#define PLAYING_ENDED 2
#define PLAYING_NOT_STARTED 3 

typedef struct _SCameraFrame
{
	float eyeX, eyeY, eyeZ;
	float lookX, lookY, lookZ;
	float upX, upY, upZ;
	unsigned int time;
	bool isCurrentCamera;

	_SCameraFrame(const float _eyeX, const float _eyeY, const float _eyeZ,
		const float _lookX, const float _lookY, const float _lookZ,
		const float _upX, const float _upY, const float _upZ,
		const unsigned int _time)
	{
		eyeX = _eyeX;
		eyeY = _eyeY;
		eyeZ = _eyeZ;
		lookX = _lookX;
		lookY = _lookY;
		lookZ = _lookZ;
		upX = _upX;
		upY = _upY;
		upZ = _upZ;
		time = _time;		
		isCurrentCamera = ((eyeX == 0.0f) && (eyeY == 0.0f) && (eyeZ == 0.0f) &&
			(lookX == 0.0f) && (lookY == 0.0f) && (lookZ == 0.0f) &&
			(upX == 0.0f) && (upY == 0.0f) && (upZ == 0.0f));
	}
} SCameraFrame;

class CCameraTrackController
{
public:
	CCameraTrackController();
	~CCameraTrackController();

	bool LoadTrack(const std::wstring fileName);
	bool LoadTrack( void* apData, int auDataSize);
	void StartTrack();

	int GetCameraParams(CVector3D& eye, CVector3D& lookAt);

	unsigned int GetTrackTime();
	
private:
	bool LoadTrack( TiXmlDocument& doc);

	MP_VECTOR<SCameraFrame *> m_frames;
	__int64 m_startTime; 
	bool m_isPlaying;
};