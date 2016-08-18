// nrm3DGroup.cpp: implementation of the nrm3DGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrm3DGroup.h"
#include "nrm3DScene.h"
#include "nrmPlugin.h"
#include "MapManager.h"
#include <algorithm>
#include <ifile.h>
#include "TinyXML.h"
#include "UserData.h"
#include "HelperFunctions.h"
#include "ResourceUrlParser.h"
#include <IAsynchResourcemanager.h>

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

nrm3DGroup::nrm3DGroup(mlMedia* apMLMedia): 
	nrm3DObject(apMLMedia),
	m_rmmlLoadAllCalled(false),
	MP_VECTOR_INIT(m_childObjects)
{
}

void nrm3DGroup::OnChanged(int /*eventID*/)
{
	bool isAllLoaded = true;

	std::vector<C3DObject *>::iterator it = m_childObjects.begin();
	std::vector<C3DObject *>::iterator itEnd = m_childObjects.end();

	for ( ; it != itEnd; it++)
	if (!(*it)->GetLODGroup()->IsGeometryLoaded())
	{
		isAllLoaded = false;
	}

	if (isAllLoaded && !m_rmmlLoadAllCalled)
	{
		GetMLMedia()->GetILoadable()->onLoad(RMML_LOAD_STATE_ALL_RESOURCES_LOADED);
		m_rmmlLoadAllCalled = true;
	}
}

void nrm3DGroup::LoadGroup(ifile* file)
{
	int size = file->get_file_size();
	char* str = MP_NEW_ARR(char, size + 1);
	file->read((unsigned char *)str, size);
	str[size] = 0;
	std::string xml = str;
	StringReplace(xml, "<?xml version=\"1.0\" encoding=\"ASCII\"?>", "");
	MP_DELETE_ARR(str);

	char tmp[2];
	tmp[1] = 0;

	bool isScripted = mpMLMedia->GetI3DGroup()->GetScripted();

	TiXmlDocument doc;
	doc.Parse(xml.c_str());

	TiXmlNode* group = doc.FirstChild("group");

	if (group)
	{
		TiXmlNode* item = group->FirstChild("item");
		
		while (item)
		{
			if ((!item->ToElement()->Attribute("type")) ||
				(!item->ToElement()->Attribute("name")) ||
				(!item->ToElement()->Attribute("src")))
			{
				continue;
			}

			std::string type = item->ToElement()->Attribute("type");
			if (type != "object3d")
			{
				continue;
			}

			std::string name = item->ToElement()->Attribute("name");
			std::string src = item->ToElement()->Attribute("src");

			if ((!name.empty()) && (!src.empty()))
			{
				USES_CONVERSION;
				std::string path = W2A( file->get_file_path());
				int pos1 = path.find_last_of("\\");
				int pos2 = path.find_last_of("/");
				int pos = (pos1 > pos2) ? pos1 : pos2;
				path = path.substr(0, pos);
				path += src;

				gRM->resLib->GetAsynchResMan()->SetLoadingMesh(m_obj3d);

				int modelID = g->mm.LoadAsynch(path, path);

				if (modelID != -1)
				{
					CRotationPack rot;

					CLODGroup* lodGroup;
					if (-1 == g->lod.GetObjectNumber(path))
					{
						int lodGroupID = g->lod.AddObject(path);
						lodGroup = g->lod.GetObjectPointer(lodGroupID);
						lodGroup->AddModelAsLODLevel(&modelID, 1, 1, DEFAULT_LOD3_DIST);
						lodGroup->Finalize();
					}
					else
					{
						int lodGroupID = g->lod.GetObjectNumber(path);
						lodGroup = g->lod.GetObjectPointer(lodGroupID);
					}

					int objectID = g->o3dm.AddObject3D(lodGroup, 0, 0, 0, &rot, false);
					C3DObject* child = g->o3dm.GetObjectPointer(objectID);
					if (isScripted)
					{
						CreateRMMLObjectFor3DObject(child, name.c_str());
					}
					else
					{
						child->SetUserData(USER_DATA_NRMOBJECT, this);
					}
					g->o3dm.RenameObject(objectID, path);
					assert(child);

					child->AttachTo(m_obj3d);

					path = "";

					child->AddChangesListener(this);
					m_childObjects.push_back(child);
				}

				gRM->resLib->GetAsynchResMan()->SetLoadingMesh(NULL);
			}

			item = group->IterateChildren("item", item);
		}
	}

	std::string path;

	while (size > 0)
	{
		file->read((unsigned char *)&tmp[0], 1);

		if (tmp[0] != '\n')
		{
			path += tmp;
		} 
		else
		{
			
		}

		size--;
	}

//	ScriptSelfChildren();
}

