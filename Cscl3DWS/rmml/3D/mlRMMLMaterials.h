#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLMaterials_H_
#define _mlRMMLMaterials_H_

#include "mlRMMLMap.h"
#include "mlRMMLMaterial.h"

namespace rmml {

/**
 * Класс набора материалов для объектов 3D-сцен RMML
 */

class mlRMMLMaterials :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable
{
	v_elems mvChangeListeners;
public:
	mlRMMLMaterials(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLMaterials(void);
MLJSCLASS_DECL

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

public:
	void AddChangeListener(mlRMML3DObject* ap3DO);
	void RemoveChangeListener(mlRMML3DObject* ap3DO);
	void NotifyChangeListeners(mlRMMLMaterial* apMat, mlRMMLMap* apMap = NULL);
	mlIMaterial* GetMaterial(unsigned char auID);
	bool GetMaterialTextureSrc(mlRMMLMaterial* apMat, mlRMMLMap* apMap, mlString &asSrc);
private:
	void SetMaterialDescription(int aiMatIdx, const wchar_t* apwcDescr);
};

}

#endif








