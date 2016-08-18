
#include "StdAfx.h"
#include "Shader.h"
#include "XMLPropertiesList.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "AddNCacheDataTask.h"
#include "cal3d/memory_leak.h"
#include "FileFunctions.h"

/*loc = glGetUniformLocationARB(m_programObject[j], name);*/ 

#define GETLOGICALLOCATION(name, logicalLocation) int logicalLocation = -1; \
	MP_MAP<MP_STRING, int>::iterator it = m_locationByName.find(MAKE_MP_STRING(name)); \
	if (it != m_locationByName.end()) \
	{ \
		logicalLocation = (*it).second; \
	} \
	else \
	{ \
		int locations[DEFINES_COUNT_2ST]; \
		int loc; \
		bool isFound = false; \
		for (unsigned int j = 0; j < m_definesCount2st; j++) \
		{ \
			TUniformMap::iterator it = m_uniformMapBySlot[j].find(MAKE_MP_STRING(name)); \
			if (it == m_uniformMapBySlot[j].end()) \
			{ \
				loc = -1; \
			} \
			else \
			{ \
				loc = (*it).second; \
				isFound = true; \
			} \
			locations[j] = loc; \
		} \
		if (isFound) \
		{ \
			logicalLocation = m_physicalLocationsCount; \
			for (unsigned int k = 0; k < m_definesCount2st; k++) \
			{ \
				m_physicalLocationsByLogical[m_physicalLocationsCount][k] = locations[k]; \
			} \
			m_physicalLocationsCount++; \
			m_locationByName.insert(std::pair<MP_STRING, int>(MAKE_MP_STRING(name), logicalLocation)); \
		} \
	} \

bool noshadows = false;

bool IsVendorDefine(std::string& name)
{
	if ((name == "vendor_ati") || (name == "vendor_nvidia") || (name == "vendor_intel") ||
		 (name == "vendor_sis") || (name == "vendor_s3"))
	{
		return true;
	}
	
	return false;
}

std::string GetVendorDefine()
{
	switch (g->gi.GetVendorID())
	{
	case VENDOR_ATI:
		return "#define vendor_ati\n";

	case VENDOR_NVIDIA:
		return "#define vendor_nvidia\n";

	case VENDOR_INTEL:
		return "#define vendor_intel\n";

	case VENDOR_S3:
		return "#define vendor_s3\n";

	case VENDOR_SIS:
		return "#define vendor_sis\n";
	}

	return "#define vendor_unknown\n";
}

int GetDefineID(std::string& name)
{
	// если количество дефайнов станет большим,
	// переписать на std::map
	if (name == "shadowmap")
	{
		return 0;
	}

	return -1;
}

std::string GetDefineStr(int defineID)
{
	if (0 == defineID)
	{
		return "#define shadowmap\n";
	}

	return "";
}

CShader::CShader():
	MP_WSTRING_INIT(m_path),
	MP_WSTRING_INIT(m_vxFileName),
	MP_WSTRING_INIT(m_vxFileName2),
	MP_WSTRING_INIT(m_frFileName),
	MP_WSTRING_INIT(m_frFileName2),
	MP_MAP_INIT(m_defaults),
	MP_MAP_INIT(m_varsUpdateType),
	MP_MAP_INIT(m_locationByName)
{
	Init();

	for (int i = 0; i < DEFINES_COUNT_2ST; i++)
	{
		MP_MAP_DC_INIT(m_uniformMapBySlot[i]);
	}
}

void CShader::SetAlphaTestDisableOnATI(bool isDisabled)
{
	if (g->gi.GetVendorID() == VENDOR_ATI)
	{
		m_isAlphaTestDisableOnATI = isDisabled;
	}
}

void CShader::Init()
{
	m_isVendorSpecific = false;
	m_isLightMatrixChanged = true;
	m_physicalLocationsCount = 0;
	m_isShadowmapNeeded = false;
	m_shadowsExistLocation = -1;
	m_lightMatrixLocation2 = -1;
	m_lightMatrixLocation = -1;
	m_isDefinesChanged = true;
	m_isAlphaTestDisableOnATI = false;
	m_isCompiled = false;
	m_lastMaterialID = -1;
	m_definesCount2st = 1;
	m_definesCount = 0;
	m_slot = 0;

	memset(&m_defines[0], 0, sizeof(int) * DEFINES_COUNT);
	memset(&m_vertexShader[0], 0, sizeof(GLhandleARB) * DEFINES_COUNT);
	memset(&m_fragmentShader[0], 0, sizeof(GLhandleARB) * DEFINES_COUNT);
	memset(&m_programObject[0], 0, sizeof(GLhandleARB) * DEFINES_COUNT);

	int k;
	for (k = 0; k < DEFINES_COUNT; k++)
	{
		m_definesMap[k] = -1;
	}

	for (k = 0; k < DEFINES_COUNT_2ST; k++)
	{
		m_uniformMapBySlot[k].clear();
	}

	for (int x = 0; x < MAX_SHADER_VARIABLES_COUNT; x++)
	for (int y = 0; y < DEFINES_COUNT_2ST; y++)
	{
		m_physicalLocationsByLogical[x][y] = -1;
		m_valuesByLogicalLocation[x][y] = 0.0f;
	}

	m_defaults.clear();
	m_varsUpdateType.clear();
	m_locationByName.clear();
	m_physicalLocationsCount = 0;
}

