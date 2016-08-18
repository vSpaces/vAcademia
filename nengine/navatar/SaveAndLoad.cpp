
#include "stdafx.h"
#include "FakeObject.h"
#include "XMLAttributes.h"
#include "HelperFunctions.h"
#include "SkeletonAnimationObject.h"
#include "LoadingCal3DObjectsTask.h"

bool CAvatarObject::LoadAndBind(std::string src, C3DObject** ppLoadModel)
{
	C3DObject* obj = m_avatarParts[0].obj3d;


	assert(obj);
	if (!obj)
	{
		return false;
	}

	assert(obj->GetLODGroup());
	if (!obj->GetLODGroup())
	{
		return false;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)obj->GetAnimation();	
//	assert(sao);
	if (!sao)
	{
		return false;
	}

	int saoID = g->skom.GetObjectNumber(obj, src);
	if (saoID != -1)
	{
		g->skom.DeleteObject(saoID);
	}

	CSkeletonAnimationObject* newSAO = MP_NEW(CSkeletonAnimationObject);
	if (!newSAO->Load(src, false, sao)) 
	{
		g->lw.WriteLn(L"[AVED] failed to load: ", src);
		MP_DELETE(newSAO);
		return false;
	}

	saoID = g->skom.ManageObject(newSAO, obj, src);

	std::string name = src;
	name += IntToStr(g->o3dm.GetCount());
	int objectID = g->o3dm.AddObject(name);
	C3DObject* newObj = g->o3dm.GetObjectPointer(objectID);
	newObj->SetAnimation(newSAO);
	CRotationPack rot;
	newObj->SetRotation(&rot);		
	newObj->SetZOffset(30.0f);
	newObj->SetLODGroup(obj->GetLODGroup());
	
	newObj->AttachTo(obj);
	obj->GetLODGroup()->Finalize();

	newObj->SetLocalCoords(0, 0, 0);

	*ppLoadModel = newObj;

	g->lw.WriteLn("[AVED] success load: ", src);

	int startTime = (int)g->tp.GetTickCount();

	if (m_isMyAvatar)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)newObj->GetAnimation();
		bool isLoaded = sao->IsContainsExternalTextures();
		while (!isLoaded)
		{
			isLoaded = true;

			CModel* model = sao->GetModel();

			for (int i = 0; i < model->GetTextureCount() - 1; i++)
			{
				int textureID = model->GetTextureNumber(i);
				CTexture* texture = g->tm.GetObjectPointer(textureID);
				if (texture->GetTextureWidth() == 0)
				{
					isLoaded = false;
				}
			}

			if (!isLoaded)
			{
				g->tm.Update();
				Sleep(1);

				if (g->tp.GetTickCount() - startTime > 2000)
				{
					break;
				}
			}	
		}
	}

	return true;
}

bool CAvatarObject::LoadAndBindDevided(std::string src, C3DObject** ppLoadModel, void* data, unsigned int size)
{
	STransitionalInfo* info = MP_NEW(STransitionalInfo);
	info->obj = (m_avatarParts.size() > 0) ? m_avatarParts[0].obj3d : NULL;

	if (!LoadAndBindCal3DObjects(info, src, data, size))
	{
		if (info->newSAO)
		{
			info->newSAO->FreeResources();
			if (info->newSAO->GetID() == 0xFFFFFFFF)
			{
				MP_DELETE(info->newSAO);
			}
		}
		info->Clear();
		MP_DELETE(info);
		return false;
	}
	
	LoadAndBindEngineObjects(info, src, ppLoadModel);

	MP_DELETE(info);

	return true;
}

