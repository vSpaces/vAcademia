
#include "StdAfx.h"
#include "Assert.h"
#include <glew.h>
#include "FrameBuffer.h"
#include "GlobalSingletonStorage.h"

void DeleteRenderbuffer(unsigned int bufferID)
{
	/*if (!glIsRenderbufferEXT(bufferID))
	{
		int ii = 0;
	}*/
	GLFUNC(glDeleteRenderbuffersEXT)(1, &bufferID);
	/*int glErr = glGetError();
	if (glErr != 0)
	{
		int ii = 0;
	}*/
}

CFrameBuffer::CFrameBuffer(int width, int height, int flags):
	m_width(width),
	m_height(height),
	m_flags(flags),
	m_multisamplePower(1),
	m_target(GL_TEXTURE_2D),
	m_frameBufferID(0xFFFFFFFF),
	m_depthBufferID(0xFFFFFFFF),
	m_msColorBufferID(0xFFFFFFFF),
	m_msFrameBufferID(0xFFFFFFFF),
	m_msDepthBufferID(0xFFFFFFFF),
	m_stencilBufferID(0xFFFFFFFF),
	m_msStencilBufferID(0xFFFFFFFF),
	m_depthStencilTextureID(0xFFFFFFFF),
	m_msDepthStencilBufferID(0xFFFFFFFF)
{
}

int CFrameBuffer::GetDepthFormatFromFlags(int flags)
{
	int	depthFormat = 0;

	if (flags & depth16)
	{
		depthFormat = GL_DEPTH_COMPONENT16_ARB;
	}
	else if (flags & depth24)
	{
		depthFormat = GL_DEPTH_COMPONENT24_ARB;
	}
	else if (flags & depth32)
	{
		depthFormat = GL_DEPTH_COMPONENT32_ARB;
	}

	return depthFormat;
}

int CFrameBuffer::GetStencilFormatFromFlags(int flags)
{
	int stencilFormat = 0;

	if (flags & stencil1)
	{
		stencilFormat = GL_STENCIL_INDEX1_EXT;
	}
	else if (flags & stencil4)
	{
		stencilFormat = GL_STENCIL_INDEX4_EXT;
	}
	else if (flags & stencil8)
	{
		stencilFormat = GL_STENCIL_INDEX8_EXT;
	}
	else if (flags & stencil16)
	{
		stencilFormat = GL_STENCIL_INDEX16_EXT;
	}

	return stencilFormat;
}

void CFrameBuffer::SetMultisamplePower(unsigned char multisamplePower)
{
	m_multisamplePower = multisamplePower;	
}

unsigned char CFrameBuffer::GetMultisamplePower()const
{
	return m_multisamplePower;
}

int	CFrameBuffer::GetWidth()const
{
	return m_width;
}
	
int	CFrameBuffer::GetHeight()const
{
	return m_height;
}
	
bool CFrameBuffer::IsStencilExists()const
{
	return (m_stencilBufferID != 0xFFFFFFFF);
}
	
bool CFrameBuffer::IsDepthExists()const
{
	return (m_depthBufferID != 0xFFFFFFFF);
}

void CFrameBuffer::PrepareForReading()
{
	if (m_multisamplePower > 1)
	if (g->gi.GetVendorID() == VENDOR_ATI)
	{		
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);		
	}
	else
	{		
		GLFUNC(glBindFramebufferEXT)(GL_READ_FRAMEBUFFER_EXT, m_frameBufferID);
		GLFUNC(glBindFramebufferEXT)(GL_DRAW_FRAMEBUFFER_EXT, m_msFrameBufferID);
	}
}

void CFrameBuffer::EndReading()
{
	if (m_multisamplePower > 1)
	if (g->gi.GetVendorID() == VENDOR_ATI)
	{
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_msFrameBufferID);
	}
	else
	{		
		GLFUNC(glBindFramebufferEXT)(GL_READ_FRAMEBUFFER_EXT, 0);		
	}
}

