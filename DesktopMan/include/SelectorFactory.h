#pragma once
#include "selectors/IDesktopSelector.h"
#include "SharingContext.h"
#include <vector>

class SelectorFactory
{
public:
	SelectorFactory(void);
	~SelectorFactory(void);

	void AddSelector(IDesktopSelector* aSelector);
	void DestroyDeletedSelectors();

private:
	MP_VECTOR<IDesktopSelector*> m_deletingSelectors;
	CCriticalSection	csFactory;
};
