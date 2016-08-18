
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

void CSkeletonAnimationObject::PerformEvents()
{
	// in main thread

	std::vector<SEvent *>::iterator it = m_events.begin();
	std::vector<SEvent *>::iterator itEnd = m_events.end();

	for ( ; it != itEnd; it++)
	{
		switch ((*it)->eventID)
		{
		/*case EVENT_ID_ASSIGN_MOTION:
			AssignMotionByEvent((*it)->eventData);
			break;*/

		case EVENT_ID_ADD_IDLE:
			AddIdleByEvent((*it)->eventData);
			break;

		case EVENT_ID_REMOVE_IDLES:
			RemoveIdlesByEvent((*it)->eventData);
			break;

		case EVENT_ID_LOCK_IDLES:
			LockIdlesByEvent((*it)->eventData);
			break;

		case EVENT_ID_UNLOCK_IDLES:
			UnlockIdlesByEvent((*it)->eventData);
			break;

		case EVENT_ID_CLEAR_ACTION:
			ClearActionByEvent((*it)->eventData);
			break;

		case EVENT_ID_CLEAR_CYCLE:
			ClearCycleByEvent((*it)->eventData);
			break;

		case EVENT_ID_BLEND_CYCLE:
			{
				BlendCycleByEvent((*it)->eventData);
				if (!m_isStartApplyIdle)
				{
					m_isStartApplyIdle = true;
					m_startIdlesTime =g->tp.GetTickCount();
				}	
			}
			break;

		case EVENT_ID_EXECUTE_ACTION:
			{
				ExecuteActionByEvent((*it)->eventData);
				if (!m_isStartApplyIdle)
				{
					m_isStartApplyIdle = true;
					m_startIdlesTime =g->tp.GetTickCount();
				}	
			}
			break;

		case EVENT_ID_SET_ACTION:
			{
				SetActionByEvent((*it)->eventData);
				if (!m_isStartApplyIdle)
				{
					m_isStartApplyIdle = true;
					m_startIdlesTime =g->tp.GetTickCount();
				}	
			}
			break;
		}
		MP_DELETE((*it));//delete (*it);
	}

	m_events.clear();
}

bool CSkeletonAnimationObject::IsStartApplyIdle()
{
	return m_isStartApplyIdle;
}

bool CSkeletonAnimationObject::IsEndApplyIdle()
{
	return m_isEndApplyIdle;
}

__int64 CSkeletonAnimationObject::GetStartIdlesTime()
{
	return m_startIdlesTime;
}

void CSkeletonAnimationObject::SaveEvent(const int eventID, void* const eventData)
{
	// in main thread

	SEvent* event = MP_NEW(SEvent);//new SEvent();
	event->eventID = eventID;
	event->eventData = eventData;

	m_events.push_back(event);
}

int CSkeletonAnimationObject::AssignMotionByEvent(void* const eventData)
{
	// in main thread

	int motionID = (int)eventData;

	CMotion* motion = g->motm.GetObjectPointer(motionID);

	if (motion->GetCoreAnimation())
	{
		int cal3dMotionID = m_calCoreModel->attachCoreAnimation(motion->GetCoreAnimation());
		m_calModel->checkCoreAnimations();
		return cal3dMotionID;
	}

	return -1;
}

void CSkeletonAnimationObject::ClearActionByEvent(void* const eventData)
{
	// in main thread

	SClearEvent* clearEvent = (SClearEvent *)eventData;

	if (!m_calModel)
	{
		MP_DELETE(clearEvent);//delete clearEvent;
		return;
	}

	if (0xFFFFFFFF == clearEvent->id)
	{
		MP_DELETE(clearEvent);//delete clearEvent;
		return;
	}

	unsigned int cal3DMotionID = AssignMotion( clearEvent->id);

	//assert(cal3DMotionID < m_motions.size());
	if (cal3DMotionID >= m_motions.size())
	{
		MP_DELETE(clearEvent);//delete clearEvent;
		return;
	}

	if ((m_calModel->getMixer()) && (m_motions[cal3DMotionID] != -1))
	{
		m_calModel->getMixer()->clearAction(cal3DMotionID, clearEvent->time);
	}

	MP_DELETE(clearEvent);//delete clearEvent;
}

