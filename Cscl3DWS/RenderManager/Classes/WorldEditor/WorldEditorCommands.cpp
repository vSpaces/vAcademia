
#include "StdAfx.h"
#include <math.h>
#include "UserData.h"
#include "WorldEditor.h"
#include "HelpFunctions.h"
#include "RMContext.h"
#include "../../ObjectsCreator.h"

#define DEF_MESSAGE_START	void CWorldEditor::ProcessCommandById(int cmdID, CDataStorage* commandData) { if (cmdID != 0 && cmdID != 1)	{		switch(cmdID)		{
#define DEF_MESSAGE_HANDLER(x, y) case CMD_##x: { ##y(commandData); } break;
#define DEF_MESSAGE_END default: break; } } }

	DEF_MESSAGE_START
		DEF_MESSAGE_HANDLER(DELETE, OnDelete);
		DEF_MESSAGE_HANDLER(UPDATE_LODS, OnUpdateLods);
		DEF_MESSAGE_HANDLER(SELECT, OnSelect);
		DEF_MESSAGE_HANDLER(STAT, OnStat);
		DEF_MESSAGE_HANDLER(FREEZE, OnFreeze);						
		DEF_MESSAGE_HANDLER(SHOW_COLLISIONS, OnShowCollisions);
		DEF_MESSAGE_HANDLER(SHOW_INVISIBLE_COLLISIONS, OnShowInvisibleCollisions);
		DEF_MESSAGE_HANDLER(SHOW_NEAREST_PATHES, OnShowNearestPathes);
		DEF_MESSAGE_HANDLER(SWITCH_OFF_PATHES, OnSwitchOffPathes);
		DEF_MESSAGE_HANDLER(ENABLE_WIREFRAME, OnEnableWireframe);
		DEF_MESSAGE_HANDLER(SELECT_SAME, OnSelectSame);
		DEF_MESSAGE_HANDLER(SELECT_MASK, OnSelectByMask);
		DEF_MESSAGE_HANDLER(RENAME, OnRename);
		DEF_MESSAGE_HANDLER(GET_MODEL_INFO, OnGetModelInfo); 
		DEF_MESSAGE_HANDLER(MOVE, OnMove);			
		DEF_MESSAGE_HANDLER(ROTATE, OnRotate);
		DEF_MESSAGE_HANDLER(MATERIAL, OnChangeMaterial);
		DEF_MESSAGE_HANDLER(SCALE, OnScale);
		DEF_MESSAGE_HANDLER(SHADOW, OnShadow);
		DEF_MESSAGE_HANDLER(CHANGE_COLLISION, OnChangeCollision);
		DEF_MESSAGE_HANDLER(OBJECT_COPY, OnObjectCopy);
		DEF_MESSAGE_HANDLER(OBJECT_CREATE, OnObjectCreate);
		DEF_MESSAGE_HANDLER(FREE_CAMERA, OnFreeCamera);
		DEF_MESSAGE_HANDLER(REFRESH, OnRefresh);
		DEF_MESSAGE_HANDLER(TELEPORT, OnTeleportToNextActiveObject);
		DEF_MESSAGE_HANDLER(DRAGMODE, OnDragMode);
		DEF_MESSAGE_HANDLER(CLASS, OnClass);
		DEF_MESSAGE_HANDLER(PARAMS, OnParams);
		DEF_MESSAGE_HANDLER(LOCK, OnLock);	
		DEF_MESSAGE_HANDLER(SET_GROUP, OnGroup);	
		DEF_MESSAGE_HANDLER(CHANGE_GROUP, OnChangeGroup);	
		DEF_MESSAGE_HANDLER(HELLO, OnHello);
		DEF_MESSAGE_HANDLER(TELEPORT_BY_COORDS, OnTeleportByCoordsToObject);
	DEF_MESSAGE_END

void CWorldEditor::OnDelete(CDataStorage* deletingData)
{
	// команда удаления
	short objectsCount = 0;
	std::string objectName = "";

	if (!deletingData->Read(objectsCount) || objectsCount == 0)
	{
		return;
	}

	CDataStorage* selectingData = NULL;

	MP_NEW_V(selectingData, CDataStorage, 10);
	byte selectingCmd = CMD_SELECT;
	selectingData->Add(selectingCmd);

	C3DObject* obj = NULL;
	std::vector <C3DObject*> objects;
	for (int objectIndex = 0; objectIndex < objectsCount; objectIndex ++)
	{
		if (!deletingData->Read(objectName))
		{
			MP_DELETE(selectingData);
			return;
		}
			
		obj = GetObjectByName(objectName);
		if (obj != NULL)
		{
			obj->SetVisible(!(obj->IsVisible()));

			if ((int)(obj->GetUserData(USER_DATA_LEVEL)) != LEVEL_NO_COLLISION)
			{
				CLODGroup* objectGroup = obj->GetLODGroup();
				if (obj->IsVisible())
				{
					Sleep(1);
					objectGroup->ChangeCollisions(obj, true);
				}
				else
				{
					objectGroup->ChangeCollisions(obj, false);
				}
			}

			if (obj->IsVisible())
			{
				obj->SetBoundingBoxVisible(true);
				objects.push_back(obj);
			}
		}
	}

	m_activeObjects.Deactivate();

	objectsCount = (short)objects.size();
	selectingData->Add(objectsCount);
	for (int objIndex = 0; objIndex < objectsCount; objIndex ++)
	{
		C3DObject* objct = objects[objIndex];
		m_activeObjects.AddObject(objct);
		std::wstring objctName = objct->GetName();
		selectingData->Add(objctName);
	}

	if (objectsCount > 1)
	{
		byte selectFlag = 0;
		selectingData->Add(selectFlag);
		m_commandsData.AddOutCommand(selectingData);
	}
	else if (objectsCount == 1 && obj->IsVisible())
	{
		AddObjectInfo(selectingData, obj, false);
	}
	else
	{
		MP_DELETE(selectingData);
	}
}