void CShader::Reload()
{
	Init();
	Load(m_path, m_isVendorSpecific);
}

bool CShader::Load(std::string& vxShader, std::string& frShader)
{
	bool res = true;

	m_vertexShader[0] = GLFUNCR(glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB));
	m_fragmentShader[0] = GLFUNCR(glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB));
	
	const char* text = vxShader.c_str();
	GLFUNC(glShaderSourceARB)(m_vertexShader[0], 1, &text, NULL);
	const char* text2 = frShader.c_str();
	GLFUNC(glShaderSourceARB)(m_fragmentShader[0], 1, &text2, NULL);

	if (!CompileShader(m_vertexShader[0]))
	{
		res = false;
	}
	
	if (!CompileShader(m_fragmentShader[0]))
	{
		res = false;
	}
	
	m_programObject[0] = GLFUNCR(glCreateProgramObjectARB());
	GLFUNC(glAttachObjectARB)(m_programObject[0], m_vertexShader[0]);
	GLFUNC(glAttachObjectARB)(m_programObject[0], m_fragmentShader[0]);

	if (!LinkProgram(m_programObject[0]))
	{
		res = false;
	}

	return res;
}

void CShader::AddPredefined(int /*location*/, int /*valueID*/)
{
	
}

unsigned int CShader::GetAttribLocation(const char* name)
{
	return glGetAttribLocation(m_programObject[m_slot], name);
}

GLhandleARB CShader::GetProgramObject()const
{
	return m_programObject[m_slot];
}

void CShader::SetDefineValue(int defineID, bool defineValue)
{
	if (m_definesMap[defineID] != -1)
	{
		m_isDefinesChanged = true;
		m_defines[m_definesMap[defineID]] = defineValue;
	}
}

void CShader::BindDefines()
{
	if (m_isDefinesChanged)
	{
		int st2 = 1;
		m_slot = 0;

		for (unsigned int k = 0; k < m_definesCount; k++)
		{
			m_slot += (m_defines[k] ? 1 : 0) * st2;
			st2 *= 2;
		}

		m_isDefinesChanged = false;
		m_lastMaterialID = -1;
	}
}

bool CShader::IsShadowmapNeeded()const
{
	if (m_definesMap[SHADER_DEFINE_SHADOWMAP] != -1)
	{
		return m_defines[m_definesMap[SHADER_DEFINE_SHADOWMAP]];
	}

	return false;
}

bool CShader::Bind(int lastUsedByMaterialID)
{
	if (!m_isCompiled)
	{
		return false;
	}

	bool res = (m_lastMaterialID != lastUsedByMaterialID);
	m_lastMaterialID = lastUsedByMaterialID;
	
	GLFUNC(glUseProgramObjectARB)(m_programObject[m_slot]);

#ifdef SHADOWMAPS
	if (m_isLightMatrixChanged)
	if (IsShadowmapNeeded())
	if (g->ne.GetWorkMode() != WORK_MODE_SHADOWMAPRENDER)
	{
		m_isLightMatrixChanged = false;
		g->matm.BindDepthTexture(7);
		SetUniformMatrix(m_lightMatrixLocation2, &g->sm.m_lightMatrix2[0]);
		SetUniformMatrix(m_lightMatrixLocation, &g->sm.m_lightMatrix[0]);
	}
#endif

	return res;
}

void CShader::Unbind()
{
	if (!m_isCompiled)
	{
		return;
	}

	GLFUNC(glUseProgramObjectARB)(0);
}

int CShader::GetVariableUpdateType(std::string& name)
{
	MP_MAP<MP_STRING, int>::iterator it = m_varsUpdateType.find(MAKE_MP_STRING(name));
	if (it != m_varsUpdateType.end())
	{
		return (*it).second;
	}
	else
	{
		return SHADER_VAR_UPDATE_TYPE_ON_CHANGE;
	}
}

__int64 CShader::GetModifyTime(std::wstring& path)
{
	struct _stat buf;
	int res = _wstat(path.c_str(), &buf);
	__int64 time = 0;
	if (0 == res)
	{
		time = buf.st_mtime;
	}
	return time;
}

__int64 CShader::GetModifyTime(std::wstring& path, std::wstring& path2)
{
	__int64 res = GetModifyTime(path);
	if (res == 0)
	{
		res = GetModifyTime(path2);
	}

	return res;
}

unsigned int CShader::GetModifyTime(std::wstring& vPath, std::wstring& vPath2,
							   std::wstring& fPath, std::wstring& fPath2)
{
	unsigned int res = (unsigned int)GetModifyTime(vPath, vPath2) + (unsigned int)GetModifyTime(fPath, fPath2);
	return res;
}