// загрузка нового ресурса
bool nrm3DGroup::SrcChanged()
{
	USES_CONVERSION;
	const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
	if (!pwcSrc)
	{
		return false;
	}

	if (m_obj3d)
	{
		ClearScriptedInternalObjects();
	}

	// tandy: пока дальность и тень просто удаляем из src
	std::string fn(W2A(pwcSrc));
	std::string tmp = fn;
	int pos = fn.find_first_of(";");
	std::string sSrc = fn;
	int dist = DEFAULT_LOD1_DIST;
	bool isShadowsEnabled = true;

	if (pos != -1)
	{
		int allCount = 0;
		while (pos != -1)
		{
			std::string temp = tmp.substr(0, pos);
			tmp.erase(tmp.begin(), tmp.begin() + pos + 1);
			if (allCount%3 == 0)
			{
				if (allCount > 1)
				{
					temp = ":geometry\\" + temp;
				}
				sSrc = temp.c_str();
			}
			else if (allCount%3 == 1)
			{
				dist = rtl_atoi(temp.c_str());
			}
			else if (allCount%3 == 2)
			{
				isShadowsEnabled = (temp == "1");
			}

			allCount++;

			pos = tmp.find_first_of(";");
			if(allCount >= 3)
				break; // tandy: пока считаем, что у группы только один src
		}
	}

	int objectID = g->o3dm.AddObject(sSrc);
	m_obj3d = g->o3dm.GetObjectPointer(objectID);
	CRotationPack rot;
	m_obj3d->SetRotation(&rot);

	IAsynchResource* asynchRes = gRM->resLib->GetAsynchResMan()->GetAsynchResource(sSrc.c_str(), true, 0, this, gRM->resLib);
	if (asynchRes)
	{
		// resource not found on local disk
		asynchRes->AddObject(m_obj3d);
	}
	else
	{
		ifile* file = gRM->resLib->GetResFile(W2A(pwcSrc));
		LoadGroup(file);
	}

	m_obj3d->SetUserData(USER_DATA_NRMOBJECT, this);

	ScanPlugins();
	InitializePlugins();

	AttachChildrenFromScript();

	return true;
}

void nrm3DGroup::PosChanged(ml3DPosition &pos)
{
	if (m_obj3d)
	{
		m_obj3d->SetCoords((float)pos.x, (float)pos.y, (float)pos.z);
	}
}

void nrm3DGroup::ScaleChanged(ml3DScale &scl)
{
	if (m_obj3d)
	{
		m_obj3d->SetScale((float)scl.x, (float)scl.y, (float)scl.z);
	}
}

void nrm3DGroup::RotationChanged(ml3DRotation &rot)
{
	if (m_obj3d)
	{
		if (m_obj3d->GetRotation())
		{
			m_obj3d->GetRotation()->SetAsDXQuaternion((float)rot.x, (float)rot.y, (float)rot.z, (float)rot.a);
		}
	}
}

// изменился флаг проверки на пересечения
void nrm3DGroup::CheckCollisionsChanged()
{
	// do nothing
} 

