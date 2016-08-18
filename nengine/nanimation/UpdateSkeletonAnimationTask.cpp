
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "UpdateSkeletonAnimationTask.h"

CUpdateSkeletonAnimationTask::CUpdateSkeletonAnimationTask():
	MP_VECTOR_INIT(m_saoList)
{
	OnDone();

	InitializeCriticalSection(&m_cs);
}

void CUpdateSkeletonAnimationTask::PrintInfo()
{
	std::string names;

	std::vector<CSkeletonAnimationObject *>::iterator it = m_saoList.begin();
	std::vector<CSkeletonAnimationObject *>::iterator itEnd = m_saoList.end();

	USES_CONVERSION;
	for ( ; it != itEnd; it++)
	if ((*it)->GetObject3D())
	{
		names += WCharStringToString((*it)->GetObject3D()->GetName());
		names += " ";
	}

	g->lw.WriteLn("UpdateSkeletonAnimationTask ", names);
}

void CUpdateSkeletonAnimationTask::AddAnimation(CSkeletonAnimationObject* sao)
{
	m_saoList.push_back(sao);
}

void CUpdateSkeletonAnimationTask::SetParams(int deltaTime, float distSq)
{
	m_deltaTime = deltaTime;
	m_distSq = distSq;
}

void CUpdateSkeletonAnimationTask::Start()
{
	EnterCriticalSection(&m_cs);

	std::vector<CSkeletonAnimationObject *>::iterator it = m_saoList.begin();
	std::vector<CSkeletonAnimationObject *>::iterator itEnd = m_saoList.end();

	for ( ; it != itEnd; it++)
	if ((*it)->GetObject3D()->GetParent() == NULL)
	{
		(*it)->Update(m_deltaTime, m_distSq);
	}

	it = m_saoList.begin();
	for ( ; it != itEnd; it++)
	if ((*it)->GetObject3D()->GetParent() != NULL)
	{
		(*it)->Update(m_deltaTime, m_distSq);
	}

	m_saoList.clear();

	LeaveCriticalSection(&m_cs);
}

void CUpdateSkeletonAnimationTask::RemoveAnimation(CSkeletonAnimationObject* sao)
{
	EnterCriticalSection(&m_cs);

	std::vector<CSkeletonAnimationObject *>::iterator it = m_saoList.begin();
	std::vector<CSkeletonAnimationObject *>::iterator itEnd = m_saoList.end();

	for ( ; it != itEnd; it++)
	if (*it == sao)
	{
		m_saoList.erase(it);
		break;
	}

	LeaveCriticalSection(&m_cs);
}

void CUpdateSkeletonAnimationTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CUpdateSkeletonAnimationTask::~CUpdateSkeletonAnimationTask()
{
	DeleteCriticalSection(&m_cs);
}