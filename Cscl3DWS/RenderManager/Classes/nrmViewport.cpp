// nrmViewport.cpp: implementation of the nrmViewport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmViewport.h"
#include "nrmCamera.h"
#include "nrm3DGroup.h"
#include "nrm3DObject.h"
#include "nrmActiveX.h"
#include "nrmBrowser.h"
#include "nrm3DScene.h"
#include "windows.h"
#include "UserData.h"
#include <IMouseListener.h>
#include "MouseController.h"
#include "CameraInLocation.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

nrmViewport::nrmViewport(mlMedia* apMLMedia): 
	nrmObject(apMLMedia),
	active_camera(NULL)
{
	m_viewport = MP_NEW(CViewport);
	m_viewport->SetUserData(USER_DATA_NRMOBJECT, this);

	mlMedia*	mpMLHint = apMLMedia->GetIViewport()->GetHint();
	if (mpMLHint)
	{
		const wchar_t* pwcSrc = mpMLHint->GetILoadable()->GetSrc();
		if (pwcSrc)
		{
			mlString sSRC=pwcSrc;
			if (!sSRC.empty())
			{
				mlIHint*	pmlIHint = mpMLHint->GetIHint();
				mlStyle*	pmlStyle = NULL;
				if ((pmlIHint != NULL) && (pmlStyle = pmlIHint->GetStyle())!=NULL) 
				{
					int fontSize = 14;
					if( pmlStyle->fontSize.val != -1)
					{
						fontSize = pmlStyle->fontSize.val;
					}
				}
			}
		}
	}
}

void	nrmViewport::OnSetRenderManager()
{
	ATLASSERT(m_pRenderManager);
	m_pRenderManager->RegisterViewport(this);
	AbsXYChanged();
	SizeChanged();
	AbsDepthChanged();
	CameraChanged();
}

// изменилось абсолютное положение
void nrmViewport::AbsXYChanged()
{
	mlIVisible* pMLIViewport=mpMLMedia->GetIVisible();
	mlPoint	xy = pMLIViewport->GetAbsXY();
	m_viewport->SetOffset(xy.x, xy.y);
}

// изменилось абсолютные значения ширины и высоты
void nrmViewport::SizeChanged()
{
	mlIVisible* pMLIViewport=mpMLMedia->GetIVisible();
	mlSize	size = pMLIViewport->GetSize();

	if ((m_viewport->GetWidth() == size.width) && (m_viewport->GetHeight() == size.height))
	{
		return;
	}

	m_viewport->SetSize(size.width, size.height);
	size.width = m_viewport->GetWidth();
	size.height = m_viewport->GetHeight();
	pMLIViewport->SetSize(size);
}

// изменилась абсолютная глубина
void nrmViewport::AbsDepthChanged()
{
	// not supported now
}

// изменилась абсолютная видимость
void nrmViewport::AbsVisibleChanged()
{
	// ??
}

mlSize nrmViewport::GetMediaSize()
{
	mlSize size; ML_INIT_SIZE(size);
	return size;
}

// установка новой камеры
void nrmViewport::SetCamera(nrmCamera* cam)
{
	nrm3DScene* scene3d = NULL;
	if (cam)
	if (cam->GetMLMedia()->GetI3DObject()->GetScene3D())
	{
		scene3d = (nrm3DScene*)cam->GetMLMedia()->GetI3DObject()->GetScene3D()->GetSafeMO();
	}

	if ((scene3d) && (g->scm.GetActiveScene() == scene3d->m_scene))
	if ((cam != active_camera) && (active_camera))
	{
		active_camera->OnCameraChangedTo(cam);
	}	

	active_camera = cam;

	if (!cam)
	{
		m_viewport->SetCamera(NULL);
		return;
	}

	m_viewport->SetCamera(cam->m_camera);
	
	if (scene3d)
	{
		if (scene3d->m_scene)
		{
			scene3d->m_scene->SetViewport(m_viewport);
		}
	}
}

