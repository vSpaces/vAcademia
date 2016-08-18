#pragma once

namespace rmml {

// change listener interface
struct mlIPointCL{
	virtual void PointChanged(mlPoint* apPoint) = 0;
};

/**
 * JS-класс для структуры прямоугольника RMML
 */

class mlPointJSO:
					public mlJSClass
{
	mlPoint mPoint;
	mlPoint* mpPoint;
	mlIPointCL* mpRCL;
public:
	mlPointJSO(void){
		ML_INIT_POINT(mPoint)
		mpPoint = &mPoint;
		mpRCL = NULL;
	}
	void destroy() { MP_DELETE_THIS; }
	virtual ~mlPointJSO(void){}
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlPointJSO)
private:
	enum {
		JSPROP_x,
		JSPROP_y
	};
	void NotifyChangeListener(){
		if(mpRCL != NULL)
			mpRCL->PointChanged(mpPoint);
	}
public:
	void SetPointRef(mlPoint* apPoint, mlIPointCL* apRCL = NULL){
		mpPoint = apPoint;
		mpRCL = apRCL;
	}
	mlPoint GetPoint(){ return *mpPoint; }
	void SetPoint(const mlPoint aPoint){ *mpPoint = aPoint; }
};

}