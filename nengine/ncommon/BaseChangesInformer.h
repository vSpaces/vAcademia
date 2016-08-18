
#pragma once

#include "CommonCommonHeader.h"

#include "IChangesListener.h"
#include <vector>
#include <map>

class CBaseChangesInformer
{
public:
	CBaseChangesInformer();
	~CBaseChangesInformer();

	void AddChangesListener(IChangesListener* changesListener);
	void DeleteChangesListener(IChangesListener* changesListener);

	void ClearListeners();

	void OnAfterChanges(int eventID = 0);

private:
	MP_VECTOR<IChangesListener *> m_listeners;
	MP_MAP<IChangesListener *, int> m_listenersList;
};