bool nrm3DGroup::ChildAdded(mlMedia* /*apChild*/)
{
	return true;
}

bool nrm3DGroup::ChildRemoving(mlMedia* /*apChild*/)
{
	return true;
}

void nrm3DGroup::ScriptedChanged()
{
	assert(FALSE);

	// not supported now
}

bool nrm3DGroup::CreateRMMLObjectFor3DObject(C3DObject* obj3d, const char* aName)
{
	assert(obj3d);
	if (!obj3d)	
	{
		return false;
	}

	// если это объект, описанный в самом скрипте
	if (obj3d->GetUserData(USER_DATA_NRMOBJECT) != NULL)
	{
		return true;
	}

	std::string	name = aName; // obj3d->GetName().c_str();

	rmml::mlObjectInfo4Creation oOI4C;
	oOI4C.musType = MLMT_OBJECT;
	if (name.size() > 0)
	{
		oOI4C.mszName = name.c_str();
	}
	oOI4C.mpParent = mpMLMedia;

	mlMedia* pmlMedia = m_pRenderManager->GetSceneManager()->CreateObject(oOI4C);
		
	assert(pmlMedia);
	if (pmlMedia)
	{
		assert(pmlMedia->GetType() == MLMT_OBJECT);
		if ((pmlMedia->GetType() == MLMT_OBJECT) || (pmlMedia->GetType() == MLMT_GROUP))
		{
			assert(pmlMedia->GetType() == MLMT_OBJECT); 
			// для MLMT_GROUP - не отлаживалось
			nrm3DObject* object = (nrm3DObject*)pmlMedia->GetSafeMO();
			assert(object);
			object->set_auto_scripted_parent(this);
			object->m_obj3d = obj3d;
			assert(find(auto_scripted_object.begin(), auto_scripted_object.end(), obj3d) == auto_scripted_object.end());
			auto_scripted_object.push_back(obj3d);
		}
	}
	else
	{
		wchar_t* pwcName = mpMLMedia->GetStringProp("name");
		rm_trace("Warning: scripted group %S object can not be named. Current name is %s", pwcName ? pwcName : L"", name);
	}
	
	return false;
}

/*
Перебирает все описанные в RMML вложенные в группу объекты, и прикрепляет их к самой группе.
*/
void nrm3DGroup::AttachChildrenFromScript()
{
	assert(GetMLMedia());
	mlMedia** ppChildren = GetMLMedia()->GetChildren();
	if (!ppChildren)	
	{
		return;
	}

	mlMedia** ppChildren_start = ppChildren;
	while (*ppChildren != NULL)
	{
		AttachScriptedChild(*ppChildren);
		ppChildren++;
	}

	GetMLMedia()->FreeChildrenArray(ppChildren_start);
}

bool nrm3DGroup::AttachScriptedChild(mlMedia* apChild)
{
	assert(apChild);
	if (!m_obj3d)	
	{
		return false;
	}
	if (!apChild)	
	{
		return false;
	}
	if ((apChild->GetType() != MLMT_OBJECT) && (apChild->GetType() != MLMT_GROUP)
		&& (apChild->GetType() != MLMT_COMMINICATION_AREA))
	{
		return false;
	}

	nrmObject* apObject = (nrmObject*)apChild->GetSafeMO();
	assert(apObject);
	if (apObject)
	{
		nrm3DObject*	ap3DObject = (nrm3DObject*)apObject;
		if(ap3DObject->m_obj3d) // if added by Tandy
			ap3DObject->m_obj3d->AttachTo(m_obj3d);
		ap3DObject->set_auto_scripted_parent( this);
		if(ap3DObject->m_obj3d) // if added by Tandy
			auto_scripted_object.push_back(ap3DObject->m_obj3d);
	}
	else
	{
		assert(false);
		return false;
	}
	
	return true;
}

