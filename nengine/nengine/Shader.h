
#pragma once

#include "CommonEngineHeader.h"

#include "CommonObject.h"
#include <glew.h>
#include <map>
#include <vector>
#include <string>
#include "Vector2d.h"
#include "Vector3d.h"
#include "Vector4d.h"

extern bool noshadows;

#define MAX_SHADER_VARIABLES_COUNT	50

// учитывать шедоумап или нет (сам шедоумап всегда на 7 стейдже и всегда "shadow2D shadowMap;"
#define SHADER_DEFINE_SHADOWMAP 0
	
#define DEFINES_COUNT			1
#define DEFINES_COUNT_2ST		2

#define SHADER_VAR_UPDATE_TYPE_ONCE				1
#define SHADER_VAR_UPDATE_TYPE_ON_CHANGE		2
#define SHADER_VAR_UPDATE_TYPE_ONCE_AT_FRAME	3

typedef MP_MAP_DC<MP_STRING, int> TUniformMap;

class CShader : public CCommonObject
{
public:
	CShader();
	~CShader();

	__forceinline GLhandleARB GetVertexShader()const
	{
		return m_vertexShader[m_slot];
	}

	__forceinline GLhandleARB GetFragmentShader()const
	{
		return m_fragmentShader[m_slot];
	}

	GLhandleARB GetProgramObject()const;
	
	__forceinline void SetLastUsedByMaterialID(int materialID)
	{
		m_lastMaterialID = materialID;
	}

	__forceinline int GetLastUsedByMaterialID()const
	{
		return m_lastMaterialID;
	}

	__forceinline bool IsMaterialChanged(int lastUsedByMaterialID)
	{
		bool isChanged = (lastUsedByMaterialID != m_lastMaterialID);
		m_lastMaterialID = lastUsedByMaterialID;
			
		return isChanged;
	}

	void SetAlphaTestDisableOnATI(bool isDisabled);
	__forceinline bool IsAlphaTestDisableOnATI()const
	{
		return m_isAlphaTestDisableOnATI;
	}

	bool IsShadowmapNeeded()const;

	void SetDefineValue(int defineID, bool defineValue);
	void BindDefines();

	bool Load(std::wstring path, bool isVendorSpecific);
	bool Load(std::string& vxShader, std::string& frShader);
	void Reload();

	unsigned int GetAttribLocation(const char* name);

	// set vector for shader
	int SetUniformVector(const char* name, const CVector4D& value);
	void SetUniformVector(int location, const CVector4D& value);
	
	// set 3D vector for shader
	int SetUniformVector3D(const char* name, const CVector3D& value);
	void SetUniformVector3D(int location, const CVector3D& value);	

	// set 2D vector for shader
	int SetUniformVector2D(const char* name, const CVector2D& value);
	void SetUniformVector2D(int location, const CVector2D& value);	

	// set texture for shader
	int SetUniformTexture(const char *name, int texUnit);
	void SetUniformTexture(int location, int texUnit);

	// set float value for shader
	int SetUniformFloat(const char * name, float value);
	void SetUniformFloat(int location, float value);

	// set matrix for shader
	int SetUniformMatrix(const char* name, float* value);
	void SetUniformMatrix(int location, float* value);

	int SetUniformPredefined(std::string name, int valueID);
	void SetUniformPredefined(int location, int valueID);

	int GetVariableUpdateType(std::string& name);

	bool Bind(int lastUsedByMaterialID = -1);
	void Unbind();

	void AddPredefined(int location, int valueID);

	__forceinline void OnLightMatrixChanged()
	{
		m_isLightMatrixChanged = true;
	}

	int GetDefaultsCount()const;
	void GetDefaultVar(int num, std::string& name, float& value);

	void SetShadowsExistStatus(bool areShadowsExist);

	void SaveBinaries();

	void DumpUniformsDEBUG();

	static std::wstring GetLoadingShaderFileName();

private:
	bool LinkProgram(GLhandleARB program);
	bool CompileShader(GLhandleARB shader);
	void Init();

	std::string GetCacheFileName(std::wstring& path, std::wstring& vxFileName, std::wstring& fsFileName, std::wstring& vxFileName2, std::wstring& fsFileName2, int slot);
	bool LoadPrecompiled(std::wstring path, std::wstring vxFileName, std::wstring fsFileName, std::wstring vxFileName2, std::wstring fsFileName2, int slot);
	void SavePrecompiled(std::wstring path, std::wstring vxFileName, std::wstring fsFileName, std::wstring vxFileName2, std::wstring fsFileName2, int slot);

	std::string GetInfoLog(GLhandleARB shader);

	bool OnLoadingPrecompiledShaderFailed(unsigned char* data);

	bool LoadShaderSource(GLhandleARB shader, std::wstring& fileName);
	void ReadStringFromCryptFile(FILE* fl, char* buf, int& length);
	std::string LoadTextFile(std::wstring& fileName);

	__int64 GetModifyTime(std::wstring& path);
	__int64 GetModifyTime(std::wstring& path, std::wstring& path2);
	unsigned int GetModifyTime(std::wstring& vPath, std::wstring& vPath2, std::wstring& fPath, std::wstring& fPath2);

	GLhandleARB m_vertexShader[DEFINES_COUNT_2ST];
	GLhandleARB m_fragmentShader[DEFINES_COUNT_2ST];
	GLhandleARB m_programObject[DEFINES_COUNT_2ST];

	unsigned int m_definesCount;
	unsigned int m_definesCount2st;
	int m_slot;
	int m_lightMatrixLocation;
	int m_lightMatrixLocation2;
	int m_shadowsExistLocation;
	bool m_isDefinesChanged;
	bool m_isVendorSpecific;

	bool m_defines[DEFINES_COUNT];
	int m_definesMap[DEFINES_COUNT];

	TUniformMap	m_uniformMapBySlot[DEFINES_COUNT_2ST];

	int m_lastMaterialID;
	
	bool m_isCompiled;
	bool m_isShadowmapNeeded;
	bool m_isLightMatrixChanged;
	bool m_isAlphaTestDisableOnATI;
	
	MP_WSTRING m_path;
	MP_WSTRING m_vxFileName, m_vxFileName2;
	MP_WSTRING m_frFileName, m_frFileName2;

	MP_MAP<MP_STRING, float> m_defaults;

	MP_MAP<MP_STRING, int> m_varsUpdateType;

	int m_physicalLocationsByLogical[MAX_SHADER_VARIABLES_COUNT][DEFINES_COUNT_2ST];
	int m_physicalLocationsCount;

	float m_valuesByLogicalLocation[MAX_SHADER_VARIABLES_COUNT][DEFINES_COUNT_2ST];
	MP_MAP<MP_STRING, int> m_locationByName;
};