void CWorldEditor::OnUpdateLods(CDataStorage* data)
{
	std::string objectName = "";
	std::string url = "";

	if (!data->Read(objectName))
	{
		MP_DELETE(data);
		return;
	}
			
	if (!data->Read(url))
	{
		MP_DELETE(data);
		return;
	}

	int objectID = g->o3dm.GetObjectNumber(objectName);
	if (objectID != -1)
	{
		StringReplace(url, "\\\\", "\\");
		ReloadLODGroup(g->o3dm.GetObjectPointer(objectID), url);
	}
}

void CWorldEditor::OnSelect(CDataStorage* selectingData)
{
	// команда выделения
	short objectsCount = 0;
	std::string objectName = "";

	if (!selectingData->Read(objectsCount) || objectsCount == 0)
		return;

	m_activeObjects.Deactivate();

	CDataStorage* selectData = NULL;

	MP_NEW_V(selectData, CDataStorage, 10);
	byte selectCmd = CMD_SELECT;
	selectData->Add(selectCmd);

	C3DObject* obj = NULL;
	std::vector <C3DObject*> objects;
	
	for (int objectIndex = 0; objectIndex < objectsCount; objectIndex ++)
	{
		if (!selectingData->Read(objectName))
		{
			MP_DELETE(selectingData);
			return;
		}

		if (objectName == "")
		{
			break;
		}

		obj = GetObjectByName(objectName);

		if (obj != NULL)
		{
			if (obj->IsVisible())
			{
				obj->SetBoundingBoxVisible(true);
				objects.push_back(obj);
			}
		}
	}

	objectsCount = (short)objects.size();
	if (objectsCount > 0)
	{
		selectData->Add(objectsCount);
		for (int objIndex = 0; objIndex < objectsCount; objIndex ++)
		{
			C3DObject* objct = objects[objIndex];
			std::wstring objctName = objct->GetName();
			selectData->Add(objctName);
			m_activeObjects.AddObject(objct);
		}

		if ((objectsCount > 1) && (m_dragType !=4))
		{
			byte selectFlag = 0;
			selectData->Add(selectFlag);
			m_commandsData.AddOutCommand(selectData);
		}
		else if (objectsCount == 1)
		{
			AddObjectInfo(selectData, obj, false);
		}
	}
	else
	{
		MP_DELETE(selectData);
	}
}

void CWorldEditor::OnStat(CDataStorage* /*commandData*/)
{
	m_statsNeed = true;
}

void CWorldEditor::OnFreeze(CDataStorage* freezingData)
{
	byte freezState;

	if (freezingData->Read(freezState))
	{
		CComString functionName;
		if (freezState == 1)
		{
			functionName = "scene.disableMovementMyAvatar()";
			StopAvatar();
		}
		else
		{				
			functionName = "scene.enableMovementMyAvatar()";
		}

		if (m_mapManager != NULL)
		{
			m_mapManager->ExecJS(functionName);
		}				
	}
}

void CWorldEditor::OnShowCollisions(CDataStorage* scData)
{
	byte scState;

	if (scData->Read(scState))
	{
		g->rs.SetBool(DEBUG_PHYSICS_OBJECTS, (scState == 1));				
	}
}

void CWorldEditor::OnShowInvisibleCollisions(CDataStorage* scData)
{
	byte scState;

	if (scData->Read(scState))
	{
		std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
		std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		if ((int)(*it)->GetUserData(USER_DATA_LEVEL) == LEVEL_CAMERA_COLLISION_ONLY)
		{
			(*it)->SetVisible(scState == 1);
		}
	}
}

void CWorldEditor::OnShowNearestPathes(CDataStorage* spData)
{
	byte spState;

	if (spData->Read(spState))
	{
		g->pf.SetVisible(spState);
	}
}

