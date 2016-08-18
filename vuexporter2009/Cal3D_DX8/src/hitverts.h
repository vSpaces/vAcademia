#ifndef CAL_HITBOXES_H
#define CAL_HITBOXES_H

#include "vector.h"
#include "quaternion.h"

typedef struct _SUBMESHPOINT{
	int			subid;
	int			vertid;
	CalVector	pos;
} SUBMESHPOINT;

typedef struct _HITBOX{
	bool inited;
	CalVector	min;
	CalVector	max;
	CalVector	corners[8];
	_HITBOX::_HITBOX(){ inited=false;}
} HITBOX;

typedef struct _HIBOXVERT{
	SUBMESHPOINT vert;
	CalVector  pos;
	//
	_HIBOXVERT::_HIBOXVERT(){}
	_HIBOXVERT::_HIBOXVERT( int sid, int vid, CalVector ps){
		pos = ps;
		vert.subid = sid;
		vert.vertid = vid;
	}
} HIBOXVERT;

typedef struct _BOUNDVERTS{
	bool inited;
	SUBMESHPOINT min;
	SUBMESHPOINT max;
	CalVector	corners[8];
	_BOUNDVERTS::_BOUNDVERTS(){
		inited=false;
	}
} BOUNDVERTS;

#endif