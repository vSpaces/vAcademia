
#pragma once

#include "CommonEngineHeader.h"

#include "BaseChangesInformer.h"
#include "BaseBufferObject.h"
#include "CommonObject.h"
#include "VBOSubObject.h"
#include "Renderer.h"
#include "IModel.h"
#include <vector>

#include "plane3d.h"
#include "quaternion3d.h"

#include "IPhysics.h"
#include "IAsynchResource.h"
#include "IAsynchResourcehandler.h"

#define MAX_PARTS	64

#define MODEL_NOT_LOADED	0
#define MODEL_LOADING		1
#define MODEL_LOADED		2
#define MODEL_EMPTY			3

#define MODEL_LOAD_ERROR_FILE_NOT_FOUND		1
#define MODEL_LOAD_ERROR_FILE_EMPTY			2
#define MODEL_LOAD_ERROR_FILE_DAMAGED		3
#define MODEL_LOAD_ERROR_MATERIALS_DAMAGED	4
#define MODEL_LOAD_ERROR_DECOMPRESS_FAILED	5
#define MODEL_LOAD_ERROR_TASK_FAILED		6
#define MODEL_LOAD_ERROR_CACHE_DAMAGED		7
#define MODEL_LOAD_ERROR_PARSE_FAILED		8

typedef struct _SPrimitiveDescription
{
	int type;

	CVector3D translation;
	CQuaternion rotation;
	CVector3D scale;

	float lengthX;
	float lengthY;
	float lengthZ;
	
	_SPrimitiveDescription()
	{
		translation.Set(0, 0, 0);
		rotation.Set(0, 0, 0, 1);
		scale.Set(0, 0, 0);

		type = 0;

		lengthX = 0;
		lengthY = 0;
		lengthZ = 0;		
	}

	_SPrimitiveDescription(const _SPrimitiveDescription& prim)
	{
		type = prim.type;
		lengthX = prim.lengthX;
		lengthY = prim.lengthY;
		lengthZ = prim.lengthZ;

		translation = prim.translation;
		rotation = prim.rotation;
		scale = prim.scale;
	}

} SPrimitiveDescription;

typedef struct _SMaterialParams
{
	int textureOffset;
	int materialID;
	int textureID;
	int twoSidedMode;
	bool isMaterialSuited[MODE_INDEX_COUNT];
	bool isBillBoardOnCPU;
	bool isTextureTraceable;

	_SMaterialParams()
	{
		isTextureTraceable = false;
		isBillBoardOnCPU = false;

		for (int i = 0; i < MODE_INDEX_COUNT; i++)
		{
			isMaterialSuited[i] = false;
		}

		twoSidedMode = 0;
		materialID = -1;
		textureOffset = -1;
		textureID = -1;
	}
} SMaterialParams;

class CModel : public CCommonObject, public IModel, public IPhysics, public IAsynchResourceHandler, public CBaseChangesInformer, public IChangesListener
{
public:
	CModel();
	~CModel();

	unsigned int GetLoadingState();
	void SetLoadingState(const unsigned char loadingState, const unsigned char errorCode);

	void SetSourceSize(unsigned int sourceSize);

	void SetUsage(int usage);
	int GetUsage()const;

	void CloneMaterials();

	void GetMaterialBounds(unsigned int materialID, float& minX, float& minY, float& minZ, 
								  float& maxX, float& maxY, float& maxZ);

	unsigned char GetRenderMode()const;
	void SetRenderMode(unsigned char mode);

	void SetUpdateType(unsigned char updateType);
	unsigned char GetUpdateType()const;

	void AddPrimitive(char type,
		float transX, float transY, float transZ,
		float rotX, float rotY, float rotZ, float rotW,
		float scaleX, float scaleY, float scaleZ,
		float lengthX, float lengthY, float lengthZ);

	void SetPostfix(std::string postfix);
	std::string GetPostfix()const;

	unsigned int GetPrimitivesCount()const;
	SPrimitiveDescription* GetPrimitive(unsigned int index)const;

	void AddTexture(int offset, std::string name);
	std::string GetTextureName(unsigned int num)const;
	void SetTextureNumber(unsigned int num, unsigned int textureID);
	int GetTextureCount()const;
	int GetTextureNumber(unsigned int num)const;
	int GetTextureOffset(unsigned int num)const;

	void SetModelMinX(float value);
	void SetModelMaxX(float value);
	void SetModelMinY(float value);
	void SetModelMaxY(float value);
	void SetModelMinZ(float value);
	void SetModelMaxZ(float value);

