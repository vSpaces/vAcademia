
#pragma once

#define GLEW_STATIC
#include "std_define.h"

#include <windows.h>
#include "glew.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include <string>
#include <stdlib.h>

#pragma  warning (disable: 4786)
#pragma  warning (disable: 4702)
#pragma  warning (disable: 4505)

#define SHADOWMAPS 1

#include "..\..\rtl\rtl.h"

#define NENGINE_BUILD
#include "..\..\common\memprofiling.h"
#undef NENGINE_BUILD

#include "GlobalSingletonStorage.h"
