
#pragma once

#include "SceneObject.h"

class CRenderingCallback : public ISceneObject
{
public:
	CRenderingCallback();	
	~CRenderingCallback();

	void SetCallBack(void(*callbackfunc)());
	void Draw();

private:
	void (*m_callback)();
};