
#include "StdAfx.h"
#include <Assert.h>
#include "ExtensionsInfo.h"
#include <glew.h>
#include "HelperFunctions.h"

	DEF_EXT_START
		DEF_EXT(blend_minmax)
		DEF_EXT(blend_subtract)
		DEF_EXT(ati_separate_stencil)
		DEF_EXT(ext_stencil_two_side)
		DEF_EXT(arb_texture_compression)
		DEF_EXT(sgis_generate_mipmap)
		DEF_EXT(arb_fragment_shader)
		DEF_EXT(arb_multisample)
		DEF_EXT(arb_multitexture)
		DEF_EXT(arb_occlusion_query)
		DEF_EXT(arb_point_parameters)
		DEF_EXT(arb_point_sprite)
		DEF_EXT(arb_texture_cube_map)
		DEF_EXT(arb_texture_non_power_of_two)
		DEF_EXT(arb_texture_rectangle)
		DEF_EXT(arb_vertex_buffer_object)
		DEF_EXT(arb_vertex_shader)
		DEF_EXT(ext_abgr)
		DEF_EXT(ext_bgra)
		DEF_EXT(ext_copy_texture)
		DEF_EXT(ext_draw_range_elements)
		DEF_EXT(ext_framebuffer_blit)
		DEF_EXT(ext_framebuffer_multisample)
		DEF_EXT(ext_framebuffer_object)
		DEF_EXT(ext_framebuffer_srgb)
		DEF_EXT(ext_gpu_program_parameters)
		DEF_EXT(ext_packed_depth_stencil)
		DEF_EXT(ext_point_parameters)
		DEF_EXT(ext_stencil_wrap)
		DEF_EXT(ext_subtexture)
		DEF_EXT(ext_texture3d)
		DEF_EXT(ext_texture_compression_s3tc)
		DEF_EXT(ext_texture_cube_map)
		DEF_EXT(ext_texture_filter_anisotropic)
		DEF_EXT(ext_texture_rectangle)
		DEF_EXT(ext_texture_srgb)
		DEF_EXT(ext_vertex_array)
		DEF_EXT(arb_shader_objects)
		DEF_EXT(ati_mem_info)
		DEF_EXT(arb_vertex_array_object)
		DEF_EXT(arb_pixel_buffer_object)
		DEF_EXT(arb_get_program_binary)		
	DEF_EXT_END

CExtensionsInfo::CExtensionsInfo():
	m_isExtensionsParsed(false),
	MP_STRING_INIT(m_extensionsString),
	MP_MAP_INIT(m_extensions)
{
	InitExtensionsList();
}

std::string CExtensionsInfo::GetExtensionsString()
{
	if (!m_isExtensionsParsed)
	{
		unsigned char* extensions = (unsigned char*)GLFUNCR(glGetString(GL_EXTENSIONS));

		if (extensions)
		{
			m_extensionsString = (char*)extensions;
			m_extensionsString = StringToLower(m_extensionsString);

			ParseExtensions();
		}
		else
		{
			m_extensionsString = "";
		}
	}

	return m_extensionsString;
}

bool CExtensionsInfo::IsExtensionSupported(std::string extension)
{
	extension = StringToLower(extension);
	if (!m_isExtensionsParsed)
	{
		GetExtensionsString();
	}

	return (m_extensionsString.find(extension) != -1);
}

bool CExtensionsInfo::IsExtensionSupported(int extensionID)
{
	assert(extensionID >= 0);
	assert(extensionID < MAX_EXTENSIONS_COUNT);

	if (!m_isExtensionsParsed)
	{
		GetExtensionsString();
	}

	return m_extensionsSupport[extensionID];
}

void CExtensionsInfo::ParseExtensions()
{
	MP_MAP<MP_STRING, int>::iterator it = m_extensions.begin();
	MP_MAP<MP_STRING, int>::iterator itEnd = m_extensions.end();

	m_isExtensionsParsed = true;

	for ( ; it != itEnd; it++)
	{
		m_extensionsSupport[(*it).second] = IsExtensionSupported((*it).first);
	}

	// workaround for Intel driver bug
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		SOpenGLVersion ver = g->gi.GetOpenGLVersionNumber();
		if (ver.major < 2)
		{
			m_extensionsSupport[GLEXT_arb_occlusion_query] = false;
		}
	}
}

CExtensionsInfo::~CExtensionsInfo()
{
}