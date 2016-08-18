#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLGroup_H_
#define _mlRMMLGroup_H_

namespace rmml {

/**
 * Класс групп 3D-объектов RMML
 */

class mlRMMLGroup :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMML3DObject,
					public mlRMMLPButton,
					public mlI3DGroup
{
public:
	mlRMMLGroup(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLGroup(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_gravitation = 1,
		JSPROP_intangible,
		JSPROP_scripted,		// если true, то на все объекты группы, загружаемые по src создаются RMML-объекты
		JSPROP_color,		// цвет, который добавляется ко всем материалам объекта в степени, заданной color.a
		JSPROP_opacity,		// степень непрозрачности объекта (0..0xFFFF)
	};
	bool scripted;

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	bool Freeze(){ 
		if(!mlRMMLElement::Freeze()) return false; 
		return FreezeChildren(); 
	}
	bool Unfreeze(){ 
		if(!mlRMMLElement::Unfreeze()) return false; 
		return UnfreezeChildren(); 
	}

// реализация  mlRMMLLoadable
MLILOADABLE_IMPL

// реализация  mlIButton
MLIBUTTON_IMPL

// реализация  mlI3DObject
MLI3DOBJECT_IMPL
	bool GetBillboard(){ return false; }
	bool GetUnpickable(){ return false; }

	void ChildAdded(mlRMMLElement* apNewChild);
	void ChildRemoving(mlRMMLElement* apChild);

	void SetMaterialTiling(int, float) {};
	void EnableShadows(bool isEnabled){};
	void ChangeCollisionLevel(int level){};

// реализация  moI3DObject
	void BillboardChanged(){}
	void UnpickableChanged(){}
	bool IsFullyLoaded() {return false;}
	bool IsIntersectedWithOtherObjects() {return false;}
	void LevelChanged(int /*level*/) {};
	int GetCurrentLevel() { return 0; }

// реализация mlI3DGroup
mlI3DGroup* GetI3DGroup(){ return this; }

	bool GetScripted(){ return scripted; }

// реализация moI3DGroup
protected:
	moI3DGroup* GetmoI3DGroup();
	void ScriptedChanged();

};

}

#endif
