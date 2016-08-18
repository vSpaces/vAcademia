
#include "StdAfx.h"
#include "TinyXML.h"
#include "XMLList.h"
#include "BlendsManager.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"

CBlendsManager::CBlendsManager()
{
}

void CBlendsManager::AsyncPreloadBlends()
{
	g->taskm.AddTask(&m_preloadBlendsTask, MAY_BE_MULTITHREADED, PRIORITY_LOW, true);
}

bool CBlendsManager::IsBlendsPreloaded()
{
#ifdef _DEBUG
	return true;
#else
	return m_preloadBlendsTask.IsPerformed();
#endif
}

void CBlendsManager::PreloadBlends()
{
	stat_blendsPreloadTime = (int)g->tp.GetTickCount();

	CXMLList avatarList(GetApplicationRootDirectory() + L"avatars.xml", true);
	std::string xmlPath;
	while (avatarList.GetNextValue(xmlPath))
	{
		PreloadBlend(xmlPath);
	}

	stat_blendsPreloadTime = (int)g->tp.GetTickCount() - stat_blendsPreloadTime;
}

void CBlendsManager::PreloadBlend(const std::string& path)
{
	TiXmlDocument doc(path.c_str());
	bool isLoaded = doc.LoadFile();

	if (!isLoaded)
	{
		isLoaded = g->rl->LoadXml( (void*)&doc, path);
		if (!isLoaded)
		{
			return;
		}
	}

	TiXmlNode* object = doc.FirstChild("object");
	if (!object)
	{
		return;
	}

	TiXmlNode* blends = object->FirstChild("blends");

	if (!blends)
	{
		return;
	}

	std::string dir = "";
	int pos = path.find_last_of("\\");
	int pos2 = path.find_last_of("/");
	if (pos2 > pos)
	{
		pos = pos2;
	}

	if (pos != -1)
	{
		dir = path.substr(0, pos + 1);
	}
	
	TiXmlNode* blend = blends->FirstChild("blend");

	while (blend != NULL)
	{
		if (!blend->ToElement()->Attribute("src"))
		{
			continue;
		}

		if (!blend->ToElement()->Attribute("target"))
		{
			continue;
		}

		std::string src = dir;
		src += blend->ToElement()->Attribute("src");

		LoadBlend(src);
			
		blend = blends->IterateChildren(blend);		
	}
}

CSkeletonAnimationObject* CBlendsManager::LoadBlend(const std::string& path, CSkeletonAnimationObject* sao)
{
	std::string name = path;
	StringReplace(name, "ui:ui\\", "ui:");
	
	USES_CONVERSION;
	if (sao)
	{
		g->skom.ManageObject(sao, NULL, path.c_str());		
		sao->SetName( A2W(name.c_str()));
		ManageObject(sao);
		return sao;
	}

	int objectID = GetObjectNumber(name);
	if (objectID != -1)
	{
		return GetObjectPointer(objectID);
	}

	int saoID = g->skom.AddObject(NULL, path.c_str());
	CSkeletonAnimationObject* _sao = g->skom.GetObjectPointer(saoID);
	if (_sao->Load(path, true, NULL))
	{		
		_sao->SetName( A2W( name.c_str()));
		ManageObject(_sao);
		return _sao;
	}
	else
	{
		g->skom.DeleteObject(_sao->GetID());		
		return NULL;
	}
}

CBlendsManager::~CBlendsManager()
{
	ClearAllLinks();
}