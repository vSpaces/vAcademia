
#pragma once

#include "ifile.h"

class CCachingManager
{
public:
	CCachingManager();
	~CCachingManager();

	std::string GetCacheName(std::string& path, const __int64& modifyTime, const bool isTexture);
	std::string GetCacheName(ifile* const file, const bool isTexture);

private:
	std::string GetTexturePrefix(const bool isTexture);
};