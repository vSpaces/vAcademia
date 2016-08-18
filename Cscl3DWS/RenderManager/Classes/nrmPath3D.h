// nrmPath3D.h: interface for the nrmPath3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmPath3D_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmPath3D_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#include "nrmanager.h"
#include "nrmObject.h"

//////////////////////////////////////////////////////////////////////////
class nrmObject;
class nrmCharacter;
class nrm3DObject;

//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class nrmPath3D :	public nrmObject,
					public moIPath3D
{
public:
	friend	class	nrmCharacter;
	friend	class	nrm3DObject;

	nrmPath3D(mlMedia* apMLMedia);
	//ibspline*	get_path(){ return pibspline;}
	virtual ~nrmPath3D();

// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализация  moIPath3D
public:
	moIPath3D* GetIPath3D(){ return this; }
	ml3DPosition getPosition(double adKoef);
	ml3DPosition getLinearPosition(double adKoef);
	ml3DPosition getNormal(double adKoef);
	ml3DPosition getTangent(double adKoef);
	ml3DPosition getNormalByLinearPos(double adKoef);
	ml3DPosition getTangentByLinearPos(double adKoef);

private:
/*	iloadable*	piloadable;
	ibspline*	pibspline;*/
};

#endif // !defined(AFX_nrmPath3D_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
