// ResManCreation.h: interface for the CResManCreation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESMANCREATION_H__46EDE0E6_2C75_4D18_96B7_0664BF4F6A7C__INCLUDED_)
#define AFX_RESMANCREATION_H__46EDE0E6_2C75_4D18_96B7_0664BF4F6A7C__INCLUDED_

//#include "ResManIntr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CResMan;

class CResManCreation  
{
public:
	CResManCreation();
	virtual ~CResManCreation();
	//void setManager( IUnknown* aIResMan, CResMan* aResMan);
	void setManager( CResMan* aResMan);
protected:
	// —сылка на менеджер ресурсов
	CResMan* pResMan;
	// —сылка на интерфейс менеджера ресурсов
	//CComQIPtr< IResMan> spIResMan;
};

#endif // !defined(AFX_RESMANCREATION_H__46EDE0E6_2C75_4D18_96B7_0664BF4F6A7C__INCLUDED_)
