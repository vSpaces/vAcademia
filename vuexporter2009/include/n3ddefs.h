#if !defined(TERRDEFS)
#define TERRDEFS

#include "../Cal3D_DX8/src/vector.h"
#include "../Cal3D_DX8/src/quaternion.h"

//#define DIRECT3D_VERSION         0x0900

#include <algorithm>
#include "n3dbase.h"
#include "n3dmath3d.h"

class CShader;
class CVisibleObject;

using namespace natura3d;

typedef struct _TERVERT{
	D3DXVECTOR3		pos;	// coords
	D3DXVECTOR3		normal;	// coords
	DWORD	diffuse;
	FLOAT			tu, tv; // Vertex texture coordinates
	FLOAT			tu1, tv1; // Vertex texture coordinates
	//
} TERVERT;

typedef struct _LINEVERT{
	D3DXVECTOR3		pos;	// coords
	DWORD	diffuse;
	//
	_LINEVERT(){};
	_LINEVERT(D3DXVECTOR3 apos, DWORD adif){ diffuse = adif; pos = apos;}
} LINEVERT;

#define D3DFVF_TERVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2)
#define D3DFVF_LINEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

typedef struct _TERRAIN{
	//
	int		dwWidth;
	int		dwLength;
	//
	DWORD	dwMinHeight;
	DWORD	dwMaxHeight;
	//
	float	hillness;
	//
	float	dwWidthSubdivs;
	float	dwLengthSubdivs;
	//
} TERRAIN;

typedef struct _TERDESC{
	TERVERT*		m_pData;
	TERRAIN			desc;	// coords
} TERDESC;

typedef struct _LODLEVEL{
	DWORD		dwLod;
	float		fLen;
	D3DXVECTOR3	min;
	D3DXVECTOR3	max;
	DWORD		id;
	//
	_LODLEVEL::_LODLEVEL(DWORD adwlod=2, float aflen=0.0){
		dwLod = adwlod;
		fLen = aflen;
	}
	//	
} LODLEVEL;


typedef struct _INDDESC{
	LPDIRECT3DINDEXBUFFER8	m_pIVB;
	DWORD		dwIndCount;
	DWORD		dwTriCount;
	DWORD		dwVertCount;
	LODLEVEL	lod;
} INDDESC;

TERVERT* GetPoint(TERDESC	data, int x, int y);
float Get2Vec3Length( D3DXVECTOR3* v1, D3DXVECTOR3* v2);

typedef struct NATURA3D_API _plane_t{ 
	CalVector normal; // нормаль к плоскости (A, B, C) 
	float dist; // коэфициент D 
	_plane_t::_plane_t(){}
	_plane_t::~_plane_t(){}
} plane_t; 

