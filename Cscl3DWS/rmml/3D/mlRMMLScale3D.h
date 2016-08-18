#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLScale3D_H_
#define _mlRMMLScale3D_H_

namespace rmml {

struct mlI3DScale{
	virtual void SetScl_x(double adNewVal)=0;
	virtual void SetScl_y(double adNewVal)=0;
	virtual void SetScl_z(double adNewVal)=0;
	virtual ml3DScale GetScl()=0;
	virtual void SetScl(ml3DScale &scl)=0;
};

/**
 * JS-класс для 3D-масштаба RMML
 */

class mlScale3D:
					public mlJSClass
{
public:
	mlScale3D(void);
	void destroy() { MP_DELETE_THIS; }
	~mlScale3D(void);
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlScale3D)
private:
	enum {
		JSPROP_x,
		JSPROP_y,
		JSPROP_z,
	};
	bool bRef;
	union{
		ml3DScale* pScl;
		mlI3DScale* mpI3DScl;
	};
	void CreateScl(){
		if(!bRef && pScl==NULL){ 
			pScl = MP_NEW( ml3DScale);
			ML_INIT_3DSCALE(*pScl);
		}
	}
	JSFUNC_DECL(clone)
public:
	void Set3DSclRef(mlI3DScale* apI3DScl){
		if(!bRef && pScl!=NULL) MP_DELETE( pScl);
		bRef=true;
		mpI3DScl=apI3DScl;
	}
	ml3DScale GetScl();
	void SetScl(ml3DScale &scl);
};

}

#endif