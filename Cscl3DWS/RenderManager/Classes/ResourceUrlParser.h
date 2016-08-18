
#pragma once

#include "../CommonRenderManagerHeader.h"

#include <string>
#include <vector>

#define DEFAULT_SHADOW_POSITION -200, 0, 620
#define DEFAULT_SHADOW_POWER	12.5f
#define DEFAULT_LOD1_DIST		2000
#define DEFAULT_LOD3_DIST		20000
#define MAX_LOD_COUNT			10

class CResourceUrlParser
{
public:
	CResourceUrlParser(std::string& url);
	~CResourceUrlParser();

	unsigned int GetPartCount()const;
	std::string GetSrc(const unsigned int index)const;
	bool GetShadowFlag(const unsigned int index)const;
	unsigned int GetDistance(const unsigned int index)const;

	bool IsUnique()const;
	bool IsLODForDistanceExists(const unsigned int distance)const;

	void Insert(const std::string& src, const unsigned int distance, const bool isShadowEnabled);

private:
	bool m_isUnique;

	MP_VECTOR<MP_STRING> m_src;
	MP_VECTOR<unsigned int> m_distance;
	MP_VECTOR<bool> m_shadowFlag;
};