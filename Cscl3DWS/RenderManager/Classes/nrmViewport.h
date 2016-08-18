// nrmViewport.h: interface for the nrmViewport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmViewport_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmViewport_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#include "Viewport.h"
#include "nrmanager.h"
#include "nrmObject.h"
#include "nrmMOIVisibleStub.h"
#include "nrmBrowser.h"

//////////////////////////////////////////////////////////////////////////
class nrmCamera;

//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class nrmViewport :	
					public nrmObject,
					public nrmMOIVisibleStub,
					public moIViewport
{
public:
	nrmViewport(mlMedia* apMLMedia);
	virtual ~nrmViewport();

// реализация moMedia
public:
	moIVisible* GetIVisible(){ return this; }

// реализация  moIVisible
public:
	// Свойства
	void AbsXYChanged(); // изменилось абсолютное положение
	void SizeChanged();
	void AbsDepthChanged(); // изменилась абсолютная глубина
	void AbsVisibleChanged(); // изменилась абсолютная видимость
	void AbsOpacityChanged(){}; // изменилась абсолютная видимость
	mlSize GetMediaSize(); 
	int IsPointIn(const mlPoint /*aPnt*/){ return -1; }

// реализация  moIVieport
public:
	moIViewport* GetIViewport(){ return this; }
	mlMedia* GetObjectUnderMouse(mlPoint &aMouseXY, bool abGroundOnly = false, ml3DPosition *apXYZ=NULL, ml3DLocation *apLocation=NULL, 
		int *apiType=NULL, ml3DPosition *apXYTexture=NULL, mlMedia** apaCheckObjects=NULL, 
		mlMedia** appTextureMedia=NULL, mlPoint* apXYTextureMedia=NULL, bool* abpGround=NULL);
	bool GetProjection(ml3DPosition &aXYZ, mlPoint &aXY);
	bool GetProjection(mlMedia* ap3DO, mlPoint &aXY, mlRect &aBounds);
	void GetRay(mlPoint &aXY, ml3DPosition &aXYZ0, ml3DPosition &aXYZ);
	void SetLODScale(float lodScale);
	void CameraChanged();
	void InclExclChanged();
	void SoundVolumeChanged();
	void TraceObjectOnMouseMoveChanged();
	void onPress(mlPoint apntXY);	// нажали где-то (apntXY) на viewport-е
	void onRelease();				// отпустили кнопку мыши
	mlRect GetBounds(mlMedia* apObject);
	// получить точку пересечения луча с экрана с плоскостью, заданной точкой и нормалью
	ml3DPosition GetIntersectionWithPlane(mlPoint aXY, ml3DPosition aPlane, ml3DPosition aNormal, bool& abBackSide);
	// получить точку проекции 3D-точки на землю с учетом гравитации
	ml3DPosition GetProjectionToLand(ml3DPosition a3DPoint);
	// получить точку проекции 3D-точки на землю с учетом объектов
	ml3DPosition GetProjectionToObjects(ml3DPosition a3DPoint);
	// получить строковый идентификатор объекта по 2D-координатам
	bool GetObjectStringIDAt(mlPoint aXY, mlOutString& asStringID);

public:
	BOOL render();
	void ShowHint();
	void SetCamera( nrmCamera* cam);	// установка новой камеры
	nrmCamera* GetCamera(){ return active_camera;}
//	std::vector<ivisibleobject*>&	get_includes(){ return vIncludes;}
//	std::vector<ivisibleobject*>&	get_excludes(){ return vExcludes;}
	virtual	void	OnSetRenderManager();
	CViewport*		m_viewport;

protected:

	//CSpeechCloud*	hintCloud;
//	void CollectChildren(std::vector<ivisibleobject*>& array, mlMedia** ppmlMedia);
	//ivisibleobject* getN3DObjectUnderMouse(mlPoint &aMouseXY, CalVector& ip);
	//iwindow*		getTexturedActiveX(mlPoint apntXY, float& ax, float& ay, nrmBrowser** apibrowser);
	//std::vector<ivisibleobject*>	vIncludes;
	//std::vector<ivisibleobject*>	vExcludes;
	nrmCamera*		active_camera;
};

#endif // !defined(AFX_nrmViewport_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
