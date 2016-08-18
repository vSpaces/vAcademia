
#include "StdAfx.h"
#include "Statistics.h"
#include "HelperFunctions.h"

int stat_allObjectCount;
int stat_updatedObjectCount;
int stat_drawedObjectCount;
int stat_polygonCount;
int stat_transparentPolygonCount;
int stat_solidPolygonCount;
int stat_alphatestPolygonCount;
int stat_shadowObjectsCount;
int stat_2dAllCount;
int stat_2dVisibleSpritesCount;
int stat_2dVisibleTextCount;
int stat_drawCount;
int stat_physicsObjectCount;
int stat_dynamicTextureCount;
int stat_textureCount;
int stat_texturesSize;
int stat_motionCount;
int stat_materialChangeCount;

// loading stats
int stat_allModelsTime;
int stat_allModelsZlibTime;
int stat_allModelsNormalsTime;
int stat_allMotionsTime;
int stat_allCalTime;
int stat_normalObjectsTime;
int stat_scriptedObjectsTime;
int stat_avatarObjectsTime;
int stat_blendsPreloadTime;
int stat_openGLInitTime;
int stat_resourcesPreloadTime;
int stat_shaderCompileTime;
int stat_cacheTime;

CStatistics::CStatistics():
	m_clothesQueueTime(0),
	m_compositeTexturesQueueTime(0),
	m_saoQueueTime(0)
{
	stat_textureCount = 0;

	// loading stats
	stat_allModelsTime = 0;
	stat_allModelsZlibTime = 0;
	stat_allModelsNormalsTime = 0;
	stat_allMotionsTime = 0;
	stat_allCalTime = 0;
	stat_normalObjectsTime = 0;
	stat_scriptedObjectsTime = 0;
	stat_avatarObjectsTime = 0;
	stat_blendsPreloadTime = 0;
	stat_openGLInitTime = 0;
	stat_resourcesPreloadTime = 0;
	stat_shaderCompileTime = 0;
	stat_cacheTime = 0;	
}

void CStatistics::OnNewFrame()
{
	stat_allObjectCount = 0;
	stat_updatedObjectCount = 0;
	stat_drawedObjectCount = 0;
	stat_polygonCount = 0;
	stat_transparentPolygonCount = 0;
	stat_solidPolygonCount = 0;
	stat_alphatestPolygonCount = 0;
	stat_drawCount = 0;
	stat_materialChangeCount = 0;
	stat_2dVisibleTextCount = 0;
	stat_2dVisibleSpritesCount = 0;
	m_compositeTexturesQueueTime -= g->rs.GetInt(MAX_COMPOSITE_TEXTURES_FRAME_TIME);
	if (m_compositeTexturesQueueTime < 0)
	{
		m_compositeTexturesQueueTime = 0;
	}
	m_clothesQueueTime -= g->rs.GetInt(MAX_CLOTHES_LOADING_FRAME_TIME);
	if (m_clothesQueueTime < 0)
	{
		m_clothesQueueTime = 0;
	}
	m_saoQueueTime -= g->rs.GetInt(MAX_SAO_LOADING_FRAME_TIME);
	if (m_saoQueueTime < 0)
	{
		m_saoQueueTime = 0;
	}
}

std::string CStatistics::GetStatString()
{
	std::string res = "Всего объектов: ";
	res += IntToStr(stat_allObjectCount);
	res += "\n";
	res += "Апдейтилось объектов: ";
	res += IntToStr(stat_updatedObjectCount);
	res += "\n";
	res += "Объектов видимых в кадре: ";
	res += IntToStr(stat_drawedObjectCount);
	res += "\n";
	res += "Количество вызовов на отрисовку 3д: ";
	res += IntToStr(stat_drawCount);
	res += "\n";
	res += "Полигонов объектов: ";
	res += IntToStr(stat_polygonCount);
	res += "\n";
	res += "Объектов с тенью: ";
	res += IntToStr(stat_shadowObjectsCount);
	res += "\n";
	res += "Непрозрачных полигонов: ";
	res += IntToStr(stat_solidPolygonCount);
	res += "\n";
	res += "Полигонов с прозрачностью: ";
	res += IntToStr(stat_transparentPolygonCount);
	res += "\n";
	res += "Полигонов с альфа-тестом: ";
	res += IntToStr(stat_alphatestPolygonCount);
	res += "\n";
	res += "Кол-во 2д элементов: ";
	res += IntToStr(stat_2dAllCount);
	res += "\n";
	res += "Кол-во видимых 2д спрайтов: ";
	res += IntToStr(stat_2dVisibleSpritesCount);
	res += "\n";
	res += "Кол-во видимых фрагментов текста: ";
	res += IntToStr(stat_2dVisibleTextCount);
	res += "\n";
	res += "Кол-во текстур: ";
	res += IntToStr(stat_textureCount);
	res += "\n";
	res += "Кол-во динамических текстур: ";
	res += IntToStr(stat_dynamicTextureCount);
	res += "\n";
	res += "Общий размер текстур: ";
	res += IntToStr(stat_texturesSize);
	res += "\n";
	res += "Кол-во физических объектов: ";
	res += IntToStr(stat_physicsObjectCount);
	res += "\n";
	res += "Кол-во движений объектов: ";
	res += IntToStr(stat_motionCount);
	res += "\n";
	res += "Кол-во реальных смен материала: ";
	res += IntToStr(stat_materialChangeCount);
	res += "\n";

	return res;
}

std::string CStatistics::GetLoadStatString()
{
	std::string res = "MA = ";
	res += IntToStr(stat_allModelsTime);
	res += "; MZ = ";
	res += IntToStr(stat_allModelsZlibTime);
	res += "; MN = ";
	res += IntToStr(stat_allModelsNormalsTime);
	res += "; MTA = ";
	res += IntToStr(stat_allMotionsTime); 
	res += "; CA = ";
	res += IntToStr(stat_allCalTime);
	res += "; NOA = ";
	res += IntToStr(stat_normalObjectsTime);
	res += "; SOA = ";
	res += IntToStr(stat_scriptedObjectsTime);
	res += "; AOA = ";
	res += IntToStr(stat_avatarObjectsTime);
	res += "; BPL = ";
	res += IntToStr(stat_blendsPreloadTime);
	res += "; OGLI = ";
	res += IntToStr(stat_openGLInitTime);
	res += "; RPL = ";
	res += IntToStr(stat_resourcesPreloadTime);
	res += "; SC = ";
	res += IntToStr(stat_shaderCompileTime);
	res += "; CACHE = ";
	res += IntToStr(stat_cacheTime);
		
	return res;
}

int CStatistics::GetClothesQueueTime()const
{
	return m_clothesQueueTime;
}

void CStatistics::AddClothesQueueTime(int delta)
{
	m_clothesQueueTime += delta;
}

int CStatistics::GetCompositeTexturesQueueTime()const
{
	return m_compositeTexturesQueueTime;
}

void CStatistics::AddCompositeTexturesQueueTime(int delta)
{
	m_compositeTexturesQueueTime += delta;
}

int CStatistics::GetSAOQueueTime()const
{
	return m_saoQueueTime;
}

void CStatistics::AddSAOQueueTime(int delta)
{
	m_saoQueueTime += delta;
}

CStatistics::~CStatistics()
{

}