void CWorldEditor::OnSwitchOffPathes(CDataStorage* pfData)
{
	std::string objectName = "";
	if (!pfData->Read(objectName))
	{
		return;
	}

	int objectID = g->o3dm.GetObjectNumber(objectName);

	C3DObject* obj3d = GetObjectByName(objectName);

	if (obj3d == NULL)
	{
		return;
	}

	int pfState;

	if (pfData->Read(pfState))
	{
		if (pfState == 1)
		{
			if (obj3d->IsNeedToSwitchOffPathes())
				return;

			g->pf.Delete3DObjectsPath(objectID);
			obj3d->SetNeedSwitchOffPathes(true);

		}
		else
		{	
            byte* pathBytes = MP_NEW_ARR(byte, pfData->GetSize());

			int i = 0;

			while(pfData->Read(pathBytes[i]))
			{
				i++;
			}

			if ( i!=0)
			{
				int modelID = obj3d->GetLODGroup()->GetLodLevel(0)->GetModelID();

				if (!obj3d->IsNeedToSwitchOffPathes()) 
					return;

				obj3d->SetNeedSwitchOffPathes(false);
				g->pf.OnPropertyBlockLoaded(pathBytes, i , "pathlines.bin", modelID);
				g->pf.OnObjectCreated(objectID, modelID, true);
				
			}
		}

	}
}

void CWorldEditor::OnEnableWireframe(CDataStorage* scData)
{
	byte scState;

	if (scData->Read(scState))
	{
		if (g->scm.GetActiveScene())
		{
			g->scm.GetActiveScene()->SetWireframeMode(scState == 1);
		}
	}
}

void CWorldEditor::OnSelectSame(CDataStorage* selectingData)
{
	// команда выделения копий объекта
	std::string objectName = "";
	if (!selectingData->Read(objectName))
	{
		return;
	}

	C3DObject* obj = GetObjectByName(objectName);
	if (obj == NULL)
	{
		return;
	}
	
	m_activeObjects.Deactivate();
	CLODGroup* objectGroup = obj->GetLODGroup();

	std::vector<C3DObject*> sameObjects = objectGroup->GetObjects();
	std::vector<C3DObject*>::iterator it = sameObjects.begin();
	std::vector<C3DObject*>::iterator itEnd = sameObjects.end();

	MP_NEW_P(sameSelectData, CDataStorage, 100);	
	byte sameSelectCommand = CMD_SELECT;
	sameSelectData->Add(sameSelectCommand);

	std::vector<C3DObject*> objects;
	for ( ; it != itEnd; it++)
	{
		if ((*it)->IsVisible())
		{
			objects.push_back(*it);
		}
	}

	short count = (short)objects.size();
	sameSelectData->Add(count);

	it = objects.begin();
	itEnd = objects.end();
	USES_CONVERSION;
	for ( ; it != itEnd; it++)
	{
#pragma warning(push)
#pragma warning(disable : 4239)
		(*it)->SetBoundingBoxVisible(true);
		m_activeObjects.AddObject(*it);
		sameSelectData->Add(std::string( W2A((*it)->GetName())));
#pragma warning(pop)
	}

	if (count > 1)
	{
		byte selectFlag = 1;
		sameSelectData->Add(selectFlag);

		m_commandsData.AddOutCommand(sameSelectData);
	}
	else if (count == 1)
	{
		AddObjectInfo(sameSelectData, obj, false);
	}
	else
	{
		MP_DELETE(sameSelectData);
	}
}

void CWorldEditor::OnSelectByMask(CDataStorage* selectingData)
{
	std::string objMask = "";

	if (!selectingData->Read(objMask))
	{
		return;
	}

	MP_NEW_P(selectMaskData, CDataStorage, 10);
	byte selectCommand = CMD_SELECT;
	selectMaskData->Add(selectCommand);

	std::vector<C3DObject*> objects;

	USES_CONVERSION;
	for (int i = 0; i < g->o3dm.GetCount(); i++)
	{
		C3DObject* obj = g->o3dm.GetObjectPointer(i);
		if (obj == NULL)
		{
			continue;
		}
		std::string objName = W2A(obj->GetName());

		if (WildCompare(objMask.c_str(), objName.c_str()))
		{
			if (obj->IsVisible())// && obj->GetAnimation() == NULL)
			{
				objects.push_back(obj);						
			}
		}
	}

	m_activeObjects.Deactivate();

	short count = (short)objects.size();
	selectMaskData->Add(count);

	std::vector<C3DObject*>::iterator it = objects.begin();
	std::vector<C3DObject*>::iterator itEnd = objects.end();

	for ( ; it != itEnd; it++)
	{
#pragma warning(push)
#pragma warning(disable : 4239)
		selectMaskData->Add(std::string( W2A((*it)->GetName())));
		(*it)->SetBoundingBoxVisible(true);
		m_activeObjects.AddObject((*it));
#pragma warning(pop)
	}

	if (count > 1)
	{
		byte selectFlag = 1;
		selectMaskData->Add(selectFlag);

		m_commandsData.AddOutCommand(selectMaskData);
	}
	else if (count == 1)
	{
		AddObjectInfo(selectMaskData, m_activeObjects.GetObject(0));
	}
	else
	{
		MP_DELETE(selectMaskData);
	}
}