int CFrameBuffer::Create(bool isAutoCreateTextures)
{
	assert(m_width > 0);
	assert(m_height > 0);

	if ((m_width <= 0) || (m_height <= 0))
	{
		return false;
	}

	GLFUNC(glGenFramebuffersEXT)(1, &m_frameBufferID);
	GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);

	if (m_multisamplePower > 1)
	{
		GLFUNC(glGenFramebuffersEXT)(1, &m_msFrameBufferID);	
	}

	try
	{
		if (isAutoCreateTextures)
		{
			int	depthFormat = GetDepthFormatFromFlags(m_flags);
			int stencilFormat = GetStencilFormatFromFlags(m_flags);

			if (m_flags & depth24_stencil8)
			{
				m_depthStencilTextureID = CreateDepthStencilTexture();
				AttachDepthStencilTexture(m_depthStencilTextureID);
			}
			else
			{
				if (depthFormat != 0)
				{
					m_depthBufferID = CreateDepthBuffer(depthFormat, false);
					AttachDepthBuffer(m_depthBufferID, false);
				}

				if (stencilFormat != 0)
				{
					m_stencilBufferID = CreateStencilBuffer(stencilFormat, false);
					AttachStencilBuffer(m_stencilBufferID, false);
				}
			}

			if (m_multisamplePower > 1)
			{
				if (m_flags & depth24_stencil8)
				{
					m_msDepthStencilBufferID = CreateDepthStencilBuffer(true);
					if (m_msDepthStencilBufferID == 0xFFFFFFFF)
					{
						m_multisamplePower = 1;
					}
					else
					{
						AttachDepthStencilBuffer(m_msDepthStencilBufferID, true);
					}
				}
				else
				{
					if (depthFormat != 0)
					{
						m_msDepthBufferID = CreateDepthBuffer(depthFormat, true);
						if (m_msDepthBufferID == 0xFFFFFFFF)
						{
							m_multisamplePower = 1;

							if (m_msFrameBufferID != 0xFFFFFFFF)
							{
								GLFUNC(glDeleteFramebuffersEXT)(1, &m_msFrameBufferID);
								m_msFrameBufferID = 0xFFFFFFFF;
							}
						}
						else
						{
							AttachDepthBuffer(m_msDepthBufferID, true);							
						}
					}

					if (stencilFormat != 0)
					{
						m_msStencilBufferID = CreateStencilBuffer(stencilFormat, true);						
						if (m_msStencilBufferID == 0xFFFFFFFF)
						{
							m_multisamplePower = 1;

							if (m_msDepthBufferID != 0xFFFFFFFF)
							{
								DeleteRenderbuffer(m_msDepthBufferID);
								m_msDepthBufferID = 0xFFFFFFFF;
							}

							if (m_msFrameBufferID != 0xFFFFFFFF)
							{
								GLFUNC(glDeleteFramebuffersEXT)(1, &m_msFrameBufferID);
								m_msFrameBufferID = 0xFFFFFFFF;
							}
						}
						else
						{
							AttachStencilBuffer(m_msStencilBufferID, true);
						}
					}
				}
			}

			if (m_multisamplePower > 1)
			{
				m_msColorBufferID = CreateColorBuffer(true);
				if (m_msColorBufferID == 0xFFFFFFFF)
				{
					m_multisamplePower = 1;
			
					if (m_msDepthBufferID != 0xFFFFFFFF)
					{
						DeleteRenderbuffer(m_msDepthBufferID);
						m_msDepthBufferID = 0xFFFFFFFF;
					}

					if (m_msStencilBufferID != 0xFFFFFFFF)
					{
						DeleteRenderbuffer(m_msStencilBufferID);
						m_msStencilBufferID = 0xFFFFFFFF;
					}

					if (m_msDepthStencilBufferID != 0xFFFFFFFF)
					{
						DeleteRenderbuffer(m_msDepthStencilBufferID);
						m_msDepthStencilBufferID = 0xFFFFFFFF;
					}

					if (m_msFrameBufferID != 0xFFFFFFFF)
					{
						GLFUNC(glDeleteFramebuffersEXT)(1, &m_msFrameBufferID);
						m_msFrameBufferID = 0xFFFFFFFF;
					}					
				}
				else
				{
					AttachColorBuffer(m_msColorBufferID, true);
				}
			}
		}
	}
	catch (...)
	{
		return -1;
	}

	GLenum status = GLFUNCR(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));
	
	GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, 0);
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);

	return (status == GL_FRAMEBUFFER_COMPLETE_EXT) ? 1 : 0;
}

