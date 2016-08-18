
#include "StdAfx.h"
#include <Assert.h>
#include <float.h>
#include "Model.h"
#include "CommonMemory.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"
#include "__resource_log.h"
#include "MemoryProfiler.h"

CModel::CModel():
	m_vbo(NULL),
	vx(NULL),
	vxOld(NULL),
	vn(NULL),
	fs(NULL),
	tns(NULL),
	m_isOk(true),	
	m_noVBO(false),
	m_faceCount(0),
	m_errorCode(0),
	m_sourceSize(0),
	m_vertexCount(0),
	m_textureCount(0),	
	m_faceOrient(GL_CW),
	m_loadPriority(PRIORITY_LOW),
	m_renderMode(MODE_RENDER_SOLID),
	m_updateType(UPDATE_TYPE_STATIC),
	m_loadingState(MODEL_EMPTY),
	MP_VECTOR_INIT(m_primitives),
	MP_VECTOR_INIT(m_materials),
	MP_VECTOR_INIT(m_textureInfo),
	MP_STRING_INIT(m_postfix),
	m_isTextureCompressionPermitted(true)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		tx[i] = NULL;
	}

	for (i = 0; i < MAX_PARTS; i++)
	{
		m_vbos[i] = NULL;
//		m_dls[i] = -1;
	}

	m_settings[0] = true;
	m_settings[1] = 255;
	m_settings[2] = 255;
	m_settings[3] = 255;
	m_settings[4] = 0;
	m_settings[5] = 0;
	m_settings[6] = 0;

	for (int i = 0; i < 6; i++)
	{
		m_modelsMaxCoords[i] = 0;
	}

	m_textureInfo.reserve(20);

	InitializeCriticalSection(&m_cs);
}

void CModel::PermitTextureCompression(bool isPermitted)
{
	m_isTextureCompressionPermitted = isPermitted;
}

void CModel::SetSourceSize(const unsigned int sourceSize)
{
	m_sourceSize = sourceSize;
}

void CModel::SetLoadPriority(unsigned char loadPriority)
{
	m_loadPriority = loadPriority;
}

void CModel::SetTwoSided(int materialID, bool isTwoSided)
{
	if (materialID >= MAX_PARTS)
	{
		return;
	}

	m_params[materialID].twoSidedMode = isTwoSided ? 3 : 0;
}

bool CModel::Lock()
{
	EnterCriticalSection(&m_cs);

	if (IsDeleted())
	{
		g->lw.WriteLn("Error lock model ", GetName());
		return false;
	}

	return true;
}

void CModel::Unlock()
{
	LeaveCriticalSection(&m_cs);
}

void CModel::GetMaterialBounds(unsigned int materialID, float& minX, float& minY, float& minZ, 
								  float& maxX, float& maxY, float& maxZ)
{
	if (materialID < m_textureCount)
	{
		int startFaceID = m_params[materialID].textureOffset;
		int endFaceID = m_params[materialID + 1].textureOffset;

		if (-1 == endFaceID)
		{
			endFaceID = m_faceCount;
		}

		minX = 1000000.0f;
		minY = 1000000.0f;
		minZ = 1000000.0f;
		maxX = -minX;
		maxY = -minY;
		maxZ = -minZ;

		for (int index = startFaceID * 3; index < endFaceID * 3; index++)
		{
			int vertexIndex = fs[index];
			float x = vx[vertexIndex * 3];
			float y = vx[vertexIndex * 3 + 1];
			float z = vx[vertexIndex * 3 + 2];
			 
			if (x > maxX)
			{
				maxX = x;
			}

			if (y > maxY)
			{
				maxY = y;
			}

			if (z > maxZ)
			{
				maxZ = z;
			}

			if (x < minX)
			{
				minX = x;
			}

			if (y < minY)
			{
				minY = y;
			}

			if (z < minZ)
			{
				minZ = z;
			}
		}

		minX = 0.0f;
		maxX = sqrtf((maxX - minX) * (maxX - minX) + (maxY - minY) * (maxY - minY));
		minY = 0.0f;
		maxY = 0.0f;
	}
	else
	{
		minX = 0.0f;
		minY = 0.0f;
		minZ = 0.0f;
		maxX = 0.0f;
		maxY = 0.0f;
		maxZ = 0.0f;
	}
}

void CModel::CloneMaterials()
{
	USES_CONVERSION;
	for (unsigned int i = 0; i < m_textureCount; i++)
	if (m_params[i].materialID != -1)
	{
		std::string matName = "material_";
		matName += WCharStringToString(GetName());
		matName += "_";
		matName += IntToStr(i);
		m_params[i].materialID = g->matm.CloneMaterial(m_params[i].materialID, matName);
	}
}

bool CModel::IsOk()const
{
	return m_isOk;
}

