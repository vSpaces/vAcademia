#pragma once

#include "mlRMMLPosition3D.h"
#include "mlRMMLScale3D.h"
#include "mlRMMLRotation3D.h"
#include "mlRMMLMaterials.h"

namespace rmml {

#define EVNM_onMouseDown L"onMouseDown"
#define EVNM_onRMouseDown L"onRMouseDown"
#define EVNM_onMouseMove L"onMouseMove"
#define EVNM_onRMouseUp L"onRMouseUp"
#define EVNM_onMouseUp L"onMouseUp"
#define EVNM_onPassed L"onPassed"
#define EVNM_onCollision L"onCollision"

#define SET_GET_POS_C(C) \
	void SetPos_##C(double aiNewVal){ \
		Create3DPos(); mp3DPSR->pPos->C=aiNewVal; \
		PosChanged(*(mp3DPSR->pPos)); \
	} \
	double GetPos_##C(){ return GetPos().C; }

#define SET_GET_SCL_C(C) \
	void SetScl_##C(double aiNewVal){ \
		Create3DScl(); mp3DPSR->pScl->C=aiNewVal; \
		ScaleChanged(*(mp3DPSR->pScl)); \
	} \
	double GetScl_##C(){ return GetScale().C; }

#define SET_GET_ROT_C(C) \
	void SetRot_##C(double aiNewVal){ \
		Create3DRot(); mp3DPSR->pRot->C=aiNewVal; \
		RotationChanged(*(mp3DPSR->pRot)); \
	} \
	double GetRot_##C(){ return GetRotation().C; }

class mlRMMLMotion;
class mlRMMLShadows3D;

/**
 * RMML-подкласс для 3D-объектов RMML-классов
 * (object, group, light, camera, character)
 * (не путать с прототипами JavaScript!)
 */
