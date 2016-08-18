
#include "StdAfx.h"
#include "WorldEditor.h"
#include "UserData.h"
#include "HelpFunctions.h"
#include "../../ObjectsCreator.h"
#include "../nrmCharacter.h"
#include "RMContext.h"
#include "PathFinder.h"
#include "SimpleThreadStarter.h"

#define DRAG_X				10
#define DRAG_Y				11
#define DRAG_Z				12
#define MIN_DBLCLK_TIME		200

_SNewObjectInfo::_SNewObjectInfo():
	MP_STRING_INIT(name),
	MP_STRING_INIT(url),
	MP_STRING_INIT(className),
	MP_STRING_INIT(params)
{
	level = 0;
	libId = -1;
}

CWorldEditor::CWorldEditor():
	m_doSelectObjectUnderMouse(false),
	m_lastSelectedTextureID(-1),
	m_placeObjectByMouse(false),
	m_dragObjectByMouse(0),
	m_mButtonPress(false),
	m_lButtonPress(false),
	m_needRefresh(false),
	m_lastCameraTime(0),
	m_firstMouseDownClkTime(0),
	m_mouseDownXLClk(0),
	m_mouseDownYLClk(0),
	m_isFirstUpClk(false),
	m_isDoubleUpClk(false),
	m_firstMouseUpClkTime(0),
	m_mouseUpXLClk(0),
	m_mouseUpYLClk(0),
	m_isFirstDownClk(false),
	m_isDoubleDownClk(false),
	m_hPipeThread(NULL),
	m_isRunning(false),
	m_mapManager(NULL),
	m_statsNeed(false),
	m_freeCam(false),
	m_dragType(0),
	m_mouseX(-1),
	m_mouseY(-1),
	m_firstPoint(0,0,0),
	m_secondPoint(0,0,0),
	m_currentObjectID(-1),
	m_isNeedToMovePoint(false),
	m_isSelectSegment(false)
{
	m_dragCursor = CopyCursor(LoadCursor(NULL, IDC_SIZEALL));
	m_normalCursor = CopyCursor(LoadCursor(NULL, IDC_ARROW));
	m_placeCursor = CopyCursor(LoadCursor(NULL, IDC_CROSS));
	m_oldCursor = CopyCursor(GetCursor());
}

void CWorldEditor::OnChar(int keycode)
{
	MP_NEW_P(keyCommand, CDataStorage, 10);
	byte  keyCmdId = CMD_KEY_CODE;
	keyCommand->Add(keyCmdId);

	int editorKey = -1;
	if (keycode == 46 || keycode == 83 || keycode == 73 || keycode == 90 || keycode == 89 ||
		keycode == 67 || keycode == 77 || keycode == 9 || keycode == 70 ||  keycode == 'A' ||
		keycode == 'N' || keycode == 'R' || keycode =='L' ||
		((keycode >= 113) && (keycode <= 119 ) ))
	{
		editorKey = keycode;
	}

	if (editorKey != -1)
	{
		byte ctrl = 0;
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			ctrl = 1;
		}

		byte shift = 0;
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			shift = 1;
		}

		byte alt = 0;
		if (GetKeyState(VK_MENU) & 0x8000)
		{
			alt = 1;
		}

		if ((alt == 1 ) && (m_isSelectSegment) &&
			(keycode == 'A' || keycode == 'N' || keycode == 'R' || keycode =='L'))
		{
			if (keycode == 'A')
			{
				g->pf.SetTypeOfSelectSegment(ANY_ROUND_SEGMENT_TYPE);
			}
			else if (keycode == 'N')
			{
				g->pf.SetTypeOfSelectSegment(COMMON_ROUND_SEGMENT_TYPE);
			}
			else if (keycode == 'R')
			{
				g->pf.SetTypeOfSelectSegment(RIGHT_ROUND_SEGMENT_TYPE);
			}
			else if (keycode =='L')
			{
				g->pf.SetTypeOfSelectSegment(LEFT_ROUND_SEGMENT_TYPE);
			}

			int objectID = g->o3dm.GetObjectNumber(m_activeObjects.GetObject(0)->GetName());
			C3DObject* object = g->o3dm.GetObjectPointer(objectID);

			MP_NEW_P(addNewSegmentCommand, CDataStorage, 1000);

			AssemblePathesPackage(addNewSegmentCommand, object);

			m_commandsData.AddOutCommand(addNewSegmentCommand);

		}
		else
		{
			keyCommand->Add(editorKey);
			keyCommand->Add(ctrl);
			keyCommand->Add(alt);
			keyCommand->Add(shift);

			m_commandsData.AddOutCommand(keyCommand);
		}
	}
	else
	{
		MP_DELETE(keyCommand);
	}
}

void CWorldEditor::SetMButtonPress(bool enable, int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;

	if (enable)
	{	
		if (m_mButtonPress)
		{
			// кнопка мыши была отпущена за пределами окна
			m_oldCursor = CopyCursor(m_normalCursor);
		}
		else
		{
			m_oldCursor = CopyCursor(GetCursor());
		}		
		SetSystemCursor(CopyCursor(m_dragCursor), 32512); 
	}
	else
	{
		SetSystemCursor(CopyCursor(m_oldCursor), 32512);
	}

	m_mButtonPress = enable;
}

void CWorldEditor::SetLButtonUp(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;

	m_lButtonPress = false;
	m_firstMouseUpClkTime = g->ne.time;
	m_isFirstUpClk = true;
	m_mouseUpXLClk = m_mouseX;
	m_mouseUpYLClk = m_mouseY;

}

void CWorldEditor::UpdateCloneableObjectsPath(C3DObject* object)
{
	if (!object)
	{
		return;
	}

	std::vector<C3DObject *>& objects = object->GetLODGroup()->GetObjects();

	std::vector<C3DObject *>::iterator it = objects.begin();
	std::vector<C3DObject *>::iterator itEnd = objects.end();

	for ( ; it != itEnd; it++)
	if (*it != object)
	{
		g->pf.CopyPath(object->GetID(), (*it)->GetID());
	}
}