bool CFrameBuffer::IsOk()const
{
	assert(m_frameBufferID != 0xFFFFFFFF);
	if (m_frameBufferID == 0xFFFFFFFF)
	{
		return false;
	}

	unsigned int currentFrameBufferID;
	GLFUNC(glGetIntegerv)(GL_FRAMEBUFFER_BINDING_EXT,(int *)&currentFrameBufferID);

	if (currentFrameBufferID != m_frameBufferID)
	{
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);
		GLFUNC(glReadBuffer)(GL_COLOR_ATTACHMENT0_EXT);
	}

	GLenum status = GLFUNCR(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));

	if(currentFrameBufferID != m_frameBufferID)
	{
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, currentFrameBufferID);
	}

	return (status == GL_FRAMEBUFFER_COMPLETE_EXT);
}

bool CFrameBuffer::IsMultisampleOk()const
{
	assert(m_msFrameBufferID != 0xFFFFFFFF);
	if (m_msFrameBufferID == 0xFFFFFFFF)
	{
		return false;
	}

	unsigned int currentFrameBufferID;
	GLFUNC(glGetIntegerv)(GL_FRAMEBUFFER_BINDING_EXT,(int *)&currentFrameBufferID);

	if (currentFrameBufferID != m_msFrameBufferID)
	{
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_msFrameBufferID);
		GLFUNC(glReadBuffer)(GL_COLOR_ATTACHMENT0_EXT);		
	}

	GLenum status = GLFUNCR(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));

	if (currentFrameBufferID != m_msFrameBufferID)
	{
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, currentFrameBufferID);
	}

	return (status == GL_FRAMEBUFFER_COMPLETE_EXT);
}

bool CFrameBuffer::Bind()
{
	assert(m_frameBufferID != 0xFFFFFFFF);
	if (m_frameBufferID == 0xFFFFFFFF)
	{
		return false;
	}

	if (m_multisamplePower > 1)
	{
		assert(m_msFrameBufferID != 0xFFFFFFFF);
		if (m_msFrameBufferID == 0xFFFFFFFF)
		{
			return false;
		}

		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_msFrameBufferID);		
	}
	else
	{
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);		
	}

	return true;
}

bool CFrameBuffer::Unbind()
{
	assert(m_frameBufferID != 0xFFFFFFFF);
	if (m_frameBufferID == 0xFFFFFFFF)
	{
		return false;
	}

	if (m_multisamplePower > 1)
	{
		assert(m_msFrameBufferID != 0xFFFFFFFF);
		if (m_msFrameBufferID == 0xFFFFFFFF)
		{
			return false;
		}

		GLFUNC(glBindFramebufferEXT)(GL_READ_FRAMEBUFFER_EXT, m_msFrameBufferID);
		GLFUNC(glBindFramebufferEXT)(GL_DRAW_FRAMEBUFFER_EXT, m_frameBufferID);
		GLFUNC(glBlitFramebufferEXT)(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	}

	GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, 0);

	return true;
}

bool CFrameBuffer::SetCurrentTarget(int target)
{
	if ((target != GL_TEXTURE_2D) && (target != GL_TEXTURE_RECTANGLE_ARB) && ((target < GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB) || (target > GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB)))
	{
		return false;
	}

	m_target = target;
	return true;
}

bool CFrameBuffer::ReplaceColorTexture(unsigned int textureID, int targetID)
{
	if (AttachColorTexture(textureID, targetID))
	{
		return false;
	}

	return true;
}

bool CFrameBuffer::ReplaceDepthTexture(unsigned int textureID)
{
	if (!DetachDepthTexture())
	{
		return false;
	}

	if (!AttachDepthTexture(textureID))
	{
		return false;
	}

	return true;
}

