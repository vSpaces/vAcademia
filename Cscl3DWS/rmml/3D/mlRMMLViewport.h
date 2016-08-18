#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLViewport_H_
#define _mlRMMLViewport_H_

namespace rmml {

class mlRMMLCamera;
#define MLELPN_CAMERA "_camera"
#define EVNM_onHintShown L"onHintShown"
#define EVNM_onShowHint L"onShowHint"
#define EVNM_onHideHint L"onHideHint"

/**
 * Класс вьюпорта для 3D-сцен
 */

class mlRMMLViewport :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLVisible,
					public mlRMMLPButton,
					public mlIViewport,
					public moIViewport
{
public:
	mlRMMLViewport(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLViewport(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP__camera = 1,
		JSPROP_show_hints,
		JSPROP_include,
		JSPROP_exclude,
		JSPROP_soundVolume,
		JSPROP_traceObjectOnMouseMove,
		EVID_onHintShown,
		EVID_onShowHint,
		EVID_onHideHint
	};
	JSString* _camera;
	mlRMMLCamera* mpCamera;
	bool show_hints;
	bool mbExclude;
	JSObject* mjsoInclExclArr;
	bool mbProbablyHandlerFuncIsSet;	// возможно установлена функция-обработчик
	class mlStringVector: public std::vector<void*>{
		typedef std::vector<void*> baseClass;
	public:
		typedef mlString** iterator;
		typedef mlString*& reference;
		virtual ~mlStringVector(){
			clear();
		}
		void clear(){ 
			baseClass::iterator vi;
			for(vi = baseClass::begin(); vi != baseClass::end(); vi++){
				mlString* psStr = (mlString*)(void*)*vi;
				MP_DELETE_ARR( psStr);
			}
			baseClass::clear();
		}

#if _MSC_VER >= 1600
		iterator begin(){ return ((iterator)baseClass::begin()._Ptr); }
		iterator end(){ return ((iterator)baseClass::end()._Ptr); }
#elif _MSC_VER > 1200
		iterator begin(){ return ((iterator)baseClass::begin()._Myptr); }
		iterator end(){ return ((iterator)baseClass::end()._Myptr); }
#else
		iterator begin(){ return ((iterator)baseClass::begin()); }
		iterator end(){ return ((iterator)baseClass::end()); }
#endif
		void push_back(const mlString& asStr){
			MP_NEW_P( psNewStr, mlString, asStr);
			baseClass::push_back(psNewStr);
		}
		reference back(){ return (reference)baseClass::back(); }
		reference front(){  return (reference)baseClass::front(); }
		void pop_back(){
			MP_DELETE( back());
			baseClass::pop_back();
		}
		reference operator[](size_type _Pos){
			return (reference)(*(baseClass*)this)[_Pos];
		}
	} mvInclExcl;
	MP_VECTOR<mlMedia*> mvInclExclOut;
	double soundVolume;
	bool traceObjectOnMouseMove;

//	void SetCamera(JSString* ajssName);

	JSFUNC_DECL(getObjectIn)
	JSFUNC_DECL(getProjection)
	JSFUNC_DECL(getBounds)
	JSFUNC_DECL(getRay)
	JSFUNC_DECL(getIntersectionWithPlane) // получить точку пересечения луча с экрана с плоскостью, заданной точкой и нормалью
	JSFUNC_DECL(getProjectionToLand) // получить точку проекции 3D-точки на землю с учетом гравитации
	JSFUNC_DECL(getProjectionToObjects) // получить точку проекции 3D-точки на землю с учетом объектов
	JSFUNC_DECL(setLODScale)
	JSFUNC_DECL(getCloudIn)	// получить по 2D-координатам cloud и видимый RMML-элемент в нем

	void FillInclExclOut();
	mlRMMLElement* Resolve3DObject(const wchar_t* apwcRef);
	void ResolveInclExcl();
	JSBool ParseInclExcl(wchar_t* apwcValue, bool abExclude);
	JSBool SetInclExclProp(jsval ajsv, bool abExclude);

public:
	void SetTraceObjectOnMouseMove(bool abValue);
	bool GetTraceObjectOnMouseMoveValue();

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	EventSpec* GetEventSpec2(){ return _EventSpec; } 
	mlresult CreateMedia(omsContext* amcx);
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlRMMLElement* GetButtonUnderMouse(mlPoint &aMouseXY, mlButtonUnderMouse &aBUM);
	mlRMMLElement* GetRefPropContext(const wchar_t* apwcName);
	bool SetEventData(char aidEvent, mlSynchData &Data);
	void UnknownPropIsSet();

// реализация mlRMMLVisible
public:
	mlRMMLElement* GetVisibleAt(const mlPoint aPnt, mlIVisible** appVisible=NULL);
	bool CallOnBeforePress(mlButtonUnderMouse &aBUM); // mlRMMLElement* ap3DObject=NULL, ml3DPosition* apXYZ=NULL, mlRMMLElement* apTextureObject=NULL, mlPoint* appntTextureXY=NULL);

// реализация  mlIVisible
MLIVISIBLE_IMPL

// реализация  mlIButton
MLIBUTTON_IMPL
	virtual void OnPress(mlPoint apntXY){ onPress(apntXY); }
	virtual void OnRelease(){ onRelease(); }

// реализация mlIViewport
	mlIViewport* GetIViewport(){ return this; }
	// Дочерние элементы
	mlMedia* GetHint();
	// Свойства
	mlMedia* GetCamera();
	mlMedia** GetInclude();
	mlMedia** GetExclude();
	double GetSoundVolume();
	bool GetTraceObjectOnMouseMove(){ return traceObjectOnMouseMove; }
	// События
	void onHintShown(mlMedia* apObject);
	void onShowHint(mlMedia* apObject, mlPoint aScreenXY, const wchar_t* apwcText);
	void onHideHint();
	

// реализация moIViewport
	mlMedia* GetObjectUnderMouse(mlPoint &aMouseXY, bool abGroundOnly, ml3DPosition *apXYZ=NULL, ml3DLocation *apLocation=NULL, 
		int *apiType=NULL, ml3DPosition *apXYTexture=NULL, mlMedia** apaCheckObjects=NULL, 
		mlMedia** appTextureMedia=NULL, mlPoint* apXYTextureMedia=NULL, bool* abpGround=NULL);
	bool GetProjection(ml3DPosition &aXYZ, mlPoint &aXY);
	bool GetProjection(mlMedia* ap3DO, mlPoint &aXY, mlRect &aBounds);
	void GetRay(mlPoint& aXY, ml3DPosition &aXYZ0, ml3DPosition &aXYZ);
	void SetLODScale(float lodScale);
	// Свойства
	void CameraChanged();
	void InclExclChanged();
	void SoundVolumeChanged();
	void TraceObjectOnMouseMoveChanged();
	// События
	void onPress(mlPoint apntXY);	// нажали где-то (apntXY) на viewport-е
	void onRelease();				// отпустили кнопку мыши
	// Методы
	mlRect GetBounds(mlMedia* apObject);
	ml3DPosition GetIntersectionWithPlane(mlPoint aXY, ml3DPosition aPlane, ml3DPosition aNormal, bool& abBackSide);
	// получить точку проекции 3D-точки на землю с учетом гравитации
	ml3DPosition GetProjectionToLand(ml3DPosition a3DPoint);
	// получить точку проекции 3D-точки на землю с учетом объектов
	ml3DPosition GetProjectionToObjects(ml3DPosition a3DPoint);
	bool GetObjectStringIDAt(mlPoint aXY, mlOutString& asStringID);

};

}

#endif