bool CAvatarObject::LoadAndBindCal3DObjects(STransitionalInfo* info, std::string src, void* data, unsigned int size)
{
	if (!info)
	{
		return false;
	}

	assert(info->obj);
	if (!info->obj)
	{
		return false;
	}

	if (!info->obj->GetLODGroup())
	{
		return false;
	}

	assert(info->obj->GetLODGroup());

	info->sao = (CSkeletonAnimationObject *)info->obj->GetAnimation();	
	//	assert(sao);
	if (!info->sao)
	{
		return false;
	}

	info->newSAO = MP_NEW(CSkeletonAnimationObject);
	USES_CONVERSION;
	std::wstring name = m_obj3d->GetName();
	name += L"__";
	name += A2W( src.c_str());
	info->newSAO->SetName( name);

	if (!info->newSAO->LoadCal3DObjects(src, info, false, info->sao, data, size))
	{
		g->lw.WriteLn("[AVED] failed to load: ", src);
		MP_DELETE(info->newSAO);
		info->newSAO = NULL;
		return false;
	}

	return true;
}

void CAvatarObject::LoadAndBindEngineObjects(STransitionalInfo* info, std::string src, C3DObject** ppLoadModel)
{	
	int saoID = g->skom.GetObjectNumber(info->obj, src);
	if (saoID != -1)
	{
		g->skom.DeleteObject(saoID);
	}

	saoID = g->skom.ManageObject(info->newSAO, info->obj, src);

	info->newSAO->LoadEngineObjects(info->sao, info);

	std::string name = src;
	name += IntToStr(g->o3dm.GetCount());
	int objectID = g->o3dm.AddObject(name);
	C3DObject* newObj = g->o3dm.GetObjectPointer(objectID);
	newObj->SetAnimation(info->newSAO);
	CRotationPack rot;
	newObj->SetRotation(&rot);		
	newObj->SetZOffset(30.0f);
	newObj->SetLODGroup(info->obj->GetLODGroup());
	newObj->AttachTo(info->obj);
	info->obj->GetLODGroup()->Finalize();

	newObj->SetLocalCoords(0, 0, 0);

	*ppLoadModel = newObj;

	g->lw.WriteLn("[AVED] success load: ", src);
}

void	CAvatarObject::DeleteAvatarPart(SAvatarObjPart& part)
{	
	part.parameters.clear();
	if (part.objName != HEAD_DEFAULT_NAME)
	{
		ClearUserResources(part);	
	}
}

void	CAvatarObject::UndressAvatar()
{
	if (m_avatarParts.size() == 0)
	{
		return;
	}

	// не с 0 так как 0 - тело, которое не надо перегружать и очищать
	for (int k = m_avatarParts.size() - 1; k > 0; k--)
	{
		SAvatarObjPart& part = m_avatarParts[k];
		DeleteAvatarPart(part);
	}

	m_avatarParts.erase(m_avatarParts.begin() + 1, m_avatarParts.end());

	if (m_obj3d->GetLODGroup())
	{
		m_obj3d->GetLODGroup()->DeleteAllExcept(m_obj3d);
	}

	m_savedParameters.clear();
	
	m_commonParameters.parameters.clear();
	m_clothesBrightness.clear();

	std::vector<SCommandsParam*>::iterator it1 = m_rgbvColorCommands.begin();
	std::vector<SCommandsParam*>::iterator it1End = m_rgbvColorCommands.end();

	for ( ; it1 != it1End; it1++)
	{
		MP_DELETE(*it1);
	}
	m_rgbvColorCommands.clear();

	m_changeTextureCommands.clear();

	std::vector<SCommandsParam*>::iterator it2 = m_blendCommands.begin();
	std::vector<SCommandsParam*>::iterator it2End = m_blendCommands.end();

	for ( ; it1 != it1End; it1++)
	{
		MP_DELETE(*it2);
	}

	m_blendCommands.clear();

	ClearTasks();

	if (m_compositeTextureManager)
	{
		m_compositeTextureManager->UpdateLinksToCompositeTextures(m_avatarParts[0].obj3d, false);
		//m_compositeTextureManager->DeleteAllCompositeTextures();
	}

	m_headID = 1;
}

void CAvatarObject::HandleLoadMesh(const std::string& partName, const std::string& partSrc)
{
	SaveAvatarObjPartsInfo(partName, partSrc, NULL);
	/*if (GetIndexPart(partName) == -1)
	{
		SAvatarObjPart newMesh;
		newMesh.objName = partName;
		newMesh.objSrc = partSrc;

		if (LoadAndBind(newMesh.objSrc, &newMesh.obj3d))
		{
			m_avatarParts.push_back(newMesh);
		}
	}*/
	
}

