
#include "StdAfx.h"
#include "MimicController.h"
#include "XMLPropertiesList.h"
#include "PlatformDependent.h"
#include "MimicTargetEnumerator.h"
#include "GlobalSingletonStorage.h"
#include "SkeletonAnimationObject.h"

unsigned int SIMPLE_EYE_MOVING_DELTA_TIME = 1;
unsigned int EYE_MOVING_TIME = 1;
unsigned int EYE_CLOSE_DELTA_TIME = 1;
unsigned int EYE_CLOSE_TIME = 1;
unsigned int SMILE_TIME = 1;
unsigned int SMILE_DELAY_TIME = 1;
unsigned int SMILE_STOP_TIME = 1;

CMimicController::CMimicController():
	m_needToDisableSmileAndSpeaking(false),
	m_oldLipSyncNextLexemID(0xFFFFFFFF),
	m_lipSyncNextLexemID(0xFFFFFFFF),		
	m_isLipSyncEnabled(false),
	m_smileVectorKoef(1.0f),	
	m_lastEyeMovingTime(0),
	m_isEyeBlinking(false),	
	m_visemeStartTime(0),
	m_lipSyncKoef(0.0f),
	m_isEyeMoving(false),
	m_lastBlinkTime(0),
	m_isUpdated(false),	
	m_visemeTime(0),
	m_visemeID(0),
	m_sao(NULL),
	m_AUtimeout(0)
{
	for (int k = 0; k < MAX_BLEND_COUNT; k++)
	{
		m_blends[k] = NULL;
	}

	for (int k = 0; k < 2; k++)
	{
		m_startSmileTime[k] = 0;
		m_isSmiling[k] = false;
	}

	static bool isSettingsLoaded = false;

	if (!isSettingsLoaded)
	{
		CXMLPropertiesList mimicSettings(GetApplicationRootDirectory() + L"mimic_settings.xml", ERROR_IF_NOT_FOUND);
		SIMPLE_EYE_MOVING_DELTA_TIME = mimicSettings.GetInteger("simple_eye_moving_delta_time");
		EYE_MOVING_TIME = mimicSettings.GetInteger("eye_moving_time");
		EYE_CLOSE_DELTA_TIME = mimicSettings.GetInteger("eye_close_delta_time");
		EYE_CLOSE_TIME = mimicSettings.GetInteger("eye_close_time");
		SMILE_TIME = mimicSettings.GetInteger("smile_time");
		SMILE_DELAY_TIME = mimicSettings.GetInteger("smile_delay_time");
		SMILE_STOP_TIME = mimicSettings.GetInteger("smile_stop_time");

		isSettingsLoaded = true;
	}
}

// 1 - закрытый рот
// 2 - А
// 3 - О
// 4 - И
// 5 - У
void CMimicController::SetLipSyncNextLexemID(const int lexemID, const unsigned int time)
{
	m_oldLipSyncNextLexemID = m_lipSyncNextLexemID;
	m_lipSyncNextLexemID = lexemID;
	m_lipSyncNextLexemAllTime = time;
	m_lipSyncNextLexemStartTime = (int)g->ne.time;
}

void CMimicController::DisableLipSync()
{
	m_lipSyncNextLexemID = 0xFFFFFFFF;
}

void CMimicController::SetMimicTarget(CSkeletonAnimationObject* const sao)
{
	m_sao = sao;
}
	
int CMimicController::AddBlend(const std::string& src, const std::string& target, const std::string& param)
{
	int targetID = CMimicTargetEnumerator::GetInstance()->GetTargetID(target);

	if (-1 == targetID)
	{
		return ADD_BLEND_TARGET_NOT_FOUND;
	}

	if ((param != "") && (TARGET_SMILE == targetID))
	{
		m_smileVectorKoef = (float)atof((char *)param.c_str());
	}

	
	CSkeletonAnimationObject* sao = g->bm.LoadBlend(src);
	if (sao)
	{
		m_blends[targetID] = sao;
		return ADD_BLEND_OK;
	}
	else
	{
		return ADD_BLEND_LOAD_ERROR;
	}
}