void CModel::ComputeVertexTangents(int v0, int v1, int v2)
{
	float du1 = tx[0][v1 * 2] - tx[0][v0 * 2];
	float dv1 = tx[0][v1 * 2 + 1] - tx[0][v0 * 2 + 1];
	float du2 = tx[0][v2 * 2] - tx[0][v0 * 2];
	float dv2 = tx[0][v2 * 2 + 1] - tx[0][v0 * 2 + 1];

	float prod1 = (du1*dv2-dv1*du2);
	float prod2 = (du2*dv1-dv2*du1);
	if ((fabs(prod1) < 0.000001)||(fabs(prod2) < 0.000001)) return;

	float x = dv2/prod1;
	float y = dv1/prod2;

	CVector3D vec1(vx[v1 * 3] - vx[v0 * 3], vx[v1 * 3 + 1] - vx[v0 * 3 + 1], vx[v1 * 3 + 2] - vx[v0 * 3 + 2]);
	CVector3D vec2(vx[v2 * 3] - vx[v0 * 3], vx[v2 * 3 + 1] - vx[v0 * 3 + 1], vx[v2 * 3 + 2] - vx[v0 * 3 + 2]);
	CVector3D tangent = (vec1 * ((float)x)) + (vec2 * ((float)y));

	// Step 2. Orthonormalize the tangent.
	float component = (tangent.Dot(CVector3D(vn[v0 * 3], vn[v0 * 3 + 1], vn[v0 * 3 + 2])));
	tangent -= (CVector3D(vn[v0 * 3], vn[v0 * 3 + 1], vn[v0 * 3 + 2]) * ((float)component));
	tangent.Normalize();

	CVector3D binormal = tangent.Cross(CVector3D(vn[v0 * 3], vn[v0 * 3 + 1], vn[v0 * 3 + 2]));
	if ((tangent.Cross(binormal)).Dot(CVector3D(vn[v0 * 3], vn[v0 * 3 + 1], vn[v0 * 3 + 2])) < 0)
	{
		tns[v0 * 4 + 3] = -1.0f;
	}

	// Step 3: Add the estimated tangent to the overall estimate for the vertex.
	tns[v0 * 4] += tangent.x;
	tns[v0 * 4 + 1] += tangent.y;
	tns[v0 * 4 + 2] += tangent.z;
}


void CModel::ComputeTangents()
{
	if (tns || !g->rs.IsNotOnlyLowQualitySupported())
	{
		return;
	}

	tns = MP_NEW_ARR( float,m_vertexCount * 4);

	int tangentID;
	for (tangentID = 0; tangentID < m_vertexCount; tangentID++)
	{
		tns[tangentID * 4] = 0.0f;
		tns[tangentID * 4 + 1] = 0.0f;
		tns[tangentID * 4 + 2] = 0.0f;
		tns[tangentID * 4 + 3] = 1.0f;
	}

	int faceId;
	for (faceId = 0; faceId < m_faceCount; faceId++)
	{
		ComputeVertexTangents(fs[faceId * 3 + 0], fs[faceId * 3 + 1], fs[faceId * 3 + 2]);
		ComputeVertexTangents(fs[faceId * 3 + 1], fs[faceId * 3 + 2], fs[faceId * 3 + 0]);
		ComputeVertexTangents(fs[faceId * 3 + 2], fs[faceId * 3 + 0], fs[faceId * 3 + 1]);
	}

	for (tangentID = 0; tangentID < m_vertexCount; tangentID++)
	{
		float r = tns[tangentID * 4] * tns[tangentID * 4] + tns[tangentID * 4 + 1] * tns[tangentID * 4 + 1] + tns[tangentID * 4 + 2] * tns[tangentID * 4 + 2];
		if (r != 0.0f)
		{
			r = sqrtf(r);
			tns[tangentID * 4 + 0] /= r;
			tns[tangentID * 4 + 1] /= r;
			tns[tangentID * 4 + 2] /= r;
		}
		else
		{
			tns[tangentID * 4] = 0.0f;
			tns[tangentID * 4 + 1] = 0.0f;
			tns[tangentID * 4 + 2] = 1.0f;
		}
	}

	m_vbo->SetTangentsArray(tns);
}

unsigned char CModel::GetRenderMode()const
{
	return m_renderMode;
}

void CModel::SetRenderMode(unsigned char mode)
{
	m_renderMode = mode;
}

void CModel::SetUpdateType(unsigned char updateType)
{
	m_updateType = updateType;
}

unsigned char CModel::GetUpdateType()const
{
	return m_updateType;
}

void CModel::ReCountBoundingBox()
{
	if (m_vertexCount == 0)
	{
		SetModelMinX(0);
		SetModelMaxX(0);
		SetModelMinY(0);
		SetModelMaxY(0);
		SetModelMinZ(0);
		SetModelMaxZ(0);
		return;
	}

	float max_x, min_x, max_y, min_y, max_z, min_z;
	max_x = -50000000.0f;
	min_x = 50000000.0f;
	max_y = -50000000.0f;
	min_y = 50000000.0f;
	max_z = -50000000.0f;
	min_z = 50000000.0f;

	for (int i = 0; i < m_vertexCount; i++)
	{
		if (vx[i * 3 + 0] > max_x) 
		{
			max_x = vx[i * 3 + 0];
		}
		if (vx[i * 3 + 0] < min_x) 
		{
			min_x = vx[i * 3 + 0];
		}
		if (vx[i * 3 + 1] > max_y)
		{
			max_y = vx[i * 3 + 1];
		}
		if (vx[i * 3 + 1] < min_y) 
		{
			min_y = vx[i * 3 + 1];
		}
		if (vx[i * 3 + 2] > max_z) 
		{
			max_z = vx[i * 3 + 2];
		}
		if (vx[i * 3 + 2] < min_z) 
		{
			min_z = vx[i * 3 + 2];
		}
	}

	SetModelMinX(min_x);
	SetModelMaxX(max_x);
	SetModelMinY(min_y);
	SetModelMaxY(max_y);
	SetModelMinZ(min_z);
	SetModelMaxZ(max_z);
}

