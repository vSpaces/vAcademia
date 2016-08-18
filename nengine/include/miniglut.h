
#pragma once

#include "OGLdebug.h"

#define INIT_QUADRIC if (!quadObj) { quadObj = gluNewQuadric(); gluQuadricDrawStyle(quadObj, GLU_FILL); gluQuadricNormals(quadObj, GLU_SMOOTH); }

namespace miniglut
{
	static GLUquadricObj* quadObj = NULL;
	
	static void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks)
	{
		INIT_QUADRIC
		GLFUNC(gluSphere)(quadObj, radius, slices, stacks);
	}

	static void glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
	{
		INIT_QUADRIC
		GLFUNC(gluCylinder)(quadObj, base, 0.0, height, slices, stacks);
	}

	static void drawBox(GLfloat size, GLenum type)
	{
		static GLfloat n[6][3] =
		{
			{-1.0, 0.0, 0.0},
			{0.0, 1.0, 0.0},
			{1.0, 0.0, 0.0},
			{0.0, -1.0, 0.0},
			{0.0, 0.0, 1.0},
			{0.0, 0.0, -1.0}
		};

		static GLint faces[6][4] =
		{
			{0, 1, 2, 3},
			{3, 2, 6, 7},
			{7, 6, 5, 4},
			{4, 5, 1, 0},
			{5, 6, 2, 1},
			{7, 4, 0, 3}
		};

		GLfloat v[8][3];
		GLint i;

		v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
		v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
		v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
		v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
		v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
		v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

		for (i = 5; i >= 0; i--) 
		{
			GLFUNC(glBegin)(type);
			GLFUNC(glNormal3fv)(&n[i][0]);
			GLFUNC(glTexCoord2f)(0, 0);
			GLFUNC(glVertex3fv)(&v[faces[i][0]][0]);
			GLFUNC(glTexCoord2f)(0, 1);
			GLFUNC(glVertex3fv)(&v[faces[i][1]][0]);
			GLFUNC(glTexCoord2f)(1, 1);
			GLFUNC(glVertex3fv)(&v[faces[i][2]][0]);
			GLFUNC(glTexCoord2f)(1, 0);
			GLFUNC(glVertex3fv)(&v[faces[i][3]][0]);
			GLFUNC(glEnd)();
		}
	}

	static void glutSolidCube(GLfloat size)
	{
		drawBox(size, GL_QUADS);
	}

	static void glutWireCube(GLfloat size)
	{
		drawBox(size, GL_LINE_LOOP);
	}

	static void glutWireSphere(GLdouble radius, GLint slices, GLint stacks)
	{
		GLFUNC(glPolygonMode)(GL_FRONT_AND_BACK, GL_LINE);
        GLFUNC(glutSolidSphere)(radius, slices, stacks);
		GLFUNC(glPolygonMode)(GL_FRONT_AND_BACK, GL_FILL);
	}
};