void CMimicController::SetViseme(const unsigned int visemeID, const unsigned int time)
{
	m_visemeID = visemeID;
	m_visemeTime = g->ne.time + time;
	m_visemeStartTime = g->ne.time;
	m_needToDisableSmileAndSpeaking = true;
}

void CMimicController::UpdateUserViseme()
{
	if (m_needToDisableSmileAndSpeaking)
	{
		DisableSmileAndSpeaking();
		m_needToDisableSmileAndSpeaking = false;
	}

	float delay = 500.0f;
	if (m_visemeTime - m_visemeStartTime < 1000)
	{
		delay = (float)(m_visemeTime - m_visemeStartTime) / 3.0f;
	}
	
	float koef = (float)(g->ne.time - m_visemeStartTime) / delay;
	if (koef > 1.0f)
	{
		koef = 1.0f;
	}

	if (m_visemeTime - g->ne.time < delay)
	{
		koef = (float)(m_visemeTime - g->ne.time) / delay;
	}

	switch (m_visemeID)
	{
	case VISEME_SMILE_SMALL:
		if (m_blends[TARGET_SMILE_START])
		{
			m_sao->SetBlendWeight(m_blends[TARGET_SMILE_START], koef);
		}
		else
		{
			m_sao->SetBlendWeight(m_blends[TARGET_SMILE], koef);
		}
		break;

	case VISEME_SMILE:
		if (m_blends[TARGET_SMILE])
		{
			m_sao->SetBlendWeight(m_blends[TARGET_SMILE], koef);
		}
		else
		{
			m_sao->SetBlendWeight(m_blends[TARGET_SMILE_START], koef);
		}
		break;

	case VISEME_CLOSED_MOUTH:
		m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC1], koef);
		break;

	case VISEME_A:
		m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC2], koef);
		break;

	case VISEME_O:
		m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC3], koef);
		break;

	case VISEME_U:
		m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC4], koef);
		break;

	case VISEME_Y:
		m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC5], koef);
		break;

	case VISEME_CLOSED_EYES:
		m_sao->SetBlendWeight(m_blends[TARGET_EYE_CLOSE], koef);
		break;

	case VISEME_AU0:
		m_sao->SetBlendWeight(m_blends[TARGET_AU0], koef);
		break;

	case VISEME_AU1:
		m_sao->SetBlendWeight(m_blends[TARGET_AU1], koef);
		break;

	case VISEME_AU2:
		m_sao->SetBlendWeight(m_blends[TARGET_AU2], koef);
		break;

	case VISEME_AU3:
		m_sao->SetBlendWeight(m_blends[TARGET_AU3], koef);
		break;

	case VISEME_AU4:
		m_sao->SetBlendWeight(m_blends[TARGET_AU4], koef);
		break;

	case VISEME_AU5:
		m_sao->SetBlendWeight(m_blends[TARGET_AU5], koef);
		break;
	}
}

void CMimicController::Update()
{
	if (m_isUpdated)
	{
		return;
	}

	if (!m_sao)
	{
		return;
	}

	if (!m_sao->IsSleeped())
	{
		m_isUpdated = true;
		UpdateEyeMoving();
		UpdateEyeBlinking();

		if (m_AUtimeout == 0)
		{
			for (int i = 0; i < 6; i++) 
			{
				m_sao->SetBlendWeight(m_blends[TARGET_AU0 + i], 0.0f);
			}
			if (m_visemeTime >= g->ne.time)
			{
				UpdateUserViseme();
			}
			else
			{
				UpdateSmile();
				UpdateLipSync();
			}
		}
		else
		{
			m_AUtimeout--;
			for (int i = 0; i < 6; i++) 
			{
				m_sao->SetBlendWeight(m_blends[TARGET_AU0 + i], m_auCoeffs[i]);
			}
		}
		m_isUpdated = false;
	}
	else
	{
		UpdateSleeping();	
	}
}

void CMimicController::SetAUCoeffs(const float * au) 
{
	memcpy(m_auCoeffs, au, sizeof(float) * 6);
	const int maxTimeOut = 10;
	m_AUtimeout = maxTimeOut;
	if(m_auCoeffs[1] > 0.6f) //no other action when mouth widely opened
	{
		m_auCoeffs[0] = 0.0f;
		m_auCoeffs[2] = 0.0f;
		m_auCoeffs[3] = 0.0f;
		m_auCoeffs[4] = 0.0f;
	}
	m_auCoeffs[5] *= 3.0f;//face goes mad if AU3 is multiplied
}

