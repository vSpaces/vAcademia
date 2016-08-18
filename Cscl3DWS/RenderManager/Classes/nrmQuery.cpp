// nrmQuery.cpp: implementation of the nrmQuery class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <Assert.h>
#include "nrmQuery.h"
#include "oms/oms_context.h"

nrmQuery::nrmQuery(cm::cmICommunicationManager* apComManager):
	MP_STRING_INIT(sAnswer)
{
	assert( apComManager);
	m_pComManager = apComManager;
}

LPCTSTR	nrmQuery::query(LPCTSTR	asquery, LPCTSTR	asparams, int* apierror)
{
	USES_CONVERSION;
	assert( m_pComManager);
	const wchar_t* pwcResult = m_pComManager->Query(A2W(asquery), A2W(asparams), apierror);
	if( pwcResult)
		sAnswer = W2A(pwcResult);
	else
		sAnswer = "";
	return (LPCTSTR)sAnswer.c_str();
}

byte*	nrmQuery::query_data(LPCTSTR	asquery, LPCTSTR	asparams, DWORD* adatasize, byte** adata, int* apierror)
{
	if( !adata)	return NULL;
	if( !adatasize)	return NULL;
	USES_CONVERSION;
	assert( m_pComManager);
	int	ierror = 0;
	byte* pwcResult = m_pComManager->QueryData(
		A2W(asquery), 
		A2W(asparams), 
		adatasize, 
		adata);
	if( apierror)
		*apierror = ierror;
	return pwcResult;
}

nrmQuery::~nrmQuery()
{
}