
#pragma once

#include "CommonObject.h"

class CTexture3D : public CCommonObject
{
public:
	CTexture3D();
	~CTexture3D();

	// set GL number for texture
	void SetTextureNumber(int num);
	
	// get GL number for texture
	int GetTextureNumber()const;

private:
	int m_num;
};
