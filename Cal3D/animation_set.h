//****************************************************************************//
// animation_set.h                                                            //
//****************************************************************************//

#ifndef CAL_ANIMATION_SET_H
#define CAL_ANIMATION_SET_H


#include "global.h"
#include "animation_action.h"

class CalCoreAnimation;
class CalSkeleton;

class CAL3D_API CalAnimationSet : public CalAnimationAction
{
public:
  CalAnimationSet();
  virtual ~CalAnimationSet() { }

  void release();

  //bool	execute(float duration);
  //bool	update(float deltaTime);
  int	getDoneAction();
  void	createSetAnimation( CalSkeleton* skel, float duration);
  void  completeCallbacks(CalModel *model);

private:
  float m_durationSet;
  float m_internalTime;
  float m_lastInternalTime;
};

#endif
