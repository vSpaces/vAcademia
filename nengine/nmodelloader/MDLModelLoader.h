
#pragma once

#include "IResourceProcessor.h"
#include "BaseModelLoader.h"
#include "SNEModelLoader.h"
#include "IModelLoader.h"
#include "IModel.h"

class CMDLModelLoader : public CBaseModelLoader, public IModelLoader
{
public:
	CMDLModelLoader();
	~CMDLModelLoader();

	bool LoadModel(IResourceProcessor* rp, IModel* model);
	void LoadPhysics(IResourceProcessor* rp, IPhysics* model);
	void FreeAll();

	unsigned int GetCalculateNormalsTime();

private:
	CSNEModelLoader m_sneLoader;
};