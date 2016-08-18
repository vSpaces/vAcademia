
#pragma once

#include "IChangesListener.h"
#include "IAsynchResourceManager.h"
#include "IAsynchResourceHandler.h"
#include "BaseChangesInformer.h"
#include "IAsynchResource.h"
#include "Polygons2DAccum.h"
#include "IDynamicTexture.h"
#include "CommonObject.h"
#include "IResLibrary.h"
#include "BaseSprite.h"
#include "Color3.h"
#include <string>
#include <vector>

typedef struct _SColorKey
{
	unsigned char type;
	unsigned char r;
	unsigned char g;
	unsigned char b;
} SColorKey;

class CSprite : public CBaseSprite, public IChangesListener, public CBaseChangesInformer, public IAsynchResourceHandler
{
public:
	CSprite();
	~CSprite();

	bool IsNeedToUpdate();

	void Draw(CPolygons2DAccum* const accum);
	void Draw(const int x, const int y, CPolygons2DAccum* const accum);

	bool LoadSpriteFromXML(std::string fileName);
	bool LoadFromTexture(std::string &fileName, bool isOnlyLoad = false, bool isMipMap = false, SColorKey* colorKey = NULL);
	bool LoadFromTexture(std::wstring &fileName, bool isOnlyLoad = false, bool isMipMap = false, SColorKey* colorKey = NULL);
	bool LoadFromTextureFile(std::string &fileName);
	bool LoadFromDynamicTexture(IDynamicTexture* dynamicTexture);

	void SetWorkRect(int u1, int v1, int u2, int v2);
	void GetWorkRect(int* u1, int* v1, int* u2, int* v2)const;
	void SetFullWorkRect();
	bool IsFullWorkRect()const;

	void SetColor(unsigned char r, unsigned char g, unsigned char b);
	void GetColor(unsigned char& r, unsigned char& g, unsigned char& b);

	void SetTiling(bool isTiling);
	bool IsTilingEnabled()const;

	void SetSmooth(bool isSmooth);
	bool IsSmoothEnabled()const;

	void SetCanBeCompressed(bool canBeCompressed);
	bool CanBeCompressed()const;

	void SetAlpha(unsigned char alpha);
	unsigned char GetAlpha()const;

	void SetTextureID(int textureID);
	void SetTexture(std::string name);

	bool AddTexture(std::string fileName);
	bool AddTexture(int textureID);

	void SetDeltaTime(int deltaTime);
	int GetDeltaTime()const;

	bool IsDynamic()const;

	bool ReplaceTexture(unsigned int num, int textureID);
	bool ReplaceTexture(unsigned int num, std::string fileName);
	int GetTextureID(unsigned int num)const;

	void OnChanged(int eventID);

	bool IsPixelTransparent(const int tx, const int ty)const;
	bool GetPixelColor(const int tx, const int ty, unsigned char& r, unsigned char& g, unsigned char& b)const;	

	bool IsUniqueTextureUsed()const;
	void SetUniqueTextureStatus(bool isUniqueTextureUsed);

	void RemoveDynamicTextures();

	unsigned int MakeSnapshot(bool toSystemMemory = false);

	void SetFrameCount(int frameCount);
	int GetFrameCount()const;

	void KeepInSystemMemory(bool isKeep);
	bool IsKeepInSystemMemory()const;

	void FreeResources();

	void OnAsynchResourceLoaded(IAsynchResource* asynch);
	void OnAsynchResourceError(IAsynchResource* asynch);
	void OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, unsigned char/* percent*/);

	void SetLoadingSpriteSize();

	void SetAngle(int angle);

	void SetMaxDecreaseKoef(unsigned char maxDecreaseKoef);
	unsigned char GetMaxDecreaseKoef()const;

	void SetPermitTransparent(bool isPermit);

	void DetachTexture();

	static void GetCurrentScreen(unsigned char*& data, unsigned int& width, unsigned int& height, unsigned char& channelCount);
	
protected:
	int m_tx1, m_ty1, m_tx2, m_ty2;
	unsigned char m_alpha;
	CColor3 m_color;

private:
	void LoadFromColorTexture(std::string fileName);
	void ClearTextureList();

	void ChangeTexture(unsigned int textureID);

	static unsigned int GetCorrectScreenWidth();
		
	int m_angle;
	unsigned char m_maxDecreaseKoef;
	int m_deltaTime;
	int m_frameCount;	
	int m_lastFrameNum;
	
	bool m_isSmooth;
	bool m_isPermitTransparent;

	bool m_isTiling;
	bool m_isDynamic;
	bool m_isSnapshot;
	bool m_isUsedAsMask;
	bool m_canBeCompressed;
	bool m_isWorkRectDefined;
	bool m_isUniqueTextureUsed;
	bool m_isKeepInSystemMemory;
	
	int	m_textureID;	
};
