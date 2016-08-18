
#pragma once

#include "GlobalParams.h"

extern float sinTable[SIN_COS_COUNT + 1];
extern float cosTable[SIN_COS_COUNT + 1];

extern unsigned char glColorR, glColorG, glColorB;
extern unsigned char glBackColorR, glBackColorG, glBackColorB;

extern GLubyte glPencilOpacity;

extern bool glPenAsLine;

extern float glPencilDelta;
extern float glPencilRadius;
extern GLfloat glLastMouseX, glLastMouseY;
extern GLfloat glCurrentMouseX, glCurrentMouseY;