
#pragma once

#include "HelperFunctions.h"
#include "OGLDebug.h"

#define MAX_STATES	14

#define ST_STENCILTEST						0
#define ST_BLEND							1
#define ST_BLENDFUNC						2
#define ST_ALPHATEST						3
#define ST_ALPHAFUNC						4
#define ST_ZTEST							5
#define ST_ZWRITE							6
#define ST_VERTEX_ARRAY_CLIENT_STATE		7
#define ST_NORMAL_ARRAY_CLIENT_STATE		8
#define ST_INDEX_ARRAY_CLIENT_STATE			9
#define ST_LIGHTING							10
#define ST_CULL_FACE						11
#define ST_FRONT_FACE						12
#define ST_DEPTH_FUNC						13

#define MAX_TEXTURE_LEVELS	8

#define TT_NONE				0
#define TT_TEXTURE2D		1
#define TT_TEXTURE3D		2
#define TT_CUBEMAP			3

#define MAX_STATE_POINT_COUNT	5
#define	MAX_GL_STATES			50

typedef struct _SNewState
{
	bool isChanged;
	unsigned int value;	
} SNewState;

typedef struct _STextureLevel
{
	unsigned char type;
	bool isArrayEnabled;
} STextureLevel;

typedef struct _SScissor
{
	int x, y;
	int width, height;
	bool isEnabled;

	_SScissor()
	{
		isEnabled = false;
	}

	_SScissor(const _SScissor& a)
	{
		x = a.x;
		y = a.y;
		width = a.width;
		height = a.height;
		isEnabled = a.isEnabled;
	}
} SScissor;

class CStatesProcessor
{
friend class CGlobalSingletonStorage;

public:
	void PrepareForRender();
	void SetDefaultStates();

	bool CompareStates(const int statePointID);

	__forceinline int GetWidth()const
	{
		return m_width;
	}

	__forceinline int GetHeight()const
	{
		return m_height;
	}

	__forceinline void SetSize(int x, int y, int width, int height)
	{
		m_viewportX = x;
		m_viewportY = y;
		m_width = width;

		m_currentWidth = width;
		m_height = height;
		m_currentHeight = height;
	}

	__forceinline void GetSize(int& x, int& y, unsigned int& width, unsigned int& height)
	{
		x = m_viewportX;
		y = m_viewportY;
		width = m_width;
		height = m_height;
	}

	__forceinline void RestoreViewport()
	{
		SetViewport(m_viewportX, m_viewportY, m_width, m_height);
	}

	__forceinline unsigned int GetCurrentWidth()const
	{
		return m_currentWidth;
	}

	__forceinline unsigned int GetCurrentHeight()const
	{
		return m_currentHeight;
	}
	void SaveStates();
	void RestoreStates();

	void SetMaterial(int materialID);

	__forceinline unsigned int GetState(int stateID)
	{
		if ((stateID < 0) || (stateID >= MAX_STATES))
		{
			return 0;
		}

		return m_states[stateID];
	}

	__forceinline void SetState(int stateID, unsigned int value)
	{
		if ((stateID < 0) || (stateID >= MAX_STATES))
		{
			return;
		}

		// ранее запроса на изменение этого стейта не приходило
		if (!m_newStates[stateID].isChanged)
		{
			// у стейта и так такое значение => ничего не делаем
			if (m_states[stateID] == value)
			{
				
			}
			else // поставим в список на изменение
			{
				m_newStates[stateID].isChanged = true;
				m_newStates[stateID].value = value;
			}
		}
		else	// ранее запрос на изменение этого стейта приходил => соптимизируем
		{
			// если новое значение стейта равно уже установленному, то не будем ставить стейт
			if (m_states[stateID] == value)
			{
				m_newStates[stateID].isChanged = false;
			}
			else
			{
				m_newStates[stateID].value = value;
			}
		}
	}

	__forceinline int GetActiveTexture()
	{
		return m_activeLevel;
	}

	__forceinline bool GetTextureArrayState(int levelID)
	{
		if ((levelID >= 0) && (levelID < MAX_TEXTURE_LEVELS))
		{
			return m_textureLevels[m_activeLevel].isArrayEnabled;
		}
		else
		{
			assert(false);

			return false;
		}
	}

	__forceinline void SetTextureArrayState(bool value)
	{
		if (m_textureLevels[m_activeLevel].isArrayEnabled != value)
		{
			m_textureLevels[m_activeLevel].isArrayEnabled = value;
			ChangeClientState(GL_TEXTURE_COORD_ARRAY, value);
		}
	}

