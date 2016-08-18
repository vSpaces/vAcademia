
#include "StdAfx.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "HelperFunctions.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"

bool g_isPlaying = false;

float sinTable[SIN_COS_COUNT + 1];
float cosTable[SIN_COS_COUNT + 1];

unsigned char glColorR, glColorG, glColorB;
unsigned char glBackColorR, glBackColorG, glBackColorB;

GLubyte glPencilOpacity;

bool glPenAsLine;

float glPencilDelta;
float glPencilRadius;
unsigned int glEraserDisplayListID;
unsigned int glPenDisplayListID;
GLfloat glLastMouseX, glLastMouseY;
GLfloat glCurrentMouseX, glCurrentMouseY;

void CalculateLookupsIfNeeded()
{
	static bool isFirstRun = true;

	if (isFirstRun)
	{
		float angle = 0;
		float delta = 2 * MY_PI / (float)SIN_COS_COUNT;
		for (int i = 0; i < SIN_COS_COUNT + 1; i++, angle += delta)
		{
			sinTable[i] = sinf(angle);
			cosTable[i] = cosf(angle);
		}

		isFirstRun = false;

		// because VBO + display lists + Intel OpenGL Driver == crashes
		// we don't use display lists on Intel videocards
		if (g->gi.GetVendorID() == VENDOR_INTEL)
		{
			glPenDisplayListID = 0xFFFFFFFF;
			glEraserDisplayListID = 0xFFFFFFFF;
			return;
		}

		glEraserDisplayListID = GLFUNCR(glGenLists(1));
		GLFUNC(glNewList)(glEraserDisplayListID, GL_COMPILE);
		GLFUNC(glBegin)(GL_TRIANGLES);
		for (int i = 0; i < SIN_COS_COUNT_ERASER; i++)
		{
			float x1 = cosTable[i * 2];
			float y1 = sinTable[i * 2];
			float x2 = cosTable[i * 2 + 2];
			float y2 = sinTable[i * 2 + 2];

			GLFUNC(glVertex3f)(0.0f, 0.0f, 0.0f);
			GLFUNC(glVertex3f)(x1, y1, 0.0f); 
			GLFUNC(glVertex3f)(x2, y2, 0.0f);
		}
		GLFUNC(glEnd)();
		GLFUNC(glEndList)();

		glPenDisplayListID = GLFUNCR(glGenLists(1));
		GLFUNC(glNewList)(glPenDisplayListID, GL_COMPILE);
		GLFUNC(glBegin)(GL_TRIANGLES);
		for (int i = 0; i < SIN_COS_COUNT; i++)
		{
			float x1 = cosTable[i];
			float y1 = sinTable[i];
			float x2 = cosTable[i + 1];
			float y2 = sinTable[i + 1];

			GLFUNC(glVertex3f)(0.0f, 0.0f, 0.0f);
			GLFUNC(glVertex3f)(x1, y1, 0.0f); 
			GLFUNC(glVertex3f)(x2, y2, 0.0f);
		}
		GLFUNC(glEnd)();
		GLFUNC(glEndList)();
	}
}

void ClearZBuffer()
{
	if (g->gi.GetVendorID() != VENDOR_INTEL)
	{
		GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		GLFUNC(glColorMask)(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
}