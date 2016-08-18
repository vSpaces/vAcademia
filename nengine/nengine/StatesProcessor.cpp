
#include "StdAfx.h"
#include <assert.h>
#include "StatesProcessor.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CStatesProcessor::CStatesProcessor():
	m_blendSrc(GL_ONE_MINUS_SRC_ALPHA),
	m_blendDest(GL_SRC_ALPHA),
	m_isChangedScissors(false),
	m_isBuffersChanged(false),
	m_newVertexBufferID(-1),
	m_newNormalBufferID(-1),
	m_newIndexBufferID(-1),
	m_vertexDimension(0),
	m_vertexBufferID(-1),
	m_normalBufferID(-1),
	m_pushAttribCount(0),
	m_lastMaterialID(-1),
	m_indexBufferID(-1),
	m_newMaterialID(-1),
	m_activeLevel(0),
	m_pushCount(0),
	m_r(256),
	m_g(256),
	m_b(256),
	m_a(256)
{
	int i;
	for (i = 0; i < MAX_STATES; i++)
	{
		m_states[i] = 65535;
	}

	for (i = 0; i < MAX_TEXTURE_LEVELS; i++)
	{
		m_textureLevels[i].type = TT_NONE;
		m_textureLevels[i].isArrayEnabled = false;
	}

	ClearNewStates();

	for (int statePointID = 0; statePointID < MAX_STATE_POINT_COUNT; statePointID++)
	{
		m_savedStates[statePointID][0] = 0xFFFFFFFF;
	}
}

void CStatesProcessor::SetMaterial(int materialID)
{
	m_newMaterialID = materialID;

	if (m_newMaterialID == m_lastMaterialID)
	if (m_newMaterialID != -1)
	{
		CMaterial* mat = g->matm.GetObjectPointer(m_newMaterialID);
		if (mat->IsZeroLevelUsed())
		{
			m_lastMaterialID = -1;
		}
	}
}

void CStatesProcessor::SaveStates()
{
	// to do
//	glPushAttrib(GL_ACCUM_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_PIXEL_MODE_BIT | GL_POLYGON_BIT | GL_TEXTURE_BIT | GL_STENCIL_BUFFER_BIT | GL_SCISSOR_BIT);
}

void CStatesProcessor::RestoreStates()
{
	// to do
//	glPopAttrib();
}

void CStatesProcessor::SetDefaultStates()
{
	GLFUNC(glEnable)(GL_DEPTH_TEST);
	m_states[ST_ZTEST] = true;
	m_newStates[ST_ZTEST].isChanged = false;

	GLFUNC(glDisable)(GL_BLEND);
	m_states[ST_BLEND] = false;
	m_newStates[ST_BLEND].isChanged = false;

	GLFUNC(glBlendFunc)(GL_SRC_ALPHA, GL_ONE);
	m_blendSrc = GL_SRC_ALPHA;
	m_blendDest = GL_ONE;

	GLFUNC(glDepthMask)(GL_TRUE);
	m_states[ST_ZWRITE] = true;
	m_newStates[ST_ZWRITE].isChanged = false;

	GLFUNC(glEnable)(GL_CULL_FACE);
	m_states[ST_CULL_FACE] = true;
	m_newStates[ST_CULL_FACE].isChanged = false;

	GLFUNC(glFrontFace)(GL_CCW);
	m_states[ST_FRONT_FACE] = GL_CCW;
	m_newStates[ST_FRONT_FACE].isChanged = false;

	GLFUNC(glDisable)(GL_ALPHA_TEST);
	m_states[ST_ALPHATEST] = false;
	m_newStates[ST_ALPHATEST].isChanged = false;

	GLFUNC(glDepthFunc)(GL_LEQUAL);
	m_states[ST_DEPTH_FUNC] = GL_LEQUAL;
	m_newStates[ST_DEPTH_FUNC].isChanged = false;

	GLFUNC(glColor4ub)(255, 255, 255, 255);
	m_r = 255;
	m_g = 255;
	m_b = 255;
	m_a = 255;
}

