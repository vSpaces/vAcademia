#pragma once

namespace rmml {

// change listener interface
struct mlIRectCL{
	virtual void RectChanged(mlRect* apRect) = 0;
};

/**
 * JS-класс для структуры прямоугольника RMML
 */

class mlRectangleJSO:
					public mlJSClass
{
	mlRect mRect;
	mlRect* mpRect;
	mlIRectCL* mpRCL;
public:
	mlRectangleJSO(void){
		ML_INIT_RECT(mRect)
		mpRect = &mRect;
		mpRCL = NULL;
	}
	void destroy() { MP_DELETE_THIS; }
	virtual ~mlRectangleJSO(void){}
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlRectangleJSO)
private:
	enum {
		JSPROP_left,
		JSPROP_right,
		JSPROP_top,
		JSPROP_bottom
	};
	void NotifyChangeListener(){
		if(mpRCL != NULL)
			mpRCL->RectChanged(mpRect);
	}
public:
	void SetRectRef(mlRect* apRect, mlIRectCL* apRCL = NULL){
		mpRect = apRect;
		mpRCL = apRCL;
	}
	mlRect GetRect(){ return *mpRect; }
	void SetRect(const mlRect aRect){ *mpRect = aRect; }
};

}