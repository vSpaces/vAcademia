
#pragma once

#include "IPhysicsPrimitive.h"

class CBasePhysicsPrimitive : public IPhysicsPrimitive
{
public:
	CBasePhysicsPrimitive();
	virtual ~CBasePhysicsPrimitive();

	void SetInitialDimensions(float rx, float ry, float rz, float rw);
	void SetInitialCoords(float x, float y, float z);
	void SetUserData(void* value);

	float GetInitialX()const;
	float GetInitialY()const;
	float GetInitialZ()const;
	void GetInitialDimensions(float& rx, float& ry, float& rz, float& rw)const;
	void GetInitialCoords(float& x, float& y, float& z)const;
	void* GetUserData()const;

private:
	float m_x, m_y, m_z;
	float m_rx, m_ry, m_rz, m_rw;
	void* m_userData;
};