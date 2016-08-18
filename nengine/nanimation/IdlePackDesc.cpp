
#include "StdAfx.h"
#include "IdlesPackDesc.h"

_SIdlesPackDesc::_SIdlesPackDesc(int idleCount):
	MP_VECTOR_INIT(motionIDs),
	MP_VECTOR_INIT(between),
	MP_VECTOR_INIT(freq)
{
	motionIDs.reserve(idleCount);
	between.reserve(idleCount);
	freq.reserve(idleCount);
}