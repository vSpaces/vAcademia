#ifndef __MY_TYPES_H__
#define __MY_TYPES_H__

#include "vector.h"
#include "quaternion.h"

typedef CalVector Point3d;
typedef CalVector Vector3d;
typedef CalVector Color3d;

typedef std::vector<Point3d>	PointVec;
typedef PointVec::iterator		PointVecIt;
typedef std::vector<double>		DoubleVec;
typedef std::vector<D3DXMATRIX> MatrixVec;

//Channel Mappings
typedef enum {
    INVALID_PLCMNT,
    XROT, YROT, ZROT,
    XPOS, YPOS, ZPOS
} ParamPlacement;

#endif //__MY_TYPES_H__