void CModel::ScaleGeometry(float scaleX, float scaleY, float scaleZ)
{
	for (int i = 0; i < GetVertexCount(); i++)
	{
		vx[i * 3 + 0] *= scaleX;
		vx[i * 3 + 1] *= scaleY;
		vx[i * 3 + 2] *= scaleZ;
	}
}

bool CModel::GetCullFaceSetting()const
{
	return (m_settings[0] == 1);
}

unsigned char CModel::GetRColor()const
{
	return m_settings[1];
}

unsigned char CModel::GetGColor()const
{
	return m_settings[2];
}

unsigned char CModel::GetBColor()const
{
	return m_settings[3];
}

unsigned char CModel::GetAlignX()const
{
	return m_settings[4];
}

unsigned char CModel::GetAlignY()const
{
	return m_settings[5];
}

unsigned char CModel::GetAlignZ()const
{
	return m_settings[6];
}

void CModel::SetRColor(unsigned char color)
{
	m_settings[1] = color;
}

void CModel::SetGColor(unsigned char color)
{
	m_settings[2] = color;
}

void CModel::SetBColor(unsigned char color)
{
	m_settings[3] = color;
}

void CModel::SetCullFaceSetting(bool cullFace)
{
	m_settings[0] = cullFace;
}

void CModel::SetAlignX(unsigned char align)
{
	m_settings[4] = align;
}

void CModel::SetAlignY(unsigned char align)
{
	m_settings[5] = align;
}

void CModel::SetAlignZ(unsigned char align)
{
	m_settings[6] = align;
}

void CModel::AddTexture(int offset, std::string name)
{
	if (m_textureCount < MAX_PARTS)
	{
		m_params[m_textureCount].textureOffset = offset;
		if (m_params[m_textureCount].twoSidedMode != 3)
		{
			m_params[m_textureCount].twoSidedMode = 0;
		}
		m_textureInfo.push_back(MAKE_MP_STRING(name));

		m_textureCount++;
	}
}

std::string CModel::GetTextureName(unsigned int num)const
{
	if ((num >= 0) && (num < m_textureInfo.size()))
	{
		return m_textureInfo[num];
	}
	else
	{
		return "";
	}
}

void CModel::SetTraceableTexture(unsigned int num, bool isTraceable)
{
	if (num >= m_textureCount)
	{
		return;
	}

	if (!m_params[num].isTextureTraceable)
	{
		m_params[num].isTextureTraceable = isTraceable;
	}	
}

void CModel::SetTextureNumber(unsigned int num, unsigned int textureID)
{
	if (num >= m_textureCount)
	{
		return;
	}

	bool isTextureChanged = (m_params[num].textureID != (int)textureID);
	m_params[num].textureID = textureID;
	//m_params[num].isTextureTraceable = false;

	if (textureID != -1)
	{
		CTexture* texture = g->tm.GetObjectPointer(textureID);
		assert(texture);
		texture->AddChangesListener(this);
		if (isTextureChanged)
		{
			texture->AddRef();
		}
	}

	OnChanged(3);
}

int CModel::GetTextureCount()const
{
	return m_textureCount;
}

/*std::vector<int>* CModel::GetTextureOffsetsVector()
{
	return &m_offsetInfo;
}

std::vector<int>* CModel::GetTextureIDsVector()
{
	return &m_textureNumbers;
}

std::vector<int>* CModel::GetTwoSidedModeVector()
{
	return &m_twoSidedMode;
}

std::vector<int>* CModel::GetMaterialIDsVector()
{
	return &m_materialIDs;
}

std::vector<bool>* CModel::GetMaterialSuitnessVector()
{
	return &m_isMaterialSuitedForRenderingMode;
}

std::vector<bool>* CModel::GetBillboardOnCPUPermissionVector()
{
	return &m_billboardOnCPUPermission;
}*/

int CModel::GetTextureNumber(unsigned int num)const
{
	if ((num >= 0) && (num < m_textureCount))
	{
		return m_params[num].textureID;
	}
	else
	{
		return 0;
	}
}

int CModel::GetTextureOffset(unsigned int num)const
{
	if ((num >= 0) && (num < m_textureCount))
	{
		return m_params[num].textureOffset;
	}
	else
	{
		return 0;
	}
}

void CModel::SetTextureOffset(unsigned int num, int value)
{
	if ((num >= 0) && (num < m_textureCount))
	{
		m_params[num].textureOffset = value;
	}
}

void CModel::SetModelMinX(float value)
{
	m_modelsMaxCoords[0] = value;
}

void CModel::SetModelMaxX(float value)
{
	m_modelsMaxCoords[1] = value;
}

void CModel::SetModelMinY(float value)
{
	m_modelsMaxCoords[2] = value;
}

void CModel::SetModelMaxY(float value)
{
	m_modelsMaxCoords[3] = value;
}

void CModel::SetModelMinZ(float value)
{
	m_modelsMaxCoords[4] = value;
}

void CModel::SetModelMaxZ(float value)
{
	m_modelsMaxCoords[5] = value;
}

