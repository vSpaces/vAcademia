#pragma once

class CSafeRect
{
public:
	CSafeRect(unsigned int screenWidth, unsigned int screenHeight);
	~CSafeRect();

	void Init(int x1, int y1, int x2, int y2);
	void Init2(unsigned int x1, unsigned int y1, unsigned int width, unsigned int height);

	unsigned int GetX()const;
	unsigned int GetY()const;

	unsigned int GetX2()const;
	unsigned int GetY2()const;

	unsigned int GetWidth()const;
	unsigned int GetHeight()const;

private:
	unsigned int m_x, m_y, m_x2, m_y2;
	unsigned int m_screenWidth, m_screenHeight;
};