void CStatesProcessor::PrepareForRender()
{
	if (m_newMaterialID != m_lastMaterialID)
	{
		if (m_lastMaterialID != -1)
		{
			g->matm.UnbindMaterial(m_lastMaterialID);
		}

		if (m_newMaterialID != -1)
		{
			g->matm.BindMaterial(m_newMaterialID);
		}

		m_lastMaterialID = m_newMaterialID;
	}

	if (m_isBuffersChanged)
	{
		if (m_newNormalBufferID != m_normalBufferID)
		{
			if (m_newNormalBufferID != -1)
			{
				GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_newNormalBufferID);
				GLFUNC(glNormalPointer)(GL_FLOAT, 0, NULL);
			}

			m_normalBufferID = m_newNormalBufferID;
		}

		if (m_newTexCoordsBufferID1 != m_texCoordsBufferID1)
		{
			if (m_newTexCoordsBufferID1 != 0xFFFFFFFF)
			{
				GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_newTexCoordsBufferID1);
				GLFUNC(glTexCoordPointer)(m_textureDimension1, GL_FLOAT, 0, NULL);
			}
	
			m_texCoordsBufferID1 = m_newTexCoordsBufferID1;
		}
			
		if (m_newTexCoordsBufferID2 != m_texCoordsBufferID2)
		{
			if (m_newTexCoordsBufferID2 != 0xFFFFFFFF)
			{
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE1_ARB);
				GLFUNC(glEnableClientState)(GL_TEXTURE_COORD_ARRAY);
				GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_newTexCoordsBufferID2);
				GLFUNC(glTexCoordPointer)(m_textureDimension2, GL_FLOAT, 0, NULL);
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE0_ARB);
			}
			else
			{
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE1_ARB);
				GLFUNC(glDisableClientState)(GL_TEXTURE_COORD_ARRAY);
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE0_ARB);
			}

			m_texCoordsBufferID2 = m_newTexCoordsBufferID2;
		}

		if (m_newTexCoordsBufferID3 != m_texCoordsBufferID3)
		{
			if (m_newTexCoordsBufferID3 != 0xFFFFFFFF)
			{
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE2_ARB);
				GLFUNC(glEnableClientState)(GL_TEXTURE_COORD_ARRAY);
				GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_newTexCoordsBufferID3);
				GLFUNC(glTexCoordPointer)(m_textureDimension3, GL_FLOAT, 0, NULL);
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE0_ARB);
			}
			else
			{
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE2_ARB);
				GLFUNC(glDisableClientState)(GL_TEXTURE_COORD_ARRAY);
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE0_ARB);
			}

			m_texCoordsBufferID3 = m_newTexCoordsBufferID3;
		}

		if (m_newVertexBufferID != m_vertexBufferID)
		{
			if (m_newVertexBufferID != -1)
			{
				GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_newVertexBufferID);
				GLFUNC(glVertexPointer)(m_vertexDimension, GL_FLOAT, 0, NULL);		
			}

			m_vertexBufferID = m_newVertexBufferID;
		}

		if (m_newIndexBufferID != m_indexBufferID)
		{
			if (m_newIndexBufferID != -1)
			{
				GLFUNC(glBindBufferARB)(GL_ELEMENT_ARRAY_BUFFER, m_newIndexBufferID);
			}

			m_indexBufferID = m_newIndexBufferID;
		}

		m_isBuffersChanged = false;
	}

	SNewState* state = &m_newStates[0];
	for (int i = 0; i < MAX_STATES; i++, state++)
	if (state->isChanged)
	{
		BindState(i, state->value);
		m_states[i] = m_newStates[i].value;
		state->isChanged = false;
	}

	if (m_isChangedScissors)
	{
		if (m_newScissor.isEnabled)
		{
			GLFUNC(glEnable)(GL_SCISSOR_TEST); 
			GLFUNC(glScissor)(m_newScissor.x, m_currentHeight - m_newScissor.y - m_newScissor.height, 
				m_newScissor.width, m_newScissor.height);
		}
		else
		{
			GLFUNC(glDisable)(GL_SCISSOR_TEST);
		}

		m_oldScissor = m_newScissor;
		m_isChangedScissors = false;
	}
}

