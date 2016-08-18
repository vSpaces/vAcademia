
#include "StdAfx.h"
#include "PBO.h"
#include <glew.h>
#include "GlobalSingletonStorage.h"
#include "OGLDebug.h"

CPBO::CPBO(unsigned int width, unsigned int height, unsigned char channelCount, bool isDirectChannelOrder, unsigned char usage):
	m_usage(usage),
	m_width(width),
	m_height(height),
	m_bufferID(0xFFFFFFFF),
	m_channelCount(channelCount),
	m_isDirectChannelOrder(isDirectChannelOrder)
{
	GLFUNC(glGenBuffersARB)(1, &m_bufferID);

	unsigned int bufferType = (m_usage == PBO_USAGE_READ_TEXTURE) ? GL_PIXEL_PACK_BUFFER_ARB : GL_PIXEL_UNPACK_BUFFER_ARB;
	unsigned int bufferUsage = (m_usage == PBO_USAGE_READ_TEXTURE) ? GL_DYNAMIC_READ : GL_STREAM_DRAW_ARB;
	GLFUNC(glBindBufferARB)(bufferType, m_bufferID);
	GLFUNC(glBufferDataARB)(bufferType, width * height * channelCount, 0, bufferUsage);
	GLFUNC(glBindBufferARB)(bufferType, 0);
}

unsigned int CPBO::GetPixelFormat()const
{
	if (3 == m_channelCount)
	{
		return (m_isDirectChannelOrder) ? GL_RGB : GL_BGR_EXT;
	}
	else
	{
		return (m_isDirectChannelOrder) ? GL_RGBA : GL_BGRA_EXT;
	}
}

void CPBO::Capture(int level, int offset)
{
	if (m_usage == PBO_USAGE_READ_TEXTURE)
	{
		GLFUNC(glBindBufferARB)(GL_PIXEL_PACK_BUFFER_ARB, m_bufferID);
		if ((m_channelCount != 1) || (m_height != 1))
		{
			GLFUNC(glGetTexImage)(GL_TEXTURE_2D, level, GetPixelFormat(), GL_UNSIGNED_BYTE, (void *)offset);
		}
		else
		{
			GLFUNC(glGetCompressedTexImageARB)(GL_TEXTURE_2D, level, (void *)offset);
		}
		GLFUNC(glBindBufferARB)(GL_PIXEL_PACK_BUFFER_ARB, 0);
	}
	else
	{
		GLFUNC(glBindBufferARB)(GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferID);
	}
}

void CPBO::Uncapture()
{
	if (m_usage != PBO_USAGE_READ_TEXTURE)
	{
		GLFUNC(glBindBufferARB)(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	}
}

void* CPBO::GetPointer()
{
	if (m_usage == PBO_USAGE_READ_TEXTURE)
	{
		GLFUNC(glBindBufferARB)(GL_PIXEL_PACK_BUFFER_ARB, m_bufferID);
		GLubyte* src = (GLubyte*)GLFUNCR(glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB));
		return src;
	}
	else
	{
		GLFUNC(glBindBufferARB)(GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferID);
		GLubyte* src = (GLubyte*)GLFUNCR(glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB));
		return src;
	}
}

void CPBO::ReleasePointer()
{
	if (m_usage == PBO_USAGE_READ_TEXTURE)
	{
		GLFUNC(glUnmapBufferARB)(GL_PIXEL_PACK_BUFFER_ARB);     
		GLFUNC(glBindBufferARB)(GL_PIXEL_PACK_BUFFER_ARB, 0);
	}
	else
	{
		GLFUNC(glUnmapBufferARB)(GL_PIXEL_UNPACK_BUFFER_ARB);     
		GLFUNC(glBindBufferARB)(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	}
}

CPBO::~CPBO()
{
	if (m_bufferID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteBuffersARB)(1, &m_bufferID);
	}
}