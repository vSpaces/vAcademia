
#include "StdAfx.h"
#include "LoadingSAOTask.h"

CLoadingSAOTask::CLoadingSAOTask():
	m_info(NULL),		
	m_isLoaded(false),
	MP_STRING_INIT(m_path)
{
		m_info = MP_NEW(STransitionalInfo);//new STransitionalInfo();
}

void CLoadingSAOTask::SetParams(CSkeletonAnimationObject* sao, const std::string& path, const bool isBlend, CSkeletonAnimationObject* const parent)
{
	m_sao = sao;
	m_path = path;
	m_isBlend = isBlend;
	m_parent = parent;
}

void CLoadingSAOTask::PrintInfo()
{
	g->lw.WriteLn("CLoadingSAOTask ", m_path);
}

STransitionalInfo* CLoadingSAOTask::GetTransitionalInfo()
{
	return m_info;
}

std::string CLoadingSAOTask::GetPath()
{
	return m_path;
}

bool CLoadingSAOTask::IsLoaded()const
{
	return m_isLoaded;
}

void CLoadingSAOTask::Start()
{
	if ( (!m_sao) || (m_path == "") || (!m_info))
	{
		m_isLoaded = false;
		return;
	}

	m_isLoaded = m_sao->LoadCal3DObjects(m_path, m_info, m_isBlend, m_parent);
}

CSkeletonAnimationObject* CLoadingSAOTask::GetParent()
{
	return m_parent;
}

CSkeletonAnimationObject* CLoadingSAOTask::GetSAO()
{
	return m_sao;
}

void CLoadingSAOTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CLoadingSAOTask::~CLoadingSAOTask()
{
	MP_DELETE(m_info);//delete m_info;
}