inline float fLength( D3DXVECTOR3 normal){
	return sqrt( normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
};

/////////////////////////////////////////////////////////////////////////
// трансформации объектов
/////////////////////////////////////////////////////////////////////////
typedef struct NATURA3D_API _TRANSFORM3D{
	static CalVector	trUP;
	static CalVector	trFACE;
	static CalVector	trLEFT;

	CalVector		pos;
	CalQuaternion	rtn;
	CalVector		scl;
	CalVector		local_scl;
	//
	_TRANSFORM3D::_TRANSFORM3D(){
		pos.set(0,0,0);
		rtn=CalQuaternion( CalVector(0,0,0), 1);
		scl.set(1,1,1);
		local_scl.set(1,1,1);
	}

	_TRANSFORM3D::_TRANSFORM3D(const CalVector&		apos,
								const CalQuaternion&	artn,
								const CalVector&		ascl,
								const CalVector&		alocal_scl)
	{
		pos = apos;
		rtn=artn;
		scl = ascl;
		local_scl = alocal_scl;
	}

} TRANSFORM3D;

typedef struct _LIGHTCOLOR{
	float r;
    float g;
    float b;
    float a;
	_LIGHTCOLOR::_LIGHTCOLOR( float _r, float _g, float _b, float _a){
		r=_r; g=_g; b=_b; a=_a;
	}
} LIGHTCOLOR;

#define LCOLOR(r,g,b,a) *((D3DCOLORVALUE*)&LIGHTCOLOR(r,g,b,a))

// New definitions
typedef float		myfloat;
typedef DWORD		mydword;
typedef WORD		myword;
typedef int			myint;

struct myvec3d: public D3DVECTOR
{
	void operator=(const CalVector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
};
//typedef CalVector	myvec3d;

/*////////////////////////////////////////////////
//	Common 4d vertex for computing
*/////////////////////////////////////////////////
typedef	struct _vertex4d{
	float x;
	float y;
	float z;
	float w;


	_vertex4d::_vertex4d(){x=y=z=0.0; w=1.0;}
	_vertex4d::_vertex4d(CalVector v){x=v.x; y=v.y; z=v.z; w=1.0;}


	_vertex4d _vertex4d::transform(D3DMATRIX& m)
	{
		_vertex4d ret;
		ret.x = x*m._11+y*m._21+z*m._31+w*m._41;
		ret.y = x*m._12+y*m._22+z*m._32+w*m._42;
		ret.z = x*m._13+y*m._23+z*m._33+w*m._43;
		ret.w = x*m._14+y*m._24+z*m._34+w*m._44;
		x=ret.x;	y=ret.y;	z=ret.z;	w=ret.w;
		return ret;
	}

	_vertex4d _vertex4d::transform(D3DXMATRIX& m)
	{
		return transform((D3DMATRIX)m);
	}

} vertex4d;

/*////////////////////////////////////////////////
//	Common 3d vertex for render
*/////////////////////////////////////////////////
typedef	struct _vertex3drnd{
	myvec3d			pos;		// position
	myvec3d			normal;		// normal
	mydword			diffuse;	// color
	myfloat			tu, tv;		// texture coordinates

	_vertex3drnd(){}
	_vertex3drnd(float px, float py, float pz, float nx, float ny, float nz, DWORD adiff, float atu, float atv)
	{
		pos.x = px;	pos.y = py;	pos.z = pz;
		normal.x = nx;	normal.y = ny;	normal.z = nz;
		diffuse = adiff;
		tu = atu; tv = atv;
	}
} vertex3drnd;
/*////////////////////////////////////////////////
//	Shader for common 3d vertex
*/////////////////////////////////////////////////
#define	vertex3dsh	D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1

/*////////////////////////////////////////////////
//	Vertex for stencil shadows
*/////////////////////////////////////////////////
typedef	struct _vertexstencilshadow{
	CalVector		pos;		// position
	mydword			diffuse;	// color
	_vertexstencilshadow(){}
	_vertexstencilshadow(float px, float py, float pz, DWORD adiff)
	{
		pos.x = px;	pos.y = py;	pos.z = pz;
		diffuse = adiff;
	}
} vertexstencilshadow;
/*////////////////////////////////////////////////
//	Shader for stencil shadow vertex
*/////////////////////////////////////////////////
#define	vertexstencilshadowsh	D3DFVF_XYZ | D3DFVF_DIFFUSE

typedef	struct _commonvertex3d{
	CalVector		pos;		// position
	CalVector		normal;		// normal
	myfloat			rwh;
	mydword			diffuse;	// color
	myfloat			tu, tv;		// texture coordinates
	myfloat			tu2, tv2;		// texture coordinates

	BOOL _commonvertex3d::operator==(const _commonvertex3d& f){	
		return (pos==f.pos&&normal==f.normal&&rwh==f.rwh&&diffuse==f.diffuse&&
			tu==f.tu&&tv==f.tv&&tu2==f.tu2&&tv2==f.tv2)?true:false;
	}
} commonvertex3d;

/*////////////////////////////////////////////////
//	Common 3d vertex for render
*/////////////////////////////////////////////////
typedef	struct _vertex3drndlm
{
	myvec3d			pos;		// position
	myvec3d			normal;		// normal
	myfloat			tu, tv;		// texture coordinates
	myfloat			tu2, tv2;	// texture coordinates
} vertex3drndlm;

/*////////////////////////////////////////////////
//	Shader for common 3d vertex with light map
*/////////////////////////////////////////////////
#define	vertex3dlmsh	D3DFVF_XYZ | D3DFVF_NORMAL | /*D3DFVF_DIFFUSE | */\
						D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1)

/*////////////////////////////////////////////////
//	Common 3d vertex for 3dobjects
*/////////////////////////////////////////////////
typedef	struct _vertex3dunlit{
	myvec3d			pos;		// position
	myvec3d			normal;		// normal
	myfloat			tu, tv;		// texture coordinates
} vertex3dunlit;

/*////////////////////////////////////////////////
//	Shader for common 3d unlit vertex
*/////////////////////////////////////////////////
#define	vertex3dunlitsh	D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1

/*////////////////////////////////////////////////
//	Common 2d vertex for render
*/////////////////////////////////////////////////
typedef	struct _vertex2drnd{
	myfloat			x;			// x position
	myfloat			y;			// y position
	myfloat			z;			// y position
	myfloat			rwh;		// y position
	mydword			diffuse;	// color
	myfloat			tu, tv;		// texture coordinates
	//
	_vertex2drnd()
	{
		x = 0.0f;	y = 0.0f;	z = 0.0f;	diffuse = 0xFFFFFFFF;	tu = 0.0f;	tv = 0.0f;	rwh = 1.0f;
	}
	_vertex2drnd(myfloat ax, myfloat ay, myfloat az, mydword adiff, myfloat atu, myfloat atv)
	{
		x = ax;	y = ay;	z = az;	diffuse = adiff;	tu = atu;	tv = atv;	rwh = 1.0f;
	}
} vertex2drnd;

/*////////////////////////////////////////////////
//	Shader for common 2d vertex
*/////////////////////////////////////////////////
#define	vertex2dspr	D3DFVF_XYZRHW  | D3DFVF_DIFFUSE | D3DFVF_TEX1

/*////////////////////////////////////////////////
//	Common 2d vertex for render
*/////////////////////////////////////////////////
typedef	struct _vertex2dwithmask{
	myfloat			x;			// x position
	myfloat			y;			// y position
	myfloat			z;			// y position
	myfloat			rwh;		// y position
	mydword			diffuse;	// color
	myfloat			tu, tv;		// texture coordinates
	myfloat			tu2, tv2;		// 2nd texture coordinates
	//
	_vertex2dwithmask()
	{
		x = 0.0f;	y = 0.0f;	z = 0.0f;	diffuse = 0xFFFFFFFF;	tu = 0.0f;	tv = 0.0f;	tu2 = 0.0f;	tv2 = 0.0f;	rwh = 1.0f;
	}
	_vertex2dwithmask(myfloat ax, myfloat ay, myfloat az, mydword adiff, myfloat atu, myfloat atv, myfloat atu2, myfloat atv2)
	{
		x = ax;	y = ay;	z = az;	diffuse = adiff;	tu = atu;	tv = atv;	tu2 = atu2;	tv2 = atv2;	rwh = 1.0f;
	}
} vertex2dwithmask;

/*////////////////////////////////////////////////
//	Shader for 2d vertex with 2 maps
*/////////////////////////////////////////////////
#define	vertex2dwithmasksh	D3DFVF_XYZRHW  | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1)

/*////////////////////////////////////////////////
//	Common 3d vertex for operations
*/////////////////////////////////////////////////
typedef	struct _vertex3d{
	CalVector	pos;
	CalVector	normal;
	mydword		diffuse;
	myfloat		tu, tv;
	//
	_vertex3d::_vertex3d(){}
	_vertex3d::~_vertex3d(){}
	_vertex3d::_vertex3d(const CalVector& apos, const CalVector& anorm=CalVector(0,1,0), DWORD adiff=D3DCOLOR_ARGB(255,255,255,255), float atu=0.0, float atv=0.0){
		pos=apos; normal=anorm;
		diffuse=adiff;
		tu=atu; tv=atv;
	}
	void _vertex3d::fill(commonvertex3d& tofill){
		//memcpy( (void*)&tofill, (LPVOID)this, sizeof(vertex3drnd));
		tofill.pos = pos;
		tofill.normal = normal;
		tofill.diffuse = diffuse;
		tofill.tu = tu;
		tofill.tv = tv;
	}
	void _vertex3d::fill(commonvertex3d* tofill){
		tofill->pos = pos;
		tofill->normal = normal;
		tofill->diffuse = diffuse;
		tofill->tu = tu;
		tofill->tv = tv;
	}
	void _vertex3d::operator=(const _vertex3d& v){
		pos = v.pos;
		normal = v.normal;
		diffuse = v.diffuse;
		//tu = v.tu;
		//tv = v.tv;
	}
} vertex3d;


//////////////////////////////////////////////////////////////////////////
// Size
//////////////////////////////////////////////////////////////////////////
typedef struct _n3dSize
{
	static	_n3dSize	Empty;
	// Members
	int	width;
	int	height;
	// Constructors
	_n3dSize(){};
	_n3dSize(int aw, int ah)
	{
		width = aw;
		height = ah;
	};
	_n3dSize(const _n3dSize& instance)
	{
		width = instance.width;
		height = instance.height;
	};
} n3dSize;

/*////////////////////////////////////////////////
//	Triangle face
*/////////////////////////////////////////////////
typedef struct NATURA3D_API _triangle3d{
	myword			verts[3];
	commonvertex3d*	vertices[3];
	CalVector		normal;
	CalVector		base;
	CalVector		en[3];
	myint			tid[2];
	myint			mid;		// id материала
	float			dist;
	int				shid;		// shader's id
	int				submesh_id;	// submesh id
	DWORD			type;

	_triangle3d::_triangle3d();

	void _triangle3d::calc_normal(const CalVector& v1, const CalVector& v2, const CalVector& v3);
	void _triangle3d::get_bounds(CalVector& min, CalVector& max) const;
	void _triangle3d::calc_normal();
	void _triangle3d::calc_base(CalVector v1, CalVector v2, CalVector v3);
	void _triangle3d::calc_en(const CalVector& v1, const CalVector& v2, const CalVector& v3);
	void _triangle3d::calc_en();
	void _triangle3d::init_params(commonvertex3d* averts);
	BOOL _triangle3d::operator<(const _triangle3d& f);
	BOOL _triangle3d::operator>(const _triangle3d& f);
	BOOL _triangle3d::operator==(const _triangle3d& f);
	int _triangle3d::ray_intersect(const CalVector &ro, const CalVector &rd, CalVector &ip, float &adist);

}	triangle3d;

/*////////////////////////////////////////////////
//	2D box with collinear edges to world axes edges
*/////////////////////////////////////////////////
typedef struct _point2d{
	float x;
	float y;
	_point2d::_point2d(){}
	_point2d::_point2d(float ax, float ay){x=ax; y=ay;}
	//
	void _point2d::operator=(const _point2d& p){ x = p.x; y = p.y;}
	_point2d _point2d::operator+(const _point2d& p){ return _point2d(x+p.x, y+p.y);}
	_point2d _point2d::operator-(const _point2d& p){ return _point2d(x-p.x, y-p.y);}
	_point2d _point2d::operator-=(const _point2d& p){ return _point2d(x-p.x, y-p.y);}
	_point2d _point2d::operator+=(const _point2d& p){ return _point2d(x+p.x, y+p.y);}
	_point2d _point2d::operator/(const float p){ return _point2d(x/p, y/p);}
	_point2d _point2d::operator/=(const float p){ return _point2d(x/p, y/p);}
	_point2d _point2d::operator*(const float p){ return _point2d(x*p, y*p);}
	_point2d _point2d::operator*=(const float p){ return _point2d(x*p, y*p);}
	bool	operator==(const _point2d& point){ return point.x==x&&point.y==y;}
	operator POINT(){ POINT pnt; pnt.x = x; pnt.y = y; return pnt;}
} point2d;

/*////////////////////////////////////////////////
//	2D box with collinear edges to world axes edges
*/////////////////////////////////////////////////
typedef struct _box2d{
	point2d	min;
	point2d	max;
	point2d	verts[4];

	_box2d::_box2d(){}
	_box2d::_box2d(point2d amin, point2d amax){min=amin; max=amax;}
	void _box2d::set(point2d amin, point2d amax){min=amin; max=amax;}
	float _box2d::width(){return max.x-min.x;}
	float _box2d::height(){return max.y-min.y;}
	float _box2d::maxdim(){
		float w=width();
		float l=height();
		return w>l?w:l;
	}
	void _box2d::fill_verts(){
		verts[0] = min;	
		verts[1] = min;	verts[1].y = max.y;
		verts[2] = max;
		verts[3] = max;	verts[3].x = min.y;
	}
	bool _box2d::intersect(_box2d box){
		fill_verts();
		int i=0;
		for(i=0; i < 4; i++)	if( box.contain(verts[i])) return true;
		box.fill_verts();
		for(i=0; i < 4; i++)	if( contain(box.verts[i])) return true;
		return false;
	}
	point2d _box2d::center(){	return (min+max)/2;}
	bool _box2d::contain(point2d v){ 
		return !(v.x>max.x||v.x<min.x||v.y>max.y||v.y<min.y);
	}
} box2d;

/*////////////////////////////////////////////////
//	3D box with collinear edges to world axes edges
*/////////////////////////////////////////////////
typedef struct NATURA3D_API _box3d{
	CalVector	min;
	CalVector	max;
	CalVector	verts[8];
	static CalVector	normals[8];
	static CalVector	facenorm[6];
	static CalVector	edgenorm[12];
	static int facevert[6][4];
	static int edgevert[12][2];
	static CalVector edgedir[12];
	static CalVector edgedirth[12];

	_box3d::_box3d(){}
	_box3d::_box3d(const CalVector& amin, const CalVector amax){min=amin; max=amax;}
	void _box3d::set(const CalVector& amin, const CalVector& amax){min=amin; max=amax;}
	void _box3d::add(CalVector& v){min.minimize(v); max.maximize(v);}
	float _box3d::width(){return max.x-min.x;}
	float _box3d::length(){return max.z-min.z;}
	float _box3d::height(){return max.y-min.y;}
	float _box3d::maxdim() const{
		float w=max.x - min.x;
		float l=max.y - min.y;
		float h=max.z - min.z;
		if( w>=l && w>=h) return w;
		if( l>=w && l>=h) return l;
		return h;
	}
	float _box3d::maxdimXZ(){
		float w=width();
		float l=max.z-min.z;
		return w>l?w:l;
	}
	void _box3d::fill_verts();
	bool _box3d::intersect(const _box3d& box) const;

	bool _box3d::contain(const _box3d& box) const{
		/*for( int i=0; i<3; i++)
			if( min[i]>box.min[i] || max[i]<box.max[i])	return false;
		return true;*/
		if( min.x > box.min.x || max.x < box.max.x)	return false;
		if( min.y > box.min.y || max.y < box.max.y)	return false;
		if( min.z > box.min.z || max.z < box.max.z)	return false;
		return true;
	}

	CalVector _box3d::center() const
	{	return (min+max)/2;}
	bool _box3d::contain(const CalVector& v) const
	{ return !(v.x>max.x||v.x<min.x||v.y>max.y||v.y<min.y||v.z>max.z||v.z<min.z);}
	box2d _box3d::projectionXY(){ return box2d(point2d(min.x, min.y),point2d(max.x, max.y));}
	box2d _box3d::projectionXZ(){ return box2d(point2d(min.x, min.z),point2d(max.x, max.z));}
	box2d _box3d::projectionYZ(){ return box2d(point2d(min.y, min.z),point2d(max.y, max.z));}
	CalVector	_box3d::get_vert(int id){
		switch(id){
		case 0: return min; break;
		case 1: return CalVector( min.x, max.y, min.z); break;
		case 2: return CalVector( max.x, max.y, min.z); break;
		case 3: return CalVector( max.x, min.y, min.z); break;
		case 4: return CalVector( min.x, min.y, min.z); break;
		case 5: return CalVector( min.x, max.y, max.z); break;
		case 6: return max; break;
		case 7: return CalVector( max.x, min.y, min.z); break;
		}
		ATLASSERT( FALSE);
		return min; 
	}
	_box3d _box3d::get_portion(int id){
		switch(id){
		case 1: return _box3d(min, center()); break;
		case 2: 
			{
				CalVector	p1 = min;	p1.y = center().y;
				CalVector	p2 = center(); p2.y = max.y;
				return _box3d(p1, p2); 
			}break;
		case 3: 
			{
				CalVector	p1 = center();	p1.z = min.z;
				CalVector	p2 = max;		p2.z = center().z;
				return _box3d(p1, p2); 
			}break;
		case 4: 
			{
				CalVector	p1 = min;	 p1.x = center().x;
				CalVector	p2 = center(); p2.x = max.x;
				return _box3d(p1, p2); 
			}break;
		case 5:
			{
				CalVector	p1 = min;	 p1.z = center().z;
				CalVector	p2 = center(); p2.z = max.z;
				return _box3d(p1, p2); 
			}break;
		case 6: 
			{
				CalVector	p1 = center();	 p1.x = min.x;
				CalVector	p2 = max;		 p2.x = center().x;
				return _box3d(p1, p2); 
			}break;
		case 7: 
			{
				CalVector	p1 = center();
				CalVector	p2 = max;
				return _box3d(p1, p2); 
			}break;
		case 8: 
			{
				CalVector	p1 = center();	 p1.y = min.y;
				CalVector	p2 = max;		 p2.y = center().y;
				return _box3d(p1, p2); 
			}break;
		}
		ATLASSERT( FALSE);
		return _box3d(min, max); 
	}
	_box3d _box3d::append(const _box3d& box)
	{
		min.minimize(box.min);
		max.maximize(box.max);
		return *this;
	}
	_box3d _box3d::init(const CalVector& center, float dim)
	{
		min = center - dim;
		max = center + dim;
		return *this;
	}
	_box3d& _box3d::init(const CalVector& center, const CalVector& dim)
	{
		min = center - dim;
		max = center + dim;
		return *this;
	}
	_box3d& _box3d::init(CalVector* pverts, int count)
	{
		if( count == 0)	return *this;
		min = pverts[0];
		max = pverts[0];
		for( int i=0; i<8; i++)
		{
			if(pverts->x>max.x)	max.x = pverts->x;	if(pverts->x<min.x)	min.x = pverts->x;
			if(pverts->y>max.y)	max.y = pverts->y;	if(pverts->y<min.y)	min.y = pverts->y;
			if(pverts->z>max.z)	max.z = pverts->z;	if(pverts->z<min.z)	min.z = pverts->z;
			pverts++;
		}
		return *this;
	}
	void _box3d::align()
	{
		min = verts[0];
		max = verts[0];
		for( int i=0; i<8; i++)
		{
			CalVector& v1 = verts[i];
			if(v1.x>max.x)	max.x = v1.x;	if(v1.x<min.x)	min.x = v1.x;
			if(v1.y>max.y)	max.y = v1.y;	if(v1.y<min.y)	min.y = v1.y;
			if(v1.z>max.z)	max.z = v1.z;	if(v1.z<min.z)	min.z = v1.z;
		}
		fill_verts();
	}
	_box3d _box3d::transform(const CalQuaternion& q, const CalVector& v){
		fill_verts();
		min.set(100000000.0f, 100000000.0f, 100000000.0f);
		max.set(-100000000.0f, -100000000.0f, -100000000.0f);
		for(int i=0; i<8; i++)
		{
			verts[i] *= q;
			verts[i] += v;
		
			CalVector& v1 = verts[i];
			if(v1.x>max.x)	max.x = v1.x;	if(v1.x<min.x)	min.x = v1.x;
			if(v1.y>max.y)	max.y = v1.y;	if(v1.y<min.y)	min.y = v1.y;
			if(v1.z>max.z)	max.z = v1.z;	if(v1.z<min.z)	min.z = v1.z;
		}
		fill_verts();
		return *this;
	}
	_box3d _box3d::transform(const CalQuaternion& q, const CalVector& v, const CalVector& s){
		fill_verts();
		min.set(100000000.0f, 100000000.0f, 100000000.0f);
		max.set(-100000000.0f, -100000000.0f, -100000000.0f);
		for(int i=0; i<8; i++)
		{
			verts[i] *= q;
			verts[i][0] *= s[0];	verts[i][1] *= s[1];	verts[i][2] *= s[2];
			verts[i] += v;

			CalVector& v1 = verts[i];
			if(v1.x>max.x)	max.x = v1.x;	if(v1.x<min.x)	min.x = v1.x;
			if(v1.y>max.y)	max.y = v1.y;	if(v1.y<min.y)	min.y = v1.y;
			if(v1.z>max.z)	max.z = v1.z;	if(v1.z<min.z)	min.z = v1.z;
		}
		fill_verts();
		return *this;
	}
	_box3d _box3d::transform(const D3DXMATRIX& m){
		fill_verts();
		min.set(100000000.0f, 100000000.0f, 100000000.0f);
		max.set(-100000000.0f, -100000000.0f, -100000000.0f);
		for(int i=0; i<8; i++)
		{
			verts[i].transform((D3DMATRIX*)&m);

			CalVector& v1 = verts[i];
			if(v1.x>max.x)	max.x = v1.x;	if(v1.x<min.x)	min.x = v1.x;
			if(v1.y>max.y)	max.y = v1.y;	if(v1.y<min.y)	min.y = v1.y;
			if(v1.z>max.z)	max.z = v1.z;	if(v1.z<min.z)	min.z = v1.z;
		}
		fill_verts();		
		return *this;
	}
	void _box3d::make_max_box(bool ax=true, bool ay=true, bool az=true){
		float md=maxdim();
		CalVector dims;
		dims.x = ax?md/2.0f:(max.x-min.x)/2;
		dims.y = ay?md/2.0f:(max.y-min.y)/2;
		dims.z = az?md/2.0f:(max.z-min.z)/2;
		init( center(), dims);
	}
	void _box3d::set_proj(bool ax=true, bool ay=true, bool az=true, box2d proj=box2d()){
		assert(!(ax&&ay&&az));
		CalVector cnt=center();
		point2d dim = (proj.max - proj.min)/2;
		int i1 = 0, i2 = 0;
		if( ax&&ay){ i1=0; i2=1;}
		if( ax&&az){ i1=1; i2=2;}
		if( ay&&az){ i1=2; i2=0;}
		min[i1] = cnt[i1] - dim.x;	max[i1] = cnt[i1] + dim.x;
		min[i2] = cnt[i2] - dim.y;	max[i2] = cnt[i2] + dim.y;
	}
	static	int _box3d::edge_collision(CalVector& p1,CalVector& p2,CalVector& dir,CalVector& p3,CalVector& p4,float& dist,CalVector& ip);
	int _box3d::ray_intersect(const CalVector &ro, const CalVector &rd, float &tnear, float &tfar) const;
	int _box3d::ray_intersect(const CalVector &ro, const CalVector &rd, CalVector &ip, CalVector &resp) const;
	int _box3d::collide(_box3d& bbox,CalVector& dir,float& len,CalVector& normal,CalVector& ip) const;
	//int box3d::collide(CVisibleObject* obj,CalVector& dir,float& len,CalVector& normal,CalVector& ip);
	int _box3d::collide(std::vector<triangle3d>& faces,CalVector& dir,float& len,CalVector& normal,CalVector& ip);
	_box3d _box3d::operator+(const CalVector& f){	return _box3d(min+f, max+f);}
	_box3d _box3d::operator-(const CalVector& f){	return _box3d(min-f, max-f);}
	void _box3d::operator+=(const CalVector& f){	min+=f, max+=f;}
	void _box3d::operator-=(const CalVector& f){	min-=f, max-=f;}
	BOOL _box3d::operator==(const _box3d& f){ return max.x!=f.max.x||max.y!=f.max.y||max.z!=f.max.z||\
		min.x!=f.min.x||min.y!=f.min.y||min.z!=f.min.z?false:true;}
	BOOL _box3d::operator!=(const _box3d& f){ return max.x!=f.max.x||max.y!=f.max.y||max.z!=f.max.z||\
		min.x!=f.min.x||min.y!=f.min.y||min.z!=f.min.z?true:false;}
	BOOL	_box3d::visible_from(const CalVector& v, const _box3d& b)  const;
	static _box3d	_box3d::temporary(_box3d src, CalVector dir, float len)
	{
		_box3d	temp_pos = src;
		temp_pos.min += dir*len;
		temp_pos.max += dir*len;
		temp_pos.append(src);
		return temp_pos;
	}
	bool	is_flat(){ return (max.z-min.z)<0.1f || (max.y-min.y)<0.1f || (max.x-min.x)<0.1f;}

	friend	NATURA3D_API _box3d operator+(const _box3d& bx, const CalVector& f);
} box3d;

#define	MINIMIZEVEC3D( v1, v2)	if( v2.x < v1.x)	v1.x = v2.x;	\
									if( v2.y < v1.y)	v1.y = v2.y;	\
									if( v2.z < v1.z)	v1.z = v2.z;

#define	MAXIMIZEVEC3D( v1, v2)	if( v2.x > v1.x)	v1.x = v2.x;	\
									if( v2.y > v1.y)	v1.y = v2.y;	\
									if( v2.z > v1.z)	v1.z = v2.z;

#define GRAB_POINTER(x){ x->AddRef();}

#define RELEASE_POINTER(x){ if(x) x->Release();}

/*////////////////////////////////////////////////
//	Polygon face
*/////////////////////////////////////////////////
struct _plane3d;

typedef struct _polygon3d{
	std::vector<CalVector>	vertices;
	std::vector<_plane3d>		edge_planes;
	CalVector		normal;
	float		dist;

	_polygon3d::_polygon3d(){
		normal.set(0,0,0);
		dist = 0;
	}

	_polygon3d::_polygon3d(_plane3d& plane);

	_polygon3d::_polygon3d(const _polygon3d& p);

	void clear()
	{
		vertices.clear();
		edge_planes.clear();
	}

	_polygon3d::~_polygon3d();

	// add new vertex pointer to polygon
	void add_vert(CalVector vert){ vertices.push_back(vert);}
	
	// compute all edges planes
	void compute_edges();

	CalVector	center()
	{
		CalVector	pos(0,0,0);
		std::vector<CalVector>::iterator	it;
		for(it=vertices.begin(); it!=vertices.end(); it++)
			pos += *it;
		pos /= vertices.size();
		return pos;
	};

	// for future changes of VERTICES type
	CalVector	get_vpos(int idx){ return vertices[idx];}

	void swap_verts(int i, int j)
	{
		CalVector	pv = vertices[i];
		vertices[i] = vertices[j];
		vertices[j] = pv;
	}

	bool coplanar(_polygon3d* p)
	{
		float dot = normal.dot(p->normal);
		if( dot>COPLANAR_PL_DOTm1 && (float)fabs(dist-p->dist) < COPLANAR_PL_DIST)
			return true;
		return false;
	}

	bool in_same_plane(_polygon3d* p)
	{
		float dot = normal.dot(p->normal);
		if( dot>COPLANAR_PL_DOTm1 && (float)fabs(dist-p->dist) < COPLANAR_PL_DIST)
			return true;
		if( dot<-COPLANAR_PL_DOTm1 && (float)fabs(dist+p->dist) < COPLANAR_PL_DIST)
			return true;
		return false;
	}

	int intersect(_polygon3d& p1, _polygon3d& pres);
	int intersect_verts(_polygon3d& p1, _polygon3d& pres);
	int intersect_edges(_polygon3d& p1, _polygon3d& pres);
	bool coplanar(_polygon3d& p){ return coplanar(&p);}
	void order_verts();	
	bool is_null(){ return vertices.size()>0?false:true;}
	box3d	get_bounds(){
		box3d ret;
		assert(vertices.size()>0);
		ret.init(get_vpos(0), 0.0f);
		for(unsigned int i=1; i<vertices.size(); i++)
			ret.add(get_vpos(i));
		return ret;
	}

	float distance(CalVector& v)
	{ return normal.dot(v)-dist; }

	CalVector	project(CalVector v)
	{
		float d = distance(v);
		CalVector	in = normal;
		in.invert();
		v -= in*d;
		return v;
	}

	bool	contain(CalVector& v);

	CalVector get_inside_point();
}	polygon3d;

/*////////////////////////////////////////////////
//	Overload of D3DCOLORVALUE struct
*/////////////////////////////////////////////////
typedef struct _myd3dcolor {
    float r;
    float g;
    float b;
    float a;
	_myd3dcolor::_myd3dcolor(){}
	_myd3dcolor::_myd3dcolor(float ar, float ag, float ab, float aa){
		r=ar; g=ag; b=ab; a=aa;
	}
	void _myd3dcolor::fill(D3DCOLORVALUE& val){ val.r=r; val.g=g; val.b=b; val.a=a;}
} myd3dcolor;



typedef struct _n3dresource{
	DWORD					usage;
	D3DPOOL					pool;
	
	_n3dresource(){usage=0/*D3DUSAGE_WRITEONLY*/; pool=D3DPOOL_MANAGED;}
} n3dresource;

/*////////////////////////////////////////////////
//	Vertex buffer description
*/////////////////////////////////////////////////
typedef struct _vbdesc : public n3dresource{
	LPDIRECT3DVERTEXBUFFER8 buffer;
	DWORD					vcount;
	DWORD					fvf;
	DWORD					stride;

	_vbdesc(){buffer=NULL; vcount=0; fvf=0; stride=0;}
} vbdesc;

/*////////////////////////////////////////////////
//	Texture description
*/////////////////////////////////////////////////
//enum	TXTYPE	{ TX_SIMPLE, TX_CUBEMAP, TX_EMPTY};

#define	TX_SIMPLE		1
#define	TX_CUBEMAP		2
#define	TX_EMPTY		4

//#define	TX_ANIMATION	8
//#define	TX_VIDEO		16

//typedef	DWORD TXTYPE;

/*////////////////////////////////////////////////
//	Index buffer description
*/////////////////////////////////////////////////
typedef struct _ibdesc
{
	LPDIRECT3DINDEXBUFFER8	buffer;				// pointer to data
	DWORD					vcount;				// indices count
	DWORD					index;				// index in renderer array
	DWORD					BaseVertexIndex;	// start index to drawing vertices
	DWORD					BaseIndicesIndex;	// index in big index array
	int						shaderID;
	//myint					tid[3];				// texture id
	//CShader*				shader;
	//int						mid;
	_ibdesc::_ibdesc()
	{
		//mid=-1;
		//tid[0]=-1;
		//tid[1]=-1;
		//tid[2]=-1;
		BaseVertexIndex=0;
		BaseIndicesIndex=0;
		shaderID = 0;
		//shader = NULL;
	}
	//void _ibdesc::set_material(int	id){mid=id;};
	//void	set_shader(CShader& psh);
	void	set_shader(int ashaderID){ shaderID = ashaderID;}
} ibdesc;

/*////////////////////////////////////////////////
//	Face array container
*/////////////////////////////////////////////////
/*typedef struct _facecontainer{
	enum	INIT_TYPE	{IT_OBJECTS, IT_POINTERS};
	DWORD						fcount;
	std::vector<triangle3d>*	pfaces;
	std::vector<triangle3d*>*	ppfaces;
	INIT_TYPE					type;
	//
	_facecontainer::_facecontainer(){}
	_facecontainer::_facecontainer(std::vector<triangle3d>*	apfaces){ pfaces=apfaces; type=IT_OBJECTS;}
	_facecontainer::_facecontainer(std::vector<triangle3d*>*	apfaces){ ppfaces=apfaces; type=IT_POINTERS;}
	void _facecontainer::init(std::vector<triangle3d>*	apfaces){ pfaces=apfaces; type=IT_OBJECTS;}
	void _facecontainer::init(std::vector<triangle3d*>*	apfaces){ ppfaces=apfaces; type=IT_POINTERS;}
	////////////////////////////////////////////////
	//	Get different faces count by texture id
	////////////////////////////////////////////////
	DWORD _facecontainer::get_arrays_count();

	////////////////////////////////////////////////
	//	Get faces count by specified texture id
	////////////////////////////////////////////////
	DWORD _facecontainer::get_face_count_by_tid(int tid, int mid, int tlid=-1, int shid=-1);

	////////////////////////////////////////////////
	//	Sorting faces array by texture id
	////////////////////////////////////////////////
	void _facecontainer::sort_by_id(DWORD artype);

} facecontainer;*/

/*//////////////////////////////////////////////////////////////
//	Struct for fast sorting <data> array with standart STL <algorithm>.sort
*///////////////////////////////////////////////////////////////
typedef	struct _sort_package{
	LPVOID	data;
	float	value;
	_sort_package::_sort_package(){}
	_sort_package::_sort_package(LPVOID apdata, float aval){ data=apdata; value=aval;}
	BOOL _sort_package::operator<(const _sort_package& f){	return value<f.value?true:false;}
	BOOL _sort_package::operator>(const _sort_package& f){	return value>f.value?true:false;}
	BOOL _sort_package::operator<=(const _sort_package& f){	return value<=f.value?true:false;}
	BOOL _sort_package::operator>=(const _sort_package& f){	return value>=f.value?true:false;}
} sort_package;

/*//////////////////////////////////////////////////////////////
//	Describing the one SUBMESH SKIN parameters of the CalModel
*///////////////////////////////////////////////////////////////
typedef struct _submesh_skin
{
	int	t_id;				// номер текстуры
	int	m_id;				// номер материала
	//
	_submesh_skin::_submesh_skin(){}
	_submesh_skin::_submesh_skin(int at_id, int am_id){ t_id=at_id; m_id=am_id;}
} submesh_skin;

/*//////////////////////////////////////////////////////////////
//	Plane definition
*///////////////////////////////////////////////////////////////
typedef struct _plane3d
{
	CalVector normal;	//!< Plane normal
	float d0;			//!< Perpendicular distance from the plane to the origin

	//! Default constructor
	_plane3d() : d0(0) 
	{ }
	
	//! Constructor from components
	_plane3d(CalVector n, float dist) : 
		normal(n),d0(dist)
	{ }

	//! Constructor from components
	_plane3d(CalVector n, CalVector v) : 
		normal(n)
	{
		d0 = normal.dot(v);
	}

	//! Constructor from components
	_plane3d(CalVector v1, CalVector v2, CalVector v3)
	{ 
		CalVector	e1, e2;
		e1 = v2 - v1;
		e2 = v3 - v1;
		normal = e1.cross(e2);
		normal.normalize();
		d0 = v1.dot(normal);
	}

	//! Copy-constructor
	_plane3d(const _plane3d& in) : normal(in.normal), d0(in.d0) 
	{ }

	//! Atribuition operator
	void operator=(const _plane3d& in) 
	{ 
		normal = in.normal;
		d0 = in.d0;
	}

	//! Compute the perpendicular distance from a point to the plane
	float distance(CalVector& v)
	{ return normal.dot(v)-d0; }

	//! Intersect a ray (ro,rd) with the plane and return intersection point (ip) and distance to intersection (dist)
	int ray_intersect(const CalVector& ro, const CalVector& rd,CalVector& ip,float& dist);

	bool contain(triangle3d*	face)
	{
		float dot=face->normal.dot(normal);
		float ds=d0-face->dist;
		FG_FPABS(ds);
		if( ds<COPLANAR_PL_DIST && dot > COPLANAR_PL_DOTm1)	return true;
		return false;
	}

	bool contain(triangle3d	face)
	{
		return contain(&face);
	}

	bool coplanar(_plane3d& p)
	{
		float dot = normal.dot(p.normal);
		if( dot>COPLANAR_PL_DOTm1 && (float)fabs(d0-p.d0) < COPLANAR_PL_DIST)
			return true;
		return false;
	}

	CalVector	project(CalVector v)
	{
		float d = distance(v);
		CalVector	in = normal;
		v -= in*d;
		return v;
	}

} plane3d;


/*template <STRIDE, SHADER>
typedef struct _vertex_format
{
	int	stride;
	int	shader;

	vertex_format::vertex_format()
	{
		stride = STRIDE;
		shader = SHADER;
	}
} vertex_format;*/

template<class T>
class graph_matrix{
	T*	edges;
	int		n, m;
public:
	// Constructors
	graph_matrix(){ edges=NULL;}

	graph_matrix(int i, int j){ edges=NULL; init(i,j);}

	~graph_matrix(){ if(edges) free( edges);}

	// Methods
	void init(int i, int j, T val)
	{
		n = i;	m = j;
		edges = (T*)calloc( 1, sizeof(T)*n*m);
		for(int ii=0; ii<i; ii++)
			for(int jj=0; jj<j; jj++)
				set(ii,jj,val);
	}

	T get(unsigned int i, unsigned int j)
	{
		return edges[i*n + j];
	}

	void set(unsigned int i, unsigned int j, T val)
	{
		edges[i*n + j] = val;
	}

	int get_cc(){ return m;}	// return col count
	int get_rc(){ return n;}	// return row count

};

typedef struct _PACKDATA
{
	LPVOID	data;
	int		w, h;
	int		pitch;
	D3DFORMAT format;
} PACKDATA;

/************************************************************************/
/* Привязка точки к системе координат
/*  anormal - нормаль к векторам FACE и SIDE 
/*  aplane - угол поворота вектора FACE 
/*  aup - угол поворота вектора FACE после трансформации (anormal, aplane)
/*			вокруг нормали к этому вектору и вектору UP
/************************************************************************/
typedef struct NATURA3D_API _POINT_LINK
{
	CalVector	local_point;

	//******************************
	_POINT_LINK()
	{
		local_point.set(0,0,0);
	}
	
	void		init(TRANSFORM3D	trn, CalVector point);
	CalVector	get_point(const TRANSFORM3D&	trn);

} POINT_LINK;

/************************************************************************/
/* Rectangle class
/************************************************************************/
typedef struct _rect2d : public tagRECT{
	// Methods
	_rect2d::_rect2d(){}
	_rect2d::_rect2d(int l, int t, int w, int h)
	{	left = l; top = t; right = l + w; bottom = t+h;}
	int	width(){ return right-left;}
	int	height(){ return bottom-top;}

	// CRect wrapper
	bool	PtInRect(point2d p)
	{
		return !(p.x>right || p.x<left ||p.y>bottom || p.y<top);
	}
	bool	PtInRect(POINT p)
	{
		return !(p.x>right || p.x<left ||p.y>bottom || p.y<top);
	}
	bool	operator==(const _rect2d& rect)
	{
		return rect.left==left&&rect.right==right&&rect.bottom==bottom&&rect.top==top;
	}
	point2d	CenterPoint() {return point2d(left+(right-left)/2, top+(bottom-top)/2);}
	int	Width(){ return width();}
	int	Height(){ return height();}
	bool IsRectNull(){ return ::IsRectEmpty(this);}
	void UnionRect(const _rect2d* r1, const _rect2d* r2){ ::UnionRect(this, r1, r2);}
	void IntersectRect(const _rect2d* r1, const _rect2d* r2){ ::IntersectRect(this, r1, r2);}
//	operator RECT() const { RECT rect={left,top,right,bottom}; return rect;}
} rect2d;

typedef struct NATURA3D_API _matrix3d: public D3DMATRIX
{
	// Constructors
	_matrix3d()
	{
		make_identity();
	}
	_matrix3d(D3DMATRIX m)
	{
		_11 = m._11;	_12 = m._12;	_13 = m._13;	_14 = m._14;
		_21 = m._21;	_22 = m._22;	_23 = m._23;	_24 = m._24;
		_31 = m._31;	_32 = m._32;	_33 = m._33;	_34 = m._34;
		_41 = m._41;	_42 = m._42;	_43 = m._43;	_44 = m._44;
	}
	_matrix3d(D3DXMATRIX m)
	{
		_11 = m._11;	_12 = m._12;	_13 = m._13;	_14 = m._14;
		_21 = m._21;	_22 = m._22;	_23 = m._23;	_24 = m._24;
		_31 = m._31;	_32 = m._32;	_33 = m._33;	_34 = m._34;
		_41 = m._41;	_42 = m._42;	_43 = m._43;	_44 = m._44;
	}
	// Methods
	_matrix3d	get_invert()
	{
		float det;
		matrix3d	minvert;
		D3DXMatrixInverse((D3DXMATRIX*)&minvert, &det, (D3DXMATRIX*)this);
		return minvert;
	}
	_matrix3d	make_identity()
	{
		_11 = 1;	_12 = 0;	_13 = 0;	_14 = 0;
		_21 = 0;	_22 = 1;	_23 = 0;	_24 = 0;
		_31 = 0;	_32 = 0;	_33 = 1;	_34 = 0;
		_41 = 0;	_42 = 0;	_43 = 0;	_44 = 1;
		return *this;
	}
	_matrix3d&	operator*=(const _matrix3d&	mult);
	_matrix3d&	translate(const CalVector&	trans, bool after = true);
	_matrix3d&	rotate(const CalQuaternion&	quat, bool after = true);
	_matrix3d&	scale(const CalVector&	scl, bool after = true);

	// Operators
	const _matrix3d& operator=(_matrix3d m)
	{
		memcpy(this, &m, sizeof(m));
		return *this;
	}
	operator D3DMATRIX() const
	{
		return *this;
	}
	operator D3DXMATRIX() const
	{
		return *this;
	}
} matrix3d;

/************************************************************************/
/* Point with float values
/************************************************************************/
typedef struct _FPOINT
{
	float	x, y;
	_FPOINT(){ x=0; y=0;}
	_FPOINT(float ax, float ay){ x=ax; y=ay;}
}	FPOINT;

/************************************************************************/
/* Size with float values
/************************************************************************/
typedef struct _SizeF
{
	float	width, height;
	_SizeF(){ width=0; height=0;}
	_SizeF(const _SizeF& instance){ width=instance.width; height=instance.height;}
	_SizeF&	operator=(const _SizeF& instance){ width=instance.width; height=instance.height; return *this;}
	_SizeF(float awidth, float aheight){ width=awidth; height=aheight;}
}	SizeF;

/************************************************************************/
/* Rectangle width float values
/************************************************************************/
typedef struct _RectangleF
{
	float	left, top, width, height;
	_RectangleF(){left=top=width=height=0;}
	_RectangleF(float aleft, float atop, float aw, float ah)
	{
		left=aleft; top=atop; width=aw; height=ah;
	}
	_RectangleF& operator=(const _RectangleF& instance){left=instance.left;top=instance.top;width=instance.width;height=instance.height;return *this;}
	_RectangleF(FPOINT p, SizeF sz)
	{
		left = p.x;
		top = p.y;
		width = sz.width;
		height = sz.height;
	}
	float	right(){	return left+width;}
	float	bottom(){	return top+height;}
}	RectangleF;


// --------------------- end of this header file ---------------------
#endif