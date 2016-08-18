
#pragma once

#include "CommonObject.h"
#include "Polygons2DAccum.h"

#define SPRITE_TYPE_SPRITE			1
#define SPRITE_TYPE_PAPER			2
#define SPRITE_TYPE_TEXT			3
#define SPRITE_TYPE_FORMATTED_TEXT	4
#define SPRITE_TYPE_LINE			5

//#define USE_VISIBLE_DESTINATION		1

class C2DScene;

class IVisibilityProvider
{
public:
	virtual bool IsVisible() = 0;
};

class CBaseSprite : public CCommonObject
{
public:
	CBaseSprite(unsigned char type);
	~CBaseSprite();

	virtual bool IsNeedToUpdate();

	virtual void Draw(CPolygons2DAccum* const accum) = 0;
	virtual void Draw(const int x, const int y, CPolygons2DAccum* const accum) = 0;
	
	void SetScene2D(C2DScene* const scene2d);
	C2DScene* GetScene2D()const;
	
	void SetCoords(const int x, const int y);		
	void SetSize(const int width, const int height);
	void SetDepth(int depth);
#ifndef USE_VISIBLE_DESTINATION
	void SetVisible(const bool isVisible);
#else
	void SetVisible(const bool isVisible, const int from);

	void SetVisibilityProvider(IVisibilityProvider* visibilityProvider);
	IVisibilityProvider* GetVisibilityProvider()const;
#endif
	void SetLoadedStatus(const bool isLoaded);
	void SetScissors(const int x, const int y, const int width, const int height);
	
	__forceinline void GetCoords(int& x, int& y)
	{
		x = m_x;
		y = m_y;
	}

	__forceinline int GetX()const
	{
		return m_x;
	}

	__forceinline int GetY()const
	{
		return m_y;
	}

	__forceinline bool IsSizeDefined()
	{
		return m_isSizeDefined;
	}

	__forceinline void GetSize(int& width, int& height)
	{
		width = m_width;
		height = m_height;
	}

	__forceinline int GetWidth()const
	{
		return m_width;
	}

	__forceinline int GetHeight()const
	{
		return m_height;
	}

	__forceinline void SetResourceID(const unsigned char id)
	{
		memcpy(((unsigned char *)&m_sortValue), &id, 1);
	}

	__forceinline unsigned char GetResourceID()const
	{
		return *(unsigned char *)&m_sortValue;
	}

	__forceinline unsigned int GetSortValue()const
	{
		return m_sortValue;
	}

	__forceinline int GetDepth()const
	{
		return m_depth;
	}

	__forceinline bool IsVisible()const
	{
		return m_isVisible;
	}

	__forceinline bool IsLoaded()const
	{
		return m_isLoaded;
	}

	__forceinline void GetScissors(int& x, int& y, int& width, int& height)const
	{
		x = m_scissorsX;
		y = m_scissorsY;
		width = m_scissorsWidth;
		height = m_scissorsHeight;
	}

	void IncrementUselessAsMask();
	void DecrementUselessAsMask();
	void SetMaskSprite(CBaseSprite* const mask);
	CBaseSprite* GetMaskSprite()const;
	__forceinline bool IsUsedAsMask()const
	{
		return (m_uselessAsMaskCount > 0);
	}	
	
	void OnBeforeDraw(CPolygons2DAccum* const accum);
	void OnAfterDraw(CPolygons2DAccum* const accum);
	
	void SetType(const unsigned char type);
	__forceinline unsigned char GetType()const
	{
		return m_type;
	}	
	
protected:
	int m_uselessAsMaskCount;
	int m_width, m_height;
	int m_x, m_y;
	int m_depth;

	unsigned int m_sortValue;

	int m_scissorsX, m_scissorsY;
	int m_scissorsWidth, m_scissorsHeight;

	bool m_isLoaded;
	bool m_isVisible;
	bool m_isSizeDefined;
	unsigned char m_type;

	C2DScene* m_scene2d;
	CBaseSprite* m_maskSprite;

#ifdef USE_VISIBLE_DESTINATION
	IVisibilityProvider* m_visibilityProvider;
#endif
};