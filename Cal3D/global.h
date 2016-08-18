//****************************************************************************//
// global.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_GLOBAL_H
#define CAL_GLOBAL_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "std_define.h"

// autoconf/automake includes
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// platform dependent includes
#include "platform.h"

// standard includes
#include <cstdlib>
#include <cmath>

// debug includes
#include <cassert>

// STL includes
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include "windows.h"

#define D3D_OVERLOADS

// Direct3d types
#include <D3d8.h>
#include <D3dx8math.h>

typedef float D3DVALUE;

typedef struct _D3DVERTEX { 
	D3DVALUE x; 
	D3DVALUE y; 
	D3DVALUE z; 
	D3DVALUE nx; 
	D3DVALUE ny; 
	D3DVALUE nz; 
	D3DVALUE tu; 
	D3DVALUE tv; 
} D3DVERTEX, *LPD3DVERTEX; 

//#include <d3dtypes.h>
/*#include <d3d.h>
#include <d3dxmath.h>*/

enum CALROTTYPE  {CRT_ROOT, CRT_POINT};
enum CALMOVETYPE {CMT_ABSOLUTE, CMT_RELATIVE};
enum CALOPTYPE   {CAL_REPLACE, CAL_AFTER};
enum CALPLANES   {XOY, XOZ, YOZ};

#define	ANIMTYPE_NONE		0
#define ANIMTYPE_CYCLE		1
#define ANIMTYPE_POSE		2
#define ANIMTYPE_ACTION		3
#define ANIMTYPE_SET		4

// global Cal3D constants
namespace Cal
{
  // global typedefs
  typedef void *UserData;

  // file magic cookies
  const char SKELETON_FILE_MAGIC[4]  = { 'C', 'S', 'F', '\0' };
  const char FACE_FILE_MAGIC[4]  = { 'F', 'C', 'M', '\0' };
  const char ANIMATION_FILE_MAGIC[4] = { 'C', 'A', 'F', '\0' };
  const char RELANIMATION_FILE_MAGIC[4] = { 'C', 'A', 'R', '\0' };
  const char MESH_FILE_MAGIC[4]      = { 'C', 'M', 'F', '\0' };
  const char MESH_ALONE_FILE_MAGIC[4]      = { 'C', 'M', 'S', '\0' };
  const char MATERIAL_FILE_MAGIC[4]  = { 'C', 'R', 'F', '\0' };
  const char FAPU_FILE_MAGIC[4]  = { 'F', 'P', 'U', '\0' };
  const char FAP_FILE_MAGIC[4]  = { 'F', 'A', 'P', '\0' };

  // library version
  const unsigned int LIBRARY_VERSION = 1011;

  // Version 1000
  //	1. Add emissive component to materials

  // Version 1001
  //	1. make animations suitable for all characters. Add bone_name to animation file, that contains
  //	common animation.

  // Version 1002
  //	1. Add material map type to files

  // Version 1003
  //	1. Add projection track on the Bip bone to core animation

  // Version 1004
  //	1. Add shadow volume data to CalCoreModel

  // Version 1005
  //	1. Translation and Rotation added to CalCoreMesh

  // Version 1006
  //	1. Save flags to animation

  // Version 1007
  //	1. Save flags data to animation

  // Version 1008
  //	1. Nothing changes in file formats, besides PROJECT_BONE z position may be different than ZERO

  // Version 1009
  //	1. Save common skeleton bones length to the common animation files

  // Version 1010
  //	1. FAP file format changed

  // Version 1011
  //	1. FAP file format changed:
  //		added mesh parameters to control_points
	

  // file versions
  const unsigned int CURRENT_FILE_VERSION = LIBRARY_VERSION;
  const unsigned int EARLIEST_COMPATIBLE_FILE_VERSION = 699;

  const unsigned int DIFFUSE_TEXTURE = 1;
  const unsigned int SPECULAR_TEXTURE = 2;
  const unsigned int BUMP_TEXTURE = 8;

  const int PROJECTION_BONE_ID = -1;
  const int NULL_PARENT_BONE_ID = -1;
  const int NO_SUCH_BONE_ID = -2;

  #define FACESIZEISWORD

  void caldoutFF(LPSTR str, float w1=FLT_MAX, float w2=FLT_MAX);
};

#endif

//****************************************************************************//