void CSkeletonAnimationObject::ClearCycleByEvent(void* const eventData)
{
	// in main thread

	SClearEvent* clearEvent = (SClearEvent *)eventData;

	if (!m_calModel)
	{
		MP_DELETE(clearEvent);//delete clearEvent;
		return;
	}

	if (0xFFFFFFFF == clearEvent->id)
	{
		MP_DELETE(clearEvent);//delete clearEvent;
		return;
	}

	unsigned int cal3DMotionID = AssignMotion( clearEvent->id);

	//assert(cal3DMotionID < m_motions.size());
	if (cal3DMotionID >= m_motions.size())
	{
		MP_DELETE(clearEvent);//delete clearEvent;
		return;
	}

	if ((m_calModel->getMixer()) && (m_motions[cal3DMotionID] != -1))
	{
		m_calModel->getMixer()->clearCycle(cal3DMotionID, clearEvent->time);
	}

	MP_DELETE(clearEvent);//delete clearEvent;
}

void CSkeletonAnimationObject::BlendCycleByEvent(void* const eventData)
{
	// in main thread with adding event

	SBlendCycleEvent* blendCycleEvent = (SBlendCycleEvent*)eventData;

	if (!m_calModel)
	{
		MP_DELETE(blendCycleEvent);//delete blendCycleEvent;
		return;
	}

	if (0xFFFFFFFF == blendCycleEvent->id)
	{
		MP_DELETE(blendCycleEvent);//delete blendCycleEvent;
		return;
	}

	CMotion* motion = g->motm.GetObjectPointer( blendCycleEvent->id);
	assert( motion);
	if( !motion)
	{
		return;
	}

	if (motion->GetCompatabilityID() != m_motionCompatabilityID)
	{
		return;
	}

	unsigned int cal3DMotionID = AssignMotion( blendCycleEvent->id);

	assert(cal3DMotionID < m_motions.size());
	if (cal3DMotionID >= m_motions.size())
	{
		MP_DELETE(blendCycleEvent);//delete blendCycleEvent;
		return;
	}

	if ((m_calModel->getMixer()) && (m_motions[cal3DMotionID] != -1))
	{
		AdjustSkeletonTransformation();
		m_calModel->getMixer()->blendCycle(cal3DMotionID, blendCycleEvent->weight, blendCycleEvent->delay, blendCycleEvent->isAsynch, motion->GetDuration() / 1000.0f);
	}

	MP_DELETE(blendCycleEvent);//delete blendCycleEvent;
}

void CSkeletonAnimationObject::AddIdleByEvent(void* const eventData)
{
	SAddIdleEvent* addIdleEvent = (SAddIdleEvent*)eventData;

	CMotion* motion = g->motm.GetObjectPointer( addIdleEvent->motionID);
	if ((!motion) || (!motion->GetCoreAnimation()))
	{
		MP_DELETE(addIdleEvent);//delete addIdleEvent;
		return;
	}

	if (motion->GetCompatabilityID() != m_motionCompatabilityID)
	{
		return;
	}

	SIdleInfo* idleInfo = MP_NEW(SIdleInfo);//new SIdleInfo();
	idleInfo->motionID = addIdleEvent->motionID;
	idleInfo->motion = g->motm.GetObjectPointer(addIdleEvent->motionID);
	idleInfo->between = addIdleEvent->between;
	idleInfo->freq =addIdleEvent-> freq;
	idleInfo->lastTimePassed = -addIdleEvent->between;

	m_idles.push_back(idleInfo);

	MP_DELETE(addIdleEvent);//delete addIdleEvent;
}

