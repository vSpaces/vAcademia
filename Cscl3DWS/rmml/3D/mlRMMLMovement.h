#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLMovement_H_
#define _mlRMMLMovement_H_

namespace rmml {

/**
 * Класс для задания ходьбы персонажей RMML
 */

class mlRMMLMovement :	
					public mlRMMLElement,
					public mlJSClass,
					public mlIMovement
{
public:
	mlRMMLMovement(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLMovement(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_stepLength = 1,
		JSPROP_rotSpeed
	};
	double step_length;
	double rot_speed;

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация mlIMovement
public:
	mlMedia* GetMotion(const wchar_t* apwcName){
		return (mlMedia*)GetChild(apwcName);
	}
	double	GetStepLength()
	{
		return step_length;
	}
	double	GetRotSpeed()
	{
		return rot_speed;
	}
};

}

#endif
