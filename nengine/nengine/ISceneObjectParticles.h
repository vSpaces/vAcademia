
#pragma once

#include "SceneObject.h"

class ISceneObjectParticles : public ISceneObject
{
public:
	virtual float GetDistSq()=0;
	virtual void UpdateDistToCamera()=0;
	virtual void Update(DWORD t)=0;
};