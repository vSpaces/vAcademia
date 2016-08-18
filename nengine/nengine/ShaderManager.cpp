
#include "stdafx.h"
#include "XMLList.h"
#include "NEngine.h"
#include "ShaderManager.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"
#include "FileFunctions.h"

float g_shaderTime;

CShaderManager::CShaderManager():
	MP_MAP_INIT(m_predefinedParamList),
	MP_MAP_INIT(m_paramTypeList),
	m_isShaderingEnabled(true),
	m_isDefinesChanged(true),
	m_lastShadowmapSize(0),
	m_lastShader(NULL),
	m_isInit(false)	
{
	m_predefinedParamList.insert(PredefinedParamList::value_type(SHADER_PREDEFINED_PARAM_TIME_STR, SHADER_PREDEFINED_PARAM_TIME));
	m_predefinedParamList.insert(PredefinedParamList::value_type(SHADER_PREDEFINED_PARAM_CAMERAPOS_STR, SHADER_PREDEFINED_PARAM_CAMERAPOS));
	m_predefinedParamList.insert(PredefinedParamList::value_type(SHADER_PREDEFINED_PARAM_LIGHTPOS_STR, SHADER_PREDEFINED_PARAM_LIGHTPOS));
	m_predefinedParamList.insert(PredefinedParamList::value_type(SHADER_PREDEFINED_PARAM_MOVELIGHTPOS_STR, SHADER_PREDEFINED_PARAM_MOVELIGHTPOS));
	m_predefinedParamList.insert(PredefinedParamList::value_type(SHADER_PREDEFINED_PARAM_LIGHTNESS_STR, SHADER_PREDEFINED_PARAM_LIGHTNESS));
	m_predefinedParamList.insert(PredefinedParamList::value_type(SHADER_PREDEFINED_PARAM_LIGHTCOLOR_STR, SHADER_PREDEFINED_PARAM_LIGHTCOLOR));
	m_predefinedParamList.insert(PredefinedParamList::value_type(SHADER_PREDEFINED_PARAM_TANGENT_STR, SHADER_PREDEFINED_PARAM_TANGENT));

	m_paramTypeList.insert(ParamTypeList::value_type(SHADER_PARAM_FLOAT_STR, SHADER_PARAM_FLOAT));
	m_paramTypeList.insert(ParamTypeList::value_type(SHADER_PARAM_INT_STR, SHADER_PARAM_INT));
	m_paramTypeList.insert(ParamTypeList::value_type(SHADER_PARAM_VECTOR_STR, SHADER_PARAM_VECTOR));
	m_paramTypeList.insert(ParamTypeList::value_type(SHADER_PARAM_TEXTURE_STR, SHADER_PARAM_TEXTURE));

	ReserveMemory(50);
}

void CShaderManager::UnbindShader()
{
	if (!m_isShaderingEnabled)
	{
		return;
	}

	m_lastShader = NULL;
	GLFUNC(glUseProgramObjectARB)(0);
}

int CShaderManager::LoadShaderFromMemory(std::string name, std::string& vxShader, std::string& frShader)
{
	int shaderID = GetObjectNumber(name);
	CShader* shader = GetObjectPointer(shaderID);
	if (shader->Load(vxShader, frShader))
	{
		return shaderID;
	}
	else
	{
		g->sm.DeleteObject(shaderID);
		return -1;
	}
}

int CShaderManager::LoadShaderFromMemory(std::wstring name, std::string& vxShader, std::string& frShader)
{
	int shaderID = GetObjectNumber(name);
	CShader* shader = GetObjectPointer(shaderID);
	if (shader->Load(vxShader, frShader))
	{
		return shaderID;
	}
	else
	{
		g->sm.DeleteObject(shaderID);
		return -1;
	}
}

void CShaderManager::SetDefineValue(int defineID, bool value)
{
	m_defines[defineID] = value;
	m_isDefinesChanged = true;
}

