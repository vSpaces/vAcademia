// nrmPath3D.cpp: implementation of the nrmPath3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmPath3D.h"
//#include "3dloader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define UP_VECTOR	(CalVector(0,0,1))

nrmPath3D::nrmPath3D(mlMedia* apMLMedia) : nrmObject(apMLMedia)
{
/*	pibspline = NULL;
	piloadable = NULL;*/
}

nrmPath3D::~nrmPath3D()
{

}

// загрузка нового ресурса
bool nrmPath3D::SrcChanged()
{
	/*USES_CONVERSION;
	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	//if(!pwcSrc) return false;
	mlString sSRC; 
	if( pwcSrc)
		sSRC = pwcSrc;

	if( !pibspline)
	{
		piloadable = get_iobjslib()->create_bspline();
		if( !piloadable)
		{
			rm_trace("Can`t create Path3D : %s\n", W2A(pwcSrc));
			return false;
		}
		GET_INTERFACE(ibspline,piloadable,&pibspline);
		GET_INTERFACE(ibase,piloadable,&pibase);
		assert(pibase);
		assert(pibspline);
	}
	
	if( !pwcSrc || !piloadable->load(W2A(pwcSrc)))
	{
		rm_trace("Can`t load Path3D : %s\n", W2A(pwcSrc));
		return false;
	}*/

	return true;
}

ml3DPosition nrmPath3D::getPosition(double /*adKoef*/)
{
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	/*if( pibspline)
	{
		CalVector vpos = pibspline->get_pos(adKoef);
		pos.x=vpos.x;	pos.y=vpos.y;	pos.z=vpos.z;
	}*/
	return pos;
}

ml3DPosition nrmPath3D::getLinearPosition(double /*adKoef*/)
{
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	/*if( pibspline)
	{
		CalVector vpos = pibspline->get_linear_pos(adKoef);
		pos.x=vpos.x;	pos.y=vpos.y;	pos.z=vpos.z;
	}*/
	return pos;
}

ml3DPosition nrmPath3D::getNormal(double /*adKoef*/)
{
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	/*if( pibspline)
	{
		CalVector vtangent = pibspline->get_tangent(adKoef);
		vtangent = vtangent.cross( UP_VECTOR);
		pos.x=vtangent.x;	pos.y=vtangent.y;	pos.z=vtangent.z;
	}*/
	return pos;
}

ml3DPosition nrmPath3D::getTangent(double /*adKoef*/)
{
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	/*if( pibspline)
	{
		CalVector vtangent = pibspline->get_tangent(adKoef);
		pos.x=vtangent.x;	pos.y=vtangent.y;	pos.z=vtangent.z;
	}*/
	return pos;
}

ml3DPosition nrmPath3D::getNormalByLinearPos(double /*adKoef*/)
{
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	/*if( pibspline)
	{
		CalVector vtangent = pibspline->get_linear_tangent(adKoef);
		vtangent = vtangent.cross( UP_VECTOR);
		pos.x=vtangent.x;	pos.y=vtangent.y;	pos.z=vtangent.z;
	}*/
	return pos;
}

ml3DPosition nrmPath3D::getTangentByLinearPos(double /*adKoef*/)
{
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	/*if( pibspline)
	{
		CalVector vtangent = pibspline->get_linear_tangent(adKoef);
		pos.x=vtangent.x;	pos.y=vtangent.y;	pos.z=vtangent.z;
	}*/
	return pos;
}

