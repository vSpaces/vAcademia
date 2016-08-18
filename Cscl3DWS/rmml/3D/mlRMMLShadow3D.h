#pragma once

namespace rmml {

#define MLSHTN_NONE L"none"
#define MLSHTN_PROJECTIVE L"projective"
#define MLSHTN_STENCIL L"stencil"

class ml3DShadow{
protected:
	mlE3DShadowType type;
	ml3DPlane plane;
	mlColor color;
	union{
		JSString* mjssLight;
		mlRMMLLight* light;
	};
	enum {
		JSPROP_type,
		JSPROP_plane,
		JSPROP_color,
		JSPROP_light
	};
	enum{
		SHPS_type=1<<0,
		SHPS_plane=1<<1,
		SHPS_color=1<<2,
		SHPS_light=1<<3,
	};
	unsigned char mbtPropSet;
public:
	ml3DShadow(){
		type=MLSHT_NONE;
		ML_INIT_3DPLANE(plane);
		ML_INIT_COLOR(color);
		mjssLight=NULL;
		light=NULL;
	}
protected:
	void SetJSProperty(JSContext *cx, int aiID, jsval *vp);
	void GetJSProperty(JSContext *cx, int aiID, jsval *vp);
	virtual void NotifyChangeListeners(){}
	virtual mlRMMLElement* GetElem_ml3DShadow(){ return NULL; }
	virtual bool IsChildrenMLElsCreated(){ return false; }
private:
	bool ParsePlane(JSContext* cx, wchar_t* &apwc, ml3DPlane &aPlane);
	bool SetLightRefJSProp(JSContext *cx, jsval *vp);
};

/**
 * JS-класс для тени 3D-объектов RMML
 */

class mlRMMLShadow3D:	
					public mlRMMLElement,
					public mlJSClass,
					public ml3DShadow,
					public mlIShadow
{
	void NotifyChangeListeners();
	mlRMMLElement* GetElem_ml3DShadow(){ return this; }
	bool IsChildrenMLElsCreated(){ return (mpMO!=NULL); }
public:
	mlRMMLShadow3D(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLShadow3D(void);
MLJSCLASS_DECL

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация mlIShadows
	mlIShadow* GetIShadow(){ return this; } 
	mlE3DShadowType GetType();
	ml3DPlane GetPlane();
	mlColor GetColor();
	mlMedia* GetLight();

private:
	bool ParseType(JSContext* cx, wchar_t* &apwc);
public:
	void ParseValue(JSContext* cx, wchar_t* apwcValue);
};

}
