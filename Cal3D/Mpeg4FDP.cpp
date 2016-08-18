// Mpeg4FDP.cpp: implementation of the CMpeg4FDP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "global.h"
#include "Mpeg4FDP.h"
#include "corebone.h"
#include "bone.h"
#include "mesh.h"
#include "submesh.h"
#include "model.h"
#include "skeleton.h"
#include "logfile.h"

/*#include "submesh.h"
#include "coresubmesh.h"
#include "renderer.h"*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMpeg4FDP::CMpeg4FDP():
	MP_VECTOR_INIT(cur_faps),
	MP_VECTOR_INIT(infl_array),
	MP_VECTOR_INIT(infl_points),
	MP_VECTOR_INIT(control_points),
	MP_VECTOR_INIT(expressions),
	MP_VECTOR_INIT(cur_cpoints)
{
	IRISD0 = ES0 = ENS0 = MNS0 = MW0 = AU = 0;
	memset( &n_points, 0, 4*MAX_CONTROL_GROUP*MAX_CONTROL_POINTS_INGROUP);
	clear();
}

CMpeg4FDP::~CMpeg4FDP()
{
	for( int ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			if( !n_points[ig][in])	continue;
			control_point*	cp = n_points[ig][in];
			MP_DELETE( cp);
		}
	}
}

/*//////////////////////////////////////////////////////////////////////////////////
// Add control point (from FDP points) to face.
// It`s didn`t take in account the neighbour vertices. They are calculated later.
/*//////////////////////////////////////////////////////////////////////////////////
#ifndef FPU_DEBUG
bool	CMpeg4FDP::add_control_point(int meshid, int submeshid, int id, DWORD color)
#else
bool	CMpeg4FDP::add_control_point(int meshid, int submeshid, int id, int maxid, DWORD color)
#endif
{
	if( color == 0xFFFFFF)	return true;
	//
	int	group_id = (color & 0xFF0000) >> 16;	// get MPEG4 group id
	int	in_group_id = color & 0x0000FF;			// get MPEG4 id in group

//char buf[255]; memset( &buf, 0, 255);
//sprintf( buf, "gr=%d, id=%d\n", group_id, in_group_id);OutputDebugString( buf);

	if( group_id >= MAX_CONTROL_GROUP || group_id < ST_GROUP_NUM) return true;
	if( in_group_id >= MAX_CONTROL_POINTS_INGROUP || in_group_id < ST_INGROUP_NUM) return true;

	// проверим, может на этой точке уже есть контрольная точка..
	// Тогда ее надо убрать
	for( int ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			control_point*	cp = n_points[ig][in];
			if( !cp)	continue;
			if( cp->mesh_id == meshid &&
				cp->submesh_id == submeshid &&
				cp->point_id == id)
			{
				remove_control_point( cp->group_id, cp->id);
			}
		}
	}

	// new control point
	if( n_points[group_id][in_group_id] == NULL)
	{
		MP_NEW_V2( n_points[group_id][in_group_id],  control_point, group_id, in_group_id);
		cp_num++;
	}
	control_point* cp = n_points[group_id][in_group_id];
	cp->points.clear();
	cp->mesh_id = meshid;
	cp->submesh_id = submeshid;
	cp->point_id = id;
#ifndef FPU_DEBUG
	cp->add_point( meshid, submeshid, id, 1.0f);
#else
	cp->add_point( meshid, submeshid, id, maxid, 1.0f);
#endif

	return true;
}

bool	CMpeg4FDP::remove_control_point(int agroup, int asubgroup)
{
	control_point* cp = n_points[agroup][asubgroup];
	if( cp)
	{
		cp_num--;
		MP_DELETE( cp);
		n_points[agroup][asubgroup] = NULL;
	}
	return true;
}

void CMpeg4FDP::calculate_FAPU(CalModel *pmodel)
{
	assert(pmodel);
	if( pmodel && pmodel->getVectorMesh().size() > 0)
	{
		calculate_FAPU(pmodel->getVectorMesh()[0]->getCoreMesh());
	}
}

void CMpeg4FDP::calculate_FAPU(CalCoreMesh *pmesh)
{
	int g1, g2;
	int n1, n2;
	CalVector	v1, v2, base, pos;
	std::vector<CalCoreSubmesh *>	vectorSubmesh = pmesh->getVectorCoreSubmesh();

	WORD	fapu_groups[10][2]={{3,1},{3,3},				// IRISD0
								{3,1},{3,2},				// ES0
								{3,11/*-3,5-*/},{9,15},		// ENS0
								{9,15},{2,2},				// MNS0
								{8,3},{8,4}};				// MW0

	CalQuaternion	qr(CalVector(0,0,1),0);
	if(pBone) 
		qr = ((CalBone*)pBone)->getRotationAbsolute();
	qr.conjugate();

	for( int i=0; i<5; i++)
	{
		g1 = fapu_groups[2*i][0];	n1 = fapu_groups[2*i][1];
		g2 = fapu_groups[2*i+1][0];	n2 = fapu_groups[2*i+1][1];
		if( n_points[g1][n1] && n_points[g2][n2])
		{
			v1 = vectorSubmesh[n_points[g1][n1]->points[0].submesh_id]->getVectorVertex()[n_points[g1][n1]->points[0].id].position;
			v2 = vectorSubmesh[n_points[g2][n2]->points[0].submesh_id]->getVectorVertex()[n_points[g2][n2]->points[0].id].position;
			//v1 *= qr;
			//v2 *= qr;

			switch(i)
			{
				case 0:	IRISD0	= abs(v1.z - v2.z);	break;
				case 1:	ES0		= abs(v1.x - v2.x);	break;
				case 2:	ENS0	= abs(v1.z - v2.z);	break;
				case 3:	MNS0	= abs(v1.z - v2.z);	break;
				case 4:	MW0		= abs(v1.x - v2.x);	break;
			}
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		NEUTRAL FACE POSITION
	//	1. the coordinate system is right-handed;
	//	2. head axes are parallel to the world axes;
	//	3. gaze is in direction of the Z axis;
	//	4. all face muscle are relaxed;
	//	5. eyelids are tangent to the iris;
	//	6. pupil diameter is 1/3 of iris diameter;
	//	7. lips are in contact; the line of the lips is horizontal and at the same height of lip corners;
	//	8. the mouth is closed and the upper teeth touch the lower ones;
	//	9. the tongue is flat, horizontal with the tip of the tongue touching the boundary between upper and lower teeth.
	*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	assert(n_points[AXE_CENTER_GROUP_ID][AXE_CENTER_SUBGROUP_ID]);
	control_point*	rootPoint = n_points[AXE_CENTER_GROUP_ID][AXE_CENTER_SUBGROUP_ID];
	submesh_face_point	roopFirstPoint = rootPoint->points[0];
	std::vector<CalCoreSubmesh::Vertex>&	vertices = vectorSubmesh[roopFirstPoint.submesh_id]->getVectorVertex();
	base = vertices[roopFirstPoint.id].position;

	for( int ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			control_point*	cp = n_points[ig][in];
			if( !cp)	continue;
	
			//if( n_points[ig][in]->points.size() == 0)	continue;

			// vertex position
			int	submeshId = cp->submesh_id;
			int	vertexId = cp->point_id;
			int	submeshSize = vectorSubmesh.size();
			if( submeshId >= 0 && submeshId < submeshSize)
			{
				int	vertexSize = vectorSubmesh[submeshId]->getVectorVertex().size();
				if( vertexId < vertexSize)
				{
					cp->v_neutral = vectorSubmesh[submeshId]->getVectorVertex()[vertexId].position;
					cp->v_neutral -= base;
				}
			}
		}
	}
	//
#ifdef CAL3D_EXPORTER
	savelog();
#endif
}

