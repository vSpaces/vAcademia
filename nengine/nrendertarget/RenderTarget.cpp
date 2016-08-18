
#include "StdAfx.h"
#include "RenderTarget.h"
#include "GlobalSingletonStorage.h"

CRenderTarget::CRenderTarget(int width, int height, int flags):
	m_pbufferTextureID(0xFFFFFFFF),
	m_target(GL_TEXTURE_2D),
	m_isRTEmulation(false),
	m_frameBuffer(NULL),
	m_pbuffer(NULL),
	m_flags(flags),
	m_isOk(true)
{
	/*int majorVersion, minorVersion;
	g->gi.GetDriverVersion(majorVersion, minorVersion);

	bool isFramebufferUsed = ((g->ei.IsExtensionSupported(GLEXT_ext_framebuffer_object)) && (!((g->gi.GetVendorID() == VENDOR_ATI) && (majorVersion < 8) && (!g->cc.IsAtiBuildSupport(build)))));*/

	bool isFramebufferUsed = g->cc.AreExtendedAbilitiesSupported();

	if (!isFramebufferUsed)
	{
		int mode = 0;

		/*if ((flags & CFrameBuffer::depth16) || (flags & CFrameBuffer::depth24)
			|| (flags & CFrameBuffer::depth32) || (flags & CFrameBuffer::depth24_stencil8))
		{*/
			//mode = mode + MODE_DEPTH;
		//}

		if (flags & CFrameBuffer::depth24_stencil8)
		{
			mode = mode + MODE_STENCIL;
		}
		else
		{
			mode = mode + MODE_DEPTH;
		}

		mode = mode + MODE_ALPHA;
		mode = mode + MODE_TEXTURE_2D;;
					
		MP_NEW_V3(m_pbuffer, CPBuffer, width, height, mode);
	}
	else 
	{
		MP_NEW_V3(m_frameBuffer, CFrameBuffer, width, height, flags);
	}
}

unsigned char CRenderTarget::GetType()
{
	//
	//return RENDER_TARGET_MAINBUFFER;

	if (m_isRTEmulation)
	{
		return RENDER_TARGET_MAINBUFFER;
	}

	if (!m_isOk)
	{
		return RENDER_TARGET_INVALID;
	}

	return (m_frameBuffer) ? RENDER_TARGET_FBO : RENDER_TARGET_PBUFFER;
}
	
int	CRenderTarget::GetWidth()const
{
	if (!m_isOk)
	{
		return 0;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->GetWidth();
	}
	else
	{
		return m_pbuffer->GetWidth();
	}
}

int	CRenderTarget::GetHeight()const
{
	if (!m_isOk)
	{
		return 0;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->GetHeight();
	}
	else
	{
		return m_pbuffer->GetHeight();
	}
}

bool CRenderTarget::IsStencilBufferExists()const
{
	if (!m_isOk)
	{
		return false;
	}

	return (((m_flags & MODE_STENCIL_1) == 0) || ((m_flags & MODE_STENCIL_4) == 0)
			|| ((m_flags & MODE_STENCIL_8) == 0) || ((m_flags & MODE_STENCIL_16) == 0));
}

bool CRenderTarget::IsDepthBufferExists()const
{
	if (!m_isOk)
	{
		return false;
	}

	return (((m_flags & MODE_DEPTH_16) == 0) || ((m_flags & MODE_DEPTH_24) == 0)
			|| ((m_flags & MODE_DEPTH_32) == 0));
}

void CRenderTarget::SetOk(bool isOk)
{
	m_isOk = isOk;
}
		
bool CRenderTarget::IsOk() const
{
	return m_isOk;
}

