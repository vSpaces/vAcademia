
#include "StdAfx.h"
#include "BaseChangesInformer.h"
#include "cal3d/memory_leak.h"

CBaseChangesInformer::CBaseChangesInformer():
	MP_VECTOR_INIT(m_listeners),
	MP_MAP_INIT(m_listenersList)
{
}

void CBaseChangesInformer::AddChangesListener(IChangesListener* changesListener)
{
	if (m_listenersList.find(changesListener) != m_listenersList.end())
	{
		return;
	}

	m_listeners.push_back(changesListener);
	m_listenersList.insert(std::map<IChangesListener *, int>::value_type(changesListener, 1));
}

void CBaseChangesInformer::DeleteChangesListener(IChangesListener* changesListener)
{
	std::map<IChangesListener *, int>::iterator it = m_listenersList.find(changesListener);
	
	if (it != m_listenersList.end())
	{
		m_listenersList.erase(it);
	}

	std::vector<IChangesListener *>::iterator iter = m_listeners.begin();
	std::vector<IChangesListener *>::iterator iterEnd = m_listeners.end();

	for ( ; iter != iterEnd; iter++)
	if ((*iter) == changesListener)
	{
		m_listeners.erase(iter);
		break;
	}
}

void CBaseChangesInformer::OnAfterChanges(int eventID)
{
	std::vector<IChangesListener *>::iterator iter = m_listeners.begin();
	std::vector<IChangesListener *>::iterator iterEnd = m_listeners.end();

	for ( ; iter != iterEnd; iter++)
	{
		(*iter)->OnChanged(eventID);
	}
}

void CBaseChangesInformer::ClearListeners()
{
	m_listeners.clear();
	m_listenersList.clear();
}

CBaseChangesInformer::~CBaseChangesInformer()
{
}