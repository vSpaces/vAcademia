
#include "StdAfx.h"
#include "UpdateDynamicTexturesTask.h"
#include "cal3d/memory_leak.h"

CUpdateDynamicTexturesTask::CUpdateDynamicTexturesTask():
	MP_VECTOR_INIT(m_dynamicTextures)
{
	OnDone();
}

void CUpdateDynamicTexturesTask::AddDynamicTexture(IDynamicTexture* dynamicTexture)
{
	m_dynamicTextures.push_back(dynamicTexture);
}

void CUpdateDynamicTexturesTask::Start()
{
	std::vector<IDynamicTexture *>::iterator it = m_dynamicTextures.begin();
	std::vector<IDynamicTexture *>::iterator itEnd = m_dynamicTextures.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->UpdateFrame();
	}

	m_dynamicTextures.clear();
}

void CUpdateDynamicTexturesTask::PrintInfo()
{
	g->lw.WriteLn("UpdateDynamicTexturesTask");
}

void CUpdateDynamicTexturesTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CUpdateDynamicTexturesTask::~CUpdateDynamicTexturesTask()
{
}