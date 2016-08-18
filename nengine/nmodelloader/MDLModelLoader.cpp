
#include "StdAfx.h"
#include "MDLModelLoader.h"
#include "GlobalSingletonStorage.h"
#include "../../cal3d/memory_leak.h"

CMDLModelLoader::CMDLModelLoader()
{
}

bool CMDLModelLoader::LoadModel(IResourceProcessor* rp, IModel* model)
{
	rp->GetShort();
	short faceCount = rp->GetShort();
	model->SetFaceCount(faceCount);
	short vertexCount = rp->GetShort();
	model->SetVertexCount(vertexCount);
	
	// additional model info is identical for MDL and SNE formats
	// that's why this strange code is appeared
	m_sneLoader.ReadAdditionalModelInfo(rp, model);

	float* vx = model->GetVertexPointer();
	float* tx = model->GetTextureCoordsPointer(0);
	unsigned short* fs = model->GetIndexPointer();

	int i;
	for (i = 0; i < vertexCount; i++)
	{
		long t_m_p = (long)rp->GetLong();
		vx[i * 3 + 0] = float(t_m_p) / 65535.0f;
		t_m_p = rp->GetLong();
		vx[i * 3 + 2] = float(t_m_p) / 65535.0f;
		t_m_p = rp->GetLong();
		vx[i * 3 + 1] = float(t_m_p) / 65535.0f;
		t_m_p = 0;
		t_m_p = (unsigned long)rp->GetShort();
		tx[i * 2 + 0] = float(t_m_p) / 256.0f;
		t_m_p = 0;
		t_m_p = (unsigned long)rp->GetShort();
		tx[i * 2 + 1] = float(t_m_p) / 256.0f;
	}

	for (i = 0; i < faceCount; i++)
	{
		fs[i * 3 + 0] = rp->GetShort();
		fs[i * 3 + 1] = rp->GetShort();
		fs[i * 3 + 2] = rp->GetShort();
	}

	CalculateNormals(model);
	
	model->ReCountBoundingBox();

	return true;
}


void CMDLModelLoader::LoadPhysics(IResourceProcessor* /*rp*/, IPhysics* /*model*/)
{

}

void CMDLModelLoader::FreeAll()
{
	CBaseModelLoader::FreeAll();
	CBaseModelLoader* ldr = this;
	MP_DELETE(ldr);
}

unsigned int CMDLModelLoader::GetCalculateNormalsTime()
{
	return GetCalculateTime();
}

CMDLModelLoader::~CMDLModelLoader()
{
}