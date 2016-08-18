
#include <math.h>

namespace rmmlplha{

typedef struct tagPOINT
{
    long  x;
    long  y;
} POINT, *PPOINT, *NPPOINT, *LPPOINT;

class CPoint : public tagPOINT
{
public:
// Constructors

	// create an uninitialized point
	CPoint();
	// create from two integers
	CPoint(int initX, int initY);
	// create from another point
	CPoint(POINT initPt);

// Operations

// translate the point
	void Offset(int xOffset, int yOffset);
	void Offset(POINT point);

	BOOL operator==(POINT point) const;
	BOOL operator!=(POINT point) const;
	void operator+=(POINT point);
	void operator-=(POINT point);

// Operators returning CPoint values
	CPoint operator-() const;
	CPoint operator+(POINT point) const;

	CPoint operator-(POINT point) const;
};

// CPoint
__forceinline CPoint::CPoint()
	{ /* random filled */ }
__forceinline CPoint::CPoint(int initX, int initY)
	{ x = initX; y = initY; }
#if !defined(_AFX_CORE_IMPL) || !defined(_AFXDLL) || defined(_DEBUG)
__forceinline CPoint::CPoint(POINT initPt)
	{ *(POINT*)this = initPt; }
#endif
__forceinline void CPoint::Offset(int xOffset, int yOffset)
	{ x += xOffset; y += yOffset; }
__forceinline void CPoint::Offset(POINT point)
	{ x += point.x; y += point.y; }
__forceinline BOOL CPoint::operator==(POINT point) const
	{ return (x == point.x && y == point.y); }
__forceinline BOOL CPoint::operator!=(POINT point) const
	{ return (x != point.x || y != point.y); }
__forceinline void CPoint::operator+=(POINT point)
	{ x += point.x; y += point.y; }
__forceinline void CPoint::operator-=(POINT point)
	{ x -= point.x; y -= point.y; }
__forceinline CPoint CPoint::operator-() const
	{ return CPoint(-x, -y); }
__forceinline CPoint CPoint::operator+(POINT point) const
	{ return CPoint(x + point.x, y + point.y); }

__forceinline CPoint CPoint::operator-(POINT point) const
	{ return CPoint(x - point.x, y - point.y); }

class CFloatVector  
{
public:
	void MakeSine(int angle);
	CFloatVector operator -(CFloatVector v);
	CFloatVector operator +(CFloatVector v);
	bool IsHorizontal();
	bool IsVertical();
	double GetLength();
	CFloatVector(double xx, double yy);
	CFloatVector(CPoint point);
	CFloatVector operator *(CFloatVector v);
	double y;
	double x;
	CFloatVector operator *=(CFloatVector v);
	CFloatVector(){x=y=0;}
	virtual ~CFloatVector(){}
	CFloatVector operator =(CFloatVector v);
	CFloatVector operator =(CPoint point);

