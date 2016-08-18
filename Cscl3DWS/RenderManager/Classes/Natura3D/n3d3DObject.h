// n3d3DObject.h: interface for the Ccn3DObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_cn3DObject_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
#define AFX_cn3DObject_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class nrm3DObject;


//////////////////////////////////////////////////////////////////////////
class cn3DObject
{
public:
	cn3DObject(nrm3DObject* pc = NULL);
	virtual ~cn3DObject();

protected:
	nrm3DObject*	prmObject;
};

#endif // !defined(AFX_cn3DObject_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
