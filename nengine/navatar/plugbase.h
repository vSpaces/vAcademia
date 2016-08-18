// PlugBase.h: interface for the CPlugBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PlugBase_H__DBEA1B1B_43F0_4FE3_9EDA_311BCC236FB9__INCLUDED_)
#define AFX_PlugBase_H__DBEA1B1B_43F0_4FE3_9EDA_311BCC236FB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommonAvatarHeader.h"

#include <map>
#include "rmIPlugin.h"
#include "rmIPlGetProps.h"

class CPlugBase
{
public:
	CPlugBase();
	virtual ~CPlugBase();

public:
	bool	add_plugin(IScriptCaller* rmpluginresponder, IRMBasePlugin* rmbaseplugin);
	bool	is_registered_already(IScriptCaller*);
	void	destroy_instance(IScriptCaller*);

private:
	MP_MAP<IScriptCaller*, IRMBasePlugin*> m_plugins;
};

#endif // !defined(AFX_PlugBase_H__DBEA1B1B_43F0_4FE3_9EDA_311BCC236FB9__INCLUDED_)
