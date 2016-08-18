#pragma once

namespace rmml {

// change listener interface
struct mlIColorCL{
	virtual void ColorChanged(mlColor* apColor) = 0;
};

/**
 * JS-класс для структуры цвета RMML
 */

class mlColorJSO:	
					public mlJSClass
{
	mlColor mColor;
	mlColor* mpColor;
	mlIColorCL* mpCCL;
public:
	mlColorJSO(void){
		ML_INIT_COLOR(mColor)
		mpColor = &mColor;
		mpCCL = NULL;
	}
	void destroy() { MP_DELETE_THIS; }
	~mlColorJSO(void){}
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlColorJSO)
private:
	enum {
		JSPROP_r,
		JSPROP_g,
		JSPROP_b,
		JSPROP_a
	};
	void NotifyChangeListener(){
		if(mpCCL != NULL)
			mpCCL->ColorChanged(mpColor);
	}
public:
	void SetColorRef(mlColor* apColor, mlIColorCL* apCCL = NULL){
		mpColor = apColor;
		mpCCL = apCCL;
	}
	mlColor GetColor();
	void SetColor(mlColor &aColor);
};

}