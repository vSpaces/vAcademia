// Mpeg4FDP_cp.cpp: implementation of the CMpeg4FDP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "global.h"
#include "Mpeg4FDP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_control_point::_control_point()
{ 
	group_id=-1;
	id=-1;
	mesh_id=-1;
	submesh_id=-1;
	point_id=-1;
}

control_point::~control_point()
{
	points.clear();
	nbrs.clear();
}

_control_point::_control_point(int gid, int igid)
{
	group_id = gid;
	id=igid;
	mesh_id=-1;
	submesh_id=-1;
	point_id=-1;
}

void control_point::add_neighbour(int gid, int igid, float dist)
{
	for(unsigned int i=0; i<nbrs.size(); i++)
		if( nbrs[i].group_id==gid && nbrs[i].id==igid)	return;
	nbrs.push_back(NDef(gid,igid,dist));
}

#ifndef FPU_DEBUG
void control_point::add_point(int mid, int smid, int iid, float w=0.0)
{
	for( int i=0; i<points.size(); i++){
		if( points[i].mesh_id==mid && points[i].submesh_id==smid && points[i].id==iid) return;
	}
	submesh_face_point	fp( mid, smid, iid, w);
	fp.parent_gid = group_id;
	fp.parent_id = id;
	points.push_back( fp);
}
#else
void control_point::add_point(int mid, int smid, int iid, int maxid, float w)
{
	for( unsigned int i=0; i<points.size(); i++){
		if( points[i].mesh_id==mid && points[i].submesh_id==smid && points[i].id==iid && points[i].max_mesh_id==maxid) return;
	}
	submesh_face_point	fp( mid, smid, iid, maxid, w);
	fp.parent_gid = group_id;
	fp.parent_id = id;
	points.push_back( fp);
}
#endif

void control_point::erase_point(int index)
{
	if( index>=0 && index<points.size())
		points.erase( points.begin() + index);
}

void control_point::clear_points()
{
	points.clear();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

_Mpeg4FAPDesc::_Mpeg4FAPDesc()
{
}

_Mpeg4FAPDesc::_Mpeg4FAPDesc( const Mpeg4FAPDesc& cpy)
{
	name = cpy.name;
	units = cpy.units;
	ubd = cpy.ubd;
	posm = cpy.posm;
	group_id = cpy.group_id;
	subgroup_id = cpy.subgroup_id;
}

_Mpeg4FAPDesc::_Mpeg4FAPDesc(LPSTR n, FAPMRM u, FAPUBDIR ub, FAPDIR pm, int g, int sg)
{
	name += n;
	units=u;
	ubd=ub;
	posm=pm;
	group_id=g;
	subgroup_id=sg;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
_FACEExpressionsPack::_FACEExpressionsPack()
{ 
	count=0;
}

_FACEExpressionsPack::~_FACEExpressionsPack()
{ 
	std::vector<FACEExpression*>::iterator	pit;
	for(pit=pexpressions.begin(); pit!=pexpressions.end(); pit++)
	{
		FACEExpression*	exp = *pit;
		if(exp != NULL)
			delete exp;
	}
	pexpressions.clear();
}

void FACEExpressionsPack::add_expression(FACEExpression* pe)
{ 
	pexpressions.push_back(pe);
}