void CWorldEditor::SetLButtonUpAfterUpdate(int x, int y)
{	
	if (m_dragType == 6) // режим перемещения, отправляем весь путь объекта.
	{
		if  (m_isNeedToMovePoint)
		{
			if (!m_activeObjects.GetObject(0))
			{
				return;
			}

			int objectID  = g->o3dm.GetObjectNumber(m_activeObjects.GetObject(0)->GetName());
			C3DObject* object = g->o3dm.GetObjectPointer(objectID);

			MP_NEW_P(addNewSegmentCommand, CDataStorage, 500);

			AssemblePathesPackage(addNewSegmentCommand, object);

			m_commandsData.AddOutCommand(addNewSegmentCommand);

			UpdateCloneableObjectsPath(object);
		}

		m_lButtonPress = false;
	}	
}
void CWorldEditor::SetLButtonDown(int x, int y)
{
	if (!m_activeObjects.GetObject(0))
	{
		return;
	}

	m_mouseX = x;
	m_mouseY = y;

	if ((m_isFirstDownClk) && (g->ne.time - m_firstMouseDownClkTime < MIN_DBLCLK_TIME))
	{
		SetLMouseDblClick(x, y);
		m_isFirstDownClk = false;
		return;
	}
	
	m_firstMouseDownClkTime = g->ne.time;
	m_isFirstDownClk = true;
	m_mouseDownXLClk = m_mouseX;
	m_mouseDownYLClk = m_mouseY;
}

void CWorldEditor::SetLButtonDownAfterUpdate(int x, int y)
{
	if (!m_activeObjects.GetObject(0))
	{
		return;
	}

	m_mouseX = x;
	m_mouseY = y;

	if (m_dragType == 4)
	{
		std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
		CViewport* viewport = (*it)->GetViewport();

		int objectID = g->o3dm.GetObjectNumber(m_activeObjects.GetObject(0)->GetName());

		bool isObjectWithPath = false;

		//Клик левой только на уже существующие контрольные точки(IsPointInCurrent3DObjectsPath)/нет ни одной контрольной точки(!isObjectWithPath).
		if (!g->pf.IsPointInCurrent3DObjectsPath(m_mouseX, m_mouseY, viewport, objectID, &isObjectWithPath))
		{
			if (!isObjectWithPath)
			{
				m_firstPoint = viewport->GetSelected3DCoords();
			}
		}
		else 
		{
			m_firstPoint = g->pf.GetCurrentPoint();
		}
	}
	else if (m_dragType == 5)
	{
		std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
		CViewport* viewport = (*it)->GetViewport();

		int objectID = g->o3dm.GetObjectNumber(m_activeObjects.GetObject(0)->GetName());

		if (g->pf.DeleteOrSelect3DObjectsSegment(m_mouseX, m_mouseY, viewport))
		{
			C3DObject* object = g->o3dm.GetObjectPointer(objectID);

			MP_NEW_P(addNewSegmentCommand, CDataStorage, 1000);

			AssemblePathesPackage(addNewSegmentCommand, object);

			m_commandsData.AddOutCommand(addNewSegmentCommand);

			UpdateCloneableObjectsPath(object);
		}

	}
	else if ((m_dragType == 6) && (m_isNeedToMovePoint))
	{
		m_lButtonPress = true;
	}
	else if (m_dragType == 10)
	{
		std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
		CViewport* viewport = (*it)->GetViewport();

		CVector3D vBegin, vEnd; 

		if (g->pf.DeleteOrSelect3DObjectsSegment(m_mouseX, m_mouseY, viewport, &vBegin, &vEnd))
		{
			gRM->graphicDebug->GoToSegment(&vBegin, &vEnd);
		}
	}
	else if (m_dragType == 11)
	{
		std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
		CViewport* viewport = (*it)->GetViewport();

		CVector3D vBegin, vEnd; 

		if (g->pf.DeleteOrSelect3DObjectsSegment(m_mouseX, m_mouseY, viewport, &vBegin, &vEnd))
		{
			m_isSelectSegment = true;
		}
	}
}

void CWorldEditor::SetLMouseDblClick(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;

	if (!m_activeObjects.GetObject(0))
	{
		return;
	}

	if (m_dragType == 4 && m_firstPoint != CVector3D(0,0,0))
	{
		std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
		CViewport* viewport = (*it)->GetViewport();

		int objectID = g->o3dm.GetObjectNumber(m_activeObjects.GetObject(0)->GetName());

		if (g->pf.IsPointInCurrent3DObjectsPath(m_mouseX, m_mouseY, viewport, objectID))
		{
			m_secondPoint = g->pf.GetCurrentPoint();

		}
		else
		{
			m_secondPoint = viewport->GetSelected3DCoords();
		}

		g->pf.SetCurrentPoint(&m_secondPoint);
		g->pf.AddSegment(m_firstPoint, m_secondPoint, objectID, COMMON_ROUND_SEGMENT_TYPE);

		C3DObject* object = g->o3dm.GetObjectPointer(objectID);

		MP_NEW_P(addNewSegmentCommand, CDataStorage, 1000);

		AssemblePathesPackage(addNewSegmentCommand, object);

		m_commandsData.AddOutCommand(addNewSegmentCommand);

		m_firstPoint = m_secondPoint;
		m_secondPoint = CVector3D(0,0,0);

		UpdateCloneableObjectsPath(object);
	}
	else if (m_dragType == 6)
	{
		std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
		CViewport* viewport = (*it)->GetViewport();

		if (g->pf.IsPointInCurrent3DObjectsPath(m_mouseX, m_mouseY, viewport , m_currentObjectID))
		{
			m_isNeedToMovePoint = true;
		}
	}
}

void CWorldEditor::AssemblePathesPackage(CDataStorage* data, C3DObject* object)
{
	CVector3D objCoords = object->GetCoords();

	CQuaternion q = object->GetRotation()->GetAsDXQuaternion();
	q.Normalize();
	q.w *= -1;

	std::vector<SSegment*>& v = g->pf.FindCurrent3DObjectsPath();

	byte dragModeCmdId = CMD_DRAGMODE;
	data->Add(dragModeCmdId);
	data->Add(m_dragType);

	std::vector<SSegment*>::iterator it = v.begin();
	std::vector<SSegment*>::iterator itEnd = v.end();

	for (;it!=itEnd;it++)
	{
		CVector3D vtemp = CVector3D((*it)->begin.x - objCoords.x, (*it)->begin.y - objCoords.y, (*it)->begin.z - objCoords.z);
		vtemp *= q;
		data->Add(vtemp.x);
		data->Add(vtemp.y);
		data->Add(vtemp.z);

		vtemp = CVector3D((*it)->end.x - objCoords.x, (*it)->end.y - objCoords.y,(*it)->end.z - objCoords.z);
		vtemp *= q;

		data->Add(vtemp.x);
		data->Add(vtemp.y);
		data->Add(vtemp.z);

		data->Add((*it)->typeOfSegment);
	}
}

