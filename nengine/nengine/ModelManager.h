#pragma once

#include "CommonEngineHeader.h"

#include "IPropertyBlockLoadHandler.h"
#include "CachingManager.h"
#include "RotationPack.h"
#include "ModelLoader.h"
#include "Vector3d.h"
#include "Manager.h"
#include "Model.h"
#include "LoadingModelTask.h"

#include "ifile.h"
#include "ireslibrary.h"

#define	MODEL_OPTIMIZATION_NONE							0
#define	MODEL_OPTIMIZATION_VERTEX_CACHE					1
#define	MODEL_OPTIMIZATION_VERTEX_CACHE_AND_OVERDRAW	2

typedef struct _SNeighborPair
{
	MP_STRING sign1, sign2, path1, path2;

	_SNeighborPair();	
	_SNeighborPair(const _SNeighborPair& other);
} SNeighborPair;

typedef struct _SLoadingModel
{
	CModel* model;
	MP_STRING cacheFileName;

	_SLoadingModel();
} SLoadingModel;

typedef struct _SSaveLoadingModelTaskInfo
{
	CModel* model;
	MP_STRING path;
	ifile* file;
	MP_VECTOR<CMaterial *> materials;
	MP_VECTOR<void *> owners;

	_SSaveLoadingModelTaskInfo(CModel* _model, std::string& _path, ifile* _file,
								std::vector<CMaterial *>& _materials, std::vector<void *>* _owners);
} SSaveLoadingModelTaskInfo;

class CMemFileResource;

class CModelManager : public CManager<CModel>, CCachingManager
{
friend class CGlobalSingletonStorage;

public:
	void AlignModel(int nModel, int CenterX, int CenterY, int CenterZ);

	void Update();

	void LoadAll();
	void LoadNeighbors();

	void ShowModelOptimal(int nModel);
	void ShowModelOptimal(int nModel, float dX, float dY, float dZ, CRotationPack* rotation, float scaleX, float scaleY, float scaleZ);
	int ShowModelWithMaterialDetect(int nModel, float d_x, float d_y, float d_z, CRotationPack* rotation, float scaleX, float scaleY, float scaleZ, int mouseX, int mouseY);
	void MakeVBO(int nModel);

	void PrepareOptimalModel(int nModel);
	void OptimizeModel(int nModel, int method);
	void SwitchVBO(int nModel);

	void ReInitialize();
	void RelinkMaterials();
	int LinkTextureToModel(std::string _texturePath, std::string path, bool isNeedExpand, std::vector<void*>* owners, bool canBeCompressed, bool isNormalMap);
	void LinkTexturesToModel(CModel* model, std::string path, bool isNeedExpand = true, std::vector<void*>* owners = NULL);	

	int CloneModel(int num);
	//int LoadFromFile(std::string name, std::string path);
	void LoadFromIFile(CModel* model, ifile* file, std::vector<void *>* owners = NULL);
	int LoadAsynch(std::string path, std::string name);

	int GetSupportedRenderModes(int modelID);

	void UpdateNotFoundTextures(int modelID);

	void SetInvertCCWFlag(const bool isNeedInvert);

	void AddPropertyBlockLoadHandler(const std::string& fileName, IPropertyBlockLoadHandler* const propertyBlockLoadHandler);
	void RemovePropertyBlockLoadHandler(const std::string& fileName);

	CModel* GetCurrentRenderingModel()const;

private:
	CModelManager();
	CModelManager(const CModelManager&);
	CModelManager& operator=(const CModelManager&);
	~CModelManager();

	void LoadModel(CModel* model, std::string& path, CMemFileResource* fileResource, ifile* file,
			std::vector<CMaterial *>& materials, std::vector<void *>* owners);

	unsigned int LoadMaterials(CModel* model, unsigned int& sign, unsigned char* fileData, std::vector<CMaterial *>& materials);

	void GenerateNeighborDataIfNeeded(CModel* model, std::string& path);

	MP_VECTOR<SNeighborPair> m_neighbors;

	bool m_isNeedInvertCCW;
	CModelLoader m_modelLoader;

	MP_MAP<MP_STRING, IPropertyBlockLoadHandler *> m_propertyBlockLoadHandlerList;

	CModel* m_currentRenderingModel;

	int t1, t2;

	MP_VECTOR<CLoadingModelTask*> m_loadingModelTasks;

	MP_VECTOR<CReadingCacheResourceTask *> m_readUncompressedModelsTasks;

	MP_VECTOR<SSaveLoadingModelTaskInfo* > m_loadingModelTaskInfo;
};
