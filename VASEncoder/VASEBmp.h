#pragma once

#include "pixeldefs.h"

struct Pixel24 
{
	BYTE r;
	BYTE g;
	byte b;
};

class CVASEBmp
{
public:
	CVASEBmp(void);
	CVASEBmp(void * pDIB, const int & width, const int & height, const int & bpp);
	~CVASEBmp(void);

	inline void CVASEBmp::Create(const int & width, const int & height, const int & bpp)
	{
		m_width = width;
		m_height = height;
		m_bpp = bpp;
		m_pDIB = MP_NEW_ARR( BYTE, width * height * (bpp / 8));
		isMemAllocate = true;
	}

	inline void CVASEBmp::Create(const int & width, const int & height, const int & bpp, void* lpBits)
	{
		m_width = width;
		m_height = height;
		m_bpp = bpp;
		m_pDIB = lpBits;
		isMemAllocate = false;
	}

	inline BYTE * operator[](int x)
	{
		return &((BYTE *)m_pDIB)[x * m_width * (m_bpp >> 8)];
	}

	inline BYTE * GetPixAdress(const int & line, const int & column)
	{
		return &((BYTE *)m_pDIB)[(line * m_width + column) * (m_bpp >> 3)];
	}

	inline BYTE * GetLineAdress(const int & line)
	{
			return &((BYTE *)m_pDIB)[line * m_width * (m_bpp >> 3)];
	}

	inline SIZE GetSize()
	{
		SIZE size;
		size.cx = m_width;
		size.cy = m_height;
		return size;
	}

	inline int GetPitch()
	{
		return m_width * (int)((m_bpp+7) / 8);
	}

	inline int GetHeight()
	{
		return m_height;
	}

	inline int GetWidth()
	{
		return m_width;
	}

	inline BYTE * GetpDIB()
	{
		return (BYTE *)m_pDIB;
	}

	inline int GetBPP()
	{
		return m_bpp;
	}

	inline int GetDataSize()
	{
		return m_height * m_width * (m_bpp / 8);
	}

	inline bool HasAlpha()
	{
		return (m_bpp == 32) ? true: false;
	}

private:

	void * m_pDIB;
	int m_width;
	int m_height;
	int m_bpp; // бит в пикселе
	bool isMemAllocate;
};
