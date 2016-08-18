
#include "StdAfx.h"
#include <assert.h>
#include "RotationPack.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CRotationPack::CRotationPack():
m_model(NULL)
{
	m_quat.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_isEmptyRotation = false;
}

void CRotationPack::Clear()
{
	m_quat.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_rotation.angle = 0;
	m_rotation.x = 0;
	m_rotation.y = 0;
	m_rotation.z = 1;
	m_isEmptyRotation = false;
	m_model = NULL;
}

CRotationPack::CRotationPack(TiXmlNode* /*rotationBlock*/)
{
	/*TiXmlNode* rotation = rotationBlock->FirstChild();
	
	while (rotation)
	{
		if (rotation->ToElement()->Attribute("angle"))
		{
			if ((rotation->ToElement()->Attribute("x")) &&
				(rotation->ToElement()->Attribute("y")) &&
				(rotation->ToElement()->Attribute("z")))
			{
				float angle = (float)atof(rotation->ToElement()->Attribute("angle"));
				float x = (float)atof(rotation->ToElement()->Attribute("x"));
				float y = (float)atof(rotation->ToElement()->Attribute("y"));
				float z = (float)atof(rotation->ToElement()->Attribute("z"));

				AddSimpleRotating(angle, x, y, z);
			}
		}
		else
		{
			if ((rotation->ToElement()->Attribute("x")) &&
				(rotation->ToElement()->Attribute("y")) &&
				(rotation->ToElement()->Attribute("z")) &&
				(rotation->ToElement()->Attribute("startangle")) &&
				(rotation->ToElement()->Attribute("endangle")) &&
				(rotation->ToElement()->Attribute("time")))
			{
				float startAngle = (float)atof(rotation->ToElement()->Attribute("startangle"));
				float endAngle = (float)atof(rotation->ToElement()->Attribute("endangle"));
				float x = (float)atof(rotation->ToElement()->Attribute("x"));
				float y = (float)atof(rotation->ToElement()->Attribute("y"));
				float z = (float)atof(rotation->ToElement()->Attribute("z"));
				__int64 time = rtl_atoi(rotation->ToElement()->Attribute("time"));

				AddDynamicRotating(startAngle, endAngle, g->ne.time, time, x, y, z, true);
			}
		}

		rotation = rotationBlock->IterateChildren(rotation);
	}*/
}

void CRotationPack::PrintToXML(FILE* /*file*/)
{
	/*fprintf(file, "\t\t<rotationpack>\n");

	std::vector<SRotation *>::iterator iter = m_rotations.begin();
	std::vector<SRotation *>::iterator end = m_rotations.end();

	for ( ; iter != end; iter++)
	{
		if ((*iter)->startAngle == (*iter)->endAngle)
		{
			fprintf(file, "\t\t\t<rotation angle=\"%f\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", (*iter)->startAngle, (*iter)->x, (*iter)->y, (*iter)->z);
		}
		else
		{
			fprintf(file, "\t\t\t<rotation startangle=\"%f\" endangle=\"%f\" time=\"%f\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", (*iter)->startAngle, (*iter)->endAngle, (*iter)->allTime, (*iter)->x, (*iter)->y, (*iter)->z);
		}
	}

	fprintf(file, "\t\t</rotationpack>\n");*/
}

void CRotationPack::SetAsDXQuaternion(float x, float y, float z, float w)
{
	#define RTN_EPS 0.00001
	if( x<RTN_EPS && y<RTN_EPS && z<RTN_EPS &&
		x>-RTN_EPS && y>-RTN_EPS && z>-RTN_EPS)
	{
		m_rotation.angle = 0;
		m_rotation.x = 0;
		m_rotation.y = 0;
		m_rotation.z = 1;
	}
	else
	{
	float rotX = 0;
	float rotY = 0;
	float rotZ = 1;
	float rotAngle = 0;

	float vl = (float)sqrt(x * x + y * y + z * z + w * w);
	if (vl > 0.00000000001f)
	{
		float ivl = 1.0f/vl;
		rotX = x * ivl;
		rotY = y * ivl;
		rotZ = z * ivl;
		w = w * ivl;
	}
								
	rotAngle = -2 * acosf(w) * 180.0f / 3.141592653589f;
	
	m_rotation.angle = rotAngle;
	m_rotation.x = rotX;
	m_rotation.y = rotY;
	m_rotation.z = rotZ;
	}

	m_quat.x = x;
	m_quat.y = y;
	m_quat.z = z;
	m_quat.w = w;

	if (m_model)
	{
		ApplyRotationToVertexs();
	}
}