bool CWorldEditor::IsMButtonPressed()
{
	return m_mButtonPress;
}

bool CWorldEditor::IsLButtonPressed()
{
	return m_lButtonPress;
}

bool CWorldEditor::IsFreeCam()
{
	return m_freeCam;
}

void CWorldEditor::StopDragObjectByMouse()
{
	C3DObject* object = m_activeObjects.GetObject(0);
	if (!object)
	{
		return;
	}

	if (m_dragType == 1 || m_dragObjectByMouse == DRAG_X || m_dragObjectByMouse == DRAG_Y || m_dragObjectByMouse == DRAG_Z)
	{
		MP_NEW_P(moveResult, CDataStorage, 10);
		byte moveCmd = CMD_MOVE;
		moveResult->Add(moveCmd);

		CVector3D moveDelta = object->GetCoords()- m_moveInitCoords;

		moveResult->Add(moveDelta.x);
		moveResult->Add(moveDelta.y);
		moveResult->Add(moveDelta.z);

		m_commandsData.AddOutCommand(moveResult);

	}
	else if (m_dragType == 2)
	{
		// добавление в историю результата поворота
		MP_NEW_P(rotateResult, CDataStorage, 10);
		byte rotateCmd = CMD_ROTATE;
		rotateResult->Add(rotateCmd);

		short objCount = 1;

		float angleX = 0;
		float angleY = 0;
		float angleZ = 0; 

		CQuaternion objQuat = object->GetRotation()->GetAsDXQuaternion();

		m_rotateInitQuat.Conjugate();
		m_rotateInitQuat *= objQuat;

		float angle = 0;
		CVector3D vec;
		m_rotateInitQuat.Extract(vec, angle);
		
		angle = (float)((180.0f * angle) / PI);

		if (m_dragObjectByMouse == 1)
		{
			angleZ = angle * GetSign(vec.z);
		}
		else if (m_dragObjectByMouse == 2)
		{
			angleY = angle * GetSign(vec.y);
		}
		else if (m_dragObjectByMouse == 3)
		{
			angleX = angle * GetSign(vec.x);
		}

		rotateResult->Add(angleX);
		rotateResult->Add(angleY);
		rotateResult->Add(angleZ);
		rotateResult->Add(objCount);

		std::wstring objName = object->GetName();
		rotateResult->Add(objName);

		rotateResult->Add(objQuat.x);
		rotateResult->Add(objQuat.y);
		rotateResult->Add(objQuat.z);
		rotateResult->Add(objQuat.w);

		m_commandsData.AddOutCommand(rotateResult);
	}

	m_dragObjectByMouse = 0;
}

void CWorldEditor::MouseMove(int x, int y)
{
	if (m_mouseX == -1 && m_mouseY == -1)
	{
		return;
	}

	if (m_dragObjectByMouse > 0)
	{
		if (m_dragType == 1 || m_dragObjectByMouse == DRAG_X || m_dragObjectByMouse == DRAG_Y || m_dragObjectByMouse == DRAG_Z)
		{
			std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
			CViewport* viewport = (*it)->GetViewport();

			C3DObject* obj = m_activeObjects.GetObject(0);
			if (!obj)
			{
				return;
			}
			CVector3D objCoords = obj->GetCoords();

			CPlane3D movePlane;// плоскость 
			CVector3D vecN;

			if (m_dragObjectByMouse == 1 || m_dragObjectByMouse == DRAG_X)
			{
				vecN = CVector3D(0,0,1);
			}
			else if (m_dragObjectByMouse == 2 || m_dragObjectByMouse == DRAG_Z)
			{
				vecN = CVector3D(0,1,0);
			}
			else if (m_dragObjectByMouse == 3 || m_dragObjectByMouse == DRAG_Y)
			{
				vecN = CVector3D(1,0,0);
			}

			movePlane = CPlane3D(vecN, objCoords);

			CVector3D curMouseSelect = viewport->GetSelected3DCoords();

			CVector3D eye = viewport->GetCamera()->GetEyePosition();

			CVector3D nowPoint;
			movePlane.GetIntersection(eye, curMouseSelect-eye, nowPoint);

			CVector3D oldPoint;
			movePlane.GetIntersection(eye, m_mouseSelect-eye, oldPoint);

			CVector3D moveDelta = nowPoint - oldPoint;

			if (m_dragObjectByMouse == DRAG_X)
			{
				moveDelta.y = 0;
				moveDelta.z = 0;
			}
			else if (m_dragObjectByMouse == DRAG_Y)
			{
				moveDelta.x = 0;
				moveDelta.z = 0;
			}
			else if (m_dragObjectByMouse == DRAG_Z)
			{
				moveDelta.x = 0;
				moveDelta.y = 0;
			}

			objCoords += moveDelta;
			obj->SetCoords(objCoords);
			AddObjectInfo(GetSingleObjectDataHeader(obj), obj, false);

			m_mouseSelect = viewport->GetSelected3DCoords();
		}
		else if (m_dragType == 2)
		{
			CVector3D rotateVec;
			float angle = 0;
			if (m_dragObjectByMouse == 1)
			{
				rotateVec = CVector3D(0,0,1);
				angle = (float)(m_mouseX - x);
			}
			else if (m_dragObjectByMouse == 2)
			{
				rotateVec = CVector3D(0,1,0);
				angle = (float)(m_mouseY - y);
			}
			else if (m_dragObjectByMouse == 3)
			{
				rotateVec = CVector3D(1,0,0);
				angle = (float)(y - m_mouseY);
			}
			C3DObject* obj = m_activeObjects.GetObject(0);
			if (!obj)
			{
				return;
			}
			CQuaternion objQuat = obj->GetRotation()->GetAsDXQuaternion();
		
			CQuaternion deltaQuat = CQuaternion(rotateVec , (float)((angle * PI) / 180.0f));
			objQuat *= deltaQuat;

			CRotationPack rotation;
			rotation.SetAsDXQuaternion(objQuat.x, objQuat.y, objQuat.z, objQuat.w);
			obj->SetRotation(&rotation);

			CLODGroup* objectGroup = obj->GetLODGroup();
			objectGroup->UpdateBoundingBox(obj);

			objectGroup->ChangeCollisions(obj, false);
			Sleep(1);
			objectGroup->ChangeCollisions(obj, true);
		}
		
	}
	else if (m_mButtonPress)
	{
		std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
		CCamera3D* camera = (*it)->GetViewport()->GetCamera();

		if (!camera)
		{
			return;
		}
		float deltaX = (float)(x - m_mouseX) * 10.0f;
		float deltaY = (float)(y - m_mouseY) * 10.0f;

		CVector3D deltaVectorY = camera->GetLookAt() - camera->GetEyePosition();

		deltaVectorY.z = 0;

		CVector3D deltaVectorX = deltaVectorY;
		deltaVectorX = deltaVectorX.Cross(CVector3D( 0, 0, 1));

		deltaVectorY.Normalize();
		deltaVectorX.Normalize();

		deltaVectorY *= deltaY;
		deltaVectorX *= -deltaX;

		CVector3D delta = deltaVectorY + deltaVectorX;

		camera->SetEyePosition(camera->GetEyePosition() + delta);
		camera->SetLookAt(camera->GetLookAt() + delta);
	}
	else if (m_lButtonPress)
	{		
		std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
		CViewport* viewport = (*it)->GetViewport();
		g->pf.MovePoint(&(viewport->GetSelected3DCoords()));
		m_firstPoint = viewport->GetSelected3DCoords();
	}

	m_mouseX = x;
	m_mouseY = y;
}

