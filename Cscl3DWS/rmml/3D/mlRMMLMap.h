#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLMap_H_
#define _mlRMMLMap_H_

namespace rmml {

/**
 * Класс набора материалов для объектов 3D-сцен RMML
 */

class mlRMMLMap :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					mlIMap
{
//	union{
		mlRMMLElement* source; // imageref/animref/videoref
		JSString* mjssSource;
//	};
	enum{
		REFF_SOURCE		= 1<<0 // mlRMMLShadows3D-object found (source is valid, mjssSource is invalid)
	};
	unsigned char btRefFlags;
public:
	mlRMMLMap(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLMap(void);
MLJSCLASS_DECL
	enum {
		JSPROP_source
	};
protected:
	bool GetTextureSrc(mlString &asSrc);

public:
	bool SetSourceRefJSProp(JSContext *cx, jsval *vp);
	bool CreateDefaultMapImage();
	void NotifyChangeListeners();

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация mlIMap
public:
	mlIMap* GetIMap(){ return this; } 
	mlMedia* GetSource(){ return source; }
};

}

#endif