float CModel::GetModelMinX()const
{
	return m_modelsMaxCoords[0];
}

float CModel::GetModelMaxX()const
{
	return m_modelsMaxCoords[1];
}

float CModel::GetModelMinY()const
{
	return m_modelsMaxCoords[2];
}

float CModel::GetModelMaxY()const
{
	return m_modelsMaxCoords[3];
}

float CModel::GetModelMinZ()const
{
	return m_modelsMaxCoords[4];
}

float CModel::GetModelMaxZ()const
{
	return m_modelsMaxCoords[5];
}

float CModel::GetModelLengthX()const
{
	return (m_modelsMaxCoords[1] - m_modelsMaxCoords[0]);
}

float CModel::GetModelLengthY()const
{
	return (m_modelsMaxCoords[3] - m_modelsMaxCoords[2]);
}

float CModel::GetModelLengthZ()const
{
	return (m_modelsMaxCoords[5] - m_modelsMaxCoords[4]);
}

void CModel::SetVertexCount(int count)
{
	bool isResizeNeeded = !((count < m_vertexCount) && (vx));

	int oldCount = m_vertexCount;
	m_vertexCount = count;

	if (!isResizeNeeded)
	{
		return;
	}

	float* _vx = MP_NEW_ARR(float, count * 3 + 3);
	float* _vn = MP_NEW_ARR(float, count * 3 + 3);
	float* _tx[8];

	if (vx)
	{
		memcpy(_vx, vx, oldCount * 12);
		memcpy(_vn, vn, oldCount * 12);
		MP_DELETE_ARR(vx);
		MP_DELETE_ARR(vn);

		for (int k = 0; k < 8; k++)
		if (tx[k])
		{
			_tx[k] = MP_NEW_ARR(float, count * 2 + 2);
			memcpy(_tx[k], tx[k], oldCount * 8);
			MP_DELETE_ARR(tx[k]);

			tx[k] = _tx[k];
		}
	}

	if (!tx[0])
	{
		tx[0] = MP_NEW_ARR(float, count * 2 + 2);
	}
		
	vx = _vx;
	vn = _vn;
}

void CModel::SetFaceCount(int count)
{
	bool isResizeNeeded = !((count < m_faceCount) && (fs));

	int oldCount = m_faceCount;
	m_faceCount = count;

	if (!isResizeNeeded)
	{
		return;
	}


	unsigned short* _fs = MP_NEW_ARR(unsigned short,count * 3 + 3);

	if (fs)
	{
		memcpy(_fs, fs, oldCount * 6);
		MP_DELETE_ARR(fs);
	}

	fs = _fs;
}

int CModel::GetVertexCount()const
{
	return m_vertexCount;
}

int CModel::GetFaceCount()const
{
	return m_faceCount;
}

void CModel::RelinkMaterials()
{
	MP_VECTOR<MP_STRING>::iterator it = m_materials.begin();
	MP_VECTOR<MP_STRING>::iterator itEnd = m_materials.end();
	
	for (int i = 0; it != itEnd; it++, i++)
	{
		m_params[i].materialID = GetMaterialID(*it);
		if (m_params[i].materialID != -1)
		{
			CMaterial* material = g->matm.GetObjectPointer(m_params[i].materialID);
			material->AddChangesListener(this);
		}
	}

	OnChanged(3);
}

void CModel::AddMaterial(int materialID)
{
	m_params[m_textureCount - 1].materialID = materialID;
	
	if (materialID != -1)
	{
		CMaterial* material = g->matm.GetObjectPointer(materialID);
		material->AddChangesListener(this);
	}

	OnChanged(3);
}

int CModel::GetMaterialID(std::string& materialName)
{
	USES_CONVERSION;
	std::string tmp = W2A(GetName());
	std::string postfix;
	int pos = tmp.find_last_of("\\");
	if (pos != -1)
	{
		int pos2 = tmp.find_last_of("\\", pos - 1);
		if (pos2 != -1)
		{
			postfix = tmp.substr(pos2 + 1, pos - pos2 - 1);
		}
	}

	int materialID = g->matm.GetObjectNumber(materialName + "_" + postfix + m_postfix);
	if (-1 == materialID)
	{
		materialID = g->matm.GetObjectNumber(materialName);
	}

	return materialID;
}

void CModel::SetPostfix(std::string postfix)
{
	m_postfix = postfix;
}

std::string CModel::GetPostfix()const
{
	return m_postfix;
}

void CModel::AddMaterialSafety(std::string materialName)
{
	m_materials.push_back(MAKE_MP_STRING(materialName));
}

void CModel::FinalizeMaterials()
{
	for (unsigned int i = 0; i < m_materials.size() ; i++)
	{
		int materialID = GetMaterialID(m_materials[i]);

		m_params[i].materialID = materialID; 

		if (materialID != -1)
		{
		  CMaterial* material = g->matm.GetObjectPointer(materialID);
		  material->AddChangesListener(this);
		  OnChanged(3);
		}
	}
}

void CModel::CheckMaterials()
{
	if (!g->rs.IsNotOnlyLowQualitySupported())
	for (unsigned int i = 0; i < m_materials.size() ; i++)
	{
		int materialID = GetMaterialID(m_materials[i]);

		if (materialID != 0)
		{
			CMaterial* material = g->matm.GetObjectPointer(materialID);
			if (material->GetBillboardOrient() == 1)
			{
				m_textureInfo[i] = "";
			}
		}
	}
}

