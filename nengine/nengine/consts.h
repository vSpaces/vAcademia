
#pragma once

// параметры с помощью которых bounding box объекта превращается в bounding box его теневого объема,
// сильно растянутого по оси Z
#define SHADOW_BOUNDING_BOX_OFFSET		0.5f
#define SHADOW_BOUNDING_BOX_SCALE_XY	1.2f
#define SHADOW_BOUNDING_BOX_SCALE_Z		3.0f

// размеры персонажа для физики
#define ANIM_DIMENSIONS			35.0f, 40.0f, 180.0f

// минимальное изменение высоты персонажа, начиная с которого соответствующий физический объект
// надо бы приподнять, чтобы скомпенсировать то, что пивот у физического объекта и у персонажа
// не совпадают
#define MIN_Z_CHANGE			2.0f

// минимальный размер по любому из измерений у bounding box объекта
#define MIN_SIZE				15.0f

// параметры для коррекции bounding box-ов маленьких объектов
#define MIN_BIG_OBJECT_SIZE		100.0f
#define COLLISION_CORRELATION	0.9f