void nrmViewport::CameraChanged()
{
	mlMedia* mlpCamera = mpMLMedia->GetIViewport()->GetCamera();
	 
	if (!mlpCamera)
	{
		SetCamera(NULL);
		return;
	}

	nrmCamera* camera = (nrmCamera*)mlpCamera->GetSafeMO();
	SetCamera(camera);
}

// реализация  moIVieport

// получить строковый идентификатор объекта по 2D-координатам
bool nrmViewport::GetObjectStringIDAt(mlPoint /*aXY*/, mlOutString& asStringID)
{
	if (!m_viewport->GetCamera())
	{
		return false;	// no camera on viewport
	}
	
	C3DObject* obj = m_viewport->GetObjectUnderMouse();
	if(obj == NULL)
		return false;

	if (obj->GetParent())
	{
		obj = obj->GetParent();
	}

	if (obj->IsDeleted())
	{
		return false;
	}
	std::wstring sStringID = obj->GetName();

	if(sStringID.empty())
		return false;
	
	asStringID = sStringID.c_str();
	return true;
}

// appTextureMedia - возвращаемая ссылка на rmml-2D-объект, натянутый на текстуру, в которую попали
// apXYTextureMedia - 2D-координаты положения мыши относительно этого 2D-объекта
// abpGround - является ли объект под мышкой "землей"?
mlMedia* nrmViewport::GetObjectUnderMouse(mlPoint& /*aMouseXY*/, bool abGroundOnly, ml3DPosition *apXYZ, ml3DLocation *apLocation, 
										  int* /*apiType*/, ml3DPosition* /*apXYTexture*/, mlMedia** apaCheckObjects, 
										  mlMedia** /*appTextureMedia*/, mlPoint* apXYTextureMedia, bool* abpGround)
{
	if (g->scm.GetActiveScene())
	if (m_viewport != g->scm.GetActiveScene()->GetViewport())
	{
		return (mlMedia*)NO_OBJECT;
	}

	if(abpGround != NULL)
		*abpGround = false;

	std::vector<C3DObject*> checkObjects;
	if (apaCheckObjects)
	while (*apaCheckObjects != NULL)
	{
		mlMedia* obj = *apaCheckObjects;
		nrm3DObject* nrmObj = (nrm3DObject *)obj->GetSafeMO();
		if ((nrmObj != NULL) && (!PMO_IS_NULL(nrmObj)))
		{
			checkObjects.push_back(nrmObj->m_obj3d);	
		}
		apaCheckObjects++;
	}

	if ((m_viewport->GetLastGetCoordsStatus() == GET_COORDS_FAILED) ||
		(m_viewport->GetLastGetCoordsStatus() == GET_COORDS_NOTHING_UNDER_MOUSE))
	{
		return NULL;
	}

	CVector3D coords = m_viewport->GetSelected3DCoords();
	if (apXYZ)
	{	
		apXYZ->x = coords.x;
		apXYZ->y = coords.y;
		apXYZ->z = coords.z;
	}
	
	if (!m_viewport->GetCamera())
	{
		return NULL;	// no camera on viewport
	}

	if(apLocation != NULL){
		SLocationInfo* pLocationInfo = gRM->mapMan->GetLocationByXYZ(coords.x, coords.y, coords.z);
		if(pLocationInfo != NULL){
			apLocation->ID = pLocationInfo->name.c_str();
			apLocation->params = pLocationInfo->params.c_str();
		}else{
			apLocation->ID = L"";
			apLocation->params = L"";
		}
	}

	C3DObject* obj = NULL;
	
	if (checkObjects.size() == 0)
	{
		obj = m_viewport->GetObjectUnderMouse(abGroundOnly);	
	}
	else
	{
		obj = m_viewport->GetObjectUnderMouse(checkObjects);	
	}

	if (obj)
	{
		if ((obj->GetParent()) && (obj->GetParent()->GetAnimation()))
		{
			obj = obj->GetParent();
		}

		if (obj->IsDeleted())
		{				
			return NULL;
		}

		bool isGround = ((int)(obj->GetUserData(USER_DATA_LEVEL)) == LEVEL_GROUND);
		if(abpGround != NULL)
			*abpGround = isGround;

		mlMedia* objectUnderMouse = NULL;
		if (obj->GetUserData(USER_DATA_NRMOBJECT))
		{
			objectUnderMouse = ((nrmObject *)obj->GetUserData(USER_DATA_NRMOBJECT))->GetMLMedia();
		}
		else
		{
			objectUnderMouse = active_camera->GetMLMedia();
		}	

		if (obj->GetUserData(USER_DATA_RENDER_TARGET) != NULL)
		{			
			float tx, ty;
			int textureX, textureY;
			int modelID = obj->GetLODGroup()->GetCurrentModelID(m_viewport->GetObjectUnderMouse());
			CModel* model = g->mm.GetObjectPointer(modelID);
			model->GetTextureCoordsForPoint(obj->GetCoords(), obj->GetRotation()->GetAsDXQuaternion(), obj->GetScale3D(), coords.x, coords.y, coords.z, tx, ty, textureX, textureY);
			if (apXYTextureMedia)
			{
				apXYTextureMedia->x = textureX;
				apXYTextureMedia->y = textureY;
			}
			IMouseListener* mouseListener = (IMouseListener *)obj->GetUserData(USER_DATA_RENDER_TARGET);
			if ((!objectUnderMouse) || (!objectUnderMouse->GetBooleanProp("eventsDisabled")))
			{
				gRM->mouseCtrl->UpdateMouseEvent(mouseListener, tx, ty, true);
			}
			else
			{
				gRM->mouseCtrl->DisableKeyboardInput();
				gRM->mouseCtrl->UpdateMouseEvent(NULL, 0, 0, true);
			}

			if (obj->GetUserData(USER_DATA_SHARING_TARGET) != NULL)
			{
				IMouseListener* sharingMouseListener = (IMouseListener *)obj->GetUserData(USER_DATA_SHARING_TARGET);
				gRM->mouseCtrl->UpdateMouseEvent(sharingMouseListener, tx, ty, false);
			}
		}
		else
		{
			gRM->mouseCtrl->UpdateMouseEvent(NULL, 0, 0, true);
		}			

		return objectUnderMouse;
	}
	else
	{	
		return active_camera->GetMLMedia();
	}
}


