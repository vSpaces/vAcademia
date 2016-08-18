
#pragma once

#include "CommonHeader.h"
#include <string>
#include <map>

#define MAX_BLEND_COUNT		16

#define TARGET_EYE_LEFT		0
#define TARGET_EYE_RIGHT	1
#define TARGET_EYE_CLOSE	2
#define TARGET_SMILE		3
#define TARGET_SMILE_START	4
#define TARGET_LIP_SYNC1	5
#define TARGET_LIP_SYNC2	6
#define TARGET_LIP_SYNC3	7
#define TARGET_LIP_SYNC4	8
#define TARGET_LIP_SYNC5	9
#define TARGET_AU0			10
#define TARGET_AU1			11
#define TARGET_AU2			12
#define TARGET_AU3			13
#define TARGET_AU4			14
#define TARGET_AU5			15

#define LEXEM_COUNT			5

class CSkeletonAnimationObject;

#define ADD_BLEND_TARGET_NOT_FOUND	-1
#define ADD_BLEND_LOAD_ERROR		0
#define ADD_BLEND_OK				1

#define VISEME_SMILE_SMALL			1
#define VISEME_SMILE				2
#define VISEME_CLOSED_MOUTH			3
#define VISEME_A					4
#define VISEME_O					5
#define VISEME_U					6
#define VISEME_Y					7
#define VISEME_CLOSED_EYES			8
#define VISEME_AU0					9
#define VISEME_AU1					10
#define VISEME_AU2					11
#define VISEME_AU3					12
#define VISEME_AU4					13
#define VISEME_AU5					14

class CMimicController
{
public:
	CMimicController();
	~CMimicController();

	void SetMimicTarget(CSkeletonAnimationObject* const sao);

	int AddBlend(const std::string& src, const std::string& target, const std::string& param);

	void Update();

	void SetLipSyncNextLexemID(const int lexemID, const unsigned int time);
	void DisableLipSync();

	void UpdateSleeping();
	void SetAUCoeffs(const float* au);

	void SetViseme(const unsigned int visemeID, const unsigned int time);

private:
	void UpdateEyeBlinking();
	void UpdateEyeMoving();
	void UpdateLipSync();
	void UpdateSmile();

	void UpdateUserViseme();

	void DisableSmileAndSpeaking();

	int GetTargetID(const std::string& target)const;

	CSkeletonAnimationObject* m_blends[MAX_BLEND_COUNT];
	CSkeletonAnimationObject* m_sao;

	unsigned int m_lastBlinkTime;
	unsigned int m_startSmileTime[2];
	unsigned int m_lastEyeMovingTime;

	float m_smileVectorKoef;

	float m_lipSyncKoef;
	float m_auCoeffs[6];
	int m_AUtimeout;

	unsigned int m_lipSyncNextLexemID;
	unsigned int m_oldLipSyncNextLexemID;
	unsigned int m_lipSyncNextLexemAllTime;
	unsigned int m_lipSyncNextLexemStartTime;

	__int64 m_visemeStartTime;
	__int64 m_visemeTime;
	unsigned int m_visemeID;

	bool m_isUpdated;

	bool m_isSmiling[2];
	bool m_isEyeMoving;
	bool m_isEyeBlinking;

	bool m_isLipSyncEnabled;

	bool m_needToDisableSmileAndSpeaking;
};