void CWorldEditor::Run()
{
	if (m_pipeClient.TryConnect())
	{
		CSimpleThreadStarter pipeThreadStarter(__FUNCTION__);	
		pipeThreadStarter.SetRoutine(ThreadRunner);
		pipeThreadStarter.SetParameter(this);
		m_hPipeThread = pipeThreadStarter.Start();
		m_pipeThreadId = pipeThreadStarter.GetThreadID();

		if (m_hPipeThread == NULL)
		{
			MessageBox(NULL, "Can't create thread for editor's pipe client.", NULL, MB_OK);
		}

		// отключение фишки с мышкой
		g->rs.SetInt(UNDER_MOUSE_CHECK_DELTA_TIME, 1);
		g->rs.SetBool(PRECISION_OBJECT_PICKING, true);
		m_isRunning = true;
		g->pf.EnableDebugRendering(true);
	}
}

void CWorldEditor::SendMouseSelectedObject(C3DObject* obj)
{
	if ((!m_doSelectObjectUnderMouse) || (obj->GetAnimation() != NULL) || (m_dragType == 4) || (m_dragType == 5)|| (m_dragType == 6)
		|| m_dragType == DRAG_TYPE_GO_SEGMENT || m_dragType == 11 )
	{
		return;
	}

	m_doSelectObjectUnderMouse = false;

	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		if (!m_activeObjects.IsActive(obj))
		{
			obj->SetBoundingBoxVisible(true);
			m_activeObjects.AddObject(obj);
		}
		else
		{
			m_activeObjects.Deactivate(obj);
		}
	
		short objCount = (short)m_activeObjects.Count();
		if ( (objCount > 1 || objCount == 0) && (m_dragType != 4) )
		{
			MP_NEW_P(selectingCommand, CDataStorage, 10);
			byte selectCmdId = CMD_SELECT;
			selectingCommand->Add(selectCmdId);

			selectingCommand->Add(objCount);
			USES_CONVERSION;
			for (int objIndex = 0; objIndex < objCount; objIndex ++)
			{
				std::wstring name = m_activeObjects.GetObject(objIndex)->GetName();
				selectingCommand->Add(name);
			}
			byte selectFlag = 1;
			selectingCommand->Add(selectFlag);

			m_commandsData.AddOutCommand(selectingCommand);
		}
		else if (objCount == 1)
		{
			C3DObject* object = m_activeObjects.GetObject(0);
			if (object)
			{
				AddObjectInfo(GetSingleObjectDataHeader(object), object);
			}
		}
	}
	else
	{
		if (m_activeObjects.Count() == 1)
		{
			C3DObject* object = m_activeObjects.GetObject(0);
			// проверка на перетаскивание
			std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
			CViewport* viewport = (*it)->GetViewport();

			CVector2D mouse((float)viewport->GetSelectedX(), (float)viewport->GetSelectedY());

			CVector3D coords = object->GetCoords();	

			if (m_dragType != 0)
			{
				CVector2D oo, ox, oy, xy, oz, xz, yz;			
				viewport->GetScreenCoords(coords.x, coords.y, coords.z, &oo.x, &oo.y);
				viewport->GetScreenCoords(coords.x+20, coords.y, coords.z, &ox.x, &ox.y);
				viewport->GetScreenCoords(coords.x, coords.y+20, coords.z, &oy.x, &oy.y);
				viewport->GetScreenCoords(coords.x+20, coords.y+20, coords.z, &xy.x, &xy.y);
				viewport->GetScreenCoords(coords.x, coords.y, coords.z+20, &oz.x, &oz.y);
				viewport->GetScreenCoords(coords.x+20, coords.y, coords.z+20, &xz.x, &xz.y);
				viewport->GetScreenCoords(coords.x, coords.y+20, coords.z+20, &yz.x, &yz.y);

				bool xyHitTest = SquareHitTest(oo, ox, oy, xy, mouse);
				bool xzHitTest = SquareHitTest(oo, ox, oz, xz, mouse);
				bool yzHitTest = SquareHitTest(oo, oy, oz, yz, mouse);

				CVector3D eye = viewport->GetCamera()->GetEyePosition();
				float xyLen = (CVector3D(coords.x+20, coords.y+20, coords.z)-eye).GetLength();
				float xzLen = (CVector3D(coords.x+20, coords.y, coords.z+20)-eye).GetLength();
				float yzLen = (CVector3D(coords.x, coords.y+20, coords.z+20)-eye).GetLength();

				m_dragObjectByMouse = 0;

				if (xyHitTest)
				{				
					if (xzHitTest && (xzLen > xyLen))
					{
						m_dragObjectByMouse = 2;
					}
					if (yzHitTest && (yzLen > xyLen))
					{
						m_dragObjectByMouse = 3;
					}
					if (m_dragObjectByMouse == 0)
					{
						m_dragObjectByMouse = 1;
					}			
				}
				else if (xzHitTest)
				{
					if (xyHitTest && (xyLen > xzLen))
					{
						m_dragObjectByMouse = 1;
					}
					if (yzHitTest && (yzLen > xzLen))
					{
						m_dragObjectByMouse = 3;
					}
					if (m_dragObjectByMouse == 0)
					{
						m_dragObjectByMouse = 2;
					}
				}
				else if (yzHitTest)
				{
					if (xyHitTest && (xyLen > yzLen))
					{
						m_dragObjectByMouse = 1;
					}
					if (xzHitTest && (xzLen > yzLen))
					{
						m_dragObjectByMouse = 2;
					}
					if (m_dragObjectByMouse == 0)
					{
						m_dragObjectByMouse = 3;
					}
				}
				
				if (m_dragObjectByMouse == 0)
				{
					// пирамида?
					CVector2D ox, ox1, ox2, ox3, ox4, oy, oy1, oy2, oy3, oy4, oz, oz1, oz2, oz3, oz4;

					viewport->GetScreenCoords(coords.x + 60, coords.y, coords.z, &ox.x, &ox.y);
					viewport->GetScreenCoords(coords.x + 40, coords.y, coords.z + 4, &ox1.x, &ox1.y);
					viewport->GetScreenCoords(coords.x + 40, coords.y - 4, coords.z, &ox2.x, &ox2.y);
					viewport->GetScreenCoords(coords.x + 40, coords.y, coords.z - 4, &ox3.x, &ox3.y);
					viewport->GetScreenCoords(coords.x + 40, coords.y + 4, coords.z, &ox4.x, &ox4.y);

					viewport->GetScreenCoords(coords.x, coords.y + 60, coords.z, &oy.x, &oy.y);
					viewport->GetScreenCoords(coords.x, coords.y + 40, coords.z + 4, &oy1.x, &oy1.y);
					viewport->GetScreenCoords(coords.x - 4, coords.y + 40, coords.z, &oy2.x, &oy2.y);
					viewport->GetScreenCoords(coords.x, coords.y + 40, coords.z - 4, &oy3.x, &oy3.y);
					viewport->GetScreenCoords(coords.x + 4, coords.y + 40, coords.z, &oy4.x, &oy4.y);

					viewport->GetScreenCoords(coords.x, coords.y, coords.z + 60, &oz.x, &oz.y);
					viewport->GetScreenCoords(coords.x, coords.y + 4, coords.z + 40, &oz1.x, &oz1.y);
					viewport->GetScreenCoords(coords.x - 4, coords.y, coords.z + 40, &oz2.x, &oz2.y);
					viewport->GetScreenCoords(coords.x, coords.y - 4, coords.z + 40, &oz3.x, &oz3.y);
					viewport->GetScreenCoords(coords.x + 4, coords.y, coords.z + 40, &oz4.x, &oz4.y);

					if (PyramidHitTest(ox1, ox2, ox3, ox4, ox, mouse)) // x
					{
						m_dragObjectByMouse = DRAG_X;
					}
					else if (PyramidHitTest(oy1, oy2, oy3, oy4, oy, mouse)) // y
					{
						m_dragObjectByMouse = DRAG_Y;
					}
					else if (PyramidHitTest(oz1, oz2, oz3, oz4, oz, mouse)) // z
					{
						m_dragObjectByMouse = DRAG_Z;
					}
				}			
			}

			if (m_dragObjectByMouse > 0)
			{
				m_mouseX = (int)mouse.x;
				m_mouseY = (int)mouse.y;
				m_mouseSelect = viewport->GetSelected3DCoords();
				m_moveInitCoords = coords;
				m_rotateInitQuat = obj->GetRotation()->GetAsDXQuaternion();
			}
		}

		if (m_dragObjectByMouse == 0)
		{
			if ((m_activeObjects.Count() != 1) || (m_activeObjects.Count() == 1 && !m_activeObjects.IsActive(obj)))
			{
				obj->SetBoundingBoxVisible(true);
				m_activeObjects.Deactivate();		
				m_activeObjects.AddObject(obj);
				AddObjectInfo(GetSingleObjectDataHeader(obj), obj);
			}
		}
	}

	if (m_activeObjects.GetObject(0))
	{
		m_currentObjectID = g->o3dm.GetObjectNumber(m_activeObjects.GetObject(0)->GetName());
	}

	if (g->pf.SetCurrentObjectID(m_currentObjectID))
	{
		m_firstPoint = CVector3D(0,0,0);
	}
}