void nrmViewport::ShowHint()
{
	//hintCloud->render();
}

void nrmViewport::GetRay(mlPoint &aXY, ml3DPosition &aXYZ0, ml3DPosition &aXYZ)
{
	CalVector ro, rd;
	POINT tmp;
	tmp.x = aXY.x;
	tmp.y = aXY.y;

	//active_camera->picamera->get_ray(ro, rd, tmp);

	aXYZ0.x = ro.x;
	aXYZ0.y = ro.y;
	aXYZ0.z = ro.z;

	aXYZ.x = rd.x;
	aXYZ.y = rd.y;
	aXYZ.z = rd.z;
}

bool nrmViewport::GetProjection(ml3DPosition &aXYZ, mlPoint &aXY)
{
	if (!m_viewport->GetCamera())
	{
		return false;
	}

	float x, y;
	if (m_viewport->GetScreenCoords((float)aXYZ.x, (float)aXYZ.y, (float)aXYZ.z, &x, &y))
	{
		aXY.x = (int)x;
		aXY.y = (int)y;

		return true;
	}
	else
	{
		return false;
	}
}

bool nrmViewport::GetProjection(mlMedia* /*ap3DO*/, mlPoint& /*aXY*/, mlRect& /*aBounds*/)
{
	/*icamera* picamera;
	if ((picamera=piviewport->get_camera())==NULL) return false;	// no camera on viewport
	if( ap3DO)
	{
		if( ap3DO->GetType()==MLMT_OBJECT || ap3DO->GetType()==MLMT_CHARACTER)
		{
			nrm3DObject*	pnrm3DObject = (nrm3DObject*)ap3DO->GetSafeMO();
			if( !pnrm3DObject)	return false;
			//CVisibleObject*	pVO = pnrm3DObject->get_n3dobj();
			icollisionobject*	apicollisionobject;
			GET_INTERFACE(icollisionobject, pnrm3DObject->get_pibase(), &apicollisionobject);
			if( !apicollisionobject)
			{
				rm_trace("Error: No media to get projection on\n");
				return false;
			}
			CalVector	point3d = apicollisionobject->get_translation();
			box3d		bbox = apicollisionobject->get_world_bounds();

			vertex4d	ponscreen = picamera->get_screenXY(point3d);
			aXY.x = ponscreen.x;
			aXY.y = ponscreen.y;

			RECT	rectonscreen;
			picamera->get_screen_rect(bbox, &rectonscreen);
			aBounds.left = rectonscreen.left;
			aBounds.right = rectonscreen.right;
			aBounds.top = rectonscreen.top;
			aBounds.bottom = rectonscreen.bottom;
		}
	}*/
	return true;
}

