
#pragma once

#include <string>

extern int stat_allObjectCount;
extern int stat_updatedObjectCount;
extern int stat_drawedObjectCount;
extern int stat_polygonCount;
extern int stat_transparentPolygonCount;
extern int stat_solidPolygonCount;
extern int stat_alphatestPolygonCount;
extern int stat_shadowObjectsCount;
extern int stat_2dAllCount;
extern int stat_2dVisibleSpritesCount;
extern int stat_2dVisibleTextCount;
extern int stat_drawCount;
extern int stat_physicsObjectCount;
extern int stat_dynamicTextureCount;
extern int stat_textureCount;
extern int stat_texturesSize;
extern int stat_motionCount;
extern int stat_materialChangeCount;

// loading stats
extern int stat_allModelsTime;
extern int stat_allModelsZlibTime;
extern int stat_allModelsNormalsTime;
extern int stat_allMotionsTime;
extern int stat_allCalTime;
extern int stat_normalObjectsTime;
extern int stat_scriptedObjectsTime;
extern int stat_avatarObjectsTime;
extern int stat_blendsPreloadTime;
extern int stat_openGLInitTime;
extern int stat_resourcesPreloadTime;
extern int stat_shaderCompileTime;
extern int stat_cacheTime;

class CStatistics
{
public:
	CStatistics();
	~CStatistics();

	void OnNewFrame();
	std::string GetStatString();

	std::string GetLoadStatString();

	int GetClothesQueueTime()const;
	void AddClothesQueueTime(int delta);

	int GetCompositeTexturesQueueTime()const;
	void AddCompositeTexturesQueueTime(int delta);

	int GetSAOQueueTime()const;
	void AddSAOQueueTime(int delta);

private:
	int m_clothesQueueTime;
	int m_compositeTexturesQueueTime;
	int m_saoQueueTime;
};