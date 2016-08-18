#ifndef INCL_PIXEL
#define INCL_PIXEL

#include "pixeldefs.h"
#include "Pixel.h"

class CPixel32
{
	public:
		CPixel32 ();
		CPixel32 (BYTE r, BYTE g, BYTE b, BYTE a);

		void Set (BYTE r, BYTE g, BYTE b, BYTE a);

		void SetR (BYTE R);
		void SetG (BYTE g);
		void SetB (BYTE b);
		void SetA (BYTE a);

		BYTE GetR ();
		BYTE GetG ();
		BYTE GetB ();
		BYTE GetA ();

		BYTE * GetData();

    bool operator ==(const CPixel32&);
    bool operator !=(const CPixel32&);

	private:
		BYTE m_Data[4];  
};

inline CPixel32::CPixel32()
{
}


inline CPixel32::CPixel32(BYTE r, BYTE g, BYTE b, BYTE a)
{
  Set (r, b, g, a);
}

inline BYTE * CPixel32::GetData()
{
	return &m_Data[0];
}

inline void CPixel32::Set(BYTE r, BYTE g, BYTE b, BYTE a)
{
  m_Data[RGBA_RED] = r;
  m_Data[RGBA_GREEN] = g;
  m_Data[RGBA_BLUE] = b;
  m_Data[RGBA_ALPHA] = a;
}


inline void CPixel32::SetR(BYTE r)
{
  m_Data[RGBA_RED] = r;
}


inline void CPixel32::SetG(BYTE g)
{
  m_Data[RGBA_GREEN] = g;
}


inline void CPixel32::SetB(BYTE b)
{
  m_Data[RGBA_BLUE] = b;
}


inline void CPixel32::SetA(BYTE a)
{
  m_Data[RGBA_ALPHA] = a;
}


inline BYTE CPixel32::GetR()
{
  return m_Data[RGBA_RED];
}


inline BYTE CPixel32::GetG()
{
  return m_Data[RGBA_GREEN];
}


inline BYTE CPixel32::GetB()
{
  return m_Data[RGBA_BLUE];
}


inline BYTE CPixel32::GetA()
{
  return m_Data[RGBA_ALPHA];
}

inline bool CPixel32::operator ==(const CPixel32& pixel)
{
  return (*(const LONG *)this == *(const LONG*)&pixel);
}

inline bool CPixel32::operator !=(const CPixel32& pixel)
{
  return (!(*this == pixel));
}

#endif

