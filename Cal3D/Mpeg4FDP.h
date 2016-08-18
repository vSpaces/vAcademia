// Mpeg4FDP.h: interface for the CMpeg4FDP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MPEG4FDP_H__4AA98B71_6B29_48A1_8B5B_2D5A0F85AC23__INCLUDED_)
#define AFX_MPEG4FDP_H__4AA98B71_6B29_48A1_8B5B_2D5A0F85AC23__INCLUDED_

#include "CommonCal3DHeader.h"

#include <vector>
#include "vector.h"
#include "coremesh.h"
#include "animation.h"

#define FPU_DEBUG

#define MAX_CONTROL_POINTS_INGROUP	16
#define MAX_CONTROL_GROUP			16
#define ST_GROUP_NUM				2
#define ST_INGROUP_NUM				1

#define AXE_CENTER_GROUP_ID			7
#define AXE_CENTER_SUBGROUP_ID		1

typedef struct _ctrlp_infl
{
	int		parent_gid;
	int		parent_id;
	float	weight;

	_ctrlp_infl::_ctrlp_infl(){}
	_ctrlp_infl::_ctrlp_infl(int gid, int igid, float aweight){ parent_gid=gid; parent_id=igid, weight=aweight;}

} ctrlp_infl;

typedef struct CAL3D_API _submesh_face_point{
	int	mesh_id;			// номер меша
	int	submesh_id;			// номер сабмеша
	int	id;					// номер по порядку
	float	weight;			// вес для данной точки
	int		parent_gid;		// номер группы контрольной точки
	int		parent_id;		// номер в группе контрольной точки
#ifdef FPU_DEBUG
	int			max_mesh_id;
#endif
	MP_VECTOR<ctrlp_infl>	influences;

	_submesh_face_point();
	_submesh_face_point(const _submesh_face_point& other);
	~_submesh_face_point();

#ifdef FPU_DEBUG
	_submesh_face_point::_submesh_face_point(int mid, int smid, int iid, int maxid, float w=0.0f);
#else
	_submesh_face_point::_submesh_face_point(int mid, int smid, int iid, float w=0.0f);
#endif

	void _submesh_face_point::add_influence(int gid, int igid, float weight);
	BOOL operator>(const _submesh_face_point& sp);
	BOOL operator<(const _submesh_face_point& sp);
	BOOL operator==(const _submesh_face_point& sp);
}	submesh_face_point;

/// The control_point Vertices neighbours.
typedef struct _NDef
{
  int	group_id;								// номер группы контрольных точек
  int	id;										// номер в группе контрольных точек
  float dist;

  _NDef::_NDef(){}
  _NDef::_NDef(int agid, int aid, float adist){ group_id=agid; id=aid; dist=adist;}
} NDef;

typedef struct CAL3D_API _control_point
{
	int	group_id;								// номер группы контрольных точек
	int	id;										// номер в группе контрольных точек
	CalVector	v_neutral;						// вектор нейтрального положения для данной точки
	MP_VECTOR<submesh_face_point>	points;		// массив вершин данной контрольной точки
	MP_VECTOR<NDef>				nbrs;			// массив соседних контрольных вершин
	CalVector	v_current;						// вектор текущего положения для данной точки
	// привязка к точке на модели
	int	mesh_id;
	int	submesh_id;
	int	point_id;

	_control_point(const _control_point& other);
	_control_point();
	~_control_point();
	_control_point(int gid, int igid);
	void add_neighbour(int gid, int igid, float dist);
#ifndef FPU_DEBUG
	void add_point(int mid, int smid, int iid, float w=0.0);
#else
	void add_point(int mid, int smid, int iid, int maxid, float w=0.0);
#endif
	void erase_point(int index);
	void clear_points();
}	control_point;