void CSkeletonAnimationObject::RemoveIdlesByEvent(void* const /*eventData*/)
{
	// in main thread

	StopIdles();

	std::vector<SIdleInfo *>::iterator it = m_idles.begin();
	std::vector<SIdleInfo *>::iterator itEnd = m_idles.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE((*it));//delete (*it);
	}

	m_idles.clear();
	m_lockedIdles.clear();
}

void CSkeletonAnimationObject::LockIdlesByEvent(void* const /*eventData*/)
{
	// in main thread

	if (m_idles.size() == 0)
	{
		return;
	}

	m_isIdlesPlaying = false;
	
	m_lockedIdles.clear();

	std::vector<SIdleInfo *>::iterator it = m_idles.begin();
	std::vector<SIdleInfo *>::iterator itEnd = m_idles.end();

	for ( ; it != itEnd; it++)
	{
		m_lockedIdles.push_back(*it);
	}
	
	StopIdles();
	m_idles.clear();
}

void CSkeletonAnimationObject::UnlockIdlesByEvent(void* const /*eventData*/)
{
	// in main thread

	if (m_lockedIdles.size() == 0)
	{
		return;
	}

	m_isIdlesPlaying = true;

	if (m_idles.size() != 0)
	{
		StopIdles();
		RemoveIdlesByEvent(NULL);
	}
	
	std::vector<SIdleInfo *>::iterator it = m_lockedIdles.begin();
	std::vector<SIdleInfo *>::iterator itEnd = m_lockedIdles.end();

	for ( ; it != itEnd; it++)	
	{
		SAddIdleEvent* addIdleEvent = MP_NEW(SAddIdleEvent);//new SAddIdleEvent();
		addIdleEvent->motionID = (*it)->motionID;
		addIdleEvent->between = (*it)->between;
		addIdleEvent->freq = (*it)->freq;
		AddIdleByEvent(addIdleEvent);
		MP_DELETE((*it));//delete (*it);
	}

	m_lockedIdles.clear();
}

void CSkeletonAnimationObject::SetActionByEvent(void* const eventData)
{
	// in main thread

	SSetActionEvent* setActionEvent = (SSetActionEvent*)eventData;

	if (!m_calModel)
	{
		MP_DELETE(setActionEvent);//delete setActionEvent;
		return;
	}

	if (0xFFFFFFFF == setActionEvent->id)
	{
		MP_DELETE(setActionEvent);//delete setActionEvent;
		return;
	}

	CMotion* motion = g->motm.GetObjectPointer(setActionEvent->id);
	if (motion->GetCompatabilityID() != m_motionCompatabilityID)
	{
		return;
	}

	unsigned int cal3DMotionID = AssignMotion( setActionEvent->id);

	assert(cal3DMotionID==0xffffffff || cal3DMotionID < m_motions.size());
	if (cal3DMotionID >= m_motions.size())
	{
		MP_DELETE(setActionEvent);//delete setActionEvent;
		return;
	}

	if ((m_calModel->getMixer()) && (m_motions[cal3DMotionID] != -1))
	{
		StopIdles();
		if (m_lastSetAnimationID != -1)
		{	
			SClearEvent* clearEvent = MP_NEW(SClearEvent);//new SClearEvent();
			clearEvent->id = m_lastSetAnimationID;
			clearEvent->time = 0;
			ClearActionByEvent(clearEvent);
		}
		AdjustSkeletonTransformation();
		m_calModel->getMixer()->setAction(cal3DMotionID, (unsigned int)setActionEvent->time);
		m_lastSetAnimationID = setActionEvent->id;
	}

	MP_DELETE(setActionEvent);//delete setActionEvent;
}

void CSkeletonAnimationObject::ExecuteActionByEvent(void* const eventData)
{
	// in main thread

	SExecuteActionEvent* executeActionEvent = (SExecuteActionEvent*)eventData;

	StopIdles();
	ExecuteActionImpl(executeActionEvent->id, executeActionEvent->delayIn, executeActionEvent->delayOut);

	MP_DELETE(executeActionEvent);//delete executeActionEvent;
}