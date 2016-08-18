
#pragma once

#include "CommonHeader.h"
#include "AnimationObject.h"

class CMorphAnimationObject : public CAnimationObject
{
public:
	CMorphAnimationObject();
	~CMorphAnimationObject();

	int GetType()const ;
	bool IsUpdated();

	void Update(const int deltaTime, const float distSq);
	bool Load(const std::string& path);

	void OnPosChanged();

	int GetUpdateTime()const ;

	CVector3D GetOffset()const ;
	void GetOffset(float* const x, float* const y, float* const z);

	void SetFreeze(/*bool isFrozen*/)	{};
};