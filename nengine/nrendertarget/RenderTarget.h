
#pragma once

#include "FrameBuffer.h"
#include "PBuffer.h"

#define MODE_DEPTH_16	1
#define MODE_DEPTH_24	2
#define MODE_DEPTH_32	4

#define MODE_STENCIL_1	16
#define MODE_STENCIL_4	32
#define MODE_STENCIL_8	64
#define MODE_STENCIL_16	128

#define RENDER_TARGET_INVALID	0
#define RENDER_TARGET_FBO		1
#define RENDER_TARGET_PBUFFER	2
#define RENDER_TARGET_MAINBUFFER	3

class CRenderTarget
{
public:
	CRenderTarget(int width, int height, int flags);
	~CRenderTarget();

	unsigned char GetType();
	
	int	GetWidth()const;
	int	GetHeight()const;
	bool IsStencilBufferExists()const;
	bool IsDepthBufferExists()const;

	void SetOk(bool isOk);
	bool IsOk() const;
	bool Create(bool isCreateTexturesAutomatically);
	bool Bind();
	bool Unbind();
	
	bool AttachColorTexture(unsigned texId, int no = 0);
	bool AttachDepthTexture(unsigned texId);
	
	bool DetachColorTexture();
	bool DetachDepthTexture(GLenum target);
	
	unsigned int CreateColorTexture(GLenum format = GL_RGB, GLenum internalFormat = GL_RGB8, bool isLinearInterpolation = false);
	unsigned int CreateDepthTexture(GLenum depthFormat, int depthCompareMode);
	
	static int MaxSize();

	CFrameBuffer* GetFrameBuffer()const;

private:
	int m_flags;
	int m_target;
	unsigned int m_pbufferTextureID;
	bool m_isOk;
	bool m_isRTEmulation;

	CFrameBuffer* m_frameBuffer;
	CPBuffer* m_pbuffer;
};