bool CStatesProcessor::CompareStates(const int statePointID)
{
	unsigned int currentStates[MAX_GL_STATES];	

	GLFUNC(glGetIntegerv)(GL_ALPHA_TEST, (GLint*)&currentStates[0]);
	GLFUNC(glGetIntegerv)(GL_ALPHA_TEST_FUNC, (GLint*)&currentStates[1]);
	GLFUNC(glGetIntegerv)(GL_ALPHA_TEST_REF, (GLint*)&currentStates[2]);
	GLFUNC(glGetIntegerv)(GL_BLEND, (GLint*)&currentStates[3]);
	GLFUNC(glGetIntegerv)(GL_BLEND_DST_ALPHA, (GLint*)&currentStates[4]);
	GLFUNC(glGetIntegerv)(GL_BLEND_DST_RGB, (GLint*)&currentStates[5]);
	GLFUNC(glGetIntegerv)(GL_BLEND_EQUATION_RGB, (GLint*)&currentStates[6]);
	GLFUNC(glGetIntegerv)(GL_BLEND_EQUATION_ALPHA, (GLint*)&currentStates[7]);
	GLFUNC(glGetIntegerv)(GL_BLEND_SRC_ALPHA, (GLint*)&currentStates[8]);
	GLFUNC(glGetIntegerv)(GL_BLEND_SRC_RGB, (GLint*)&currentStates[9]);
	GLFUNC(glGetIntegerv)(GL_COLOR_WRITEMASK, (GLint*)&currentStates[10]);
	GLFUNC(glGetIntegerv)(GL_CULL_FACE, (GLint*)&currentStates[14]);
    GLFUNC(glGetIntegerv)(GL_CULL_FACE_MODE, (GLint*)&currentStates[15]);
	GLFUNC(glGetIntegerv)(GL_DEPTH_BIAS, (GLint*)&currentStates[16]);
	GLFUNC(glGetIntegerv)(GL_DEPTH_FUNC, (GLint*)&currentStates[17]);
	GLFUNC(glGetIntegerv)(GL_DEPTH_TEST, (GLint*)&currentStates[18]);
	GLFUNC(glGetIntegerv)(GL_DEPTH_WRITEMASK, (GLint*)&currentStates[19]);
	GLFUNC(glGetIntegerv)(GL_FRONT_FACE, (GLint*)&currentStates[20]);
	GLFUNC(glGetIntegerv)(GL_POLYGON_OFFSET_FACTOR, (GLint*)&currentStates[21]);
	GLFUNC(glGetIntegerv)(GL_POLYGON_OFFSET_UNITS, (GLint*)&currentStates[22]);
	GLFUNC(glGetIntegerv)(GL_POLYGON_OFFSET_FILL, (GLint*)&currentStates[23]);

	int i;
	bool res = true;

	if (m_savedStates[statePointID][0] != -1)
	for (i = 0; i < 24; i++)
	if (m_savedStates[statePointID][i] != currentStates[i])
	{
		res = false;
	}

	for (i = 0; i < 24; i++)
	{
		m_savedStates[statePointID][i] = currentStates[i];
	}

	return res;
}


void CStatesProcessor::PushAttrib(GLbitfield msk)
{
	m_pushAttribCount++;

	GLFUNC(glPushAttrib)(msk);
}

void CStatesProcessor::PopAttrib()
{
	m_pushAttribCount--;

	assert(m_pushAttribCount >= 0);

	GLFUNC(glPopAttrib)();

	for (int i = 0; i < MAX_STATES; i++)
	{
		m_states[i] = 65535;
	}
}

void CStatesProcessor::SetViewport(int width, int height)
{
	m_currentWidth = width;
	m_currentHeight = height;
	GLFUNC(glViewport)(0, 0, width, height);
}

void CStatesProcessor::SetViewport(int x, int y, int width, int height)
{	
	m_currentWidth = width;
	m_currentHeight = height;
	GLFUNC(glViewport)(x, y, width, height);
}

void CStatesProcessor::PushMatrix()
{
	m_pushCount++;
	GLFUNC(glPushMatrix)();
}

void CStatesProcessor::PopMatrix()
{
	m_pushCount--;

	assert(m_pushCount >= 0);

	if (m_pushCount >= 0)
	{
		GLFUNC(glPopMatrix)();
	}
}


void CStatesProcessor::SetBlendFunc(int src, int dest)
{
	if ((src != m_blendSrc) || (dest != m_blendDest))
	{
		GLFUNC(glBlendFunc)(src, dest);
		m_blendSrc = src;
		m_blendDest = dest;
	}
}

void CStatesProcessor::SetAlphaFunc(int func, float value)
{
	if ((func != m_alphaFunc) || (value != m_alphaValue))
	{
		GLFUNC(glAlphaFunc)(func, value);
		m_alphaFunc = func;
		m_alphaValue = value;
	}
}

