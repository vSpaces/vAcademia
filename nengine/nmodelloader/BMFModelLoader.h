
#pragma once

#include "CommonModelLoaderHeader.h"

#include "IResourceProcessor.h"
#include "BaseModelLoader.h"
#include "IModelLoader.h"
#include "IModel.h"
#include <string>
#include <vector>

struct SMaterialDesc
{
	unsigned char materialSize;
	unsigned short fsUsingOffset;
	unsigned short vxUsingOffset;
	unsigned short materialFlags;	
};

typedef struct _SMaterial
{
	unsigned int diffuse;
	unsigned int ambient;
	unsigned int specular;
	unsigned int emissive;
	float power;
	unsigned int mapCount;
	MP_VECTOR<unsigned int> mapTypes;
	MP_VECTOR<MP_STRING> mapNames;

	unsigned int	flags;
	unsigned int	level;
	MP_STRING shaderPath;

	_SMaterial();
	_SMaterial(const _SMaterial& other);
} SMaterial;

struct SObjectCfg
{
	float scale;	
	
	// Instances
	bool			is_reference;	
	CVector3D		ref_translation;
	float			ref_rotation[4];
	CVector3D		ref_scale;

	SObjectCfg()
	{
		is_reference = false;
		ref_translation.Set(0,0,0);
		ref_rotation[0] = 0;
		ref_rotation[1] = 0;
		ref_rotation[2] = 1;
		ref_rotation[3] = 0;
		ref_scale.Set(1,1,1);
	};

	SObjectCfg( const SObjectCfg& m)
	{
		this->scale = m.scale;
		this->is_reference = m.is_reference;
		this->ref_translation = m.ref_translation;
		for (int k = 0; k < 4; k++)
		this->ref_rotation[k] = m.ref_rotation[k];
		this->ref_scale = m.ref_scale;
	};
};

class CBMFModelLoader : public CBaseModelLoader, public IModelLoader
{
public:
	CBMFModelLoader();
	~CBMFModelLoader();

	bool LoadModel(IResourceProcessor* rp, IModel* model);
	void LoadPhysics(IResourceProcessor* rp, IPhysics* model);

	void FreeAll();

	unsigned int GetCalculateNormalsTime();

private:
	bool LoadVersion(IResourceProcessor* rp, IModel* model, int version);

	void ReadAll(IResourceProcessor* rp);

	__forceinline void ReadBytes(char* dest, int count)
	{
		if (m_offset + count <= m_size)
		{
			memcpy(dest, m_buffer + m_offset, count);
			m_offset += count;
		}
	}

	unsigned char m_version;

	unsigned int m_size;
	unsigned int m_offset;
	unsigned char* m_buffer;

	std::string ReadZeroEndedString();
	SMaterial LoadMaterial();
};