bool nrm3DGroup::DetachScriptedChild(mlMedia* apChild)
{
	assert(apChild);

	if (!m_obj3d)	
	{
		return false;
	}
	if (!apChild)
	{
		return false;
	}
	if ((apChild->GetType() != MLMT_OBJECT) && (apChild->GetType() != MLMT_GROUP)
		&& (apChild->GetType() != MLMT_COMMINICATION_AREA))
	{
		return false;
	}

	nrmObject* apObject = (nrmObject*)apChild->GetSafeMO();
	assert(apObject);
	if (apObject)
	{
		nrm3DObject* ap3DObject = (nrm3DObject*)apObject;
		ap3DObject->set_auto_scripted_parent(NULL);
		if (ap3DObject->m_obj3d)
		{
			ap3DObject->m_obj3d->AttachTo(NULL);
		}
	}

	return true;
}

/*
	Надо убрать из группы все автоматически заскриптованные объекты.
*/
bool	nrm3DGroup::ClearScriptedInternalObject(C3DObject* obj3d)
{
	assert(obj3d);
	if (!obj3d)
	{
		return false;
	}

	std::vector<C3DObject*>::iterator it = find(auto_scripted_object.begin(), auto_scripted_object.end(), obj3d);
	if (it != auto_scripted_object.end())
	{
		auto_scripted_object.erase(it);
	}

	nrmObject* nrmObj = (nrmObject *)obj3d->GetUserData(USER_DATA_NRMOBJECT);
	if (!nrmObj)
	{
		return false;
	}

	moMedia* pmoObject = (moMedia*)nrmObj;
	if (!pmoObject)
	{
		return false;
	}

	assert(pmoObject->GetMLMedia());
	if (!pmoObject->GetMLMedia())	
	{
		return false;
	}

	return DetachScriptedChild(pmoObject->GetMLMedia());
}

bool	nrm3DGroup::ClearScriptedInternalObjects()
{
	while (!auto_scripted_object.empty())
	{
		C3DObject* obj = *(auto_scripted_object.begin());
		assert(obj);
		ClearScriptedInternalObject(obj);
	}

	return false;
}

// получает прогресс загрузки объекта
void nrm3DGroup::OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, byte /*percent*/)
{
	// ??
}

// объект загружен
void nrm3DGroup::OnAsynchResourceLoaded(IAsynchResource* asynch)
{
	LoadGroup(asynch->GetIFile());

	GetMLMedia()->GetILoadable()->onLoad(RMML_LOAD_STATE_SRC_LOADED);
}
// ошибка загрузки	
void nrm3DGroup::OnAsynchResourceError(IAsynchResource* /*asynch*/)
{
	GetMLMedia()->GetILoadable()->onLoad(RMML_LOAD_STATE_LOADERROR);
}

void	nrm3DGroup::update(DWORD dwTime, void *data)
{
	nrm3DObject::update(dwTime, data);
}

/*
Если автоскриптовый объект удаляется сборщиком-мусора раньше удаления самой группы,
то его нужно убрать из списка скритповых объектов.
*/
void	nrm3DGroup::auto_scripted_object_is_destroing(nrm3DObject* apnrm3DObject)
{
	assert(apnrm3DObject);
	if (!apnrm3DObject)	
	{
		return;
	}

	assert(apnrm3DObject->m_obj3d);
	if (!apnrm3DObject->m_obj3d)	
	{
		return;
	}

	ClearScriptedInternalObject(apnrm3DObject->m_obj3d);
}

nrm3DGroup::~nrm3DGroup()
{
	ClearScriptedInternalObjects();

	std::vector<C3DObject *>::iterator it = m_childObjects.begin();
	std::vector<C3DObject *>::iterator itEnd = m_childObjects.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->DeleteChangesListener(this);
		g->o3dm.DeleteObject((*it)->GetID());
	}

	g->o3dm.DeleteObject(m_obj3d->GetID());
}