void CWorldEditor::OnRename(CDataStorage* renamingData)
{
	// команда переименовывания
	std::string objectName = "";
	if (!renamingData->Read(objectName))
	{
		return;
	}

	std::string objectNewName = "";
	if (!renamingData->Read(objectNewName))
	{
		return;
	}

	C3DObject* obj = GetObjectByName(objectNewName);
	if (obj != NULL)
	{
		// объект с таким именем существует
		// выделим его
		m_activeObjects.Deactivate();
		obj->SetBoundingBoxVisible(true);
		m_activeObjects.AddObject(obj);
		AddObjectInfo(GetSingleObjectDataHeader(obj), obj, true);
	}
	else
	{
		// объекта с новым именем нет, переменуем
		int objectID = g->o3dm.GetObjectNumber(objectName);

		if (objectID != ERR_OBJECT_NOT_FOUND)
		{			
			g->o3dm.RenameObject(objectID, objectNewName);

			MP_NEW_P(renameAnswer, CDataStorage, 10);
			byte answerCode = CMD_RENAME_ANSWER;

			renameAnswer->Add(answerCode);
			renameAnswer->Add(objectName);
			renameAnswer->Add(objectNewName);

			m_commandsData.AddOutCommand(renameAnswer);
		}
	}
}

void CWorldEditor::OnGetModelInfo(CDataStorage* data)
{
	std::string path;

	data->Read(path);

	path = ":geometry\\" + path;

	int modelID = g->mm.GetObjectNumber(path);

	if (modelID != -1)
	{
		CModel* model = g->mm.GetObjectPointer(modelID);
		std::string info = "Количество полигонов: ";
		info += IntToStr(model->GetFaceCount());
		info += "\n";
		info += "Количество вершин: ";
		info += IntToStr(model->GetVertexCount());
		info += "\n";
		info += "Количество текстур: ";
		info += IntToStr(model->GetTextureCount() - 1);

		MessageBox(g->ne.ghWnd, info.c_str(), "Информация", MB_OK);
	}
}

void CWorldEditor::OnMove(CDataStorage* movingData)
{
	float deltaX, deltaY, deltaZ;

	if (!(movingData->Read(deltaX) && movingData->Read(deltaY) && movingData->Read(deltaZ)))
	{
		return;
	}
	C3DObject* object = NULL;
	for (unsigned int activeObjIndex = 0; activeObjIndex < m_activeObjects.Count(); activeObjIndex++)
	{
		object =  m_activeObjects.GetObject(activeObjIndex);

		float x, y, z;
		object->GetCoords(&x, &y, &z);
		object->SetCoords(x+deltaX, y+deltaY, z+deltaZ);
		CLODGroup* objectGroup = object->GetLODGroup();
		objectGroup->UpdateBoundingBox(object);

		objectGroup->ChangeCollisions(object, false);
		Sleep(1);
		objectGroup->ChangeCollisions(object, true);
	}

	if (m_activeObjects.Count() == 1 && object != NULL)
	{
		AddObjectInfo(GetSingleObjectDataHeader(object), object, false);
	}
}

void CWorldEditor::OnRotate(CDataStorage* rotatingData)
{
	float angleX, angleY, angleZ;

	if (!(rotatingData->Read(angleX) && rotatingData->Read(angleY) && rotatingData->Read(angleZ)))
	{
		return;
	}

	CVector3D xVector = CVector3D(1, 0, 0);
	CVector3D yVector = CVector3D(0, 1, 0);
	CVector3D zVector = CVector3D(0, 0, 1);

	MP_NEW_P(rotateData, CDataStorage, 10);
	short objCount = (short)m_activeObjects.Count();

	byte rotateCmd = CMD_ROTATE_ANSWER;
	rotateData->Add(rotateCmd);
	rotateData->Add(angleX);
	rotateData->Add(angleY);
	rotateData->Add(angleZ);
	rotateData->Add(objCount);

	for (int activeObjIndex = 0; activeObjIndex < objCount; activeObjIndex++)
	{
		C3DObject* object =  m_activeObjects.GetObject(activeObjIndex);

		CQuaternion objQuat = object->GetRotation()->GetAsDXQuaternion();

		if ((objQuat.x == 0.0f) && (objQuat.y == 0.0f) && (objQuat.z == 0.0f) && (objQuat.w == 0.0f))
		{
			objQuat.z = 1.0f;
		}

		CQuaternion xQuat = CQuaternion(xVector, (float)((angleX * M_PI) / 180.0f));
		objQuat *= xQuat;

		CQuaternion yQuat = CQuaternion(yVector, (float)((angleY * M_PI) / 180.0f));
		objQuat *= yQuat;

		CQuaternion zQuat = CQuaternion(zVector, (float)((angleZ * M_PI) / 180.0f));			
		objQuat *= zQuat;

		CRotationPack rotation;
		rotation.SetAsDXQuaternion(objQuat.x, objQuat.y, objQuat.z, objQuat.w);
		object->SetRotation(&rotation);

		CLODGroup* objectGroup = object->GetLODGroup();
		objectGroup->UpdateBoundingBox(object);

		objectGroup->ChangeCollisions(object, false);
		Sleep(1);
		objectGroup->ChangeCollisions(object, true);

		std::wstring objName = object->GetName();
		rotateData->Add(objName);

		rotateData->Add(objQuat.x);
		rotateData->Add(objQuat.y);
		rotateData->Add(objQuat.z);
		rotateData->Add(objQuat.w);
	}

	m_commandsData.AddOutCommand(rotateData);
}

