
#include "StdAfx.h"
#include "Clouds.h"
#include "cal3d/memory_leak.h"

void IClouds::SetCamera(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

void IClouds::GetCamera(float& x, float& y, float& z)
{
	x = m_x;
	y = m_y;
	z = m_z;
}

void IClouds::SetSize(float size)
{
	m_size = size;
}

float IClouds::GetSize()const
{
	return m_size;
}