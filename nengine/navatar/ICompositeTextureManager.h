
#pragma once

class C3DObject;

class ICompositeTextureManager
{
public:
	virtual void SetMainManager(void* manager) = 0;
	virtual void UpdateTexture(unsigned int textureID) = 0;
	virtual void SetObject3D(C3DObject* obj3d) = 0;
};