bool	CMpeg4FDP::is_control_point(int mid, int smid, int id)
{
	for( unsigned int i=0; i<control_points.size(); i++)
		if( (&control_points[i])->mesh_id==mid && (&control_points[i])->submesh_id==smid &&
			(&control_points[i])->id==id)	return true;
	return false;
}

void CMpeg4FDP::include_cpoint(control_point*	cp)
{
	std::vector<control_point*>::iterator	it;
	for( it=cur_cpoints.begin(); it<cur_cpoints.end(); it++)
		if((*it)->group_id==cp->group_id && (*it)->id==cp->id)
			return;
	cur_cpoints.push_back(cp);
}

//#ifdef CAL3D_EXPORTER
void	CMpeg4FDP::savelog()
{	
	char buf[255];
	CalLogFile	log("log.txt");
	//
	log.writeln("Presented vertices");
	//
	for( int ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		sprintf_s((char*)buf, 255, " GROUP %d", ig);
		log.writeln(buf);
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			if( !n_points[ig][in])	continue;
			sprintf_s((char*)buf, 255, "  SUB_GROUP %d", in);
			log.writeln(buf);
			control_point* point = n_points[ig][in];
			for(unsigned int i=0; i<point->points.size(); i++){
				submesh_face_point	sp=point->points[i];
				sprintf_s((char*)buf, 255, "   submesh_id=%d, id=%d, max_id=%d", sp.submesh_id, sp.id, sp.max_mesh_id+1);
				log.writeln(buf);
			}
		}
	}
	//
}

