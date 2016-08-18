//****************************************************************************//
// renderer.h                                                                 //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_RENDERER_H
#define CAL_RENDERER_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//
#include "windows.h"
#include "global.h"
#include "hitverts.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalModel;
class CalSubmesh;
class CMpeg4FDP;
class CalShadowData;
class CalCoreMaterial;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The renderer class.
  *****************************************************************************/
namespace Cal
{

typedef struct _CPOINT : public tagPOINT{

	_CPOINT(){ x=0; y=0;}

	_CPOINT( LONG ax, LONG ay){
		x=ax; y=ay;
	}

	_CPOINT(const POINT& p){
		x=p.x; y=p.y;
	}

	BOOL operator==(const _CPOINT& pcomp) const
	{ return (x==pcomp.x)&&(y==pcomp.y); }

	BOOL operator!=(const _CPOINT& pcomp) const
	{ return (x!=pcomp.x)||(y!=pcomp.y); }

	void operator+=(SIZE size)
		{ x += size.cx; y += size.cy; }
	void operator-=(SIZE size)
		{ x -= size.cx; y -= size.cy; }
	void operator+=(const _CPOINT& point)
		{ x += point.x; y += point.y; }
	void operator-=(const _CPOINT& point)
		{ x -= point.x; y -= point.y; }
	_CPOINT operator-() const
		{ return _CPOINT(-x, -y); }
	_CPOINT operator+(const _CPOINT& point) const
		{ return _CPOINT(x + point.x, y + point.y); }
	_CPOINT operator-(const _CPOINT& point) const
		{ return _CPOINT(x - point.x, y - point.y); }
} CPOINT;

typedef struct _CRECT : public tagRECT{

	BOOL _CRECT::PtInRect( CPOINT pnt){
		return ((pnt.x>=left)&&(pnt.x<=right)&&(pnt.y>=top)&&(pnt.y<=bottom));
	}
} CRECT;
}

class CAL3D_API CalRenderer
{
// member variables
public:
  friend class CalModel;
  CalModel *m_pModel;
  CalSubmesh *m_pSelectedSubmesh;
  WORD		m_dwSelectedSubmesh;
  WORD		m_dwSelectedMesh;

// constructors/destructor
public:
  CalRenderer();
	virtual ~CalRenderer();

// member functions	
public:
	bool isIntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2, const D3DXVECTOR3& vres);
	bool isMouseOn( POINT apoint, D3DMATRIX	*matWrld=NULL, D3DMATRIX	*matProj=NULL, D3DMATRIX	*matView=NULL,
										DWORD		scrw=0, DWORD		scrh=0);
	bool getHitBoxes(HITBOX** ppHb, DWORD* count);
	int getFaceCountInModel();
	int getEdgesCountInModel();
	int getVertCountInModel();
	void getVBs(IDirect3DVertexBuffer8 **ptr, IDirect3DVertexBuffer8 **puntr, int imeshid=-1, int isubmeshid=-1);
	bool useVB();
  WORD getVertList(float**, int imeshid=-1, int isubmeshid=-1);
  WORD getWireIndices(WORD** ppIndices, int imeshid, int isubmeshid);
  bool beginRendering();
  bool create(CalModel *pModel);
  void destroy();
  void endRendering();
  void getAmbientColor(unsigned char *pColorBuffer);
  void getDiffuseColor(unsigned char *pColorBuffer);
  void getEmissiveColor(unsigned char *pColorBuffer);
  int getFaceCount();
  int getFaces(unsigned short** pFaceBuffer);
  int getMapCount();
  Cal::UserData getMapUserData(int mapId);
  void setMapUserData(int mapId, Cal::UserData data);
  int getMeshCount();
  int getNormals(float *pNormalBuffer);
  float getShininess();
  void getSpecularColor(unsigned char *pColorBuffer);
  int getSubmeshCount(int meshId);
  int getTextureCoordinates(int mapId, float *pTextureCoordinateBuffer, int imeshid=-1, int isubmeshid=-1);
  int getVertexCount();
  int getVertices(float **pVertexBuffer, bool isUsualOrder);
  int getTangents(int mapID, float *pVertexBuffer);
  bool selectMeshSubmesh(const int& meshId, const int& submeshId);
  CalSubmesh* getMeshSubmesh(int meshId, int submeshId);
  CalCoreMaterial* getMaterial();
  int getSubmeshVertCount(int meshId, int submeshId);
  void recalculateUVW(const int& meshId, const int& submeshId, const CalVector& xTr, const CalVector& yTr, const int& txCoords=0);
  bool isTangentsEnabled(int mapId) const;

  void releaseVertices();
  bool lockVertices();
  void releaseIndices();
  bool lockIndices();

protected:
  LPVOID					pVertData;
  LPVOID					pIndexData;
  LPDIRECT3DVERTEXBUFFER8	pVertexBuffer;
  LPDIRECT3DINDEXBUFFER8	pIndexBuffer;

public:
	CalShadowData* getShadowData(int id);
	LPVOID get_vertices(CalSubmesh* psm);
	LPVOID get_indices(CalSubmesh* psm);
	int getMapType(int mapId);
	bool create_influences(int istep=-1, CMpeg4FDP* face=NULL);
	void get_FAP_ctrl_points(LPVOID* array, DWORD &count);
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
		CFloatVector(Cal::CPOINT point);
		CFloatVector operator *(CFloatVector v);
		double y;
		double x;
		CFloatVector operator *=(CFloatVector v);
		CFloatVector(){x=y=0;}
		virtual ~CFloatVector(){}
		CFloatVector operator =(CFloatVector v);
		CFloatVector operator =(Cal::CPOINT point);

		CFloatVector operator *(double scale);
		CFloatVector operator *=(double t);
	};
	double GetCosine(CFloatVector v1,CFloatVector v2);
	double DotProduct(CFloatVector v1, CFloatVector v2);
	class CLine  
	{
	public:
		bool IsPointOnLine(Cal::CPOINT p);
		int GetDY();
		int GetDX();
		bool IsHorizontal(float tolerance=0.0);
		bool IsSingular();
		bool IsVertical(float tolerance=0.0);
		CFloatVector GetPoint(double t);
		double GetLength();
		CLine(int x0,int y0,int x1,int y1);
		CLine(Cal::CPOINT const& v0, Cal::CPOINT const& v1);
		Cal::CPOINT p0;
		Cal::CPOINT p1;
		CLine();
		CFloatVector const Vectorize();
		virtual ~CLine();
		double GetCosine(CFloatVector v1,CFloatVector v2);
		double DotProduct(CFloatVector v1, CFloatVector v2);
	};
	bool Intersection (CLine line, Cal::CPOINT BasePoint, CFloatVector v, double *t1, int &rValue);
	bool IsPointInside(Cal::CPOINT *lpPoint, Cal::CPOINT CheckPoint, int PointCount);
};

#endif

//****************************************************************************//