void CAvatarObject::HandleChangeBodyTexture(const std::string _textureSrc)
{
	g->lw.WriteLn("HandleChangeBodyTexture ", _textureSrc);
	std::string textureSrc = _textureSrc;
	int textureID = g->tm.GetObjectNumber( textureSrc);
	if (textureID == -1)
	{				
		textureID = g->tm.LoadAsynch( textureSrc, textureSrc, NULL);
	}
	m_obj3d->GetAnimation()->GetModel()->ChangeTexture(0, textureID);

	int materialID = m_obj3d->GetAnimation()->GetModel()->GetMaterial(0);
	CMaterial* mat = g->matm.GetObjectPointer(materialID);
	STexture* tex = MP_NEW(STexture);
	tex->textureID = textureID;
	tex->isAlphaBlend = false;
	mat->SetTextureLevel(0, TEXTURE_SLOT_TEXTURE_2D, tex);

	m_bodyTexture = textureSrc;
}

_SAsynchModel::_SAsynchModel():
	MP_STRING_INIT(partName),
	MP_STRING_INIT(partSrc)
{
	asynch = NULL;
}

_SAsynchModel::_SAsynchModel(const _SAsynchModel& other):
	MP_STRING_INIT(partName),
	MP_STRING_INIT(partSrc)
{
	partName = other.partName;
	partSrc = other.partSrc;
	asynch = other.asynch;
}

void CAvatarObject::OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, unsigned char/* percent*/)
{
}

void CAvatarObject::OnAsynchResourceLoaded(IAsynchResource* asynch)
{
	std::vector<SAsynchModel>::iterator it = m_asynchLoadedModels.begin();
	std::vector<SAsynchModel>::iterator itEnd = m_asynchLoadedModels.end();

	for ( ; it != itEnd; it++)
	if (((*it).asynch->GetFileName()) && (asynch->GetFileName()))
	if (strcmp((*it).asynch->GetFileName(), asynch->GetFileName()) == 0)
	{
		SaveAvatarObjPartsInfo((*it).partName, (*it).partSrc, asynch->GetIFile());
		m_asynchLoadedModels.erase(it);		
		return;
	}
}

void CAvatarObject::OnAsynchResourceError(IAsynchResource* /*asynch*/)
{
}

void CAvatarObject::SaveAvatarObjPartsInfo(const std::string& partName, const std::string& partSrc, ifile* asynchFile)
{
	void* data = NULL;
	unsigned long size = 0;

	// временно, пока асинхронная загрузка не используется
	// фактически надо все обращения к m_asynchLoadedModels оборачивать в критическую секцию
	std::string newPath = partSrc;
	StringReplace(newPath, ".pck", ".xml");
	StringReplace(newPath, ":geometry", "ui:resources");

	if (!asynchFile)
	{
		ifile* file = g->rl->GetResMemFile(newPath.c_str());
	
		if (!file)
		{		
			/*std::string newPath = partSrc;
			StringReplace(newPath, "ui:resources\\", ":geometry\\");
			StringReplace(newPath, "ui:resources/", ":geometry/");
			StringReplace(newPath, ".xml", ".pck");
			file = g->rl->GetResMemFile(newPath.c_str());
			if (!file)
			{
				IAsynchResource* asynch = g->rl->GetAsynchResMan()->GetAsynchResource(newPath.c_str(), true, RES_TYPE_MESH, this, g->rl, NULL, NULL);
				if (asynch)
				{
					SAsynchModel tmp;
					tmp.partName = partName;
					tmp.partSrc = newPath;
					tmp.asynch = asynch;
					m_asynchLoadedModels.push_back(tmp);
				}
			}*/
		
			return;
		}
	}
	else 
	{
		size = asynchFile->get_file_size();
		data = MP_NEW_ARR(unsigned char, size);
		asynchFile->read((BYTE *)data, size);		
	}

	if (GetIndexPart(partName) == -1)
	{
		if (!m_isClothesLoadAsynch)
		{
			if (m_avatarsObjPartsInfo.size() == 0)
			{
				if ((m_avatarParts[0].obj3d) && (m_avatarParts[0].obj3d->GetLODGroup()))
				{
					m_avatarParts[0].obj3d->GetLODGroup()->SetDrawState(false);
				}
			}

			MP_NEW_P4(tmp, SAvatarsObjNameAndSrc, newPath, partName, data, size);
			m_avatarsObjPartsInfo.push_back(tmp);
		}
		else
		{
			if (m_loadingCal3DObjectsTasks.size() == 0)
			{
				if ((m_avatarParts[0].obj3d) && (m_avatarParts[0].obj3d->GetLODGroup()))
				{
					m_avatarParts[0].obj3d->GetLODGroup()->SetDrawState(false);
				}
			}

			CLoadingCal3DObjectsTask* task = MP_NEW(CLoadingCal3DObjectsTask);

			task->SetParams(this, (m_avatarParts.size() > 0) ? m_avatarParts[0].obj3d : NULL, newPath, partName, data, size);

			m_loadingCal3DObjectsTasks.push_back(task);

			g->taskm.AddTask(task, MUST_BE_MULTITHREADED, PRIORITY_LOW);

		}
	}

	if (data)
	{
		MP_DELETE_ARR(data);
	}
}

