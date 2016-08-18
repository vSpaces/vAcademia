
#pragma once

#include "Manager.h"
#include "CubeMap.h"

class CCubeMapManager : public CManager<CCubeMap>
{
public:
	CCubeMapManager();
	~CCubeMapManager();

	void LoadAll();

	void AddCubeMap(std::string name, int textureID);
	void AddCubeMap(std::wstring name, int textureID);
};