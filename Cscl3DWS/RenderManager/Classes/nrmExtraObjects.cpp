// nrmExtraObjects.cpp: implementation of the nrmExtraObjects class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmExtraObjects.h"
#include "rmml.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define rmCameraColor	0xFFFFFFFF
#define rmLightColor	0xFFFFE500
//#define rmCameraColor	0xFF0536B3
#define rmFrustumColor	0xFF99CCE5

#define EYERECTSIZE		6
#define ATRECTSIZE		3

//extern void	drawRect(irenderer* apirenderer, oms::omsRect arcRect, DWORD adwColor=0xFF000000);
/************************************************************************/
/* Camera eye
/************************************************************************/
nrmProjCameraEye::nrmProjCameraEye(mlMedia* apMLMedia) : nrmExtraObjects(apMLMedia)
{
}

nrmProjCameraEye::~nrmProjCameraEye()
{

}

void	nrmProjCameraEye::SetPosition(CalVector	avPosition)
{
	char	buffer[255];
	sprintf(buffer,"%f; %f; %f",avPosition.x,avPosition.y,avPosition.z);
	m_pMLMedia->SetProp("position", buffer);
}

void	nrmProjCameraEye::SetRotation(CalQuaternion	aqRotation)
{
	//??
}

void	nrmProjCameraEye::SetScale(CalVector	avScale)
{
	//??
}

CalVector	nrmProjCameraEye::GetPosition()
{
	/*if( m_pCamera)
		return m_pCamera->get_eye();*/
	return CalVector(0,0,0);
}

CalQuaternion	nrmProjCameraEye::GetRotation()
{
	return CalQuaternion(0,0,0,1);
}

CalVector	nrmProjCameraEye::GetScale()
{
	return CalVector(0,0,0);
}

void	nrmProjCameraEye::GetFarPlaneSize(float& /*afWidth*/, float& /*afHeight*/)
{
	/*float	diamondSize = 10;
	float		length = (m_pCamera->get_at() - m_pCamera->get_eye()).length();	
	matrix3d	projMatrix = m_pCamera->get_proj_matrix();
	afWidth = length*tan(m_pCamera->get_fov()/2);
	afHeight = afWidth*projMatrix._11/projMatrix._22;*/
}

/*void	nrmProjCameraEye::Render(icamera* apRenderCamera)
{
	if( !apRenderCamera)	return;
	float	diamondSize = 10;
	float		length = (m_pCamera->get_at() - m_pCamera->get_eye()).length();	
	matrix3d	viewMatrix = m_pCamera->get_view_matrix();
	viewMatrix = viewMatrix.get_invert();
	float		farPlaneWidth;
	float		farPlaneHeight;
	GetFarPlaneSize( farPlaneWidth, farPlaneHeight);

	matrix3d mindent;	mindent.make_identity();
	vertex4d	veye = apRenderCamera->get_screenXY(m_pCamera->get_eye(), (D3DXMATRIX&)mindent);
	vertex4d	vat = apRenderCamera->get_screenXY(m_pCamera->get_at(), (D3DXMATRIX&)mindent);

#define LCOUNT 18
	CalVector	lines[LCOUNT] = 
	{
		// Frustum
		CalVector(0, 0, 0), CalVector(-farPlaneWidth, -farPlaneHeight, -length),
		CalVector(0, 0, 0), CalVector(-farPlaneWidth, farPlaneHeight, -length),
		CalVector(0, 0, 0), CalVector(farPlaneWidth, farPlaneHeight, -length),
		CalVector(0, 0, 0), CalVector(farPlaneWidth, -farPlaneHeight, -length),
		// Plane
		CalVector(-farPlaneWidth, -farPlaneHeight, -length), CalVector(-farPlaneWidth, farPlaneHeight, -length),
		CalVector(-farPlaneWidth, farPlaneHeight, -length), CalVector(farPlaneWidth, farPlaneHeight, -length),
		CalVector(farPlaneWidth, farPlaneHeight, -length), CalVector(farPlaneWidth, -farPlaneHeight, -length),
		CalVector(farPlaneWidth, -farPlaneHeight, -length), CalVector(-farPlaneWidth, -farPlaneHeight, -length),
		// Dir
		CalVector(0, 0, 0), CalVector(0, 0, -length),
	};
	for( int i=0; i<LCOUNT; i++)
		lines[i].transform(&viewMatrix);
	get_irenderer()->set_transform(TRANSFORM_WORLD, &mindent);
	get_irenderer()->draw_lines( (CalVector*)&lines, LCOUNT/2, rmFrustumColor);

	oms::omsRect	rect;
	rect.left = veye.x - EYERECTSIZE;
	rect.right = veye.x + EYERECTSIZE;
	rect.top = veye.y - EYERECTSIZE;
	rect.bottom = veye.y + EYERECTSIZE;
	drawRect(get_irenderer(), rect, rmCameraColor);

	rect.left = vat.x - ATRECTSIZE;
	rect.right = vat.x + ATRECTSIZE;
	rect.top = vat.y - ATRECTSIZE;
	rect.bottom = vat.y + ATRECTSIZE;
	drawRect(get_irenderer(), rect, rmCameraColor);
}*/

