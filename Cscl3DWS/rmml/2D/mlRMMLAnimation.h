#pragma once


namespace rmml {

/**
 * Класс анимации RMML
 */

class mlRMMLAnimation :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMMLVisible,
					public mlRMMLContinuous,
					public mlRMMLPButton
{
public:
	mlRMMLAnimation(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLAnimation(void);
MLJSCLASS_DECL

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult Update(const __int64 alTime);
	bool Freeze();
	bool Unfreeze();

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIVisible
MLIVISIBLE_IMPL

// реализация  mlIContinuous
MLICONTINUOUS_IMPL

// реализация  mlIButton
MLIBUTTON_IMPL

};

}