void CModel::AddMaterial(std::string materialName)
{
    int materialID = GetMaterialID(materialName);
	
	m_materials.push_back(MAKE_MP_STRING(materialName));
	m_params[m_materials.size() - 1].materialID = materialID;
	
	if (materialID != -1)
	{
		CMaterial* material = g->matm.GetObjectPointer(materialID);
		material->AddChangesListener(this);
		OnChanged(3);
	}
}

void CModel::ChangeMaterial(int id, std::string materialName)
{
	if (m_params[id].materialID != -1)
	{
		CMaterial* material = g->matm.GetObjectPointer(m_params[id].materialID);
		material->DeleteChangesListener(this);
	}

	int materialID = g->matm.GetObjectNumber(materialName);

	m_params[id].materialID = materialID;

	if (materialID != -1)
	{
		CMaterial* material = g->matm.GetObjectPointer(materialID);
		material->AddChangesListener(this);
	}

	OnChanged(3);
}

void CModel::ChangeMaterial(int id, int materialID)
{
	if (m_params[id].materialID != -1)
	{
		CMaterial* material = g->matm.GetObjectPointer(m_params[id].materialID);
		material->DeleteChangesListener(this);
	}

	m_params[id].materialID = materialID;

	if (materialID != -1)
	{
		CMaterial* material = g->matm.GetObjectPointer(materialID);
		material->AddChangesListener(this);
	}

	OnChanged(3);
}

int CModel::GetMaterial(unsigned int id)
{
	if ((id >= 0) && (id < m_textureCount))
	{
		return m_params[id].materialID;
	}
	else
	{
		return -1;
	}
}

int CModel::GetTextureOrderNumber(int textureID)
{
	int materialID = 0;

	while ((GetTextureOffset(materialID) != 0) || (materialID == 0))
	{
		materialID++;
		if (GetTextureNumber(materialID) == textureID)
		{
			return materialID;
		}
	}

	return -1;
}

void CModel::ChangeTexture(int orderID, int textureID)
{
	SetTextureNumber(orderID, textureID);
}

void CModel::RotateMeshY90()
{
	for (int k = 0; k < GetVertexCount(); k++)
	{
		float tmp = -vx[k * 3 + 0];
		vx[k * 3 + 0] = vx[k * 3 + 2];
		vx[k * 3 + 2] = tmp;
	}

	float tmp = -GetModelMinX();
	SetModelMinX(-GetModelMaxX());
	SetModelMaxX(tmp);

	tmp = GetModelMinX();
	SetModelMinX(GetModelMinZ());
	SetModelMinZ(tmp);

	tmp = GetModelMaxX();
	SetModelMaxX(GetModelMaxZ());
	SetModelMaxZ(tmp);
}

void CModel::RotateMeshX90()
{
	for (int k = 0; k < GetVertexCount(); k++)
	{
		float tmp = -vx[k * 3 + 1];
		vx[k * 3 + 1] = vx[k * 3 + 2];
		vx[k * 3 + 2] = tmp;
	}

	ReCountBoundingBox();
}

void CModel::RotateMeshZ90()
{
	for (int k = 0; k < GetVertexCount(); k++)
	{
		float tmp = -vx[k * 3 + 1];
		vx[k * 3 + 1] = vx[k * 3 + 0];
		vx[k * 3 + 0] = tmp;
	}

	ReCountBoundingBox();
}

char* CModel::GetModelName()
{
	return (char*)CCommonObject::GetName();
}

float* CModel::GetVertexPointer()
{
	return vx;
}

float* CModel::GetNormalPointer()
{
	return vn;
}

float* CModel::GetTextureCoordsPointer(int level)
{
	if (!tx[level])
	{
		tx[level] = MP_NEW_ARR(float, m_vertexCount * 2);
	}

	return tx[level];
}

unsigned short* CModel::GetIndexPointer()
{
	return fs;
}

void CModel::SetFaceOrient(int faceOrient)
{
	m_faceOrient = faceOrient;
}

void CModel::OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, unsigned char /*percent*/)
{
	// do nothing
}

void CModel::OnAsynchResourceLoaded(IAsynchResource* asynch)
{
	std::string fileName = asynch->GetFileName();

#ifdef RESOURCE_LOADING_LOG
	g->lw.WriteLn("loaded model: ", fileName, " deleted ", IsDeleted());
#endif

	if (IsDeleted())
	{
		return;
	}

	std::vector<void *> owners;

	for (int i = 0; i < asynch->GetObjectCount(); i++)
	{
		void* obj;
		float koef;
		asynch->GetObject(i, &obj, &koef);
		owners.push_back(obj);
	}
	
	g->mm.LoadFromIFile(this, asynch->GetIFile(), &owners);
}

void CModel::OnAsynchResourceError(IAsynchResource* /*asynch*/)
{
	g->lw.WriteLn("[LOADWARNING] Load model error: ", GetName());

	FreeResources();

	m_isOk = false;
	OnAfterChanges();

	SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_FILE_NOT_FOUND);
}

