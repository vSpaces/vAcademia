
#pragma once

#include	<windows.h>
#include	<glew.h>
#include	<wglew.h>

#define MODE_ALPHA				1
#define MODE_DEPTH				2
#define MODE_STENCIL			4
#define MODE_ACCUM				8
#define MODE_DOUBLE				16
#define MODE_TEXTURE_1D			32
#define MODE_TEXTURE_2D			64
#define MODE_TEXTURE_CUBEMAP	128
#define MODE_TEXTURE_MIPMAP		256

class	CPBuffer
{
public:
	CPBuffer(int width, int height, int mode = MODE_ALPHA | MODE_DEPTH | MODE_STENCIL, bool isShareObjects = true);
	~CPBuffer ();

	int	GetWidth()const;
	int	GetHeight()const;
	bool IsShareObjects()const;
	int	GetMode()const;

	void Clear();
	bool Create();

	// set attribute (like WGL_TEXTURE_CUBE_MAP*_ARB)
	bool SetAttribute(int attr, int value);
	// direct rendering to this pbuffer
	bool MakeCurrent();
	// direct rendering to prev. target
	bool RestoreCurrent();	
	// whether p-buffer is lost due to mode switch
	bool IsLost()const;			
	// bind pbuffer to previously bound texture
	bool BindToTexture(unsigned int textureID, int buf = WGL_FRONT_LEFT_ARB);
	bool BindToDepthTexture(unsigned int depthTextureID);
	// unbind from texture (release)
	bool UnbindFromTexture(int buf = WGL_FRONT_LEFT_ARB);
	// set specific cubemap side, call glFlush after side is done
	bool SetCubemapSide(int side);
	void PrintLastError();

private:
	HDC	m_hDC;
	HGLRC m_hGLRC;
	HDC m_hDCsave;
	HGLRC m_hGLRCsave;

	HPBUFFERARB m_hBuffer;

	int	m_width;
	int m_height;
	int m_mode;

	int m_depthTextureID;
	unsigned int m_textureID;
	bool m_canBindTexture;

	bool m_isShareObjects;
};
