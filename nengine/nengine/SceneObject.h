
#pragma once

#include "CommonObject.h"

class ISceneObject : public CCommonObject
{
public:
	ISceneObject();
	~ISceneObject();

	virtual	void Draw() = 0;

	void SetRenderPriority(int priority);
	int	GetRenderPriority()const;

	void SetRenderMode(int renderMode);
	int GetRenderMode()const;

private:
	int m_priority;
	int m_renderMode;
};