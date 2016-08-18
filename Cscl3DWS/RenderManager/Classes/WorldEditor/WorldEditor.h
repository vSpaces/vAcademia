#pragma once

#include "../CommonRenderManagerHeader.h"

#include "GlobalSingletonStorage.h"
#include "activeobjects.h"
#include "IWorldEditor.h"
#include "CommandQueue.h"
#include "DataStorage.h"
#include "PipeClient.h"
#include "MapManager.h"
#include "Commands.h"
#include <queue>

#define DRAG_TYPE_GENERATE_QUAD_PATH	7
#define DRAG_TYPE_GENERATE_CIRCLE_PATH	8
#define DRAG_TYPE_DELETE_PATH			9
#define DRAG_TYPE_GO_SEGMENT			10

typedef struct _SNewObjectInfo 
{
	MP_STRING name;
	MP_STRING url;
	MP_STRING className;
	MP_STRING params;
	int level;
	int libId;

	_SNewObjectInfo();	
} SNewObjectInfo;

class CWorldEditor: public IWorldEditor
{
public:
	CWorldEditor();
	~CWorldEditor();

	void Run();
	bool GetConnectionState();

	void SelectClickedObject();
	bool DoSelectClickedObject();

	bool DoPlaceObjectByMouse();

	void SendMouseSelectedObject(C3DObject* obj);
	void CreateNewObjectByMouse(CVector3D* coords, bool sendAnswer = true);

	void SetMapManager(CMapManager* _mapManager);

	void Update();

	CActiveObjects* GetActiveObjects();

	void RelinkWindow();

	void ProcessCommand();

	void OnChar(int keycode);

	void SetMButtonPress(bool enable, int x = -1, int y = -1);
	void SetLButtonDown(int x, int y);
	void SetLButtonUp(int x, int y);

	void SetLMouseDblClick(int x, int y);

	bool IsMButtonPressed();
	bool IsLButtonPressed();

	void MouseMove(int x, int y) ;
	bool IsFreeCam();

	void RefreshObjects();
	bool IsNeedRefresh();

	bool IsDragObjectByMouse();
	void StopDragObjectByMouse();

	
private:
	void ProcessCommandById(int cmdID, CDataStorage* commandData);
	void OnStat(CDataStorage* commandData);
	void OnDelete(CDataStorage* deletingData);
	void OnSelect(CDataStorage* selectingData);
	void OnFreeze(CDataStorage* freezingData);
	void OnSelectSame(CDataStorage* selectingData);
	void OnSelectByMask(CDataStorage* selectingData);
	void OnRename(CDataStorage* renamingData);
	void OnMove(CDataStorage* movingData);
	void OnRotate(CDataStorage* rotatingData);
	void OnChangeMaterial(CDataStorage* materialData);
	void OnScale(CDataStorage* scalingData);
	void OnShadow(CDataStorage* shadowData);
	void OnObjectCopy(CDataStorage* copyingData);
	void OnObjectCreate(CDataStorage* creatingData);
	void OnFreeCamera(CDataStorage* freeCamData);
	void OnRefresh(CDataStorage* commandData);
	void OnTeleportToNextActiveObject(CDataStorage* commandData);
	void OnDragMode(CDataStorage* dragModeData);
	void OnClass(CDataStorage* classData);
	void OnParams(CDataStorage* classData);
	void OnLock(CDataStorage* lockData);
	void OnShowCollisions(CDataStorage* scData);
	void OnShowInvisibleCollisions(CDataStorage* scData);
	void OnShowNearestPathes(CDataStorage* spData);
	void OnSwitchOffPathes(CDataStorage* pfData);
	void OnEnableWireframe(CDataStorage* scData);
	void OnChangeCollision(CDataStorage* colData);
	void OnUpdateLods(CDataStorage* data);
	void OnGetModelInfo(CDataStorage* data);
	void OnGroup(CDataStorage* data);
	void OnChangeGroup(CDataStorage* data);
	void OnHello(CDataStorage* data);
	void OnTeleportByCoordsToObject(CDataStorage* commandData);

	void UpdateCloneableObjectsPath(C3DObject* object);
    
	void AddObjectInfo(CDataStorage* objectData, C3DObject* obj, bool isMouseSelected = true);

	C3DObject* GetObjectByName(std::string name);
	CDataStorage* GetSingleObjectDataHeader(C3DObject* object);

	bool StopAvatar();

	bool SquareHitTest(CVector2D vec1, CVector2D vec2, CVector2D vec3, CVector2D vec4, CVector2D point);
	bool PyramidHitTest(CVector2D vec1, CVector2D vec2, CVector2D vec3, CVector2D vec4, CVector2D vec5, CVector2D point);

	void ChangeMaterialParameter(std::string& materialName, std::string& paramName, std::string& paramType,
								 std::string& paramValue, bool isState);
	void AddNewMaterial(std::string& materialName, char* data);
	void DisableMaterial(std::string& materialName);

	void SetLButtonDownAfterUpdate(int x, int y);
	void SetLButtonUpAfterUpdate(int x, int y);

	void SendStats();
	void SendCamera();

	static DWORD WINAPI ThreadRunner(LPVOID param);
	void PipeClientThread();

	CDataStorage* GetIdleData();

	void AssemblePathesPackage(CDataStorage* data, C3DObject* object);

	bool m_doSelectObjectUnderMouse;
	bool m_placeObjectByMouse;

	HCURSOR m_normalCursor;
	HCURSOR m_dragCursor;
	HCURSOR m_placeCursor;
	HCURSOR m_oldCursor;

	int m_mouseX;
	int m_mouseY;
	bool m_isRunning;
	bool m_mButtonPress;
	bool m_lButtonPress;
	bool m_freeCam;

	CVector3D m_mouseSelect;

	int m_dragType; // 0 - ничего, 1 - перемещение, 2 - поворот, 3 - масштаб, 4 - добавление нового сегмента пути
	int m_dragObjectByMouse;
	CVector3D m_moveInitCoords;
	CQuaternion m_rotateInitQuat;


	bool m_clientLoadedFromEditor;
	bool m_needCommandProcessing;

	bool m_needRefresh;

	SNewObjectInfo m_newObjectInfo;

	CActiveObjects m_activeObjects;
	CPipeClient m_pipeClient;
	CCommandQueue m_commandsData;

	DWORD m_pipeThreadId; 
	HANDLE m_hPipeThread; 	

	CMapManager* m_mapManager;

	bool m_statsNeed;

	int m_lastSelectedTextureID;

	__int64 m_lastCameraTime, m_firstMouseDownClkTime, m_firstMouseUpClkTime;

	CVector3D m_firstPoint;
	CVector3D m_secondPoint;

	int m_currentObjectID;

	int m_mouseDownXLClk, m_mouseDownYLClk, m_mouseUpXLClk, m_mouseUpYLClk;

	bool m_isNeedToMovePoint;
	bool m_isFirstDownClk, m_isDoubleDownClk, m_isFirstUpClk, m_isDoubleUpClk;
	bool m_isNeedToMovePointOnBtnUp;
	bool m_isSelectSegment;
};