/*void nrmViewport::CollectChildren(std::vector<ivisibleobject*>& array, mlMedia** ppmlMedia)
{
	while( *ppmlMedia != NULL)
	{
		mlMedia*	pmlMedia = *ppmlMedia;
		moMedia*	pmoMedia = pmlMedia->GetSafeMO();
		if(pmlMedia->GetType()==MLMT_OBJECT || pmlMedia->GetType()==MLMT_CHARACTER)
		{
			iobject3d* apiobject3d;
			if( (apiobject3d=((nrm3DObject*)pmoMedia)->piobject3d) != NULL)
			{
				ivisibleobject* apivisibleobject;
				if( apiobject3d->query_interface(iid_ivisibleobject, (void**)&apivisibleobject))
				{
					array.push_back( apivisibleobject);
				}
			}
		}
		else	if(pmlMedia->GetType()==MLMT_GROUP)
		{
			mlMedia**	ppmlChildren = pmoMedia->GetMLMedia()->GetChildren();
			if (ppmlChildren)
			{
				CollectChildren(array, ppmlChildren);
			}
			else
			{
				igroup3d* apigroup3d;
				if( (apigroup3d=((nrm3DGroup*)pmoMedia)->pigroup3d) != NULL)
				{
					ivisibleobject* apivisibleobject;
					if( apigroup3d->query_interface(iid_ivisibleobject, (void**)&apivisibleobject))
					{
						array.push_back( apivisibleobject);
					}
				}
			}
			GetMLMedia()->FreeChildrenArray(ppmlChildren);
		}
		ppmlMedia++;
	}
}*/

void nrmViewport::InclExclChanged()
{
/*	ATLASSERT(piviewport);
	if(!piviewport)	return;

	mlIViewport*	pmlIViewport = GetMLMedia()->GetIViewport();

	vIncludes.clear();
	vExcludes.clear();

	mlMedia**	ppmlExcludes = pmlIViewport->GetExclude();
	if( ppmlExcludes)
	{
		CollectChildren(vExcludes, ppmlExcludes);
		piviewport->set_excludes(&vExcludes);
	}
	else
	{
		piviewport->set_excludes(NULL);
	}
	mlMedia**	ppmlIncludes = pmlIViewport->GetInclude();
	if( ppmlIncludes)
	{
		CollectChildren(vIncludes, ppmlIncludes);
		piviewport->set_includes(&vIncludes);
	}
	else
	{
		piviewport->set_includes(NULL);
	}*/
}

void nrmViewport::SoundVolumeChanged()
{
	// ??
}

void nrmViewport::TraceObjectOnMouseMoveChanged()
{
	mlIViewport* pMLIViewport = mpMLMedia->GetIViewport();
	bool traceObjectOnMouseMove = pMLIViewport->GetTraceObjectOnMouseMove();
	
	if (traceObjectOnMouseMove)
	{
		gRM->mouseCtrl->DisableMouseOptimization();
	}
	else
	{
		gRM->mouseCtrl->EnableMouseOptimization();
	}
}

