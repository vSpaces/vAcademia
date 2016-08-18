#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLMaterial_H_
#define _mlRMMLMaterial_H_

#include "mlColorJSO.h"

namespace rmml {

/**
 * Класс материала для объектов 3D-сцен RMML
 */
class mlMaterialMaps;

class mlRMMLMaterial :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlIColorCL,
					public mlIMaterial
{
	mlColor diffuse;
	JSObject* mjsoDiffuse;
	mlColor ambient;
	JSObject* mjsoAmbient;
	mlColor specular;
	JSObject* mjsoSpecular;
	mlColor emissive;
	JSObject* mjsoEmissive;
	double	power;
	unsigned char opacity;
	mlMaterialMaps* maps;
public:
	mlRMMLMaterial(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLMaterial(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_diffuse = 1,
		JSPROP_ambient,
		JSPROP_specular,
		JSPROP_emissive,
		JSPROP_power,
		JSPROP_opacity,
		JSPROP_maps
	};

public:
	void GetProps(moIMaterial* apMat);
	mlRMMLMap* GetMapElem(const wchar_t* apwcIndex, bool abCreate = true);
	void NotifyChangeListeners();
	void CreateColorJSO(JSObject* &aJso, mlColor* apColor);
	mlString GetMapName(mlRMMLMap* apMap);

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация mlIColorCL
public:
	void ColorChanged(mlColor* apColor){
		NotifyChangeListeners();
	}

// реализация mlIMaterial
public:
	mlIMaterial* GetIMaterial(){ return this; }
	
	mlColor GetDiffuse(){ return diffuse; }
	mlColor GetAmbient(){ return ambient; }
	mlColor GetSpecular(){ return specular; }
	double GetPower(){ return power; }
	mlColor GetEmissive(){ return emissive; }
	unsigned char GetOpacity(){ return opacity; }
	//
	mlMedia* GetMap(const wchar_t* apwcIndex);
};

class mlMaterialMaps:
					public mlJSClass
{
friend class mlRMMLMaterial;
	mlRMMLMaterial* mpMaterial;
public:
	mlMaterialMaps(void);
	void destroy() { MP_DELETE_THIS; }
	~mlMaterialMaps(void);
MLJSCLASS_DECL2(mlMaterialMaps)
private:
};

}

#endif
