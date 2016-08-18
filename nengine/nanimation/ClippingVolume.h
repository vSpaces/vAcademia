
#pragma once

#include "CommonHeader.h"
#include <float.h>

#define	CLIPPING_VOLUME_TYPE_BOX	1
#define	CLIPPING_VOLUME_TYPE_SPHERE	2

typedef struct _SClippingVolume
{
	unsigned char type;

	bool isEnabled;
	
	float minX;
	float minY;
	float minZ;
	float maxX;
	float maxY;
	float maxZ;

	float centerX;
	float centerY;
	float centerZ;
	float halfLengthX;
	float halfLengthY;
	float halfLengthZ;

	_SClippingVolume()
	{
		isEnabled = false;
		minX = -10000.0f;
		minY = -10000.0f;
		minZ = -10000.0f;
		maxX = 10000.0f;
		maxY = 10000.0f;
		maxZ = 10000.0f;
		centerX = 0.0f;
		centerY = 0.0f;
		centerZ = 0.0f;
		halfLengthX = 10000.0f;
		halfLengthY = 10000.0f;
		halfLengthZ = 10000.0f;
	}
} SClippingVolume;