void	CAvatarObject::SetTuneString(const std::string& tuneString)
{
	LoadAvatarFromXML(tuneString);
}

void	CAvatarObject::ClearTask(CLoadingCal3DObjectsTask* task)
{
	if ((task->IsPerformed()) || (g->taskm.RemoveTask(task)))
		{
			STransitionalInfo* info = task->GetTransitionalInfo();
			if (info)
			{
				if (info->newSAO)
				{
					info->newSAO->FreeResources();
					if (info->newSAO->GetID() == 0xFFFFFFFF)
					{
						MP_DELETE(info->newSAO);
					}
				}				
				info->Clear();				
			}
			MP_DELETE(task);			
		}
		else
		{
			task->DestroyResultOnEnd();
		}
}

void	CAvatarObject::ClearTasks()
{
	std::vector<CLoadingCal3DObjectsTask*>::iterator it = m_loadingCal3DObjectsTasks.begin();
	for (; it != m_loadingCal3DObjectsTasks.end(); it++)
	{
		CLoadingCal3DObjectsTask* task = *it;
		ClearTask(task);
	}

	m_loadingCal3DObjectsTasks.clear();	

	std::vector<SAvatarsObjNameAndSrc*>::iterator it3 = m_avatarsObjPartsInfo.begin();
	std::vector<SAvatarsObjNameAndSrc*>::iterator it3End = m_avatarsObjPartsInfo.end();
	for ( ; it3 != it3End; it3++)
	{
		MP_DELETE(*it3);
	}

	std::vector<SAsynchModel>::iterator it2 = m_asynchLoadedModels.begin();
	std::vector<SAsynchModel>::iterator it2End = m_asynchLoadedModels.end();
	for ( ; it2 != it2End; it2++)
	{
		g->rl->GetAsynchResMan()->RemoveLoadHandler((*it2).asynch, this/*, g->rl*/);
	}

	m_asynchLoadedModels.clear();
	m_avatarsObjPartsInfo.clear();
}

