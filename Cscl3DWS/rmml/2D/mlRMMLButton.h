#pragma once


namespace rmml {

/**
 * Класс кнопки RMML
 */

class mlRMMLButton :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMMLVisible,
					public mlRMMLPButton
{
public:
	mlRMMLButton(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLButton(void);
MLJSCLASS_DECL

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIVisible
MLIVISIBLE_IMPL

// реализация  mlIButton
MLIBUTTON_IMPL

};

}