void	nrmProjCameraEye::Update()
{
}

DWORD	nrmProjCameraEye::RayIntersect(CalVector& /*ro*/, CalVector& /*rd*/, float& /*dist*/)
{
	/*box3d	box;
	box.init(GetPosition(), EYERECTSIZE*dist);
	CalVector ip;
	float maxdit;
	return box.ray_intersect(ro, rd, dist, maxdit)==NO_INTERSECTION?NO_INTERSECTION:POINT_INTERSECTION;*/
	return 0;
}

/*box3d	nrmProjCameraEye::GetBounds()
{
	if( !m_pCamera)	return box3d();

	float		length = (m_pCamera->get_at() - m_pCamera->get_eye()).length();	
	matrix3d	viewMatrix = m_pCamera->get_view_matrix();
	viewMatrix = viewMatrix.get_invert();
	float		farPlaneWidth;
	float		farPlaneHeight;
	GetFarPlaneSize( farPlaneWidth, farPlaneHeight);

	box3d	bounds;
	bounds = box3d(CalVector(-farPlaneWidth, -farPlaneHeight, -length), CalVector(farPlaneWidth, farPlaneHeight, 0));
	bounds.transform(viewMatrix);
	bounds.align();
	return bounds;
}*/

/************************************************************************/
/* Camera at
/************************************************************************/
nrmProjCameraAt::nrmProjCameraAt(mlMedia* apMLMedia) : nrmExtraObjects(apMLMedia)
{
}

nrmProjCameraAt::~nrmProjCameraAt()
{

}

void	nrmProjCameraAt::SetPosition(CalVector	avPosition)
{
	char	buffer[255];
	sprintf(buffer,"%f; %f; %f",avPosition.x,avPosition.y,avPosition.z);
	m_pMLMedia->SetProp("dest", buffer);
}

void	nrmProjCameraAt::SetRotation(CalQuaternion	aqRotation)
{
	//??
}

void	nrmProjCameraAt::SetScale(CalVector	avScale)
{
	//??
}

CalVector	nrmProjCameraAt::GetPosition()
{
/*	if( m_pCamera)
		return m_pCamera->get_at();*/
	return CalVector(0,0,0);
}

CalQuaternion	nrmProjCameraAt::GetRotation()
{
	return CalQuaternion(0,0,0,1);
}

CalVector	nrmProjCameraAt::GetScale()
{
	return CalVector(0,0,0);
}

/*void	nrmProjCameraAt::Render(icamera* apRenderCamera)
{
}*/

void	nrmProjCameraAt::Update()
{
}

DWORD	nrmProjCameraAt::RayIntersect(CalVector& /*ro*/, CalVector& /*rd*/, float& /*dist*/)
{
	/*box3d	box;
	box.init(GetPosition(), ATRECTSIZE*dist);
	CalVector ip;
	float maxdit;
	return box.ray_intersect(ro, rd, dist, maxdit)==NO_INTERSECTION?NO_INTERSECTION:POINT_INTERSECTION;*/
	return 0;
}

