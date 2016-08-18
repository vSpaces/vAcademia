
#include "StdAfx.h"
#include <Math.h>
#include <vector>
#include <string>
#include <assert.h>
#include "BMFMath.h"
#include "Vector3D.h"
#include "quaternion3d.h"
#include "BMFModelLoader.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"
#include "../../cal3d/memory_leak.h"

_SMaterial::_SMaterial():
	MP_VECTOR_INIT(mapTypes),
	MP_VECTOR_INIT(mapNames),
	MP_STRING_INIT(shaderPath)
{

}

_SMaterial::_SMaterial(const _SMaterial& other):
	MP_VECTOR_INIT(mapTypes),
	MP_VECTOR_INIT(mapNames),
	MP_STRING_INIT(shaderPath)
{
	diffuse = other.diffuse;
	ambient = other.ambient;
	specular = other.specular;
	emissive = other.emissive;
	power = other.power;
	mapCount = other.mapCount;

	flags = other.flags;
	level = other.level;	

	shaderPath = other.shaderPath;
	mapTypes = other.mapTypes;
	mapNames = other.mapNames;
}

CBMFModelLoader::CBMFModelLoader():
	m_buffer(NULL),
	m_offset(0),
	m_size(0)
{	
}

void CBMFModelLoader::ReadAll(IResourceProcessor* rp)
{
	//AllocateBufferIfNeeded(rp->GetSize());
	m_offset = 0;
	//rp->Read(m_buffer, m_size);
	m_buffer = (unsigned char*)rp->GetPointer();
	m_size = rp->GetSize();
}

std::string CBMFModelLoader::ReadZeroEndedString()
{
	std::string tmp = "";
	char tmpStr[2];
	tmpStr[1] = 0;

	unsigned char p = 0;

	ReadBytes((char *)&p, 1);

	while (p != 0)
	{
		tmpStr[0] = p;
		tmp = tmp + tmpStr;

		ReadBytes((char*)&p, 1);
	}

	return tmp;
}

SMaterial CBMFModelLoader::LoadMaterial()
{
	SMaterial res;

	unsigned int magic;
	if (m_version < 4)
	{
		ReadBytes((char*)&magic, 4);
	}
		
	unsigned int version = 0;
	if (m_version < 4)
	{
		ReadBytes((char*)&version, 4);
	}
		
	// get the ambient color of the core material
	ReadBytes((char*)&res.ambient, 4);
	
	// get the diffuse color of the core material
	ReadBytes((char*)&res.diffuse, 4);
		
	// get the specular color of the core material
	if (m_version < 4)
	{
		ReadBytes((char*)&res.specular, 4);
	}
	
	// get the emissive color of the core material
	if(m_version < 4 && version >= 1000)
	{
		ReadBytes((char*)&res.emissive, 4);
	}

	ABGR2ARGBINVERTA((unsigned char*)&res.ambient);
	ABGR2ARGBINVERTA((unsigned char*)&res.diffuse);
	ABGR2ARGBINVERTA((unsigned char*)&res.specular);
	ABGR2ARGBINVERTA((unsigned char*)&res.emissive);

	// get the shininess factor of the core material
	if (m_version < 4)
	{
		ReadBytes((char*)&res.power, 4);
	}
	
	// read the number of maps
	int mapCount = 0;
	res.mapCount = 0;
	ReadBytes((char*)&res.mapCount, 2);
	ReadBytes((char*)&mapCount, 2);
			
	// load all maps
	unsigned int mapId;
	for (mapId = 0; mapId < res.mapCount; mapId++)
	{
		// get the filename length of the map
		int len = 0;
		if (m_version < 4)
		{
			ReadBytes((char*)&len, 4);
		}
		else
		{
			ReadBytes((char*)&len, 1);
		}		
				
		// read the filename of the map
		char strBuffer[255];
		ReadBytes(&strBuffer[0], len);
		strBuffer[len] = 0;

		std::string str = strBuffer;
		str = StringToUpper(str);		

		int map_type = 1;

		if (m_version < 4)
		{
			if (version >= 1002)
			{
				ReadBytes((char*)&map_type, 4);
				if (map_type == 1 || map_type == 0)
				{
					res.mapTypes.push_back(map_type);
				}
			}
		}
		else
		{
			ReadBytes((char*)&map_type, 1);
			if (map_type == 1 || map_type == 0)
			{
				res.mapTypes.push_back(map_type);
			}
		}

		if (map_type == 1 || map_type == 0)
		{
			res.mapNames.push_back(MAKE_MP_STRING(str));
		}
	}

	if (mapCount != 0)
	{
		res.mapCount = mapCount;
	}

	return res;
}

