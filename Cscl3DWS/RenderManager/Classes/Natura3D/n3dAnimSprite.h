// n3dAnimSprite.h: interface for the Ccn3dAnimSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_cn3dAnimSprite_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
#define AFX_cn3dAnimSprite_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
//#include "AnimSprite.h"
//#include "../include/plugins/n3dplugupdatable.h"

//////////////////////////////////////////////////////////////////////////
class nrmObject;

//////////////////////////////////////////////////////////////////////////
using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class cn3dAnimSprite : public iplugupdatable
{
public:
	cn3dAnimSprite(nrmObject* pc = NULL);
	virtual ~cn3dAnimSprite();

public:
	IMPLEMENT_RELEASE

	BEGIN_INTERFACE_IMPLEMENTATION
		IMPLEMENT_INTERFACE(ibaseplugin)
		IMPLEMENT_INTERFACE(iplugupdatable)
	END_INTERFACE_IMPLEMENTATION

	// реализация natura3d::in3dplvisible1
public:
	void	pre_update(DWORD time, LPVOID value, ivisibleobject* apivisibleobject, bool& bhandled){ bhandled=false;};
	void	pre_render(ivisibleobject* apivisibleobject, bool& bhandled){ bhandled=false;};
	void	post_render(ivisibleobject* apivisibleobject){}
	void	post_update(DWORD time, LPVOID value, ivisibleobject* apivisibleobject);

protected:
//	// update object every frame
//	void update(DWORD	dwTime, void* data);
	nrmObject*	prmObject;
	int cFrame;
};

#endif // !defined(AFX_cn3dAnimSprite_H__9CD0DE60_405B_484B_B0DF_888B13F86693__INCLUDED_)
