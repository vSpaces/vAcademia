#pragma once

//////////////////////////////////////////////////////////////////////////
// Типы объктов города
// неопределенный тип
namespace vespace
{
	enum	VESPACE_OBJECTS_TYPES
	{
		VE_UNDEFINED					= 0,		// неопределенный тип
		VE_OBJECT3D						= 1,		// обычный 3D объект
		VE_GROUP3D						= 2,		// группа 3D объект
		VE_PARTICLES					= 3,		// частицы
		VE_TREE							= 4,		// дерево
		VE_GROUP_LECTORE_TYPE			= 5,		// группа типа Аудитория
		VE_GROUP_PIVOT_TYPE				= 6,		// группа типа Точка привязки
		VE_LOCATIONBOUNDS				= 7,		// границы локации
		VE_AVATAR						= 8,		// аватар
		VE_COLLIDER_AABB				= 9,		// collision object AABB
		VE_COLLIDER_OOBB				= 10,		// collision object OOB
		VE_COLLIDER_MESH				= 11,		// collision object MESH
		VE_TEMPLOCATIONBOUNDS			= 14,		// границы временной локации
		VE_COMMINICATIONAREABOUNDS		= 15,		// границы временной локации общения
		VE_REFERENCE					= 64,		// 3D ссылочный объект
		VE_PRIMOBJECT3D					= 128		// объект из примитивов
	};
}