// nrmExtraObjects.h: interface for the nrmExtraObjects class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRMEXTRAOBJECTS_H__E7BD27B0_B387_46FA_B576_1F831152EA8F__INCLUDED_)
#define AFX_NRMEXTRAOBJECTS_H__E7BD27B0_B387_46FA_B576_1F831152EA8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nrmObject.h"
#include <vector.h>
#include <quaternion.h>

using	namespace rmml;
//using	namespace natura3d;
namespace rmml
{
	class mlMedia;
}
/************************************************************************/
/* Интерфейс дополнительных объектов для рендеринга в проекции:
/*	- источники освещения
 *	- камеры
/************************************************************************/
struct IExtraObjects
{
	virtual	mlMedia*		GetMLMedia() = 0;
	virtual	void			SetPosition(CalVector	avPosition) = 0;
	virtual	void			SetRotation(CalQuaternion	aqRotation) = 0;
	virtual	void			SetScale(CalVector	avScale) = 0;
	virtual	CalVector		GetPosition() = 0;
	virtual	CalQuaternion	GetRotation() = 0;
	virtual	CalVector		GetScale() = 0;
//	virtual	void			Render(icamera* apRenderCamera) = 0;
	virtual	void			Update() = 0;
	virtual DWORD			RayIntersect(CalVector& ro, CalVector& rd, float& dist) = 0;
	//virtual	box3d			GetBounds() = 0;
};

class nrmExtraObjects : public nrmObject
{
public:
	nrmExtraObjects(mlMedia* apMLMedia) : nrmObject(apMLMedia){ m_pMLMedia = apMLMedia;}
	virtual ~nrmExtraObjects(){};
	virtual IExtraObjects*	GetIExtraObjects(){ return NULL;}

protected:
	mlMedia*	m_pMLMedia;
};

/************************************************************************/
/* Camera
/************************************************************************/
class nrmProjCameraEye : public nrmExtraObjects, public IExtraObjects
{
public:
	nrmProjCameraEye(mlMedia* apMLMedia);
	virtual ~nrmProjCameraEye();
	IExtraObjects*	GetIExtraObjects(){ return this;}

	// реализация IExtraObjects
	void			SetPosition(CalVector	avPosition);
	void			SetRotation(CalQuaternion	aqRotation);
	void			SetScale(CalVector	avScale);
	CalVector		GetPosition();
	CalQuaternion	GetRotation();
	CalVector		GetScale();
	//void			Render(icamera* apRenderCamera);
	void			Update();
	DWORD			RayIntersect(CalVector& ro, CalVector& rd, float& dist);
	mlMedia*		GetMLMedia(){ return m_pMLMedia;}
	//box3d			GetBounds();

protected:
	void	GetFarPlaneSize(float& afWidth, float& afHeight);
};

class nrmProjCameraAt : public nrmExtraObjects, public IExtraObjects
{
public:
	nrmProjCameraAt(mlMedia* apMLMedia);
	virtual ~nrmProjCameraAt();
	IExtraObjects*	GetIExtraObjects(){ return this;}

	// реализация IExtraObjects
	void			SetPosition(CalVector	avPosition);
	void			SetRotation(CalQuaternion	aqRotation);
	void			SetScale(CalVector	avScale);
	CalVector		GetPosition();
	CalQuaternion	GetRotation();
	CalVector		GetScale();
	//void			Render(icamera* apRenderCamera);
	void			Update();
	DWORD			RayIntersect(CalVector& ro, CalVector& rd, float& dist);
	mlMedia*		GetMLMedia(){ return m_pMLMedia;}
	//box3d			GetBounds();
};

/************************************************************************/
/* Camera
/************************************************************************/
class nrmProjLight : public nrmExtraObjects, public IExtraObjects
{
public:
	nrmProjLight(mlMedia* apMLMedia);
	virtual ~nrmProjLight();
	IExtraObjects*	GetIExtraObjects(){ return this;}

	// реализация IExtraObjects
	void			SetPosition(CalVector	avPosition);
	void			SetRotation(CalQuaternion	aqRotation);
	void			SetScale(CalVector	avScale);
	CalVector		GetPosition();
	CalQuaternion	GetRotation();
	CalVector		GetScale();
	//void			Render(icamera* apRenderCamera);
	void			Update();
	DWORD			RayIntersect(CalVector& ro, CalVector& rd, float& dist);
	mlMedia*		GetMLMedia(){ return m_pMLMedia;}
//	box3d			GetBounds();

protected:
//	ilight* m_pLight;
};

#endif // !defined(AFX_NRMEXTRAOBJECTS_H__E7BD27B0_B387_46FA_B576_1F831152EA8F__INCLUDED_)