bool CRenderTarget::Create(bool isCreateTexturesAutomatically)
{
	if (m_frameBuffer)
	{
		m_isOk = m_frameBuffer->Create(isCreateTexturesAutomatically) != -1;

		if (g->gi.GetVendorID() == VENDOR_INTEL)
		{
			m_isRTEmulation = true;
		}
	}
	else
	{
		/*m_isOk = m_pbuffer->Create();
		if (!m_isOk)
		{
			m_isRTEmulation = true;
		}*/
		m_isOk = true;
		m_isRTEmulation = true;
	}

	return m_isOk;
}

bool CRenderTarget::Bind()
{
	if (!m_isOk)
	{
		return false;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->Bind();
	}
	else
	{
		return m_pbuffer->MakeCurrent();
	}
}

bool CRenderTarget::Unbind()
{
	if (!m_isOk)
	{
		return false;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->Unbind();
	}
	else
	{
		if (m_pbufferTextureID != -1)
		{
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_pbufferTextureID);
			m_pbuffer->BindToTexture(m_pbufferTextureID);
		}

		return m_pbuffer->RestoreCurrent();
	}
}
	
bool CRenderTarget::AttachColorTexture(unsigned textureID, int targetID)
{
	if (!m_isOk)
	{
		return false;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->AttachColorTexture(textureID, targetID);
	}
	else
	{
		m_pbufferTextureID = textureID;	
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_pbufferTextureID);
		m_pbuffer->BindToTexture(m_pbufferTextureID);

		return true;
	}
}

bool CRenderTarget::AttachDepthTexture(unsigned textureID)
{
	if (!m_isOk)
	{
		return false;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->AttachDepthTexture(textureID);
	}
	else
	{
		if (textureID != -1)
		{
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);
			return m_pbuffer->BindToDepthTexture(textureID);
		}

		return false;
	}
}
	
bool CRenderTarget::DetachColorTexture()
{
	if (!m_isOk)
	{
		return false;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->DetachColorTexture(0);
	}
	else
	{
		bool res = false;
		if (m_pbufferTextureID != -1)
		{
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_pbufferTextureID);
			res = m_pbuffer->UnbindFromTexture();
		}
		
		m_pbufferTextureID = 0xFFFFFFFF;	

		return res;
	}
}

bool CRenderTarget::DetachDepthTexture(GLenum target)
{
	if (!m_isOk)
	{
		return false;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->DetachColorTexture(target);
	}
	else
	{
		return false;
	}
}

unsigned int CRenderTarget::CreateDepthTexture(GLenum depthFormat, int depthCompareMode)
{
	if (!m_isOk)
	{
		return false;
	}

	if (m_frameBuffer)
	{
		return m_frameBuffer->CreateDepthTexture(depthFormat, depthCompareMode);
	}
	else
	{
		return CFrameBuffer::CreateDepthTexture(GL_TEXTURE_2D, GetWidth(), GetHeight(), depthFormat, depthCompareMode);
	}
}

unsigned int CRenderTarget::CreateColorTexture(GLenum format, GLenum internalFormat, bool isLinearInterpolation)
{
	//if (!m_isOk)
	//{
	//	return false;
	//}

	if (m_frameBuffer)
	{
		return m_frameBuffer->CreateColorTexture(format, internalFormat, isLinearInterpolation);
	}
	else
	{
		unsigned pbufferTex;
		GLFUNC(glGenTextures)(1, &pbufferTex);
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, pbufferTex);

		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);								// set 1-byte alignment

		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// reqiured only for glCopyTexSubImage2D
		GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, internalFormat, m_pbuffer->GetWidth(), m_pbuffer->GetHeight(),
	               0, format, GL_UNSIGNED_BYTE, NULL);

		return pbufferTex;
	}
}
	
int CRenderTarget::MaxSize()
{
	return CFrameBuffer::GetMaxSize();
}

CFrameBuffer* CRenderTarget::GetFrameBuffer()const
{
	return m_frameBuffer;
}

CRenderTarget::~CRenderTarget()
{
	if (m_frameBuffer)
	{
		MP_DELETE(m_frameBuffer);
	}

	if (m_pbuffer)
	{
		MP_DELETE(m_pbuffer);
	}
}
