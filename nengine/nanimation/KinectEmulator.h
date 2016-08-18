
#pragma once

#define MAX_KINECT_SIZE 500

#include "IKinect.h"

class CKinectEmulator : public IKinect
{
public:
	CKinectEmulator();
	~CKinectEmulator();

	bool SetData(void* data, unsigned int size);
	void ClearData();

	bool Init();
	void UnInit();
	bool SetSeatedMode(bool isSeated);
	bool IsSeated();
	void SetTrackingParams(bool isSkeletonTracking, bool isFaceTracking);
	float* GetSkeletonXYZ();
	bool IsFoundSkeleton();
	int GetPlayerIndex() { return 0; }
	float GetBipTrackingQuality();
	float GetBipsTrackingQuality();
	float GetHeadTrackingQuality();
	float GetLeftArmTrackingQuality();
	float GetRightArmTrackingQuality();
	float GetLeftFootTrackingQuality();
	float GetRightFootTrackingQuality();
	bool IsTwoSkeletonsFound();
	bool GetHeadPosition(float& pitch, float& yaw, float& roll);
	void EnableVideo(bool /*isEnabled*/) {} ;
	void GetNextVideoFrame(void** /*data*/) {} ;
	unsigned int GetNextVideoFrameSize() { return 0; }
	bool IsNextVideoFrameAvailable() { return true; }
	void GetNextDepthFrame(void** /*data*/) {} ;
	unsigned int GetNextDepthFrameSize() { return 0; }
	bool AreMarkersNotDetected() { return false; }
	bool IsSkeletonClippedRight();
	bool IsSkeletonClippedLeft();
	bool IsSkeletonClippedTop();
	bool IsSkeletonClippedBottom();
	void SetSpecificPixelsChannel(char /*channelID*/) {};
	unsigned int GetSpecificPixelsCount() { return 0; };
	float* GetSkeletonXY();
	void SetDebugMode(unsigned int /*mode*/) { };
	void SetColorDetectionKoefs(int /*mainColorKoef*/, int* /*otherColorKoef1*/, int* /*otherColorKoef2*/) {};
	void GetNextMappedDepthFrame(void** /*data*/) {}
	void GetAUCoeffs(float * /*coeffs*/, unsigned int * /*count*/);
	void GetHeadRotation(float * /*rotation*/, float * /*translation*/);
	bool IsFTSuccessful();
	void EnableFaceTracking() {}
	void DisableFaceTracking() {}

private:
	unsigned char m_data[MAX_KINECT_SIZE];
	unsigned int m_size;
};