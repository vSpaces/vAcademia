
#pragma once

#include "Quaternion3D.h"
#include "Matrix4D.h"
#include "TinyXML.h"
#include "Model.h"
#include <vector>

typedef struct _SRotation
{
	float angle;

	float x, y, z;

	_SRotation()
	{
		angle = 0;
		x = 0;
		y = 0;
		z = 1;
	}

	_SRotation(_SRotation* other)
	{
		angle = other->angle;
		x = other->x;
		y = other->y;
		z = other->z;
	}
} SRotation;

class CRotationPack
{
public:
	CRotationPack();
	CRotationPack(TiXmlNode* rotationBlock);
	~CRotationPack();

	void Clear();

	void SetAsDXQuaternion(float x, float y, float z, float w);
	void SetRotating(float angle, float x, float y, float z);
	
	void GetAsDXQuaternion(float* x, float* y, float* z, float* w);
	CQuaternion GetAsDXQuaternion();

	void ApplyRotation();
	void ApplyInverseRotation();

	void PrintToXML(FILE* file);

	CMatrix4D& GetRotationMatrix();

	void CloneTo(CRotationPack* other);

	float GetCurrentFirstAngle();

	void GetRotating(float* x, float* y, float* z, float* angle);

	bool IsEmptyRotation();
	void SetEmptyRotation(bool isEmptyRotation);

	void SetRotationModel(CModel* _model);
	void ApplyRotationToVertexs();

private:
	CQuaternion GetDXQuaternion();
	void ApplyRotation(float koef);

	SRotation m_rotation;
	CQuaternion m_quat;
	bool m_isEmptyRotation;

	CModel* m_model;
};