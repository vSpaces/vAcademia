#include "StdAfx.h"
#include "../../include/selectors/SafeRect.h"

#define SIZE_CORRECT_DIV		4

CSafeRect::CSafeRect(unsigned int screenWidth, unsigned int screenHeight):
	m_x(0),
	m_y(0),
	m_x2(0),
	m_y2(0),
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight)
{
}

void CSafeRect::Init(int x1, int y1, int x2, int y2)
{
	int lessZeroX = 0;
	int lessZeroY = 0;
		
	m_x = x1;
	m_x2 = x2;

	if (x1 < 0)
	{
		lessZeroX = -x1;
		m_x = 0;
		m_x2 += -x1;
	}

	if (m_x2 < m_x)
	{
		m_x = x2;
		m_x2 = x1;
	}

	m_y = y1;
	m_y2 = y2;

	if (y1 < 0)
	{
		lessZeroY = -y1;
		m_y = 0;
		m_y2 += -y1;
	}

	if (m_y2 < m_y)
	{
		m_y = y2;
		m_y2 = y1;
	}

	

	int tryCount = 0;
	if ((m_x2 - m_x)%SIZE_CORRECT_DIV == 0)
		m_x2--;
	else
		while ((m_x2 - m_x + 1)%SIZE_CORRECT_DIV != 0)
		{
			if (0 == tryCount%2)
			{
				if (m_x != 0)
				{
					m_x--;
				}
			}
			else
			{
				if (m_x2 + 1 < m_screenWidth)
				{
					m_x2++;
				}
			}

			tryCount++;
		}

	tryCount = 0;
	if ((m_y2 - m_y)%SIZE_CORRECT_DIV == 0)
		m_y2--;
	else
		while ((m_y2 - m_y + 1)%SIZE_CORRECT_DIV != 0)
		{
			if (0 == tryCount%2)
			{
				if (m_y != 0)
				{
					m_y--;
				}
			}
			else
			{
				if (m_y2 + 1 < m_screenHeight)
				{
					m_y2++;
				}
			}

			tryCount++;
		}
	if (lessZeroX > 0)
	{
		m_x -= lessZeroX;
		m_x2 -= lessZeroX;
	}
	if (lessZeroY > 0)
	{
		m_y -= lessZeroY;
		m_y2 -= lessZeroY;
	}
}

void CSafeRect::Init2(unsigned int x1, unsigned int y1, unsigned int width, unsigned int height)
{
//	Init(x1, y1, x1 + width, y1 + height);
}

unsigned int CSafeRect::GetX()const
{
	return m_x;
}

unsigned int CSafeRect::GetY()const
{
	return m_y;
}

unsigned int CSafeRect::GetX2()const
{
	return m_x2;
}

unsigned int CSafeRect::GetY2()const
{
	return m_y2;
}

unsigned int CSafeRect::GetWidth()const
{
	return m_x2 - m_x;
}

unsigned int CSafeRect::GetHeight()const
{
	return m_y2 - m_y;
}

CSafeRect::~CSafeRect()
{
}