bool CWorldEditor::SquareHitTest(CVector2D vec1, CVector2D vec2, CVector2D vec3, CVector2D vec4, CVector2D point)
{
	// площадь параллелограмма
	float square = GetTriangleSquare(vec1, vec2, vec3) + GetTriangleSquare(vec2, vec3, vec4);
	float eps = 2;

	// площади треугольников
	float sq1 = GetTriangleSquare(vec1, vec2, point);
	float sq2 = GetTriangleSquare(vec1, vec3, point);
	float sq3 = GetTriangleSquare(vec3, vec4, point);
	float sq4 = GetTriangleSquare(vec2, vec4, point);
	float sumSq = sq1 + sq2 + sq3 + sq4;

	if (sumSq <= square + eps)
	{
		// в прямоугольнике
		return true;
	}

	return false;
}

bool CWorldEditor::PyramidHitTest(CVector2D vec1, CVector2D vec2, CVector2D vec3, CVector2D vec4, CVector2D vec5, CVector2D point)
{

//    1
//   /|\
//  /_5_\
//2 \ | / 4
//   \|/
//    3
//
	float eps = 2;
	float square = GetTriangleSquare(vec1, vec2, vec5);

	float sq1 = GetTriangleSquare(vec1, vec2, point);
	float sq2 = GetTriangleSquare(vec2, vec5, point);
	float sq3 = GetTriangleSquare(vec1, vec5, point);

	if (sq1 + sq2 + sq3 <= square + eps)
	{
		return true;
	}

	square = GetTriangleSquare(vec2, vec3, vec5);

	sq1 = GetTriangleSquare(vec2, vec3, point);
	sq2 = GetTriangleSquare(vec3, vec5, point);
	sq3 = GetTriangleSquare(vec2, vec5, point);

	if (sq1 + sq2 + sq3 <= square + eps)
	{
		return true;
	}

	square = GetTriangleSquare(vec3, vec4, vec5);

	sq1 = GetTriangleSquare(vec3, vec4, point);
	sq2 = GetTriangleSquare(vec4, vec5, point);
	sq3 = GetTriangleSquare(vec3, vec5, point);

	if (sq1 + sq2 + sq3 <= square + eps)
	{
		return true;
	}

	square = GetTriangleSquare(vec1, vec4, vec5);

	sq1 = GetTriangleSquare(vec1, vec4, point);
	sq2 = GetTriangleSquare(vec4, vec5, point);
	sq3 = GetTriangleSquare(vec1, vec5, point);

	if (sq1 + sq2 + sq3 <= square + eps)
	{
		return true;
	}

	return false;		 
}