void CMimicController::UpdateLipSync()
{
	if ((m_lipSyncNextLexemID < 1) || (0xFFFFFFFF == m_lipSyncNextLexemID))
	{
		if (m_isLipSyncEnabled)
		{
			for (int i = 0; i < LEXEM_COUNT; i++)
			{
				m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC1 + i], 0.0f);
			}
			m_isLipSyncEnabled = false;
		}

		return;
	}

	if (0 == m_lipSyncNextLexemAllTime)
	{
		if (m_oldLipSyncNextLexemID != m_lipSyncNextLexemID)
		{
			for (unsigned int i = 0; i < LEXEM_COUNT; i++)
			{
				m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC1 + i], (m_lipSyncNextLexemID == i + 1) ? 1.0f : 0.0f);
			}
			m_isLipSyncEnabled = true;
		}
		m_oldLipSyncNextLexemID = m_lipSyncNextLexemID;
	}
	else
	{
		float koef = (float)(g->ne.time - m_lipSyncNextLexemStartTime) / (float)m_lipSyncNextLexemAllTime;
		if (koef > 1.0f)
		{	
			if (koef > 1.5f)
			{
				m_lipSyncNextLexemID = 0;
			}
			koef = 1.0f;			
		}

		if (m_lipSyncNextLexemID == m_oldLipSyncNextLexemID)
		{
			koef = 1.0f;
		}
		
		for (unsigned int i = 0; i < LEXEM_COUNT; i++)
		{
			m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC1 + i], (m_lipSyncNextLexemID == i + 1) ? koef : ((m_oldLipSyncNextLexemID == i + 1) ? 1.0f - koef : 0.0f));
		}
		m_isLipSyncEnabled = true;
	}

	if (m_isLipSyncEnabled)
	{
		if ((m_isSmiling[0]) || (m_isSmiling[1]))
		{
			if (m_blends[TARGET_SMILE_START])
			{
				m_sao->SetBlendWeight(m_blends[TARGET_SMILE_START], 0.0f);
			}
			if (m_blends[TARGET_SMILE])
			{
				m_sao->SetBlendWeight(m_blends[TARGET_SMILE], 0.0f);
			}
		}		 
	}
}

void CMimicController::DisableSmileAndSpeaking()
{
	for (int i = 0; i < LEXEM_COUNT; i++)
	{
		m_sao->SetBlendWeight(m_blends[TARGET_LIP_SYNC1 + i], 0.0f);
	}

	m_sao->SetBlendWeight(m_blends[TARGET_SMILE], 0.0f);
	m_sao->SetBlendWeight(m_blends[TARGET_SMILE_START], 0.0f);
}

void CMimicController::UpdateSleeping()
{
	m_sao->SetBlendWeight(m_blends[TARGET_EYE_CLOSE], 1.0f);
	
	m_sao->SetBlendWeight(m_blends[TARGET_EYE_RIGHT], 0.0f);
	m_sao->SetBlendWeight(m_blends[TARGET_EYE_LEFT], 0.0f);

	DisableSmileAndSpeaking();

	m_lastBlinkTime = (int)g->ne.time - EYE_CLOSE_TIME - 1;
	m_isEyeBlinking = true;
}

void CMimicController::UpdateEyeBlinking()
{
	if ((!m_isEyeBlinking) && (g->ne.time - m_lastBlinkTime > EYE_CLOSE_DELTA_TIME))
	{	
		m_lastBlinkTime = (int)g->ne.time;
		m_isEyeBlinking = true;
	}

	if ((m_isEyeBlinking) && (g->ne.time - m_lastBlinkTime > EYE_CLOSE_TIME))
	{
		m_isEyeBlinking = false;
		m_sao->SetBlendWeight(m_blends[TARGET_EYE_CLOSE], 0.0f);
	}	

	if (m_isEyeBlinking)
	{
		float koef = 2.0f * (float)(g->ne.time - m_lastBlinkTime) / (float)EYE_CLOSE_TIME;

		if (koef > 1.0f)
		{
			koef = 2.0f - koef;
		}
		
		m_sao->SetBlendWeight(m_blends[TARGET_EYE_CLOSE], koef);
	}
}

