#pragma once

#include "ikinect.h"
#include "NuiApi.h"
#include "FaceTrackLib.h"

typedef struct _SKELETON_INFO
{
	float x;
	float y;
	float z;
	int id;
} SKELETON_INFO;

class CSkeletalViewerApp: public IKinect
{
public:
	CSkeletalViewerApp();
	~CSkeletalViewerApp();

	HRESULT			Nui_Init();
	void			Nui_UnInit();

	// реализация итерфейса IKinect
	float*		GetSkeletonXYZ();
	float*		GetSkeletonXY();
	bool		Init();
	void		UnInit();
	bool        IsFoundSkeleton();
	bool		SetSeatedMode(bool isSeated);
	void		SetTrackingParams(bool isSkeletonTracking, bool isFaceTracking);
	bool		GetHeadPosition(float& pitch, float& yaw, float& roll);	

	// псевдо-приватные функции
	void SetRotations(const float pitchDegrees, const float yawDegrees, const float rollDegrees);

	bool IsSkeletonClippedRight();
	bool IsSkeletonClippedLeft();
	bool IsSkeletonClippedTop();
	bool IsSkeletonClippedBottom();

	float GetBipTrackingQuality();
	float GetBipsTrackingQuality();
	float GetHeadTrackingQuality();
	float GetLeftArmTrackingQuality();
	float GetRightArmTrackingQuality();
	float GetLeftFootTrackingQuality();
	float GetRightFootTrackingQuality();

	bool IsTwoSkeletonsFound();

	void GetGlobalRotation(float& rotX, float& rotY, float& rotZ, float& rotW);

	void EnableVideo(bool isEnabled);
	void GetNextVideoFrame(void** data);
	unsigned int GetNextVideoFrameSize();
	bool IsNextVideoFrameAvailable();

	void GetNextDepthFrame(void** data);
	unsigned int GetNextDepthFrameSize();

	int GetPlayerIndex();

	void SetSpecificPixelsChannel(char channelID);
	unsigned int GetSpecificPixelsCount();
	void	SetColorDetectionKoefs(int mainColorKoef, int* otherColorKoef1, int* otherColorKoef2);


	void SetDebugMode(unsigned int debugMode);

	void MapDepthFrameToColorFrame(void);
	void GetNextMappedDepthFrame(void** data);
	void GetAUCoeffs(float * coeffs, unsigned int * count);
	void GetHeadRotation(float * rotation, float * translation);
	bool IsFTSuccessful() { return m_isFaceTrackingSuccessful; }
	void EnableFaceTracking();
	void DisableFaceTracking();

private:
	INuiCoordinateMapper*		m_pMapper;
	
	static DWORD WINAPI     Nui_SkeletonProcessThread(LPVOID pParam);
	static DWORD WINAPI     Nui_DepthProcessThread(LPVOID pParam);
	static DWORD WINAPI     Nui_FaceProcessThread(LPVOID pParam);

	bool Nui_GotColorAlert( );
	void Nui_GotSkeletonAlert( );
	bool Nui_GotDepthAlert( );

	template <int T>
	void UpdateSpecificPixelsCount();

	int m_otherColorKoef1, m_otherColorKoef2, m_mainColorKoef;
	int* m_otherColorKoef1Arr;
	int* m_otherColorKoef2Arr;

	// thread handling
	HANDLE			m_hThNuiProcess;
	HANDLE			m_hThNuiProcess2;
	HANDLE			m_hThNuiProcess3;
	HANDLE			m_hEvNuiProcessStop;
	HANDLE			m_hEvFTProcessStop;

	HANDLE			m_hNextSkeletonEvent;
	HANDLE			m_hNextColorFrameEvent;
	HANDLE			m_pVideoStreamHandle;

	HANDLE			m_hNextDepthFrameEvent;
	HANDLE			m_pDepthStreamHandle;

	SKELETON_INFO	m_skeleton;
	bool			m_isFoundSkeleton;

	float			m_exportCoords[60];
	float			m_coords[60];
	float			m_skeletonCoords[40];


	bool			m_isFaceTracking;
	bool			m_isSkeletonTracking;
	bool			m_isVideoEnabled;

	float			m_pitch;
	float			m_yaw;
	float			m_roll;

	bool			m_isSeated;
	bool			m_isFacingUser;
	bool			m_isTwoSkeletonsFound;

	int				m_samePositionCount;
	float			m_reportedPitchAverage;
	float			m_reportedRollAverage;
	float			m_reportedYawAverage;

	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_videoCS;
	CRITICAL_SECTION m_depthCS;

	unsigned int	m_qualityFlags;
	float m_headQuality;
	float m_leftArmQuality;
	float m_rightArmQuality;
	float m_leftFootQuality;
	float m_rightFootQuality;
	float m_hipQuality;
	float m_hipsQuality;

	float m_globalRotationX;
	float m_globalRotationY;
	float m_globalRotationZ;
	float m_globalRotationW;	

	const unsigned char m_videoFrames;
	unsigned char** m_videoData;
	unsigned int m_videoDataSize;

	unsigned short* m_depthData;
	unsigned int m_depthDataSize;

	unsigned short* m_mappedDepthData;

	NUI_DEPTH_IMAGE_PIXEL* m_depthPixels;
	unsigned int m_depthPixelsSize;
	NUI_DEPTH_IMAGE_POINT* m_depthPoints;
	unsigned int m_depthPointsSize;

	long long* m_colorTimeStamp;
	long long m_depthTimeStamp;

	bool m_isVideoFrameOld;

	int m_playerIndex;

	int m_channelID;
	int m_otherChannelID1;
	int m_otherChannelID2;

	unsigned int m_specificPixelsCount;

	unsigned int m_debugMode;

	float m_rotation[3];
	float m_translations[3];
	float m_AUcoeffs[6];
	unsigned int m_AUcount;
	bool m_isFaceTrackingSuccessful;

	// Skeletal drawing
	D2D1_POINT_2F            m_points[NUI_SKELETON_POSITION_COUNT];
private:
	D2D1_POINT_2F SkeletonToScreen( Vector4 skeletonPoint, int width, int height );
	bool GetHeadPoints(FT_VECTOR3D* hint);
	unsigned short GetBestMatchingColorFrameIndex();
};




