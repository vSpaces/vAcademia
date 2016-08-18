#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLIdle_H_
#define _mlRMMLIdle_H_

namespace rmml {

#define MLELPN_MOTION "motion"

/**
 * Класс для задания ходьбы персонажей RMML
 */

class mlRMMLIdle :	
					public mlRMMLElement,
					public mlJSClass,
					public mlIIdle
{
public:
	mlRMMLIdle(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLIdle(void);
	bool mbChildrenMLElsCreated;
	JSString* mjssMotion;
MLJSCLASS_DECL
private:
	enum {
		JSPROP_motion = 1,
		JSPROP_between,
		JSPROP_freq
	};
	mlRMMLMotion* motion;
	int between;
	double freq;

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация mlIIdle
public:
	mlIIdle* GetIIdle(){ return this; }
	mlMedia* GetMotion(){ return motion; }
	int GetBetween(){ return between; }
	double GetFreq(){ return freq; }
};

}

#endif