bool CFrameBuffer::ReplaceDepthBuffer(unsigned int depthBufferID, bool isMultisample)
{
	if (!DetachDepthBuffer(isMultisample))
	{
		return false;
	}

	if (!AttachDepthBuffer(depthBufferID, isMultisample))
	{
		return false;
	}

	return true;
}

bool CFrameBuffer::ReplaceStencilBuffer(unsigned int stencilBufferID, bool isMultisample)
{
	if (!DetachStencilBuffer(isMultisample))
	{
		return false;
	}

	if (!AttachStencilBuffer(stencilBufferID, isMultisample))
	{
		return false;
	}

	return true;
}

bool CFrameBuffer::ReplaceDepthStencilTexture(unsigned int textureID)
{
	if (!DetachDepthStencilTexture())
	{
		return false;
	}

	if (!AttachDepthStencilTexture(textureID))
	{
		return false;
	}

	return true;
}

bool CFrameBuffer::AttachColorTexture(unsigned int textureID, int targetID)
{
	assert(m_frameBufferID != 0xFFFFFFFF);
	if(m_frameBufferID == 0xFFFFFFFF)
	{
		return false;
	}

	if (GL_TEXTURE_RECTANGLE_ARB == m_target)
	{
		GLFUNC(glEnable)(m_target);
	}

	unsigned int currentFrameBufferID = 0;
	if (m_multisamplePower > 1)
	{
		GLFUNC(glGetIntegerv)(GL_FRAMEBUFFER_BINDING_EXT,(int *)&currentFrameBufferID);

		if (currentFrameBufferID != m_frameBufferID)
		{
			GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);
			GLFUNC(glReadBuffer)(GL_COLOR_ATTACHMENT0_EXT);
		}
	}
	
	GLFUNC(glFramebufferTexture2DEXT)(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + targetID, m_target, textureID, 0);
		
	if (m_multisamplePower > 1)
	if(currentFrameBufferID != m_frameBufferID)
	{
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, currentFrameBufferID);
	}
	
	if (GL_TEXTURE_RECTANGLE_ARB == m_target)
	{
		GLFUNC(glDisable)(m_target);
	}

	return true;
}

bool CFrameBuffer::AttachDepthTexture(unsigned int textureID)
{
	assert(m_frameBufferID != 0xFFFFFFFF);
	if(m_frameBufferID == 0xFFFFFFFF)
	{
		return false;
	}

	GLFUNC(glFramebufferTexture2DEXT)(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, m_target, textureID, 0);

	return true;
}

bool CFrameBuffer::AttachDepthBuffer(unsigned int depthBufferID, bool isMultisample)
{
	if (isMultisample)
	{
		assert(m_msFrameBufferID != 0xFFFFFFFF);
		if (m_msFrameBufferID == 0xFFFFFFFF)
		{
			return false;
		}		

		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_msFrameBufferID);
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, depthBufferID);
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBufferID);
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);
	}
	else
	{
		assert(m_frameBufferID != 0xFFFFFFFF);
		if (m_frameBufferID == 0xFFFFFFFF)
		{
			return false;
		}
		
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, depthBufferID);
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBufferID);
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
	}

	return true;
}

bool CFrameBuffer::AttachStencilBuffer(unsigned int stencilBufferID, bool isMultisample)
{
	if (isMultisample)
	{
		assert(m_msFrameBufferID != 0xFFFFFFFF);
		if (m_msFrameBufferID == 0xFFFFFFFF)
		{
			return false;
		}

		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_msFrameBufferID);
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, stencilBufferID);
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, stencilBufferID);
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);
	}
	else
	{
		assert(m_frameBufferID != 0xFFFFFFFF);
		if (m_frameBufferID == 0xFFFFFFFF)
		{
			return false;
		}

		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, stencilBufferID);
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, stencilBufferID);
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
	}

	return true;
}

