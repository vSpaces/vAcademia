
#pragma once

#include "CommonHeader.h"
#include "Manager.h"
#include "SkeletonAnimationObject.h"
#include "PreloadBlendsTask.h"

class CBlendsManager : public CManager<CSkeletonAnimationObject>
{
public:
	CBlendsManager();
	~CBlendsManager();

	CSkeletonAnimationObject* LoadBlend(const std::string& path, CSkeletonAnimationObject* sao = NULL);

	void PreloadBlends();
	void AsyncPreloadBlends();

	bool IsBlendsPreloaded();

private:
	void PreloadBlend(const std::string& path);

private:
	CPreloadBlendsTask m_preloadBlendsTask;
};