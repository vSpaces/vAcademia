// nrmVisemes.h: interface for the nrmVisemes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmVisemes_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmVisemes_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nrmObject.h"
#include "nrmMotion.h"
class nrmCharacter;

class nrmVisemes :	public nrmObject
{
public:
	friend	class	nrmCharacter;
	nrmVisemes(mlMedia* apMLMedia);
	virtual ~nrmVisemes();
	//void	init_on_character(nrmCharacter* pchar);

// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src
//	ivisemes	*pivisemes;

private:
	CMotion*	m_motion;
//	iloadable	*piloadable;
};

#endif // !defined(AFX_nrmVisemes_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