/*//////////////////////////////////////////
// Control point id
*///////////////////////////////////////////
typedef struct _viseme_cp{
	int	group_id;								// номер группы контрольных точек
	int	id;										// номер в группе контрольных точек
	CalVector	v_current;						// вектор нейтрального положения для данной точки
	bool	onFAPUnits;

	_viseme_cp::_viseme_cp(){ onFAPUnits=false;}
} viseme_cp;

enum FAPDIR{ FD_LEFT, FD_RIGHT, FD_UP, FD_DOWN, FD_FORWARD, FD_UPWARD, FD_GROWING};
enum FAPMRM{ FD_MNS, FD_MW, FD_IRISD, FD_AU, FD_ES, FD_ENS};
enum FAPUBDIR{ FD_U, FD_B};

/*//////////////////////////////////////////
// Define parameters of one FAP
*///////////////////////////////////////////
typedef struct _Mpeg4FAPDesc{
	//
	MP_STRING name;
	FAPMRM		units;
	FAPUBDIR	ubd;
	FAPDIR		posm;
	int			group_id;
	int			subgroup_id;
	//
	_Mpeg4FAPDesc();
	_Mpeg4FAPDesc( const _Mpeg4FAPDesc& cpy);
	_Mpeg4FAPDesc::_Mpeg4FAPDesc(LPSTR n, FAPMRM u, FAPUBDIR ub, FAPDIR pm, int g, int sg);
} Mpeg4FAPDesc;

class CMpeg4FDP;

/*//////////////////////////////////////////
// Define one active FAP, 
// with initialized parameters
*///////////////////////////////////////////
typedef struct CAL3D_API _fap_on_face
{
public:
	//static Mpeg4FAPDesc	MPEG4FAPS[68];

	int			fap_id;
	CalVector	base_shift;
	float		base_unit;
	int			group_id;
	int			subgroup_id;

	CalVector	shift;

	_fap_on_face::_fap_on_face(){}

	_fap_on_face::_fap_on_face(int id);
	bool	_fap_on_face::get_vector(CalVector& v);
} fap_on_face;

/*//////////////////////////////////////////
// Define one Expression
*///////////////////////////////////////////
typedef struct CAL3D_API _MPEGExpression
{
	MP_VECTOR<fap_on_face>	faps;

	// Methods
	void _MPEGExpression::add_fap(int id, float w);
	fap_on_face* _MPEGExpression::contain(int id);
	void _MPEGExpression::delete_fap(int id);

	_MPEGExpression();
	_MPEGExpression(const _MPEGExpression& other);
} MPEGExpression;

/*//////////////////////////////////////////
// Define one Viseme
*///////////////////////////////////////////
enum	MPEG4_UNITS		{FACEUNIT_WORLD, FACEUNIT_FDP};
typedef struct CAL3D_API _MPEGViseme
{
	MPEG4_UNITS				units;
	MP_VECTOR<viseme_cp>	points;
	float iris, es, ens, mns, mw;

	_MPEGViseme();
	_MPEGViseme(const _MPEGViseme& viseme);

	// Methods
	bool	convert_to_FAPU_units(CMpeg4FDP*	face);
	bool	convert_to_space_units(CMpeg4FDP*	face);
	bool	convert_to_units(float airis, float aes, float aens, float amns, float amw, MPEG4_UNITS aunits);

	void	add_point(viseme_cp point);


} MPEGViseme;

/*//////////////////////////////////////////
// Define Face Expression
*///////////////////////////////////////////
typedef struct CAL3D_API _FACEExpression
{
enum	FACE_TYPE	{FT_EXPRESSION, FT_VISEME};

	MP_STRING	name;

	FACE_TYPE	type;
	MPEGExpression*	expression;
	MPEGViseme*		viseme;

	float			duration;
	float			destWeight;

	float			m_time;
	float			weight;
	int				state;

	// Methods
	_FACEExpression(MPEGExpression*	expr);
	_FACEExpression(MPEGViseme*	vsm);
	virtual ~_FACEExpression();
	void	_FACEExpression::set_name(LPSTR	pname);
	bool _FACEExpression::execute(float	adur, float dweight=1.0);
	bool _FACEExpression::remove(float	adur);

	void _FACEExpression::init(CMpeg4FDP* face);
	bool _FACEExpression::update(float deltaTime);

	_FACEExpression();
	_FACEExpression(const _FACEExpression& other);
} FACEExpression;

