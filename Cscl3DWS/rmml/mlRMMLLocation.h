#pragma once

namespace rmml {

//struct mlLocation3D
//{
//};

//  ласс, с помощью объектов которого можно задавать положение в 3D-мире

class mlLocation3D:	
					public mlJSClass
{
public:
	mlLocation3D(void);
	void destroy() { MP_DELETE_THIS; }
	~mlLocation3D(void);
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlLocation3D)
private:
	enum {
		JSPROP_url, // установка (или получение) всех свойств целиком одним URL-ом
		JSPROP_location, // строковый идентификатор локации 
		JSPROP_place, // числовой (или строковый) идентификатор места в локации
		JSPROP_zone, // числовой идентификатор зоны
		JSPROP_reality, // числовой идентификатор реальности
		JSPROP_point, // check point (число или строка латинскими буквами) - конкретна€ позици€ в заданном месте
		// абсолютные или относительные координаты
		JSPROP_x,
		JSPROP_y,
		JSPROP_z,
	};

	#define MLL3D_DM_LOCATION	0x001
	#define MLL3D_DM_PLACE_I	0x002
	#define MLL3D_DM_PLACE_S	0x004
	#define MLL3D_DM_X			0x008
	#define MLL3D_DM_Y			0x010
	#define MLL3D_DM_Z			0x020
	#define MLL3D_DM_ZONE		0x040
	#define MLL3D_DM_REALITY	0x080
	#define MLL3D_DM_POINT_I	0x100
	#define MLL3D_DM_POINT_S	0x200
	int miDefined; // флаги, определ€ющие то, какие параметры положени€ заданы

	// распарсить URL
	bool parseURL(const wchar_t* apwcURL);
	// создать URL
	const wchar_t* makeURL();

public:
//	mlLocation3D GetLocation(); // выдает 
	const wchar_t* GetLocationURL(); // выдает положение единым URL-ом

}; // class mlLocation3D

}
