#pragma once

namespace rmml {

struct mlI3DPosition{
	virtual void SetPos_x(double adNewVal)=0;
	virtual void SetPos_y(double adNewVal)=0;
	virtual void SetPos_z(double adNewVal)=0;
	virtual ml3DPosition GetPos()=0;
	virtual void SetPos(ml3DPosition &pos)=0;
};

/**
 * JS-класс для 3D-положения RMML
 */

class mlPosition3D:	
					public mlJSClass
{
public:
	mlPosition3D(void);
	void destroy() { MP_DELETE_THIS; }
	~mlPosition3D(void);
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlPosition3D)
private:
	enum {
		JSPROP_x,
		JSPROP_y,
		JSPROP_z,
	};
	bool bRef;
	union{
		ml3DPosition* pPos;
		mlI3DPosition* mpI3DPos;
	};
	void CreatePos(){
		if(!bRef && pPos==NULL){ 
			pPos = MP_NEW( ml3DPosition);
			ML_INIT_3DPOSITION(*pPos);
		}
	}
	JSFUNC_DECL(clone)
	JSFUNC_DECL(add)
	JSFUNC_DECL(sub)
	JSFUNC_DECL(mult)	// на число или Rotation3D
	JSFUNC_DECL(div)
	JSFUNC_DECL(dot)	// скалярное произведение
	JSFUNC_DECL(cross)	// векторное произведение
	JSFUNC_DECL(blend)	// интерполяция
	JSFUNC_DECL(normalize)	// нормализовать
	JSFUNC_DECL(length)	// длина
	JSFUNC_DECL(angle)	// возвращает поворот в градусах между этим вектором и данным в качестве аргумента
	JSFUNC_DECL(angleSigned)  // возвращает полный поворот в градусах относительно вектора up (по умолчанию (0, 0, 1)) между этим вектором и данным в качестве аргумента 
public:
	void Set3DPosRef(mlI3DPosition* apI3DPos){
		if(!bRef && pPos!=NULL) MP_DELETE( pPos);
		bRef=true;
		mpI3DPos=apI3DPos;
	}
	ml3DPosition GetPos();
	void SetPos(ml3DPosition &pos);
	double GetLength();
	static void Cross(ml3DPosition const& v1, ml3DPosition const& v2, ml3DPosition & ret);
	static double Dot(ml3DPosition const& v1, ml3DPosition const& v2);
};

}
