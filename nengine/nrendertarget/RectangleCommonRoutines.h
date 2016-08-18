
#pragma once

extern float glRectX1, glRectX2, glRectX0, glRectX3;
extern float glRectY1, glRectY2, glRectY0, glRectY3;
extern GLubyte glRectOpacity;
extern int glRectMode;
extern float glRectStroke;

void DrawSquare();
void DrawSquareStroke();
void DrawSquareShape();
void DrawSquareShapeAndClearZBuffer();