// Chem3DBase.cpp: implementation of the CAnyPluginBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Base.h"

CAnyPluginBase::CAnyPluginBase():
	m_propsContainer(NULL),
	m_scriptCaller(NULL)
{
}

bool	CAnyPluginBase::bRead(LPCTSTR param)
{
	if (!m_propsContainer)	
	{
		return false;
	}

	return m_propsContainer->IsSet(param);
}

double	CAnyPluginBase::dGet(LPCTSTR param)
{
	if (!bRead(param))	
	{
		return 0.0f;
	}

	return m_propsContainer->GetDoubleProp(param);
}

IAnyDataContainer*	CAnyPluginBase::rGet(LPCTSTR param)
{
	if (!bRead(param))	
	{
		return NULL;
	}

	return m_propsContainer->GetRefProp(param);
}

void	CAnyPluginBase::dSet(LPCTSTR param, double val)
{
	if (!bRead(param))	
	{
		return;
	}

	m_propsContainer->SetProp(param, val);
}

void	CAnyPluginBase::sSet(LPCTSTR param, LPCTSTR val)
{
	if (!bRead(param))	
	{
		return;
	}
	
	m_propsContainer->SetProp(param, val);
}

void	CAnyPluginBase::sSet(LPCTSTR param, LPCWSTR val)
{
	if (!bRead(param))	
	{
		return;
	}

	m_propsContainer->SetProp(param, val);
}

wchar_t*	CAnyPluginBase::sGet(LPCTSTR param)
{
	if(!bRead(param))	
	{
		return NULL;
	}

	return m_propsContainer->GetStringProp(param);
}

void	CAnyPluginBase::GetInfo(CLEFPLUGININFO* /*apinfo*/)
{
}

void	CAnyPluginBase::SetResponder(IScriptCaller* rmpluginresponder)
{
	m_scriptCaller = rmpluginresponder;
}

void	CAnyPluginBase::SetPropsContainer(IPropertiesContainer* propsContainer)
{
	m_propsContainer = propsContainer;
	OnSetPropsContainer();
}

unsigned long	CAnyPluginBase::ProcessMessage(int /*aiArgC*/, va_list /*aArgsV*/)
{
	return 0;
}

CAnyPluginBase::~CAnyPluginBase()
{
}