bool CFrameBuffer::AttachDepthStencilBuffer(unsigned int depthStencilBufferID, bool isMultisample)
{
	if (isMultisample)
	{
		assert(m_msFrameBufferID != 0xFFFFFFFF);
		if (m_msFrameBufferID == 0xFFFFFFFF)
		{
			return false;
		}
		
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_msFrameBufferID);
		//int glErr = glGetError();
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, depthStencilBufferID);
		//int glErr2 = glGetError();
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthStencilBufferID);
		//int glErr3 = glGetError();
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthStencilBufferID);
		//int glErr4 = glGetError();
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
		//int glErr5 = glGetError();
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);
		/*int glErr6 = glGetError();
		int ii = 0;*/
	}
	else
	{
		assert(m_frameBufferID != 0xFFFFFFFF);
		if (m_frameBufferID == 0xFFFFFFFF)
		{
			return false;
		}

		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, depthStencilBufferID);
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthStencilBufferID);
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthStencilBufferID);
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
	}

	return true;
}

bool CFrameBuffer::AttachColorBuffer(unsigned int colorBufferID, bool isMultisample)
{
	if (isMultisample)
	{
		assert(m_msFrameBufferID != 0xFFFFFFFF);
		if (m_msFrameBufferID == 0xFFFFFFFF)
		{
			return false;
		}

		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_msFrameBufferID);		
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, colorBufferID);		
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, colorBufferID);				
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);		
		GLFUNC(glBindFramebufferEXT)(GL_FRAMEBUFFER_EXT, m_frameBufferID);
	}
	else
	{
		assert(m_frameBufferID != 0xFFFFFFFF);
		if (m_frameBufferID == 0xFFFFFFFF)
		{
			return false;
		}

		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, colorBufferID);
		GLFUNC(glFramebufferRenderbufferEXT)(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, colorBufferID);		
		GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
	}

	return true;
}

bool CFrameBuffer::AttachDepthStencilTexture(unsigned int textureID)
{
	assert(m_frameBufferID != 0xFFFFFFFF);
	if(m_frameBufferID == 0xFFFFFFFF)
	{
		return false;
	}

	GLFUNC(glBindTexture)(m_target, textureID);
	GLFUNC(glFramebufferTexture2DEXT)(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, m_target, textureID, 0);
	GLFUNC(glFramebufferTexture2DEXT)(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, m_target, textureID, 0);

	return true;
}

bool CFrameBuffer::DetachColorTexture(int targetID)
{
	return AttachColorTexture(0, targetID);
}
	
bool CFrameBuffer::DetachDepthTexture()
{
	return AttachDepthTexture(0);
}

bool CFrameBuffer::DetachDepthBuffer(bool isMultisample)
{
	if (!AttachDepthBuffer(0, isMultisample))
	{
		return false;
	}

	if (isMultisample)
	{
		m_msDepthBufferID = 0xFFFFFFFF;
	}
	else
	{
		m_depthBufferID = 0xFFFFFFFF;
	}

	return true;
}

bool CFrameBuffer::DetachColorBuffer(bool isMultisample)
{
	if (!AttachColorBuffer(0, isMultisample))
	{
		return false;
	}

	if (isMultisample)
	{
		m_msColorBufferID = 0xFFFFFFFF;
	}

	return true;
}


bool CFrameBuffer::DetachStencilBuffer(bool isMultisample)
{
	if (!AttachStencilBuffer(0, isMultisample))
	{
		return false;
	}

	if (isMultisample)
	{
		m_msStencilBufferID = 0xFFFFFFFF;
	}
	else
	{
		m_stencilBufferID = 0xFFFFFFFF;
	}

	return true;
}

bool CFrameBuffer::DetachDepthStencilTexture()
{
	if (m_depthStencilTextureID != 0xFFFFFFFF)
	if (!AttachDepthStencilTexture(0))
	{
		return false;
	}

	m_depthStencilTextureID = 0xFFFFFFFF;
	return true;
}

