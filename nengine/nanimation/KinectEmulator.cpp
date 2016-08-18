
#include "StdAfx.h"
#include "KinectEmulator.h"

CKinectEmulator::CKinectEmulator()
{
}

bool CKinectEmulator::SetData(void* data, unsigned int size)
{
	if (size < MAX_KINECT_SIZE)
	{
		memcpy(m_data, data, size);
		m_size = size;
		return true;
	}
	else
	{
		return false;
	}
}

bool CKinectEmulator::Init()
{
	return true;
}

void CKinectEmulator::UnInit()
{
}

bool CKinectEmulator::IsSkeletonClippedRight()
{
	return false;
}

bool CKinectEmulator::IsSkeletonClippedLeft()
{
	return false;
}

bool CKinectEmulator::IsSkeletonClippedTop()
{
	return false;
}

bool CKinectEmulator::IsSkeletonClippedBottom()
{
	return false;
}


bool CKinectEmulator::SetSeatedMode(bool /*isSeated*/)
{
	return false;
}

bool CKinectEmulator::IsSeated()
{
	float* data = (float*)&m_data;
	bool* _data = (bool*)&data[66];
	_data++;
	return *_data;
}

void CKinectEmulator::SetTrackingParams(bool /*isSkeletonTracking*/, bool /*isFaceTracking*/)
{
}

void CKinectEmulator::ClearData()
{
	memset(m_data, 0, MAX_KINECT_SIZE);
}

float* CKinectEmulator::GetSkeletonXYZ()
{
	float* data = (float*)&m_data;
	return &data[6];
}

float* CKinectEmulator::GetSkeletonXY()
{
	return NULL;
}

bool CKinectEmulator::IsFoundSkeleton()
{
	float* data = (float*)&m_data;
	return *(bool*)&data[66];
}

float CKinectEmulator::GetBipTrackingQuality()
{
	float* data = (float*)&m_data;
	return data[0];
}

float CKinectEmulator::GetBipsTrackingQuality()
{
	if (m_size >= 66 * 4 + 3)
	{
		float* data = (float*)&m_data;
		return data[66 * 4 + 2];
	}
	else
	{
		return 1.0f;
	}
}


float CKinectEmulator::GetHeadTrackingQuality()
{
	float* data = (float*)&m_data;
	return data[1];
}

float CKinectEmulator::GetLeftArmTrackingQuality()
{
	float* data = (float*)&m_data;
	return data[2];
}

float CKinectEmulator::GetRightArmTrackingQuality()
{
	float* data = (float*)&m_data;
	return data[3];
}

float CKinectEmulator::GetLeftFootTrackingQuality()
{
	float* data = (float*)&m_data;
	return data[4];
}

float CKinectEmulator::GetRightFootTrackingQuality()
{
	float* data = (float*)&m_data;
	return data[5];
}
	
bool CKinectEmulator::GetHeadPosition(float& pitch, float& yaw, float& roll)
{
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
	return false;
}

bool CKinectEmulator::IsTwoSkeletonsFound()
{
	return false;
}

bool CKinectEmulator::IsFTSuccessful()
{
	float* data = (float*)&m_data;
	return *(bool*)&data[103];
}

void CKinectEmulator::GetHeadRotation(float * rotation, float * translation)
{
	float* data = (float*)&m_data;
	memcpy(rotation, &data[91], sizeof(float) * 3);
	memcpy(translation, &data[94], sizeof(float) * 3);
}

void CKinectEmulator::GetAUCoeffs(float * coeffs, unsigned int * count)
{
	float* data = (float*)&m_data;
	memcpy(coeffs, &data[97], sizeof(float) * 6);
	unsigned char * udata = (unsigned char*)&m_data;
	*count = udata[103 * 4 + 1];
}

CKinectEmulator::~CKinectEmulator()
{
}