bool CShader::OnLoadingPrecompiledShaderFailed(unsigned char* data)
{
	g->dc.DeleteByMask("shader");
	if (data)
	{
		MP_DELETE_ARR(data);
	}
	return false;
}

std::wstring CShader::GetLoadingShaderFileName()
{
	return GetApplicationDataDirectory() + L"\\Vacademia\\shader_loading";
}

bool CShader::LoadPrecompiled(std::wstring path, std::wstring vxFileName, std::wstring fsFileName, std::wstring vxFileName2, std::wstring fsFileName2, int slot)
{
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	if (path.find(L"water_small") != -1)
	{
		return false;
	}

	if (!g->ei.IsExtensionSupported(GLEXT_arb_get_program_binary))
	{
		return false;
	}

	std::string cacheFileName = GetCacheFileName(path, vxFileName, fsFileName, vxFileName2, fsFileName2, slot);

	if (!g->dc.CheckIfDataExists(cacheFileName))
	{
		return false;
	}

	unsigned char* data = NULL;
	int size = 0;
	g->dc.GetData((void **)&data, &size, cacheFileName);

	if ((!data) || (0 == size))
	{
		return false;
	}
	
	typedef void * (WINAPI * PFNGLPROGRAMBINARYPROC) (GLuint, GLenum, const GLvoid *, GLsizei);

	static PFNGLPROGRAMBINARYPROC glProgramBinary = 0;
	if (0 == glProgramBinary)
	{
		glProgramBinary = (PFNGLPROGRAMBINARYPROC)wglGetProcAddress("glProgramBinary");
	}

	int defineCount = *(((unsigned int*)data) + 3);
	if (defineCount != DEFINES_COUNT)
	{
		return false;
	}

	int addSize = DEFINES_COUNT * 4 + 8 + 8;
	int checkSize = *(((unsigned int*)data) + 0);
	int checkCRC = *(((unsigned int*)data) + 1);
	int len = size - addSize;

	if (checkSize != len)
	{
		g->lw.WriteLn("Failed to load precompiled shader from path ", path, " (wrong size)");
		return false;
	}

	int crc = 0;
	unsigned char* __data = (unsigned char*)data + addSize;
	for (int i = 0; i < len - 4; i++)
	{
		unsigned int* _data = (unsigned int*)(__data + i);
		crc += *_data;
	}

	if (crc != checkCRC)
	{
		g->lw.WriteLn("Failed to load precompiled shader from path ", path, " (wrong crc)");
		return false;
	}

	m_definesCount = 0;
	for (int i = 0; i < DEFINES_COUNT; i++)
	{
		m_definesMap[i] = *(((unsigned int*)data) + 4 + i);
		if (m_definesMap[i] != -1)
		{
			m_defines[m_definesCount] = false;
			m_definesCount++;
		}
	}

	m_definesCount2st = Get2st((char)m_definesCount);

	m_programObject[slot] = GLFUNCR(glCreateProgramObjectARB());
	int format = *(((unsigned int*)data) + 2);
	GLint success;

	std::wstring fileName = GetLoadingShaderFileName();
	FILE* fl = FileOpen(fileName.c_str(), L"w");
	if (!fl)
	{
		return OnLoadingPrecompiledShaderFailed(data);
	}
	
	try
	{
		GLFUNC(glProgramBinary)(m_programObject[slot], format, data + addSize, size - addSize);
		GLFUNC(glGetProgramiv)(m_programObject[slot], GL_LINK_STATUS, &success);
	}
	catch (...)
	{
		fclose(fl);
		_wremove(fileName.c_str());
		return OnLoadingPrecompiledShaderFailed(data);
	}

	fclose(fl);
	_wremove(fileName.c_str());

	MP_DELETE_ARR(data);

	if (success > 0)
	{
		m_isCompiled = true;
	}

    return (success > 0);
}

std::string CShader::GetCacheFileName(std::wstring& path, std::wstring& vxFileName, std::wstring& fsFileName, std::wstring& vxFileName2, std::wstring& fsFileName2, int slot)
{
	std::string cacheFileName = "shader_";

	std::wstring s = StringToLower(path);
	USES_CONVERSION;
	cacheFileName += W2A(path.c_str());
	StringReplace(cacheFileName, "\\", "_");
	StringReplace(cacheFileName, "/", "_");
	StringReplace(cacheFileName, ":", "_");
	cacheFileName += "_";
	cacheFileName += IntToStr(slot);
	cacheFileName += "_";
	cacheFileName += IntToStr(GetModifyTime(vxFileName, vxFileName2, fsFileName, fsFileName2));
	cacheFileName += "__";

	return cacheFileName;
}

