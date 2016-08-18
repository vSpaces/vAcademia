
#pragma once

#include "CommonRenderTargetHeader.h"

#include "IDrawingTarget.h"
#include "FrameBuffer.h"
#include "RenderTarget.h"
#include "Filter.h"

class CSimpleDrawingTarget : public IDrawingTarget
{
public:
	CSimpleDrawingTarget();
	~CSimpleDrawingTarget();

	void SaveTexture(std::wstring fileName);

	bool IsCreatedNow();

	void SetClearColor(unsigned char r = 255, unsigned char g = 255,unsigned char b = 255, unsigned char a = 255);
	void SetFormat(bool isTransparent, unsigned int flags);
	void SetWidth(unsigned int width);
	void SetHeight(unsigned int height);

	void SetInvScale(unsigned char invScale);
	unsigned char GetInvScale()const;

	unsigned int CreateTempTexture();
	unsigned int CreateEmptySmallTexture();
	void SaveBackBuffer(int textureID);
	void RestoreBackBuffer(int textureID);

	unsigned int GetWidth();
	unsigned int GetHeight();

	unsigned int GetScaledWidth();
	unsigned int GetScaledHeight();

	void AttachTexture(CTexture* texture);

	bool Create();
	void Clear();

	void ApplyGeometry(void (__cdecl *callbackfunc)(void));
	void ApplyFilter(CFilter* filter);

	void BindToApplyGeometry();
	void UnbindToApplyGeometry();

	bool IsBindedToApplyGeometry()const;

	float GetGLX(int x);
	float GetGLY(int y);

	void Destroy();

	bool IsOk();

	void SetName(char* name);
	void FreeResourcesIfNeeded();

	unsigned char GetType();

private:
	static unsigned char* GetClearedMemory(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	static unsigned char* GetMemory();

	unsigned char m_clearColorR, m_clearColorG, m_clearColorB, m_clearColorA;
	unsigned int m_backBufferTextureID;
	unsigned int m_width, m_height;
	unsigned int m_flags;
	unsigned char m_invScale;

	bool m_isTransparent;
	bool m_isCreated;
	bool m_isCleared;
	bool m_isBinded;
	bool m_isOk;

	MP_STRING m_name;
	
	CTexture* m_texture;
	CRenderTarget* m_fbo;
};