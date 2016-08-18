
#include "StdAfx.h"
#include "SNEModelLoader.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"
#include "../../cal3d/memory_leak.h"

CSNEModelLoader::CSNEModelLoader()
{
}

bool CSNEModelLoader::LoadModel(IResourceProcessor* rp, IModel* model)
{
	int vxCount = 0;
	int fsCount = 0;
	vxCount = rp->GetShort();
	fsCount = rp->GetShort();
	model->SetVertexCount(vxCount);
	model->SetFaceCount(fsCount);
			
	ReadAdditionalModelInfo(rp, model);

	float* vx = model->GetVertexPointer();
	float* tx = model->GetTextureCoordsPointer(0);
	unsigned short* fs = model->GetIndexPointer();

	int i;
	for (i = 0; i < vxCount; i++)
	{
		long t_m_p = (long)rp->GetLong();
		float* temp = (float *)&t_m_p;
		vx[i * 3 + 0] = *temp;
		t_m_p = (long)rp->GetLong();
		temp = (float *)&t_m_p;
		vx[i * 3 + 2] = *temp;
		t_m_p = (long)rp->GetLong();
		temp = (float *)&t_m_p;
		vx[i * 3 + 1] = *temp;
		t_m_p = (long)rp->GetLong();
		temp = (float *)&t_m_p;
		tx[i * 2 + 0] = *temp;
		t_m_p = (long)rp->GetLong();
		temp = (float *)&t_m_p;
		tx[i * 2 + 1] = *temp;
	}

	for (i = 0; i < fsCount; i++)
	{
		fs[i * 3 + 0] = rp->GetShort();
		fs[i * 3 + 1] = rp->GetShort();
		fs[i * 3 + 2] = rp->GetShort();
	}

	CalculateNormals(model);
		
	model->ReCountBoundingBox();

	return true;
}

void CSNEModelLoader::LoadPhysics(IResourceProcessor* /*rp*/, IPhysics* /*model*/)
{

}

void CSNEModelLoader::ReadAdditionalModelInfo(IResourceProcessor* rp, IModel* model)
{
	int materialCount = rp->GetShort();

	std::string modelName = model->GetModelName();
	std::string lastName = "";
	
	for (int i = 1; i <= materialCount; i++)
	{
		int offset = rp->GetShort();

		char ic = 1;
		std::string name = "";

		while (ic != 0)
		{
			char tmp[2];
			tmp[0] = (ic = rp->GetChar());
			tmp[1] = 0;
			name += tmp;
		}

		if ((name == "") || (name == " "))
		{
			name = lastName;
		}

		lastName = name;

		model->AddTexture(offset, name);

		name = StringToLower(name);
		modelName = StringToLower(modelName);

		StringReplace(name, ".", "_");
		StringReplace(modelName, ".", "_");

		name += ".xml";

		std::string matName = DEFAULT_MATERIAL_PATH;
		matName += modelName;
		matName += "_";
		matName += name;

		model->AddMaterial(matName);
	}

	model->AddTexture(0, "");
}

void CSNEModelLoader::FreeAll()
{
	CBaseModelLoader::FreeAll();
	CSNEModelLoader* ldr = this;
	MP_DELETE(ldr);
}

unsigned int CSNEModelLoader::GetCalculateNormalsTime()
{
	return GetCalculateTime();
}

CSNEModelLoader::~CSNEModelLoader()
{
}