void CShader::SavePrecompiled(std::wstring path, std::wstring vxFileName, std::wstring fsFileName, std::wstring vxFileName2, std::wstring fsFileName2, int slot)
{
	if (!g->ei.IsExtensionSupported(GLEXT_arb_get_program_binary))
	{
		return;
	}	

	std::string cacheFileName = GetCacheFileName(path, vxFileName, fsFileName, vxFileName2, fsFileName2, slot);
	if (g->dc.CheckIfDataExists(cacheFileName))
	{
		void* data;
		int size;
		if (g->dc.GetData(&data, &size, cacheFileName))
		{
			return;
		}

		MP_DELETE_ARR(data);
	}	

	typedef void * (WINAPI * PFNGLGETPROGRAMBINARYPROC) (unsigned int, size_t, size_t *, GLenum *, void *);
	static PFNGLGETPROGRAMBINARYPROC glGetProgramBinary = 0;
	if (0 == glGetProgramBinary)
	{
		glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)wglGetProcAddress("glGetProgramBinary");
	}

	int addSize = DEFINES_COUNT * 4 + 8 + 8;

	#define GL_PROGRAM_BINARY_LENGTH 0x8741
	GLint len=0;
	GLenum binaryFormat;
	GLFUNC(glGetProgramiv)(m_programObject[slot], GL_PROGRAM_BINARY_LENGTH, &len);
	unsigned char* binary = MP_NEW_ARR(unsigned char, len * 2 + addSize + 10);
	GLFUNC(glGetProgramBinary)(m_programObject[slot], len, NULL, &binaryFormat, binary + addSize);

	int crc = 0;
	unsigned char* data = (unsigned char*)binary + addSize;
	for (int i = 0; i < len - 4; i++)
	{
		unsigned int* _data = (unsigned int *)(data + i);
		crc += *_data;
	}

	int glErr = GLFUNC(glGetError)();
	if (glErr != 0)
	{
		g->lw.WriteLn("SavePrecompiled OpenGL error: ", glErr);
		return;
	}	

	*(unsigned int*)binary = len;
	*(((unsigned int*)binary) + 1) = crc;
	*(((unsigned int*)binary) + 2) = binaryFormat;
	*(((unsigned int*)binary) + 3) = DEFINES_COUNT;
	for (int i = 0; i < DEFINES_COUNT; i++)
	{
		*(((unsigned int*)binary) + 4 + i) = m_definesMap[i];
	}

	g->dc.AddDataAsynch(binary, len + addSize, cacheFileName);
	
	MP_DELETE_ARR(binary);
}

