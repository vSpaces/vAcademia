
#include "stdafx.h"
#include "texture3d.h"
#include "cal3d/memory_leak.h"

CTexture3D::CTexture3D()
{
}

void CTexture3D::SetTextureNumber(int num)
{
	m_num = num;
}

int CTexture3D::GetTextureNumber()const
{
	return m_num;
}

CTexture3D::~CTexture3D()
{
}