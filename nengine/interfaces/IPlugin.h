
#pragma once

struct IPlugin
{
	virtual void BeforeUpdate() = 0;
	virtual void AfterUpdate() = 0;
	virtual void BeforeRender() = 0;
	virtual void AfterRender() = 0;
	virtual void EndRendering() = 0;
	virtual void EndRendering3D() = 0;
	virtual void EndRendering2D() = 0;
	virtual void SetObject(void* object) = 0;
};