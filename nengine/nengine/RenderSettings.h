
#pragma once

#include "CommonEngineHeader.h"

#include <string>
#include <map>

#define NO_COMPRESSION						0
#define RENDER_TARGET_WITH_STENCIL_MODE		1
#define POSTEFFECT_ENABLED					2
#define NORMAL_TEXTURES_PACKING				3
#define TRANSPARENT_TEXTURES_PACKING		4
#define CUBEMAP_PACKING						5
#define TIME_OF_DAY_ENABLED					6
#define SHADERS_USING						7
#define ZONLY_PASS							8
#define PERMIT_TRANSPARENT_MIPMAP			9
#define VERTICAL_AXE_Z						10
#define POSTEFFECT_MAX_POWER				11
#define DAY_LENGTH							12
#define SHADOWS_ENABLED						13
#define DRAW_AFTER_TEXTURE_LOADING			14
#define SKIP_FRAMES_TEXTURE_LOADING			15
#define MAX_ANISOTROPY						16
#define GLOBAL_MATERIAL						17
#define MSAA								18
#define UNDER_MOUSE_CHECK_DELTA_TIME		19
#define REPROCESS_SHADOWS_DELTA_TIME		20
#define REPROCESS_ANIMATION_DELTA_TIME		21
#define SHOW_BOUNDING_BOXES					22
#define VIDEO_UPDATE_DELTA_TIME				23
#define TIME_OF_DAY_START_TIME				24
#define CLOUDS_SPEED						25
#define DYNAMIC_CLOUDS_ENABLED				26
#define DEBUG_PHYSICS_OBJECTS				27
#define VSYNC_ENABLED						28
#define FIX_TIME_OF_DAY_ENABLED				29
#define DRAW_ZONES_BOUNDS					30
#define MAX_DIST_SQ_FOR_IDLES				31
#define REPROCESS_IDLES_DELTA_TIME			32
#define MAX_TEXTURE_LOADING_TIME			33
#define COMMON_DELTA_TIME					34
#define FOV									35
#define OCCLUSION_QUERY_ENABLED				36
#define SHADOWMAP_SIZE						37
#define SHADOWMAP_SHIFT_XY					38
#define SHADOWMAP_SHIFT_Z					39
#define SHADOWMAP_BIAS						40
#define SHADOWMAP_FAR_PLANE					41
#define SHADOWMAP_NEAR_PLANE				42
#define SHADOWMAP_MIN_UPDATE_TIME			43
#define SHADOWMAP_BITS						44
#define TEXTURE_SIZE_DIV					45
#define MAX_DIST_SQ_FOR_MIMIC				46
#define NO_PBO								47
#define DISABLE_TEXTURES					48
#define PRECISION_OBJECT_PICKING			49
#define REFLECTION_TEXTURE_NEEDED			50
#define REFRACTION_TEXTURE_NEEDED				51
#define MINIMAL_NORMAL_FPS						52
#define SHOW_SKY								53
#define NAME									54
#define SHADOWMAP_MIN_UPDATE_TIME_DYNAMIC		55
#define UPDATE_ANIMATION_IN_MAIN_THREAD_ONLY	56
#define AE_FOV			57
#define AE_ASPECT		58
#define AE_Z			59
#define NORMAL_LOD_SCALE						60
#define SHOW_CHECK_OCCLUSION_TARGETS			61
#define NAVIGATION_A_LA_SL_ENABLED				62
#define MAX_CLOTHES_LOADING_FRAME_TIME			63
#define MAX_COMPOSITE_TEXTURES_FRAME_TIME		64
#define MAX_SAO_LOADING_FRAME_TIME				65
#define NO_DYNAMIC_OBJECTS						66
#define USE_VBO									67
#define USE_VA									68
#define LOW_QUALITY_ECONOMY						69
#define CAVE_SUPPORT							70
#define	INTERACTIVE_BOARD_MONITOR_ID			71
#define ONLY_2D_MODE							72
#define SCISSORS_SCALE							73
#define SHOW_AVATAR_CLOUDS						74
#define PARTICLES_UPDATE_TIME					75
#define FRAMES_SKIPPED_FOR_PARTICLES_UPDATE		76
#define MAX_PARTICLES_COUNT						77
#define MAX_PARTICLES_TO_ENABLE_LIMIT_SIZE		78
#define PARTICLE_LIMIT_SIZE						79

#define RENDER_TARGET_WITH_STENCIL_COPY_FROM_FRAMEBUFFER	1
#define RENDER_TARGET_WITH_STENCIL_PACKED_DEPTH_STENCIL		2
#define RENDER_TARGET_WITH_STENCIL_SEPARATE_BUFFERS			3

#define DEFAULT_MIN_FACES_FOR_VBO	20

#define LOW_QUALITY_MODE_ID			0

#define DEF_START void CRenderSettings::Init() {
#define DEF_END	}
#define DEF_PARAM(x, y, z) m_params.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(y), x)); m_defaultValues.insert(std::map<int, int>::value_type(x, z));
#define DEF_VALUE(x, y) m_stringsToValues.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(y), x));

class CRenderSettings
{
public:
	CRenderSettings();
	~CRenderSettings();

	void LoadAll(std::wstring fileName = L"render_settings.xml");

	void SetBool(int settingID, bool value);
	void SetInt(int settingID, int value);

	void SetQualityMode(const std::string& value);

	void CheckSettings();

	// возможность физически перейти в режим графики, более высокий чем низкий
	bool IsNotOnlyLowQualitySupported();

	std::string GetQualityMode()const
	{
		return m_qualityMode;
	}
	
	__forceinline bool GetBool(int settingID)
	{
		return (m_settings[settingID] > 0);
	}

	__forceinline int GetInt(int settingID)
	{
		return m_settings[settingID];
	}

	__forceinline int GetGlobalMaterialID()const
	{	
		return m_globalMaterialID;
	}

	void SetOculusRiftEnabled(const bool isEnabled);
	bool IsOculusRiftEnabled();

private:
	void Init();
	void OnSettingChanged(int settingID);

	int m_globalMaterialID;

	MP_MAP<MP_STRING, int> m_params;
	MP_MAP<MP_STRING, int> m_stringsToValues;

	MP_MAP<int, int> m_defaultValues;

	int* m_settings;
	int m_maxSettingID;

	MP_STRING m_qualityMode;

	bool m_isOculusRiftEnabled;
	int m_isHigherQualitySupported;	
};