
#include "StdAfx.h"
#include <Assert.h>
#include "Special.h"
#include "RenderSettings.h"
#include "HelperFunctions.h"
#include "XMLPropertiesList.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"
#include "cal3d/memory_leak.h" 

#define LOW_QUALITY_MODE_NAME			"low"
#define MEDIUM_QUALITY_MODE_NAME		"medium"
#define HIGH_QUALITY_MODE_NAME			"high"
#define VERYHIGH_QUALITY_MODE_NAME		"veryhigh"

DEF_START
		DEF_PARAM(NO_COMPRESSION, "no_compression", 0);
		DEF_PARAM(RENDER_TARGET_WITH_STENCIL_MODE, "render_target_with_stencil_mode", 0);
		DEF_PARAM(POSTEFFECT_ENABLED, "posteffect_enabled", 0);
		DEF_PARAM(NORMAL_TEXTURES_PACKING, "normal_textures_packing_enabled", 1);
		DEF_PARAM(TRANSPARENT_TEXTURES_PACKING, "transparent_textures_packing_enabled", 1);
		DEF_PARAM(TIME_OF_DAY_ENABLED, "time_of_day_enabled", 0);
		DEF_PARAM(SHADERS_USING, "shaders_using", 0);
		DEF_PARAM(ZONLY_PASS, "zonly_pass", 0);
		DEF_PARAM(CUBEMAP_PACKING, "cubemap_packing_enabled", 0);
		DEF_PARAM(PERMIT_TRANSPARENT_MIPMAP, "permit_transparent_mipmap", 0);
		DEF_PARAM(VERTICAL_AXE_Z, "vertical_axe_z", 0);	
		DEF_PARAM(POSTEFFECT_MAX_POWER, "posteffect_max_power", 7);
		DEF_PARAM(DAY_LENGTH, "day_length", 30);
		DEF_PARAM(SHADOWS_ENABLED, "shadows_enabled", true);
		DEF_PARAM(DRAW_AFTER_TEXTURE_LOADING, "draw_after_texture_loading", 0);
		DEF_PARAM(SKIP_FRAMES_TEXTURE_LOADING, "skip_frames_texture_loading", 0);
		DEF_PARAM(MAX_ANISOTROPY, "max_anisotropy", 0);
		DEF_PARAM(GLOBAL_MATERIAL, "global_material", 0);
		DEF_PARAM(MSAA, "multisample_aa", 0);
		DEF_PARAM(UNDER_MOUSE_CHECK_DELTA_TIME, "under_mouse_check_delta_time", 30);
		DEF_PARAM(REPROCESS_SHADOWS_DELTA_TIME, "reprocess_shadows_delta_time", 30);
		DEF_PARAM(REPROCESS_ANIMATION_DELTA_TIME, "reprocess_animation_delta_time", 66);
		DEF_PARAM(SHOW_BOUNDING_BOXES, "show_bounding_boxes", false);
		DEF_PARAM(VIDEO_UPDATE_DELTA_TIME, "video_update_delta_time", 60);
		DEF_PARAM(TIME_OF_DAY_START_TIME, "time_of_day_start_time", 0);
		DEF_PARAM(CLOUDS_SPEED, "clouds_speed", 0);
		DEF_PARAM(DYNAMIC_CLOUDS_ENABLED, "dynamic_clouds_enabled", 0);
		DEF_PARAM(DEBUG_PHYSICS_OBJECTS, "debug_physics_objects", 0);
		DEF_PARAM(VSYNC_ENABLED, "vsync_enabled", 0);
		DEF_PARAM(FIX_TIME_OF_DAY_ENABLED, "fixed_time_of_day", 0);
		DEF_PARAM(DRAW_ZONES_BOUNDS, "draw_zones_bounds", 0);
		DEF_PARAM(MAX_DIST_SQ_FOR_IDLES, "max_dist_sq_for_idles", 5000 * 5000);
		DEF_PARAM(REPROCESS_IDLES_DELTA_TIME, "reprocess_idles_delta_time", 100);
		DEF_PARAM(MAX_TEXTURE_LOADING_TIME, "max_texture_loading_time", 15);
		DEF_PARAM(COMMON_DELTA_TIME, "common_delta_time", 50);
		DEF_PARAM(FOV, "fov", 35);
		DEF_PARAM(OCCLUSION_QUERY_ENABLED, "occlusion_query_enabled", true);
		DEF_PARAM(SHADOWMAP_SIZE, "shadowmap_size", 2048);
		DEF_PARAM(SHADOWMAP_SHIFT_XY, "shadowmap_shift_xy", 2000);
		DEF_PARAM(SHADOWMAP_SHIFT_Z, "shadowmap_shift_z", 5000);
		DEF_PARAM(SHADOWMAP_BIAS, "shadowmap_bias", 500);
		DEF_PARAM(SHADOWMAP_NEAR_PLANE, "shadowmap_near_plane", 1000);
		DEF_PARAM(SHADOWMAP_FAR_PLANE, "shadowmap_far_plane", 10000);
		DEF_PARAM(SHADOWMAP_MIN_UPDATE_TIME, "shadowmap_min_update_time", 100);
		DEF_PARAM(SHADOWMAP_MIN_UPDATE_TIME_DYNAMIC, "shadowmap_min_update_time_dynamic", 50);
		DEF_PARAM(SHADOWMAP_BITS, "shadowmap_bits", 32);
		DEF_PARAM(TEXTURE_SIZE_DIV, "texture_size_div", 1);
		DEF_PARAM(MAX_DIST_SQ_FOR_MIMIC, "max_dist_sq_for_mimic", 1000 * 1000);
		DEF_PARAM(NO_PBO, "no_pbo", false);
		DEF_PARAM(DISABLE_TEXTURES, "disable_textures", false);
		DEF_PARAM(PRECISION_OBJECT_PICKING, "precision_object_picking", false);
		DEF_PARAM(REFLECTION_TEXTURE_NEEDED, "reflection_texture_needed", false);
		DEF_PARAM(REFRACTION_TEXTURE_NEEDED, "refraction_texture_needed", false);
		DEF_PARAM(MINIMAL_NORMAL_FPS, "minimal_normal_fps", 5);
		DEF_PARAM(SHOW_SKY, "show_sky", true);
		DEF_PARAM(NAME, "name", LOW_QUALITY_MODE_ID);
		DEF_PARAM(UPDATE_ANIMATION_IN_MAIN_THREAD_ONLY, "update_animation_in_main_thread_only", false);
		DEF_PARAM(AE_FOV, "ae_fov", 35);
		DEF_PARAM(AE_ASPECT, "ae_aspect", 67);
		DEF_PARAM(AE_Z, "ae_z", 3);
		DEF_PARAM(NORMAL_LOD_SCALE, "normal_lod_scale", 100);
		DEF_PARAM(SHOW_CHECK_OCCLUSION_TARGETS, "show_check_occlusion_targets", false);
		DEF_PARAM(MAX_CLOTHES_LOADING_FRAME_TIME, "max_clothes_loading_frame_time", 5);
		DEF_PARAM(MAX_COMPOSITE_TEXTURES_FRAME_TIME, "max_composite_textures_frame_time", 5);
		DEF_PARAM(MAX_SAO_LOADING_FRAME_TIME, "max_sao_loading_frame_time", 10);
		DEF_PARAM(NO_DYNAMIC_OBJECTS, "no_dynamic_objects", false);
		DEF_PARAM(USE_VBO, "use_vbo", true);
		DEF_PARAM(USE_VA, "use_va", true);
		DEF_PARAM(LOW_QUALITY_ECONOMY, "low_quality_economy", false);
		DEF_PARAM(CAVE_SUPPORT, "cave_support", false);
		DEF_PARAM(INTERACTIVE_BOARD_MONITOR_ID, "interactive_board_monitor_id", 1);
		DEF_PARAM(ONLY_2D_MODE, "only_2d_mode", false);
		DEF_PARAM(SCISSORS_SCALE, "scissors_scale", 1);		
		DEF_PARAM(SHOW_AVATAR_CLOUDS, "show_avatar_clouds", true);
		DEF_PARAM(PARTICLES_UPDATE_TIME, "particles_update_time", 40);
		DEF_PARAM(FRAMES_SKIPPED_FOR_PARTICLES_UPDATE, "frames_skipped_for_particles_update", 1);
		DEF_PARAM(MAX_PARTICLES_COUNT, "max_particles", 1000);
		DEF_PARAM(MAX_PARTICLES_TO_ENABLE_LIMIT_SIZE, "max_particles_to_enable_limit_size", 500);
		DEF_PARAM(PARTICLE_LIMIT_SIZE, "particle_limit_size", 300);

		DEF_VALUE(0, "false");
		DEF_VALUE(1, "true");
		DEF_VALUE(LOW_QUALITY_MODE_ID, "low");
		DEF_VALUE(RENDER_TARGET_WITH_STENCIL_COPY_FROM_FRAMEBUFFER, "copy_framebuffer");
		DEF_VALUE(RENDER_TARGET_WITH_STENCIL_PACKED_DEPTH_STENCIL, "packed_depth_stencil");
		DEF_VALUE(RENDER_TARGET_WITH_STENCIL_SEPARATE_BUFFERS, "separate_buffers");
		DEF_PARAM(NAVIGATION_A_LA_SL_ENABLED, "navigationSL", 0);
	DEF_END

