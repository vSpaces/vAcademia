// N3DCharacter.h: interface for the CN3DCharacter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3DCHARACTER_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
#define AFX_N3DCHARACTER_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class nrm3DObject;

class CN3DCharacter : public C3DCharacter  
{
public:
	CN3DCharacter(nrm3DObject* pc = NULL);
	virtual ~CN3DCharacter();
	virtual void update(DWORD	dwTime, void* data);	// update object every frame

protected:
	nrm3DObject*	prmCharacter;
};

#endif // !defined(AFX_N3DCHARACTER_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
