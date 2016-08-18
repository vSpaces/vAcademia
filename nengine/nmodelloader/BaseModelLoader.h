
#pragma once

#include "Vector3D.h"
#include "IModel.h"

#define DEFAULT_MATERIAL_PATH	"external\\materials\\"

class CBaseModelLoader
{
public:
	CBaseModelLoader();
	~CBaseModelLoader();

	void CalculateNormals(IModel* model);
	void FreeAll();

	unsigned int GetCalculateTime();

private:
	void GetNormal(CVector3D& p1, CVector3D& p2, CVector3D& p3, CVector3D& n);

	int m_faceCount;
	int m_vertexCount;

	unsigned int m_calculateNormalsTime;

	float* m_facesNormals;	
};