CRenderSettings::CRenderSettings():
	m_globalMaterialID(-2),	// undefined global material (will be defined on first global material query)
	m_isOculusRiftEnabled(false),
	m_isHigherQualitySupported(-1),	
	MP_MAP_INIT(m_params),
	MP_MAP_INIT(m_stringsToValues),
	MP_MAP_INIT(m_defaultValues),
	MP_STRING_INIT(m_qualityMode)
{
	Init();

	MP_MAP<MP_STRING, int>::iterator it = m_params.begin();
	MP_MAP<MP_STRING, int>::iterator itEnd = m_params.end();

	m_maxSettingID = 0;
	for ( ; it != itEnd; it++)
	if ((*it).second > m_maxSettingID)
	{
		m_maxSettingID = (*it).second;
	}

	m_settings = MP_NEW_ARR(int, m_maxSettingID + 1);
	memset(m_settings, -1, (m_maxSettingID + 1) * sizeof(float));

	std::map<int, int>::iterator itm = m_defaultValues.begin();
	std::map<int, int>::iterator itmEnd = m_defaultValues.end();

	for ( ; itm != itmEnd; itm++)
	{
		m_settings[(*itm).first] = (*itm).second;
	}
}

void CRenderSettings::LoadAll(std::wstring fileName)
{
	// undefined global material (will be defined on first global material query)		
	m_globalMaterialID = -2;

	std::string vendorName = g->gi.GetVendorName();
	std::vector<SCheckField> checkFields;
	SCheckField vendorCheck;
	vendorCheck.field = "if_vendor";
	vendorCheck.value = vendorName;
	checkFields.push_back(vendorCheck);

	CXMLPropertiesList settings(GetApplicationRootDirectory() + fileName, ERROR_IF_NOT_FOUND, &checkFields);

	if (settings.IsLoaded())
	{
		CLogValue val("Loading render settings from ", fileName," - ok");
		g->lw.WriteLn(val);

		MP_MAP<MP_STRING, int>::iterator it = m_params.begin();
		MP_MAP<MP_STRING, int>::iterator itEnd = m_params.end();

		for ( ; it != itEnd; it++)
		{
			int value = 0;

			std::string val = settings.GetString((*it).first);
			val = StringToLower(val);

			if (val != "")
			{
				MP_MAP<MP_STRING, int>::iterator iter = m_stringsToValues.find(MAKE_MP_STRING(val));

				if ((LOW_QUALITY_MODE_NAME == val) || (MEDIUM_QUALITY_MODE_NAME == val)
					|| (HIGH_QUALITY_MODE_NAME == val) || (VERYHIGH_QUALITY_MODE_NAME == val))
				{
					SetQualityMode(val);
					g->matm.OnMaterialQualityChanged();
				}

				if (iter != m_stringsToValues.end())
				{
					value = (*iter).second;
				}
				else
				{
					value = rtl_atoi(val.c_str());

					if ((0 == value) && (val != "0"))
						g->lw.WriteLn("[WARNING] Property ", (*it).first, " has illegal value!");
				}
			}
			else 
			{
				if (m_settings[(*it).second] == -1)
				{
					std::map<int, int>::iterator itDefValue = m_defaultValues.find((*it).second);
					if (itDefValue != m_defaultValues.end())
					{
						value = (*itDefValue).second;
					}					
				}
				else
				{
					value = m_settings[(*it).second];
					g->lw.WriteLn("[WARNING] Property ", (*it).first, " is not exists or has empty value! Value ", m_settings[(*it).second], " was set.");
				}
			}

			m_settings[(*it).second] = value;
			OnSettingChanged((*it).second);
		}
	}
	
	CheckSettings();

	m_globalMaterialID =  (g->rs.GetBool(GLOBAL_MATERIAL)) ? 0 : -1;

	g->lod.SetLODScale(1);
}

