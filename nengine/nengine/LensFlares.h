
#pragma once

#include "CommonEngineHeader.h"

#include "OcclusionQuery.h"
#include "Vector3D.h"
#include <vector>

#define MAX_FLARE_TEXTURE_COUNT		25

typedef struct _SFlare
{
	unsigned int textureID;
	unsigned int width;
	unsigned int height;
	unsigned int width2Scaled;
	unsigned int height2Scaled;
	unsigned int opacity;
	float offset;

	_SFlare()
	{
		textureID = 0xFFFFFFFF;
		width = 0;
		height = 0;
		opacity = 255;
		width2Scaled = 0;
		height2Scaled = 0;
		offset = 0.5f;
	}
} SFlare;

typedef struct _SFlareSource
{
	CVector3D position;
	COcclusionQuery* query;
	int startVisibleTime;
	int startInvisibleTime;
	bool isPointInFrustum;
	bool isVisibleOnLastFrame;

	_SFlareSource()
	{
		query = NULL;
		startVisibleTime = 0;
		startInvisibleTime = 0;
		isPointInFrustum = false;
		isVisibleOnLastFrame = false;		
	}
} SFlareSource;

class CLensFlares
{
public:
	CLensFlares();
	~CLensFlares();

	void LoadIfNeeded();
	void Draw();

private:
	void SetFlare(int flareID, std::string fileName, float offset, float scale, float opacity);
	void SetVisibleFlare(SFlareSource* source, bool isVisible);

	void AddPosition(float x, float y, float z);

	SFlare m_flareTextureID[MAX_FLARE_TEXTURE_COUNT];
	unsigned int m_flareTextureCount;

	bool m_isLoaded;

	MP_VECTOR<SFlareSource *> m_sources;
};