void CAvatarObject::LoadAvatarFromXML(std::string tuneString)
{
	m_bodyTexture = "";

	{
		std::vector<SCommandsParam*>::iterator it = m_rgbvColorCommands.begin();
		std::vector<SCommandsParam*>::iterator itEnd = m_rgbvColorCommands.end();

		for (;it!=itEnd; it++)
		{
			MP_DELETE(*it);
		}

		m_rgbvColorCommands.clear();

		m_changeTextureCommands.clear();
	}

	ClearTasks();

	CSkeletonAnimationObject* skeleton = (CSkeletonAnimationObject*)(m_obj3d->GetAnimation());

	std::string str = "";
	int found = std::string::npos;

	if (skeleton != NULL)
	{
		str = skeleton->GetPath();
		found  = str.find("body");
	}

	if (found != std::string::npos)
	{
		const char* path = str.c_str();

		if ( *(path + found + 4) != '\\' )
			m_headID = rtl_atoi(path+found+4);
	}

	StringReplace(tuneString, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>", "");	
	
	TiXmlDocument doc;
	doc.Parse(tuneString.c_str());

	TiXmlNode* xml = doc.FirstChild("avatar");
	if (!xml)
	{
		return;
	}

    if (xml->ToElement()->Attribute("head"))
	{
		ChangeHead(rtl_atoi(xml->ToElement()->Attribute("head")));
	}

	TiXmlNode* part = xml->FirstChild("part");
	
	while (part != NULL)
	{
		LoadAvatarPart(part);
		
		part = xml->IterateChildren("part", part);
	}
	
	TiXmlNode* params = xml->FirstChild("params");

	while (params != NULL)
	{
		LoadParams(params, "common");
		
		params = xml->IterateChildren("params", params);
	}

	TiXmlNode* texture = xml->FirstChild("texture");

	USES_CONVERSION;
	while (texture != NULL)
	{
		char* _src = (char *)texture->ToElement()->Attribute("src");
		char* mesh = (char *)texture->ToElement()->Attribute("mesh");
		if (_src && mesh)
		{
			std::wstring src = (_src[0] == ':') ? L"" : L"ui:resources\\avatars\\editor\\";			
			src += CharStringToWString(_src);
			
			/*if (!m_isMyAvatar)
			{*/
				m_changeTextureCommands.insert(MP_MAP<MP_STRING, MP_WSTRING>::value_type(MAKE_MP_STRING(mesh), MAKE_MP_WSTRING(src)));	
			/*}
			else
			{
				ChangeTexture(mesh, src);
			}*/
		}
		
		texture = xml->IterateChildren("texture", texture);
	}

	if (xml->ToElement()->Attribute("body_tex"))
	{
		std::string path = xml->ToElement()->Attribute("body_tex");
		if (path.size() > 0)
		{
			path = "ui:resources\\avatars\\editor\\" + path;
			HandleChangeBodyTexture(path);		
		}
	}
}

void	CAvatarObject::LoadAvatarPart(TiXmlNode* xml)
{
	if (!xml)
	{
		return;
	}

	std::string paramID, templateID, partID, src;
	std::string attrName;
	std::string attrValue;

	if (xml->ToElement()->Attribute("param_id"))
	{
		paramID = xml->ToElement()->Attribute("param_id");
	}

	if (xml->ToElement()->Attribute("template_id"))
	{
		templateID = xml->ToElement()->Attribute("template_id");
	}

	if (xml->ToElement()->Attribute("part_id"))
	{
		partID = xml->ToElement()->Attribute("part_id");
	}

	if (xml->ToElement()->Attribute("src"))
	{
		src = xml->ToElement()->Attribute("src");
	}

	if (src != "")
	{
		int pos = src.find(":geometry");
		if (pos == -1)
		{
			pos = src.find("ui:resources\\avatars\\editor\\");
			if (pos == -1)
			{
				pos = src.find("ui:resources/avatars/editor/");
				// если basePath у src отсутсвует добавляем его
				if (pos == -1)
					src = "ui:resources\\avatars\\editor\\"+src;
			}
		}
	}

	std::string	partTarget = paramID;
	partTarget += ".";
	partTarget += templateID;
	partTarget += ".";
	partTarget += partID;

	if (m_isMyAvatar)
	{
		HandleLoadMesh(partTarget, src);
	}
	else
	{
		SaveAvatarObjPartsInfo(partTarget, src);
	}

	TiXmlNode* param = xml->FirstChild("params");

	while (param != NULL)
	{
		LoadParams(param, partTarget);

		param = xml->IterateChildren("params", param);
	}
}

void	CAvatarObject::LoadParams(TiXmlNode* xml, const std::string& /*partTarget*/)
{
	TiXmlNode* param = xml->FirstChild("param");

	while (param != NULL)
	{
		std::string commandID, commandPath, commandParam, commandValue;

		if (param->ToElement()->Attribute("command"))
		{
			commandID = param->ToElement()->Attribute("command");
		}

		if (param->ToElement()->Attribute("part"))
		{
			commandPath = param->ToElement()->Attribute("part");
		}

		if (param->ToElement()->Attribute("param"))
		{
			commandParam = param->ToElement()->Attribute("param");
		}

		if (param->ToElement()->Attribute("value"))
		{
			commandValue = param->ToElement()->Attribute("value");
		}

		if (("set_body_fatness" == commandID) || ("set_body_height" == commandID) || 
			("set_alpharef" == commandID) || ("set_diffuse_color" == commandID)	|| ("interpolate_texture" == commandID)
			|| ("set_rgbv_color" == commandID) || ("set_alpha_value" == commandID) || ("set_blend_value" == commandID))
		{
			if ("set_rgbv_color" == commandID)
			{
				MP_NEW_P4(tmp, SCommandsParam, commandID, commandPath, commandParam, commandValue);
				m_rgbvColorCommands.push_back(tmp);
			}
			else if ("set_blend_value" == commandID)
			{
				MP_NEW_P4(tmp, SCommandsParam, commandID, commandPath, commandParam, commandValue);
				m_blendCommands.push_back(tmp);
			}
			else
			{
				HandleSetParameterSafely(commandID, commandPath, commandParam, commandValue);
			}
		}
				
		param = xml->IterateChildren("param", param);
	}
}

std::string	CAvatarObject::GetTuneString()
{
	TiXmlDocument* doc = MP_NEW(TiXmlDocument);
	
	C3DObject* avatar = m_obj3d;
	
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "");	
	doc->LinkEndChild(decl);

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)avatar->GetAnimation();
	
	TiXmlElement* child = new TiXmlElement("avatar");	
	doc->LinkEndChild(child);
	child->SetAttribute("name", sao->GetPath().c_str());
	child->SetAttribute("head", m_compositeTextureManager ? m_compositeTextureManager->GetHeadID() : 1);
	child->SetAttribute("body_tex", m_bodyTexture.c_str());

	std::vector<SAvatarObjPart>::iterator it = m_avatarParts.begin(), end = m_avatarParts.end();
	for (; it != end; it++)
	{
		SaveAvatarPartToXMLDoc(child, (*it));
	}
	
	SaveAvatarPartParametersToXMLDoc(child, m_commonParameters);
	SaveChangedTextures(child);
	
	std::string tuneString = "";
	TiXmlPrinter printer;
	printer.SetStreamPrinting();
	doc->Accept(&printer);
	tuneString = printer.CStr();

	MP_DELETE(doc);
		
	return tuneString;
}