void CRenderSettings::SetOculusRiftEnabled(const bool isEnabled)
{
	m_isOculusRiftEnabled = isEnabled;
}

bool CRenderSettings::IsOculusRiftEnabled()
{	
	if (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER)
	{
		return false;
	}

	return m_isOculusRiftEnabled;
}

void CRenderSettings::CheckSettings()
{
	unsigned int videoMemorySize = g->gi.GetVideoMemorySize();	
	__int64 ramSize = g->ci.GetPhysicalMemorySize();
	if (((videoMemorySize < 200 * 1024 * 1024) && (videoMemorySize != 0)) ||
		(ramSize <= 1024 * 1024 * 1024))
	{
		g->rs.SetInt(TEXTURE_SIZE_DIV, 2);
		g->rs.SetInt(SHADOWMAP_BITS, 24);
	}

	videoMemorySize = g->gi.GetVideoMemory()->GetTotalSize();
	if (videoMemorySize < 200 * 1024 * 1024)
	{
		g->rs.SetInt(TEXTURE_SIZE_DIV, ((g->gi.GetVendorID() == VENDOR_INTEL) || (g->gi.GetVendorID() == VENDOR_SIS)) ? 4 : 2);
		g->rs.SetInt(SHADOWMAP_BITS, 24);
	}

#ifdef ENABLE_INTEL_EMULATION
	g->rs.SetInt(TEXTURE_SIZE_DIV, 4);
#endif
	
	if ((g->gi.GetVendorID() == VENDOR_INTEL) || (g->gi.GetVendorID() == VENDOR_SIS))
	{
		g->rs.SetBool(SHADOWS_ENABLED, false);
	}

	if (g->gi.GetVendorID() == VENDOR_ATI)
	{
		if (g->rs.GetInt(DRAW_AFTER_TEXTURE_LOADING) != 16)
		{
			g->rs.SetBool(DRAW_AFTER_TEXTURE_LOADING, true);
		}
		else
		{
			g->rs.SetBool(DRAW_AFTER_TEXTURE_LOADING, false);
		}
	}

	g->rs.SetBool(USE_VBO, g->ei.IsExtensionSupported(GLEXT_arb_vertex_buffer_object));	

	/*if (g->gi.GetVendorID() == VENDOR_INTEL)
	{*/
		//g->rs.SetBool(USE_VBO, false);
	//}

	g->rs.SetBool(USE_VA, g->ei.IsExtensionSupported(GLEXT_ext_vertex_array));	
}
	