//#else
/*CAL3D_API Mpeg4FAPDesc	_fap_on_face::MPEG4FAPS[68] = {
	Mpeg4FAPDesc("viseme",				FD_MNS,		FD_U,	FD_DOWN,		2,	1),
	Mpeg4FAPDesc("expression",			FD_MNS,		FD_U,	FD_DOWN,		2,	1),
	Mpeg4FAPDesc("open_jaw",			FD_MNS,		FD_U,	FD_DOWN,		2,	1),
	Mpeg4FAPDesc("lower_t_midlip",		FD_MNS,		FD_B,	FD_DOWN,		2,	2),
	Mpeg4FAPDesc("raise_b_midlip",		FD_MNS,		FD_B,	FD_UP,			2,	3),
	Mpeg4FAPDesc("stretch_l_cornerlip",	FD_MW,		FD_B,	FD_LEFT,		2,	4),
	Mpeg4FAPDesc("stretch_r_cornerlip",	FD_MW,		FD_B,	FD_RIGHT,		2,	5),
	Mpeg4FAPDesc("lower_t_lip_lm",		FD_MNS,		FD_B,	FD_DOWN,		2,	6),
	Mpeg4FAPDesc("lower_t_lip_rm",		FD_MNS,		FD_B,	FD_DOWN,		2,	7),
	Mpeg4FAPDesc("raise_b_lip_lm",		FD_MNS,		FD_B,	FD_UP,			2,	8),
	Mpeg4FAPDesc("raise_b_lip_rm",		FD_MNS,		FD_B,	FD_UP,			2,	9),
	Mpeg4FAPDesc("raise_l_cornerlip",	FD_MNS,		FD_B,	FD_UP,			2,	4),
	Mpeg4FAPDesc("raise_r_cornerlip",	FD_MNS,		FD_B,	FD_UP,			2,	5),
	Mpeg4FAPDesc("thrust_jaw",			FD_MNS,		FD_U,	FD_FORWARD,		2,	1),
	Mpeg4FAPDesc("shift_jaw",			FD_MW,		FD_B,	FD_RIGHT,		2,	1),
	Mpeg4FAPDesc("push_b_lip",			FD_MNS,		FD_B,	FD_FORWARD,		2,	3),
	Mpeg4FAPDesc("push_t_lip",			FD_MNS,		FD_B,	FD_FORWARD,		2,	2),
	Mpeg4FAPDesc("depress_chin",		FD_MNS,		FD_B,	FD_UP,			2,	10),
	Mpeg4FAPDesc("close_t_l_eyelid",	FD_IRISD,	FD_B,	FD_DOWN,		3,	1),
	Mpeg4FAPDesc("close_t_r_eyelid",	FD_IRISD,	FD_B,	FD_DOWN,		3,	2),
	Mpeg4FAPDesc("close_b_l_eyelid",	FD_IRISD,	FD_B,	FD_UP,			3,	3),
	Mpeg4FAPDesc("close_b_r_eyelid",	FD_IRISD,	FD_B,	FD_UP,			3,	4),
	Mpeg4FAPDesc("yaw_l_eyeball",		FD_AU,		FD_B,	FD_LEFT,		3,	5),
	Mpeg4FAPDesc("yaw_r_eyeball",		FD_AU,		FD_B,	FD_LEFT,		3,	6),
	Mpeg4FAPDesc("pitch_l_eyeball",		FD_AU,		FD_B,	FD_DOWN,		3,	5),
	Mpeg4FAPDesc("pitch_r_eyeball",		FD_AU,		FD_B,	FD_DOWN,		3,	6),
	Mpeg4FAPDesc("thrust_l_eyeball",	FD_ES,		FD_B,	FD_FORWARD,		3,	5),
	Mpeg4FAPDesc("thrust_r_eyeball",	FD_ES,		FD_B,	FD_FORWARD,		3,	6),
	Mpeg4FAPDesc("dilate_l_pupil",		FD_IRISD,	FD_B,	FD_GROWING,		3,	5),
	Mpeg4FAPDesc("dilate_r_pupil",		FD_IRISD,	FD_B,	FD_GROWING,		3,	6),
	Mpeg4FAPDesc("raise_l_i_eyebrow",	FD_ENS,		FD_B,	FD_UP,			4,	1),
	Mpeg4FAPDesc("raise_r_i_eyebrow",	FD_ENS,		FD_B,	FD_UP,			4,	2),
	Mpeg4FAPDesc("raise_l_m_eyebrow",	FD_ENS,		FD_B,	FD_UP,			4,	3),
	Mpeg4FAPDesc("raise_r_m_eyebrow",	FD_ENS,		FD_B,	FD_UP,			4,	4),
	Mpeg4FAPDesc("raise_l_o_eyebrow",	FD_ENS,		FD_B,	FD_UP,			4,	5),
	Mpeg4FAPDesc("raise_r_o_eyebrow",	FD_ENS,		FD_B,	FD_UP,			4,	6),
	Mpeg4FAPDesc("squeeze_l_eyebrow",	FD_ES,		FD_B,	FD_RIGHT,		4,	1),
	Mpeg4FAPDesc("squeeze_r_eyebrow",	FD_ES,		FD_B,	FD_LEFT,		4,	2),
	Mpeg4FAPDesc("puff_l_cheek",		FD_ES,		FD_B,	FD_LEFT,		5,	1),
	Mpeg4FAPDesc("puff_r_cheek",		FD_ES,		FD_B,	FD_RIGHT,		5,	2),
	Mpeg4FAPDesc("lift_l_cheek",		FD_ENS,		FD_U,	FD_UP,			5,	3),
	Mpeg4FAPDesc("lift_r_cheek",		FD_ENS,		FD_U,	FD_UP,			5,	4),
	Mpeg4FAPDesc("shift_tongue_tip",	FD_MW,		FD_B,	FD_RIGHT,		6,	1),
	Mpeg4FAPDesc("raise_tongue_tip",	FD_MNS,		FD_B,	FD_UP,			6,	1),
	Mpeg4FAPDesc("thrust_tongue_tip",	FD_MW,		FD_B,	FD_FORWARD,		6,	1),
	Mpeg4FAPDesc("raise_tongue",		FD_MNS,		FD_B,	FD_UP,			6,	2),
	Mpeg4FAPDesc("tongue_roll",			FD_AU,		FD_U,	FD_UPWARD,		6,	3),
	Mpeg4FAPDesc("head_pitch",			FD_AU,		FD_B,	FD_DOWN,		7,	1),
	Mpeg4FAPDesc("head_yaw",			FD_AU,		FD_B,	FD_LEFT,		7,	1),
	Mpeg4FAPDesc("head_roll",			FD_AU,		FD_B,	FD_RIGHT,		7,	1),
	Mpeg4FAPDesc("lower_t_midlip _o",	FD_MNS,		FD_B,	FD_DOWN,		8,	1),
	Mpeg4FAPDesc("raise_b_midlip_o",	FD_MNS,		FD_B,	FD_UP,			8,	2),
	Mpeg4FAPDesc("stretch_l_cornerlip_o",FD_MW,		FD_B,	FD_LEFT,		8,	3),
	Mpeg4FAPDesc("stretch_r_cornerlip_o",FD_MW,		FD_B,	FD_RIGHT,		8,	4),
	Mpeg4FAPDesc("lower_t_lip_lm _o",	FD_MNS,		FD_B,	FD_DOWN,		8,	5),
	Mpeg4FAPDesc("lower_t_lip_rm _o",	FD_MNS,		FD_B,	FD_DOWN,		8,	6),
	Mpeg4FAPDesc("raise_b_lip_lm_o",	FD_MNS,		FD_B,	FD_UP,			8,	7),
	Mpeg4FAPDesc("raise_b_lip_rm_o",	FD_MNS,		FD_B,	FD_UP,			8,	8),
	Mpeg4FAPDesc("raise_l_cornerlip_o",	FD_MNS,		FD_B,	FD_UP,			8,	3),
	Mpeg4FAPDesc("raise_r_cornerlip _o",FD_MNS,		FD_B,	FD_UP,			8,	4),
	Mpeg4FAPDesc("stretch_l_nose",		FD_ENS,		FD_B,	FD_LEFT,		9,	1),
	Mpeg4FAPDesc("stretch_r_nose",		FD_ENS,		FD_B,	FD_RIGHT,		9,	2),
	Mpeg4FAPDesc("raise_nose",			FD_ENS,		FD_B,	FD_UP,			9,	3),
	Mpeg4FAPDesc("bend_nose",			FD_ENS,		FD_B,	FD_RIGHT,		9,	3),
	Mpeg4FAPDesc("raise_l_ear",			FD_ENS,		FD_B,	FD_UP,			10,	1),
	Mpeg4FAPDesc("raise_r_ear",			FD_ENS,		FD_B,	FD_UP,			10,	2),
	Mpeg4FAPDesc("pull_l_ear",			FD_ENS,		FD_B,	FD_LEFT,		10,	3),
	Mpeg4FAPDesc("pull_r_ear",			FD_ENS,		FD_B,	FD_RIGHT,		10,	4)
};*/


