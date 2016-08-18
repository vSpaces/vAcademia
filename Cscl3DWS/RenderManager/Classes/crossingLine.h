
#if !defined(AFX_CrossingLine__INCLUDED_)
#define AFX_CrossingLine__INCLUDED_

#include "Sprite.h"
#include "nrmObject.h"
#include "nrmMOIVisibleStub.h"
#include "NRManager.h"
#include "IChangesListener.h"

//**************************************************************
//  CPoint
//  представление точки на плоскости
//**************************************************************
class CPoint
{
public:
	CPoint(){};
	CPoint( double ax, double ay)
	{
		x = ax;
		y = ay;
	}
	double x;
	double y;
};

//**************************************************************
//  CrossType
//  типы пересечения прямых
//**************************************************************
enum enumCrossType
{
	ctParallel,    // отрезки лежат на параллельных прямых
	ctSameLine,    // отрезки лежат на одной прямой
	ctOnBounds,    // прямые пересекаются в конечных точках отрезков
	ctInBounds,    // прямые пересекаются в   пределах отрезков
	ctOutBounds    // прямые пересекаются вне пределов отрезков
};

//**************************************************************
//  CrossResultRec
//  результат проверки пересечения прямых
//**************************************************************
struct CrossResultRec
{
	enumCrossType  type;  // тип пересечения
	CPoint      pt;    // точка пересечения
};


// находит пересечение двух отрезков, выдает координаты точки пересения
class CCrossingLine 
{
public:
	CCrossingLine();
	virtual ~CCrossingLine();

	CrossResultRec *GetResult( const CPoint &p11, const CPoint &p12, const CPoint &p21, const CPoint &p22);
// реализация moMedia
private:
	CrossResultRec result;
};

#endif // !defined(AFX_CrossingLine__INCLUDED_)
