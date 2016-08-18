
#include "StdAfx.h"
#include "LoadingCal3DObjectsTask.h"

CLoadingCal3DObjectsTask::CLoadingCal3DObjectsTask():
	m_data(NULL),
	m_size(0),
	m_info(NULL),
	m_avatarObject(NULL),
	m_isLoaded(false),
	MP_STRING_INIT(m_path),
	MP_STRING_INIT(m_name)	
{
	m_info = MP_NEW(STransitionalInfo);
}

void CLoadingCal3DObjectsTask::SetParams(CAvatarObject* avatarObject, C3DObject* obj3d, const std::string& path, const std::string& name, void* data, unsigned int size)
{
	m_avatarObject = avatarObject;
	m_info->obj = obj3d;
	m_path = path;
	m_name = name;
	if ((size != 0) && (data))
	{
		m_size = size;
		m_data = MP_NEW_ARR(char, m_size);
		memcpy(m_data, data, m_size);	
	}
	else
	{
		m_data = NULL;
		m_size = 0;
	}
}

void CLoadingCal3DObjectsTask::PrintInfo()
{
	g->lw.WriteLn("LoadingCal3DObjectsTask ", m_name, " ", m_path);
}

STransitionalInfo* CLoadingCal3DObjectsTask::GetTransitionalInfo()
{
	return m_info;
}

std::string CLoadingCal3DObjectsTask::GetPath()
{
	return m_path;
}

std::string CLoadingCal3DObjectsTask::GetName()
{
	return m_name;
}

bool CLoadingCal3DObjectsTask::IsLoaded()const
{
	return m_isLoaded;
}

void CLoadingCal3DObjectsTask::Start()
{
	if ( (!m_avatarObject) || (m_path == "") || (!m_info))
	{
		m_isLoaded = false;
		return;
	}

	m_isLoaded = m_avatarObject->LoadAndBindCal3DObjects(m_info, m_path, m_data, m_size);

	FreeData();
}

void CLoadingCal3DObjectsTask::Destroy()
{
	m_info->newSAO->FreeResources();
	m_info->Clear();
	if (m_info->newSAO)
	if (m_info->newSAO->GetID() == 0xFFFFFFFF)
	{
		MP_DELETE(m_info->newSAO);				
	}
	m_info = NULL;

	FreeData();
}

void CLoadingCal3DObjectsTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

void CLoadingCal3DObjectsTask::FreeData()
{
	if (m_data)
	{
		MP_DELETE_ARR(m_data);
		m_data = NULL;
		m_size = 0;
	}
}

CLoadingCal3DObjectsTask::~CLoadingCal3DObjectsTask()
{
	if (m_info)
	{
		MP_DELETE(m_info);
	}

	FreeData();
}