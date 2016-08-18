# pragma  once

#define KINECT_VIDEO_WIDTH		640
#define KINECT_VIDEO_HEIGHT		480

#define DEBUG_MODE_NO_DEBUG						0
#define DEBUG_MODE_SHOW_MARKERS					1
#define DEBUG_MODE_SHOW_MARKERS_BY_CATEGORY		2

class IKinect
{
public:
	virtual	bool		Init() = 0;
	virtual	void		UnInit() = 0;
	virtual bool		SetSeatedMode(bool isSeated) = 0;
	virtual void		SetTrackingParams(bool isSkeletonTracking, bool isFaceTracking) = 0;
	virtual float*		GetSkeletonXYZ() = 0;
	virtual float*		GetSkeletonXY() = 0;
	virtual bool        IsFoundSkeleton() = 0;
	virtual float		GetBipTrackingQuality() = 0;
	virtual float		GetBipsTrackingQuality() = 0;
	virtual float		GetHeadTrackingQuality() = 0;
	virtual float		GetLeftArmTrackingQuality() = 0;
	virtual float		GetRightArmTrackingQuality() = 0;
	virtual float		GetLeftFootTrackingQuality() = 0;
	virtual float		GetRightFootTrackingQuality() = 0;
	virtual bool		GetHeadPosition(float& pitch, float& yaw, float& roll) = 0;
	virtual bool		IsTwoSkeletonsFound() = 0;
	virtual void		EnableVideo(bool isEnabled) = 0;
	virtual void		GetNextVideoFrame(void** data) = 0;
	virtual unsigned int GetNextVideoFrameSize() = 0;
	virtual bool		IsNextVideoFrameAvailable() = 0;
	virtual void		GetNextDepthFrame(void** data) = 0;
	virtual unsigned int GetNextDepthFrameSize() = 0;
	virtual int			GetPlayerIndex() = 0;
	virtual bool		IsSkeletonClippedRight() = 0;
	virtual bool		IsSkeletonClippedLeft() = 0;
	virtual bool		IsSkeletonClippedTop() = 0;
	virtual bool		IsSkeletonClippedBottom() = 0;
	virtual void		SetSpecificPixelsChannel(char channelID) = 0;
	virtual unsigned int GetSpecificPixelsCount() = 0;
	virtual void		SetDebugMode(unsigned int debugMode) = 0;
	virtual void		SetColorDetectionKoefs(int mainColorKoef, int* otherColorKoef1, int* otherColorKoef2) = 0;
	virtual void		GetNextMappedDepthFrame(void** data) = 0;
	virtual void		GetAUCoeffs(float * coeffs, unsigned int * count) = 0;
	virtual void		GetHeadRotation(float * rotation, float * translation) = 0;
	virtual bool		IsFTSuccessful() = 0;
	virtual void		EnableFaceTracking() = 0;
	virtual void		DisableFaceTracking() = 0;
};