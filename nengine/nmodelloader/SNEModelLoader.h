
#pragma once

#include "IResourceProcessor.h"
#include "BaseModelLoader.h"
#include "IModelLoader.h"
#include "IModel.h"

class CSNEModelLoader : public CBaseModelLoader, public IModelLoader
{
friend class CMDLModelLoader;

public:
	CSNEModelLoader();
	~CSNEModelLoader();

	bool LoadModel(IResourceProcessor* rp, IModel* model);
	void LoadPhysics(IResourceProcessor* rp, IPhysics* model);
	void FreeAll();

	unsigned int GetCalculateNormalsTime();

private:
	void ReadAdditionalModelInfo(IResourceProcessor* rp, IModel* model);
};