#pragma once

#include "SceneObject.h"

class IClouds : public ISceneObject
{
public:
	virtual ~IClouds() {};

	virtual void Destroy() = 0;

	void SetCamera(float x, float y, float z);
	void GetCamera(float& x, float& y, float& z);
	void SetSize(float size);
	float GetSize()const;

private:
	float m_x, m_y, m_z;
	float m_size;
};