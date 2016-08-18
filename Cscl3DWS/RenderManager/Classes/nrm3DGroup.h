// nrm3DGroup.h: interface for the nrm3DGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrm3DGroup_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrm3DGroup_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "../CommonRenderManagerHeader.h"

#include "nrm3DObject.h"
#include "iasynchresourcehandler.h"

class n3dGroup;

class nrm3DGroup :	public nrm3DObject,
					public moI3DGroup
{
public:
	nrm3DGroup(mlMedia* apMLMedia);
	virtual ~nrm3DGroup();


public:
	void OnChanged(int eventID);

	// реализация iasynchresourcehandler
public:
	// получает прогресс загрузки объекта
	virtual void OnAsynchResourceLoadedPersent(IAsynchResource* asynch, byte percent);
	// объект загружен
	virtual void OnAsynchResourceLoaded(IAsynchResource* asynch);
	// ошибка загрузки
	virtual void OnAsynchResourceError(IAsynchResource* asynch);

// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }
	moI3DObject* GetI3DObject(){ return this; }
	moIButton* GetIButton(){ return this; }

// реализация moI3DGroup
	void ScriptedChanged();
public:
	bool ChildAdded(mlMedia* apChild);
	bool ChildRemoving(mlMedia* apChild);

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализация  moI3DObject
public:
	// Свойства
	void PosChanged(ml3DPosition &pos); // изменилось положение 
	void ScaleChanged(ml3DScale &scl);
	void RotationChanged(ml3DRotation &rot);
	//void VisibleChanged(); // изменилась видимость
	void CheckCollisionsChanged(); // изменился флаг проверки на пересечения
	virtual void update(DWORD dwTime, void *data);
	// Some STUBS
public:
	void LevelChanged(int /*level*/){};
	int GetCurrentLevel() { return 0; }
	void	BoundingBoxChanged(){}
	void	ShadowChanged(){}
	void	MaterialsChanged(){}
	void	MaterialChanged(unsigned /*auID*/){}
	void	MaterialMapChanged(unsigned /*auID*/, const wchar_t* /*apwcIndex*/){}
	void	MotionChanged(){}
	void	GravitationChanged(){}
	void	IntangibleChanged(){}
	bool	getCollisions(mlMedia**& /*avObjects*/, int& /*aiLength*/, ml3DPosition& /*aPos3D*/){ return false; }
	bool	getIntersections(mlMedia**& /*avObjects*/, int& /*aiLength*/){ return false; }
	void	doMotion(mlMedia* /*apMotion*/){}
	void	setMotion(mlMedia* /*apMotion*/, int /*aiDurMS*/){}
	void	removeMotion(mlMedia* /*apMotion*/){}
	bool	getIntersectionsIn(mlMedia**& /*avObjects*/, int& /*aiLength*/){ return false; }
	bool	getIntersectionsOut(mlMedia**& /*avObjects*/, int& /*aiLength*/){ return false; }
	void	goPath(mlMedia* /*apPath*/, int /*aiFlags*/){}
	moIMaterial*	GetMaterial(unsigned /*auID*/){ return NULL; }
	// Методы
	mlMedia* getPath(){ return NULL; }
		// установка скорости движения объекта вдоль пути (%/сек)
	void setPathVelocity(float /*afVelocity*/){}
		// установка котрольных точек (в %-ах пути), на которых будет срабатывать
		// onPassed. getPassedCheckPoint() вернет значение контрольной точки
	void setPathCheckPoints(float* /*apfCheckPoints*/, int /*aiLength*/){}
	float getPassedCheckPoint(){ return 0.0; }
	void setPathPercent(float /*afPercent*/){}
	float getPathPercent(){ return 0.0; }
	bool attachTo(mlMedia* /*ap3DODest*/, const char* /*apszBoneName*/){ return false; }
	// Authoring
	bool Get3DPropertiesInfo(mlSynchData& /*aData*/){ return false; }
	bool Get3DProperty(char* /*apszName*/, char* /*apszSubName*/, mlOutString& /*sVal*/){ return false; }
	bool Set3DProperty(char* /*apszName*/, char* /*apszSubName*/, char* /*apszVal*/){ return false; }

// реализация  moIButton
public:
	void onKeyDown(){}
	void onKeyUp(){}
	void onSetFocus(){}
	void onKillFocus(){}
	void AbsEnabledChanged(){}

public:
	//	нотификация об удалении скриптового объекта
	void	auto_scripted_object_is_destroing(nrm3DObject* apnrm3DObject);

protected:
	// создает rmml-скриптуемый объект для объекта Natura3D
	bool CreateRMMLObjectFor3DObject(C3DObject* obj3d, const char* name);
	// удаляет все скриптовые элементы из группы
	bool ClearScriptedInternalObjects();
	// удаляет скриптовый элемент из группы
	bool ClearScriptedInternalObject(C3DObject* obj3d);
	// скриптует объекты, записанные в конфиге
//	void ScriptSelfChildren();

	// список RMML объектов - которые находятся в группе. Включает статически прописанные, 
	// динамически и заскриптованные автоматически объекты
	std::vector<C3DObject *> auto_scripted_object;

	// добавляет все объекты, которые прописаны в скрипте к себе
	void AttachChildrenFromScript();

public:
	bool AttachScriptedChild(mlMedia* apChild);

private:
	bool DetachScriptedChild(mlMedia* apChild);
	void LoadGroup(ifile* file);

	MP_VECTOR<C3DObject *> m_childObjects;
	bool m_rmmlLoadAllCalled; // (затычка) флаг, что onLoad(RMML_LOAD_STATE_ALL_RESOURCES_LOADED) у rmml-объекта уже вызывался
};

#endif // !defined(AFX_nrm3DGroup_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
