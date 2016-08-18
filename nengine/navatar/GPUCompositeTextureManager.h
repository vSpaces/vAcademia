
#pragma once

#include "ICompositeTextureManager.h"

class CGPUCompositeTextureManager : public ICompositeTextureManager
{
public:
	CGPUCompositeTextureManager();
	~CGPUCompositeTextureManager();

	void UpdateTexture(unsigned int textureID);
	void SetMainManager(void* manager);
	void SetObject3D(C3DObject* obj3d);

private:
	void* m_manager;
	C3DObject* m_obj3d;
};