void CRenderSettings::SetBool(int settingID, bool value)
{
	assert(settingID >= 0);
	assert(settingID <= m_maxSettingID);

	m_settings[settingID] = (int)value;
	OnSettingChanged(settingID);
}

void CRenderSettings::SetInt(int settingID, int value)
{
	assert(settingID >= 0);
	assert(settingID <= m_maxSettingID);

	m_settings[settingID] = value;
	OnSettingChanged(settingID);
}	

void CRenderSettings::SetQualityMode(const std::string& value)
{
	m_qualityMode = value;

	if ((m_qualityMode == "medium") && (g->gi.GetVendorID() == VENDOR_INTEL))
	{
		m_qualityMode = "medium_intel";
	}
}

void CRenderSettings::OnSettingChanged(int settingID)
{
	assert(settingID >= 0);
	assert(settingID <= m_maxSettingID);

	if (SHADOWS_ENABLED == settingID)
	if ((g->rs.GetBool(SHADERS_USING)) && (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER))
	{
		std::vector<CShader *>::iterator it = g->sm.GetLiveObjectsBegin();
		std::vector<CShader *>::iterator itEnd = g->sm.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
 			(*it)->SetShadowsExistStatus(m_settings[settingID] == 1);
		}
	}
}

bool CRenderSettings::IsNotOnlyLowQualitySupported()
{
	if (m_isHigherQualitySupported == -1)
	{
		m_isHigherQualitySupported = g->ei.IsExtensionSupported(GLEXT_arb_shader_objects);

		if (GetBool(LOW_QUALITY_ECONOMY))
		{
			m_isHigherQualitySupported = false;
		}
	}
	
	return (m_isHigherQualitySupported > 0);
}

CRenderSettings::~CRenderSettings()
{
	MP_DELETE_ARR(m_settings);
}