void CWorldEditor::OnChangeMaterial(CDataStorage* materialData)
{
	// флаг изменения параметра или создания нового материала.
	// 0 - изменение параметра, 1 -создание нового				
	byte updateFlag = 0;
	if (!materialData->Read(updateFlag))
	{
		return;
	}

	// имя материала
	std::string materialName = "";
	if (!materialData->Read(materialName))
	{
		return;
	}

	if (updateFlag == 0)
	{
		// тип параметра state или param
		std::string paramType = "";
		materialData->Read(paramType);

		// имя измененного параметра
		std::string paramName = "";
		materialData->Read(paramName);

		// новое значение параметра
		std::string paramValue = "";
		materialData->Read(paramValue);

		// тип значения (если тип state то пустая строка)
		std::string paramValueType = "";
		materialData->Read(paramValueType);

		ChangeMaterialParameter(materialName, paramName, paramValueType, paramValue, ("state" == paramType));
	}
	else if (updateFlag == 1)
	{
		// xml материала
		std::string materialDesc = "<material></material>";
		materialData->Read(materialDesc);

		AddNewMaterial(materialName, (char*)materialDesc.c_str());
	}
	else if (updateFlag == 2)
	{
		// новое состояние материала, on - off
		std::string materialState = "";
		materialData->Read(materialState);

		if (materialState == "on")
		{
			// содержимое включаемого материала
			std::string materialDesc = "";
			materialData->Read(materialDesc);

			AddNewMaterial(materialName, (char*)materialDesc.c_str());

		}
		else if (materialState == "off")
		{
			// выключение материала
			DisableMaterial(materialName);
		}
	}
}

void CWorldEditor::OnScale(CDataStorage* scalingData)
{
	float deltaX, deltaY, deltaZ;

	if (!(scalingData->Read(deltaX) && scalingData->Read(deltaY) && scalingData->Read(deltaZ)))
	{
		return;
	}
	C3DObject* object = NULL;
	for (unsigned int activeObjIndex = 0; activeObjIndex < m_activeObjects.Count(); activeObjIndex++)
	{
		object =  m_activeObjects.GetObject(activeObjIndex);
		CVector3D objScale = object->GetScale3D();

		object->SetScale(objScale.x + deltaX, objScale.y + deltaY, objScale.z + deltaZ);

		CLODGroup* objectGroup = object->GetLODGroup();
		objectGroup->UpdateBoundingBox(object);

		Sleep(1);
		objectGroup->ChangeCollisions(object, false);
		Sleep(1);
		objectGroup->ChangeCollisions(object, true);
	}

	if (m_activeObjects.Count() == 1 && object != NULL)
	{
		AddObjectInfo(GetSingleObjectDataHeader(object), object, false);
	}
}

void CWorldEditor::OnShadow(CDataStorage* shadowData)
{
	byte lodLevelIndex;
	if (!shadowData->Read(lodLevelIndex))
	{
		return;
	}

	byte shadowState;
	if (!shadowData->Read(shadowState))
	{
		return;
	}

	C3DObject* obj = m_activeObjects.GetObject(0);
	CLODGroup* objGroup = obj->GetLODGroup();

	if (shadowState == 0)
	{
		// выключение
		objGroup->SetShadowStatus(lodLevelIndex, 0);
	}
	else
	{
		// включение ??
		objGroup->SetShadowStatus(lodLevelIndex, 1);
		objGroup->OnChanged(0);
	}
}

void CWorldEditor::OnChangeCollision(CDataStorage* colData)
{
	unsigned char level;

	if (colData->Read(level))
	{
		for (unsigned int i = 0; i < m_activeObjects.Count(); i++)
		{
			C3DObject* obj = m_activeObjects.GetObject(i);
			obj->SetUserData(USER_DATA_LEVEL, (void *)level);
			obj->GetLODGroup()->ChangeCollisions(obj, false);
			if (level != 2)
			{
				obj->GetLODGroup()->ChangeCollisions(obj, true);
			}
		}
	}
}

