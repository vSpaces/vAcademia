
#pragma once

#include "CommonInfoHeader.h"

#include <string>
#include <map>

#define GLEXT_blend_minmax						1
#define GLEXT_blend_subtract					2
#define GLEXT_ati_separate_stencil				3
#define GLEXT_ext_stencil_two_side				4
#define GLEXT_arb_texture_compression			5
#define GLEXT_sgis_generate_mipmap				6
#define GLEXT_arb_fragment_shader				7
#define GLEXT_arb_multisample					8
#define GLEXT_arb_multitexture					9
#define GLEXT_arb_occlusion_query				10
#define GLEXT_arb_point_parameters				11
#define GLEXT_arb_point_sprite					12
#define GLEXT_arb_texture_cube_map				13
#define GLEXT_arb_texture_non_power_of_two		14
#define GLEXT_arb_texture_rectangle				15
#define GLEXT_arb_vertex_buffer_object			16
#define GLEXT_arb_vertex_shader					17
#define GLEXT_ext_abgr							18
#define GLEXT_ext_bgra							19
#define GLEXT_ext_copy_texture					20
#define GLEXT_ext_draw_range_elements			21
#define GLEXT_ext_framebuffer_blit				22
#define GLEXT_ext_framebuffer_multisample		23
#define GLEXT_ext_framebuffer_object			24
#define GLEXT_ext_framebuffer_srgb				25
#define GLEXT_ext_gpu_program_parameters		26
#define GLEXT_ext_packed_depth_stencil			27
#define GLEXT_ext_point_parameters				28
#define GLEXT_ext_stencil_wrap					29
#define GLEXT_ext_subtexture					30
#define GLEXT_ext_texture3d						31
#define GLEXT_ext_texture_compression_s3tc		32
#define GLEXT_ext_texture_cube_map				33
#define GLEXT_ext_texture_filter_anisotropic	34
#define GLEXT_ext_texture_rectangle				35
#define GLEXT_ext_texture_srgb					36
#define GLEXT_ext_vertex_array					37
#define GLEXT_arb_shader_objects				38
#define GLEXT_ati_mem_info						39
#define GLEXT_arb_vertex_array_object			40
#define GLEXT_arb_pixel_buffer_object			41
#define GLEXT_arb_get_program_binary			42

#define MAX_EXTENSIONS_COUNT	500

#define DEF_EXT_START void CExtensionsInfo::InitExtensionsList()	{
#define DEF_EXT(x) m_extensions.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(#x), GLEXT_##x##));
#define DEF_EXT_END	}

class CExtensionsInfo
{
public:
	CExtensionsInfo();
	~CExtensionsInfo();

	std::string GetExtensionsString();

	bool IsExtensionSupported(std::string extension);
	bool IsExtensionSupported(int extensionID);

private:
	void InitExtensionsList();
	void ParseExtensions();

	MP_STRING m_extensionsString;
	MP_MAP<MP_STRING, int> m_extensions;

	bool m_isExtensionsParsed;
	bool m_extensionsSupport[MAX_EXTENSIONS_COUNT];
};