void CShaderManager::BindDefines()
{
	if (m_isDefinesChanged)
	{
		std::vector<CShader *>::iterator it = GetLiveObjectsBegin();
		std::vector<CShader *>::iterator itEnd = GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
			for (int defineID = 0; defineID < DEFINES_COUNT; defineID++)
			{
				(*it)->SetDefineValue(defineID, m_defines[defineID]);
			}

			(*it)->BindDefines();
		}
	}

	m_isDefinesChanged = false;

	if (m_lastShadowmapSize != g->rs.GetInt(SHADOWMAP_SIZE))
	{
		std::vector<CShader *>::iterator it = GetLiveObjectsBegin();
		std::vector<CShader *>::iterator itEnd = GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		if ((*it)->IsShadowmapNeeded())
		{
			(*it)->SetUniformFloat("shadowMapSize", (float)g->rs.GetInt(SHADOWMAP_SIZE));
		}
	}
}

void CShaderManager::SetLightMatrix(float lightMatrix[16], float lightMatrix2[16])
{
	if ((memcmp(m_lightMatrix, lightMatrix, 16 * sizeof(float)) != 0)
		|| (memcmp(m_lightMatrix2, lightMatrix2, 16 * sizeof(float)) != 0))
	{
		std::vector<CShader *>::iterator it = GetLiveObjectsBegin();
		std::vector<CShader *>::iterator itEnd = GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
			(*it)->OnLightMatrixChanged();
		}
	}

	memcpy(m_lightMatrix, lightMatrix, 16 * sizeof(float));
	memcpy(m_lightMatrix2, lightMatrix2, 16 * sizeof(float));
}

int CShaderManager::GetObjectNumber(std::string name)
{
	int res = CManager<CShader>::GetObjectNumber(name);

	if (res == ERR_OBJECT_NOT_FOUND)
	{
		res = AddObject(name);
	}

	return res;
}

int CShaderManager::GetObjectNumber(std::wstring name)
{
	int res = CManager<CShader>::GetObjectNumber(name);

	if (res == ERR_OBJECT_NOT_FOUND)
	{
		res = AddObject(name);
	}

	return res;
}

void CShaderManager::OnNewFrame(bool adjustTime)
{
	for (int i = 0; i < GetCount(); i++)
	{
		CShader* shader = GetObjectPointer(i);
		shader->SetLastUsedByMaterialID(-1);
	}

	if (adjustTime)
	{
		int time = (int)g->ne.time;
		time = time%120000;
		g_shaderTime = (float)time / 1000.0f;
	}
}

void CShaderManager::ReloadShaders()
{
	g->lw.WriteLn("Reloading shaders...");
	g->matm.ClearShaderLocations();

	std::vector<CShader *>::iterator it = GetLiveObjectsBegin();
	std::vector<CShader *>::iterator itEnd = GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		(*it)->Reload();
	}

	OnAfterChanges();
	g->lw.WriteLn("Reload shaders - ok.");
}

bool CShaderManager::InitShaders()
{
	m_isInit = true;

	if ((!GLEW_ARB_shader_objects) ||
		(!GLEW_ARB_vertex_shader) ||
		(!GLEW_ARB_fragment_shader))
	{
		return false;
	}

	bool res = true;

	std::vector<std::string> addFields;
	addFields.push_back("vendorspecific");
	addFields.push_back("alphatest_disable_on_ati");
	CXMLList shadersList(GetApplicationRootDirectory() + L"shaders.xml", addFields, true);

	FILE* fl = FileOpen(CShader::GetLoadingShaderFileName(), L"r");
	if (fl)
	{
		fclose(fl);
		g->dc.DeleteByMask("shader");
	}

	g->lw.WriteLn("Loading shaders...");

	std::string path, vendorSpecific, alphaTestDisableOnATI;
	bool isSuccess = true;

	stat_shaderCompileTime = (int)g->tp.GetTickCount();

	while (isSuccess)
	{
		isSuccess = shadersList.GetNextValue(path);

		vendorSpecific = "";
		shadersList.GetOptionalFieldValue("vendorspecific", vendorSpecific);
		bool isVendorSpecific = (vendorSpecific == "true");
		alphaTestDisableOnATI = "";
		shadersList.GetOptionalFieldValue("alphatest_disable_on_ati", alphaTestDisableOnATI);
		bool isAlphaTestDisableOnATI = (alphaTestDisableOnATI == "true");

		if (isSuccess)
		{
			int shaderID = GetObjectNumber(path);
			CShader* shader = GetObjectPointer(shaderID);
			USES_CONVERSION;
			res = shader->Load(GetApplicationRootDirectory() + A2W( path.c_str()), isVendorSpecific);
			shader->SetAlphaTestDisableOnATI(isAlphaTestDisableOnATI);
		}
	}

	stat_shaderCompileTime = (int)g->tp.GetTickCount() - stat_shaderCompileTime;

	m_lastShadowmapSize = g->rs.GetInt(SHADOWMAP_SIZE);

	g->sm.SavePrecompiledShaders();

	return res;
}

