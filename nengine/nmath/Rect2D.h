
#pragma once

class CRect2D
{
public:
	__forceinline CRect2D(float _x1, float _y1, float _x2, float _y2)
	{
		x1 = _x1;
		y1 = _y1;
		x2 = _x2;
		y2 = _y2;
	}

	__forceinline ~CRect2D()
	{

	}

	__forceinline bool IsIntersected(CRect2D& rct)
	{
		return (IsPointIn(rct.x1, rct.y1) || IsPointIn(rct.x2, rct.y1) || IsPointIn(rct.x2, rct.y2)
			|| IsPointIn(rct.x1, rct.y2) || rct.IsPointIn(x1, y1) || rct.IsPointIn(x2, y1) 
			|| rct.IsPointIn(x1, y2) || rct.IsPointIn(x2, y2));
	}

private:
	__forceinline bool IsPointIn(float x, float y)
	{
		return ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2));
	}

	float x1, y1, x2, y2;
};