unsigned int CFrameBuffer::CreateColorTexture(int target, int width, int height, GLenum format, GLenum internalFormat, bool isLinearInterpolation)
{
	unsigned int textureID;

	GLFUNC(glGenTextures)(1, &textureID);

	if (GL_TEXTURE_RECTANGLE_ARB == target)
	{
		GLFUNC(glEnable)(target);
	}

    GLFUNC(glBindTexture)(target, textureID);
	GLFUNC(glTexImage2D)(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

	if (!isLinearInterpolation)
	{
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	
    GLFUNC(glTexParameteri)(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLFUNC(glTexParameteri)(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (GL_TEXTURE_RECTANGLE_ARB == target)
	{
		GLFUNC(glDisable)(target);
	}

	GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);

    return textureID;
}

unsigned int CFrameBuffer::CreateColorTexture(GLenum format, GLenum internalFormat, bool isLinearInterpolation)
{
	return CreateColorTexture(m_target, m_width, m_height, format, internalFormat, isLinearInterpolation);
}

unsigned int CFrameBuffer::CreateDepthTexture(unsigned int target, unsigned int width,
											  unsigned int height, GLenum depthFormat, 
											  int depthCompareMode)
{
	unsigned int textureID;

	GLFUNC(glGenTextures)(1, &textureID);
    GLFUNC(glBindTexture)(target, textureID);

	GLFUNC(glTexImage2D)(target, 0, depthFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	GLFUNC(glTexParameteri)(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLFUNC(glTexParameteri)(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	GLfloat color[4];
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f;
	GLFUNC(glTexParameterfv)(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);

	switch (depthCompareMode)
	{
	case DEPTH_COMPARE_NONE:
		// do nothing
		break;

	case DEPTH_COMPARE_R_LEQUAL:
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
		GLFUNC(glBindTexture)(target, 0);
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE);
		break;

	case DEPTH_COMPARE_R_GEQUAL:
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_COMPARE_FUNC_ARB, GL_GEQUAL);
		GLFUNC(glBindTexture)(target, 0);
		GLFUNC(glTexParameteri)(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE);
		break;
	}

    return textureID;
}

unsigned int CFrameBuffer::CreateDepthTexture(GLenum depthFormat, int depthCompareMode)
{
	return CreateDepthTexture(m_target, m_width, m_height, depthFormat, depthCompareMode);
}

unsigned int CFrameBuffer::CreateDepthBuffer(GLenum depthFormat, bool isMultisample)
{
	unsigned int depthBufferID;

	GLFUNC(glGenRenderbuffersEXT)(1, &depthBufferID);
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, depthBufferID);
	if (isMultisample)
	{
		GLFUNC(glRenderbufferStorageMultisampleEXT)(GL_RENDERBUFFER_EXT,  m_multisamplePower, /*depthFormat*/GL_DEPTH_COMPONENT, m_width, m_height);
		int glErr = glGetError();
		if (glErr != 0)
		{
			GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
			DeleteRenderbuffer(depthBufferID);
			return 0xFFFFFFFF;
		}
	}
	else
	{
		GLFUNC(glRenderbufferStorageEXT)(GL_RENDERBUFFER_EXT, depthFormat, m_width, m_height);
	}
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);

	return depthBufferID;
}

unsigned int CFrameBuffer::CreateStencilBuffer(GLenum stencilFormat, bool isMultisample)
{
	unsigned int stencilBufferID;

	GLFUNC(glGenRenderbuffersEXT)(1, &stencilBufferID);
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, stencilBufferID);
	if (isMultisample)
	{
		GLFUNC(glRenderbufferStorageMultisampleEXT)(GL_RENDERBUFFER_EXT, m_multisamplePower, stencilFormat, m_width, m_height);
		int glErr = glGetError();
		if (glErr != 0)
		{
			GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
			DeleteRenderbuffer(stencilBufferID);
			return 0xFFFFFFFF;
		}
	}
	else
	{
		GLFUNC(glRenderbufferStorageEXT)(GL_RENDERBUFFER_EXT, stencilFormat, m_width, m_height);
	}
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);

	return stencilBufferID;
}

