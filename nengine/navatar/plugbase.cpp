
#include "stdafx.h"
#include "PlugBase.h"

extern CRITICAL_SECTION csInit;

CPlugBase::CPlugBase():
	MP_MAP_INIT(m_plugins)
{

}

bool	CPlugBase::add_plugin(IScriptCaller* rmpluginresponder, IRMBasePlugin* rmbaseplugin)
{
	if( is_registered_already(rmpluginresponder))
	{
		return true;
	}

	EnterCriticalSection(&csInit);
	m_plugins.insert(std::map<IScriptCaller*, IRMBasePlugin*>::value_type(rmpluginresponder, rmbaseplugin));
	LeaveCriticalSection(&csInit);

	return true;
}

bool	CPlugBase::is_registered_already(IScriptCaller* rmpluginresponder)
{
	bool ret = false;

	EnterCriticalSection(&csInit);
	std::map<IScriptCaller*, IRMBasePlugin*>::iterator	it;
	if ((it = m_plugins.find(rmpluginresponder)) != m_plugins.end())
	{
		ret = true;
	}
	LeaveCriticalSection(&csInit);

	return ret;
}

void	CPlugBase::destroy_instance(IScriptCaller* rmpluginresponder)
{
	EnterCriticalSection(&csInit);

	std::map<IScriptCaller*, IRMBasePlugin*>::iterator	it;
	if( (it = m_plugins.find(rmpluginresponder)) != m_plugins.end())
	{
		it->second->FreeObject();
		m_plugins.erase(it);
	}

	LeaveCriticalSection(&csInit);
}

CPlugBase::~CPlugBase()
{
}