bool CWorldEditor::IsDragObjectByMouse()
{
	return (m_dragObjectByMouse != 0);
}

void CWorldEditor::AddObjectInfo(CDataStorage* objectData, C3DObject* obj, bool isMouseSelected /* = true */)
{
	USES_CONVERSION;
	objectData->SetObjectName( W2A(obj->GetName()));

	CVector3D objTranslation = obj->GetCoords();
	objectData->Add(objTranslation.x);
	objectData->Add(objTranslation.y);
	objectData->Add(objTranslation.z);

	CVector3D objScale = obj->GetScale3D();
	objectData->Add(objScale.x);
	objectData->Add(objScale.y);
	objectData->Add(objScale.z);

	CLODGroup* objGroup = obj->GetLODGroup();
	byte lodCount = (byte)objGroup->GetLodLevelCount();
	objectData->Add(lodCount);

	for (byte lodIndex = 0; lodIndex < lodCount; lodIndex ++)
	{
		SLODLevel* lodLevel = objGroup->GetLodLevel(lodIndex);
		objectData->Add(lodLevel->lodMaxDist);
		int shadow = lodLevel->shadowType;
		if (shadow != 0)
		{
			shadow = 1; 
		}
		else
		{
			shadow = 0;
		}
		objectData->Add(shadow);
	}

	byte selectFlag = 1;
	if (!isMouseSelected)
	{
		selectFlag = 0;
	}
	objectData->Add(selectFlag);

	m_commandsData.AddOutCommand(objectData);
}

DWORD WINAPI CWorldEditor::ThreadRunner(LPVOID lpvParam)
{
	((CWorldEditor*)lpvParam)->PipeClientThread();

	return 0;
}

void CWorldEditor::PipeClientThread()
{
	m_clientLoadedFromEditor = false;
	// общение с редактором
	while (m_pipeClient.IsConnected())
	{
		if (m_needCommandProcessing)
		{
			continue;
		}
		
		std::vector<CDataStorage*> commands;
		CDataStorage* outCommandData = m_commandsData.GetOutCommand();
		if (outCommandData == NULL)
		{			
			outCommandData = GetIdleData();
			commands.push_back(outCommandData);
		}
		else while (outCommandData != NULL)
		{
			commands.push_back(outCommandData);
			outCommandData = m_commandsData.GetOutCommand();
		}

		m_commandsData.AddInCommand(m_pipeClient.ReadData()); // чтение команды
		m_pipeClient.WriteData(commands); // отправка команды
		m_needCommandProcessing = true;

		Sleep(100);
	}	
}

void CWorldEditor::ProcessCommand()
{
	if (!m_pipeClient.IsConnected() || !m_needCommandProcessing)
	{
		return;
	}

	m_needCommandProcessing = false;

	byte cmdID = 0;
	CDataStorage* commandData = m_commandsData.GetInCommand();

	while (commandData != NULL)
	{
		if ((commandData->Read(cmdID) && cmdID != 0 && cmdID != 1) || m_clientLoadedFromEditor == false)
		{
			break;
		}

		MP_DELETE(commandData);
		commandData = m_commandsData.GetInCommand();
	}

	if (commandData != NULL)
	{
		if (m_clientLoadedFromEditor == false)
		{
			// подтверждение загрузки из редактора
			if (cmdID != CMD_HELLO)
			{
				m_pipeClient.Disconnect();
			}
			else
			{
				m_clientLoadedFromEditor = true;
			}
		}

		ProcessCommandById(cmdID, commandData);
	}

	MP_DELETE(commandData);
}

void CWorldEditor::RelinkWindow()
{
	MP_NEW_P(relinkWindowCommand, CDataStorage, 10);
	byte relinkWindowCmdId = CMD_RELINK_WINDOW;
	relinkWindowCommand->Add(relinkWindowCmdId);

	m_commandsData.AddOutCommand(relinkWindowCommand);
}

void CWorldEditor::AddNewMaterial(std::string& materialName, char* data)
{
	g->matm.LoadMaterial("external\\materials\\" + materialName, data);
	g->mm.RelinkMaterials();
}

void CWorldEditor::DisableMaterial(std::string& materialName)
{
	int materialID = g->matm.GetObjectNumber("external\\materials\\" + materialName);

	if (materialID != -1)
	{
		g->matm.DeleteObject(materialID);
		g->mm.RelinkMaterials();
	}
}

