
#include "StdAfx.h"
#include "PlatformDependent.h"
#include "ClothEditorSupporter.h"
#include "GlobalSingletonStorage.h"

CClothEditorSupporter::CClothEditorSupporter()
{
}

void CClothEditorSupporter::ReloadClipboxs()
{
	if (!g->phom.GetControlledObject())
	{
		return;
	}

	C3DObject* avatar = g->phom.GetControlledObject()->GetObject3D();
	if (!avatar)
	{
		return;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)avatar->GetAnimation();
	if (!sao)
	{
		return;
	}

	std::wstring fn = GetApplicationRootDirectory();
	fn += L"\\clipboxs.txt";

	FILE* fl = _wfopen(fn.c_str(), L"r");
	if (!fl)
	{
		return;
	}

	for (int k = 0; k < 5; k++)
	{
		int id = 0, x1 = 0, x2 = 0, y1 = 0, y2 = 0, z1 = 0, z2 = 0;
		fscanf(fl, "%i %i %i %i %i %i %i\n", &id, &x1, &x2, &y1, &y2, &z1, &z2);
		if (id != 0)
		{
			if ((x1 != 0) || (x2 != 0) || (y1 != 0) || (y2 != 0) || (z1 != 0) || (z2 != 0))
			{
				sao->SetClippingVolume(id == 9 ? CLIPPING_VOLUME_TYPE_SPHERE : CLIPPING_VOLUME_TYPE_BOX,
					id - 1, 1, (float)x1, (float)x2, 2, (float)y1, (float)y2, 3, (float)z1, (float)z2);
			}
			else
			{
				sao->DisableClippingVolume(id - 1);
			}
		}
	}

	fclose(fl);	
}

void CClothEditorSupporter::ReloadMaterials()
{
	if (!g->phom.GetControlledObject())
	{
		return;
	}

	C3DObject* avatar = g->phom.GetControlledObject()->GetObject3D();
	if (!avatar)
	{
		return;
	}

	CLODGroup* avatarLODGroup = avatar->GetLODGroup();
	if (!avatarLODGroup)
	{
		return;
	}

	std::vector<C3DObject *> lodGroupObjects;

	std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
	std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	if ((*it)->GetLODGroup() == avatarLODGroup)
	{
		lodGroupObjects.push_back(*it);
	}

	it = lodGroupObjects.begin();
	itEnd = lodGroupObjects.end();

	for ( ; it != itEnd; it++)
	{
		avatar = *it;
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)avatar->GetAnimation();
		if (!sao)
		{
			continue;
		}

		CModel* model = sao->GetModel();
		if (!model)
		{
			continue;
		}

		for (int i = 0; i < model->GetTextureCount() - 1; i++)
		{
			int materialID = model->GetMaterial(i);
			if (-1 != materialID)
			{
				g->matm.ReloadMaterial(materialID);
			}
		}
	}
}

void CClothEditorSupporter::ShowClothes(bool isVisible)
{
	if (!g->phom.GetControlledObject())
	{
		return;
	}

	C3DObject* avatar = g->phom.GetControlledObject()->GetObject3D();
	if (!avatar)
	{
		return;
	}

	CLODGroup* avatarLODGroup = avatar->GetLODGroup();
	if (!avatarLODGroup)
	{
		return;
	}

	std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
	std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	if (((*it)->GetLODGroup() == avatarLODGroup) && (*it != avatar))
	{
		(*it)->SetVisible(isVisible);
	}
}

CClothEditorSupporter::~CClothEditorSupporter()
{
}