
#include "StdAfx.h"
#include "BasePhysicsPrimitive.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CBasePhysicsPrimitive::CBasePhysicsPrimitive():
	m_userData(NULL),
	m_rx(0.001f), 
	m_ry(0.001f), 
	m_rz(0.001f), 
	m_rw(0.001f),
	m_x(0.0f),
	m_y(0.0f),
	m_z(0.0f)
{
}

void CBasePhysicsPrimitive::SetInitialDimensions(float rx, float ry, float rz, float rw)
{
	if ((rx > 0.0f) && (ry > 0.0f) && (rz > 0.0f) && (rw > 0.0f))
	{
		m_rx = rx;
		m_ry = ry;
		m_rz = rz;
		m_rw = rw;
	}
	else
	{
		m_rx = 0.001f;
		m_ry = 0.001f;
		m_rz = 0.001f;
		m_rw = 0.001f;
	}
}

void CBasePhysicsPrimitive::SetInitialCoords(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

void CBasePhysicsPrimitive::SetUserData(void* value)
{
	m_userData = value;
}

void CBasePhysicsPrimitive::GetInitialDimensions(float& rx, float& ry, float& rz, float& rw)const
{
	rx = m_rx;
	ry = m_ry;
	rz = m_rz;
	rw = m_rw;
}

void CBasePhysicsPrimitive::GetInitialCoords(float& x, float& y, float& z)const
{
	x = m_x;
	y = m_y;
	z = m_z;
}

void* CBasePhysicsPrimitive::GetUserData()const
{
	return m_userData;
}

float CBasePhysicsPrimitive::GetInitialX()const
{
	return m_x;
}

float CBasePhysicsPrimitive::GetInitialY()const
{
	return m_y;
}

float CBasePhysicsPrimitive::GetInitialZ()const
{
	return m_z;
}

CBasePhysicsPrimitive::~CBasePhysicsPrimitive()
{
}