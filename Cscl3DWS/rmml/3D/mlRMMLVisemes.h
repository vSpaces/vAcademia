#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLVisemes_H_
#define _mlRMMLVisemes_H_

namespace rmml {

/**
 * Класс визем персонажей RMML
 */

class mlRMMLVisemes :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable
{
public:
	mlRMMLVisemes(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLVisemes(void);
MLJSCLASS_DECL
private:
	enum {
//		JSPROP_inverse,
	};

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

};

}

#endif