bool CShader::Load(std::wstring path, bool isVendorSpecific)
{
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	if (path.find(L"water_small") != -1)
	{
		return false;
	}

	bool res = true;

	CXMLPropertiesList defsList(path + L"_defaults.xml", SILENCE_IF_NOT_FOUND);
	std::vector<std::string> defaults = defsList.GetAllProperties();

	CXMLPropertiesList varsList(path + L".xml", ERROR_IF_NOT_FOUND);
	std::vector<std::string> vars = varsList.GetAllProperties();

	std::vector<std::string>::iterator it = vars.begin();
	std::vector<std::string>::iterator itEnd = vars.end();

	for ( ; it != itEnd; it++)
	{
		std::string strValue = varsList.GetString(*it);

		int value = 0;

		if ("update_once" == strValue)
		{
			value = SHADER_VAR_UPDATE_TYPE_ONCE;
		}
		else if ("update_once_at_frame" == strValue)
		{
			value = SHADER_VAR_UPDATE_TYPE_ONCE_AT_FRAME;
		}
		else if ("update_on_change" == strValue)
		{
			value = SHADER_VAR_UPDATE_TYPE_ON_CHANGE;
		}

		m_varsUpdateType.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(*it), value));
	}

	m_path = path;
	m_isVendorSpecific = isVendorSpecific;
	m_vxFileName = path + L".vsp";
	m_frFileName = path + L".fsp";
	m_vxFileName2 = path + L".vsp";
	m_frFileName2 = path + L".fsp";

	if (isVendorSpecific)
	{
		switch (g->gi.GetVendorID())
		{
		case VENDOR_NVIDIA:
			m_vxFileName = path + L"_nvidia.vsp";
			m_frFileName = path + L"_nvidia.fsp";
			break;

		case VENDOR_ATI:
			m_vxFileName = path + L"_ati.vsp";
			m_frFileName = path + L"_ati.fsp";
			break;

		case VENDOR_INTEL:
			m_vxFileName = path + L"_intel.vsp";
			m_frFileName = path + L"_intel.fsp";
			break;

		case VENDOR_S3:
			m_vxFileName = path + L"_s3.vsp";
			m_frFileName = path + L"_s3.fsp";
			break;

		case VENDOR_SIS:
			m_vxFileName = path + L"_sis.vsp";
			m_frFileName = path + L"_sis.fsp";
			break;

		case VENDOR_OTHER:
			m_vxFileName = path + L"_other.vsp";
			m_frFileName = path + L"_other.fsp";
			break;
		}
	}

	unsigned int slot = 0;
	while (slot < Get2st((char)m_definesCount))
	{
		for (unsigned int k = 0; k < m_definesCount; k++)
		{
#pragma warning(push)
#pragma warning(disable : 4554)
			m_defines[k] = ((slot >> k) & 1 == 1);
#pragma warning(pop)
		}

		m_isCompiled = false;
		if (!LoadPrecompiled(path, m_vxFileName, m_frFileName, m_vxFileName2, m_frFileName2, slot))		
		{
			m_vertexShader[slot] = GLFUNCR(glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB));
			m_fragmentShader[slot] = GLFUNCR(glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB));
	
			if (!LoadShaderSource(m_vertexShader[slot], m_vxFileName))
			{
				if (!LoadShaderSource(m_vertexShader[slot], m_vxFileName2))
				{
					res = false;
				}
			}

			if (!LoadShaderSource(m_fragmentShader[slot], m_frFileName))
			{
				if (!LoadShaderSource(m_fragmentShader[slot], m_frFileName2))
				{
					res = false;
				}	
			}

			if (!CompileShader(m_vertexShader[slot]))
			{
				res = false;
			}
	
			if (!CompileShader(m_fragmentShader[slot]))
			{
				res = false;
			}
	
			m_programObject[slot] = GLFUNCR(glCreateProgramObjectARB());
			GLFUNC(glAttachObjectARB)(m_programObject[slot], m_vertexShader[slot]);
			GLFUNC(glAttachObjectARB)(m_programObject[slot], m_fragmentShader[slot]);

			if (!LinkProgram(m_programObject[slot]))
			{
				res = false;
			}

			m_isCompiled = res;
		}

		if (!m_isCompiled)
		{
			return false;
		}

		GLint uniformCount;
		GLFUNC(glGetObjectParameterivARB)(m_programObject[slot], GL_OBJECT_ACTIVE_UNIFORMS_ARB, &uniformCount);

		char _uniformName[1000];
		for (int j = 0; j < uniformCount; j++)
        {
			GLsizei size;
			GLenum type;
			GLint len;
            GLFUNC(glGetActiveUniformARB)(m_programObject[slot], j, 1000, &len, &size, &type,
                                  _uniformName);

			std::string uniformName = _uniformName;

			int location = GLFUNCR(glGetUniformLocationARB(m_programObject[slot], uniformName.c_str()));

			m_uniformMapBySlot[slot].insert(TUniformMap::value_type(MAKE_MP_STRING(uniformName), location));
		}

		slot++;
	}

	for (unsigned int k = 0; k < m_definesCount; k++)
	{
		m_defines[k] = false;
	}

	GLFUNC(glUseProgramObjectARB)(m_programObject[m_slot]);
	if (m_definesMap[SHADER_DEFINE_SHADOWMAP] != -1)
	{
		SetUniformFloat("shadowMapSize", (float)g->rs.GetInt(SHADOWMAP_SIZE));
		SetUniformTexture("shadowMap", 7);
		SetUniformTexture("shadowMap2", 6);
		m_lightMatrixLocation = SetUniformMatrix("lightMatrix", &g->sm.m_lightMatrix[0]);
		m_lightMatrixLocation2 = SetUniformMatrix("lightMatrix2", &g->sm.m_lightMatrix2[0]);
	}

	m_shadowsExistLocation = SetUniformFloat("shadowsExist", g->rs.GetBool(SHADOWS_ENABLED) ? 1.0f : 0.0f);

	it = defaults.begin();
	itEnd = defaults.end();

	for ( ; it != itEnd; it++)
	{
		std::string strValue = defsList.GetString(*it);
		float value = (float)rtl_atoi(strValue.c_str());

		SetUniformFloat((*it).c_str(), value);

		m_defaults.insert(MP_MAP<MP_STRING, float>::value_type(MAKE_MP_STRING(*it), value));
	}

	GLFUNC(glUseProgramObjectARB)(0);
	g->sm.RefreshShaderOptimization();

	return res;
}

void CShader::DumpUniformsDEBUG()
{
	GLint uniformCount;
	GLFUNC(glGetObjectParameterivARB)(m_programObject[m_slot], GL_OBJECT_ACTIVE_UNIFORMS_ARB, &uniformCount);

    char _uniformName[1000];
	for (int j = 0; j < uniformCount; j++)
	{
		GLsizei size;
		GLenum type;
		GLFUNC(glGetActiveUniformARB)(m_programObject[m_slot], j, 1000, NULL, &size, &type, _uniformName);
		std::string uniformName = _uniformName;
		int location = GLFUNCR(glGetUniformLocationARB(m_programObject[m_slot], uniformName.c_str()));
		GLfloat a[16];
		GLFUNC(glGetUniformfv)(m_programObject[m_slot], location, &a[0]);
	}
}

std::string CShader::LoadTextFile(std::wstring& fileName)
{
	FILE* file = FileOpen(fileName, L"rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		int length = ftell(file);
		fseek(file, 0, SEEK_SET);

		char* buf = MP_NEW_ARR(char, length + 1 + 8192); // прибавляем 8192, чтобы хватило на инклуды

		ReadStringFromCryptFile(file, buf, length);
		fclose(file);
		std::string shaderSource(buf, length);;

		MP_DELETE_ARR(buf);

#pragma warning(push)
#pragma warning(disable : 4172)
		return shaderSource;
#pragma warning(pop)
	}

	std::string emptyStr;
#pragma warning(push)
#pragma warning(disable : 4172)
	return emptyStr;
#pragma warning(pop)
}