void CShaderManager::SavePrecompiledShaders()
{
	g->lw.WriteLn("SavePrecompiledShaders");

	std::vector<CShader *>::iterator it = GetLiveObjectsBegin();
	std::vector<CShader *>::iterator itEnd = GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		(*it)->SaveBinaries();
	}
}

void CShaderManager::DeleteShaders()
{
	if ((!GLEW_ARB_shader_objects) ||
		(!GLEW_ARB_vertex_shader) ||
		(!GLEW_ARB_fragment_shader))
	{
		return;
	}

	for (int i = 0; i < GetCount(); i++)
	{
		CShader* tmp = GetObjectPointer(i);

		GLFUNC(glDeleteObjectARB)(tmp->GetVertexShader());
		GLFUNC(glDeleteObjectARB)(tmp->GetFragmentShader());
		GLFUNC(glDeleteObjectARB)(tmp->GetProgramObject());
	}
}

bool CShaderManager::Init()
{
	if (!InitShaders())
	{
		return false;
	}

	return true;
}

void CShaderManager::EnableShadering()
{
	m_isShaderingEnabled = true;
}

void CShaderManager::DisableShadering()
{
	m_isShaderingEnabled = false;
}

bool CShaderManager::BindShader(int shaderID, int lastUsedByMaterialID)
{
	if (!m_isShaderingEnabled)
	{
		return false;
	}

	CShader* shader = GetObjectPointer(shaderID);

	bool res = (shader->GetLastUsedByMaterialID() != lastUsedByMaterialID);
	shader->SetLastUsedByMaterialID(lastUsedByMaterialID);

	GLFUNC(glUseProgramObjectARB)(shader->GetProgramObject());

#ifdef SHADOWMAPS
	if (g->rs.GetBool(SHADERS_USING))
	if (shader->IsShadowmapNeeded())
	{
		SetUniformFloat(shaderID, "shadowMapSize", (float)g->rs.GetInt(SHADOWMAP_SIZE));
		SetUniformTexture(shaderID, "shadowMap", 7);
		SetUniformMatrix(shaderID, "lightMatrix", &m_lightMatrix[0]);
	}
#endif

	return res;
//	assert(glGetError() == GL_NO_ERROR);
}

int CShaderManager::SetUniformVector(int i, const char* name, const CVector4D& value)
{
	if (!m_isShaderingEnabled)
	{
		return false;
	}

	CShader* shader = GetObjectPointer(i);

    int loc = GLFUNCR(glGetUniformLocationARB(shader->GetProgramObject(), name));
    if (loc < 0)
	{
        return -1;
	}

    GLFUNC(glUniform4fvARB)(loc, 1, value);

	//assert(glGetError() != GL_INVALID_OPERATION);

    return loc;
}


void CShaderManager::SetUniformVector(int location, const CVector4D& value)
{
	if (!m_isShaderingEnabled)
	{
		return;
	}

    GLFUNC(glUniform4fvARB)(location, 1, value);
}

