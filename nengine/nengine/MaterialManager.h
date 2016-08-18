#pragma once

#include "Manager.h"
#include "Texture.h"
#include "Material.h"

class CMaterialManager : public CManager<CMaterial>
{
public:
	CMaterialManager();
	~CMaterialManager();

	void LoadAll();

	CMaterial* LoadMaterial(std::string fileName);
	CMaterial* LoadMaterial(std::string fileName, void* data);
	CMaterial* LoadUniqueMaterialFromIFile(std::string name, ifile* file, bool isManageTextures);
	CMaterial* LoadUniqueMaterialFromMemory(std::string name, std::string path, void* data, bool isManageTextures); 

	void ReloadMaterial(unsigned int materialID);

	void ClearShaderLocations();

	void BindMaterial(int materialID);
	void UnbindMaterial(int materialID);

	bool IsMaterialCorrespondsForRenderingMode(CTexture* texture, int materialID);

	void SetDepthTextureID(int depthTextureID, int depthTextureID2);
	void BindDepthTexture(unsigned char level);

	void BindDepthLevel();
	void UnbindDepthLevel();

	int CloneMaterial(int materialID, std::string& newMaterialName);

	void OnMaterialQualityChanged(); 

	unsigned int GetStateID(const std::string& name)const;

private:
	int m_depthTextureID;
	int m_depthTextureID2;

	unsigned char m_depthLevel;

	CMaterialStatesEnumerator m_materialStatesEnumerator;
};