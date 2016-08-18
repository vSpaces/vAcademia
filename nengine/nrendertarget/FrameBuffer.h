
#pragma once

#include <glew.h>

#define DEPTH_COMPARE_NONE		0
#define DEPTH_COMPARE_R_LEQUAL	1
#define DEPTH_COMPARE_R_GEQUAL	2

class	CFrameBuffer
{
public:
	CFrameBuffer(int width, int height, int flags = 0);
	~CFrameBuffer();

	int GetDepthFormatFromFlags(int flags);
	int GetStencilFormatFromFlags(int flags);

	int	GetWidth()const;
	int	GetHeight()const;

	bool IsStencilExists()const;
	bool IsDepthExists()const;
	
	int Create(bool isAutoCreateTextures);
	bool IsOk()const;
	bool IsMultisampleOk()const;

	void PrepareForReading();
	void EndReading();

	bool Bind();
	bool Unbind();

	bool SetCurrentTarget(int target);

	bool AttachColorTexture(unsigned int textureID, int targetID = 0);
	bool AttachDepthTexture( unsigned int textureID);
	bool AttachColorBuffer(unsigned int colorBufferID, bool isMultisample);
	bool AttachDepthBuffer(unsigned int depthBufferID, bool isMultisample);
	bool AttachStencilBuffer(unsigned int stencilBufferID, bool isMultisample);
	bool AttachDepthStencilTexture(unsigned textureID);
	bool AttachDepthStencilBuffer(unsigned int depthStencilBufferID, bool isMultisample);

	bool DetachColorTexture(int targetID = 0);
	bool DetachDepthTexture();
	bool DetachColorBuffer(bool isMultisample);
	bool DetachDepthBuffer(bool isMultisample);
	bool DetachStencilBuffer(bool isMultisample);
	bool DetachDepthStencilTexture();

	bool ReplaceColorTexture(unsigned int textureID, int targetID = 0);
	bool ReplaceDepthTexture(unsigned int textureID);
	bool ReplaceDepthBuffer(unsigned int depthBufferID, bool isMultisample);
	bool ReplaceStencilBuffer(unsigned int stencilBufferID, bool isMultisample);
	bool ReplaceDepthStencilTexture(unsigned int textureID);

	static unsigned int CreateColorTexture(int target, int width, int height, GLenum format, GLenum internalFormat, bool isLinearInterpolation);
	unsigned int CreateColorTexture(GLenum format = GL_RGB, GLenum internalFormat = GL_RGB8, bool isLinearInterpolation = false);
	unsigned int CreateDepthTexture(GLenum depthFormat, int depthCompareMode);
	unsigned int CreateColorBuffer(bool isMultisample);
	unsigned int CreateDepthBuffer(GLenum depthFormat, bool isMultisample);
	unsigned int CreateStencilBuffer(GLenum stencilFormat, bool isMultisample);
	unsigned int CreateDepthStencilBuffer(bool isMultisample);
	unsigned int CreateDepthStencilTexture();
	
	enum								// flags for depth and stencil buffers
	{
		depth16 = 1,					// 16-bit depth buffer
		depth24 = 2,					// 24-bit depth buffer
		depth32 = 4,					// 32-bit depth buffer
		
		stencil1  = 16,					// 1-bit stencil buffer
		stencil4  = 32,					// 4-bit stencil buffer
		stencil8  = 64,					// 8-bit stencil buffer
		stencil16 = 128,				// 16-bit stencil buffer

		depth24_stencil8 = 256			// packed depth-stencil
	};
	
	static int GetMaxColorAttachments();
	static int GetMaxSize();

	static unsigned int CreateDepthTexture(unsigned int target, unsigned int width,
								unsigned int height, GLenum depthFormat, int depthCompareMode);

	void SetMultisamplePower(unsigned char multisamplePower);
	unsigned char GetMultisamplePower()const;

private:
	unsigned int m_flags;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_frameBufferID;
	unsigned int m_depthBufferID;	
	unsigned int m_stencilBufferID;
	unsigned int m_depthStencilTextureID;
	unsigned int m_target;

	unsigned char m_multisamplePower;
	unsigned int m_msFrameBufferID;
	unsigned int m_msDepthBufferID;
	unsigned int m_msColorBufferID;
	unsigned int m_msStencilBufferID;
	unsigned int m_msDepthStencilBufferID;
};