void CWorldEditor::OnObjectCopy(CDataStorage* copyingData)
{
	// copy flag
	byte copyFlag = 0;
	if (!copyingData->Read(copyFlag))
	{
		return;
	}

	if (copyFlag == 1)
	{
		byte redoFlag = 0;
		if (!copyingData->Read(redoFlag))
		{
			return;
		}

		short activeObjCount = (short)m_activeObjects.Count();
		std::vector<C3DObject*> newObjects;

		std::vector<std::string> newNames;
		newNames.reserve(newNames.size());
		
		for (int i = 0; i < activeObjCount; i++)
		{
			std::string name;
			if (!copyingData->Read(name))
			{
				return;
			}
			newNames.push_back(name);
		}

		MP_NEW_P(createData, CDataStorage, 50);
		byte createCmd = CMD_OBJECT_COPY;
		createData->Add(createCmd);
		createData->Add(activeObjCount);
		createData->Add(redoFlag);

		C3DObject* obj = NULL;
		C3DObject* newObject = NULL;
		USES_CONVERSION;
		for (int objectIndex = 0; objectIndex < activeObjCount; objectIndex++)
		{			
			newObject = NULL;
			obj = m_activeObjects.GetObject(objectIndex);

			std::string url = W2A(obj->GetLODGroup()->GetName());
			int level = (int)obj->GetUserData(USER_DATA_LEVEL);
			std::wstring objName = obj->GetName();
			std::string newName = newNames[objectIndex];

			if (obj->GetUserData(USER_DATA_NRMOBJECT) == NULL)
			{
				if (!CreateObject3D(url, "", &newObject, newName, level))
				{
					return;
				}
			}
			else
			{
				if (!CreateObject3D(url, "", &newObject, newName, level))
				{
					return;
				}
				//mlMedia* objMedia = (mlMedia*)obj->GetUserData(USER_DATA_MLMEDIA);
				//objMedia->GetI3DObject()->
				//m_mapManager->CreateScriptedObject();
			}       		

			// copy info
			CVector3D objCoords = obj->GetCoords();
			CVector3D objScale = obj->GetScale3D();

			float deltaX = 0;
			float deltaY = 0;
			float deltaZ = obj->GetSize().z;

			objCoords.x += deltaX;
			objCoords.y += deltaY;
			objCoords.z += deltaZ;
			newObject->SetCoords(objCoords);
			newObject->SetScale(objScale);
			CRotationPack* objRot =  obj->GetRotation();
			CRotationPack newObjRot;
			newObjRot = *objRot;
			newObject->SetRotation(&newObjRot);

			newObject->GetLODGroup()->UpdateBoundingBox(newObject);
			newObject->SetBoundingBoxVisible(true);

			newObject->GetLODGroup()->ChangeCollisions(newObject, false);
			Sleep(1);
			newObject->GetLODGroup()->ChangeCollisions(newObject, true);

			newObjects.push_back(newObject);

			createData->Add(objName);
			createData->Add(newName);
			createData->Add(objCoords.x);
			createData->Add(objCoords.y);
			createData->Add(deltaZ);

		}

		m_commandsData.AddOutCommand(createData);

		m_activeObjects.Deactivate();
		for (unsigned int objIndex = 0; objIndex < newObjects.size(); objIndex ++)
		{
			m_activeObjects.AddObject(newObjects[objIndex]);
		}

		if (activeObjCount == 1 && newObject != NULL)
		{
			AddObjectInfo(GetSingleObjectDataHeader(newObject), newObject, false);
		}

	}
	else if (copyFlag == 2)
	{
		// команда удаления объектов при undo
		int objectsCount = m_activeObjects.Count();
		for (int objIndex = 0; objIndex < objectsCount; objIndex ++)
		{
			C3DObject* obj = m_activeObjects.GetObject(objIndex);

			int objID = g->o3dm.GetObjectNumber(obj->GetName());
			if (obj->GetLODGroup())
			{
				obj->GetLODGroup()->RemoveObject(obj);
			}
			g->o3dm.DeleteObject(objID);
		}
		m_activeObjects.Deactivate();

		C3DObject* obj = NULL;
		for (int objIndex = 0; objIndex < objectsCount; objIndex ++)
		{
			std::string objName = "";
			copyingData->Read(objName);

			 obj = GetObjectByName(objName);
			if (obj != NULL)
			{
				obj->SetBoundingBoxVisible(true);
				m_activeObjects.AddObject(obj);
			}
		}

		if (objectsCount == 1 && obj != NULL)
		{
			AddObjectInfo(GetSingleObjectDataHeader(obj), obj, false);
		}
		else if (objectsCount > 1)
		{
			MP_NEW_P(selectingCommand, CDataStorage, 10);
			byte selectCmdId = CMD_SELECT;
			selectingCommand->Add(selectCmdId);

			short objCount = (short)m_activeObjects.Count();
			selectingCommand->Add(objCount);
			for (unsigned int objIndex = 0; objIndex < m_activeObjects.Count(); objIndex ++)
			{
				std::wstring name = m_activeObjects.GetObject(objIndex)->GetName();
				selectingCommand->Add(name);
			}
			byte selectFlag = 0;
			selectingCommand->Add(selectFlag);

			m_commandsData.AddOutCommand(selectingCommand);
		}
	}
}

