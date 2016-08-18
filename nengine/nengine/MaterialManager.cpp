
#include "StdAfx.h"
#include "XMLList.h"
#include "Renderer.h"
#include "MaterialManager.h"
#include "PlatformDependent.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"

CMaterialManager::CMaterialManager():
	m_depthLevel(7),
	m_depthTextureID(-1),
	m_depthTextureID2(-1)
{
	ReserveMemory(5000);
}

void CMaterialManager::LoadAll()
{
	CXMLList matList(GetApplicationRootDirectory() + L"materials.xml", true);

	std::string materialFileName;
	while (matList.GetNextValue(materialFileName))
	{
		LoadMaterial("external\\materials\\" + materialFileName);
	}
}

CMaterial* CMaterialManager::LoadMaterial(std::string fileName, void* data)
{
	int materialID = AddObject(fileName);
	CMaterial* material = GetObjectPointer(materialID);
	material->SetID(materialID);
	material->SetSource(SOURCE_FILE, fileName);
	material->LoadFromXML(fileName, (unsigned char *)data, false);

	return material;
}

CMaterial* CMaterialManager::LoadMaterial(std::string fileName)
{
	int materialID = AddObject(fileName);
	CMaterial* material = GetObjectPointer(materialID);
	material->SetID(materialID);
	material->SetSource(SOURCE_FILE, fileName);
	USES_CONVERSION;
	material->LoadFromXML(GetApplicationRootDirectory() + A2W( fileName.c_str()), false);

	return material;
}

void CMaterialManager::ReloadMaterial(unsigned int materialID)
{
	CMaterial* material = g->matm.GetObjectPointer(materialID);
	material->Clear();

	int sourceID;
	std::string sourcePath;
	material->GetSource(sourceID, sourcePath);

	if (sourceID == SOURCE_IFILE)
	{
		ifile* fl = g->rl->GetResMemFile(sourcePath.c_str());
		material->LoadFromXML(fl, false);
	}
}

CMaterial* CMaterialManager::LoadUniqueMaterialFromIFile(std::string name, ifile* file, bool isManageTextures)
{
	int materialID = AddObject(name);	
	CMaterial* material = GetObjectPointer(materialID);
	material->SetID(materialID);
	material->LoadFromXML(file, isManageTextures);
	USES_CONVERSION;
	material->SetSource(SOURCE_IFILE, W2A(file->get_file_path()));

	return material;
}

CMaterial* CMaterialManager::LoadUniqueMaterialFromMemory(std::string name, std::string path, void* data, bool isManageTextures)
{
	int materialID = AddObject(name);	
	CMaterial* material = GetObjectPointer(materialID);
	material->SetID(materialID);
	material->LoadFromXML(path, (unsigned char *)data, isManageTextures);
	material->SetSource(SOURCE_FILE, path);

	return material;
}

void CMaterialManager::BindMaterial(int materialID)
{
	CMaterial* material = GetObjectPointer(materialID);

	material->BindMaterial();
}

void CMaterialManager::UnbindMaterial(int materialID)
{
	CMaterial* material = GetObjectPointer(materialID);

	material->UnbindMaterial();
}

void CMaterialManager::SetDepthTextureID(int depthTextureID, int depthTextureID2)
{
	m_depthTextureID = depthTextureID;
	m_depthTextureID2 = depthTextureID2;
}

void CMaterialManager::BindDepthTexture(unsigned char level)
{
	m_depthLevel = level;
	g->stp.SetActiveTexture(level);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_depthTextureID);
	GLFUNC(glDisable)(GL_TEXTURE_2D);
	g->stp.SetActiveTexture(level - 1);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_depthTextureID2);
	GLFUNC(glDisable)(GL_TEXTURE_2D);
}

void CMaterialManager::BindDepthLevel()
{
	g->stp.SetActiveTexture(m_depthLevel);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
	g->stp.SetActiveTexture(m_depthLevel - 1);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
}

void CMaterialManager::UnbindDepthLevel()
{
	g->stp.SetActiveTexture(m_depthLevel);
	GLFUNC(glDisable)(GL_TEXTURE_2D);
	g->stp.SetActiveTexture(m_depthLevel-1);
	GLFUNC(glDisable)(GL_TEXTURE_2D);
}

bool CMaterialManager::IsMaterialCorrespondsForRenderingMode(CTexture* texture, int materialID)
{
	int mode = g->mr.GetMode();
	if (mode == MODE_RENDER_ALL)
	{
		return true;
	}

	CMaterial* material = g->matm.GetObjectPointer(materialID);
	int materialMode = material->GetCorrespondingRenderingMode();
	if (materialMode != MODE_RENDER_UNKNOWN)
	{
#pragma warning(push)
#pragma warning(disable : 4800)
		return (mode & materialMode);
#pragma warning(pop)
	}
	else
	{
		bool isAlphaTest = false;
		if (material->GetState(STATE_ALPHATEST))
		{
			isAlphaTest = *(unsigned int*)material->GetState(STATE_ALPHATEST) > 0;
		}

		bool isTransparent = ((texture->IsTransparent()) && (!isAlphaTest));
		if ( ((MODE_RENDER_TRANSPARENT & mode) && (isTransparent))
			|| ((MODE_RENDER_SOLID & mode) && (!isTransparent)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

int CMaterialManager::CloneMaterial(int materialID, std::string& newMaterialName)
{
	CMaterial* material = GetObjectPointer(materialID);

	int newMaterialID = g->matm.AddObject(newMaterialName);
	CMaterial* newMaterial = g->matm.GetObjectPointer(newMaterialID);
	newMaterial->Clone(material);

	return newMaterialID;
}

void CMaterialManager::ClearShaderLocations()
{
	std::vector<CMaterial *>::iterator it = GetLiveObjectsBegin();
	std::vector<CMaterial *>::iterator itEnd = GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		(*it)->ClearShaderLocations();
	}
}

void CMaterialManager::OnMaterialQualityChanged()
{
	std::vector<CMaterial *>::iterator it = GetLiveObjectsBegin();
	std::vector<CMaterial *>::iterator itEnd = GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		if ((*it)->GetQualityParam()!= "")
		{
			(*it)->OnMaterialQualityChanged();
		}
	}
}

unsigned int CMaterialManager::GetStateID(const std::string& name)const
{
	return m_materialStatesEnumerator.GetStateID(name);
}

CMaterialManager::~CMaterialManager()
{

}