// LocalRepository.h: interface for the CLocalRepository class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALREPOSITORY_H__74AC67CE_477C_4C3F_B8AD_721C103CEF69__INCLUDED_)
#define AFX_LOCALREPOSITORY_H__74AC67CE_477C_4C3F_B8AD_721C103CEF69__INCLUDED_

#include <libxml/xpath.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLocalRepository  
{
public:
	CLocalRepository( const wchar_t* aBase);
	virtual ~CLocalRepository();

	BSTR getBaseBSTR();
	const wchar_t* getBase();

protected:
	CWComString sBase;
};

#endif // !defined(AFX_LOCALREPOSITORY_H__74AC67CE_477C_4C3F_B8AD_721C103CEF69__INCLUDED_)