bool CShader::LoadShaderSource(GLhandleARB shader, std::wstring& fileName)
{
	g->lw.Write(L"Loading shader ", fileName);

	std::string shaderSrc = LoadTextFile(fileName);

	if (shaderSrc.size() > 0)
	{
		shaderSrc = GetVendorDefine() + shaderSrc;

		for (int defineID = 0; defineID < DEFINES_COUNT; defineID++)
		if (m_definesMap[defineID] != -1)
		if (m_defines[m_definesMap[defineID]])
		{
			shaderSrc = GetDefineStr(defineID) + shaderSrc;
		}
	}

//	g->lw.WriteLn("shaderSrc = ", shaderSrc);

	if (!shaderSrc.empty())
	{
		const char* shaderstring = shaderSrc.c_str();
		const char** pshaderstring = &shaderstring;

		GLFUNC(glShaderSourceARB)(shader, 1, pshaderstring, NULL);

		g->lw.WriteLn(" - ok.");

		return true;
	}

	g->lw.WriteLn(" - failed.");

	return false;
}

std::string CShader::GetInfoLog(GLhandleARB shader)
{
	GLcharARB buffer[MAX_LOG_INFO_SIZE];
	GLsizei length;

	GLFUNC(glGetInfoLogARB)(shader, sizeof(buffer) - 1, &length, buffer);

#pragma warning(push)
#pragma warning(disable : 4172)
#pragma warning(disable : 4239)
	return std::string(buffer, length);
#pragma warning(pop)
}

bool CShader::CompileShader(GLhandleARB shader)
{
	GLint compileStatus;

	g->lw.Write("Compile shader", ltRenderManager);

	GLFUNC(glCompileShaderARB)(shader);
	GLFUNC(glGetObjectParameterivARB)(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);

	bool result = (compileStatus == GL_TRUE);

	if (!result)
	{
		g->lw.WriteLn("");
		g->lw.WriteLn(GetInfoLog(shader));
	}
	else
	{
		g->lw.PrintLn(" - ok.");
	}

	return result;
}

bool CShader::LinkProgram(GLhandleARB program)
{
	GLint linkStatus;
	if (g->ei.IsExtensionSupported(GLEXT_arb_get_program_binary))
	{
		typedef void * (WINAPI * PFNGLPROGRAMPARAMETERIPROC) (GLuint, GLenum, GLint);

		static PFNGLPROGRAMPARAMETERIPROC glProgramParameteri = 0;
		if (0 == glProgramParameteri)
		{	
			glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)wglGetProcAddress("glProgramParameteri");
		}

		#define	GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
		GLFUNC(glProgramParameteri)(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	}
	GLFUNC(glLinkProgramARB)(program);

	g->lw.Write("Linking program");

	GLFUNC(glGetObjectParameterivARB)(program, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);

	if (!linkStatus)
	{
		g->lw.WriteLn("");
		g->lw.WriteLn(GetInfoLog(program));
	}
	else
	{
		g->lw.PrintLn(" - ok.");
	}

	return linkStatus == GL_TRUE;
}

void CShader::ReadStringFromCryptFile(FILE* file, char* buf, int& length)
{
	unsigned char p, newP = 0;	
	int i = 0;
	int destI = -1;
	int k = 0;

	std::string newString;

	while (i < length)
	{
		i++;
		destI++;
		k++;
		fread(&p, 1, 1, file);

		newP = (unsigned char)((p + 255 - (8 + k))%255);

		buf[destI] = newP;
				
		if (newP == 10)
		{
			if (newString.size() > 6)
			{
				if (newString.substr(0, 6) == "#ifdef")
				{
					int pos1 = newString.find_first_of(" ");
					std::string defineName = newString.substr(pos1 + 1, newString.size() - pos1 - 1);

					if (!IsVendorDefine(defineName))
					{
						int defineID = GetDefineID(defineName);

						if (defineID != -1)
						{				
							if (m_definesMap[defineID] == -1)
							{
								m_definesMap[defineID] = m_definesCount;
								m_defines[m_definesCount] = false;
								m_definesCount++;
								m_definesCount2st = Get2st((char)m_definesCount);
							}
						}
					}
				}
			}

			if (newString.size() > 8)
			{
				if (newString.substr(0, 8) == "include[")
				{
					int pos1 = newString.find_first_of("]");
					USES_CONVERSION;
					std::wstring fileName = A2W(newString.substr(8, pos1 - 8).c_str());
					std::wstring fullPath = GetApplicationRootDirectory();
					fullPath += fileName;
					std::string include = LoadTextFile(fullPath);

					int difference = include.size() - newString.size();
					assert(difference >= 0);

					destI -= newString.size();

					for (unsigned int l = 0; l < include.size(); l++)
					{
						buf[destI] = include[l];
						destI++;
					}

					length += (difference + 1);
					buf[destI - 1] = 10;
					destI--;
				}
			}

			k = 0;
			newString = "";
		}
		else
		{
			char tmp[2];
			tmp[0] = buf[destI];
			tmp[1] = 0;
			newString += tmp;
		}
	}

	buf[i] = 0;
}