void CModel::AddPrimitive(char type,
						  float transX, float transY, float transZ,
						  float rotX, float rotY, float rotZ, float rotW,
						  float scaleX, float scaleY, float scaleZ,
						  float lengthX, float lengthY, float lengthZ)
{
	SPrimitiveDescription primitive;

	primitive.type = type;
	primitive.translation.Set(transX, transY, transZ);
	primitive.rotation.Set(rotX, rotY, rotZ, rotW);
	primitive.scale.Set(scaleX, scaleY, scaleZ);
	primitive.lengthX = lengthX;
	primitive.lengthY = lengthY;
	primitive.lengthZ = lengthZ;

	m_primitives.push_back(primitive);
}

unsigned int CModel::GetPrimitivesCount()const
{
	return m_primitives.size();
}

SPrimitiveDescription* CModel::GetPrimitive(unsigned int index)const
{
	if (index < m_primitives.size())
	{
		return (SPrimitiveDescription*)&m_primitives[index];
	}
	else
	{
		return NULL;
	}
}

bool CModel::GetTraceablePlanePoints(CVector3D translation, CQuaternion rotation, CVector3D scale,
									 CVector3D* v)
{
	if (!v)
	{
		return false;
	}

	int startFaceID = -1, endFaceID = -1;
	for (int i = 0; i < GetTextureCount() - 1; i++)
	if (m_params[i].isTextureTraceable == true)
	{
		if (startFaceID == -1)
		{
			startFaceID = m_params[i + 0].textureOffset;
		}
		endFaceID = m_params[i + 1].textureOffset;

		if (endFaceID <= 0)
		{
			endFaceID = m_faceCount;
		}
	}

	if ((-1 == startFaceID) || (-1 == endFaceID))
	{
		return false;
	}

	std::vector<int> indices;
	
	for (int k = startFaceID * 3; k < endFaceID * 3; k += 3)
	for (int i = 0; i < 3; i++)
	{
		int newIndex = fs[k + i];
		bool isFound = false;

		std::vector<int>::iterator it = indices.begin();
		std::vector<int>::iterator itEnd = indices.end();
		for ( ; it != itEnd; it++)
		if (*it == newIndex)
		{
			isFound = true;
			break;
		}

		if (!isFound)
		{
			indices.push_back(fs[k + i]);			
		}
	}

	if (indices.size() == 8)
	{
		for (int i = 0; i < 4; i++)
		{
			indices.erase(indices.begin());
		}
	}

	assert(indices.size() == 4);
	if (indices.size() == 4)
	{
		std::vector<int>::iterator it = indices.begin();
		std::vector<int>::iterator itEnd = indices.end();

		int i = 0;
		for ( ; it != itEnd; it++, i++)
		{
			int startOffset = (*it) * 3;
			CVector3D v1 = CVector3D(vx[startOffset], vx[startOffset + 1], vx[startOffset + 2]);		
			v1.x =  v1.x * scale.x;
			v1.y =  v1.y * scale.y;
			v1.z =  v1.z * scale.z;
			v1 *= rotation;
			v1 += translation;

			v[i] = v1;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void CModel::GetTextureCoordsForPoint(CVector3D translation, CQuaternion rotation, CVector3D scale, float x, float y, float z, float& _tx, float& _ty, int& textureX, int& textureY)
{ 
	int startFaceID = -1, endFaceID = -1;
	int textureID = 0;
	for (int i = 0; i < GetTextureCount() - 1; i++)
	if (m_params[i].isTextureTraceable == true)
	{
		textureID = i;
		if (startFaceID == -1)
		{
			startFaceID = m_params[i + 0].textureOffset;
		}
		endFaceID = m_params[i + 1].textureOffset;

		if (endFaceID <= 0)
		{
			endFaceID = m_faceCount;
		}
	}

	CVector3D point = CVector3D(x, y, z);

	// ищем ближайший треугольник к точке пространства
	float minDistance = FLT_MAX, curDistance;
	int vxShift1 = 0, vxShift2 = 0, vxShift3 = 0;	
	CPlane3D plane;
	CVector3D v1, v2, v3, vsave1, vsave2, vsave3;

	for (int k = startFaceID * 3; k < endFaceID * 3; k += 3)
	{
		int index1 = fs[k];
		int index2 = fs[k + 1];
		int index3 = fs[k + 2];
		v1 = CVector3D(vx[index1 * 3], vx[index1 * 3 + 1], vx[index1 * 3 + 2]);
		v2 = CVector3D(vx[index2 * 3], vx[index2 * 3 + 1], vx[index2 * 3 + 2]);
		v3 = CVector3D(vx[index3 * 3], vx[index3 * 3 + 1], vx[index3 * 3 + 2]);

		v1.x =  v1.x * scale.x;
		v1.y =  v1.y * scale.y;
		v1.z =  v1.z * scale.z;
		if (!vxOld)
		{
			v1 *= rotation;
		}
		v1 += translation;
		
		v2.x =  v2.x * scale.x;
		v2.y =  v2.y * scale.y;
		v2.z =  v2.z * scale.z;
		if (!vxOld)
		{
			v2 *= rotation;
		}
		v2 += translation;
		
		v3.x =  v3.x * scale.x;
		v3.y =  v3.y * scale.y;
		v3.z =  v3.z * scale.z;
		if (!vxOld)
		{
			v3 *= rotation;
		}
		v3 += translation;
		
		plane = CPlane3D(v1, v2, v3);

		curDistance = plane.GetDistance(point);

		if (abs(curDistance) <= minDistance)
		{
			minDistance = abs(curDistance);
			vxShift1 = index1;
			vxShift2 = index2;
			vxShift3 = index3;
			
			vsave1 = v1;
			vsave2 = v2;
			vsave3 = v3;		
		}
	}

	// решаем уравнение v1*a+v2*b+v3*c=v
	// методом Крамера
	float x1, y1, z1, x2, y2, z2, x3, y3, z3;
	
	x1 = vsave1.x;
	y1 = vsave1.y;
	z1 = vsave1.z;

	x2 = vsave2.x;
	y2 = vsave2.y;
	z2 = vsave2.z;

	x3 = vsave3.x;
	y3 = vsave3.y;
	z3 = vsave3.z;

	double delta = x1*y2*z3 + x2*y3*z1 + x3*y1*z2 - x3*y2*z1 - x2*y1*z3 - x1*y3*z2;

	double deltaA = x*y2*z3 + x2*y3*z + x3*y*z2 - x3*y2*z - x2*y*z3 - x*y3*z2;
	double deltaB = x1*y*z3 + x*y3*z1 + x3*y1*z - x3*y*z1 - x*y1*z3 - x1*y3*z;
	double deltaC = x1*y2*z + x2*y*z1 + x*y1*z2 - x*y2*z1 - x2*y1*z - x1*y*z2;

	if ((delta == 0.0f) && (deltaA*deltaA+deltaB*deltaB+deltaC*deltaC != 0))
	{
		// нет решений
	}

	if (delta == 0.0f && deltaA*deltaA == 0.0f && deltaB*deltaB == 0.0f && deltaC*deltaC == 0.0f)
	{
		// бесконечность и далее
	}

	double a = 0, b = 0, c = 0;

	a = deltaA / delta;
	b = deltaB / delta;
	c = deltaC / delta;

	// расчитываем текстурные координаты
	float tx1, ty1,	tx2, ty2, tx3, ty3;

	tx1 = tx[0][vxShift1 * 2];
	ty1 = tx[0][vxShift1 * 2 + 1];

	tx2 = tx[0][vxShift2 * 2];
	ty2 = tx[0][vxShift2 * 2 + 1];

	tx3 = tx[0][vxShift3 * 2];
	ty3 = tx[0][vxShift3 * 2 + 1];

	_tx = (float)(a * tx1 + b * tx2 + c * tx3);
	_ty = (float)(a * ty1 + b * ty2 + c * ty3);

	CMaterial* material = g->matm.GetObjectPointer(m_params[textureID].materialID);
	textureID = material->GetTextureID(0);
	CVector2D scale = material->GetTextureMatrixKoefs();

	CTexture* texture = g->tm.GetObjectPointer(textureID);
	int textureWidth = texture->GetTextureWidth();
	int textureHeight = texture->GetTextureHeight();	
	if (texture->GetMaxDecreaseKoef() != 1)
	{
		textureWidth *= texture->GetMaxDecreaseKoef();
		textureHeight *= texture->GetMaxDecreaseKoef();
	}
	textureX = (int)((float)textureWidth * _tx);
	textureY = (int)((float)textureHeight * _ty);
	if ((scale.x > 0) && (scale.x != 1.0f))
	{
		textureX -= (int)(textureWidth / 2);
	}
	if ((scale.y > 0) && (scale.y != 1.0f))
	{
		textureY -= (int)(textureHeight / 2);
	}
	textureX = (int)(textureX * scale.x);
	textureY = (int)(textureY * scale.y);
	if ((scale.x > 0) && (scale.x != 1.0f))
	{
		textureX += (int)(textureWidth / 2);
	}
	if ((scale.y > 0) && (scale.y != 1.0f))
	{
		textureY += (int)(textureHeight / 2);
	}
}

void CModel::FreeResources()
{
	EnterCriticalSection(&m_cs);

#ifdef RESOURCE_LOADING_LOG
	g->lw.WriteLn("CModel::FreeResources(): ", GetName());
#endif

	if (fs != NULL)
		MP_DELETE_ARR(fs);	
	if (vx != NULL)
		MP_DELETE_ARR(vx);
	if (vxOld != NULL)
		MP_DELETE_ARR(vxOld);
	if (vn != NULL)
		MP_DELETE_ARR(vn);
	if (tns != NULL)
		MP_DELETE_ARR(tns);

	fs = NULL;
	vx = NULL;
	vxOld = NULL;
	vn = NULL;
	tns = NULL;

	int i;
	for (i = 0; i < 8; i++)
	{
		if (tx[i] != NULL)
			MP_DELETE_ARR(tx[i]);
		tx[i] = NULL;
	}

	m_vertexCount = 0;
	m_faceCount = 0;

	if (m_vbo != NULL)
	{
		MP_DELETE(m_vbo);
		m_vbo = NULL;
	}

	for (i = 0; i < MAX_PARTS; i++)
	if (m_vbos[i] != NULL)
	{
		MP_DELETE(m_vbos[i]);//delete m_vbos[i];
		m_vbos[i] = NULL;
	}
	
	m_materials.clear();
	m_primitives.clear();
	m_textureInfo.clear();
	m_postfix = "";
	m_textureCount = 0;

	g->mm.DeleteObject(GetID());

	LeaveCriticalSection(&m_cs);
}

void CModel::OnChanged(int eventID)
{	
#ifdef RESOURCE_LOADING_LOG
	g->lw.WriteLn("Model OnChanged: eventID ", eventID, " name = ", GetName(), " isdeleted = ", IsDeleted());
#endif

	{
		int oldMode = g->mr.GetMode();

		if (0 == m_textureCount)
		{
			return;
		}

		for (unsigned int i = 0; i < m_textureCount - 1; i++)
		{
			int materialID = m_params[i].materialID;

			if (-1 == materialID)
			{
				materialID = g->rs.GetGlobalMaterialID();
			}

			CMaterial* material = g->matm.GetObjectPointer(materialID);
			m_params[i].isBillBoardOnCPU = (material->GetBillboardOrient() == 2);


			if (m_vbo)
			{
				if (material->IsTangentsNeeded())
				{
					ComputeTangents();
				}

				int billboard = material->GetBillboardOrient();

				if (m_vbos[i])
				if (billboard > 0)
				{
					if (!m_vbos[i]->IsCorrected())
					{
						m_vbos[i]->SetBillboardSize(material->GetShaderParamFloat("sizeXY"), material->GetShaderParamFloat("sizeZ"));

						m_vbos[i]->UpdateBillboardOrient(vx, tx[0], GetVertexCount(), fs, (billboard == 2));
					}
					else if (billboard == 2)
					{
						m_vbos[i]->UpdateBillboardOrient(vx, tx[0], GetVertexCount(), fs, true);
					}
				}
			}

			if (m_params[i].twoSidedMode != 3)
			{
				int twoSidedMode = 0;
				if (material->IsTwoSidedTransparencyNeeded())
				{
					twoSidedMode = 1;
				}
				if (material->IsTwoSidedNeeded())
				{
					twoSidedMode = 2;
				}			

				m_params[i].twoSidedMode = twoSidedMode;
			}		

			if (m_params[i].textureID != -1)
			{
				CTexture* texture = g->tm.GetObjectPointer(m_params[i].textureID);

				g->mr.SetMode(MODE_RENDER_SOLID);
				m_params[i].isMaterialSuited[MODE_INDEX_SOLID] = g->matm.IsMaterialCorrespondsForRenderingMode(texture, materialID);
				
				g->mr.SetMode(MODE_RENDER_TRANSPARENT);
				m_params[i].isMaterialSuited[MODE_INDEX_TRANSPARENT] = g->matm.IsMaterialCorrespondsForRenderingMode(texture, materialID);
				
				g->mr.SetMode(MODE_RENDER_ALPHATEST);
				m_params[i].isMaterialSuited[MODE_INDEX_ALPHATEST] = g->matm.IsMaterialCorrespondsForRenderingMode(texture, materialID);
				
				g->mr.SetMode(MODE_RENDER_ALL);
				m_params[i].isMaterialSuited[MODE_INDEX_ALL] = g->matm.IsMaterialCorrespondsForRenderingMode(texture, materialID);

				m_params[i].isMaterialSuited[MODE_INDEX_SOLID_TRANSPARENT] = 
					(m_params[i].isMaterialSuited[MODE_INDEX_SOLID] ||
						m_params[i].isMaterialSuited[MODE_INDEX_TRANSPARENT]);

				m_params[i].isMaterialSuited[MODE_INDEX_TRANSPARENT_ALPHATEST] = 
					(m_params[i].isMaterialSuited[MODE_INDEX_TRANSPARENT] ||
						m_params[i].isMaterialSuited[MODE_INDEX_ALPHATEST]);

				m_params[i].isMaterialSuited[MODE_INDEX_SOLID_ALPHATEST] = 
					(m_params[i].isMaterialSuited[MODE_INDEX_SOLID] ||
						m_params[i].isMaterialSuited[MODE_INDEX_ALPHATEST]);
			}
			else
			{
				m_params[i].isMaterialSuited[MODE_INDEX_SOLID] = true;
				m_params[i].isMaterialSuited[MODE_INDEX_TRANSPARENT] = false;
				m_params[i].isMaterialSuited[MODE_INDEX_ALPHATEST] = false;
				m_params[i].isMaterialSuited[MODE_INDEX_ALL] = true;
				m_params[i].isMaterialSuited[MODE_INDEX_SOLID_TRANSPARENT] = true;
				m_params[i].isMaterialSuited[MODE_INDEX_TRANSPARENT_ALPHATEST] = false;
				m_params[i].isMaterialSuited[MODE_INDEX_SOLID_ALPHATEST] = true;
			}
		}

		g->mr.SetMode(oldMode);
	}
	
	if (3 != eventID)
	{
		OnAfterChanges(eventID);
	}
}

unsigned int CModel::GetLoadingState()
{
	return m_loadingState;
}

void CModel::SetLoadingState(const unsigned char loadingState, const unsigned char errorCode)
{
	m_errorCode = errorCode;
	m_loadingState = loadingState;
	if (errorCode != 0)
	{
		g->lw.WriteLn("Model ", GetName()," load failed with error code ", (int)errorCode, " source size: ", m_sourceSize);
	}
}

CModel::~CModel()
{
	FreeResources();
	DeleteCriticalSection(&m_cs);
}