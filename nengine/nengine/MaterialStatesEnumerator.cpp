
#include "StdAfx.h"
#include "MaterialStatesEnumerator.h"

CMaterialStatesEnumerator::CMaterialStatesEnumerator():
	MP_MAP_INIT(m_statesToID)
{
	InitStatesDefinitions();
}

STATES_START
	STATE_DEF("alphatest", STATE_ALPHATEST)
	STATE_DEF("alphavalue", STATE_ALPHAVALUE)
	STATE_DEF("alphafunc", STATE_ALPHAFUNC)
	STATE_DEF("anisotropy", STATE_ANISOTROPY)
	STATE_DEF("color", STATE_COLOR)
	STATE_DEF("blend", STATE_BLEND)
	STATE_DEF("two_sided_transparency", STATE_TWO_SIDED_TRANSPARENCY)
	STATE_DEF("two_sided", STATE_TWO_SIDED)
	STATE_DEF("billboard_orient", STATE_BILLBOARD_ORIENT)
	STATE_DEF("blend_method", STATE_BLEND_METHOD)
	STATE_DEF("z_write", STATE_Z_WRITE)
	STATE_DEF("need_tangents", STATE_NEED_TANGENTS)
	STATE_DEF("texture_matrix", STATE_TEXTURE_MATRIX)
	STATE_DEF("treat_as_alphatest", STATE_TREAT_AS_ALPHATEST)
	STATE_DEF("line_size", STATE_LINE_SIZE)
	STATE_DEF("min_filter", STATE_MIN_FILTER)				
    STATE_DEF("mipmap", STATE_MIPMAP)					
	STATE_DEF("mag_filter", STATE_MAG_FILTER)				
	STATE_DEF("mipmap_bias", STATE_MIPMAP_BIAS)				
	STATE_DEF("anisotropy_power", STATE_ANISOTROPY_POWER)			
STATES_END

unsigned int CMaterialStatesEnumerator::GetStateID(const std::string& name)const
{
	MP_MAP<MP_STRING, int>::const_iterator it = m_statesToID.find(MAKE_MP_STRING(name));

	if (it != m_statesToID.end())
	{
		return (*it).second;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}

CMaterialStatesEnumerator::~CMaterialStatesEnumerator()
{
}