// CalShadowData.h: interface for the CalShadowData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALSHADOWDATA_H__54A93F35_5F1B_4ED6_9859_CE59A113FF04__INCLUDED_)
#define AFX_CALSHADOWDATA_H__54A93F35_5F1B_4ED6_9859_CE59A113FF04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vector"

class CalCoreMesh;

typedef struct _ShadowEdge
{
	unsigned	short	face1, face2;
	int	vertex1, vertex2;
	_ShadowEdge(){}
	_ShadowEdge(unsigned	short	f1, unsigned	short f2, int v1, int v2)
	{
		face1 = f1;
		face2 = f2;
		vertex1 = v1;
		vertex2 = v2;
	}
}	ShadowEdge;

class CalShadowData  
{
public:
	CalShadowData * clone();
	bool createOn(CalCoreMesh* ap_Model);
	CalShadowData();
	virtual ~CalShadowData();

	std::vector<ShadowEdge>	edges;

};

#endif // !defined(AFX_CALSHADOWDATA_H__54A93F35_5F1B_4ED6_9859_CE59A113FF04__INCLUDED_)
