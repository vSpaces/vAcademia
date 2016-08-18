#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLObject_H_
#define _mlRMMLObject_H_

namespace rmml {

#define EVNM_onLODChanged L"onLODChanged"

/**
 * Класс 3D-объектов RMML
 */

class mlRMMLObject :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMML3DObject,
					public mlRMMLPButton
{
	bool billboard;
	bool unpickable;
	bool mbProbablyHandlerFuncIsSet;	// возможно установлена функция-обработчик
public:
	mlRMMLObject(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLObject(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_shadow = 1,
		JSPROP_materials,
		JSPROP_motion,
		JSPROP_gravitation,
		JSPROP_intangible,
		JSPROP_billboard,
		JSPROP_unpickable, // если true, то не учитывать при поиске объекта под мышкой
		JSPROP_color,		// цвет, который добавляется ко всем материалам объекта в степени, заданной color.a
		JSPROP_opacity,		// степень непрозрачности объекта (0..0xFFFF)
		EVID_onLODChanged,
	};

	JSFUNC_DECL(queryFullStateZatuchka)
	JSFUNC_DECL(getSubobjBounds)

// реализация mlRMML3DObject
	virtual void SetUnpickable(bool abVal){ unpickable = abVal; UnpickableChanged(); }
	virtual bool ProbablyHandlerFuncIsSet(){ return mbProbablyHandlerFuncIsSet; }
	virtual jsval GetHandlerFunc(const wchar_t* apwcFuncName);

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	virtual bool NeedToClearRefs(){ return true; }
	virtual mlRMMLElement* GetElemToClearRefsIn(){ return (mlRMMLElement*)GetScene3D(); }
	virtual void ClearRefs(){ ClearRefs_mlRMML3DObject(); }
	void UnknownPropIsSet(){ mbProbablyHandlerFuncIsSet = true; }
	virtual mlresult Update(const __int64 alTime);
	virtual bool SetEventData(char aidEvent, mlSynchData &Data);
	// установить необходимые свойства в Event на кнопочные события
	virtual void SetButtonEventData(char aidEvent){ mlRMML3DObject::SetButtonEventData(aidEvent); }

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlI3DObject
MLI3DOBJECT_IMPL
	bool GetBillboard(){ return billboard; }
	bool GetUnpickable(){ return unpickable; }
	void onLODChanged(double adVisionDistance);

// реализация  moI3DObject
	void BillboardChanged();
	void UnpickableChanged();

	void SetMaterialTiling(int, float) {};
	void EnableShadows(bool isEnabled){};
	void ChangeCollisionLevel(int level){};
	bool IsFullyLoaded() {return false;}
	bool IsIntersectedWithOtherObjects() { return false; };
	virtual void LevelChanged(int /*level*/) {};
	virtual int GetCurrentLevel() { return 0; }

// реализация  mlIButton
MLIBUTTON_IMPL

public:
	virtual void PosChanged(ml3DPosition &aPos);
};

}

#endif
