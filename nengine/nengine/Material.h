
#pragma once

#include "CommonEngineHeader.h"

#include "MaterialStatesEnumerator.h"
#include "BaseChangesInformer.h"
#include "CommonObject.h"
#include "Vector4D.h"
#include "Vector2D.h"
#include "Shader.h"
#include <ifile.h>
#include <vector>
#include <map>
#include "Manager.h"

#pragma  warning (disable: 4786)

#define SOURCE_FILE						1
#define SOURCE_IFILE					2

#define TEXTURE_SLOT_EMPTY				0
#define TEXTURE_SLOT_TEXTURE_2D			1
#define TEXTURE_SLOT_TEXTURE_3D			2
#define TEXTURE_SLOT_TEXTURE_CUBEMAP	3
#define TEXTURE_SLOT_ANIMATION			4

typedef struct _STextureSlot
{
	short slotID;
	short type;
	void* content;

	void CopyFrom(_STextureSlot* slot);

	_STextureSlot()
	{
		type = TEXTURE_SLOT_EMPTY;
		content = NULL;
		slotID = 0;
	}
} STextureSlot;

typedef struct _STexture
{
	int textureID;
	bool isAlphaBlend;

	_STexture()
	{
		textureID = -1;
		isAlphaBlend = false;
	}
} STexture;

typedef struct _SParam 
{
	MP_STRING name;
	int paramType;
	int location;
	unsigned int param[4];

	void CopyFrom(_SParam* param);

	_SParam();
	~_SParam();	
} SParam;

typedef struct _STextureAnimation 
{
	int delta;
	MP_VECTOR<int> textureIDs;

	_STextureAnimation();
} STextureAnimation;

typedef struct _SAsynchTexture
{
	MP_STRING name;
	unsigned char level;
	unsigned char subID;
	unsigned char type;
	bool isPacked;

	_SAsynchTexture();
	_SAsynchTexture(_SAsynchTexture* a);	
} SAsynchTexture;

class CMaterial : public CCommonObject, public CBaseChangesInformer
{
public:
	CMaterial();
	~CMaterial();

	void SetUniqueStatus(bool isUnique);
	bool IsUnique()const;

	void LoadFromXML(const std::string& fileName, unsigned char* const data, bool isManageTextures);
	void LoadFromXML(const std::wstring& fileName, bool isManageTextures);
	void LoadFromXML(ifile* const  file, bool isManageTextures);

	void SetTextureLevel(const int level, const short type, void* const content);
	int GetTextureID(const unsigned int textureLevel);

	void SetShaderID(const int shaderID);
	int GetShaderID();

	void SetShaderParamValue(std::string& name, std::string& type, std::string& value);

	void SetShaderFloatParameter(std::string name, float param);
	void SetShaderIntParameter(std::string name, int param);
	void SetShaderVectorParameter(std::string name, CVector4D vec);
	void SetShaderTextureParameter(std::string name, int textureLevel);
	void SetPredefinedParam(std::string name, int valueID);

	void SetQualityParam(const std::string& name);
	std::string GetQualityParam() const;

	float GetShaderParamFloat(std::string name);

	void BindMaterial();
	void UnbindMaterial();

	unsigned int* GetState(int stateID);
	void AddState(std::string name, std::string value);

	int GetCorrespondingRenderingMode()const;

	void ReInitialize();

	bool IsTwoSidedNeeded()const;
	int GetBillboardOrient()const;
	bool IsTwoSidedTransparencyNeeded()const;
	bool IsTangentsNeeded()const;
	bool IsZeroLevelUsed()const;

	bool IsAlphaBlendExists();

	void SetID(int id);
	int GetID();

	int GetNotFoundTexturesCount()const;
	std::string& GetNotFoundTextureName(unsigned int id);
	int GetNotFoundTextureType(unsigned int id);
	bool GetNotFoundTexturePackedStatus(unsigned int id);
	bool IsNotFoundTextureNormalMap(unsigned int id);
	void UpdateNotFoundTexture(unsigned int id, unsigned int textureID);

	void EnableTextureMatrix(const CVector2D& textureScale);
	void DisableTextureMatrix();
	CVector2D GetTextureMatrixKoefs();

	void Clear();
	void ClearShaderLocations();

	void FreeResources();

	void Clone(CMaterial* material);

	void SetSource(int sourceID, std::string fileName);
	void GetSource(int& sourceID, std::string& fileName);

	void DetectFiltering();
	void OnMaterialQualityChanged(); 

	void SetFiltration(const int minFilter, const int magFilter, const int anisotropy, const float mipmapBias);

	void DumpTexturesDEBUG();

private:
	void Init();
	void LoadFromXML(void* const doc, const std::wstring& fileName, bool isManageTextures);
	void UpdateRenderingMode();
	void OnFirstShaderBind();
	void UpdateUsedTextureSlots();

	void SetShaderParams();
	void SetShaderParamsImpl();
	void SetMaterialStates();

	unsigned int GetMinFilter(const unsigned char minFilterParam, const unsigned char mipmapParam);

	CVector2D m_textureScale;
	CShader* m_shader;

	MP_VECTOR<STextureSlot *> m_usedTextureSlots;

	STextureSlot m_textureSlots[8];	
	MP_MAP<MP_STRING, SParam*> m_shaderParamsMap;
	MP_MAP<MP_STRING, SParam*> m_updateOnceShaderParamsMap;
	MP_VECTOR<SParam*> m_shaderParams;

	MP_STRING m_qualityParam;

	SParam* m_states[STATE_COUNT];

	int m_id;
	int m_shaderID;
	int m_renderMode;
	int m_alphaBlendTextureID;
	int m_normalMapLevelID;

	bool m_isFirstBind;
	bool m_isManageTextures;
	bool m_isQualityChanged;
	bool m_isTextureMatrixChanged;
	bool m_isShaderParamsOptimized;
	bool m_isUnique;
	bool m_isQualityUsed;

	unsigned char m_mipmapParam;
	unsigned char m_minFilterParam;

	unsigned int m_magFilter;
	unsigned int m_minFilter;
	unsigned int m_anisotropy;
	float m_mipmapBias;

	MP_VECTOR<SAsynchTexture *> m_notFoundTextures;

	MP_STRING m_sourcePath;
	int m_sourceID;
		
	unsigned char* m_data;
};