bool CBMFModelLoader::LoadModel(IResourceProcessor* rp, IModel* model)
{
	// сигнатура
	char sign[3];
	ReadAll(rp);
	ReadBytes(&sign[0], 3);
	
	if (!(('B' == sign[0]) && ('M' == sign[1]) && ('F' == sign[2])))
	{
		return false;
	}

	// версия
	unsigned char version = 0;
	ReadBytes((char *)&version, 1);
	m_version = version;

	// переключаемся между версиями
	if( version <= 5)
	{
		return LoadVersion(rp, model, version);
	}
	else
	{
		assert(false);
		return false;
	}
}

void CBMFModelLoader::LoadPhysics(IResourceProcessor* /*rp*/, IPhysics* model)
{
	if (m_version < 4)
	{
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	// Чтение примитивов
	unsigned short primitiveCount;
	ReadBytes((char *)&primitiveCount, 2);
	
	char primitiveType;	

	CVector3D primTranslation;
	CQuaternion primRotation;
	CVector3D primScale;

	float x_length, y_length, z_length, r;

	for (int i = 0; i < primitiveCount; i++)
	{
		ReadBytes(&primitiveType, 1);

		if (primitiveType == 1)
		{
			ReadBytes((char*)&primTranslation, sizeof(primTranslation));
			ReadBytes((char*)&primRotation, sizeof(primRotation));
			ReadBytes((char*)&primScale, sizeof(primScale));

			ReadBytes((char *)&x_length, sizeof(x_length));
			ReadBytes((char *)&y_length, sizeof(y_length));
			ReadBytes((char *)&z_length, sizeof(z_length));
			// сохраняем примитив
			model->AddPrimitive(primitiveType, primTranslation.x, primTranslation.y, primTranslation.z,
								primRotation.x, primRotation.y, primRotation.z, primRotation.w,
								primScale.x, primScale.y, primScale.z,
								x_length, y_length, z_length);
		}
		else if (primitiveType == 2)
		{
			ReadBytes((char*)&primTranslation, sizeof(primTranslation));
			ReadBytes((char*)&primScale, sizeof(primScale));

			ReadBytes((char *)&r, sizeof(r));
			// сохраняем примитив
			model->AddPrimitive(primitiveType, primTranslation.x, primTranslation.y, primTranslation.z,
								0, 0, 0, 0,
								primScale.x, primScale.y, primScale.z,
								r*2, r*2, r*2);
		}
	}	
}

bool CBMFModelLoader::LoadVersion(IResourceProcessor* rp, IModel* model, int version)
{
	// Загрузить смещение, поворот, скейл

	CVector3D tmpTranslation;
	float tmpRotation[4];
	CVector3D tmpScale;

	model->SetFaceOrient(0x0901 /*GL_CCW*/);

	ReadBytes((char*)&tmpTranslation, sizeof(tmpTranslation));
	ReadBytes((char*)&tmpRotation, sizeof(float) * 4);
	ReadBytes((char*)&tmpScale, sizeof(tmpScale));

	CVector3D deltaTranslation = tmpTranslation;
	AlignVectorToWorld(tmpTranslation);
	deltaTranslation = deltaTranslation - tmpTranslation;
    
	// Загрузить конфигурашку модели
	SObjectCfg objCfg;
	int i;

	if (version < 4)
	{
		unsigned char tmp;
		ReadBytes((char *)&tmp, 1);
		objCfg.is_reference = (tmp != 0);

		if (objCfg.is_reference)
		{
			ReadZeroEndedString();

			ReadBytes((char *)&objCfg.ref_rotation, 16);
			ReadBytes((char *)&objCfg.ref_translation, 12);
			ReadBytes((char *)&objCfg.ref_scale, 12);

			AlignVectorToWorld(objCfg.ref_translation);
		}
		else
		{
			ReadBytes((char *)&objCfg.scale, sizeof(objCfg.scale));

			ReadZeroEndedString();
			ReadZeroEndedString();
			ReadZeroEndedString();
		}

		unsigned char propertyCount = 0;
		ReadBytes((char *)&propertyCount, 1);
		
		for (i = 0; i < propertyCount; i++)
		{
			ReadZeroEndedString();
			ReadZeroEndedString();
		}

		unsigned char boxCount = 0;
		ReadBytes((char *)&boxCount, 1);
		
		for (i = 0; i < boxCount; i++)
		{
			float tmpBox[6];

			ReadBytes((char*)&tmpBox, 24);
		}

		if( objCfg.is_reference)
		{
			return false;
		}
	}

	// Загрузить материалы
	unsigned short materialCount;
	ReadBytes((char *)&materialCount, 2);
	
	SMaterialDesc* matDescs = (SMaterialDesc*)_alloca((materialCount + 1) * sizeof(SMaterialDesc));

	for (i = 0; i < materialCount; i++)
	{
		SMaterialDesc tmp;

		ReadBytes((char*)&tmp.materialSize, 1);
		ReadBytes((char*)&tmp.fsUsingOffset, 2);
		ReadBytes((char*)&tmp.vxUsingOffset, 2);
		ReadBytes((char*)&tmp.materialFlags, 2);

		if (version < 4)
		{
			ReadZeroEndedString();
		}

		matDescs[i] = tmp;
	}

	SMaterialDesc tmp;

	tmp.fsUsingOffset = 0xFFFF;
	tmp.vxUsingOffset = 0xFFFF;
	matDescs[materialCount] = tmp;

	std::vector<SMaterial> materials;

	// загрузка CalMaterial
	for (i = 0; i < materialCount; i++)
	{
		SMaterial material = LoadMaterial();
		materials.push_back(material);
	}

	// Загрузка скелета
	unsigned short skeletonSize = 0;
	if (version < 4)
	{
		ReadBytes((char *)&skeletonSize, 2);
	}

	// Загрузка вершин и фейсов
	unsigned short vertexCount = 0;
	unsigned short faceCount = 0;

	float  scale = 1;
	float ofsX = 0, ofsY = 0, ofsZ = 0;

	ReadBytes((char *)&vertexCount, 2);
	ReadBytes((char *)&faceCount, 2);

	if ((vertexCount <= 0) || (faceCount <= 0))
	{
		return false;
	}

	model->SetVertexCount(vertexCount);
	model->SetFaceCount(faceCount);

	unsigned char isFloatVertex = 0;
	if (version >= 5)
	{
		ReadBytes((char *)&isFloatVertex, 1);
	}

	if( version == 1)
	{
		ReadBytes((char *)&scale, sizeof(scale));
		ReadBytes((char *)&ofsX, sizeof(ofsX));
		ReadBytes((char *)&ofsY, sizeof(ofsY));
		ReadBytes((char *)&ofsZ, sizeof(ofsZ));		
	}
	else
	{
		int i_scale = 1;
		int i_ofsX = 0, i_ofsY = 0, i_ofsZ = 0;

		ReadBytes((char *)&i_scale, sizeof(i_scale));
		ReadBytes((char *)&i_ofsX, sizeof(i_ofsX));
		ReadBytes((char *)&i_ofsY, sizeof(i_ofsY));
		ReadBytes((char *)&i_ofsZ, sizeof(i_ofsZ));

		scale = (float)i_scale;
		ofsX = (float)i_ofsX;
		ofsY = (float)i_ofsY;
		ofsZ = (float)i_ofsZ;
	}

	int matNum = 0;

	int mapCount = 0;
	unsigned short u, v;
	short xyz[3];
	float _xyz[3], uu = 0, vv = 0;

	std::vector<unsigned short> faces;

	unsigned int maxMapCount = 0;

	for (i = 0; i < materialCount; i++)
	if (materials[i].mapCount > maxMapCount)
	{
		maxMapCount = materials[i].mapCount;
	}

	float* vx = model->GetVertexPointer();
	float* tx = model->GetTextureCoordsPointer(0);
	unsigned short* fs = model->GetIndexPointer();

	/*float* tx2 = NULL;
	if (maxMapCount > 1)
	{
		tx2 = model->GetTextureCoordsPointer(1);
	}*/

	int cnt = 0;

	if ((unsigned int)(rp->GetSize() - m_offset) < (unsigned int)(vertexCount * (2 + 2 * isFloatVertex) * 3))
	{
		return false;
	}

	for (i = 0; i < vertexCount; i++)
	{
		if (matDescs[matNum].vxUsingOffset == i)
		{
			mapCount = materials[matNum].mapCount;
			matNum++;
		}

		if (0 == isFloatVertex)
		{
			ReadBytes((char *)&xyz, 6);
			
			if(version == 1)
			{
				_xyz[0] = (float)(xyz[0] / scale + ofsX);
				_xyz[1] = (float)(xyz[1] / scale + ofsY);
				_xyz[2] = (float)(xyz[2] / scale + ofsZ);
			}
			else
			{
				_xyz[0] = (float)(xyz[0] * scale + ofsX);
				_xyz[1] = (float)(xyz[1] * scale + ofsY);
				_xyz[2] = (float)(xyz[2] * scale + ofsZ);
			}

			_xyz[0] = tmpTranslation.x + deltaTranslation.x + _xyz[0];
			_xyz[1] = tmpTranslation.y + deltaTranslation.y + _xyz[1];
			_xyz[2] = tmpTranslation.z + deltaTranslation.z + _xyz[2];

			AlignVectorcToWorld(_xyz[0], _xyz[1], _xyz[2]);
			_xyz[0] -= tmpTranslation.x;
			_xyz[1] -= tmpTranslation.y;
			_xyz[2] -= tmpTranslation.z;

			_xyz[0] += 0.5f;
			_xyz[1] += 0.5f;
			_xyz[2] += 0.5f;
		}
		else
		{
			ReadBytes((char *)&_xyz, 12);
		}

		vx[i * 3 + 0] = _xyz[0];
		vx[i * 3 + 1] = _xyz[1];
		vx[i * 3 + 2] = _xyz[2];

		for (int k = 0; k < mapCount; k++)
		{
			if( version < 3)
			{
				ReadBytes((char*)&u, 2);
				ReadBytes((char*)&v, 2);
				uu = (float)u / 255.0f - 128.0f;
				vv = 1 - ((float)v / 255.0f - 128.0f);
			}
			else
			{
				ReadBytes((char*)&uu, sizeof(float));
				ReadBytes((char*)&vv, sizeof(float));
				vv = 1 - vv;
			}

			switch (k)
			{
			case 0:
				tx[cnt++] = uu;
				tx[cnt++] = vv;
				break;

			case 1:
				/*tx2[cnt2++] = uu;
				tx2[cnt2++] = vv;*/
				break;
			}			
		}

		if ((unsigned int)mapCount > maxMapCount) 
		{
			maxMapCount = mapCount;
		}

		/*if ((mapCount < 2) && (tx2))
		{
			tx2[cnt2++] = uu;
			tx2[cnt2++] = vv;
		}*/

		if (mapCount < 1)
		{
			tx[cnt++] = uu;
			tx[cnt++] = vv;
		}
	}

	if ((unsigned int)(rp->GetSize() - m_offset) >= (unsigned int)(faceCount * 2 * 3))
	{
		ReadBytes((char*)fs, faceCount * 2 * 3);
	}
	else
	{
//		assert(false);
		return false;
	}

	for (int j = 0; j < faceCount * 3; j += 3)
	{		
		if ((fs[j] >= vertexCount) || (fs[j + 1] >= vertexCount) || (fs[j + 2] >= vertexCount))
		{
			fs[j] = 0;
			fs[j + 1] = 0;
			fs[j + 2] = 0;
		}
	}

	for (int k = 0; k < ((materialCount < 64) ? materialCount : 64); k++)
	{
		unsigned int ambient = materials[k].ambient;
		unsigned int diffuse = materials[k].diffuse;

		int r = (ambient & 0x000000FF) + (diffuse & 0x000000FF);
		int g = ((ambient & 0x0000FF00) >> 8) + ((diffuse & 0x0000FF00) >> 8);
		int b = ((ambient & 0x00FF0000) >> 16) + ((diffuse & 0x00FF0000) >> 16);
		int a = ((ambient & 0xFF000000) >> 24) + ((diffuse & 0xFF000000) >> 24);
		ClampValue(r, 0, 255);
		ClampValue(g, 0, 255);
		ClampValue(b, 0, 255);
		ClampValue(a, 0, 255);

		unsigned int color = r + (g << 8) + (b << 16) + (a << 24);

		std::string textureName = "";	
		if ((materials[k].mapNames.size() > 0) && (materials[k].mapNames[0] != ""))
		{
			textureName = materials[k].mapNames[0];
		}
		else
		{
			textureName = "#" + ULongToStr(color);
		}
		model->AddTexture(matDescs[k].fsUsingOffset, textureName);

		
		if (materials[k].mapNames.size() > 0)
		{
			std::string matName = "external\\materials\\";
			matName += materials[k].mapNames[0];
			matName = StringToLower(matName);
			StringReplace(matName, ".", "_");
			matName += ".xml";

			model->AddMaterialSafety(matName);
			if (matDescs[k].materialFlags == 1)
			{
				model->SetTwoSided(k, true);
			}
		}
		else
		{
			model->AddMaterialSafety("-1");
			if (matDescs[k].materialFlags == 1)
			{
				model->SetTwoSided(k, true);
			}
		}
	}

	model->AddTexture(0, "");
	
	CalculateNormals(model);

	model->ReCountBoundingBox();


	return true;
}

void CBMFModelLoader::FreeAll()
{
	CBaseModelLoader::FreeAll();
	CBMFModelLoader* ldr = this;
	MP_DELETE(ldr);
}

unsigned int CBMFModelLoader::GetCalculateNormalsTime()
{
	return GetCalculateTime();
}

/*void CBMFModelLoader::CleanUpMemory()
{
	if (m_buffer)
	{
		MP_DELETE_ARR(m_buffer);
		m_buffer = NULL;
		m_size = 0;
		m_offset = 0;
	}
}*/

/*void CBMFModelLoader::AllocateBufferIfNeeded(unsigned int size)
{
	if (m_size < size)
	{
		CleanUpMemory();

		m_buffer = MP_NEW_ARR(unsigned char, size);
		m_size = size;
	}
}*/

CBMFModelLoader::~CBMFModelLoader()
{
//	CleanUpMemory();
}