void CWorldEditor::ChangeMaterialParameter(std::string& materialName, std::string& paramName, std::string& paramType,
											std::string& paramValue, bool isState)
{
	int materialID = g->matm.GetObjectNumber("external\\materials\\" + materialName);
	if (materialID != -1)
	{
		CMaterial* material = g->matm.GetObjectPointer(materialID);

		if (isState)
		{
			material->AddState(paramName, paramValue);
		}
		else
		{
			if ((paramType == "texture") || (paramType == "cubemap"))
			{
				if (!m_activeObjects.GetObject(0))
				{
					return;
				}

				USES_CONVERSION;
				std::string path = W2A( m_activeObjects.GetObject(0)->GetLODGroup()->GetName());
				if (path[0] == ':')
				{
					path = path.substr(1);
				}

				int pos = path.find_first_of(";");
				if (pos != -1)
				{
					path = path.substr(0, pos - 1);
				}

				path = path.substr(0, path.find_last_of("\\"));
				path = path.substr(0, path.find_last_of("\\"));

				path += "\\textures\\";
				path += paramValue;

				int level = rtl_atoi(paramName.c_str());

				if (paramType == "texture")
				{
					STexture* tex = MP_NEW(STexture);
					tex->textureID = g->tm.LoadAsynch(paramValue, path, NULL);
					tex->isAlphaBlend = false;
					material->SetTextureLevel(level, TEXTURE_SLOT_TEXTURE_2D, tex);
				}
				else
				{
					int textureID = g->cmm.AddObject(paramValue);
					CCubeMap* cubeMap = g->cmm.GetObjectPointer(textureID);
					if (cubeMap->GetCubeMapID() == -1)
					{
						g->tm.LoadAsynch(paramValue, path, NULL);
					}
					material->SetTextureLevel(level, TEXTURE_SLOT_TEXTURE_CUBEMAP, (void*)textureID);
				}
			}
			else
			{
				material->SetShaderParamValue(paramName, paramType, paramValue);
			}			
		}
	}
}

CDataStorage* CWorldEditor::GetIdleData()
{
	MP_NEW_P(idleData, CDataStorage, 10);
	BYTE idleByte = CMD_IDLE;
	idleData->Add(idleByte);

	return idleData;
}

void CWorldEditor::SetMapManager(CMapManager* _mapManager)
{
	m_mapManager = _mapManager;
}

void CWorldEditor::SendCamera()
{
	if (m_pipeClient.IsConnected())
	{
		if (g->ne.time - m_lastCameraTime < 1000)
		{
			return;
		}

		m_lastCameraTime = g->ne.time;

		if (!g->scm.GetActiveScene())
		{
			return;
		}

		if (!g->scm.GetActiveScene()->GetViewport())
		{
			return;
		}

		if (!g->scm.GetActiveScene()->GetViewport()->GetCamera())
		{
			return;
		}

		CVector3D eye = g->scm.GetActiveScene()->GetViewport()->GetCamera()->GetEyePosition();
		CVector3D lookAt = g->scm.GetActiveScene()->GetViewport()->GetCamera()->GetLookAt();

		MP_NEW_P(command, CDataStorage, 25);
		byte cmdID = CMD_CURRENT_CAMERA;
		command->Add(cmdID);
		command->Add(eye.x);
		command->Add(eye.y);
		command->Add(eye.z);
		command->Add(lookAt.x);
		command->Add(lookAt.y);
		command->Add(lookAt.z);

		m_commandsData.AddOutCommand(command);
	}
}

void CWorldEditor::SendStats()
{
	if (m_isRunning)
	{
		g->rs.SetInt(UNDER_MOUSE_CHECK_DELTA_TIME, 1);
		g->rs.SetBool(PRECISION_OBJECT_PICKING, true);
	}
	else
	{
		return;
	}

	if (g->scm.GetActiveScene())
	if (g->scm.GetActiveScene()->GetViewport())
	{
		int textureID = g->scm.GetActiveScene()->GetViewport()->GetSelectedTextureID();
		C3DObject* obj = g->scm.GetActiveScene()->GetViewport()->GetObjectUnderMouse();
		if (textureID != m_lastSelectedTextureID)
		{
			CTexture* texture = g->tm.GetObjectPointer(textureID);

			MP_NEW_P(command, CDataStorage, 300);

			byte cmdID = CMD_SELECT_TEXTURE;
			command->Add(cmdID);
			std::wstring name = texture->GetName();
			command->Add(name);
			int lod = 0;
			if (obj)
			{
				lod = obj->GetLODNumber();
			}
			command->Add(lod);
		
			m_commandsData.AddOutCommand(command);

			m_lastSelectedTextureID = textureID;
		}
	}

	if (m_statsNeed)
	{
		std::string stats;

		unsigned int polyCount = 0;
		unsigned int nowPolyCount = 0;
		for (unsigned int i = 0; i < m_activeObjects.Count(); i++)
		{
			C3DObject* obj = m_activeObjects.GetObject(i);

			int modelID = obj->GetLODGroup()->GetLodLevel(0)->GetModelID();
			CModel* model = g->mm.GetObjectPointer(modelID);
			polyCount += model->GetFaceCount();

			int lodID = obj->GetLODNumber();
			if (lodID >= 0)
			{
				SLODLevel* lodLevel = obj->GetLODGroup()->GetLodLevel(lodID);
				modelID = lodLevel->GetModelID();
				if (obj->GetDistSq() < lodLevel->lodMaxDistSq)
				{
					model = g->mm.GetObjectPointer(modelID);
					nowPolyCount += model->GetFaceCount();
				}
			}
		}

		stats += "Полигонов в выдел. объектах: ";
		stats += IntToStr(polyCount);
		stats += "/";
		stats += IntToStr(nowPolyCount);
		stats += "\n";

		stats += g->stat.GetStatString();

		stats += "Коллизия: ";
		stats += g->phom.GetCollisionObjectName();

		C3DObject* avatar = m_mapManager->GetAvatar();
		CVector3D coords;
		CVector3D rotVector;
		float rotAngle = 0;
		if (avatar)
		{
			coords = avatar->GetCoords();
			CQuaternion rot = avatar->GetRotation()->GetAsDXQuaternion();
			rot.Extract(rotVector, rotAngle);
		}

		MP_NEW_P(statsData, CDataStorage, 10);
		byte statsCmdID = CMD_STAT;
		statsData->Add(statsCmdID);
		statsData->Add(stats);

		if (avatar)
		{
			statsData->Add(coords.x);
			statsData->Add(coords.y);
			statsData->Add(coords.z);

			statsData->Add(rotAngle);
			statsData->Add(rotVector.x);
			statsData->Add(rotVector.y);
			statsData->Add(rotVector.z);
		}

		m_commandsData.AddOutCommand(statsData);
		m_statsNeed = false;
	}
}

