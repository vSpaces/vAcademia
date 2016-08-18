
#pragma once

#include "Texture.h"

class IDrawingTarget
{
public:
	virtual void SetInvScale(unsigned char invScale) = 0;
	virtual void SetClearColor(unsigned char r = 255, unsigned char g = 255,unsigned char b = 255, unsigned char a = 255) = 0;
	virtual void SetFormat(bool isTransparent, unsigned int flags) = 0;
	virtual void SetWidth(unsigned int width)= 0;
	virtual void SetHeight(unsigned int height) = 0;
	virtual unsigned char GetInvScale()const = 0;

	virtual unsigned int GetWidth() = 0;
	virtual unsigned int GetHeight() = 0;

	virtual unsigned int GetScaledWidth() = 0;
	virtual unsigned int GetScaledHeight() = 0;

	virtual unsigned int CreateTempTexture() = 0;
	virtual void SaveBackBuffer(int textureID = -1) = 0;
	virtual void RestoreBackBuffer(int textureID = -1) = 0;

	virtual void AttachTexture(CTexture* texture) = 0;
	virtual void SaveTexture(std::wstring fileName) = 0;

	virtual bool Create() = 0;
	virtual bool IsOk() = 0;
	virtual void Clear() = 0;

	virtual void ApplyGeometry(void (__cdecl *callbackfunc)(void)) = 0;

	virtual float GetGLX(int x) = 0;
	virtual float GetGLY(int y) = 0;

	virtual void BindToApplyGeometry() = 0;
	virtual void UnbindToApplyGeometry() = 0;

	virtual bool IsBindedToApplyGeometry()const = 0;

	virtual void Destroy() = 0;

	virtual unsigned char GetType() = 0;

	virtual bool IsCreatedNow() = 0;
};