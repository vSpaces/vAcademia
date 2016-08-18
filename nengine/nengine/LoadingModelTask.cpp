
#include "StdAfx.h"
#include "LoadingModelTask.h"

CLoadingModelTask::CLoadingModelTask():
	m_fileResource(NULL),
	m_modelLoader(NULL),
	m_isInCache(false),
	m_isLoaded(false),
	m_model(NULL),
	m_uncompressedSize(0),
	MP_STRING_INIT(m_path),
	MP_STRING_INIT(m_cacheFileName),
	MP_VECTOR_INIT(m_owners),
	MP_VECTOR_INIT(m_materials)
{

}

void CLoadingModelTask::SetParams(CModel* model, std::string& path, ifile* file,
								  std::vector<CMaterial *>& materials, std::vector<void *>* owners)
{
	m_path = path;
	if (file)
	{
		m_fileResource = MP_NEW(CMemoryResource);
		m_fileResource->Init(file);
	}
	m_materials = materials;

	if (owners != NULL)
		m_owners = *owners;
	
	m_model = model;
}

void CLoadingModelTask::SetParams(CModel* model, std::string& path, std::vector<CMaterial *>& materials, std::vector<void *>* owners)
{
	m_path = path;
	m_materials = materials;

	if (owners != NULL)
		m_owners = *owners;

	m_model = model;
}

void CLoadingModelTask::SetFileInfo(void* uncompressedData, unsigned int uncompressedSize)
{
	m_fileResource = MP_NEW(CMemoryResource);

	m_fileResource->SetSize(uncompressedSize);
	m_fileResource->AttachData(uncompressedData);
}

std::string CLoadingModelTask::GetPath()
{
	return m_path;
}

CModel* CLoadingModelTask::GetModel()
{
	return m_model;
}

std::vector<void *>* CLoadingModelTask::GetOwners()
{
	return &m_owners;
}

 std::vector<CMaterial *> CLoadingModelTask::GetMaterials()
{
	return m_materials;
}

void CLoadingModelTask::SetCacheState(bool isInCache, std::string cacheFileName, unsigned int uncompressedSize)
{
	m_isInCache = isInCache;
	m_cacheFileName = cacheFileName;	
	m_uncompressedSize = uncompressedSize;
}

bool CLoadingModelTask::IsLoaded()const
{
	return m_isLoaded;
}

void CLoadingModelTask::Start()
{
	if ((m_path == "") || (!m_fileResource) || (!m_model))
	{
		m_isLoaded = false;
		return;
	}

	m_modelLoader = MP_NEW(CModelLoader);

	m_isLoaded = m_modelLoader->LoadModel(m_path, m_fileResource, m_model);

	if (m_isLoaded)
		m_modelLoader->LoadPhysics(m_path, m_fileResource, m_model);

	MP_DELETE(m_modelLoader);
	m_modelLoader = NULL;
	MP_DELETE(m_fileResource);
	m_fileResource = NULL;
}

void CLoadingModelTask::PrintInfo()
{
	g->lw.WriteLn("CLoadingModelTask ", m_path);
}

void CLoadingModelTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CLoadingModelTask::~CLoadingModelTask()
{
	if (m_fileResource)
	{
		MP_DELETE(m_fileResource);
	}
}