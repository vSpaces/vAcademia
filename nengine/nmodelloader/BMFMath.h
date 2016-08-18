
#pragma once

#include "Vector3D.h"

void AlignFloatToWorld(float& f);
void AlignVectorToWorld(CVector3D& v);
void AlignVectorcToWorld(float& x, float& y, float& z);
bool IsVectorsCoplanar(const CVector3D& v1, const CVector3D& v2);
void ABGR2ARGBINVERTA(unsigned char* color);
void ClampValue(int& value, int min, int max);