void CShader::SetUniformVector(int location, const CVector4D& value)
{
	if (location != -1)
	{
		int oglLocation = m_physicalLocationsByLogical[location][m_slot];
		if (oglLocation != -1)
		{
			GLFUNC(glUniform4fvARB)(oglLocation, 1, value);
		}
	}
}

void CShader::SetUniformVector3D(int location, const CVector3D& value)
{
	if (location != -1)
	{
		int oglLocation = m_physicalLocationsByLogical[location][m_slot];
		if (oglLocation != -1)
		{
			GLFUNC(glUniform3fvARB)(oglLocation, 1, value);
		}
	}
}

void CShader::SetUniformVector2D(int location, const CVector2D& value)
{
	if (location != -1)
	{
		int oglLocation = m_physicalLocationsByLogical[location][m_slot];
		if (oglLocation != -1)
		{
			GLFUNC(glUniform2fvARB)(oglLocation, 1, value);
		}
	}
}

void CShader::SetUniformTexture(int location, int texUnit)
{
	if (location != -1)
	{
		int oglLocation = m_physicalLocationsByLogical[location][m_slot];
		if (oglLocation != -1)
		{
			GLFUNC(glUniform1iARB)(oglLocation, texUnit);
		}
	}
}

void CShader::SetUniformFloat(int location, float value)
{
	if (location != -1)
	{
		int oglLocation = m_physicalLocationsByLogical[location][m_slot];
		if (oglLocation != -1)
		{
			GLFUNC(glUniform1fARB)(oglLocation, value);
		}
	}
}

void CShader::SetUniformMatrix(int location, float* value)
{
	if (location != -1)
	{
		int oglLocation = m_physicalLocationsByLogical[location][m_slot];
		if (oglLocation != -1)
		{
				GLFUNC(glUniformMatrix4fvARB)(oglLocation, 1, GL_FALSE, value);
		}
	}
}

int CShader::SetUniformVector(const char* name, const CVector4D& value)
{
	GETLOGICALLOCATION(name, logicalLocation);

	if (logicalLocation != -1)
	for (unsigned int k = 0; k < m_definesCount2st; k++)
	if (m_physicalLocationsByLogical[logicalLocation][k] != -1)
	{
		GLFUNC(glUseProgramObjectARB)(m_programObject[k]);
		GLFUNC(glUniform4fvARB)(m_physicalLocationsByLogical[logicalLocation][k], 1, value);
	}

	GLFUNC(glUseProgramObjectARB)(m_programObject[m_slot]);
	g->sm.RefreshShaderOptimization();

	return logicalLocation;
}

int CShader::SetUniformVector3D(const char* name, const CVector3D& value)
{
	GETLOGICALLOCATION(name, logicalLocation);

	if (logicalLocation != -1)
	for (unsigned int k = 0; k < m_definesCount2st; k++)
	if (m_physicalLocationsByLogical[logicalLocation][k] != -1)
	{
		GLFUNC(glUseProgramObjectARB)(m_programObject[k]);
		GLFUNC(glUniform3fvARB)(m_physicalLocationsByLogical[logicalLocation][k], 1, value);
	}

	GLFUNC(glUseProgramObjectARB)(m_programObject[m_slot]);

	return logicalLocation;
}

int CShader::SetUniformVector2D(const char* name, const CVector2D& value)
{
	GETLOGICALLOCATION(name, logicalLocation);

	if (logicalLocation != -1)
	for (unsigned int k = 0; k < m_definesCount2st; k++)
	if (m_physicalLocationsByLogical[logicalLocation][k] != -1)
	{
		GLFUNC(glUseProgramObjectARB)(m_programObject[k]);
		GLFUNC(glUniform2fvARB)(m_physicalLocationsByLogical[logicalLocation][k], 1, value);
	}

	GLFUNC(glUseProgramObjectARB)(m_programObject[m_slot]);

	return logicalLocation;
}

int CShader::SetUniformTexture(const char* name, int texUnit)
{
	GETLOGICALLOCATION(name, logicalLocation);

	if (logicalLocation != -1)
	for (unsigned int k = 0; k < m_definesCount2st; k++)
	if (m_physicalLocationsByLogical[logicalLocation][k] != -1)
	{
		GLFUNC(glUseProgramObjectARB)(m_programObject[k]);
		GLFUNC(glUniform1iARB)(m_physicalLocationsByLogical[logicalLocation][k], texUnit);
	}

	GLFUNC(glUseProgramObjectARB)(m_programObject[m_slot]);
	g->sm.RefreshShaderOptimization();

	return logicalLocation;
}