int CShaderManager::SetUniformTexture(int i, const char* name, int texUnit)
{
	if (!m_isShaderingEnabled)
	{
		return false;
	}

	CShader* shader = GetObjectPointer(i);

    int loc = GLFUNCR(glGetUniformLocationARB(shader->GetProgramObject(), name));
    if (loc < 0)
	{
        return -1;
	}

    GLFUNC(glUniform1iARB)(loc, texUnit);

    return loc;
}

void CShaderManager::SetUniformTexture(int location, int texUnit)
{
	if (!m_isShaderingEnabled)
	{
		return;
	}

	GLFUNC(glUniform1iARB)(location, texUnit);
}

int CShaderManager::SetUniformFloat(int i, const char* name, float value)
{
	if (!m_isShaderingEnabled)
	{
		return false;
	}

	CShader* shader = GetObjectPointer(i);

    int loc = GLFUNCR(glGetUniformLocationARB(shader->GetProgramObject(), name));

    if (loc < 0)
	{
        return -1;
	}

    GLFUNC(glUniform1fARB)(loc, value);

    return loc;
}

void CShaderManager::SetUniformFloat(int location, float value)
{
	if (!m_isShaderingEnabled)
	{
		return;
	}

    GLFUNC(glUniform1fARB)(location, value);
}

int CShaderManager::SetUniformMatrix(int i, const char* name, float* value)
{
	if (!m_isShaderingEnabled)
	{
		return false;
	}

	CShader* shader = GetObjectPointer(i);

    int loc = GLFUNCR(glGetUniformLocationARB(shader->GetProgramObject(), name));

    if (loc < 0)
	{
        return -1;
	}

    GLFUNC(glUniformMatrix4fvARB)(loc, 1, GL_FALSE, value);

    return loc;
}

void CShaderManager::SetUniformMatrix(int location, float* value)
{
	if (!m_isShaderingEnabled)
	{
		return;
	}

    GLFUNC(glUniformMatrix4fvARB)(location, 1, GL_FALSE, value);
}

int CShaderManager::GetPredefinedParamID(std::string param)
{
	PredefinedParamList::iterator iter = m_predefinedParamList.find(MAKE_MP_STRING(param));

	if (iter != m_predefinedParamList.end())
	{
		return (*iter).second;
	}
	else
	{
		return -1;
	}
}

int CShaderManager::GetParamTypeID(std::string paramType)
{
	ParamTypeList::iterator iter = m_paramTypeList.find(MAKE_MP_STRING(paramType));

	if (iter != m_paramTypeList.end())
	{
		return (*iter).second;
	}
	else
	{
		return -1;
	}
}