/*box3d	nrmProjCameraAt::GetBounds()
{
	box3d	bounds;
	bounds.init(GetPosition(), 0.1);
	return bounds;
}*/
/************************************************************************/
/* Light
/************************************************************************/
nrmProjLight::nrmProjLight(mlMedia* apMLMedia) : nrmExtraObjects(apMLMedia)
{

}

nrmProjLight::~nrmProjLight()
{

}

void	nrmProjLight::SetPosition(CalVector	avPosition)
{
/*	if( m_pLight)
		m_pLight->set_translation(avPosition);*/
}

void	nrmProjLight::SetRotation(CalQuaternion	aqRotation)
{

}

void	nrmProjLight::SetScale(CalVector	avScale)
{
}

CalVector	nrmProjLight::GetPosition()
{
/*	if( m_pLight)
		return m_pLight->get_translation();*/
	return CalVector(0,0,0);
}

CalQuaternion	nrmProjLight::GetRotation()
{
	return CalQuaternion(0,0,0,1);
}

CalVector	nrmProjLight::GetScale()
{
	return CalVector(0,0,0);
}

/*void	nrmProjLight::Render(icamera* apRenderCamera)
{
	if( !apRenderCamera)	return;
	float	diamondSize = 10;

	float	factor = 1.0f;
	iviewport*	mainViewport = get_ivplib()->get_default();
	if( apRenderCamera->get_type() == ECT_ORTHO)
	{
		factor = (float)apRenderCamera->get_frustum_width()/mainViewport->get_width();
	}
	else
	{
		CalVector origin = GetPosition();
		CalVector	dir = apRenderCamera->get_at() - apRenderCamera->get_eye();
		dir.normalize();
		origin -= apRenderCamera->get_eye();
		float	projLen = dir.dot(origin);
		float	cs = cos(apRenderCamera->get_fov()/2.0f);
		float	farPlaneWidth = projLen*cs;
		float	nearPlaneWidth = apRenderCamera->get_min_plane_dist()*cs;
		factor = farPlaneWidth/nearPlaneWidth/mainViewport->get_width();
	}
	diamondSize *= factor;
	CalVector	lines[24] = 
	{
		// Plain
		CalVector(-diamondSize, diamondSize, 0),	CalVector(diamondSize, diamondSize, 0), 
		CalVector(diamondSize, diamondSize, 0),		CalVector(diamondSize, -diamondSize, 0), 
		CalVector(diamondSize, -diamondSize, 0),		CalVector(-diamondSize, -diamondSize, 0), 
		CalVector(-diamondSize, -diamondSize, 0),		CalVector(-diamondSize, diamondSize, 0), 
		// Upper
		CalVector(-diamondSize, diamondSize, 0),	CalVector(0, 0, diamondSize), 
		CalVector(diamondSize, diamondSize, 0),	CalVector(0, 0, diamondSize), 
		CalVector(diamondSize, -diamondSize, 0),	CalVector(0, 0, diamondSize), 
		CalVector(-diamondSize, -diamondSize, 0),	CalVector(0, 0, diamondSize), 
		// Lower
		CalVector(-diamondSize, diamondSize, 0),	CalVector(0, 0, -diamondSize), 
		CalVector(diamondSize, diamondSize, 0),	CalVector(0, 0, -diamondSize), 
		CalVector(diamondSize, -diamondSize, 0),	CalVector(0, 0, -diamondSize), 
		CalVector(-diamondSize, -diamondSize, 0),	CalVector(0, 0, -diamondSize)
	};
	matrix3d	mid;// = apRenderCamera->get_proj_matrix();
	mid.make_identity();
	mid._41 = GetPosition().x;
	mid._42 = GetPosition().y;
	mid._43 = GetPosition().z;
	get_irenderer()->set_transform(TRANSFORM_WORLD, &mid);
	get_irenderer()->draw_lines( (CalVector*)&lines, 12, rmLightColor);
}*/

void	nrmProjLight::Update()
{
}

DWORD	nrmProjLight::RayIntersect(CalVector& /*ro*/, CalVector& /*rd*/, float& /*dist*/)
{
//	return NO_INTERSECTION;
	return 0;
}

/*box3d	nrmProjLight::GetBounds()
{
	box3d	bounds;
	bounds.init(GetPosition(), 0.1);
	return bounds;
}*/