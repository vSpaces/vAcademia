#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLHint_H_
#define _mlRMMLHint_H_

namespace rmml {

/**
 * Класс всплывающих подсказок 3D-объектов RMML 
 * (элемент указывается для viewport-а)
 */

class mlRMMLHint :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlIHint
{
	bool mbStylesRef; // pStyles is reference to element
	JSObject* mjsoStyles;
public:
	mlRMMLHint(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLHint(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_style = 1,
		JSPROP_delayBefore,
		JSPROP_showDelay
	};
	JSString* style;
	long delayBefore;
	long showDelay;

	mlRMMLElement* FindStyles(JSString* ajssStylesName);

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация mlIHint
public:
	mlIHint* GetIHint(){ return this; } 
	mlStyle* GetStyle(const char* apszTag=NULL);
};

}

#endif