void CMpeg4FDP::apply_fap(int id, float /*dur*/)
{
	cur_faps.push_back( fap_on_face(id));
	init_fap_params((fap_on_face*)&*(cur_faps.end()-1));
}


_fap_on_face::_fap_on_face(int id)
{
	fap_id=id;
}
//#endif

bool	_fap_on_face::get_vector(CalVector& /*v*/)
{
	/*Mpeg4FAPDesc*	fap = &MPEG4FAPS[fap_id];
	if( fap->ubd == FD_GROWING)	return false;
	v = shift;*/
	return true;
}

bool CMpeg4FDP::create_influences(int /*istep*/)
{
	return false;
}

void CMpeg4FDP::relax()
{
	for( int ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			if( !n_points[ig][in])	continue;
			n_points[ig][in]->v_current.set(0.0,0.0,0.0);
		}
	}
}

void CMpeg4FDP::clear()
{
	infl_points.clear();
	infl_array.clear();
	lastindex=0;
	cp_num = 0;
	pBone=NULL;
	for( int ig=0; ig<MAX_CONTROL_GROUP; ig++)
		for( int in1=0; in1<MAX_CONTROL_POINTS_INGROUP; in1++)
			if( n_points[ig][in1])
			{
				MP_DELETE( n_points[ig][in1]);
			}

	memset( &n_points, 0, 4*MAX_CONTROL_GROUP*MAX_CONTROL_POINTS_INGROUP);
	IRISD0 = 0;
	ES0 = 0;
	ENS0 = 0;
	MNS0 = 0;
	MW0 = 0;
	AU = 0;
}

