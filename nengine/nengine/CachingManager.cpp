
#include "StdAfx.h"
#include "CachingManager.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"

CCachingManager::CCachingManager()
{
}

std::string CCachingManager::GetTexturePrefix(const bool isTexture)
{
	if (!isTexture)
	{
		return "";
	}

	return "tsd" + IntToStr(g->rs.GetInt(TEXTURE_SIZE_DIV)) + "_";
}

std::string CCachingManager::GetCacheName(std::string& path, const __int64& modifyTime, const bool isTexture)
{
	std::string cacheFileName = GetTexturePrefix(isTexture);
	USES_CONVERSION;
	cacheFileName += path.c_str();
	StringReplace(cacheFileName, "\\", "_");
	StringReplace(cacheFileName, "/", "_");
	StringReplace(cacheFileName, ":", "_");

	unsigned int time1 = *(unsigned int *)&modifyTime;
	unsigned int time2 = *(((unsigned int *)&modifyTime) + 1);

	cacheFileName += "_";
	cacheFileName += IntToStr(time1);
	cacheFileName += "__";
	cacheFileName += IntToStr(time2);

	return cacheFileName;
}

std::string CCachingManager::GetCacheName(ifile* const file, const bool isTexture)
{
	USES_CONVERSION;
	std::string path = W2A(file->get_file_path());
	
	__int64 modifyTime = file->get_modify_time();
	
	return GetCacheName(path, modifyTime, isTexture);
}

CCachingManager::~CCachingManager()
{
}