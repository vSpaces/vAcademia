#include "stdafx.h"
#include "objstorage.h"

CObjStorage* CObjStorage::GetInstance()
{
	static CObjStorage* obj;

	if (!obj)
	{
		obj = MP_NEW( CObjStorage);
	}

	return obj;
}

CObjStorage::CObjStorage()
{
	m_pResourceManager = NULL;
	m_pCommunicationManager = NULL;
	m_lw = NULL;
}

CObjStorage::CObjStorage(const CObjStorage&)
{

}

CObjStorage::~CObjStorage()
{

}

void CObjStorage::SetResMan(res::resIResourceManager* resourceManager)
{
	m_pResourceManager = resourceManager;
}

void CObjStorage::SetComMan(cm::cmICommunicationManager* communicationManager)
{
	m_pCommunicationManager = communicationManager;
}

void CObjStorage::SetILogWriter(ILogWriter* alw)
{
	m_lw = alw;
}

ILogWriter* CObjStorage::GetILogWriter()
{
	return m_lw;
}

res::resIResourceManager* CObjStorage::GetResMan()
{
	return m_pResourceManager;
}

cm::cmICommunicationManager* CObjStorage::GetComMan()
{
	return m_pCommunicationManager;
}