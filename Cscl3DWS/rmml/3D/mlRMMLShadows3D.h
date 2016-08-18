#pragma once

namespace rmml {

/**
 * JS-класс для теней 3D-объектов RMML
 */

class mlRMMLShadows3D:	
					public mlRMMLElement,
					public mlJSClass,
					public ml3DShadow,
					public mlIShadows
{
	v_elems mvChangeListeners;
	mlRMMLElement* GetElem_ml3DShadow(){ return this; }
public:
	mlRMMLShadows3D(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLShadows3D(void);
MLJSCLASS_DECL

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация mlIShadows
	mlIShadows* GetIShadows(){ return this; } 
	mlMedia* GetShadow(int aiNum);
	mlE3DShadowType GetType(){ return type; }
	ml3DPlane GetPlane(){ return plane; }
	mlColor GetColor(){ return color; }
	mlMedia* GetLight(){ return light; }

public:
	void AddChangeListener(mlRMML3DObject* ap3DO);
	void RemoveChangeListener(mlRMML3DObject* ap3DO);
	void NotifyChangeListeners();
};

}
