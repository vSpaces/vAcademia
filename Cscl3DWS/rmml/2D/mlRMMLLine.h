#pragma once

#include "2D\mlRectangleJSO.h"
#include "mlColorJSO.h"

namespace rmml {

/**
 * Класс 2D-линии RMML
 */

class mlRMMLLine :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLVisible,
					public mlRMMLPButton,
					public mlIColorCL,
					public mlILine,
					public moILine
{
protected:
	mlPoint mXY2;
	mlColor color;
	JSObject* mjsoColor;
	bool smooth;
	int thickness;

public:
	mlRMMLLine(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLLine(void);
MLJSCLASS_DECL
private:
	enum{
		JSPROP_x2 = 1,
		JSPROP_y2,
		JSPROP_color,	// цвет, примешиваемый к изображению
		JSPROP_thickness,
		JSPROP_smooth,
	};

	void CreateColorJSO(JSObject* &jso, mlColor* apColor);

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	void DuplicateMedia(mlRMMLElement* apSource);

// реализация mlIColorCL
public:
	void ColorChanged(mlColor* apColor){
		ColorChanged();
	}

// реализация  mlIVisible
MLIVISIBLE_IMPL
	virtual mlRect GetBounds();

// реализация  mlIButton
MLIBUTTON_IMPL

// реализация mlILine
public:
	mlILine* GetILine(){ return this; }
	virtual mlPoint GetAbsXY2();
	virtual int GetThickness(){ return thickness; }
	mlColor GetColor(){ return color; }
	bool GetSmooth(){ return smooth; }

// реализация moILine
public:
	void XY2Changed();
	void ColorChanged();
	void ThicknessChanged();
	void SmoothChanged();

	// сделать себе копию изображения
	void DuplicateFrom(mlMedia* apSourceImage){
		if(PMO_IS_NULL(mpMO)) return;
		return mpMO->GetIImage()->DuplicateFrom(apSourceImage);
	}
};

}