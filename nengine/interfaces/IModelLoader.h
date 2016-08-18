
#pragma once

#include "IModel.h"
#include "IPhysics.h"
#include "IResourceProcessor.h"
#include "TimeProcessor.h"

class IModelLoader
{
public:
	// Load model from data stream defined by IResourceProcessor and store 3d model data via IModel
	virtual bool LoadModel(IResourceProcessor* rp, IModel* model) = 0;
	virtual void LoadPhysics(IResourceProcessor* rp, IPhysics* model) = 0;
	// Free all resources
	virtual void FreeAll() = 0;

	virtual unsigned int GetCalculateNormalsTime() = 0;
};

extern CTimeProcessor* g_tp;