void CMimicController::UpdateEyeMoving()
{
	if ((!m_isEyeMoving) && (g->ne.time - m_lastEyeMovingTime > SIMPLE_EYE_MOVING_DELTA_TIME))
	{
		m_lastEyeMovingTime = (int)g->ne.time;
		m_isEyeMoving = true;
	}

	if ((m_isEyeMoving) && (g->ne.time - m_lastEyeMovingTime > EYE_MOVING_TIME))
	{
		m_isEyeMoving = false;
	}	

	if (m_isEyeMoving)
	{
		float koef = 4.0f * (float)(g->ne.time - m_lastEyeMovingTime) / (float)EYE_MOVING_TIME;
		if ((koef > 1.0f) && (koef <= 3.0f))
		{
			koef = 2.0f - koef;
		}

		if (koef > 3.0f)
		{
			koef = koef - 4.0f;
		}

		if (koef > 0.0f)
		{
			m_sao->SetBlendWeight(m_blends[TARGET_EYE_LEFT], 0.0f);
			m_sao->SetBlendWeight(m_blends[TARGET_EYE_RIGHT], koef);
		}
		else
		{
			m_sao->SetBlendWeight(m_blends[TARGET_EYE_RIGHT], 0.0f);
			m_sao->SetBlendWeight(m_blends[TARGET_EYE_LEFT], -koef);
		}
	}
}

void CMimicController::UpdateSmile()
{		
	if (g->cm.GetActiveCamera())
	{
		CVector3D delta = g->cm.GetActiveCamera()->GetLookAt() - g->cm.GetActiveCamera()->GetEyePosition();		

		if (m_sao->GetRotatedFaceVector().Dot(delta) < 0.0f)
		{
			if ((!m_isSmiling[0]) && (!m_isSmiling[1]))
			{
				m_isSmiling[0] = true;
				m_isSmiling[1] = true;
				m_startSmileTime[0] = (int)g->ne.time;
					m_startSmileTime[1] = (int)g->ne.time + SMILE_DELAY_TIME + SMILE_TIME + SMILE_STOP_TIME;
			}
		}
		else
		{
			m_isSmiling[0] = false;
			m_isSmiling[1] = false;
		}
	}

	if (m_isLipSyncEnabled)
	{
		m_isSmiling[0] = false;
		m_isSmiling[1] = false;
	}

	for (int k = 0; k < 2; k++)
	{
	int smileTime = SMILE_TIME;

	if (m_blends[TARGET_SMILE_START])			
	if (k == 0)
	{
		smileTime *= 3;
	}

	if ((m_isSmiling[k]) && (g->ne.time - m_startSmileTime[k] > SMILE_DELAY_TIME) && (g->ne.time - m_startSmileTime[k] < SMILE_DELAY_TIME + smileTime + SMILE_STOP_TIME))
	{
		float time = (float)(g->ne.time - m_startSmileTime[k] - SMILE_DELAY_TIME);
		if (time > smileTime * 0.5f)
		{
			if (time > smileTime * 0.5f + SMILE_STOP_TIME)
			{
				time -= SMILE_STOP_TIME;
			}
			else
			{
				time = smileTime * 0.5f;
			}
		}

		float koef = 2.0f * time / (float)smileTime;
		if (koef > 1.0f)
		{
			koef = 2.0f - koef;
		}

		if (0 == k)
		{
			if (m_blends[TARGET_SMILE_START])
			{
				m_sao->SetBlendWeight(m_blends[TARGET_SMILE_START], koef);
			}
			else
			{
				m_sao->SetBlendWeight(m_blends[TARGET_SMILE], koef);
			}
		}
		else
		{
			if (m_blends[TARGET_SMILE_START])
			{
				m_sao->SetBlendWeight(m_blends[TARGET_SMILE], koef);
			}
		}
	}
	}
}

CMimicController::~CMimicController()
{
}