void CMpeg4FDP::raise_cur_faps(float /*k*/)
{
	/*for( int i=0; i<cur_faps.size(); i++)
		(&cur_faps[i])->unit_count = k;*/
}

control_point* CMpeg4FDP::getCP(int g, int i)
{
	return n_points[g][i];
}


/*//////////////////////////////////////////
// Define one Expression
*///////////////////////////////////////////
void _MPEGExpression::add_fap(int id, float /*w*/)
{
	if( contain(id))	delete_fap(id);
	fap_on_face	new_fap(id);
	faps.push_back(id);
}

fap_on_face* _MPEGExpression::contain(int id)
{
	for(unsigned int i=0; i<faps.size(); i++)
		if( (&faps[i])->fap_id == id)	return	&faps[i];
	return NULL;
}


void _MPEGExpression::delete_fap(int id)
{
	std::vector<fap_on_face>::iterator	ifaps;
	for(ifaps=faps.begin(); ifaps!=faps.end(); ifaps++)
		if( ifaps->fap_id == id)
		{
			faps.erase(ifaps);
			return;
		}
}

/*//////////////////////////////////////////
// Define one Viseme
*///////////////////////////////////////////
void MPEGViseme::add_point(viseme_cp point)
{
	points.push_back(point);
}

void CMpeg4FDP::init_fap_params(fap_on_face * /*fap*/)
{
	/*Mpeg4FAPDesc*	afap = &fap_on_face::MPEG4FAPS[fap->fap_id];
	switch( afap->posm)
	{
		case FD_LEFT:
			fap->base_shift.set(1,0,0);
			break;
		case FD_RIGHT:
			fap->base_shift.set(-1,0,0);
			break;
		case FD_UP:
			fap->base_shift.set(0,1,0);
			break;
		case FD_DOWN:
			fap->base_shift.set(0,-1,0);
			break;
		case FD_FORWARD:
			fap->base_shift.set(0,0,1);
			break;
		case FD_UPWARD:
			fap->base_shift.set(0,0,-1);
			break;
	}

	switch( afap->units)
	{
		case FD_MNS:
			fap->base_unit = MNS0 / 1024.0f;
			break;
		case FD_MW:
			fap->base_unit = MW0 / 1024.0f;
			break;
		case FD_IRISD:
			fap->base_unit = IRISD0 / 1024.0f;
			break;		
		case FD_ES:
			fap->base_unit = ES0 / 1024.0f;
			break;
		case FD_ENS:
			fap->base_unit = ENS0 / 1024.0f;
			break;
	}

	fap->group_id = afap->group_id;
	fap->subgroup_id = afap->subgroup_id;
	fap->shift = fap->base_shift;
	fap->shift *= fap->base_unit*300;*/
}

