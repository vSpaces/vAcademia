
#include "StdAfx.h"
#include <Assert.h>
#include "ModelLoader.h"
#include "MDLModelLoader.h"
#include "SNEModelLoader.h"
#include "BMFModelLoader.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"
#include "../../cal3d/memory_leak.h"

CModelLoader::CModelLoader():
	MP_MAP_INIT(m_modelLoadersList),
	MP_VECTOR_INIT(m_modelLoaders)
{
	InitStandardModelLoaders();
}

bool CModelLoader::LoadModel(std::string& path, IResourceProcessor* rp, IModel* model)
{
	std::string ext = path.substr(path.size() - 3, 3);
	ext = StringToUpper(ext);

	MP_MAP<MP_STRING, IModelLoader *>::iterator it = m_modelLoadersList.find(MAKE_MP_STRING(ext));
	if (it != m_modelLoadersList.end())
	{
		model->Lock();		
		bool res = (*it).second->LoadModel(rp, (IModel*)model);
		model->Unlock();
		return res;
	}
	else
	{
		assert(true);
		return false;
	}
}

bool CModelLoader::LoadModel(unsigned int modelTypeID, IResourceProcessor* rp, IModel* model)
{
	assert(modelTypeID >= 0);
	assert(modelTypeID < m_modelLoaders.size());

	if ((modelTypeID >= 0) && (modelTypeID < m_modelLoaders.size()))
	{
		return m_modelLoaders[modelTypeID]->LoadModel(rp, (IModel*)model);
	}
	else
	{
		return false;
	}
}

void CModelLoader::LoadPhysics(std::string& path, IResourceProcessor* rp, IPhysics* model)
{
	std::string ext = path.substr(path.size() - 3, 3);
	ext = StringToUpper(ext);

	MP_MAP<MP_STRING, IModelLoader *>::iterator it = m_modelLoadersList.find(MAKE_MP_STRING(ext));
	if (it != m_modelLoadersList.end())
	{
		(*it).second->LoadPhysics(rp, model);
	}
}

void CModelLoader::LoadPhysics(unsigned int modelTypeID, IResourceProcessor* rp, IPhysics* model)
{
	assert(modelTypeID >= 0);
	assert(modelTypeID < m_modelLoaders.size());

	if ((modelTypeID >= 0) && (modelTypeID < m_modelLoaders.size()))
	{
		m_modelLoaders[modelTypeID]->LoadPhysics(rp, model);
	}
}

unsigned int CModelLoader::RegisterModelType(std::string extension, IModelLoader* modelLoader)
{
	m_modelLoaders.push_back(modelLoader);
	m_modelLoadersList.insert(MP_MAP<MP_STRING, IModelLoader *>::value_type(MAKE_MP_STRING(StringToUpper(extension)), modelLoader));

	return (m_modelLoaders.size() - 1);
}

void CModelLoader::UnregisterModelType(unsigned int modelTypeID)
{
	assert(modelTypeID >= 0);
	assert(modelTypeID < m_modelLoaders.size());

	if ((modelTypeID >= 0) && (modelTypeID < m_modelLoaders.size()))
	{
		MP_MAP<MP_STRING, IModelLoader *>::iterator it = m_modelLoadersList.begin();
		MP_MAP<MP_STRING, IModelLoader *>::iterator itEnd = m_modelLoadersList.end();

		IModelLoader* deletedLoader = m_modelLoaders[modelTypeID];
		m_modelLoaders[modelTypeID] = NULL;

		for ( ; it != itEnd; it++)
		if ((*it).second == deletedLoader)
		{
			m_modelLoadersList.erase(it);
			break;
		}
	}
}

void CModelLoader::InitStandardModelLoaders()
{
	// The order of model format registration is chosen according to format IDs in defines
	//RegisterModelType("SNE", new CSNEModelLoader());
	//RegisterModelType("MDL", new CMDLModelLoader());
	CBMFModelLoader* ldr = MP_NEW(CBMFModelLoader);
	RegisterModelType("BMF", ldr);
}

unsigned int CModelLoader::GetCalculateNormalsTime()const
{
	return m_modelLoaders[0]->GetCalculateNormalsTime();
}

CModelLoader::~CModelLoader()
{
	std::vector<IModelLoader *>::iterator it = m_modelLoaders.begin();
	std::vector<IModelLoader *>::iterator itEnd = m_modelLoaders.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->FreeAll();
	}
}