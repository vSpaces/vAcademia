#include "StdAfx.h"
#include "..\include\SelectorFactory.h"
#include "..\include\CSHelper.h"

SelectorFactory::SelectorFactory(void) : MP_VECTOR_INIT(m_deletingSelectors)
{
	m_deletingSelectors.clear();
	csFactory.Init();
}

SelectorFactory::~SelectorFactory(void)
{
	// будет ожидать завершения потока инициализации на деструкторе
	for (unsigned int i =0; i < m_deletingSelectors.size(); i++)
	{
		SAFE_DELETE(m_deletingSelectors[i]);
	}
	m_deletingSelectors.clear();
	csFactory.Term();
}

void SelectorFactory::AddSelector(IDesktopSelector* aSelector)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	m_deletingSelectors.push_back(aSelector);
}

void SelectorFactory::DestroyDeletedSelectors()
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	std::vector<IDesktopSelector*>		liveSelector;

	for (unsigned int i =0; i < m_deletingSelectors.size(); i++)
	{
		if (m_deletingSelectors[i] != NULL)
		{
			if (!m_deletingSelectors[i]->IsLive())
			{
				SAFE_DELETE(m_deletingSelectors[i]);
			}
			else
				liveSelector.push_back(m_deletingSelectors[i]);
		}
	}
	m_deletingSelectors.clear();
	m_deletingSelectors = liveSelector;
}