	__forceinline void SetColor(unsigned char r, unsigned char g, unsigned char b)
	{
		SetColor(r, g, b, 255);
	}

	__forceinline void EnableScissors(int x, int y, int width, int height)
	{
		m_newScissor.x = x;
		m_newScissor.y = y;
		m_newScissor.width = width;
		m_newScissor.height = height;
		m_newScissor.isEnabled = true;

		m_isChangedScissors = (m_newScissor.isEnabled != m_oldScissor.isEnabled) ||
			(m_newScissor.x != m_oldScissor.x) || (m_newScissor.y != m_oldScissor.y) ||
			(m_newScissor.width != m_oldScissor.width) || (m_newScissor.height != m_oldScissor.height);
	}

	__forceinline void DisableScissors()
	{
		m_newScissor.isEnabled = false;
		m_isChangedScissors = (m_newScissor.isEnabled != m_oldScissor.isEnabled);
	}

	__forceinline void SetVertexBuffer(int vertexBufferID, int vertexDimension)
	{
		m_newVertexBufferID = vertexBufferID;
		m_vertexDimension = vertexDimension;
		m_isBuffersChanged = true;
	}

	__forceinline void SetIndexBuffer(int indexBufferID)
	{
		m_newIndexBufferID = indexBufferID;
		m_isBuffersChanged = true;
	}

	__forceinline void SetNormalBuffer(int normalBufferID)
	{
		m_newNormalBufferID = normalBufferID;
		m_isBuffersChanged = true;
	}

	__forceinline void SetTextureCoordsBuffer(int textureDimension1, int texCoordsBufferID1, int textureDimension2, int texCoordsBufferID2,
		int textureDimension3, int texCoordsBufferID3)
	{
		m_newTexCoordsBufferID1 = texCoordsBufferID1;
		m_newTexCoordsBufferID2 = texCoordsBufferID2;
		m_newTexCoordsBufferID3 = texCoordsBufferID3;

		m_textureDimension1 = textureDimension1;
		m_textureDimension2 = textureDimension2;
		m_textureDimension3 = textureDimension3;

		m_isBuffersChanged = true;
	}

	void BindState(int stateID, unsigned int value);
	void SetColor(unsigned char r, unsigned char _g, unsigned char b, unsigned char a);
	void SetTextureState(unsigned char type);
	void SetActiveTexture(unsigned char levelID);
	void SetAlphaFunc(int func, float value);
	void SetBlendFunc(int src, int dest);
	void PopMatrix();
	void PushMatrix();
	void SetViewport(int x, int y, int width, int height);
	void SetViewport(int width, int height);
	void PopAttrib();
	void PushAttrib(GLbitfield msk);

private:
	CStatesProcessor();
	CStatesProcessor(const CStatesProcessor&);
	CStatesProcessor& operator=(const CStatesProcessor&);
	~CStatesProcessor();

	void ChangeEDState(unsigned int state, bool value);
	void ChangeClientState(unsigned int state, bool value);

	__forceinline void CStatesProcessor::ClearNewStates()
	{
		for (int i = 0; i < MAX_STATES; i++)
		{
			m_newStates[i].isChanged = false;
		}
	}

	STextureLevel m_textureLevels[MAX_TEXTURE_LEVELS];

	unsigned int m_states[MAX_STATES];
	SNewState m_newStates[MAX_STATES];

	unsigned int m_savedStates[MAX_STATE_POINT_COUNT][MAX_GL_STATES];

	int m_vertexBufferID;
	int m_newVertexBufferID;
	int m_indexBufferID;
	int m_newIndexBufferID;
	int m_normalBufferID;
	int m_newNormalBufferID;
	int m_texCoordsBufferID1, m_texCoordsBufferID2, m_texCoordsBufferID3;
	int m_newTexCoordsBufferID1, m_newTexCoordsBufferID2, m_newTexCoordsBufferID3;

	int m_textureDimension1, m_textureDimension2, m_textureDimension3;
	
	int m_vertexDimension;

	int m_pushCount;
	int m_pushAttribCount;

	unsigned char m_activeLevel;

	unsigned short m_r, m_g, m_b, m_a;

	SScissor m_oldScissor;
	SScissor m_newScissor;

	int m_lastMaterialID;
	int m_newMaterialID;
	int m_blendSrc, m_blendDest;
	int m_alphaFunc;
	float m_alphaValue;

	bool m_isChangedScissors;
	bool m_isBuffersChanged;

	int m_viewportX, m_viewportY;
	// screen parameteres
	unsigned int m_width, m_height;
	unsigned int m_currentWidth, m_currentHeight;
};