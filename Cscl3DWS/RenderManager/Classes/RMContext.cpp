
#include "StdAfx.h"
#include "RMContext.h"

CRMContext* gRM = CRMContext::GetInstance();

CRMContext::CRMContext():	
	nrMan(NULL),
	wbMan(NULL),
	shbMan(NULL),
	resLib(NULL),
	mapMan(NULL),
	scene2d(NULL),
	mouseCtrl(NULL),	
	cameraInLocation(NULL),
	lang(LANGUAGE_RUSSIAN),
	groupCtrl(NULL)
{	 
	loadingCtrl = MP_NEW(CLoadingEventsController);
	graphicDebug = MP_NEW(CGraphicsDebugSupporter);
	minSysReq = MP_NEW(CMinimumSystemRequirements);
}

void CRMContext::SetNRManager(CNRManager* _nrman)
{
	nrMan = _nrman;
	groupCtrl = MP_NEW(CGroupController);
}

void CRMContext::SetWhiteBoardManager(CWhiteBoardManager* _wbman)
{
	wbMan = _wbman;
}

void CRMContext::SetMouseController(CMouseController* _mouseCtrl)
{
	mouseCtrl = _mouseCtrl;
}

void CRMContext::SetSharingBoardManager(CSharingBoardManager* _shbman)
{
	shbMan = _shbman;
}

void CRMContext::Set2DScene(C2DScene* _scene2d)
{
	scene2d = _scene2d;
}

void CRMContext::SetResLibrary(crmResLibrary* _resLib)
{
	resLib = _resLib;
}

void CRMContext::SetCameraInLocation(CCameraInLocation* _cameraInLocation)
{
	cameraInLocation = _cameraInLocation;
}


void CRMContext::SetLastScreen(CLastScreen* _lastScreen)
{
	lastScreen = _lastScreen;
}

void  CRMContext::SetGroupController(CGroupController* _groupCtrl)
{
	groupCtrl = _groupCtrl; 
}

void CRMContext::SetMapManager(CMapManager* _mapMan)
{
	mapMan = _mapMan;
}

void CRMContext::SetMapManagerServer(CMapManagerServer* _mapManServer)
{
	mapManServer = _mapManServer;
}

CRMContext* CRMContext::GetInstance()
{
	static CRMContext* obj = NULL;

	if (!obj)
	{
		// Намеренно, чтобы сохранить синглтонность :)
		obj = new CRMContext();
	}

	return obj;
}

CRMContext::~CRMContext()
{
	MP_DELETE(groupCtrl);
	MP_DELETE(loadingCtrl);
}
