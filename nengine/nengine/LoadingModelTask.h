
#pragma once

#include "CommonEngineHeader.h"

#include <vector>
#include "ITask.h"
#include "ModelLoader.h"
#include "Model.h"
#include "Material.h"
#include "MemoryResource.h"

class CLoadingModelTask : public ITask
{
public:
	CLoadingModelTask();
	~CLoadingModelTask();

	void SetParams(CModel* model, std::string& path, ifile* file,
		std::vector<CMaterial *>& materials, std::vector<void *>* owners);

	void SetParams(CModel* model, std::string& path, std::vector<CMaterial *>& materials, std::vector<void *>* owners);

	void SetFileInfo(void* uncompressedData, unsigned int uncompressedSize);

	std::string GetPath();
	CModel* GetModel();
	std::vector<void *>* GetOwners();
	std::vector<CMaterial *> GetMaterials();
	
	bool IsLoaded()const;

	void PrintInfo();

	void Start();

	void SetCacheState(bool isInCache, std::string cacheFileName, unsigned int size);

	void Destroy() { assert(false); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	CModelLoader* m_modelLoader;

	MP_STRING m_path;
	MP_STRING m_cacheFileName;

	CMemoryResource* m_fileResource;

	bool m_isLoaded;
	bool m_isInCache;

	CModel* m_model;

	MP_VECTOR<void *> m_owners;
	MP_VECTOR<CMaterial *> m_materials;

	unsigned int m_uncompressedSize;
};