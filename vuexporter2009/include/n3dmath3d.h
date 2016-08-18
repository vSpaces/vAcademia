#if !defined(N3DMATHDEFS)
#define N3DMATHDEFS

#include <math.h>
#include "n3ddefs.h"
//#include "n3doobb.h"

//! lower bound for overflow detection
#define FG_SMALL 1.0e-4f
//! upper bound for overflow detection
#define FG_BIG   1.0e+10f
//! no intersection found
#define	UNDEF_INTERSECTION		0x0
//! intersection found
#define	POINT_INTERSECTION		0x1
//! no intersection found
#define	NO_INTERSECTION			0xFFFFFFFF
//! returns the bitwise representation of a floating point number
#define FG_FPBITS(fp)		(*(int *)&(fp))
//! returns the absolute value of a floating point number in bitwise form
#define FG_FPABSBITS(fp)	(FG_FPBITS(fp)&0x7FFFFFFF)
//! returns the signal bit of a floating point number
#define FG_FPSIGNBIT(fp)	(FG_FPBITS(fp)&0x80000000)
//! returns the absolute value of a floating point number
#define FG_FPABS(fp)		(*((int *)&fp)=FG_FPABSBITS(fp))
//! returns the value of 1.0f in bitwise form
#define FG_FPONEBITS		0x3F800000
//! represents cos(45)
#define FG_COS45			0.7071067811865475244f
//! represents the square root of 3
//#define FG_SQRT3			1.0f/1.7320508075688772935f
#define FG_SQRT3			FG_COS45
//
#define MAX_DWORD			0xFFFFFFFF

//! return A value from ARGB format
#define A_ARGB(x)			((x&0xFF000000)>>24)
//! return R value from ARGB format
#define R_ARGB(x)			((x&0x00FF0000)>>16)
//! return G value from ARGB format
#define G_ARGB(x)			((x&0x0000FF00)>>8)
//! return B value from ARGB format
#define B_ARGB(x)			(x&0x000000FF)

float STR_TO_FLOAT(LPCSTR s);

#define TO_RAD(x)	(x*3.14f/180.0f)

// Mistakes
#define		COPLANAR_PL_DOT		0.01f					// ошибка дл€ скал€рного произведелни€ нормалей
#define		COPLANAR_PL_DOTm1	(1.0f-COPLANAR_PL_DOT)
#define		COPLANAR_PL_DIST	0.1f					// ошибка дл€ рассто€ни€ до плоскости
#define		COLLIDE_DIST_TRESH	0.1f					// ошибка при рассчете коллизий
#define		INTRSECT_DIST_TRESH	0.01f					// ошибка при рассчете коллизий

#endif