int CShaderManager::SetUniformPredefined(int shaderID, std::string name, int valueID)
{
	switch (valueID)	
	{	
	case SHADER_PREDEFINED_PARAM_TIME:
		{
			return SetUniformFloat(shaderID, name.c_str(), g_shaderTime);
		}
		break;

	case SHADER_PREDEFINED_PARAM_CAMERAPOS:
		{
			float x, y, z;
			g->cm.GetCameraEye(&x, &y, &z);
			return SetUniformVector(shaderID, name.c_str(), CVector4D(x, y, z, 1.0f));
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTCOLOR:
		{
			return SetUniformVector(shaderID, name.c_str(), g->dtc.GetLightColor());
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTPOS:
		{
			float x = 0.0f, y = 0.0f, z = 0.0f;
			//g->cm.GetCameraEye(&x, &y, &z);
			if (g->rs.GetBool(VERTICAL_AXE_Z))
			{
				z += 1000.0f;

				if (!g->rs.GetBool(TIME_OF_DAY_ENABLED))
				{
					x += 3.0f;
					y += 3.0f;
				}
				else
				{
					x += 1000.0f * cosf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
					y += 1000.0f * sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
				}
			}
			else
			{
				if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
				{
					x += 1000.0f * cosf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
					z += 1000.0f * sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
				}
				else
				{
					x += 3.0f;
					z += 3.0f;
				}

				y += 1000.0f;
			}
			return SetUniformVector(shaderID, name.c_str(), CVector4D(x, y, z, 1.0f));
		}
		break;


	case SHADER_PREDEFINED_PARAM_MOVELIGHTPOS:
		{
			float x = 0.0f, y = 0.0f, z = 0.0f;
			g->cm.GetCameraEye(&x, &y, &z);
			if (g->rs.GetBool(VERTICAL_AXE_Z))
			{
				z += 1000.0f;

				if (!g->rs.GetBool(TIME_OF_DAY_ENABLED))
				{
					x += 3.0f;
					y += 3.0f;
				}
				else
				{
					x += 1000.0f * cosf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
					y += 1000.0f * sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
				}
			}
			else
			{
				if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
				{
					x += 1000.0f * cosf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
					z += 1000.0f * sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
				}
				else
				{
					x += 3.0f;
					z += 3.0f;
				}

				y += 1000.0f;
			}
			return SetUniformVector(shaderID, name.c_str(), CVector4D(x, y, z, 1.0f));
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTNESS:
		{
			float lightness = 1.0f;

			if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
			{
				lightness = (float)g->dtc.GetColorIntensivityHDR() / 255.0f;
			}
			
			return SetUniformFloat(shaderID, name.c_str(), lightness);
		}
		break;
	}

	return -1;
}

void CShaderManager::SetUniformPredefined(int location, int valueID)
{
	switch (valueID)	
	{	
	case SHADER_PREDEFINED_PARAM_TIME:
		{
			SetUniformFloat(location, g_shaderTime);

			g->lw.WriteLn("shader time", g_shaderTime);
		}
		break;

	case SHADER_PREDEFINED_PARAM_CAMERAPOS:
		{
			float x, y, z;
			g->cm.GetCameraEye(&x, &y, &z);
			SetUniformVector(location, CVector4D(x, y, z, 1.0f));
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTCOLOR:
		{
			SetUniformVector(location, g->dtc.GetLightColor());
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTPOS:
		{
			float x = 0.0f, y = 0.0f, z = 0.0f;
			//g->cm.GetCameraEye(&x, &y, &z);
			if (g->rs.GetBool(VERTICAL_AXE_Z))
			{
				z += 1000.0f;

				if (!g->rs.GetBool(TIME_OF_DAY_ENABLED))
				{
					x += 3.0f;
					y += 3.0f;
				}
				else
				{
					x += 1000.0f * cosf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
					y += 1000.0f * sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
				}
			}
			else
			{
				if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
				{
					x += 1000.0f * cosf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
					z += 1000.0f * sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
				}
				else
				{
					x += 3.0f;
					z += 3.0f;
				}

				y += 1000.0f;
			}
			SetUniformVector(location, CVector4D(x, y, z, 1.0f));
		}
		break;


	case SHADER_PREDEFINED_PARAM_MOVELIGHTPOS:
		{
			float x = 0.0f, y = 0.0f, z = 0.0f;
			g->cm.GetCameraEye(&x, &y, &z);
			if (g->rs.GetBool(VERTICAL_AXE_Z))
			{
				z += 1000.0f;

				if (!g->rs.GetBool(TIME_OF_DAY_ENABLED))
				{
					x += 3.0f;
					y += 3.0f;
				}
				else
				{
					x += 1000.0f * cosf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
					y += 1000.0f * sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
				}
			}
			else
			{
				if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
				{
					x += 1000.0f * cosf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
					z += 1000.0f * sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f);
				}
				else
				{
					x += 3.0f;
					z += 3.0f;
				}

				y += 1000.0f;
			}
			SetUniformVector(location, CVector4D(x, y, z, 1.0f));
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTNESS:
		{
			float lightness = 1.0f;

			if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
			{
				lightness = (float)g->dtc.GetColorIntensivityHDR() / 255.0f;
			}
			
			SetUniformFloat(location, lightness);
		}
		break;
	}
}

bool CShaderManager::IsShadersInit()
{
	return m_isInit;
}

CShaderManager::~CShaderManager()
{
	DeleteShaders();
}