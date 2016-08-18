#pragma once

//#include "2D\mlRectangleJSO.h"

namespace rmml {

/**
 * Класс рамки, нарезающейся из изображения 
 */

class mlRMMLImageFrame :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMMLVisible,
					public mlRMMLPButton,
					public mlIImageFrame,
					public moIImageFrame
{
protected:
	bool tile;
public:
	mlRMMLImageFrame(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLImageFrame(void);
MLJSCLASS_DECL
private:
	enum{
		JSPROP_margin = 1, // сколько пикселов от края - рамка
		JSPROP_marginTop,
		JSPROP_tile,
		JSPROP_marginBottom,
		JSPROP_marginLeft,
		JSPROP_marginRight,
	};
	int margin;
	int marginTop;
	int marginBottom;
	int marginLeft;
	int marginRight;

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlIImageFrame* GetIImageFrame(){ return this; }

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIVisible
MLIVISIBLE_IMPL

// реализация  mlIButton
MLIBUTTON_IMPL

// реализация mlIImageFrame
public:
	virtual int GetMargins(int& aiMarginTop, int& aiMarginBottom, int& aiMarginLeft, int& aiMarginRight);
	bool GetTile(){ return tile; }

// реализация moIImageFrame
public:
	virtual void MarginsChanged();
	void TileChanged();

};

} // namespace rmml