void CMpeg4FDP::apply_fap(FACEExpression *exp)
{
	if( exp->type == FACEExpression::FT_EXPRESSION)
	{
		for( unsigned int i=0; i<exp->expression->faps.size(); i++)
		{
			fap_on_face	fof = exp->expression->faps[i];
			if( !getCP(fof.group_id,fof.subgroup_id))	continue;
			control_point*	point = getCP(fof.group_id,fof.subgroup_id);
			if( fof.shift.x==0 && fof.shift.y==0 && fof.shift.z==0) continue;
			point->v_current += fof.shift * exp->weight;
			//
			//Cal::caldoutFF("anim w=%.2f\n", exp->weight);
			//
			include_cpoint(point);
		}
	}

	if( exp->type == FACEExpression::FT_VISEME)
	{
		int size = exp->viseme->points.size();
		for( int i=0; i<size; i++)
		{
			viseme_cp&	cp_desc = exp->viseme->points[i];
			control_point*	cp = getCP(cp_desc.group_id,cp_desc.id);
			if( !cp)	continue;
			cp->v_current += cp_desc.v_current * exp->weight;

			include_cpoint(cp);
		}
	}
}

void CMpeg4FDP::execute(FACEExpression* fe, float dur, float dweight)
{
	FACEExpression* fexpress = get_expression(fe);

	if( fexpress == NULL)
	{
		fe->init(this);
		expressions.push_back(fe);
	}
	fe->execute(dur, dweight);
}

void CMpeg4FDP::remove(FACEExpression* fe, float dur)
{
	FACEExpression* fexpress = get_expression(fe);

	if( fexpress == NULL)	return;
	fe->remove(dur);
}

/*//////////////////////////////////////////
// Update control points positions
*///////////////////////////////////////////
void CMpeg4FDP::update(float dur)
{
	relax();
	cur_cpoints.clear();
	//
	std::vector<FACEExpression*>::iterator	it = expressions.begin();
	while( it!=expressions.end())
	{
		if( (*it)->update(dur))
		{
			apply_fap( *it);
			++it;
		}
		else
		{
			it = expressions.erase(it);
		}
	}

	for( unsigned int i=0; i<cur_faps.size(); i++)
	{
		apply_fap( &cur_faps[i]);
	}
}

void CMpeg4FDP::apply_fap(fap_on_face *fap)
{
	if( !getCP(fap->group_id, fap->subgroup_id))	return;
	control_point*	cp = getCP(fap->group_id, fap->subgroup_id);
	cp->v_current += fap->shift;
	include_cpoint(cp);
}

_FACEExpression::_FACEExpression():
	MP_STRING_INIT(name)
{
}

_FACEExpression::_FACEExpression(const _FACEExpression& other):
	MP_STRING_INIT(name)
{
	name = other.name;
	type = other.type;
	expression = other.expression;
	viseme = other.viseme;
	duration = other.duration;
	destWeight = other.destWeight;
	m_time = other.m_time;
	weight = other.weight;
	state = other.state;
}

// Methods
_FACEExpression::_FACEExpression(MPEGExpression*	expr):
	MP_STRING_INIT(name)
{
	viseme = NULL;
	type = FT_EXPRESSION;
	expression = expr;
}

_FACEExpression::_FACEExpression(MPEGViseme*	vsm):
	MP_STRING_INIT(name)
{
	expression = NULL;
	type = FT_VISEME;
	viseme = vsm;
}

_FACEExpression::~_FACEExpression()
{
	if( expression != NULL)
		MP_DELETE( expression);
	if( viseme != NULL)
		MP_DELETE( viseme);
}

void	FACEExpression::set_name(LPSTR	pname)
{
	name = pname;
}

bool FACEExpression::execute(float	adur, float dweight)
{
	m_time = 0;
	duration = adur;
	destWeight = dweight;
	state = CalAnimation::STATE_IN;
	return true;
}

bool FACEExpression::remove(float	adur)
{
	m_time = 0;
	duration = adur;
	state = CalAnimation::STATE_OUT;
	return true;
}