typedef struct _FACEExpressionsPack
{
	//Methods	
	virtual ~_FACEExpressionsPack();
	void add_expression(FACEExpression* pe);
	// Variables
	MP_VECTOR<FACEExpression*>	pexpressions;
	int	count;

	_FACEExpressionsPack();
	_FACEExpressionsPack(const _FACEExpressionsPack& other);
} FACEExpressionsPack;

/*
 *	Definition of emotions array. Used in Mixer to execute emotion from block, such as
 *		execute 5 emotion from "Happy" pack
 */
typedef struct _EM_PACK {
	_FACEExpression**	emotions;
	int					count;
} EM_PACK;

class CAL3D_API CMpeg4FDP  
{
public:
	CMpeg4FDP();
	virtual ~CMpeg4FDP();

public:
	bool initialize(CalModel* pmodel);
	void remove_all_emotions();
	FACEExpression* get_expression(FACEExpression* fe);
	void apply_fap(fap_on_face* fap);
	void update(float dur);
	void execute(FACEExpression* fe, float dur=1.0, float dweight=1.0);
	void remove(FACEExpression* fe, float dur=1.0);
	void apply_fap(FACEExpression* exp);
	void relax();
	void init_fap_params(fap_on_face* fap);
	control_point* getCP(int g, int i);
	void raise_cur_faps(float k);
	void clear();
	bool create_influences(int istep = 0);
	void apply_fap(int id, float dur);
	void calculate_FAPU(CalCoreMesh *pmesh);
	void calculate_FAPU(CalModel *pmodel);
#ifndef FPU_DEBUG
	bool	add_control_point(int meshid, int submeshid, int id, DWORD color);
#else
	bool	add_control_point(int meshid, int submeshid, int id, int maxid, DWORD color);
#endif
	bool	remove_control_point(int agroup, int asubgroup);

	/*
	IRISD0 = 3.1.y - 3.3.y = 3.2.y - 3.4.y
	IRIS Diameter (by definition it is equal to the distance
	between upper and lower eyelid) in neutral face
	IRISD = IRISD0 / 1024
	ES0 = 3.5.x - 3.6.x Eye Separation ES = ES0 / 1024
	ENS0 = 3.5.y - 9.15.y Eye - Nose Separation ENS = ENS0 / 1024
	MNS0 = 9.15.y - 2.2.y Mouth - Nose Separation MNS = MNS0 / 1024
	MW0 = 8.3.x - 8.4.x Mouth - Width Separation MW = MW0 / 1024
	AU Angular Unit 10-5 rad
	*/
	float IRISD0,ES0,ENS0,MNS0,MW0,AU;
	control_point*			n_points[MAX_CONTROL_GROUP][MAX_CONTROL_POINTS_INGROUP];	// позиции точек в нейтральном положении

	int		cp_num;	// количество контрольных точек
	MP_VECTOR<fap_on_face>	cur_faps;

	MP_VECTOR<submesh_face_point>	infl_array;
	int								lastindex;

	MP_VECTOR<submesh_face_point>	infl_points;	// передвигаемые точки
	LPVOID							pBone;

	MP_VECTOR<submesh_face_point>	control_points;
	bool	is_control_point(int mid, int smid, int id);

	// Runtime
	MP_VECTOR<FACEExpression*>		expressions;
	MP_VECTOR<control_point*>			cur_cpoints;
	void include_cpoint(control_point*	cp);

//#ifdef CAL3D_EXPORTER
	void	savelog();
//#endif
};

#endif // !defined(AFX_MPEG4FDP_H__4AA98B71_6B29_48A1_8B5B_2D5A0F85AC23__INCLUDED_)
