// nrmLight.h: interface for the nrmLight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmLight_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmLight_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#include "nrmObject.h"

//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class nrmLight :	public nrmObject,
					public moI3DObject

{
public:
	nrmLight(mlMedia* apMLMedia);
	virtual ~nrmLight();

// реализаци€ moMedia
public:
	moILoadable* GetILoadable(){ return this; }
	moI3DObject* GetI3DObject(){ return this; }
// реализаци€  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализаци€  moI3DObject
public:
	void showAxis() {}
	void hideAxis() {}
	
	int GetCurrentLevel() { return 0; }
	void LevelChanged(int level) {}

	// —войства
	ml3DPosition nrmLight::GetPos();
	void PosChanged(ml3DPosition &pos);
	void RotationChanged(ml3DRotation &rot);
	ml3DScale GetScale(); 
	void ScaleChanged(ml3DScale &scl);
	ml3DRotation GetRotation(); 
	void VisibleChanged(); // изменилась видимость
	void InCameraSpaceChanged() {}
	void CheckCollisionsChanged(); // изменилс€ флаг проверки на пересечени€
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
	// ћетоды
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
		// установка скорости движени€ объекта вдоль пути (%/сек)
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
	ml3DBounds getSubobjBounds(int /*aiMatID*/){
		ml3DBounds bnd; ML_INIT_3DBOUNDS(bnd); return bnd;
	}
	int moveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions);
	ml3DPosition getAbsolutePosition();
	ml3DRotation getAbsoluteRotation(); 
	bool attachSound(mlMedia* /*pAudio*/, ml3DPosition& /*pos3D*/){ return false; }
	bool detachSound(mlMedia* /*pAudio*/){ return false; }
	bool addPotentialTexture(mlMedia* /*apVisibleObject*/){ return false; }
	bool addPotentialTexture(const wchar_t* /*apwcSrc*/){ return false; }
	void BillboardChanged(){}
	void UnpickableChanged(){}
	void SetMaterialTiling(int, float)
	{
	}
	void EnableShadows(bool /*isEnabled*/){}
	void ChangeCollisionLevel(int /*level*/){}
	bool HasAttachParent() { return false;}
	// Authoring
	bool Get3DPropertiesInfo(mlSynchData& /*aData*/){ return false; }
	bool Get3DProperty(char* /*apszName*/, char* /*apszSubName*/, mlOutString& /*sVal*/){ return false; }
	bool Set3DProperty(char* /*apszName*/, char* /*apszSubName*/, char* /*apszVal*/){ return false; }
	void SetMaterialDescription(int /*aiMatIdx*/, const wchar_t* /*apwcDescr*/){}
	std::string getAllTextures(){return "";}
	int detectObjectMaterial(int /*x*/, int /*y*/){return -1;}
	bool IsFullyLoaded() {return false;}
	bool IsIntersectedWithOtherObjects() {return false;}

	// ѕолучить путь к файлу карты текстуры, если он доступен через ResManager
	virtual bool GetMaterialTextureSrc(int /*aiMatIdx*/, const wchar_t* /*apwcMapName*/, mlOutString & /*asSrc*/){ return false; }
/*
public:
	std::vector<ilight*>&	get_lights(){ return lights;}

protected:
	std::vector<ilight*>	lights;*/
};

#endif // !defined(AFX_nrmLight_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)