void CRotationPack::GetAsDXQuaternion(float* x, float* y, float* z, float* w)
{
	*x = m_quat.x;
	*y = m_quat.y;
	*z = m_quat.z;
	*w = m_quat.w;
}

void CRotationPack::ApplyRotationToVertexs()
{
	if (m_model->GetLoadingState() != MODEL_LOADED)
	{
		return;
	}

	m_quat.Normalize();
	
	if (!m_model->vxOld)
	{
		m_model->vxOld = MP_NEW_ARR(float, m_model->GetVertexCount() * 3);
		memcpy(m_model->vxOld, m_model->vx,m_model->GetVertexCount()*3*sizeof(float));
	}
	else
	{
		memcpy(m_model->vx,m_model->vxOld,m_model->GetVertexCount()*3*sizeof(float));
	}

	CVector3D* v = (CVector3D*)&m_model->vx[0];

	for (int i = 0; i < m_model->GetVertexCount(); i++)
	{
		*v *= m_quat;
		v++;
	}

	if (m_model->m_vbo)
	{
		m_model->m_vbo->SetVertexArray(&m_model->vx[0], m_model->GetVertexCount());
	}
}

/*void CRotationPack::GetAsDXQuaternion(float* x, float* y, float* z, float* w)
{
	float _x = m_rotation.x;
	float _y = m_rotation.y;
	float _z = m_rotation.z;

	float r = sqrt(_x * _x + _y * _y + _z * _z);
	if(r > 0.0f)
	{
		_x /= r;
		_y /= r;
		_z /= r;	
	}

	float angle = -m_rotation.angle / 2.0f;

	*x = _x * sinf(angle * PI / 180.0f);
	*y = _y * sinf(angle * PI / 180.0f);
	*z = _z * sinf(angle * PI / 180.0f);
	*w = cosf(angle * PI / 180.0f);
}*/

CQuaternion CRotationPack::GetAsDXQuaternion()
{
	return m_quat;
}

CQuaternion CRotationPack::GetDXQuaternion()
{
	float angle = -m_rotation.angle / 2.0f;

	float x = m_rotation.x;
	float y = m_rotation.y;
	float z = m_rotation.z;
	float r = sqrt(x * x + y * y + z * z);
	if (r > 0.0f)
	{
		x /= r;
		y /= r;
		z /= r;
	}

	CQuaternion q;
	q.x = x * sinf(angle * MY_PI / 180.0f);
	q.y = y * sinf(angle * MY_PI / 180.0f);
	q.z = z * sinf(angle * MY_PI / 180.0f);
	q.w = cosf(angle * MY_PI / 180.0f);

	return q;
}

void CRotationPack::GetRotating(float* x, float* y, float* z, float* angle)
{	
	*x = m_rotation.x;
	*y = m_rotation.y;
	*z = m_rotation.z;
	*angle = m_rotation.angle;
}
	
void CRotationPack::SetRotating(float angle, float x, float y, float z)
{
	m_rotation.angle = angle;
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;

	CQuaternion q = GetDXQuaternion();
	m_quat = q;

	if (m_model)
	{
		ApplyRotationToVertexs();
	}
}

void CRotationPack::ApplyRotation(float koef)
{
	GLFUNC(glRotatef)(m_rotation.angle * koef, m_rotation.x, m_rotation.y, m_rotation.z);
}

float CRotationPack::GetCurrentFirstAngle()
{
	return m_rotation.angle;
}

void CRotationPack::ApplyRotation()
{
	ApplyRotation(1.0f);
}

void CRotationPack::ApplyInverseRotation()
{
	ApplyRotation(-1.0f);
}

CMatrix4D& CRotationPack::GetRotationMatrix()
{
#pragma warning(push)
#pragma warning(disable : 4172)
#pragma warning(disable : 4239)
	assert(false);
	return CMatrix4D();
#pragma warning(pop)
}

void CRotationPack::CloneTo(CRotationPack* other)
{
	other->m_rotation = m_rotation;
	other->m_quat = m_quat;
}

bool CRotationPack::IsEmptyRotation()
{
	return m_isEmptyRotation;
}

void CRotationPack::SetEmptyRotation(bool isEmptyRotation)
{
	m_isEmptyRotation = isEmptyRotation; 
}

void CRotationPack::SetRotationModel(CModel* _model)
{
	m_model = _model; 
}

CRotationPack::~CRotationPack()
{
}
