#pragma once

#include "SceneObject.h"

class CUpdateCallback : public ISceneObject
{
public:
	CUpdateCallback();	
	~CUpdateCallback();

	void SetCallBack(void(*callbackfunc)());
	void Draw();
	void Update();

private:
	void (*m_callback)();
};