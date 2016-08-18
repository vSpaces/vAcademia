// nrmQuery.h: interface for the nrm3DGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmQuery_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmQuery_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "../CommonRenderManagerHeader.h"

#include "windows.h"

//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class nrmQuery// : public natura3d::iserverquery
{
public:
	nrmQuery(cm::cmICommunicationManager* apComManager);
	virtual ~nrmQuery();

// реализация iserverquery
public:
	LPCTSTR	query(LPCTSTR	asquery, LPCTSTR	asparams, int* apierror = NULL);
	byte*	query_data(LPCTSTR	asquery, LPCTSTR	asparams, DWORD* adatasize, byte** adata, int* apierror = NULL);

private:
	MP_STRING	sAnswer;
	cm::cmICommunicationManager* m_pComManager;
};

#endif // !defined(AFX_nrmQuery_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
