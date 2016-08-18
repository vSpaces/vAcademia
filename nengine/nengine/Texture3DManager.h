
#pragma once

#include "LibTexture3D.h"
#include "Texture3d.h"
#include "Manager.h"

class CTexture3DManager : private CManager<CTexture3D>
{
friend class CGlobalSingletonStorage;

public:
	// return object number or add new object to list if object couldn't find
	int GetObjectNumber(std::string name);

	// bind 3D texture
	void BindTexture3D(int num);

	// load all 3D textures from XML-based list
	void LoadAll();

private:
	CTexture3DManager();
	CTexture3DManager(const CTexture3DManager&);
	CTexture3DManager& operator=(const CTexture3DManager&);
	~CTexture3DManager();
};
