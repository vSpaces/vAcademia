#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLLight_H_
#define _mlRMMLLight_H_

namespace rmml {

/**
 * Класс источника света для 3D-сцен RMML
 */

class mlRMMLLight :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMML3DObject
{
public:
	mlRMMLLight(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLLight(void);
MLJSCLASS_DECL

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlI3DObject
MLI3DOBJECT_IMPL
	bool GetBillboard(){ return false; }
	bool GetUnpickable(){ return false; }

// реализация  moI3DObject
	void BillboardChanged(){}
	void UnpickableChanged(){}
	bool IsFullyLoaded() {return false;}
	bool IsIntersectedWithOtherObjects() {return false;}

	void SetMaterialTiling(int, float) {};
	void EnableShadows(bool isEnabled){};
	void ChangeCollisionLevel(int level){};
	void LevelChanged(int /*level*/) {};
	int GetCurrentLevel() { return 0; }
};

}

#endif
