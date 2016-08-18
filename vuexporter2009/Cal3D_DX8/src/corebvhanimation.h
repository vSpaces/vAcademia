#ifndef CAL_CoreBvhAnimation_H
#define CAL_CoreBvhAnimation_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "coreanimation.h"
#include "global.h"
#include "vector.h"

class CAL3D_API CalCoreBvhAnimation : public CalCoreAnimation
{
// constructors/destructor
public:
  CalCoreBvhAnimation();
  virtual ~CalCoreBvhAnimation();

  virtual void initialize(CalSkeleton* skeleton);
  void	setBipUpPosition( float position);

private:
	bool			m_bInitialized;
	CalSkeleton*	p_InitedSkeleton;
	float			m_bvhBipUpPosition;
};

#endif

//****************************************************************************//
