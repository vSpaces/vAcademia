
#pragma once

//#define OGL_DEBUG_ENABLED	1


#ifdef OGL_DEBUG_ENABLED

#define TO_STR(a) #a
#define GLFUNC(x)	g->lw.WriteLn("GL: ", TO_STR(x), " ptr: ", (int)x); x
#define GLFUNCR(x)	x; g->lw.WriteLn("GL: ", TO_STR(x)); 

#else

#define GLFUNC(x)	x
#define GLFUNCR(x)	x

#endif