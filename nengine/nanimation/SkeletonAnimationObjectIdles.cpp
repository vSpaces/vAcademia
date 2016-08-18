
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "SkeletonAnimationObject.h"
#include "SkeletonAnimationObjectEvents.h"

#include "cal3d\animation_action.h"
#include "cal3d\animation_cycle.h"
#include "cal3d\CoreMaterial.h"
#include "cal3d\CoreSubmesh.h"
#include "cal3d\Submesh.h"
#include "cal3d\Skeleton.h"
#include "cal3d\Renderer.h"
#include "cal3d\Mixer.h"
#include "cal3d\Mesh.h"
#include "cal3d\Bone.h"
#include "cal3d\coreanimationflags.h"

#define IDLE_PAUSE_TIME		2000

bool CSkeletonAnimationObject::IsSleeped()const
{
	return m_isSleeped;
}

void CSkeletonAnimationObject::SetSleepingState(const bool isSleeped)
{
	m_isSleeped = isSleeped;
}

void CSkeletonAnimationObject::AddIdle(const int motionID, const int between, const int freq)
{
	// in main thread

	SAddIdleEvent* addIdleEvent = MP_NEW(SAddIdleEvent);//new SAddIdleEvent();
	addIdleEvent->motionID = motionID;	
	addIdleEvent->between = between;
	addIdleEvent->freq = freq;
	SaveEvent(EVENT_ID_ADD_IDLE, addIdleEvent);
}

void CSkeletonAnimationObject::RemoveIdles()
{
	// in main thread

	SaveEvent(EVENT_ID_REMOVE_IDLES, NULL);
}

void CSkeletonAnimationObject::LockIdles()
{
	// in main thread

	SaveEvent(EVENT_ID_LOCK_IDLES, NULL);
}

void CSkeletonAnimationObject::UnlockIdles()
{
	// in main thread

	SaveEvent(EVENT_ID_UNLOCK_IDLES, NULL);
}

void CSkeletonAnimationObject::StopIdles()
{
	// in main thread
	// private method

	m_isIdlesPlaying = false;

	// Remove all idles animation
	std::vector<SIdleInfo *>::iterator it = m_idles.begin();
	std::vector<SIdleInfo *>::iterator itEnd = m_idles.end();

	for ( ; it != itEnd; it++)
	{
		//if ((*it)->isPlaying)
		{
			SClearEvent* clearEvent = MP_NEW(SClearEvent);//new SClearEvent();
			clearEvent->id = (*it)->motionID;
			clearEvent->time = 0.0f;
			ClearActionByEvent(clearEvent);
			(*it)->isPlaying = false;
		}
	}
}

bool CSkeletonAnimationObject::CanIdleStart(const int _motionID)
{
	// asynch execution

	//!! Здесь AssignMotion нужно оставить
	int motionID = AssignMotion(_motionID);
	CalMixer* mixer = m_calModel->getMixer();
	if (!mixer)	
	{
		return false;
	}

	std::list<CalAnimationAction *>::iterator iteratorAnimationAction = mixer->m_listAnimationAction.begin();
	while (iteratorAnimationAction != mixer->m_listAnimationAction.end())
	{
		// Зкшьфкн анимации можно смешивать с идлами
		if( ((*iteratorAnimationAction)->getFlags() & CAL3D_COREANIM_PRIMARY) == 0)
		{
			if (mixer->containSameBones((*iteratorAnimationAction)->coreAnimID, motionID))
				return false;
		}

		iteratorAnimationAction++;
	}

	std::list<CalAnimationCycle *>::iterator iteratorAnimationCycle = mixer->m_listAnimationCycle.begin();
	while (iteratorAnimationCycle != mixer->m_listAnimationCycle.end())
	{
		if (mixer->containSameBones((*iteratorAnimationCycle)->coreAnimID, motionID))
			return false;

		iteratorAnimationCycle++;
	}

	return true;	
}

void CSkeletonAnimationObject::PlayNextIdleMotion()
{
	// asynch execution
	if( !m_calModel || !m_calModel->getMixer())
	{
		return;
	}

	if (m_idles.empty())
	{
		return;
	}

	if (m_obj3d && m_obj3d->IsFrozen())
	{
		return;
	}	

	if (m_kinectController)
	if (m_kinectController->IsEnabled())
	{
		return;
	}

	bool isPlaying = false;

	int idlePauseTime = IDLE_PAUSE_TIME;
	if (m_idles.size() == 1)
	{
		idlePauseTime = 0;
	}

	unsigned int tick = (unsigned int)g->ne.time;
	unsigned int freqSum = 0;
	std::vector<SIdleInfo *>::iterator it = m_idles.begin();
	std::vector<SIdleInfo *>::iterator itEnd = m_idles.end();
	for ( ; it != itEnd; it++)
	{
		if ((!(*it)->isPlaying) && (tick - (*it)->lastTimePassed >= (unsigned int)(*it)->between))
		{
			freqSum += (*it)->freq;
		}
		else	if ((*it)->isPlaying)
		{
			// #2979 При перемотке записи назад записанный аватар сел мимо стула
			// В режиме паузы флаг окончания анимации сбрасывался раньше, чем она реально заканчивалась. 
			// Поэтому после остановки текущих идлов она не останавливалась. Сейчас стоит более медленная, но 
			// честная проверка
			CalAnimation*	pAnimation = NULL;
			unsigned int cal3DMotionID = AssignMotion( (*it)->motionID);
			m_calModel->getMixer()->findAnimationById( cal3DMotionID, &pAnimation);
			if( !pAnimation)
			{
				(*it)->isPlaying = false;
				freqSum += (*it)->freq;
			}
			else
			{
				isPlaying = true;
			}
		}
	}

	if (isPlaying)
	{
		return;
	}

	// не оптимайзить, иначе rand - выдает не нормальное распределение
	int	rndInt = rand();
	float	rnd = rndInt / (float)RAND_MAX;	rnd *= freqSum;
	// end of 'не оптимайзить'
	freqSum = 0;
	it = m_idles.begin();
	for ( ; it != itEnd; it++)
	{
		if ((((*it)->freq == -1) || (!(*it)->isPlaying && (tick - (*it)->lastTimePassed >= (unsigned int)(*it)->between))) &&
			((*it)->freq != -2))
		{
			freqSum += (*it)->freq;
			if ((rnd <= freqSum) || ((*it)->freq == -1))	// this is out idle
			{
				if (CanIdleStart((*it)->motionID))
				{
					if ((*it)->freq == -1)
					{
						(*it)->freq = -2;
					}

					(*it)->isPlaying = true;
					(*it)->startTime = g->ne.time;
					(*it)->lastTimePassed = tick;
					ExecuteActionImpl((*it)->motionID, 0, 0);

					m_isIdlesPlaying = true;
				}
				return;
			}
		}
	}
}