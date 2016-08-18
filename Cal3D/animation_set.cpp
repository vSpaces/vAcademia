//****************************************************************************//
// animation_set.cpp                                                          //
//****************************************************************************//

#include "StdAfx.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "animation_set.h"
#include "error.h"
#include "coreanimation.h"
#include "model.h"
#include "animsequencer.h"
#include "skeleton.h"
#include "coretrack.h"
#include "corekeyframe.h"
#include "animcallback.h"
#include "bone.h"
#include "memory_leak.h"

 /*****************************************************************************/
/** Constructs the animation action instance.
  *
  * This function is the default constructor of the animation action instance.
  *****************************************************************************/

CalAnimationSet::CalAnimationSet()
  : CalAnimationAction()
  , m_internalTime(0)
{
  m_type = TYPE_SET;
}

/*bool CalAnimationSet::execute(float duration)
{
  m_weight = 0.0f;
  m_time = 0.0f;
  m_durationSet = duration;
  m_internalTime = 0.0f;
  m_lastInternalTime = 0.0f;
  m_duration = duration;
  return true;
}

bool CalAnimationSet::update(float deltaTime)
{
	return CalAnimationAction::update( deltaTime);
  if( m_state == STATE_LAST_UPDATE)
    return false;

  m_lastInternalTime = m_internalTime;
  m_internalTime += deltaTime;
  if( m_internalTime < m_durationSet)
  {
      m_weight = (m_internalTime - m_lastInternalTime) / (m_durationSet - m_lastInternalTime);
      return true;
  }
  m_weight = 1.0f;
  m_state = STATE_LAST_UPDATE;
  if (m_pSequencer) m_pSequencer->animExecuted(coreAnimID);
  return true;
}*/

void CalAnimationSet::release()
{
	MP_DELETE_THIS;
}

int CalAnimationSet::getDoneAction()
{
	return AT_SET;
}

void	CalAnimationSet::createSetAnimation( CalSkeleton* skel, float duration)
{
	// 1. сделать анимацию не зависящей от Core
	// 2. удалить из треков все лишние кадры
	// 3. добавить в каждый трек нулевым кадром положение кости из скелета
	if( !bSelfMaster)
		copyCoreAnimation( skel);
	setDuration(duration);
	std::list<CalCoreTrack *>& tracks = getListCoreTrack();
	std::list<CalCoreTrack *>::iterator it = tracks.begin(), end = tracks.end();
	for( ;it!=end;)
	{
		CalCoreTrack* track = (*it);
		CalBone* bone = skel->getBone( track->getCoreBoneId());
		if( !bone)
		{
			MP_DELETE( track);
			it = tracks.erase( it);
			continue;
		}
		else
		{
			it++;
		}
		std::map<float, CalCoreKeyframe *>& frames = track->getMapCoreKeyframe();
		if( frames.size() == 0)
			continue;
		CalCoreKeyframe* firstFrame = frames.begin()->second;
		firstFrame->setTime( getCoreAnimation()->getDuration());
		frames.erase( frames.begin());
		while( frames.size()!=0)
		{
			MP_DELETE( (frames.begin())->second);
			frames.erase( frames.begin());
		}
		if( duration != 0) 
		{
			CalCoreKeyframe* zeroFrame = MP_NEW( CalCoreKeyframe);

			CalVector frameTranslation = bone->getTranslation();
			CalQuaternion frameRotation = bone->getRotation();

			if( track->getCoreBoneId() == Cal::PROJECTION_BONE_ID)
			{
				applyInverseProjectionBoneTransformation( frameTranslation, frameRotation);
			}
			zeroFrame->setTranslation( frameTranslation);
			zeroFrame->setRotation( frameRotation);
			zeroFrame->setTime(0);
			track->addCoreKeyframe( zeroFrame);
		}
		track->addCoreKeyframe( firstFrame);
	}
}

void CalAnimationSet::completeCallbacks(CalModel *model)
{
	std::vector<CalCoreAnimation::CallbackRecord>& list = getCoreAnimation()->getCallbackList();
	for (size_t i=0; i<list.size(); i++)
		list[i].callback->AnimationSet(model, model->getUserData());
}


//****************************************************************************//
