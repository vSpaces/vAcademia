
#include "StdAfx.h"
#include "PBuffer.h"
#include "GlobalSingletonStorage.h"

#define	MAX_ATTRIBUTES	20
#define	MAX_PFORMATS	20

class	AttrList
{
private:
	int	m_attrs[2 * MAX_ATTRIBUTES];
	int	m_attrCount;
public:
	AttrList()
	{
		m_attrCount = 0;

		Clear();
	}

	void Clear()
	{
		for (int i = 0; i < 2 * MAX_ATTRIBUTES; i++)
		{
			m_attrs[i] = 0;
		}
	}

	void Add(int attr, int value)
	{
		m_attrs[2 * m_attrCount] = attr;
		m_attrs[2 * m_attrCount + 1] = value;
		m_attrCount++;
	}

	int	GetAttributeCount()const
	{
		return m_attrCount;
	}

	const int* GetAttributes()const
	{
		return m_attrs;
	}
};

CPBuffer::CPBuffer(int width, int height, int mode, bool isShareObjects):
	m_isShareObjects(isShareObjects),
	m_canBindTexture(true),
	m_depthTextureID(-1),
	m_hGLRCsave(NULL),
	m_height(height),
	m_hBuffer(NULL),
	m_hDCsave(NULL),
	m_width(width),
	m_hGLRC(NULL),
	m_mode(mode),
	m_hDC(NULL)
{
}

int	CPBuffer::GetWidth()const
{
	return m_width;
}

int	CPBuffer::GetHeight()const
{
	return m_height;
}

bool CPBuffer::IsShareObjects()const
{
	return m_isShareObjects;
}

int	CPBuffer::GetMode()const
{
	return m_mode;
}

void CPBuffer::Clear()
{
	if (m_hBuffer != NULL)
	{
		GLFUNC(wglDeleteContext)(m_hGLRC);
		GLFUNC(wglReleasePbufferDCARB)(m_hBuffer, m_hDC);
		GLFUNC(wglDestroyPbufferARB)(m_hBuffer);
	}
}

bool CPBuffer::Create()
{
	AttrList floatAttrs;
	AttrList intAttrs;
	int format;

	/*HGLRC hCurrentRC = wglGetCurrentContext();
	HDC	hCurrentDC = wglGetCurrentDC();*/
	HGLRC hCurrentRC = g->ne.ghRC;
	HDC	hCurrentDC = g->ne.ghDC;

	g->lw.WriteLn("Creating pbuffer");
	
	// check for extension support & initialization
	if ((!wglChoosePixelFormatARB) || (!wglCreatePbufferARB) ||
	    (!wglGetPbufferDCARB) || (!wglQueryPbufferARB) ||
	    (!wglReleasePbufferDCARB) || (!wglDestroyPbufferARB))
	{
		g->lw.WriteLn("P-buffer error1");
		return false;
	}

	m_canBindTexture = true;
	if ((m_mode & MODE_TEXTURE_1D) || (m_mode & MODE_TEXTURE_2D) || (m_mode & MODE_TEXTURE_CUBEMAP))
	if ((!wglBindTexImageARB) || (!wglReleaseTexImageARB) || (!wglSetPbufferAttribARB))
	{
		m_canBindTexture = false;
	}

	intAttrs.Add(WGL_SUPPORT_OPENGL_ARB,  GL_TRUE);
	intAttrs.Add(WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE);
	intAttrs.Add(WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB);
	intAttrs.Add(WGL_RED_BITS_ARB, 8);
	intAttrs.Add(WGL_GREEN_BITS_ARB, 8);
	intAttrs.Add(WGL_BLUE_BITS_ARB, 8);
	intAttrs.Add(WGL_DOUBLE_BUFFER_ARB, (m_mode & MODE_DOUBLE ? GL_TRUE : GL_FALSE));

	if (m_mode & MODE_ALPHA)
	{
		intAttrs.Add(WGL_ALPHA_BITS_ARB, 8);
	}

	if (m_mode & MODE_DEPTH)
	{
		intAttrs.Add(WGL_DEPTH_BITS_ARB, 24);
	}

	if (m_mode & MODE_STENCIL)
	{ 
		intAttrs.Add(WGL_STENCIL_BITS_ARB, 8);
	}

	if (m_mode & MODE_ACCUM)
	{
		intAttrs.Add(WGL_ACCUM_BITS_ARB, 32);
	}

	if (m_canBindTexture)
	if ((m_mode & MODE_TEXTURE_1D) || (m_mode & MODE_TEXTURE_2D) || (m_mode & MODE_TEXTURE_CUBEMAP))
	if (m_mode & MODE_ALPHA)
	{
		intAttrs.Add(WGL_BIND_TO_TEXTURE_RGBA_ARB, GL_TRUE);
	}
	else
	{
		intAttrs.Add(WGL_BIND_TO_TEXTURE_RGB_ARB, GL_TRUE);
	}

	int pixelFormats[MAX_PFORMATS];
	unsigned numFormats = 0;

	if (!wglChoosePixelFormatARB(hCurrentDC, intAttrs.GetAttributes(), (const float*)floatAttrs.GetAttributes(),
									MAX_PFORMATS, pixelFormats, &numFormats))
	{
		g->lw.WriteLn("P-buffer error2");
		return false;
	}

	if (numFormats < 1)
	{
		g->lw.WriteLn("P-buffer error3");
		return false;
	}

	format = pixelFormats[0];

	AttrList props;

	if ((m_mode & MODE_TEXTURE_1D) || (m_mode & MODE_TEXTURE_2D))
	if (m_mode & MODE_ALPHA)
	{
		props.Add(WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB);
	}
	else
	{
		props.Add(WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGB_ARB);
	}

	if (m_canBindTexture)
	if (m_mode & MODE_TEXTURE_1D)
	{
		props.Add(WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_1D_ARB);
	}
	else if (m_mode & MODE_TEXTURE_2D)
	{
		props.Add(WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB);
	}
	else if (m_mode & MODE_TEXTURE_CUBEMAP)
	{
		props.Add(WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_CUBE_MAP_ARB);
	}

	if (m_mode & MODE_TEXTURE_MIPMAP)
	{
		props.Add(WGL_MIPMAP_TEXTURE_ARB, GL_TRUE);
	}

	m_hBuffer = GLFUNCR(wglCreatePbufferARB(hCurrentDC, format, m_width, m_height, props.GetAttributes()));
	if (!m_hBuffer)
	{
		DWORD err = GetLastError();
        switch (err) 
		{
			case ERROR_INVALID_PIXEL_FORMAT: g->lw.WriteLn("ERROR_INVALID_PIXEL_FORMAT");break;
			case ERROR_NO_SYSTEM_RESOURCES: g->lw.WriteLn("ERROR_NO_SYSTEM_RESOURCES");break;
			case ERROR_INVALID_DATA: g->lw.WriteLn("ERROR_INVALID_DATA");break;
			default: g->lw.WriteLn("unknown problem");break;
        }

		return false;
	}

	m_hDC = GLFUNCR(wglGetPbufferDCARB(m_hBuffer));
	if (!m_hDC)
	{
		g->lw.WriteLn("P-buffer error5");
		return false;
	}

	m_hGLRC = GLFUNCR(wglCreateContext(m_hDC));
	if (!m_hGLRC)
	{
		g->lw.WriteLn("P-buffer error6");
		return false;
	}

	if (m_isShareObjects)
	{
		GLFUNC(wglShareLists)(hCurrentRC, m_hGLRC);
		int err = GetLastError();
		if (err != 0)
		{
			m_isShareObjects = false;
			g->lw.WriteLn("wglShareLists failed");
			return false;
		}
		/*	wglShareLists( m_hGLRC, hCurrentRC);
			int err = GetLastError();
			if (err != 0)
			{
				g->lw.WriteLn("wglShareLists failed 2");
	}

			return false;
		}*/
	}

	// get real size
	GLFUNC(wglQueryPbufferARB)(m_hBuffer, WGL_PBUFFER_WIDTH_ARB,  &m_width);
	GLFUNC(wglQueryPbufferARB)(m_hBuffer, WGL_PBUFFER_HEIGHT_ARB, &m_height);
	m_canBindTexture = false;


	g->lw.WriteLn("P-buffer created ok. ", m_width, " ", m_height);

	return true;
}

