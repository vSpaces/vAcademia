
#pragma once

#include "KinectPowerPointManipulator.h"

class CKinectPointer
{
public:
	CKinectPointer();
	~CKinectPointer();

	void StopShow();
	void ShowPointer(float x, float y);

	void SetPowerPointManipulator(CKinectPowerPointManipulator* ppMan);

private:
	unsigned int GetLParam(short x, short y);	

	void SetInteractiveBoardMonitorID(unsigned int displayID);

	std::vector<MONITORINFOEX> GetMonitors();

	CKinectPowerPointManipulator* m_ppMan;
		
	unsigned int m_displayWidth;
	unsigned int m_displayHeight;
	unsigned int m_displayOffsetX;
};