void CAvatarObject::SaveChangedTextures(TiXmlNode* node)
{
	MP_MAP<MP_STRING, MP_WSTRING>::iterator it = m_newTexturesMap.begin();
	MP_MAP<MP_STRING, MP_WSTRING>::iterator itEnd = m_newTexturesMap.end();

	USES_CONVERSION;
	for ( ; it != itEnd; it++)
	{
		TiXmlElement* partChild = new TiXmlElement("texture");	
		node->LinkEndChild(partChild);
		SaveAttribute(partChild, "mesh", (*it).first.c_str());
		SaveAttribute(partChild, "src", WStringToString((*it).second));
	}

	
}

void CAvatarObject::SaveAvatarPartToXMLDoc(TiXmlNode* node, SAvatarObjPart& avatarPart)
{
	TiXmlElement* partChild = new TiXmlElement("part");	
	node->LinkEndChild(partChild);
	std::string	target = avatarPart.objName;
	std::string	paramID;	
	{ 
		int index = target.find("."); 
		if (index != -1)
		{ 
			paramID = target.substr(0, index); 
			target = (target.c_str() + index + 1);
		}
	}
	std::string	templateID;
	{ 
		int index = target.find("."); 
		if (index != -1)
		{ 
			templateID = target.substr(0, index); 
			target = (target.c_str() + index + 1);
		}
	}
	std::string	partID = target;
	if (paramID.size() != 0)	
	{
		SaveAttribute(partChild, "param_id", paramID);
	}
	if (templateID.size() != 0)	
	{
		SaveAttribute(partChild, "template_id", templateID);
	}
	if (partID.size() != 0)	
	{
		SaveAttribute(partChild, "part_id", partID);
	}
	if (avatarPart.objSrc.size() != 0)	
	{
		SaveAttribute(partChild, "src", avatarPart.objSrc.c_str());
	}

	SaveAvatarPartParametersToXMLDoc( partChild, avatarPart);
}

