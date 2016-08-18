
#pragma once

#include "CommonModelLoaderHeader.h"

#include "IModelLoader.h"
#include <vector>
#include <map>

#define MODEL_FORMAT_SMD				0
#define MODEL_FORMAT_SNE				1
#define MODEL_FORMAT_BMF				2

#define STANDARD_MODEL_FORMAT_COUNT		3

class CModelLoader
{
public:
	CModelLoader();
	~CModelLoader();

	bool LoadModel(std::string& path, IResourceProcessor* rp, IModel* model);
	bool LoadModel(unsigned int modelTypeID, IResourceProcessor* rp, IModel* model);

	void LoadPhysics(std::string& path, IResourceProcessor* rp, IPhysics* model);
	void LoadPhysics(unsigned int modelTypeID, IResourceProcessor* rp, IPhysics* model);

	unsigned int RegisterModelType(std::string extension, IModelLoader* modelLoader);
	void UnregisterModelType(unsigned int modelTypeID);

	unsigned int GetCalculateNormalsTime()const;

private:
	void InitStandardModelLoaders();

	MP_MAP<MP_STRING, IModelLoader*> m_modelLoadersList;
	MP_VECTOR<IModelLoader*> m_modelLoaders;
};