void CWorldEditor::OnObjectCreate(CDataStorage* creatingData)
{
	byte flag = 0;
	if (!creatingData->Read(flag))
	{
		return;
	}

	if (flag == 1 || flag == 3)
	{
		std::string objectName = "";
		creatingData->Read(objectName);

		std::string newObjectName = "";
		creatingData->Read(newObjectName);

		std::string objectUrl = "";
		creatingData->Read(objectUrl);
		objectUrl = ":geometry\\" + objectUrl;

		int objectLevel = 0;
		creatingData->Read(objectLevel);

		std::string objectClass = "";
		creatingData->Read(objectClass);

		std::string objectParams = "";
		creatingData->Read(objectParams);

		int objectLibId = 0;
		creatingData->Read(objectLibId);

		m_newObjectInfo.name = newObjectName;
		m_newObjectInfo.url = objectUrl;
		m_newObjectInfo.className = objectClass;
		m_newObjectInfo.level = objectLevel;
		m_newObjectInfo.params = objectParams;
		m_newObjectInfo.libId = objectLibId;

		if (flag == 1)
		{
			m_placeObjectByMouse = true;
			SetSystemCursor(CopyCursor(m_placeCursor), 32512); 
		}
		else
		{
			float x, y, z;
			creatingData->Read(x);
			creatingData->Read(y);
			creatingData->Read(z);

			CVector3D objectCoords(x, y, z);
			CreateNewObjectByMouse(&objectCoords, false);
		}		
	}
	else if (flag == 2)
	{
		std::string objectName = "";
		creatingData->Read(objectName);	

		C3DObject* obj = GetObjectByName(objectName);
		if (obj != NULL)
		{
			int objID = g->o3dm.GetObjectNumber(obj->GetName());
			if (obj->GetLODGroup())
			{
				obj->GetLODGroup()->RemoveObject(obj);
			}
			g->o3dm.DeleteObject(objID);
			m_activeObjects.Deactivate();
		}

		short objCount = 0;
		creatingData->Read(objCount);
		obj = NULL;
		for (int objIndex = 0; objIndex < objCount; objIndex++)
		{
			creatingData->Read(objectName);
			obj = GetObjectByName(objectName);
			if (obj != NULL)
			{
				obj->SetBoundingBoxVisible(true);
				m_activeObjects.AddObject(obj);
			}
		}

		if (objCount == 1 && obj != NULL)
		{			
			AddObjectInfo(GetSingleObjectDataHeader(obj), obj, false);
		}
		else if (objCount > 1)
		{
			MP_NEW_P(selectingCommand, CDataStorage, 10);
			byte selectCmdId = CMD_SELECT;
			selectingCommand->Add(selectCmdId);

			selectingCommand->Add(objCount);
			for (unsigned int objIndex = 0; objIndex < m_activeObjects.Count(); objIndex ++)
			{
				std::wstring name = m_activeObjects.GetObject(objIndex)->GetName();
				selectingCommand->Add(name);
			}
			byte selectFlag = 0;
			selectingCommand->Add(selectFlag);

			m_commandsData.AddOutCommand(selectingCommand);
		}
	}
}

void CWorldEditor::OnFreeCamera(CDataStorage* freeCamData)
{
	byte freeCamState = 0;

	if (freeCamData->Read(freeCamState))
	{
		if (freeCamState == 1)
		{
			// отвязать камеру
			m_freeCam = true;
			m_mapManager->ExecJS("scene.SetCameraMode( scene.CAMERA_MODE_FREE);");
		}
		else
		{	
			// привязать камеру
			m_freeCam = false;
			m_mapManager->ExecJS("scene.SetCameraMode( scene.CAMERA_MODE_THIRD_VIEW);");
		}		
	}
}

void CWorldEditor::OnRefresh(CDataStorage* /*commandData*/)
{
	m_needRefresh = true;
}

void CWorldEditor::OnTeleportToNextActiveObject(CDataStorage* /*commandData*/)
{
	StopAvatar();

	if (m_mapManager != NULL)
	{
		C3DObject* avatar = m_mapManager->GetAvatar();

		C3DObject* teleportObject = m_activeObjects.GetNextTeleportObject();

		if (teleportObject != NULL)
		{
			CVector3D objectCoords = teleportObject->GetCoords();
			CVector3D avatarCoords = avatar->GetCoords();

			avatarCoords.x = objectCoords.x - (teleportObject->GetSize().x / 2) - 100;
			avatarCoords.y = objectCoords.y;

			avatar->SetCoords(avatarCoords);
		}
	}
}

void CWorldEditor::OnTeleportByCoordsToObject(CDataStorage* commandData)
{
	StopAvatar();

	if (m_mapManager != NULL)
	{
		C3DObject* avatar = m_mapManager->GetAvatar();

		int x = 0, y = 0, z = 0;

		commandData->Read(x);
		commandData->Read(y);
		commandData->Read(z);

		CVector3D avatarCoords(x, y, z);

		avatar->SetCoords(avatarCoords);
	}
}