	float GetModelMinX()const;
	float GetModelMaxX()const;
	float GetModelMinY()const;
	float GetModelMaxY()const;
	float GetModelMinZ()const;
	float GetModelMaxZ()const;

	float GetModelLengthX()const;
	float GetModelLengthY()const;
	float GetModelLengthZ()const;

	void SetTextureOffset(unsigned int num, int value);

	void SetTwoSided(int materialID, bool isTwoSided);

	void SetVertexCount(int count);
	void SetFaceCount(int count);

	int GetVertexCount()const;
	int GetFaceCount()const;

	bool GetCullFaceSetting()const;
	unsigned char GetRColor()const;
	unsigned char GetGColor()const;
	unsigned char GetBColor()const;
	unsigned char GetAlignX()const;
	unsigned char GetAlignY()const;
	unsigned char GetAlignZ()const;

	void SetRColor(unsigned char color);
	void SetGColor(unsigned char color);
	void SetBColor(unsigned char color);
	void SetCullFaceSetting(bool cullFace);
	void SetAlignX(unsigned char align);
	void SetAlignY(unsigned char align);
	void SetAlignZ(unsigned char align);

	void ScaleGeometry(float scaleX, float scaleY, float scaleZ);
	void ReCountBoundingBox();

	void AddMaterial(int materialID);
	void AddMaterial(std::string materialName);

	void AddMaterialSafety(std::string materialName);
	void FinalizeMaterials();

	int GetMaterial(unsigned int id);
	void ChangeMaterial(int id, std::string materialName);
	void ChangeMaterial(int id, int materialID);

	int GetTextureOrderNumber(int textureID);
	void ChangeTexture(int orderID, int textureID);

	bool Lock();	
	void Unlock();

	void RotateMeshY90();
	void RotateMeshX90();
	void RotateMeshZ90();

	void RelinkMaterials();
	
	char* GetModelName();

	__forceinline int GetFaceOrient()const
	{
		return m_faceOrient;
	}
	void SetFaceOrient(int faceOrient);

	float* GetVertexPointer();
	float* GetNormalPointer();
	float* GetTextureCoordsPointer(int level);
	unsigned short* GetIndexPointer();

	void FreeResources();

	void OnChanged(int eventID);

	void ComputeVertexTangents(int v0, int v1, int v2);
	void ComputeTangents();

	void SetTraceableTexture(unsigned int num, bool isTraceable);

	bool IsOk()const;

	void SetLoadPriority(unsigned char loadPriority);
	__forceinline unsigned char GetLoadPriority()const
	{
		return m_loadPriority;
	}

	__forceinline bool IsTextureCompressionPermitted()const
	{
		return m_isTextureCompressionPermitted;
	}
	void PermitTextureCompression(bool isPermitted);

	__forceinline unsigned char GetLoadingErrorCode()const
	{
		return m_errorCode;
	}

	void CheckMaterials();

	float* vx;
	float* vxOld;
	float* vn;
	float* tx[8];
	float* tns;

	unsigned short* fs;

	CBaseBufferObject* m_vbo;
	CVBOSubObject* m_vbos[MAX_PARTS];
	SMaterialParams m_params[MAX_PARTS];
	unsigned int m_textureCount;

//	int m_dls[MAX_PARTS];

	bool m_noVBO;

public:
	void OnAsynchResourceLoadedPersent(IAsynchResource* asynch, unsigned char percent);
	void OnAsynchResourceLoaded(IAsynchResource* asynch);
	void OnAsynchResourceError(IAsynchResource* asynch);

	void GetTextureCoordsForPoint(CVector3D translation, CQuaternion rotation, CVector3D scale, float x, float y, float z, float& _tx, float& _ty, int& textureX, int& textureY);
	bool GetTraceablePlanePoints(CVector3D translation, CQuaternion rotation, CVector3D scale,
									 CVector3D* v);

private:
	int GetMaterialID(std::string& materialName);

	float m_modelsMaxCoords[6];

	int m_vertexCount;
	int m_faceCount;

	unsigned int m_sourceSize;

	unsigned char m_settings[7];
	unsigned char m_loadPriority;
	unsigned char m_loadingState;
	unsigned char m_renderMode;
	unsigned char m_updateType;
	unsigned char m_errorCode;

	bool m_isOk;
	bool m_isTextureCompressionPermitted;

	int m_faceOrient;
	
	MP_VECTOR<SPrimitiveDescription> m_primitives;

	MP_VECTOR<MP_STRING> m_materials;
	MP_VECTOR<MP_STRING> m_textureInfo;
	MP_STRING m_postfix;

	CRITICAL_SECTION m_cs;
};