void CAvatarObject::SaveAvatarPartParametersToXMLDoc(TiXmlNode* node, SAvatarObjPart& avatarPart)
{
	if (!avatarPart.parameters.empty() )	//	если у меша есть бленды то 
	{
		TiXmlElement* params = new TiXmlElement("params");		
		node->LinkEndChild(params);
		MP_MAP<MP_STRING, SAvatarParam>::iterator	it = avatarPart.parameters.begin(),
			end = avatarPart.parameters.end();
		for (; it!=end; it++)
		{
			TiXmlElement* paramsChild = new TiXmlElement("param");						
			params->LinkEndChild(paramsChild);
			SAvatarParam param = it->second;
			SaveAttribute(paramsChild, "command", param.commandID.c_str());
			SaveAttribute(paramsChild, "part", param.commandPath.c_str());
			SaveAttribute(paramsChild, "param", param.commandParam.c_str());
			SaveAttribute(paramsChild, "value", param.commandValue.c_str());
		}
	}
}

void	CAvatarObject::ExportTuneString(std::string varName)
{
	std::string tuneString = GetTuneString();
	if (tuneString.size() == 0)	
	{
		return;
	}
	g->lw.WriteLn("Export init data: ", tuneString);
	// убираем basePath тк он общий для всех src	
	StringReplace(tuneString, "ui:resources\\avatars\\editor\\", "");
	sSet(varName.c_str(), tuneString.c_str());
}

void CAvatarObject::CheckDressAndSuperDress()
{
	if (CheckTemplateLoad("dress"))
	{
		dSet("isDress", true);
	}
	else
	{
		dSet("isDress", false);
	}

	if (CheckTemplateLoad("super_dress"))
	{
		dSet("isSuperDress", true);
	}
	else
	{
		dSet("isSuperDress", false);
	}
}

void	CAvatarObject::ImportTuneString(std::string tuneString)
{
	if (tuneString.size() == 0)	
	{
		return;
	}

	SetTuneString(tuneString);

	CheckDressAndSuperDress();
}

bool CAvatarObject::IsAvatarLoaded()
{
	if (m_avatarsObjPartsInfo.size()!=0)
	{
		g->lw.WriteLn("IsAvatarLoaded: m_avatarsObjPartsInfo.size()!=0 ");
		return false;
	}

	if (m_loadingCal3DObjectsTasks.size()!=0)
	{	
		g->lw.WriteLn("IsAvatarLoaded: m_loadingCal3DObjectsTasks.size()!=0 ");
		return false;
	}

	std::vector<SAvatarObjPart>::iterator it = m_avatarParts.begin();
	std::vector<SAvatarObjPart>::iterator itEnd = m_avatarParts.end();

	for (; it!=itEnd; it++)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)(*it).obj3d->GetAnimation();

		if (!sao)
		{
			g->lw.WriteLn("IsAvatarLoaded: !sao ");
			return false;
		}

		CModel* model = sao->GetModel();

		if (!model)
		{
			g->lw.WriteLn("IsAvatarLoaded: !model ");
			return false;
		}

		if ( model->GetVertexCount() ==  0)
		{
			g->lw.WriteLn("IsAvatarLoaded: model->GetVertexCount() ==  0");
			return false;
		}

		for (int i = 0; i < model->GetTextureCount(); i++)
		{
			unsigned int textureID = model->GetTextureNumber(i);

			CTexture* texture = g->tm.GetObjectPointer(textureID);

			if (!texture)
			{	
				g->lw.WriteLn("IsAvatarLoaded: !texture");
				return false;
			}

			if (texture->GetLoadedStatus() == TEXTURE_NOT_LOADED_YET) 
			{
				g->lw.WriteLn("IsAvatarLoaded: texture->GetLoadedStatus() = " , texture->GetLoadedStatus());
				return false;
			}
 		}
	}	
		return true;	
}		