void CWorldEditor::Update()
{
	SendStats();
	SendCamera();

	if ((m_isFirstDownClk) && (g->ne.time - m_firstMouseDownClkTime > MIN_DBLCLK_TIME))
	{
		SetLButtonDownAfterUpdate(m_mouseDownXLClk, m_mouseDownYLClk);
		m_isFirstDownClk = false;		
	}

	if ((m_isFirstUpClk) && (g->ne.time - m_firstMouseUpClkTime > MIN_DBLCLK_TIME))
	{
		SetLButtonUpAfterUpdate(m_mouseDownXLClk, m_mouseDownYLClk);
		m_isFirstUpClk = false;		
	}
}

bool CWorldEditor::GetConnectionState()
{
	return m_pipeClient.IsConnected();
}

void CWorldEditor::SelectClickedObject()
{
	if (!m_doSelectObjectUnderMouse)
	{
		m_doSelectObjectUnderMouse = true;
	}
}

bool CWorldEditor::DoSelectClickedObject()
{
	return m_doSelectObjectUnderMouse;
}

CActiveObjects* CWorldEditor::GetActiveObjects()
{
	return &m_activeObjects;
}

C3DObject* CWorldEditor::GetObjectByName(std::string name)
{
	int objectId = g->o3dm.GetObjectNumber(name);

	C3DObject* obj = NULL;
	if (objectId != ERR_OBJECT_NOT_FOUND)
	{
		obj = g->o3dm.GetObjectPointer(objectId);
	}

	return obj;
}

bool CWorldEditor::StopAvatar()
{
	if (!m_mapManager)
	{
		return false;
	}

	nrmCharacter* avatar = (nrmCharacter*)m_mapManager->GetAvatar()->GetUserData(USER_DATA_NRMOBJECT);
	
	return (avatar) ? avatar->stopGo() : false;
}

CDataStorage* CWorldEditor::GetSingleObjectDataHeader(C3DObject* object)
{
	MP_NEW_P(objectData, CDataStorage, 10);
		byte selectCmd = CMD_SELECT;
		objectData->Add(selectCmd);
		short count = 1;
		objectData->Add(count);

		std::wstring objectName = object->GetName();
		objectData->Add(objectName);

		return objectData;
}


bool CWorldEditor::DoPlaceObjectByMouse()
{
	return m_placeObjectByMouse;
}

void CWorldEditor::CreateNewObjectByMouse(CVector3D* coords, bool sendAnswer /* = true */)
{
	m_placeObjectByMouse = false;
	m_doSelectObjectUnderMouse = false;

	C3DObject* newObject;
	if (!CreateObject3D(m_newObjectInfo.url, m_newObjectInfo.className, &newObject, m_newObjectInfo.name, m_newObjectInfo.level))
	{
		return;
	}
	newObject->GetRotation()->SetAsDXQuaternion(0.0f, 0.0f, 1.0f, 0.0f);
	
	if (sendAnswer)
	{
		SetSystemCursor(CopyCursor(m_normalCursor), 32512); 

		coords->z += newObject->GetSize().z / 2;
		MP_NEW_P(createCommand, CDataStorage, 10);
		byte createCmdId = CMD_OBJECT_CREATE;
		createCommand->Add(createCmdId);

		createCommand->Add(m_newObjectInfo.libId);
		createCommand->Add(m_newObjectInfo.name);
		createCommand->Add(coords->x);
		createCommand->Add(coords->y);
		createCommand->Add(coords->z);

		short activeObjCount = (short)m_activeObjects.Count();
		createCommand->Add(activeObjCount);
		USES_CONVERSION;
		for (unsigned int objIndex = 0; objIndex < m_activeObjects.Count(); objIndex++)
		{
			std::wstring objName = m_activeObjects.GetObject(objIndex)->GetName();
			createCommand->Add(objName);
		}

		m_commandsData.AddOutCommand(createCommand);
	}

	newObject->SetCoords(*coords);

	m_activeObjects.Deactivate();
	newObject->SetBoundingBoxVisible(true);
	m_activeObjects.AddObject(newObject);

	newObject->GetLODGroup()->ChangeCollisions(newObject, false);
	Sleep(1);
	newObject->GetLODGroup()->ChangeCollisions(newObject, true);

	AddObjectInfo(GetSingleObjectDataHeader(newObject), newObject, false);
}

void CWorldEditor::RefreshObjects()
{
	for (unsigned int objIndex = 0; objIndex < m_activeObjects.Count(); objIndex++)
	{
		// обновление объекта
		C3DObject* obj = m_activeObjects.GetObject(objIndex);

		if ((int)obj->GetUserData(USER_DATA_LEVEL) == LEVEL_GROUND)
		{
			obj->GetLODGroup()->ChangeCollisions(obj, false);
		}

		if ((obj->GetAnimation() != NULL) && (obj->GetUserData(USER_DATA_NRMOBJECT) != NULL))
		{
			mlMedia* avatarMedia = ((nrmObject*)obj->GetUserData(USER_DATA_NRMOBJECT))->GetMLMedia();
			if (avatarMedia != NULL)
			{
				const wchar_t* pwcSrc = avatarMedia->GetILoadable()->GetSrc();
				USES_CONVERSION;
				std::string src = W2A(pwcSrc);
				int saoID = g->skom.GetObjectNumber(obj, src);
				if (saoID != -1)
				{
					g->skom.DeleteObject(saoID);
				}
				saoID = g->skom.AddObject(obj, src);
				CSkeletonAnimationObject* newAnimation = g->skom.GetObjectPointer(saoID);				
				if (newAnimation->Load(src, false, NULL))
				{
					obj->SetAnimation(newAnimation);
				}
			}			
		}
		else
		{
			obj->GetLODGroup()->ReloadAll();
		}		
	}

	m_needRefresh = false;
}

bool CWorldEditor::IsNeedRefresh()
{
	return m_needRefresh;
}

CWorldEditor::~CWorldEditor()
{
	m_pipeClient.Disconnect();

	SetSystemCursor(m_normalCursor, 32512);

	DestroyCursor(m_dragCursor);	
	DestroyCursor(m_oldCursor);
	DestroyCursor(m_placeCursor);
}