#pragma once

//////////////////////////////////////////////////////////////////////////
// Типы объктов города
// неопределенный тип
namespace vespace
{
	enum	VESPACE_OBJECTS_TYPES
	{
		VE_UNDEFINED		= 0,		// неопределенный тип
		VE_OBJECT3D			= 1,		// обычный 3D объект
		VE_GROUP3D			= 2,		// группа 3D объект
		VE_TREE				= 4,		// дерево
		VE_SKYBOX			= 5,		// sky box
		VE_SUNLENSFLARE		= 6,		// sun lensflare
		VE_LOCATIONBOUNDS	= 7,		// границы локации
		VE_AVATAR			= 8,		// аватар
		VE_COLLIDER_AABB	= 9,		// collision object AABB
		VE_COLLIDER_OOBB	= 10,		// collision object OOB
		VE_COLLIDER_MESH	= 11,		// collision object MESH
		VE_REFERENCE		= 64,		// 3D ссылочный объект
		VE_PRIMOBJECT3D		= 128,		// объект из примитивов

		VE_COLLIDER_OBJECT	= 12,		// коллизионный примитив
		VE_SITTING_PLACE	= 13,		// место для сиденья
		VE_CAMERA			= 14,		// камера
		VE_CAMERA_TARGET	= 15,		// цель камеры

	};
}