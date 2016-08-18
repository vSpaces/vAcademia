
#include "StdAfx.h"
#include "VisemeController.h"
#include "model.h"
#include "corebone.h"
#include "coremodel.h"
#include "coretrack.h"
#include "bone.h"
#include "coreskeleton.h"
#include "animation.h"
#include "skeleton.h"
#include "memory_leak.h"

CalVisemeController::CalVisemeController(CalModel* pModel):
	m_motionID(-1),
	m_model(pModel),
	m_isEnabled(false),
	m_currentViseme1ID(-1),
	m_currentViseme2ID(-2),
	m_currentVisemeKoef(0.0f),
	MP_VECTOR_INIT(m_bones),
	MP_VECTOR_INIT(m_boneIDs)
{
}

void CalVisemeController::SetState(int viseme1ID, int viseme2ID, float koef)
{
	m_currentViseme1ID = viseme1ID;
	m_currentViseme2ID = viseme2ID;
	m_currentVisemeKoef = koef;
}

void CalVisemeController::Update()
{
	if (!m_isEnabled)	
	{
		return;
	}

	if (m_motionID != -1)
	{
		CalCoreAnimation* anim = m_model->getCoreModel()->getCoreAnimation(m_motionID);
		if (anim == NULL)
		{
			return;
		}
		std::list<CalCoreTrack *>& tracks = anim->getListCoreTrack();
		if ( tracks.empty() )
		{
			return;
		}
		std::list<CalCoreTrack *>::iterator	it = tracks.begin(),
										end = tracks.end();
		for ( ; it != end; it++)
		{
			std::vector<CalBone *>::iterator boneIt = m_bones.begin();

			std::vector<int>::iterator idIt = m_boneIDs.begin();
			std::vector<int>::iterator idItEnd = m_boneIDs.end();

			for ( ; idIt != idItEnd; idIt++, boneIt++)
			if ((*it)->getCoreBoneId() == *idIt)
			{
				int keyFrameCount = (*it)->getMapCoreKeyframe().size();

				CalVector t1, t2;
				CalQuaternion r1, r2;
				(*it)->getState((float)m_currentViseme1ID, (float)keyFrameCount, t1, r1);
				(*it)->getState((float)m_currentViseme2ID, (float)keyFrameCount, t2, r2);

				r1.blend(m_currentVisemeKoef, r2);
				t1.blend(m_currentVisemeKoef, t2);

				CalBone* bone = m_model->getSkeleton()->getBone(*idIt);
				if (bone)
				{
					bone->setTranslation(t1);
					bone->setRotation(r1);
					bone->calculateState();
				}
			}
		}
	}

	return;
}

bool CalVisemeController::Init(int motionID)
{
	if (!m_model->getSkeleton())
	{
		return false;
	}

	int headBoneID = -1;

	if (m_model->getSkeleton()->getCoreSkeleton()->containBone("Head"))
	{
		headBoneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("Head");
	}

	if (m_model->getSkeleton()->getCoreSkeleton()->containBone("head"))
	{
		headBoneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("head");
	}

	if (-1 == headBoneID)
	{
		return false;
	}

	CalBone* headBone = m_model->getSkeleton()->getBone(headBoneID);
	if (!headBone)
	{
		return false;
	}

	m_bones.clear();
	m_boneIDs.clear();

	std::list<int>& childList = headBone->getCoreBone()->getListChildId();

	std::list<int>::iterator it = childList.begin();
	std::list<int>::iterator itEnd = childList.end();

	for ( ; it != itEnd; it++)
	{
		CalBone* bone = m_model->getSkeleton()->getBone(*it);

		if (bone)
		{
			m_bones.push_back(bone);
			m_boneIDs.push_back(*it);
		}
	}

	Enable(true);

	m_motionID = motionID;

	return true;
}

void CalVisemeController::Enable(bool isEnabled)
{
	m_isEnabled = isEnabled;
}

bool CalVisemeController::IsEnabled()const
{
	return m_isEnabled;
}

CalVisemeController::~CalVisemeController()
{
}