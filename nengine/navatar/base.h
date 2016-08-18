// Chem3DBase.h: interface for the CChem3DBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Chem3DBase_H__DBEA1B1B_43F0_4FE3_9EDA_311BCC236FB9__INCLUDED_)
#define AFX_Chem3DBase_H__DBEA1B1B_43F0_4FE3_9EDA_311BCC236FB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rmIPlugin.h"
#include "rmIPlGetProps.h"

class CAnyPluginBase : public IRMBasePlugin
{
public:
	CAnyPluginBase();
	virtual ~CAnyPluginBase();

public:
	bool				bRead(LPCTSTR param);
	double				dGet(LPCTSTR param);
	IAnyDataContainer*	rGet(LPCTSTR param);
	wchar_t*			sGet(LPCTSTR param);
	void				dSet(LPCTSTR param, double val);
	void				sSet(LPCTSTR param, LPCTSTR val);
	void				sSet(LPCTSTR param, LPCWSTR val);

	// реализация IRMBasePlugin
public:
	void GetInfo(CLEFPLUGININFO* info);
	void SetResponder(IScriptCaller* rmpluginresponder);
	void SetPropsContainer(IPropertiesContainer* propscontainer);
	unsigned long ProcessMessage(int aiArgC, va_list aArgsV);	

public:
	virtual	void	OnSetPropsContainer(){};
	
protected:
	IPropertiesContainer*	m_propsContainer;
	IScriptCaller*			m_scriptCaller;
};

#endif // !defined(AFX_Chem3DBase_H__DBEA1B1B_43F0_4FE3_9EDA_311BCC236FB9__INCLUDED_)