mlRect nrmViewport::GetBounds(mlMedia* /*apObject*/)
{
	assert(false);
	mlRect rcBounds; ML_INIT_RECT(rcBounds);

	return rcBounds;
}

// получить точку пересечения луча с экрана с плоскостью, заданной точкой и нормалью
ml3DPosition nrmViewport::GetIntersectionWithPlane(mlPoint aXY, ml3DPosition aPlane, ml3DPosition aNormal, bool& abBackSide)
{
	abBackSide = false;
	ml3DPosition posRet; ML_INIT_3DPOSITION(posRet);

	if (!m_viewport)
	{
		return posRet;
	}

	if (!m_viewport->GetCamera())
	{
		return posRet;
	}

	CPlane3D plane(CVector3D((float)aNormal.x, (float)aNormal.y, (float)aNormal.z), CVector3D((float)aPlane.x, (float)aPlane.y, (float)aPlane.z));

	CVector3D point;
	m_viewport->GetZNearCoords(aXY.x, aXY.y, point.x, point.y, point.z);

	CVector3D delta = point - (m_viewport->GetCamera()->GetEyePosition() + CVector3D(0, 0, m_viewport->GetCamera()->GetZOffset()));
	abBackSide = (delta.z > 0);
	CVector3D res;
	if (plane.GetIntersection((m_viewport->GetCamera()->GetEyePosition() + CVector3D(0, 0, m_viewport->GetCamera()->GetZOffset())), delta, res))
	{
		posRet.x = res.x;
		posRet.y = res.y;
		posRet.z = res.z;
}	

	return posRet;
}

// получить точку проекции 3D-точки на землю с учетом гравитации
ml3DPosition nrmViewport::GetProjectionToLand(ml3DPosition a3DPoint)
{
	ml3DPosition posRet; ML_INIT_3DPOSITION(posRet);
	CVector3D res = g->phom.GetProjectionToLand(CVector3D((float)a3DPoint.x, (float)a3DPoint.y, (float)a3DPoint.z));
	posRet.x = res.x;
	posRet.y = res.y;
	posRet.z = res.z;

	return posRet;
}

// получить точку проекции 3D-точки на землю с учетом объектов
ml3DPosition nrmViewport::GetProjectionToObjects(ml3DPosition a3DPoint)
{
	ml3DPosition posRet; ML_INIT_3DPOSITION(posRet);
	float resultZ;
	CVector3D res = g->phom.GetProjectionToLand(CVector3D((float)a3DPoint.x, (float)a3DPoint.y, (float)a3DPoint.z));
	if (!g->phom.GetProjectionToObjects(CVector3D((float)a3DPoint.x, (float)a3DPoint.y, a3DPoint.z), resultZ))
	{
		resultZ = GetProjectionToLand(a3DPoint).z;
	}
	posRet.x = a3DPoint.x;
	posRet.y = a3DPoint.y;
	posRet.z = resultZ;

	return posRet;
}

// нажали где-то (apntXY) на viewport-е
void nrmViewport::onPress(mlPoint /*apntXY*/)
{
}

void nrmViewport::SetLODScale(float lodScale)
{
	g->lod.SetLODScale(lodScale);
}

// отпустили кнопку мыши
void nrmViewport::onRelease()
{

}

BOOL	 nrmViewport::render()
{
	return true;
}

nrmViewport::~nrmViewport()
{
	m_pRenderManager->UnregisterViewport(this);

	if (m_viewport)
	{
		std::vector<CScene *>::iterator it = g->scm.GetLiveObjectsBegin();
		std::vector<CScene *>::iterator itEnd = g->scm.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		if ((*it)->GetViewport() == m_viewport)
		{
			(*it)->SetViewport(NULL);
		}

		m_viewport->SetCamera(NULL);
		MP_DELETE(m_viewport);
	}

	m_pRenderManager->ShowHint(this, FALSE);
}