void FACEExpression::init(CMpeg4FDP* face)
{
	if( type == FACEExpression::FT_EXPRESSION)
	{
		for(unsigned int i=0; i<expression->faps.size(); i++)
		{
			fap_on_face*	fof = &expression->faps[i];
			face->init_fap_params(fof);
		}
	}
	else	if( type == FACEExpression::FT_VISEME)
	{
		viseme->convert_to_space_units( face);
	}
}

bool _FACEExpression::update(float deltaTime)
{
  // update animation action time
  m_time += deltaTime;

  // handle IN phase
  if((unsigned int)state == CalAnimation::STATE_IN)
  {
    // cehck if we are still in the IN phase
    if(m_time < duration)
    {
      weight = m_time / duration * destWeight;
    }
    else
    {
      state = CalAnimation::STATE_STEADY;
      weight = destWeight;
	  return true;
    }
  }

  // hanbdle STEADY
  if((unsigned int)state == CalAnimation::STATE_STEADY)
  {
	weight = destWeight;
  }

  // handle OUT phase
  if((unsigned int)state == CalAnimation::STATE_OUT)
  {
    // cehck if we are still in the OUT phase
    if(m_time < duration)
    {
      weight = (duration - m_time) / duration * destWeight;
    }
    else
    {
      // we reached the end of the action animation
      weight = 0.0f;
      return false;
    }
  }
  return true;
}

FACEExpression* CMpeg4FDP::get_expression(FACEExpression *fe)
{
	for(unsigned  int i=0; i<expressions.size(); i++)
	{
		if( expressions[i] == fe)	return fe;
		if( (fe->name.size() > 0) && (expressions[i]->name == fe->name))
			return expressions[i];
	}
	return NULL;
}


_MPEGViseme::_MPEGViseme():
	MP_VECTOR_INIT(points)
{
	units = FACEUNIT_WORLD;
	iris = 0;
	es = 0;
	ens = 0;
	mns = 0;
	mw = 0;
}

_MPEGViseme::_MPEGViseme(const _MPEGViseme& viseme):
	MP_VECTOR_INIT(points)
{
	units = viseme.units;
	points = viseme.points;
	iris = viseme.iris;
	es = viseme.es;
	ens = viseme.ens;
	mns = viseme.mns;
	mw = viseme.mw;
}

bool CMpeg4FDP::initialize(CalModel *pmodel)
{
	assert(pmodel);
	if( !pmodel)
	{
		return false;
	}
	// get bone vector of the skeleton
	std::vector<CalBone *>& vectorBone = pmodel->getSkeleton()->getVectorBone();
	// lock all vertices data
	int mc, sc;
	std::vector<CalMesh *>& vectorMesh = pmodel->getVectorMesh();
	mc = vectorMesh.size();
	if( mc == 0)
	{
		return false;
	}
	std::vector<CalSubmesh *>& vectorSubmesh = vectorMesh[0]->getVectorSubmesh();
	sc = vectorSubmesh.size();
	// get vertex vector of the core submesh
	control_point*	cp = n_points[AXE_CENTER_GROUP_ID][AXE_CENTER_SUBGROUP_ID];
	assert( cp);
	if( !cp)
	{
		return false;
	}
	assert( cp->points.size()>0);
	if( cp->points.size() == 0)
	{
		return false;
	}
	submesh_face_point*	sfp = NULL;
	for( unsigned int ip=0; ip<cp->points.size(); ip++)
	{
		if( cp->points[ip].weight == 1)
		{
			sfp = &cp->points[ip];
		}
		assert( sfp);
		if( !sfp)
		{
			continue;
		}
		if( (int)vectorSubmesh.size() <= sfp->submesh_id)
		{
			sfp = NULL;
			continue;
		}

		std::vector<CalCoreSubmesh::Vertex>& vectorVertex = vectorSubmesh[sfp->submesh_id]->getCoreSubmesh()->getVectorVertex();
		if( (int)vectorVertex.size() <= sfp->id)
		{
			assert( false);
			return false;
		}
		CalCoreSubmesh::Vertex& vertex = vectorVertex[sfp->id];
		int inflSize = vertex.vectorInfluence.size();
		for (int i=0; i<inflSize; i++)
		{
			CalCoreSubmesh::Influence& influence = vertex.vectorInfluence[i];
			if( influence.boneId < (int)vectorBone.size())
			{
				CalBone* pTempBone = vectorBone[influence.boneId];
				std::string	sBoneName(pTempBone->getCoreBone()->getName());
				if(vertex.vectorInfluence.size()==1 || sBoneName.find("Head")!=-1 || sBoneName.find("head")!=-1)
				{
					pBone = (LPVOID)vectorBone[influence.boneId];
					return true;
				}
			}
		}
	}
	// only one bone to base vertex
	assert( false);
	return false;
}

