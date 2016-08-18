
#pragma once

#include "CommonEngineHeader.h"

#include "Vector4d.h"
#include "Manager.h"
#include "Shader.h"
#include "BaseChangesInformer.h"
#include "GlobalInterfaceStorage.h"

#define SHADER_PARAM_FLOAT			1
#define SHADER_PARAM_FLOAT_STR		MAKE_MP_STRING("float")
#define SHADER_PARAM_INT			2
#define SHADER_PARAM_INT_STR		MAKE_MP_STRING("int")
#define SHADER_PARAM_VECTOR			3
#define SHADER_PARAM_VECTOR_STR		MAKE_MP_STRING("vector")
#define SHADER_PARAM_TEXTURE		4
#define SHADER_PARAM_TEXTURE_STR	MAKE_MP_STRING("texture")

#define SHADER_PARAM_PREDEFINED		10

#define SHADER_PREDEFINED_PARAM_TIME			1
#define SHADER_PREDEFINED_PARAM_TIME_STR		MAKE_MP_STRING("%time")
#define SHADER_PREDEFINED_PARAM_CAMERAPOS		2
#define SHADER_PREDEFINED_PARAM_CAMERAPOS_STR	MAKE_MP_STRING("%camera_position")
#define SHADER_PREDEFINED_PARAM_LIGHTPOS		3
#define SHADER_PREDEFINED_PARAM_LIGHTPOS_STR	MAKE_MP_STRING("%light_position")
#define SHADER_PREDEFINED_PARAM_LIGHTNESS		4
#define SHADER_PREDEFINED_PARAM_LIGHTNESS_STR	MAKE_MP_STRING("%lightness")
#define SHADER_PREDEFINED_PARAM_LIGHTCOLOR		5
#define SHADER_PREDEFINED_PARAM_LIGHTCOLOR_STR	MAKE_MP_STRING("%light_color")
#define SHADER_PREDEFINED_PARAM_MOVELIGHTPOS		6
#define SHADER_PREDEFINED_PARAM_MOVELIGHTPOS_STR	MAKE_MP_STRING("%move_light_position")
#define SHADER_PREDEFINED_PARAM_TANGENT			7
#define SHADER_PREDEFINED_PARAM_TANGENT_STR		MAKE_MP_STRING("%tangent")

#define MAX_LOG_INFO_SIZE	16384

#pragma  warning (disable: 4786)

typedef MP_MAP<MP_STRING, int> PredefinedParamList;
typedef MP_MAP<MP_STRING, int> ParamTypeList;

extern float g_shaderTime;

class CShaderManager : public CManager<CShader>, public CBaseChangesInformer
{
friend class CGlobalSingletonStorage;

public:
	// initialize Shader Manager
	bool Init();
	void ReloadShaders();

	int LoadShaderFromMemory(std::string name, std::string& vxShader, std::string& frShader);
	int LoadShaderFromMemory(std::wstring name, std::string& vxShader, std::string& frShader);

	// return object number or add new object to list if object couldn't find
	int GetObjectNumber(std::string name);
	int GetObjectNumber(std::wstring name);

	void SetDefineValue(int defineID, bool value);
	void BindDefines();

	// bind shader from list
	bool BindShader(int num, int lastUsedByMaterialID = -1);
	// unbind shaders
	void UnbindShader();

	__forceinline void RefreshShaderOptimization()
	{
		m_lastShader = NULL;
	}

	__forceinline bool BindShader(CShader* shader, int lastUsedByMaterialID = -1)
	{
		if (m_lastShader != shader)
		{
			shader->Bind(lastUsedByMaterialID);
			m_lastShader = shader;
			return true;
		}
		else
		{
			return shader->IsMaterialChanged(lastUsedByMaterialID);
		}
	}

	bool IsShadersInit();
	void OnNewFrame(bool adjustTime = true);

	void EnableShadering();
	void DisableShadering();

	// set vector for shader
	int SetUniformVector(int i, const char * name, const CVector4D& value);
	void SetUniformVector(int location, const CVector4D& value);
	// set texture for shader
	int SetUniformTexture(int i, const char *name, int texUnit);
	void SetUniformTexture(int location, int texUnit);
	// set float value for shader
	int SetUniformFloat(int i, const char * name, float value);
	void SetUniformFloat(int location, float value);
	// set matrix for shader
	int SetUniformMatrix(int i, const char* name, float* value);
	void SetUniformMatrix(int location, float* value);

	int SetUniformPredefined(int shaderID, std::string name, int valueID);
	void SetUniformPredefined(int location, int valueID);

	int GetPredefinedParamID(std::string param);
	int GetParamTypeID(std::string paramType);

	void SetLightMatrix(float lightMatrix[16], float lightMatrix2[16]);

	void SavePrecompiledShaders();

public:
	float m_lightMatrix[16];
	float m_lightMatrix2[16];

private:
	CShaderManager();
	CShaderManager(const CShaderManager&);
	CShaderManager& operator=(const CShaderManager&);
	~CShaderManager();

	void DeleteShaders();
	bool InitShaders();

	CShader* m_lastShader;

	PredefinedParamList m_predefinedParamList;
	ParamTypeList m_paramTypeList;
	int m_lastShadowmapSize;

	bool m_isInit;

	bool m_isShaderingEnabled;

	bool m_defines[DEFINES_COUNT];
	bool m_isDefinesChanged;
};
