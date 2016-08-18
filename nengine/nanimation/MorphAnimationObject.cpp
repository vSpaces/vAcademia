
#include "StdAfx.h"
#include "MorphAnimationObject.h"

CMorphAnimationObject::CMorphAnimationObject()
{
}

int CMorphAnimationObject::GetUpdateTime()const
{
	return 0;
}

int CMorphAnimationObject::GetType()const
{
	return TYPE_MORPHING;
}

void CMorphAnimationObject::OnPosChanged()
{
}

void CMorphAnimationObject::Update(const int /*deltaTime*/, const float /*distSq*/)
{
}

bool CMorphAnimationObject::Load(const std::string& /*path*/)
{
	return true;
}

bool CMorphAnimationObject::IsUpdated()
{
	return true;
}

CVector3D CMorphAnimationObject::GetOffset()const
{
	return CVector3D(0, 0, 0);
}

void CMorphAnimationObject::GetOffset(float* const x, float* const y, float* const z)
{
	*x = 0.0f;
	*y = 0.0f;
	*z = 0.0f;
}

CMorphAnimationObject::~CMorphAnimationObject()
{
}