void CMpeg4FDP::remove_all_emotions()
{
	expressions.clear();
}

bool MPEGViseme::convert_to_FAPU_units(CMpeg4FDP*	face)
{
	if( !face)	return false;
	if( units == FACEUNIT_WORLD)
	{
		if( !convert_to_units( face->IRISD0, face->ES0, face->ENS0, face->MNS0, face->MW0, FACEUNIT_FDP))
		{
			return false;
		}
	}
	iris = face->IRISD0;
	es = face->ES0;
	ens = face->ENS0;
	mns = face->MNS0;
	mw = face->MW0;
	return true;
}

bool MPEGViseme::convert_to_space_units(CMpeg4FDP*	face)
{
	if( !face)	return false;
	if( units == FACEUNIT_FDP)
	{
		return convert_to_units( face->IRISD0, face->ES0, face->ENS0, face->MNS0, face->MW0, FACEUNIT_WORLD);
	}
	return true;
}

bool	MPEGViseme::convert_to_units(float airis, float aes, float aens, float amns, float amw, MPEG4_UNITS aunits)
{
	std::vector<viseme_cp>	apoints;
	for(unsigned int i1=0; i1<points.size(); i1++)
		apoints.push_back( points[i1]);

	int	ps = apoints.size();
	for( int i=0; i<ps; i++)
	{
		viseme_cp&	bcp = apoints[i];

		CalVector	k(0,0,0);
		switch(bcp.group_id) 
		{
			case 2:	case 5:	case 6:	case 7:	case 8:
				k.set(amw, amw, amw);
				break;
			case 3:	case 4:
				k.set(airis, airis, airis);
				break;
			case 9:
				if( bcp.id < 8)
					k.set(aens, aens, aens);
				else
					k.set(amw, amw, amw);
				break;
			default:
				{
					assert(false);
					return false;
				}
				
		}

		if( k.x==0 && k.y==0 && k.z==0)
		{
			assert(false);
			return false;
		}

		if( aunits == FACEUNIT_FDP)
		{
			bcp.v_current.x /= k.x;
			bcp.v_current.y /= k.y;
			bcp.v_current.z /= k.z;
		}
		else
		{
			bcp.v_current.x *= k.x;
			bcp.v_current.y *= k.y;
			bcp.v_current.z *= k.z;
		}
	}
	// save parameters
	points.clear();
	for( int i2=0; i2<ps; i2++)
		points.push_back(apoints[i2]);
	iris = airis;	es = aes;	ens = aens;	mns = amns;	mw = amw;
	units = aunits;
	return true;
}

submesh_face_point::~submesh_face_point()
{ 
	//influences.clear();
}

void submesh_face_point::add_influence(int gid, int igid, float weight)
{
	influences.push_back(ctrlp_infl(gid, igid, weight));
}

BOOL submesh_face_point::operator>(const _submesh_face_point& sp){ return weight<sp.weight?TRUE:FALSE;}
BOOL submesh_face_point::operator<(const _submesh_face_point& sp){ return weight>sp.weight?TRUE:FALSE;}
BOOL submesh_face_point::operator==(const _submesh_face_point& sp){ return weight==sp.weight;}

#ifdef FPU_DEBUG
	_submesh_face_point::_submesh_face_point(int mid, int smid, int iid, int maxid, FLOAT w):
#else
	_submesh_face_point::_submesh_face_point(int mid, int smid, int iid, float w):
#endif
		MP_VECTOR_INIT(influences)
	{
		mesh_id = mid;
		submesh_id=smid;
		id = iid;
		weight=w;
#ifdef FPU_DEBUG
		max_mesh_id=maxid;
#endif
	}

_submesh_face_point::_submesh_face_point():
	MP_VECTOR_INIT(influences)
{ 
	weight=0.0;
}

_submesh_face_point::_submesh_face_point(const _submesh_face_point& other):
	MP_VECTOR_INIT(influences)
{ 
	mesh_id = other.mesh_id;
	submesh_id = other.submesh_id;
	id = other.id;
	weight = other.weight;
	parent_gid = other.parent_gid;
	parent_id = other.parent_id;
#ifdef FPU_DEBUG
	max_mesh_id = other.max_mesh_id;
#endif
	influences = other.influences;
}