
#pragma once

#include "2DScene.h"
#include "NRManager.h"
#include "LastScreen.h"
#include "crmResLibrary.h"
#include "MouseController.h"
#include "CameraInLocation.h"
#include "WhiteBoardManager.h"
#include "SharingBoardManager.h"
#include "../GraphicsDebugSupporter.h"
#include "LoadingEventsController.h"
#include "WorldEditor/GroupController.h"
#include "MinimumSystemRequirements.h"

#define LANGUAGE_RUSSIAN		0
#define LANGUAGE_ENGLISH		1

class CRMContext
{
public:
	static CRMContext* GetInstance();

	void SetNRManager(CNRManager* _nrman);
	void SetWhiteBoardManager(CWhiteBoardManager* _wbman);
	void SetMouseController(CMouseController* _mouseCtrl);
	void SetSharingBoardManager(CSharingBoardManager* _shbman);
	void Set2DScene(C2DScene* _2dscene);
	void SetResLibrary(crmResLibrary* _resLib);
	void SetCameraInLocation(CCameraInLocation* _cameraInLocation);
	void SetLastScreen(CLastScreen* _lastScreen);
	void SetMapManager(CMapManager* _mapMan);
	void SetMapManagerServer(CMapManagerServer* _mapManServer);
	void SetGroupController(CGroupController* _groupCtrl);	

	C2DScene* scene2d;
	crmResLibrary* resLib;
	CNRManager* nrMan;
	CLastScreen* lastScreen;
	CWhiteBoardManager* wbMan;
	CMouseController* mouseCtrl;
	CSharingBoardManager* shbMan;
	CCameraInLocation* cameraInLocation;
	CMapManager* mapMan;
	CMapManagerServer* mapManServer;
	CGroupController* groupCtrl;
	CLoadingEventsController* loadingCtrl;
	CGraphicsDebugSupporter* graphicDebug;
	CMinimumSystemRequirements* minSysReq;

	int lang;

private:
	CRMContext();
	CRMContext(const CRMContext&);
	CRMContext& operator=(const CRMContext&);
	~CRMContext();
};

extern CRMContext* gRM;