// nrmCamera.h: interface for the nrmCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmCamera_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmCamera_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nrmanager.h"
#include "nrmObject.h"
#include "Camera.h"
#include "CameraTarget.h"

class CCameraController;

class nrmCamera :	public nrmObject,
					public moI3DObject,
					public moIButton,
					public moICamera
{
public:
	nrmCamera(mlMedia* apMLMedia);
	virtual ~nrmCamera();

// реализация moMedia
public:
	moILoadable*	GetILoadable()	{ return this; }
	moIButton*		GetIButton()	{ return this; }
	moICamera*		GetICamera()	{ return this; }
	moI3DObject*	GetI3DObject()	{ return this; }
	void			PropIsSet();

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализация  moIButton
public:
	void onKeyDown();
	void onKeyUp();	
	void onSetFocus(){}
	void onKillFocus(){}
	void AbsEnabledChanged(){}

// реализация  moI3DObject
public:
	void showAxis() {}
	void hideAxis() {}
	
	int GetCurrentLevel() { return 0; }
	void LevelChanged(int level) {}

	// Свойства
	CCameraController* GetCameraController();

	ml3DPosition GetPos();
	void PosChanged(ml3DPosition &pos);
	void RotationChanged(ml3DRotation &rot);
	ml3DScale GetScale(); 
	void ScaleChanged(ml3DScale &scl);
	ml3DRotation GetRotation(); 
	void VisibleChanged(); // изменилась видимость
	void InCameraSpaceChanged(){}
	void CheckCollisionsChanged(){} // изменился флаг проверки на пересечения
	void BoundingBoxChanged(){}
	void ShadowChanged(){}
	moIMaterial* GetMaterial(unsigned /*auID*/){ return NULL; }
	void MaterialsChanged(){}
	void MaterialChanged(unsigned /*auID*/){}
	void MaterialMapChanged(unsigned /*auID*/, const wchar_t* /*apwcIndex*/){}
	void MotionChanged(){}
	void GravitationChanged(){}
	void IntangibleChanged(){}
	void BoundingBoxColorChanged(){}
	bool getCollisions(mlMedia**& /*avObjects*/, int& /*aiLength*/, ml3DPosition& /*aPos3D*/){ return false; }
	bool getIntersections(mlMedia**& /*avObjects*/, int& /*aiLength*/){ return false; }
	void ColorChanged(){}
	void OpacityChanged(){}
	// Методы
	void doMotion(mlMedia* /*apMotion*/){}
	void setMotion(mlMedia* /*apMotion*/, int /*aiDurMS*/){}
	void removeMotion(mlMedia* /*apMotion*/){}
	bool getIntersectionsIn(mlMedia**& /*avObjects*/, int& /*aiLength*/){ return false; }
	bool getIntersectionsOut(mlMedia**& /*avObjects*/, int& /*aiLength*/){ return false; }
	void goPath(mlMedia* /*apPath*/, int /*aiFlags*/){}
	void goPath(ml3DPosition* /*apPath*/, int /*aiPointCount*/, unsigned int /*auShiftTime*/){}
	mlMedia* getPath(){ return NULL; }
		// найти путь до указанного места
	bool findPathTo(ml3DPosition &/*aTo*/, ml3DPosition &/*aFrom*/, ml3DPosition* &/*avPath*/, int &/*aiLength*/){ return false; }
		// установка скорости движения объекта вдоль пути (%/сек)
	void setPathVelocity(float /*afVelocity*/){}
		// установка котрольных точек (в %-ах пути), на которых будет срабатывать
		// onPassed. getPassedCheckPoint() вернет значение контрольной точки
	void setPathCheckPoints(float* /*apfCheckPoints*/, int /*aiLength*/){}
	float getPassedCheckPoint(){ return 0.0; }
	void setPathPercent(float /*afPercent*/){}
	float getPathPercent(){ return 0.0; }
	bool attachTo(mlMedia* /*ap3DODest*/,const char* /*apszBoneName*/){ return false; }
	ml3DBounds getBounds(mlMedia* /*ap3DOCoordSysSrc*/){
		ml3DBounds bnd; ML_INIT_3DBOUNDS(bnd); return bnd;
	}
	// получение bounds-ов подобъекта, на который натянут материал с идентификатором aiMatID
	ml3DBounds getSubobjBounds(int /*aiMatID*/){
		ml3DBounds bnd; ML_INIT_3DBOUNDS(bnd); return bnd;
	}
	int moveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions);
	ml3DPosition getAbsolutePosition();
	ml3DRotation getAbsoluteRotation(); 
	std::string getAllTextures(){return "";}
	int detectObjectMaterial(int x, int y){return -1;}
	bool attachSound(mlMedia* /*pAudio*/, ml3DPosition& /*pos3D*/){ return false; }
	bool detachSound(mlMedia* /*pAudio*/){ return false; }
	bool addPotentialTexture(mlMedia* /*apVisibleObject*/){ return false; }
	bool addPotentialTexture(const wchar_t* /*apwcSrc*/){ return false; }
	void BillboardChanged(){}
	void UnpickableChanged(){}
	bool HasAttachParent() { return false;}
	// Authoring
	bool Get3DPropertiesInfo(mlSynchData& aData);
	bool Get3DProperty(char* apszName,char* apszSubName, mlOutString &sVal);
	bool Set3DProperty(char* apszName,char* apszSubName,char* apszVal);
	virtual	void	OnSetRenderManager();
	void SetMaterialDescription(int /*aiMatIdx*/, const wchar_t* /*apwcDescr*/){}

	// Получить путь к файлу карты текстуры, если он доступен через ResManager
	virtual bool GetMaterialTextureSrc(int aiMatIdx, const wchar_t* apwcMapName, mlOutString &asSrc){ return false; }

	void OnCameraChangedTo(nrmCamera* cam);

// реализация  moICamera
public:
	void LinkToChanged();
	void FOVChanged();
	void FixedPositionChanged();
	void DestChanged();
	void DestOculusChanged();
	void UpChanged();
	void ResetDestOculus();
	ml3DPosition GetDest();
	ml3DPosition GetDestOculus(); 
	ml3DPosition GetUp(); 
	void MaxLinkDistanceChanged();
	void SetNearPlane(float nearPlane);
	int destMoveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions);
	void Rotate(int aiDirection, int aiTime);
	void Move(int aiDirection, int aiTime);
	bool LoadCamera(ifile* file);
	bool IsFullyLoaded() {return false;}
	bool IsIntersectedWithOtherObjects() {return false;}
		// воспроизводит движение камеры по треку из файла
	void PlayTrackFile(const wchar_t* apwcFilePath);

	float GetZLevel();

	void ClearOffset();
	bool HasOffset();

	void CorrectPosition();

	void SetMaterialTiling(int, float)
	{
	}

	void EnableShadows(bool isEnabled){}
	void ChangeCollisionLevel(int level){}

public:
	void					SetDefaultCam();
	CCamera3D*				m_camera;

protected:
	CCameraController*		m_cameraController;
	CCameraTarget			m_cameraTarget;

	CCameraController*		GetMovingController();

};

#endif // !defined(AFX_nrmCamera_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
	