
#pragma once

#include "CommonEngineHeader.h"

#define STATE_ALPHATEST					0
#define STATE_ALPHAVALUE				1
#define STATE_ALPHAFUNC					2
#define STATE_ANISOTROPY				3
#define STATE_COLOR						4
#define STATE_BLEND						5
#define STATE_TWO_SIDED_TRANSPARENCY	6
#define STATE_TWO_SIDED					7
#define STATE_BILLBOARD_ORIENT			8
#define STATE_BLEND_METHOD				9
#define STATE_Z_WRITE					10
#define STATE_NEED_TANGENTS				11
#define STATE_TEXTURE_MATRIX			12
#define STATE_TREAT_AS_ALPHATEST		13
#define STATE_LINE_SIZE					14
#define STATE_OPACITY					15
#define STATE_MIN_FILTER				16
#define STATE_MIPMAP					17
#define STATE_MAG_FILTER				18
#define STATE_MIPMAP_BIAS				19
#define STATE_ANISOTROPY_POWER			20

#define STATE_COUNT						21

#define STATES_START	void CMaterialStatesEnumerator::InitStatesDefinitions()	{
#define STATE_DEF(x, y)	m_statesToID.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(x), y));
#define STATES_END		}

class CMaterialStatesEnumerator
{
public:
	CMaterialStatesEnumerator();
	~CMaterialStatesEnumerator();
		
	unsigned int GetStateID(const std::string& name)const;

private:
	void InitStatesDefinitions();

	MP_MAP<MP_STRING, int> m_statesToID;
};