bool CPBuffer::MakeCurrent()
{
	m_hDCsave   = GLFUNCR(wglGetCurrentDC());
	m_hGLRCsave = GLFUNCR(wglGetCurrentContext());

	int res = GLFUNCR(wglMakeCurrent(m_hDC, m_hGLRC));

	return (res != 0);
}

bool CPBuffer::RestoreCurrent()
{
	static unsigned char* buf = NULL;

	if (!m_canBindTexture)
	{
		// fuck Intel!!!
		if (m_isShareObjects)
		{
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_textureID);
			GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);
		}		
	}

	if (!m_canBindTexture)
	if (m_depthTextureID != -1)
	{
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_depthTextureID);
		GLFUNC(glCopyTexImage2D)(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, m_width, m_height, 0);		
	}

	if ((!m_hDCsave) || (!m_hGLRCsave))
	{
		return false;
	}

	BOOL result = GLFUNCR(wglMakeCurrent(m_hDCsave, m_hGLRCsave));

	m_hDCsave = NULL;
	m_hGLRCsave = NULL;

	return (result > 0);
}

bool CPBuffer::IsLost()const
{
	int	lost = 0;
	GLFUNC(wglQueryPbufferARB)(m_hBuffer, WGL_PBUFFER_LOST_ARB, &lost);

	return (lost != 0);
}

bool CPBuffer::BindToTexture(unsigned int textureID, int buf)
{
	m_textureID = textureID;
	if (m_canBindTexture)
	{
		bool res = GLFUNCR(wglBindTexImageARB(m_hBuffer, buf)) > 0;
		return (res != GL_FALSE);
	}
	else
	{
		return false;
	}
}

bool CPBuffer::BindToDepthTexture(unsigned int textureID)
{
	m_depthTextureID = textureID;

	if (m_canBindTexture)
	{
		bool res = GLFUNCR(wglBindTexImageARB(m_hBuffer, WGL_DEPTH_COMPONENT_NV)) > 0;
		return (res != GL_FALSE);
	}
	else
	{
		return false;
	}
}

bool CPBuffer::UnbindFromTexture(int buf)
{
	if (m_canBindTexture)
	{
		bool res = GLFUNCR(wglReleaseTexImageARB(m_hBuffer, buf)) > 0;
		return (res != GL_FALSE);
	}
	else
	{
		return false;
	}
}

bool CPBuffer::SetAttribute(int attr, int value)
{
	AttrList attrs;
	attrs.Add(attr, value);

	bool res = GLFUNCR(wglSetPbufferAttribARB(m_hBuffer, attrs.GetAttributes())) > 0;

	return (res != GL_FALSE);
}

bool CPBuffer::SetCubemapSide(int side)
{
	if ((m_mode & MODE_TEXTURE_CUBEMAP) == 0)
	{
		return false;
	}

	GLFUNC(glFlush)();

	return SetAttribute(WGL_CUBE_MAP_FACE_ARB, side);
}

CPBuffer::~CPBuffer()
{
	Clear();
}