void CStatesProcessor::SetActiveTexture(unsigned char levelID)
{
	if ((levelID >= 0) && (levelID < MAX_TEXTURE_LEVELS))
	{
		m_activeLevel = levelID;
		GLFUNC(glActiveTextureARB)(GL_TEXTURE0_ARB + m_activeLevel);
	}
}

void CStatesProcessor::SetTextureState(unsigned char type)
{
	if (m_textureLevels[m_activeLevel].type != type)
	{
		switch (m_textureLevels[m_activeLevel].type)
		{
		case TT_NONE:
			// do nothing
			break;

		case TT_TEXTURE2D:
			GLFUNC(glDisable)(GL_TEXTURE_2D);
			break;

		case TT_TEXTURE3D:
			GLFUNC(glDisable)(GL_TEXTURE_3D);
			break;

		case TT_CUBEMAP:
			GLFUNC(glDisable)(GL_TEXTURE_CUBE_MAP);
			break;
		}

		switch (type)
		{
		case TT_NONE:
		// do nothing
			break;

		case TT_TEXTURE2D:
			GLFUNC(glEnable)(GL_TEXTURE_2D);
			break;

		case TT_TEXTURE3D:
			GLFUNC(glEnable)(GL_TEXTURE_3D);
			break;

		case TT_CUBEMAP:
			GLFUNC(glEnable)(GL_TEXTURE_CUBE_MAP);
			break;
		}

		m_textureLevels[m_activeLevel].type = type;
	}
}


void CStatesProcessor::SetColor(unsigned char r, unsigned char _g, unsigned char b, unsigned char a)
{
	if ((m_r != r) || (m_b != b) || (m_g != _g) || (m_a != a))
	{
		GLFUNC(glColor4ub)(r, _g, b, a);

		m_r = r;
		m_g = _g;
		m_b = b;
		m_a = a;
	}
}

void CStatesProcessor::BindState(int stateID, unsigned int value)
{
	switch (stateID)
	{
	case ST_STENCILTEST:
		ChangeEDState(GL_STENCIL_TEST, BoolFromUInt(value) ? GL_TRUE : GL_FALSE);
		break;
	case ST_BLEND:
		ChangeEDState(GL_BLEND, BoolFromUInt(value));
		break;
	case ST_BLENDFUNC:
		break;
	case ST_ALPHATEST:
		ChangeEDState(GL_ALPHA_TEST, BoolFromUInt(value));
		break;
	case ST_ALPHAFUNC:
		break;
	case ST_ZTEST:
		ChangeEDState(GL_DEPTH_TEST, BoolFromUInt(value));
		break;
	case ST_ZWRITE:
		GLFUNC(glDepthMask)(value == 0 ? GL_FALSE : GL_TRUE);
		break;
	case ST_VERTEX_ARRAY_CLIENT_STATE:
		ChangeClientState(GL_VERTEX_ARRAY, BoolFromUInt(value));
		break;
	case ST_NORMAL_ARRAY_CLIENT_STATE:
		ChangeClientState(GL_NORMAL_ARRAY, BoolFromUInt(value));
		break;
	case ST_INDEX_ARRAY_CLIENT_STATE:
		ChangeClientState(GL_INDEX_ARRAY, BoolFromUInt(value));
		break;
	case ST_LIGHTING:
		ChangeEDState(GL_LIGHTING, BoolFromUInt(value));
		break;
	case ST_CULL_FACE:
		ChangeEDState(GL_CULL_FACE, BoolFromUInt(value));
		break;
	case ST_FRONT_FACE:
		GLFUNC(glFrontFace)(value);
		break;
	case ST_DEPTH_FUNC:
		GLFUNC(glDepthFunc)(value);
		break;
	}
}

void CStatesProcessor::ChangeEDState(unsigned int state, bool value)
{
	if (value)
	{
		GLFUNC(glEnable)(state);
	}
	else
	{
		GLFUNC(glDisable)(state);
	}
}

void CStatesProcessor::ChangeClientState(unsigned int state, bool value)
{
	if (value)
	{
		GLFUNC(glEnableClientState)(state);
	}
	else
	{
		GLFUNC(glDisableClientState)(state);
	}
}

CStatesProcessor::~CStatesProcessor()
{

}