class mlRMML3DObject: 
					//public mlJSClassProto,
					public mlI3DObject,
					public moI3DObject,
					public mlIColorCL
{
friend class mlRMMLButton;
friend class mlRMMLElement;
public:
	mlRMML3DObject(void);
	~mlRMML3DObject(void);
MLJSPROTO_DECL
private:
	enum {JSPROP0_mlRMML3DObject=TIDB_mlRMML3DObject-1,
		JSPROP__pos,
		JSPROP__scale,
		JSPROP__rotation,
		JSPROP__visible,
		JSPROP__mouse,
		JSPROP_intersections,
		JSPROP_checkCollisions,
		JSPROP_boundingBox,
		JSPROP_boundingBoxColor,
		JSPROP_level,
		JSPROP_inCameraSpace,
		TIDE_mlRMML3DObject
	};
public:
	enum { EVID_mlRMML3DObject=TIDE_mlRMML3DObject-1,
		EVID_onMouseDown,
		EVID_onRMouseDown,
		EVID_onMouseMove,
		EVID_onRMouseUp,
		EVID_onMouseUp,
		EVID_onPassed,
		EVID_onCollision,
		TEVIDE_mlRMML3DObject
	};
	enum {
		EVSM_onPassed = (1 << 6)
	};
protected:
	JSFUNC_DECL(doMotion)
	void doMotion(mlRMMLMotion* apMotion);
	JSFUNC_DECL(setMotion)
	void setMotion(mlRMMLMotion* apMotion,int aiDurMS);
	JSFUNC_DECL(removeMotion)
	void removeMotion(mlRMMLMotion* apMotion);
	JSFUNC_DECL(getCollisions)
	JSFUNC_DECL(getIntersectionsIn)
	JSFUNC_DECL(getIntersectionsOut)
	JSFUNC_DECL(findPathTo)	// найти свободный путь из одной точки в другую
	JSFUNC_DECL(goPath)
	JSFUNC_DECL(getPath)
	JSFUNC_DECL(setPathCheckPoints)
	JSFUNC_DECL(getPathPercent)
	JSFUNC_DECL(setPathPercent)
	JSFUNC_DECL(attachTo)	
	JSFUNC_DECL(getBounds)
	JSFUNC_DECL(moveTo)
	JSFUNC_DECL(getAbsolutePosition)
	JSFUNC_DECL(getAbsoluteRotation)
	JSFUNC_DECL(attachSound)
	JSFUNC_DECL(detachSound)
	JSFUNC_DECL(addPotentialTexture); // добавить потенциальную текстуру
	JSFUNC_DECL(setBoundingBoxColor)
	JSFUNC_DECL(getAllTextures)
	JSFUNC_DECL(detectObjectMaterial)
	JSFUNC_DECL(setMaterialTiling)
	JSFUNC_DECL(showAxis)
	JSFUNC_DECL(hideAxis)
	JSFUNC_DECL(changeCollisionLevel)
	JSFUNC_DECL(enableShadows)
	JSFUNC_DECL(isFullyLoaded)
	JSFUNC_DECL(isIntersectedWithOtherObjects)
	JSFUNC_DECL(lockPosition)
protected:
	bool mbVisible;
	bool mbInCameraSpace;
	struct ml3DPSR{
		ml3DPosition* pPos;
		struct mlJS3DPos: public mlI3DPosition{
			mlRMML3DObject* mp3DObject;
			JSObject* mjsoPos;
			mlJS3DPos(mlRMML3DObject* ap3DObject);
			// mlI3DPosition
			void SetPos_x(double adNewVal){ mp3DObject->SetPos_x(adNewVal); }
			void SetPos_y(double adNewVal){ mp3DObject->SetPos_y(adNewVal); }
			void SetPos_z(double adNewVal){ mp3DObject->SetPos_z(adNewVal); }
			ml3DPosition GetPos(){ return mp3DObject->GetPos(); }
			void SetPos(ml3DPosition &aPos){ mp3DObject->SetPos(aPos); }
		};
		mlJS3DPos* mpJSPos;
		//
		ml3DScale* pScl;
		struct mlJS3DScl: public mlI3DScale{
			mlRMML3DObject* mp3DObject;
			JSObject* mjsoScl;
			mlJS3DScl(mlRMML3DObject* ap3DObject);
			// mlI3DScale
			void SetScl_x(double adNewVal){ mp3DObject->SetScl_x(adNewVal); }
			void SetScl_y(double adNewVal){ mp3DObject->SetScl_y(adNewVal); }
			void SetScl_z(double adNewVal){ mp3DObject->SetScl_z(adNewVal); }
			ml3DScale GetScl(){ return mp3DObject->GetScale(); }
			void SetScl(ml3DScale &aScl){ mp3DObject->SetScale(aScl); }
		};
		mlJS3DScl* mpJSScl;
		//
		ml3DRotation* pRot;
		struct mlJS3DRot: public mlI3DRotation{
			mlRMML3DObject* mp3DObject;
			JSObject* mjsoRot;
			mlJS3DRot(mlRMML3DObject* ap3DObject);
			// mlI3DRotation
			void SetRot_x(double adNewVal){ mp3DObject->SetRot_x(adNewVal); }
			void SetRot_y(double adNewVal){ mp3DObject->SetRot_y(adNewVal); }
			void SetRot_z(double adNewVal){ mp3DObject->SetRot_z(adNewVal); }
			void SetRot_a(double adNewVal){ mp3DObject->SetRot_a(adNewVal); }
			ml3DRotation GetRot(){ return mp3DObject->GetRotation(); }
			void SetRot(ml3DRotation &aRot){ mp3DObject->SetRotation(aRot); }
		};
		mlJS3DRot* mpJSRot;
		//
		ml3DPSR(){
			pPos=NULL; pScl=NULL; pRot=NULL;
			mpJSPos=NULL;
			mpJSScl=NULL;
			mpJSRot=NULL;
		}
		~ml3DPSR(){
			if(pPos) MP_DELETE( pPos);
			if(mpJSPos) MP_DELETE( mpJSPos);
			if(pScl) MP_DELETE( pScl);
			if(mpJSScl) MP_DELETE( mpJSScl);
			if(pRot) MP_DELETE( pRot);
			if(mpJSRot) MP_DELETE( mpJSRot);
		}
	};
	ml3DPSR* mp3DPSR;
	bool checkCollisions;
	unsigned char boundingBox;
	mlColor boundingBoxColor;
	enum{
		REFF_SHADOW		= 1<<0, // mlRMMLShadows3D-object found (shadow is valid, mjssShadow is invalid)
		REFF_MOTION		= 1<<1,
		REFF_MATERIALS	= 1<<2,
	};
	unsigned char btRefFlags;
	union{
		JSString* mjssShadow;
		mlRMMLShadows3D* shadow;
	};
	union{
		JSString* mjssMotion;
		mlRMMLMotion* motion;
	};
	bool in_motion;
	union{
		JSString* mjssMaterials;
		mlRMMLMaterials* materials;
	};
	bool SetShadowRefJSProp(JSContext *cx, jsval *vp);
	bool SetMaterialsRefJSProp(JSContext *cx, jsval *vp);
	void CreateDefaultMaterials();
	bool SetMotionRefJSProp(JSContext *cx, jsval *vp);
	bool intangible;
	int level;
	bool gravitation;
	mlColor color;	// цвет, который добавляется ко всем материалам объекта в степени, заданной color.a
	unsigned short opacity;	// степень непрозрачности объекта (0..0xFFFF)
public:
	void NotifyShadowsElemDestroyed(mlRMMLShadows3D* apShadowEl){
		if(shadow==apShadowEl) shadow=NULL;
	}

private:
	typedef std::vector<ml3DPosition> PointVectorPath;
	PointVectorPath* mpPVPath;	// последний путь в виде массива трехмерных точек, переданный в RM через goPath
private:
	void Create3DPSR(){ if(mp3DPSR==NULL) mp3DPSR = MP_NEW( ml3DPSR); }
	void Create3DPos(){
		Create3DPSR();
		//if(mp3DPSR->pPos!=NULL) return;
		if(mp3DPSR->pPos==NULL)
			mp3DPSR->pPos = MP_NEW( ml3DPosition);	
		moI3DObject* pmo3D=GetmoI3DObject(); 
		if(pmo3D==NULL){ ML_INIT_3DPOSITION(*(mp3DPSR->pPos));
		}else *(mp3DPSR->pPos)=pmo3D->GetPos();
	}
	void CreateJS3DPos();
	//
	void Create3DScl(){
		Create3DPSR();
		if(mp3DPSR->pScl!=NULL) return;
		mp3DPSR->pScl = MP_NEW( ml3DScale); 
		moI3DObject* pmo3D=GetmoI3DObject(); 
		if(pmo3D==NULL){ ML_INIT_3DSCALE(*(mp3DPSR->pScl));
		}else *(mp3DPSR->pScl)=pmo3D->GetScale();
	}
	void CreateJS3DScl();
	//
	void Create3DRot(){
		Create3DPSR();
		if(mp3DPSR->pRot!=NULL) return;
		mp3DPSR->pRot = MP_NEW( ml3DRotation); 
		moI3DObject* pmo3D=GetmoI3DObject(); 
		if(pmo3D==NULL){ ML_INIT_3DROTATION(*(mp3DPSR->pRot));
		}else *(mp3DPSR->pRot)=pmo3D->GetRotation();
	}
	void CreateJS3DRot();
	void Normalize(ml3DRotation &aRot);
	// реализация mlIColorCL
	void ColorChanged(mlColor* apColor);
public:
	static bool ParsePos(const wchar_t* apwcVal, ml3DPosition* pPos);
	static bool ParseScl(const wchar_t* apwcVal, ml3DScale* pScl);
	static bool ParseRot(const wchar_t* apwcVal, ml3DRotation* pRot);

public:
	virtual mlRMMLElement* GetElem_mlRMML3DObject()=0;
	virtual mlRMMLElement* GetParent_mlRMML3DObject()=0;
	void SetPSR2MO();
	bool O3d_SetEventData(char aidEvent, mlSynchData &Data);
	// установить необходимые свойства в Event на кнопочные события
	virtual void SetButtonEventData(char aidEvent);
	// установить необходимые свойства в Event на события видимого объекта
	void O3d_SetVisibleEventData(char aidEvent);
	//
	void ClearRefs_mlRMML3DObject();
	void ClearIntersections();
	virtual void SetUnpickable(bool abVal){ }
	virtual bool ProbablyHandlerFuncIsSet(){ return false; }
	virtual jsval GetHandlerFunc(const wchar_t* apwcFuncName){ return JSVAL_NULL; }
	void SetColorJSProp(JSContext* cx, jsval aJSVal);
	void GetColorJSProp(JSContext* cx, jsval* apJSVal);
	void SetOpacityJSProp(JSContext* cx, jsval aJSVal);
	void GetOpacityJSProp(JSContext* cx, jsval* apJSVal);
	int getCurrentLevel();
	void levelChanged(int level);

public:

	bool CanHandleEvent(char &aidEvent){
		if(aidEvent < mlRMMLVisible::TEVIDE_mlRMMLVisible && aidEvent > mlRMMLVisible::EVID_mlRMMLVisible){
			if(!GetAbsVisible()) return false;
			aidEvent=(aidEvent - mlRMMLVisible::EVID_mlRMMLVisible) + EVID_mlRMML3DObject;
			O3d_SetVisibleEventData(aidEvent);
			return true;
		}
		if(aidEvent < TEVIDE_mlRMML3DObject && aidEvent > EVID_mlRMML3DObject){
			if(!GetAbsVisible()) return false;
		}
		return true;
	}
	SET_GET_POS_C(x)
	SET_GET_POS_C(y)
	SET_GET_POS_C(z)
	//
	SET_GET_SCL_C(x)
	SET_GET_SCL_C(y)
	SET_GET_SCL_C(z)
	//
	SET_GET_ROT_C(x)
	SET_GET_ROT_C(y)
	SET_GET_ROT_C(z)
	SET_GET_ROT_C(a)

	ml3DPosition GetAbsolutePosition();

// реализация mlI3DObject
public:
	mlMedia* GetParentGroup();
	mlMedia* GetScene3D();
	// Свойства
	bool GetVisible(){ if(GetParent_mlRMML3DObject() == NULL) return false; return mbVisible; }
	bool GetInCameraSpace(){ if(GetParent_mlRMML3DObject() == NULL) return false; return mbInCameraSpace; }
	bool GetCheckCollisions(){ return checkCollisions; }
	int GetBoundingBox(){ return boundingBox; }
	void IntersectionChanged();
	mlMedia* GetShadows(){ if(btRefFlags & REFF_SHADOW)	return (mlMedia*)shadow; return NULL; }
	mlIMaterial* GetMaterial(unsigned char auID);
	mlMedia* GetMotion(){ if(btRefFlags & REFF_MOTION)	return (mlMedia*)motion; return NULL; }
	bool GetGravitation(){ return gravitation; }
	bool GetIntangible(){ return intangible; }
	mlColor GetBoundingBoxColor();
	void SyncronizeCoords(); // нужно синхронизировать координаты
	// События
	void onPassed();
		// объект столкнулся с одним или несколькими объектами
	virtual void onCollision(mlMedia** aapObjects, int aFlags, ml3DPosition endPoint, int pathPointCount, bool isStatic);
	virtual void onLODChanged(double adVisionDistance){}
	// прочие
	bool GetAbsVisible();

// реализация moI3DObject
public:
	moI3DObject* GetmoI3DObject();
	// Свойства
	ml3DPosition GetPos(); 
	void SetPos(ml3DPosition &aPos); 
	virtual void PosChanged(ml3DPosition &pos); // изменилось положение 
	ml3DScale GetScale(); 
	void SetScale(ml3DScale &aScl); 
	void ScaleChanged(ml3DScale &scl);
	ml3DRotation GetRotation(); 
	void SetRotation(ml3DRotation &aRot); 
	void RotationChanged(ml3DRotation &rot);
	void VisibleChanged(); // изменилась видимость
	void InCameraSpaceChanged();
	void CheckCollisionsChanged(); // изменился флаг проверки на пересечения
	void BoundingBoxChanged();
	bool getCollisions(mlMedia** &avObjects, int &aiLength, ml3DPosition &aPos3D);
	bool getIntersections(mlMedia** &avObjects, int &aiLength);
	void ShadowChanged();
	moIMaterial* GetMaterial(unsigned auID);
	void MaterialsChanged();
	void MaterialChanged(unsigned auID);
	void MaterialMapChanged(unsigned auID, const wchar_t* apwcIndex);
	void MotionChanged();
	void GravitationChanged();
	void IntangibleChanged();
	void BoundingBoxColorChanged();
	mlColor GetColor();	// примешиваемый ко всем материалам объекта цвет
	void ColorChanged();
	unsigned char GetOpacity(); // 0..0xFF степень непрозрачности
	void OpacityChanged();
	// Методы
	void doMotion(mlMedia* apMotion){}
	void setMotion(mlMedia* apMotion,int aiDurMS){}
	void removeMotion(mlMedia* apMotion){}
	bool getIntersectionsIn(mlMedia** &avObjects, int &aiLength);
	bool getIntersectionsOut(mlMedia** &avObjects, int &aiLength);
	void goPath(mlMedia* apPath, int aiFlags);
	void goPath(ml3DPosition* apPath, int aiPointCount, unsigned int auShiftTime);
	bool HasAttachParent() { return false;}
	mlMedia* getPath();
		// найти путь до указанного места
	bool findPathTo(ml3DPosition &aFrom, ml3DPosition &aTo, ml3DPosition* &avPath, int &aiLength);
		// установка скорости движения объекта вдоль пути (%/сек)
	void setPathVelocity(float afVelocity);
	void setPathCheckPoints(float* apfCheckPoints, int aiLength);
	float getPassedCheckPoint();
	void setPathPercent(float afPercent);
	float getPathPercent();
	bool attachTo(mlMedia* ap3DODest,const char* apszBoneName);
	ml3DBounds getBounds(mlMedia* ap3DOCoordSysSrc);
		// получение bounds-ов подобъекта, на который натянут материал с идентификатором aiMatID
	virtual ml3DBounds getSubobjBounds(int aiMatID);
	int moveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions);
	ml3DPosition getAbsolutePosition();
	ml3DRotation getAbsoluteRotation(); 
	bool attachSound(mlMedia* pAudio, ml3DPosition& pos3D);
	bool detachSound(mlMedia* pAudio);
	bool addPotentialTexture(mlMedia* apVisibleObject);
	bool addPotentialTexture(const wchar_t* apwcSrc);
	void setMaterialTiling(int textureSlotID, float tilingKoef);
	// Authoring
	bool Get3DPropertiesInfo(mlSynchData &aData);
	bool Get3DProperty(char* apszName,char* apszSubName, mlOutString &sVal);
	bool Set3DProperty(char* apszName,char* apszSubName,char* apszVal);
	// другие
	void SetMaterialDescription(int aiMatIdx, const wchar_t* apwcDescr);
	std::string getAllTextures();
	int detectObjectMaterial(int x, int y);
	void showAxis();
	void hideAxis();
	void changeCollisionLevel(int level);
	void enableShadows(bool isEnabled);
	void enableLockPos(bool isLocked);
	bool isFullyLoaded();
	bool isIntersectedWithOtherObjects();
	// Получить путь к файлу карты текстуры, если он доступен через ResManager
	virtual bool GetMaterialTextureSrc(int aiMatIdx, const wchar_t* apwcMapName, mlOutString &asSrc);

	wchar_t* GetClassName();
private:
	bool m_positionLocked;
};

#define MLI3DOBJECT_IMPL \
public: \
	mlRMMLElement* GetElem_mlRMML3DObject(){ return this; } \
	mlRMMLElement* GetParent_mlRMML3DObject(){ return mpParent; } \
	mlI3DObject* GetI3DObject(){ return this; } \
	mlRMML3DObject* Get3DObject(){ return this; } \
	virtual void ParentNulled(){ VisibleChanged(); }


__forceinline bool ParseMatID(const wchar_t* &apwc, int* apInt){
	if(*apwc == L'\0' || *apwc != L'_')
		return false;
	apwc++;
	if(*apwc == L'\0' || *apwc != L'_')
		return false;
	apwc++;
	if(*apwc>=L'0' && *apwc<=L'9'){
		*apInt=*apwc-L'0';
		apwc++;
	}else return false;
	while(*apwc>=L'0' && *apwc<=L'9'){
		*apInt=*apInt*10+(*apwc-L'0');
		apwc++;
	}
	return true;
}

}