int CShader::SetUniformFloat(const char* name, float value)
{
	GETLOGICALLOCATION(name, logicalLocation);

	if (logicalLocation != -1)
	for (unsigned int k = 0; k < m_definesCount2st; k++)
	if (m_physicalLocationsByLogical[logicalLocation][k] != -1)
	{
		GLFUNC(glUseProgramObjectARB)(m_programObject[k]);
		GLFUNC(glUniform1fARB)(m_physicalLocationsByLogical[logicalLocation][k], value);
	}

	GLFUNC(glUseProgramObjectARB)(m_programObject[m_slot]);
	g->sm.RefreshShaderOptimization();

	return logicalLocation;
}

int CShader::SetUniformMatrix(const char* name, float* value)
{
	GETLOGICALLOCATION(name, logicalLocation);

	if (logicalLocation != -1)
	for (unsigned int k = 0; k < m_definesCount2st; k++)
	if (m_physicalLocationsByLogical[logicalLocation][k] != -1)
	{
		GLFUNC(glUseProgramObjectARB)(m_programObject[k]);
		GLFUNC(glUniformMatrix4fvARB)(m_physicalLocationsByLogical[logicalLocation][k], 1, GL_FALSE, value);
	}

	GLFUNC(glUseProgramObjectARB)(m_programObject[m_slot]);
	g->sm.RefreshShaderOptimization();

	return logicalLocation;
}

int CShader::SetUniformPredefined(std::string name, int valueID)
{
	switch (valueID)	
	{	
	case SHADER_PREDEFINED_PARAM_TIME:
		{
			return SetUniformFloat(name.c_str(), g_shaderTime);
		}
		break;

	case SHADER_PREDEFINED_PARAM_CAMERAPOS:
		{
			float x, y, z;
			g->cm.GetCameraEye(&x, &y, &z);
			return SetUniformVector(name.c_str(), CVector4D(x, y, z, 1.0f));
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTCOLOR:
		{
			return SetUniformVector(name.c_str(), g->dtc.GetLightColor());
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
			return SetUniformVector(name.c_str(), CVector4D(x, y, z, 1.0f));
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
			return SetUniformVector(name.c_str(), CVector4D(x, y, z, 1.0f));
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTNESS:
		{
			float lightness = 1.0f;

			if (g->rs.GetBool(TIME_OF_DAY_ENABLED))
			{
				lightness = (float)g->dtc.GetColorIntensivityHDR() / 255.0f;
			}

			return SetUniformFloat(name.c_str(), lightness);
		}
		break;
	}

	return -1;
}

void CShader::SetUniformPredefined(int location, int valueID)
{
	switch (valueID)	
	{	
	case SHADER_PREDEFINED_PARAM_TIME:
		{
			SetUniformFloat(location, g_shaderTime);
		}
		break;

	case SHADER_PREDEFINED_PARAM_CAMERAPOS:
		{
			float x, y, z;
			g->cm.GetCameraEye(&x, &y, &z);
			/*if (m_valuesByLogicalLocation[location][m_slot] != x + y + z)
			{*/
				SetUniformVector(location, CVector4D(x, y, z, 1.0f));
			/*	m_valuesByLogicalLocation[location][m_slot] = x + y + z;
			}*/
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTCOLOR:
		{
			SetUniformVector(location, g->dtc.GetLightColor());
		}
		break;

	case SHADER_PREDEFINED_PARAM_LIGHTPOS:
		{
			//if (m_valuesByLogicalLocation[location][m_slot] != 1.0f)
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
				//m_valuesByLogicalLocation[location][m_slot] = 1.0f;
			}
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

int CShader::GetDefaultsCount()const
{
	return m_defaults.size();
}

void CShader::GetDefaultVar(int num, std::string& name, float& value)
{
	MP_MAP<MP_STRING, float>::iterator it = m_defaults.begin();

	for (int i = 0; i < num; i++)
	{
		it++;
	}

	name = (*it).first;
	value = (*it).second;
}

void CShader::SetShadowsExistStatus(bool areShadowsExist)
{
	if (m_shadowsExistLocation != -1)
	{
		int oldSlot = m_slot;

		for (unsigned int slotID = 0; slotID < m_definesCount2st; slotID++)
		{
			m_slot = slotID;
			GLFUNC(glUseProgramObjectARB)(m_programObject[slotID]);
			SetUniformFloat(m_shadowsExistLocation, areShadowsExist ? 1.0f : 0.0f);
		}

		GLFUNC(glUseProgramObjectARB)(0);
		g->sm.RefreshShaderOptimization();

		m_slot = oldSlot;
	}
}

void CShader::SaveBinaries()
{
	if (m_isCompiled)
	{
		for (unsigned int slotID = 0; slotID < m_definesCount2st; slotID++)
		{
			SavePrecompiled(m_path, m_vxFileName, m_frFileName, m_vxFileName2, m_frFileName2, slotID);
		}
	}
	else
	{
		g->lw.WriteLn("shader is not compiled");
	}
}

CShader::~CShader()
{
	m_locationByName.clear();
}