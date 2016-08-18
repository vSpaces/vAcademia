#pragma once

#include <math.h>

namespace rmml {

struct mlI3DRotation{
	virtual void SetRot_x(double adNewVal)=0;
	virtual void SetRot_y(double adNewVal)=0;
	virtual void SetRot_z(double adNewVal)=0;
	virtual void SetRot_a(double adNewVal)=0;
	virtual ml3DRotation GetRot()=0;
	virtual void SetRot(ml3DRotation &aRot)=0;
};

#define PI 3.14159265358979323846

// представление поворота вектором и углом поворота
struct ml3DRotationVA{
	double x,y,z,a;
	ml3DRotationVA(){
		x = y = z = a = 0.0;
	}
	ml3DRotationVA(const ml3DRotation aRot){
		ml3DRotation rot = aRot;
		Normalize(rot);
		
		#define RTN_EPS 0.00001
		if( rot.x<RTN_EPS && rot.y<RTN_EPS && rot.z<RTN_EPS &&
			rot.x>-RTN_EPS && rot.y>-RTN_EPS && rot.z>-RTN_EPS)
		{
			x = 0;
			y = 0;
			z = 1;
			a = 0;
			return;
		}
		
		a = acos(rot.a) * 2; /* * RADIANS*/
		double sin_angle = sqrt(1.0 - rot.a * rot.a);

		if ( fabs( sin_angle ) < 0.0005 )
		  sin_angle = 1;

		x = rot.x/sin_angle;
		y = rot.y/sin_angle;
		z = rot.z/sin_angle;
	}
	ml3DRotation GetQuat(){
		double sin_a = sin( a / 2 );
		double cos_a = cos( a / 2 );

		ml3DRotation rot;
		double norm = sqrt(x*x+y*y+z*z);
		if (norm != 0.0)
		{
			rot.x = x/norm * sin_a;
			rot.y = y/norm * sin_a;
			rot.z = z/norm * sin_a;
			rot.a = cos_a;
		}
		else
		{
			rot.x = 0.0 * sin_a;
			rot.y = 0.0 * sin_a;
			rot.z = 1.0 * sin_a;
			rot.a = cos_a;
		}
		Normalize(rot);
		return rot;
	}
private:
	void Normalize(){
		double norm = sqrt(x*x+y*y+z*z+a*a);
		if (norm == 0)
			return;
		x /= norm;
		y /= norm;
		z /= norm;
		a /= norm;
	}
	void Normalize(ml3DRotation &rot){
		double norm = sqrt(rot.x * rot.x + rot.y * rot.y + rot.z * rot.z + rot.a * rot.a);
		if (norm == 0)
			return;
		rot.x /= norm;
		rot.y /= norm;
		rot.z /= norm;
		rot.a /= norm;
	}
};

/**
 * JS-класс для 3D-масштаба RMML
 */

class mlRotation3D:
					public mlJSClass
{
public:
	mlRotation3D(void);
	void destroy() { MP_DELETE_THIS; }
	~mlRotation3D(void);
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlRotation3D)
private:
	enum {
		JSPROP_x,
		JSPROP_y,
		JSPROP_z,
		JSPROP_a,
	};
	bool bRef;
	union{
		ml3DRotation* pRot;		// quaternion
		mlI3DRotation* mpI3DRot;
	};
	ml3DRotationVA* mpRotVA; // ось поворота (если утеряна в quaternion-е)
	void CreateRot(){
		if(!bRef && pRot==NULL){ 
			pRot = MP_NEW( ml3DRotation);
			ML_INIT_3DROTATION(*pRot);
		}
	}
	JSFUNC_DECL(clone)
	JSFUNC_DECL(mult)		// на Rotation3D - дополнительный поворот
	JSFUNC_DECL(conjugate)	// обратный поворот
	JSFUNC_DECL(blend)		// интерполяция
	JSFUNC_DECL(normalize)	// нормализовать
public:
	void Set3DRotRef(mlI3DRotation* apI3DRot){
		if(!bRef && pRot!=NULL) MP_DELETE( pRot);
		bRef=true;
		mpI3DRot=apI3DRot;
	}
	ml3DRotation GetRot();
	void SetRot(ml3DRotation &aRot);
	void Normalize();
};

}