	CFloatVector operator *(double scale);
	CFloatVector operator *=(double t);
};
double GetCosine(CFloatVector v1,CFloatVector v2);
double DotProduct(CFloatVector v1, CFloatVector v2);
class CLine  
{
public:
	bool IsPointOnLine(CPoint p);
	int GetDY();
	int GetDX();
	bool IsHorizontal();
	bool IsSingular();
	bool IsVertical();
	CFloatVector GetPoint(double t);
	double GetLength();
	CLine(int x0,int y0,int x1,int y1);
	CLine(CPoint const& v0, CPoint const& v1);
	CPoint p0;
	CPoint p1;
	CLine();
	CFloatVector const Vectorize();
	virtual ~CLine();
	double GetCosine(CFloatVector v1,CFloatVector v2);
	double DotProduct(CFloatVector v1, CFloatVector v2);
};
bool Intersection (CLine line, CPoint BasePoint, CFloatVector v, double *t1, int &rValue);
bool IsPointInside(CPoint *lpPoint, CPoint CheckPoint, int PointCount);

#define SINGULAR_LINE		0
#define NORMAL_INTERSECTION 	1
#define PARALLEL_LINES		2
#define COLLINEAR_LINES		3
#define NO_INTERSECTION		4
#define TOUCH_LINE		5

bool IsPointInside(CPoint *lpPoints, CPoint CheckPoint, int PointCount){
	int i;
	int intersection_count;
	CLine line;
	bool FoundResult;
	CFloatVector v;


	if (PointCount<3) return false;

	CLine *lpLines = MP_NEW_ARR( CLine, PointCount);

	CLine *pLine = lpLines;
	for (i = 0; i<PointCount; i++)
	{
		line.p0 = lpPoints[i];
		line.p1 = lpPoints[(i+1)%PointCount];
		(*pLine) = line;
		pLine++;
	}

	pLine = lpLines;
	for (i = 0; i<PointCount;i++,pLine++)
	{
		if (pLine->IsPointOnLine(CheckPoint))
		{
			MP_DELETE_ARR( lpLines);
			return false;
		}
	}

	do
	{
		double t;
		int rv;
		v.MakeSine(rand()); 
		FoundResult = true;
		intersection_count = 0;
		pLine = lpLines;
		for (i = 0; i<PointCount; i++,pLine++)
		{
			if (!Intersection(*pLine,CheckPoint,v,&t,rv))
			{
				FoundResult = false;
				break;
			}
			if (rv == NORMAL_INTERSECTION) intersection_count++;
		}
	}while(!FoundResult);
	
	MP_DELETE_ARR( lpLines);
	return ((intersection_count&1)==1);
}

CFloatVector CFloatVector::operator =(CFloatVector v){
	x = v.x;
	y = v.y;
	return *this;
}

CFloatVector CFloatVector::operator *(CFloatVector v){
	return CFloatVector(x*v.x, y* v.y);
}

CFloatVector::CFloatVector(CPoint point){
	x = point.x;
	y = point.y;
}

CFloatVector::CFloatVector(double xx, double yy){
	x = xx;
	y = yy;
}


CFloatVector CFloatVector::operator *(double scale){
	return CFloatVector(x*scale,y*scale);
}

CFloatVector CFloatVector::operator *=(CFloatVector v){
	x *= v.x;
	y *= v.y;
	return *this;
}



CFloatVector CFloatVector::operator *=(double t){
	x *= t;
	y *= t;
	return *this;
}

double CFloatVector::GetLength(){
	return sqrt(x*x+y*y);
}

bool CFloatVector::IsVertical(){
	return (x==0);
}

bool CFloatVector::IsHorizontal(){
	return (y==0);
}

CFloatVector CFloatVector::operator =(CPoint point){
	x = point.x;
	y = point.y;
	return *this;
}

double GetCosine(CFloatVector v1,CFloatVector v2){
	double len1 = v1.x*v1.x + v1.y*v1.y;
	double len2 = v2.x*v2.x + v2.y*v2.y;
	if ((len1<1e-20)||(len2<1e-20)) return 1;
	return DotProduct(v1,v2)/sqrt(len1*len2);
}

double DotProduct(CFloatVector v1, CFloatVector v2){
	return v1.x*v2.x + v1.y*v2.y;
}


CFloatVector CFloatVector::operator +(CFloatVector v){
	return CFloatVector(x+v.x, y+v.y);
}

CFloatVector CFloatVector::operator -(CFloatVector v){
	return CFloatVector(x-v.x, y-v.y);
}

void CFloatVector::MakeSine(int angle){
	double radian;
	radian = ((double)(angle&0xffff))*9.58737992428525768573804743432465e-5;
	x = cos(radian);
	y = sin(radian);
}

CLine::CLine(){
	p0.x = p1.x = p0.y = p1.y = 0;
}

CLine::~CLine()
{

}

CLine::CLine(CPoint const& v0, CPoint const& v1){
	p0 = v0;
	p1 = v1;
}

CLine::CLine(int x0, int y0, int x1, int y1){
	CPoint v0(x0,y0);
	CPoint v1(x1,y1);
	p0 = v0;
	p1 = v1;
}

double CLine::GetLength(){
	double dx = p1.x - p0.x;
	double dy = p1.y - p0.y;
	return sqrt(dx*dx + dy*dy);
}


CFloatVector const CLine::Vectorize(){
	return CFloatVector(p1-p0);
}



CFloatVector CLine::GetPoint(double t){
	return CFloatVector((CFloatVector)(p1-p0)*t + p0);
}

bool CLine::IsVertical(){
	return (!IsSingular()&&(p0.x == p1.x));
}

bool CLine::IsSingular(){
	return ((p0==p1)!=0);
}

bool CLine::IsHorizontal(){
	return (!IsSingular()&&(p0.y == p1.y));
}

//true if no error
//false ig was error
bool Intersection (CLine line, CPoint BasePoint, CFloatVector v, double *t1, int &rValue){
	CFloatVector lv = line.Vectorize();
	CFloatVector pnt(BasePoint);
	double t = 0;

	if (line.IsSingular()){
		rValue = SINGULAR_LINE;
		*t1 = t;
		return false;
	}

	if (line.IsHorizontal()){
		if (v.y == 0){
			if (BasePoint.y == line.p0.y){
				rValue = COLLINEAR_LINES;
				*t1 = t;
				return false;
			}
			rValue = PARALLEL_LINES;
			*t1 = t;
			return true;
		}
		if (abs(GetCosine(v,lv)) > 0.999999999){
			rValue = PARALLEL_LINES;
			*t1 = t;
			return true;
		}
		double hx = v.x*((double)(line.p0.y - BasePoint.y))/v.y;
		double int_X = BasePoint.x + hx;
		t = (int_X - line.p0.x)/line.GetDX();
		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		if ((t<0)||(t>1)){
			rValue = NO_INTERSECTION;
			*t1 = t;
			return true;
		}
		if ((t == 0)||(t == 1))
		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		{
			rValue = TOUCH_LINE;
			*t1 = t;
			return false;
		}
		rValue = NORMAL_INTERSECTION;
		*t1 = t;
		return true;
	}

	if (line.IsVertical()){
		if (v.x == 0){
			if (BasePoint.x == line.p0.x){
				rValue = COLLINEAR_LINES;
				*t1 = t;
				return false;
			}
			rValue = PARALLEL_LINES;
			*t1 = t;
			return true;
		}

		if (abs(GetCosine(v,lv)) > 0.999999999){
			rValue = PARALLEL_LINES;
			*t1 = t;
			return true;
		}

		double hy = v.y*((double)(line.p0.x - BasePoint.x))/v.x;
		double int_Y = BasePoint.y + hy;
		
		t = (int_Y - line.p0.y)/line.GetDY();
		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		if ((t<0)||(t>1)){
			rValue = NO_INTERSECTION;
			*t1 = t;
			return true;
		}
		if ((t == 0)||(t == 1)){
			rValue = TOUCH_LINE;
			*t1 = t;
			return false;
		}
		rValue = NORMAL_INTERSECTION;
		*t1 = t;
		return true;
	}

	if (abs(GetCosine(v,lv)) > 0.999999999){
		rValue = PARALLEL_LINES;
		*t1 = t;
		return true;
	}

	
	if (v.IsVertical()){
		t = ((BasePoint.x - (double)line.p0.x)/(double)line.GetDX());

		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		if ((t<0)||(t>1)){
			rValue = NO_INTERSECTION;
			*t1 = t;
			return true;
		}
		if ((t == 0)||(t == 1)){
			rValue = TOUCH_LINE;
			*t1 = t;
			return false;
		}
		rValue = NORMAL_INTERSECTION;
		*t1 = t;
		return true;
	}
	if (v.IsHorizontal()){
		t = ((double)(BasePoint.y - (double)line.p0.y)/(double)line.GetDY());

		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		if ((t<0)||(t>1)){
			rValue = NO_INTERSECTION;
			*t1 = t;
			return true;
		}
		if ((t == 0)||(t == 1)){
			rValue = TOUCH_LINE;
			*t1 = t;
			return false;
		}
		rValue = NORMAL_INTERSECTION;
		return true;
	}

	CFloatVector bp(BasePoint);
	bp.y += v.y*((double)line.p0.x - bp.x)/v.x;

	CFloatVector lBase(0,(double)line.p0.y - bp.y);
	double xi = lBase.y/(v.y/v.x - lv.y/lv.x);
	t = xi/lv.x;

	CFloatVector vec = line.GetPoint(t) - pnt;
		
	if (DotProduct(vec,v)<0){
		*t1 = t;
		rValue = NO_INTERSECTION;
		return true;
	}
	if ((t<0)||(t>1)){
		rValue = NO_INTERSECTION;
		*t1 = t;
		return true;
	}
	if ((t == 0)||(t == 1))
	{
		rValue = TOUCH_LINE;
		*t1 = t;
		return false;
	}
	rValue = NORMAL_INTERSECTION;
	*t1 = t;
	return true;

}

int CLine::GetDX(){
	return (p1.x - p0.x);
}

int CLine::GetDY(){
	return (p1.y - p0.y);
}

double CLine::GetCosine(CFloatVector v1,CFloatVector v2){
	double len1 = v1.x*v1.x + v1.y*v1.y;
	double len2 = v2.x*v2.x + v2.y*v2.y;
	if ((len1<1e-20)||(len2<1e-20)) return 1;
	return DotProduct(v1,v2)/sqrt(len1*len2);
}

double CLine::DotProduct(CFloatVector v1, CFloatVector v2){
	return v1.x*v2.x + v1.y*v2.y;
}

bool CLine::IsPointOnLine(CPoint p){
	double cosine;
	if (IsSingular())  return false;
	if ((p == p0)||(p == p1)) return true;
	CFloatVector pv(p-p0);
	CFloatVector lv(Vectorize());
	cosine = GetCosine(pv,lv);
	if ((cosine>0.99999999)&&(pv.GetLength()<=lv.GetLength())) return true;
	return false;
}


}