unsigned int CFrameBuffer::CreateColorBuffer(bool isMultisample)
{
	unsigned int colorBufferID;

	GLFUNC(glGenRenderbuffersEXT)(1, &colorBufferID);
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, colorBufferID);
	if (isMultisample)
	{
		GLFUNC(glRenderbufferStorageMultisampleEXT)(GL_RENDERBUFFER_EXT, m_multisamplePower, GL_RGBA, m_width, m_height);
		int glErr = glGetError();
		if (glErr != 0)
		{
			GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
			DeleteRenderbuffer(colorBufferID);
			return 0xFFFFFFFF;
		}
	}
	else
	{
		GLFUNC(glRenderbufferStorageEXT)(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, m_width, m_height);
	}
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);

	return colorBufferID;
}

unsigned int CFrameBuffer::CreateDepthStencilBuffer(bool isMultisample)
{
	unsigned int depthStencilBufferID;

	GLFUNC(glGenRenderbuffersEXT)(1, &depthStencilBufferID);
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, depthStencilBufferID);
	if (isMultisample)
	{
		GLFUNC(glRenderbufferStorageMultisampleEXT)(GL_RENDERBUFFER_EXT, m_multisamplePower, GL_DEPTH_STENCIL_EXT, m_width, m_height);
		int glErr = glGetError();
		if (glErr != 0)
		{
			GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);
			DeleteRenderbuffer(depthStencilBufferID);
			return 0xFFFFFFFF;
		}
	}
	else
	{
		GLFUNC(glRenderbufferStorageEXT)(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, m_width, m_height);
	}
	GLFUNC(glBindRenderbufferEXT)(GL_RENDERBUFFER_EXT, 0);

	return depthStencilBufferID;
}

unsigned int CFrameBuffer::CreateDepthStencilTexture()
{
	GLuint depthStencilTextureID = 0;
	
	GLFUNC(glGenTextures)(1, &depthStencilTextureID);
	GLFUNC(glBindTexture)(m_target, depthStencilTextureID);

    GLFUNC(glTexParameterf)(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLFUNC(glTexParameterf)(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLFUNC(glTexParameterf)(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GLFUNC(glTexParameterf)(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	GLFUNC(glTexParameteri)(m_target, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	GLFUNC(glTexImage2D)(m_target, 0, GL_DEPTH24_STENCIL8_EXT, m_width, m_height, 0, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, NULL);

	return depthStencilTextureID;
}

int	CFrameBuffer::GetMaxColorAttachments()
{
    int attachmentCount;
    GLFUNC(glGetIntegerv)(GL_MAX_COLOR_ATTACHMENTS_EXT, &attachmentCount);
	return attachmentCount;
}

int	CFrameBuffer::GetMaxSize()
{
    int maxSize;
    GLFUNC(glGetIntegerv)(GL_MAX_RENDERBUFFER_SIZE_EXT, &maxSize);
	return maxSize;
}

CFrameBuffer::~CFrameBuffer()
{
	if (m_depthBufferID != 0xFFFFFFFF)
	{
		DeleteRenderbuffer(m_depthBufferID);
	}

	if (m_stencilBufferID != 0xFFFFFFFF)
	{
		DeleteRenderbuffer(m_stencilBufferID);
	}

	if (m_depthStencilTextureID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteTextures)(1, &m_depthStencilTextureID);
	}

	if (m_frameBufferID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteFramebuffersEXT)(1, &m_frameBufferID);
	}

	if (m_msDepthBufferID != 0xFFFFFFFF)
	{
		DeleteRenderbuffer(m_msDepthBufferID);
	}

	if (m_msStencilBufferID != 0xFFFFFFFF)
	{
		DeleteRenderbuffer(m_msStencilBufferID);
	}

	if (m_msColorBufferID != 0xFFFFFFFF)
	{
		DeleteRenderbuffer(m_msColorBufferID);
	}

	if (m_msDepthStencilBufferID != 0xFFFFFFFF)
	{
		DeleteRenderbuffer(m_msDepthStencilBufferID);
	}

	if (m_msFrameBufferID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteFramebuffersEXT)(1, &m_msFrameBufferID);
	}
}