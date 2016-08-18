
#pragma once

#include "Vector3D.h"

class CModel;

class CRayTraceTriMesh
{
public:
	CRayTraceTriMesh();
	~CRayTraceTriMesh();

	bool RayTraceZ(float x, float y, float* z);
	bool RayTrace(CVector3D& rayOrigin, CVector3D& rayDirection, CVector3D* pos, float* tu, float* tv);
	bool CheckRayIntersect(CVector3D& rayOrigin, CVector3D& rayDirection);

	static int GetTriCount();

	void SetModel(CModel* model);
	void SetScale(float scale);	

private:
	void FreeMemory();
	void RecountBounds();
	void UpdateModelDataIfNeeded();

	void SetVertexPointer(float* vx, int vertexCount);
	void SetFacePointer(unsigned short* fs, int faceCount);
	void SetTextureCoordsPointer(float* tc);

	bool FastRaytraceTriangle(int v0Index, int edgeStartIndex, float *t);
	bool RaytraceTriangle(int index0, int index1, int index2, float *t, float* u, float* v);

	bool CheckTrimeshSphereIntersect(CVector3D& rayOrigin, CVector3D& rayDirection)const;

	float* m_vx;
	float* m_tc;
	float* m_bounds;
	CVector3D* m_edges;
	int m_vertexCount;

	float m_minX, m_minY, m_maxX, m_maxY;

	unsigned short* m_fs;
	int m_faceCount;

	float m_radius;
	float m_radiusSq;

	CVector3D m_center;
	CVector3D m_rayOrigin;
	CVector3D m_rayDirection;

	bool m_isDataChanged;	

	float m_scale;
	CModel* m_model;	
};

extern int stat_rayCount;