void CWorldEditor::OnDragMode(CDataStorage* dragModeData)
{
	int dragMode = 0;

	dragModeData->Read(dragMode);

	m_dragType = dragMode;

	g->pf.SetGoToSegmentMode(false);

	if (m_dragType != 11)
	{
		g->pf.RefreshSelectedSegmentPoints();
	}

	if (m_dragType == 6)
	{
		CVector3D zero(0,0,0);
		g->pf.SetCurrentPoint(&zero);

		if (g->pf.GetCurrentPoint() != CVector3D(0,0,0))
		{
			m_isNeedToMovePoint = true;
		}
		else 
		{
			m_isNeedToMovePoint = false;
		}
		
	}
	else if ((m_dragType == DRAG_TYPE_GENERATE_QUAD_PATH) || (m_dragType == DRAG_TYPE_GENERATE_CIRCLE_PATH) 
		|| (m_dragType == DRAG_TYPE_DELETE_PATH))
	{
		int objectID = g->o3dm.GetObjectNumber(m_activeObjects.GetObject(0)->GetName());
		C3DObject* object = g->o3dm.GetObjectPointer(objectID);
		int modelID = object->GetLODGroup()->GetLodLevel(0)->GetModelID();		

		std::vector<C3DObject *>& objects = object->GetLODGroup()->GetObjects();

		std::vector<C3DObject *>::iterator it = objects.begin();
		std::vector<C3DObject *>::iterator itEnd = objects.end();
		for ( ; it != itEnd; it++)
		{
			g->pf.Delete3DObjectsPath((*it)->GetID());
			if (m_dragType == DRAG_TYPE_GENERATE_QUAD_PATH)
			{
				g->pf.CreateQuadrPath((*it)->GetID(), modelID);
			}
			else if (m_dragType == DRAG_TYPE_GENERATE_CIRCLE_PATH)
			{
				g->pf.CreateCirclePath((*it)->GetID(), modelID);
			}
		}		
	}	
	else if (m_dragType == DRAG_TYPE_GO_SEGMENT)
	{
		g->pf.SetGoToSegmentMode(true);
	}

}

void CWorldEditor::OnClass(CDataStorage* /*classData*/)
{
	if (m_activeObjects.Count() == 1)
	{
		C3DObject* object = m_activeObjects.GetObject(0);
		AddObjectInfo(GetSingleObjectDataHeader(object), object, false);
	}
	else if (m_activeObjects.Count() > 1)
	{

	}
}

void CWorldEditor::OnParams(CDataStorage* /*classData*/)
{
	if (m_activeObjects.Count() == 1)
	{
		C3DObject* object = m_activeObjects.GetObject(0);
		AddObjectInfo(GetSingleObjectDataHeader(object), object, false);
	}
	else if (m_activeObjects.Count() > 1)
	{

	}
}

void CWorldEditor::OnLock(CDataStorage* /*lockData*/)
{
	if (m_activeObjects.Count() == 1)
	{
		C3DObject* object = m_activeObjects.GetObject(0);
		AddObjectInfo(GetSingleObjectDataHeader(object), object, false);
	}
	else if (m_activeObjects.Count() > 1)
	{

	}
}

void CWorldEditor::OnGroup(CDataStorage* groupData)
{
	std::string objectName = "";
	if (!groupData->Read(objectName))
	{
		MP_DELETE(groupData);
		return;
	}

	std::string groupName = "";
	if (!groupData->Read(groupName))
	{
		MP_DELETE(groupData);
		return;
	}

	int groupType = 0;
	if (!groupData->Read(groupType))
	{
		MP_DELETE(groupData);
		return;
	}

	byte isUndo = 0;
	if (!groupData->Read(isUndo))
	{
		MP_DELETE(groupData);
		return;
	}

	
	gRM->groupCtrl->OnGroup(objectName, groupName, groupType);  

	if (isUndo)
	{
		MP_NEW_P(groupCommand, CDataStorage, 50);
		byte groupCmdId = CMD_SET_GROUP;
		groupCommand->Add(groupCmdId);
		groupCommand->Add(objectName);
		groupCommand->Add(groupName);
		groupCommand->Add(isUndo);
		m_commandsData.AddOutCommand(groupCommand);
	}
}

void CWorldEditor::OnChangeGroup(CDataStorage* groupData)
{
	std::string groupName = "";
	if (!groupData->Read(groupName))
	{
		MP_DELETE(groupData);
		return;
	}

	int groupType = 0;
	if (!groupData->Read(groupType))
	{
		MP_DELETE(groupData);
		return;
	}

	CVector3D groupCoord;
	if (!(groupData->Read(groupCoord.x) && groupData->Read(groupCoord.y) && groupData->Read(groupCoord.z)))
	{
		MP_DELETE(groupData);
		return;
	}
	
	gRM->groupCtrl->OnChangeGroup(groupName,groupCoord);  

}

void CWorldEditor::OnHello(CDataStorage* /*data*/)
{
	MP_NEW_P(helloCommand, CDataStorage, 50);
	byte helloCmdId = CMD_HELLO;
	helloCommand->Add(helloCmdId);
	char _serverUrl[40];
	gRM->nrMan->GetContext()->mpApp->GetSettingFromIni("paths", "server_ip", &_serverUrl[0], 35);
	std::string serverURL = _serverUrl;
	helloCommand->Add(serverURL);
	m_commandsData.AddOutCommand(helloCommand);
}
