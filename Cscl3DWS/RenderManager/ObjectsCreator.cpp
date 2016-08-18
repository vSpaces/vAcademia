
#include "StdAfx.h"
#include "ObjectsCreator.h"
#include "CameraInLocation.h"
#include "ResourceUrlParser.h"
#include "Classes\WhiteBoardManager.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

void PatchUrlIfNeededWithDistance(CResourceUrlParser* url, const unsigned int screenDistance)
{
	if (!url->IsLODForDistanceExists(screenDistance))
	{
		std::string src = "";
		bool isShadowEnabled = false;
		unsigned int index = 0;

		while ((index < url->GetPartCount()) && (url->GetDistance(index) < screenDistance))
		{
			src = url->GetSrc(index);
			isShadowEnabled = url->GetShadowFlag(index);

			index++;
		}

		if ("" == src)
		{
			src = url->GetSrc(0);
		}	

		url->Insert(src, screenDistance, isShadowEnabled);
	}
}

void PatchUrlIfNeeded(std::string& _class, CResourceUrlParser* url)
{
	if (("panel_object" == _class) || ("panel_object_editable" == _class)
		|| ("panel_object_interactive" == _class) || ("panel_object_interactive_editable" == _class))
	{
		PatchUrlIfNeededWithDistance(url, 6000);
		if (gRM->wbMan->GetHideDistance() != 6000)
		{
			PatchUrlIfNeededWithDistance(url, gRM->wbMan->GetHideDistance());
		}
	}
}

bool LoadModels(CResourceUrlParser* url, C3DObject* obj3d, int* modelID, std::string& postfix, bool isHighPriority)
{
	gRM->resLib->GetAsynchResMan()->SetLoadingMesh(obj3d);

	for (unsigned int i = 0; i < url->GetPartCount(); i++)
	{
		modelID[i] = g->mm.GetObjectNumber(url->GetSrc(i) + postfix);
		obj3d->SetUserData(USER_DATA_LOD, (void*)(3 - (int)(i * 3 / url->GetPartCount())));
						
		if (-1 == modelID[i])
		{
			modelID[i] = g->mm.LoadAsynch(url->GetSrc(i), url->GetSrc(i) + postfix);
		}

		CModel* model = g->mm.GetObjectPointer(modelID[i]);

		if (url->IsUnique())
		{			 
			 model->SetPostfix(postfix);
		}

		model->SetLoadPriority(isHighPriority ? PRIORITY_HIGH : PRIORITY_LOW);

		if (model->GetLoadingState() == MODEL_NOT_LOADED)
		{
			gRM->resLib->GetAsynchResMan()->SetLoadingMesh(NULL);
			return false;
		}
	}

	gRM->resLib->GetAsynchResMan()->SetLoadingMesh(NULL);

	return true;
}

void SetupLODGroup(CResourceUrlParser* url, CLODGroup* lodGroup, int* modelID)
{
	lodGroup->ClearAll();

	for (unsigned int i = 0; i < url->GetPartCount(); i++)
	{
		lodGroup->AddModelAsLODLevel(&modelID[i], url->GetShadowFlag(i), 1, (float)url->GetDistance(i));
	}
			
	lodGroup->Finalize();
}

void ReloadLODGroup(C3DObject* obj, std::string& newURL)
{
	if ((!newURL.empty()) && (newURL.find(":") == -1))
	{
		newURL = ":geometry\\" + newURL;
	}

	CResourceUrlParser url(newURL);

	std::string postfix = "";
	int modelID[MAX_LOD_COUNT];
	LoadModels(&url, obj, &modelID[0], postfix, false);

	SetupLODGroup(&url, obj->GetLODGroup(), &modelID[0]);	
}

bool CreateObject3D(std::string _url, std::string _class, C3DObject** obj, std::string name, int level, void* nrmObj, IChangesListener* changesListener, bool isHighPriority)
{
	CResourceUrlParser url(_url);

	PatchUrlIfNeeded(_class, &url);

	*obj = MP_NEW(C3DObject);

	std::string postfix = "";
	if (url.IsUnique())
	{
		postfix = "_" + IntToStr((int)(*obj));
	}

	CLODGroup* lodGroup;
	bool isLODGroupFound = false;
	int lodGroupID = g->lod.GetObjectNumber(_url + postfix);
	if (-1 == lodGroupID)
	{
		lodGroupID = g->lod.AddObject(_url + postfix);
		lodGroup = g->lod.GetObjectPointer(lodGroupID);
	}
	else
	{
		lodGroup = g->lod.GetObjectPointer(lodGroupID);
		isLODGroupFound = true;
	}

	USES_CONVERSION;
	(*obj)->SetName( A2W(name.c_str()));

	int modelID[MAX_LOD_COUNT];
	if (!LoadModels(&url, *obj, &modelID[0], postfix, isHighPriority))
	{
		if (!isLODGroupFound)
		{
			g->lod.DeleteObject(lodGroupID);
		}
		return false;
	}

	CRotationPack rot;
	
	if (!isLODGroupFound)
	{
		SetupLODGroup(&url, lodGroup, &modelID[0]);
	}
	

	// затычка от дублирования
	/* Nap 19.07.2010
	if (name.find("_reality__") != -1)
	{
		int pos = name.find("__");
		name = name.substr(0, pos);
	}
	*/

	if (changesListener)
	{
		(*obj)->AddChangesListener(changesListener);
	}
	if (level >= 0)
	{
		(*obj)->SetUserData(USER_DATA_LEVEL, (void*)level);
	}

	g->o3dm.AddObject3D((*obj), lodGroup, 0, 0, 0, &rot, false);

	int oldObjectID = g->o3dm.GetObjectNumber(name);
	if (oldObjectID != -1)
	{
		g->o3dm.DeleteObject(oldObjectID);
	}

	g->o3dm.ManageObject((*obj));
	(*obj)->SetUserData(USER_DATA_NRMOBJECT, nrmObj);

	assert((*obj));

	if ((LEVEL_CAMERA_COLLISION_TOO == level) || (LEVEL_SEA == level))
	{
		gRM->cameraInLocation->AddLocationRange(*obj);
	}


	if (_class.find("editable")!= std::string::npos)
	{
		(*obj)->SetUserData(USER_DATA_EDITABLE, (void *)1);
	}
	else
	{
		(*obj)->SetUserData(USER_DATA_EDITABLE, 0);
	}

	if (_class.size() > 0)
	{
		(*obj)->SetLODDistanceCanBeChangedStatus(false);
	}

	return true;
}