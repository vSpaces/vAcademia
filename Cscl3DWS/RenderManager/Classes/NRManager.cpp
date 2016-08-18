
#include "stdafx.h"
#include <Assert.h>
#include "NRManager.h"
#include "rmcomp.h"
#include "nrmMath3D.h"
#include "nrmQuery.h"
#include "MapManager.h"
#include "SyncMan.h"
#include "ServiceMan.h"
#include "cscl3dws.h"
#include "IniFile.h"

// Medias
#include "nrmPlugin.h"
#include "nrmObject.h"
#include "nrmPath3D.h"
#include "nrmImage.h"
#include "nrmAnim.h"
#include "nrm3DObject.h"
#include "nrmVisemes.h"
#include "nrmCharacter.h"
#include "nrmViewport.h"
#include "nrmCamera.h"
#include "nrmMotion.h"
#include "nrmLight.h"
#include "nrmCloud.h"
#include "nrm3DScene.h"
#include "nrm3DGroup.h"
#include "nrmMaterial.h"
#include "nrmText.h"
#include "nrmAudio.h"
#include "nrmActiveX.h"
#include "nrmBrowser.h"
#include "nrmFlash.h"
#include "nrmVideo.h"
#include "nrmEdit.h"
#include "nrmLine.h"
#include "nrmParticles.h"
#include "UserData.h"
#include "nrmImageFrame.h"
#include "crmResLibrary.h"
#include "nrm3DSoundManager.h"
#include "NRManager.h"
#include "iasynchresourcemanager.h"
#include "..\nrmDrawManager.h"
#include "notifies.h"
#include "MouseController.h"

#include <glew.h>
#include "HelperFunctions.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"
#include "EngineInit.h"
#include "DynamicClouds.h"
#include "CubemapClouds.h"
#include "__crashtest.h"

#include "LastScreen.h"
#include "WhiteBoardManager.h"
#include "sharingboardmanager.h"
#include "TexturesPreloader.h"

#include "RMContext.h"
#include "..\GlobalDefines.h"

#define DEFAULT_WIDTH	800
#define DEFAULT_HEIGHT	600
#define DEFAULT_BPP		32
#define DEFAULT_FULLSCR	FALSE

#include "WorldEditor/WorldEditor.h"
#include "rmml.h"
#include "../../../Common/MD5.h"

#include "EngineWarnings.h"
#include "GlobalKinect.h"

#include "KnownProxyServersList.h"

struct LinkedMem {
	UINT32	uiVersion;
	DWORD	uiTick;
	float	fAvatarPosition[3];
	float	fAvatarFront[3];
	float	fAvatarTop[3];
	wchar_t	name[256];
	float	fCameraPosition[3];
	float	fCameraFront[3];
	float	fCameraTop[3];
	wchar_t	identity[256];
	UINT32	context_len;
	unsigned char context[256];
	wchar_t description[2048];
};

LinkedMem *lm = NULL;


void initMumble() {
	HANDLE hMapObject = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(LinkedMem), L"MumbleLink"); //OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"MumbleLink");
	if (hMapObject == NULL)
		return;

	lm = (LinkedMem *) MapViewOfFile(hMapObject, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkedMem));
	if (lm == NULL) {
		CloseHandle(hMapObject);
		hMapObject = NULL;
		return;
	}
}

void CNRManager::UpdateMumble() 
{
	if ((!m_context) || (!m_context->mpMapMan))
	{
		return;
	}
	float avatarX, avatarY, avatarZ;
	m_context->mpMapMan->GetAvatarPosition(avatarX, avatarY, avatarZ);
	float avatarRotX, avatarRotY, avatarRotZ, avatarRotW;
	m_context->mpMapMan->GetAvatarQuaternion(avatarRotX, avatarRotY, avatarRotZ, avatarRotW);

	CQuaternion avatarRot(avatarRotX, avatarRotY, avatarRotZ, avatarRotW);

	CCamera3D* pCamera = g->cm.GetActiveCamera();
	if ( !pCamera)
		return;
	CVector3D cam_pos = pCamera->GetEyePosition();	
	CVector3D cam_dir = pCamera->GetLookAt();
	cam_dir.Normalize();

	CVector3D frontAvatarVec(1, 0, 0);
	frontAvatarVec = frontAvatarVec * avatarRot;
	frontAvatarVec.Normalize();

	if (! lm)
		return;

	if(lm->uiVersion != 2) {
		wcsncpy(lm->name, L"TestLink", 256);
		wcsncpy(lm->description, L"TestLink is a test of the Link plugin.", 2048);
		lm->uiVersion = 2;
	}
	lm->uiTick++;

	// Left handed coordinate system.
	// X positive towards "left".
	// Y positive towards "up".
	// Z positive towards "into screen".
	//
	// 1 unit = 1 meter
	frontAvatarVec /= 100.0f;
	cam_pos /= 100.0f;
	cam_dir /= 100.0f;
	avatarX /= 100.0f;
	avatarY /= 100.0f;
	avatarZ /= 100.0f;

	// Unit vector pointing out of the avatars eyes (here Front looks into scene).
	lm->fAvatarFront[0] = frontAvatarVec.x;
	lm->fAvatarFront[1] = frontAvatarVec.z;
	lm->fAvatarFront[2] = frontAvatarVec.y;

	// Unit vector pointing out of the top of the avatars head (here Top looks straight up).
	lm->fAvatarTop[0] = 0.0f;
	lm->fAvatarTop[1] = 1.0f;
	lm->fAvatarTop[2] = 0.0f;

	// Position of the avatar (here standing slightly off the origin)
	lm->fAvatarPosition[0] = avatarX;
	lm->fAvatarPosition[1] = avatarZ;
	lm->fAvatarPosition[2] = avatarY;

	// Same as avatar but for the camera.
	lm->fCameraPosition[0] = cam_pos.x;
	lm->fCameraPosition[1] = cam_pos.z;
	lm->fCameraPosition[2] = cam_pos.y;

	lm->fCameraFront[0] = cam_dir.x;
	lm->fCameraFront[1] = cam_dir.z;
	lm->fCameraFront[2] = cam_dir.y;

	lm->fCameraTop[0] = 0.0f;
	lm->fCameraTop[1] = 1.0f;
	lm->fCameraTop[2] = 0.0f;

	const wchar_t* name = m_context->mpSM->GetClientStringID();
	// Identifier which uniquely identifies a certain player in a context (e.g. the ingame Name).
	if (wcslen(name) < 256)
		wcsncpy(lm->identity, name, wcslen(name));

	// Context should be equal for players which should be able to hear each other positional and
	// differ for those who shouldn't (e.g. it could contain the server+port and team)
	memcpy(lm->context, "ContextBlob\x00\x01\x02\x03\x04", 16);
	lm->context_len = 16;
}

CNRManager::CNRManager(oms::omsContext* context):
	m_screenHeight(DEFAULT_HEIGHT),
	m_isPhysicsInitialized(false),
	m_isResolutionChanged(false),
	m_screenWidth(DEFAULT_WIDTH),
	m_renderingStarted(false),
	m_screenBPP(DEFAULT_BPP),
	rmAuthRenderedComp(NULL),
	m_resourceManager(NULL),
	rmUIRenderedComp(NULL),
	m_hintViewport(NULL),
	m_isLogEnabled(true),
	rmRenderedComp(NULL),
	m_sceneManager(NULL),
	m_soundManager(NULL),
	m_lastUpdateTime(0),
	m_updateResTime(0),
	m_context(context),
	m_updateTime(0),
	m_syncMan(NULL),
	m_nextTick(0),
	m_query(NULL),
	m_wnd(NULL),
	m_graphicQualityController(NULL),
	m_browsersCSForChildWindows(NULL),
	m_isNeedToLoadUserSettings(false),	
	MP_VECTOR_INIT(m_dynamicTexturesForDeleting),
	MP_MAP_INIT(rmCompositions),
	MP_VECTOR_INIT(m_controllers),
	MP_VECTOR_INIT(m_viewports),
	m_videoCaptureSupport(context)
{
	MP_NEW_V(m_soundMan3D, nrm3DSoundManager, this);
	
	setlocale(LC_ALL, "Russian");
	m_math3D = MP_NEW(nrmMath3D);

	gRM->SetNRManager(this);
	crmResLibrary* resLib = MP_NEW(crmResLibrary);
	gRM->SetResLibrary(resLib);

	context->mpResLoadingStat = resLib->GetResloadingStat();

	// эти ссылки не надо удалять
	CWhiteBoardManager* wbMan = MP_NEW(CWhiteBoardManager);
	MP_NEW_P(shbMan, CSharingBoardManager, context);
	CMouseController* mouseCtrl = MP_NEW(CMouseController);
	CLastScreen* lastScreen = MP_NEW(CLastScreen);
	CCameraInLocation* cameraInLocation = MP_NEW(CCameraInLocation);
	// --
	
	m_worldEditor = MP_NEW(CWorldEditor);
	m_clothEditorSupporter = MP_NEW(CClothEditorSupporter);
	MP_NEW_V(m_graphicQualityController, CGraphicQualityController, context);
	m_userSettings.AddDefaultSettingValueProvider(m_graphicQualityController);

	InitializeCriticalSection(&m_deleteDynamicTexturesCS);

	initMumble();

	m_cloudsVisibilitySize = 1000;
	m_cloudsVisibility = MP_NEW_ARR(bool, m_cloudsVisibilitySize);

	m_isNeedCaveSnapshot = false;
}

void CNRManager::SetPlayingEnabled( int iType, bool bEnabled)
{
	if ( m_soundMan3D != NULL)
	{
		m_soundMan3D->SetPlayingEnabled( iType, bEnabled);
	}
}

bool CNRManager::GetPlayingEnabled( int iType)
{
	if ( m_soundMan3D == NULL)
	{
		return true;
	}
	return m_soundMan3D->GetPlayingEnabled( iType);
}

std::vector<SResourceConsumingApplication> CNRManager::GetResourceConsumingApplications()
{
	std::vector<SResourceConsumingApplication> apps;

	const int bigCPUUsage = 5;
	std::vector<SCPUProcessInfo> list1;
	g->pi.GetProcessesWithCPUUsageHigherThan(list1, bigCPUUsage);

	const unsigned int bigMemorySize = 100 * 1024 * 1024;
	std::vector<SMemoryProcessInfo> list2;
	g->pi.GetProcessesWithMemoryUsageHigherThan(list2, bigMemorySize);

	std::vector<SCPUProcessInfo>::iterator it1 = list1.begin();
	std::vector<SCPUProcessInfo>::iterator it1End = list1.end();
	for ( ; it1 != it1End; it1++)
	{
		SResourceConsumingApplication rca((*it1).processName, (*it1).cpuUsage, 0);

		apps.push_back(rca);
	}

	std::vector<SMemoryProcessInfo>::iterator it2 = list2.begin();
	std::vector<SMemoryProcessInfo>::iterator it2End = list2.end();

	for ( ; it2 != it2End; it2++)
	{
		std::vector<SResourceConsumingApplication>::iterator it = apps.begin();
		std::vector<SResourceConsumingApplication>::iterator itEnd = apps.end();		

		bool isFound = false;

		for ( ; it != itEnd; it++)
		if ((*it).name == (*it2).processName)
		{
			(*it).memorySize  = (*it2).memorySize;
			isFound = true;
			break;
		}

		if (!isFound)
		{
			SResourceConsumingApplication rca((*it2).processName, 0, (*it2).memorySize);

			apps.push_back(rca);
		}
	}

	return apps;
}

void* CNRManager::GetTaskManager()
{
	return ((ITaskManager*)&g->taskm);
}

unsigned int CNRManager::GetCurrentFPS()
{
	if( g != NULL)
		return g->ne.GetCurrentFPS();
	return 0;
}

void CNRManager::SetLogMode(bool isWorking)
{
	m_isLogEnabled = isWorking;
}

void CNRManager::EnableLastScreen(bool isEnabled)
{
	gRM->lastScreen->Enable(isEnabled);
}

int CNRManager::GetQualityMode() 
{
	return m_graphicQualityController->GetQualityMode();
}

void CNRManager::LoadingModeChanged(bool isEnabled)
{
	if (!isEnabled)
	{
		gRM->lastScreen->Hide();
	}
}

// Для отладки: включает/выключает режим показа всех скрытых объектов
void CNRManager::ShowHidden3DObjects(bool /*abShow*/)
{
	// ??
}

std::vector<unsigned int> CNRManager::GetEngineWarningIDs()
{
	CEngineWarnings w(&m_userSettings);
	return w.GetIDs();
}

bool CNRManager::ToogleRenderState(int stateID)
{
	bool bRes = false;
	switch (stateID)
	{
	case -1:
		{
			if (g->scm.GetActiveScene())
			{
				g->scm.GetActiveScene()->GetViewport()->GetCamera()->IncOffset();
			}
			break;
		}

	case -2:
		{
			if (g->scm.GetActiveScene())
			{
				g->scm.GetActiveScene()->GetViewport()->GetCamera()->DecOffset();
			}
			break;
		}

	case 0:
		{
			bRes = !g->rs.GetBool(DRAW_ZONES_BOUNDS);
			g->rs.SetBool(DRAW_ZONES_BOUNDS, bRes);
			break;
		}
	

	/*
	case 1:
		g->cm.GetActiveCamera()->SetEyePosition(-52851.676, 9289.9531, 794.31567);
		g->cm.GetActiveCamera()->SetLookAt(-53411.023, 9333.5059, 787.23370);
		break;

	case 2:
		g->cm.GetActiveCamera()->SetEyePosition(-54143.297, 13096.017, 847.10687);
		g->cm.GetActiveCamera()->SetLookAt(-54405.578, 13591.226, 786.99530);
		break;

	case 3:
		g->cm.GetActiveCamera()->SetEyePosition(-64326.262, 14020.628, 786.83997);
		g->cm.GetActiveCamera()->SetLookAt(-64882.113, 13926.359, 787.05432);
		break;

	case 4:
		g->cm.GetActiveCamera()->SetEyePosition(-67832.813, 11657.141, 784.41199);
		g->cm.GetActiveCamera()->SetLookAt(-67368.156, 11256.531, 788.99976);
		break;

	case 5:
		g->cm.GetActiveCamera()->SetEyePosition(-66701.281, 10969.299, 789.84760);
		g->cm.GetActiveCamera()->SetLookAt(-67026.781, 10430.729, 793.00012);
		break;

	case 6:
		g->cm.GetActiveCamera()->SetEyePosition(-70122.703, 5554.6753, 836.04224);
		g->cm.GetActiveCamera()->SetLookAt(-70592.000, 6126.8418, 787.15027);
		break;

	case 7:
		g->cm.GetActiveCamera()->SetEyePosition(-75382.000, 15478.267, 431.44891);
		g->cm.GetActiveCamera()->SetLookAt(-76164.922, 15872.518, 517.40051);
		break;

	case 8:
		g->cm.GetActiveCamera()->SetEyePosition(-66028.773, 19825.750, 670.95776);
		g->cm.GetActiveCamera()->SetLookAt(-65312.020, 20498.146, 785.91644);
		break;

	case 9:
		g->cm.GetActiveCamera()->SetEyePosition(-41814.418, -2205.7681, 1311.8660);
		g->cm.GetActiveCamera()->SetLookAt(-39738.551, -964.71387, 787.16101);
		break;

	case 10:
		g->cm.GetActiveCamera()->SetEyePosition(-36427.805, 7310.5967, 944.43909);
		g->cm.GetActiveCamera()->SetLookAt(-35398.809, 9040.5293, 429.60107);
		break;
	*/
	}

	if (stateID == 48)
	{
		gRM->graphicDebug->ToogleGroundVisibility();
	}
	else if (stateID == 49)
	{
		g->lw.WriteLn(g->stat.GetLoadStatString());
	}
	else if (stateID == 50)
	{
		gRM->graphicDebug->SaveObjectVisibilityList();
	}
	else if (stateID == 51)
	{
		gRM->graphicDebug->ReloadMasks();
	}
	else if (stateID == 52)
	{
		gRM->graphicDebug->EnableTextures();
	}
	else if (stateID == 53)
	{
		gRM->graphicDebug->DisableTextures();
	}
	else if (stateID == 54)
	{
		gRM->graphicDebug->ShowTexturesList();
	}
	else if (stateID == 55)
	{
		g->sm.ReloadShaders();
	}
	else if (stateID == 56)
	{
		gRM->graphicDebug->SaveGoTo();
	}
	else if (stateID == 57)
	{
		gRM->graphicDebug->ReplayGoTo();
	}
	else if (stateID == 58)
	{
		gRM->graphicDebug->MeasureAllPathesTime();
	}

	if (stateID == 100)
	{
		m_clothEditorSupporter->ReloadClipboxs();
	}
	else if (stateID == 101)
	{
		m_clothEditorSupporter->ReloadMaterials();
	}
	else if (stateID == 102)
	{
		m_clothEditorSupporter->ShowClothes(false);
	}
	else if (stateID == 103)
	{
		m_clothEditorSupporter->ShowClothes(true);
	}
	else if (stateID == 200)
	{
		/*g->lw.WriteLn("Testing path finding");

		for (int i = 0; i < 250; i++)
		{
			CVector3D from;
			from.x = rand() / 32768.0f * 200000.0f - 100000.0f;
			from.y = rand() / 32768.0f * 200000.0f - 100000.0f;
			from.z = 1000.0f;

			CVector3D to;
			to.x = rand() / 32768.0f * 200000.0f - 100000.0f;
			to.y = rand() / 32768.0f * 200000.0f - 100000.0f;
			to.z = 1000.0f;

			CPathLine pathLine;
			g->pf.FindPath(&pathLine, from, to);
		}

		g->lw.WriteLn("Testing path finding - ok");*/
	}
	else if (stateID == 411)
	{
		g->pf.EnableDebugRendering(true);
		g->pf.SetVisible(true);
	}
	else if (stateID == 477)
	{
		gRM->graphicDebug->SaveAvatarsTextures();
	}
	else if (stateID == 488)
	{
		if ((g->phom.GetControlledObject()) && (g->phom.GetControlledObject()->GetObject3D()))
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)g->phom.GetControlledObject()->GetObject3D()->GetAnimation();
			sao->DumpMotionsInfoToLog();
		}
	}
	else if (stateID == 300)
	{
		g->taskm.PrintTaskInfo();
	}
	else if (stateID == 800)
	{		
		g->ne.SetShowFPSNeededStatus(!g->ne.IsShowFPSNeeded());
	}
	else if (stateID == 999)
	{		
		gRM->graphicDebug->CopyCameraToClipboard();
	}
	else if (stateID == 998)
	{		
		gRM->graphicDebug->PlayCameraTrack();
	}
	else if (stateID == 777)
	{		
		g->rs.SetBool(SHOW_CHECK_OCCLUSION_TARGETS, !g->rs.GetBool(SHOW_CHECK_OCCLUSION_TARGETS));
	}
	else if (stateID == 1001)
	{	
		g->rs.SetInt(NORMAL_LOD_SCALE, 100);
		g->lod.SetLODScale(1);
	}
	else if (stateID == 1002)
	{	
		g->rs.SetInt(NORMAL_LOD_SCALE, 300);
		g->lod.SetLODScale(1);
	}
	else if (stateID == 1003)
	{	
		g->rs.SetInt(NORMAL_LOD_SCALE, 400);
		g->lod.SetLODScale(1);
	}
	else if (stateID == 1004)
	{	
		g->rs.SetInt(NORMAL_LOD_SCALE, 500);
		g->lod.SetLODScale(1);
	}
	else if (stateID == 1100)
	{
		g->shdw.NeedToDumpShadowQueue();
	}
	else if (stateID == 2000)
	{	
		//m_context->mpSM->SafeCallGC(false, true);
		if (gMP) gMP->WriteToLogTotalFileInfo();
	}
	else if (stateID == 2001)
	{	
		//m_context->mpSM->SafeCallGC(false, true);
		if (gMP) gMP->WriteToLogDetailedFileInfo();
	}
	else if (stateID == 2002)
	{	
		//m_context->mpSM->SafeCallGC(false, true);
		if (gMP) gMP->WriteToLogTotalFileInfoWithDiference();
		gRM->graphicDebug->SaveObjectsComparisonPoint();
	}
	else if (stateID == 2003)
	{	
		//m_context->mpSM->SafeCallGC(false, true);
		if (gMP) gMP->WriteToLogFunctionsInfo();
	}
	else if (stateID == 2004)
	{
		gRM->graphicDebug->CompareObjectsWithSavedPoint();
	}
	else if (stateID == 2005)
	{
		gRM->graphicDebug->MakeUrlToClipboard();
	}
	else if (stateID == 1091)
	{
		if (g->rs.GetBool(FIX_TIME_OF_DAY_ENABLED))
		{
			g->ne.ZeroProgramTime();
			g->rs.SetBool(FIX_TIME_OF_DAY_ENABLED, false);
		}
		else
		{
			g->rs.SetBool(FIX_TIME_OF_DAY_ENABLED, true);
		}
	}
	else if (stateID == 4096)
	{	
		g->fm.SaveAllFontsTextures();
	}
	else if (stateID == 5000)
	{
		if ((g->phom.GetControlledObject()) && (g->phom.GetControlledObject()->GetObject3D()))
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)g->phom.GetControlledObject()->GetObject3D()->GetAnimation();
			sao->DumpKinectInfoToLog();
		}	
	}
	else if (stateID == 6001)
	{
		if ((g->phom.GetControlledObject()) && (g->phom.GetControlledObject()->GetObject3D()))
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)g->phom.GetControlledObject()->GetObject3D()->GetAnimation();
			sao->ReloadKinectSettings();
		}
	}
	else if (stateID == 3321)
	{
		gRM->scene2d->SetVisible(!gRM->scene2d->IsVisible());
	}
	else if (stateID == 3322)
	{
		m_isNeedCaveSnapshot = true;
	}

	return bRes;
}

unsigned char CNRManager::GetMaxAntialiasingPower()
{
	return g->gi.GetMaxAntialiasingPower();
}

ILogWriter* CNRManager::GetLogWriter()
{
	if (!g)
	{
		return NULL;
	}
		
	return m_context->mpLogWriter;
}

void CNRManager::ReloadUserSetting()
{
	m_userSettings.LoadAll();
	m_userSettings.LoadGlobal();
	g->rs.SetInt(INTERACTIVE_BOARD_MONITOR_ID, rtl_atoi(m_userSettings.GetSetting(USER_SETTING_DISPLAYID_FOR_INTERACTIVE_BOARD).c_str()));
}

bool CNRManager::GetUserSetting(wchar_t* _name, char* apwcValue, unsigned short aSizeValue)
{
	USES_CONVERSION;
	std::string name = W2A(_name);
	std::string val = "";	

	if (aSizeValue < 1)
		return false;

	apwcValue[0] = '\0';

	if ("quality" == name)
	{		
		val = IntToStr(m_graphicQualityController->GetQualityMode());
	}
	else if ("vsync" == name)
	{
		val = (g->rs.GetBool(VSYNC_ENABLED)) ? "true" : "false";		
	}
	else if ("distance_koef" == name)
	{
		val = IntToStr(g->rs.GetInt(NORMAL_LOD_SCALE));
	}	
	else
	{
		val = m_userSettings.GetSetting(name);
	}

	if(val.size() == 0)
		return false;

	if(val.size() < aSizeValue){
		rtl_strcpy(apwcValue, aSizeValue, val.c_str());
	}else{
		strncpy(apwcValue, val.c_str(), aSizeValue - 1);
		apwcValue[aSizeValue - 1] = '\0';
	}

	return true;
}

int CNRManager::SetUserSetting(wchar_t* _name, wchar_t* _value)
{
	USES_CONVERSION;
	std::string name = W2A(_name);
	std::string value = W2A(_value);

	bool bRes = false;
	if ("distance_koef" == name)
	{
		int distanceKoef = rtl_atoi(value.c_str());
		bRes = m_userSettings.SaveSetting(USER_SETTING_DISTANCE_KOEF, IntToStr(distanceKoef));
		
		g->rs.SetInt(NORMAL_LOD_SCALE, distanceKoef);
		g->lod.SetLODScale(1);
	}
	else if ("show_clouds" == name)
	{		
		m_userSettings.SaveSetting(USER_SETTING_SHOW_CLOUDS, value);
		g->rs.SetBool(SHOW_AVATAR_CLOUDS, value == "true");		
	}
	else if ("quality" == name)
	{
		int qualityMode = rtl_atoi(value.c_str());
		bRes = m_userSettings.SaveSetting(USER_SETTING_QUALITY_MODE, IntToStr(qualityMode));
		int lodScale = g->rs.GetInt(NORMAL_LOD_SCALE);
		int res = m_graphicQualityController->SetQualityMode(qualityMode);
		g->rs.SetInt(NORMAL_LOD_SCALE, lodScale);

		return res;
	}
	else if ("multisample_power" == name)
	{
		if (m_graphicQualityController->GetPostEffect())
		{
			int power = m_graphicQualityController->AdjustMultisamplePower(rtl_atoi(value.c_str()));
			m_graphicQualityController->GetPostEffect()->SetMultisamplePower(power);
			g->rs.SetInt(MSAA, power);
		}
		m_userSettings.SaveSettingWithDelay(USER_SETTING_MULTISAMPLE_POWER, value, 20000);
	}
	else if ("vsync" == name)
	{
		bRes = m_userSettings.SaveSetting(name, (value == "1") ? "true" : "false");
		g->rs.SetBool(VSYNC_ENABLED, value == "1");
		g->ne.UpdateVSync();
	}
	else if ("password" == name)
	{		
		if ( value.size() > 0)
		{	
			GetEncodingPassword(value);
			bRes = m_userSettings.SaveSetting(name, value, true);
		}
		else
			bRes = m_userSettings.SaveSetting(name, value, true);
	}
	else if ("disableWarning" == name)
	{
		bRes = m_userSettings.SaveSetting("disable_" + value, "true", true);
	}
	else if ("enableWarning" == name)
	{
		bRes = m_userSettings.SaveSetting("enable_" + value, "true");
	}
	else 
	{
		bRes = m_userSettings.SaveSetting(name, value, true);
	}
	if( bRes)
		m_userSettings.SaveAllExludeGlobalSettings();

	return 0;
}

void CNRManager::GetEncodingPassword(std::string& value)
{
	char *sEncodingValue = MP_NEW_ARR(char, md5::MD5_BUFFERSIZE);				
	//rtl_strcpy(	sEncodingValue, strlen(sEncodingValue), value.c_str());
	md5::CalculateMD5( (BYTE *) value.c_str(), &sEncodingValue, md5::MD5_BUFFERSIZE);
	std::string valueForSalting = value;
	value = sEncodingValue;
	std::string AA = md5::MakeSaltedHash(valueForSalting);
	value += "|"; value += AA;
	MP_DELETE_ARR(sEncodingValue);
}

void CNRManager::GetEncodingPassword(std::wstring& value)
{
	USES_CONVERSION;
	std::string svalue = W2A(value.c_str());
	GetEncodingPassword( svalue);
	value = A2W(svalue.c_str());
}


int CNRManager::SetActiveCameraToScene(void* _scene)
{
	CScene* scene = (CScene*)_scene;

	nrm3DScene* scene3d = (nrm3DScene*)scene->GetUserData(USER_DATA_NRMOBJECT);
	if (scene3d)
	{
		if ((scene3d->m_mapManager) && (!scene3d->m_mapManager->IsRender3DNeeded()))
		{
			if ((scene3d->m_scene) && (scene3d->m_scene->GetViewport()))
			{
				g->cm.SetActiveCamera(scene3d->m_scene->GetViewport()->GetCamera());
			}

			if (scene3d->m_mapManager->IsPhysicsNeeded())
			{
				return -1;
			}

			return 0;
		}
		else if ((scene3d->m_mapManager) && (scene3d->m_mapManager->IsRender3DNeeded()))
		{
			g->ne.UpdateVSync();
		}
	}

	return 1;
}

cm::cmICommunicationManager* CNRManager::GetComMan()
{
	return m_comManager;
}
 
omsresult CNRManager::CreateMO(mlMedia* apMLMedia)
{
	if (apMLMedia==NULL) return OMS_ERROR_INVALID_ARG;
	
	nrmObject*	pnrmObject = NULL;
	if (apMLMedia->GetType()==MLMT_ACTIVEX)
	{
		MP_NEW_P(obj, nrmActiveX, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
	}
	else if (apMLMedia->GetType()==MLMT_BROWSER)
	{
		MP_NEW_P(obj, nrmBrowser, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
	}
	else if (apMLMedia->GetType()==MLMT_FLASH)
	{
		MP_NEW_P(obj, nrmFlash, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
	}
	else if (apMLMedia->GetType()==MLMT_PATH3D)
	{
		MP_NEW_P(obj, nrmPath3D, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_PLUGIN)
	{
		MP_NEW_P(obj, nrmPlugin, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
	}
	else if (apMLMedia->GetType()==MLMT_VIDEO)
	{
		MP_NEW_P(obj, nrmVideo, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
		
	}
	else if (apMLMedia->GetType()==MLMT_AUDIO || apMLMedia->GetType()==MLMT_SPEECH)
	{
		MP_NEW_P(obj, nrmAudio, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
		m_soundMan3D->UpdateSound( (nrmAudio*) pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_IMAGE)
	{
		MP_NEW_P(obj, nrmImage, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_ANIMATION)
	{
		MP_NEW_P(obj, nrmAnim, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_TEXT)
	{
		MP_NEW_P(obj, nrmText, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_LINE)
	{
		MP_NEW_P(obj, nrmLine, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_OBJECT)
	{
		MP_NEW_P(obj, nrm3DObject, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
		
	}
	/*else if (apMLMedia->GetType()==MLMT_COMMINICATION_AREA)
	{
		MP_NEW_P(obj, nrm3DObject, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
	}*/
	else if (apMLMedia->GetType()==MLMT_CHARACTER)
	{
		MP_NEW_P(obj, nrmCharacter, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
		
	}
	else if (apMLMedia->GetType()==MLMT_CAMERA)
	{
		MP_NEW_P(obj, nrmCamera, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
		
	}
	else if (apMLMedia->GetType()==MLMT_VIEWPORT)
	{
		MP_NEW_P(obj, nrmViewport, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
		if (m_soundMan3D != NULL)
			m_soundMan3D->add_registry((nrmViewport *) pnrmObject);

	}
	else if (apMLMedia->GetType()==MLMT_LIGHT)
	{
		MP_NEW_P(obj, nrmLight, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
		
	}
	else if (apMLMedia->GetType()==MLMT_SCENE3D)
	{
		MP_NEW_P(obj, nrm3DScene, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		CopyToLibrary(apMLMedia);
		
	}
	else if (apMLMedia->GetType()==MLMT_GROUP)
	{
		MP_NEW_P(obj, nrm3DGroup, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_MOTION)
	{
		MP_NEW_P(obj, nrmMotion, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_VISEMES)
	{
		MP_NEW_P(obj, nrmVisemes, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_INPUT)
	{
		MP_NEW_P(obj, nrmEdit, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
		
	}
	else if (apMLMedia->GetType()==MLMT_CLOUD)
	{
		MP_NEW_P(obj, nrmCloud, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		apMLMedia->SetMO(pnrmObject);
	}
	else if (apMLMedia->GetType()==MLMT_IMAGE_FRAME)
	{
		MP_NEW_P(obj, nrmImageFrame, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);

	}
	else if (apMLMedia->GetType()==MLMT_PARTICLES)
	{
		MP_NEW_P(obj, nrmParticles, apMLMedia);
		pnrmObject = obj;
		assert(apMLMedia->GetSafeMO()==NULL);
		pnrmObject->SetMLMedia(apMLMedia);
		apMLMedia->SetMO(pnrmObject);
	}
	else
		return OMS_ERROR_NOT_SUPPORTED;

	if (pnrmObject != NULL)
	{
		pnrmObject->SetRenderManager(this);

		if (apMLMedia->GetType()==MLMT_SCENE3D)
		{
			OnCreateWorldScene((nrm3DScene*)pnrmObject);
		}

		pnrmObject->mType = apMLMedia->GetType();
	}

	
	return OMS_OK;
}

void CNRManager::GetOSVersion(unsigned int* apuMajor,  unsigned int* apuMinor,  unsigned int* apuBuild)
{
	g->ci.GetOSVersion( apuMajor,  apuMinor,  apuBuild);
}

void CNRManager::MLMediaDeleting(mlMedia* apMLMedia){
	DeleteFromLibrary(apMLMedia);
}

omsresult CNRManager::DestroyMO(moMedia* apMOMedia)
{
	if (apMOMedia == NULL) 
	{
		return OMS_ERROR_INVALID_ARG;
	}

	if (apMOMedia->GetType()==MLMT_IMAGE)
	{
		nrmImage* img = (nrmImage*)apMOMedia;
		MP_DELETE(img);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_TEXT)
	{
		nrmText* text = (nrmText*)apMOMedia;
		MP_DELETE(text);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_PATH3D)
	{
		nrmPath3D* path = (nrmPath3D*)apMOMedia;
		MP_DELETE(path);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_PLUGIN)
	{
		nrmPlugin* plugin = (nrmPlugin*)apMOMedia;
		MP_DELETE(plugin);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_VIDEO)
	{
		nrmVideo* video = (nrmVideo*)apMOMedia;
		MP_DELETE(video);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_ACTIVEX)
	{
		nrmActiveX* activeX = (nrmActiveX*)apMOMedia;
		MP_DELETE(activeX);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_BROWSER)
	{
		nrmBrowser* browser = (nrmBrowser*)apMOMedia;
		MP_DELETE(browser);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_FLASH)
	{
		nrmFlash* flash = (nrmFlash*)apMOMedia;
		MP_DELETE(flash);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_ANIMATION)
	{
		nrmAnim* animation = (nrmAnim*)apMOMedia;
		MP_DELETE(animation);

		return OMS_OK;
	}
	if ((apMOMedia->GetType()==MLMT_AUDIO) || (apMOMedia->GetType()==MLMT_SPEECH))
	{
		nrmAudio* audio = (nrmAudio *)apMOMedia;
		m_soundMan3D->RemoveSound(audio);
		if ( audio != NULL)
		{
			audio->Destroy();
		}

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_OBJECT)
	{
		nrm3DObject* obj = (nrm3DObject*)apMOMedia;
		MP_DELETE(obj);

		return OMS_OK;
	}
	/*if (apMOMedia->GetType()==MLMT_COMMINICATION_AREA)
	{
		nrm3DObject* obj = (nrm3DObject*)apMOMedia;
		MP_DELETE(obj);
		return OMS_OK;
	}*/
	if (apMOMedia->GetType()==MLMT_MOTION)
	{
		nrmMotion* motion = (nrmMotion*)apMOMedia;
		MP_DELETE(motion);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_LINE)
	{
		nrmLine* line = (nrmLine*)apMOMedia;
		MP_DELETE(line);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_CAMERA)
	{
		nrmCamera* camera = (nrmCamera*)apMOMedia;
		MP_DELETE(camera);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_VIEWPORT)
	{
		nrmViewport* viewport = (nrmViewport*)apMOMedia;
		if (m_soundMan3D != NULL)
		{
			m_soundMan3D->un_registry(viewport);
		}
		MP_DELETE(viewport);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_CHARACTER)
	{
		nrmCharacter* character = (nrmCharacter*)apMOMedia;
		MP_DELETE(character);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_VISEMES)
	{
		nrmVisemes* visemes = (nrmVisemes*)apMOMedia;
		MP_DELETE(visemes);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_GROUP)
	{
		nrm3DGroup* group = (nrm3DGroup*)apMOMedia;
		MP_DELETE(group);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_SCENE3D)
	{
		nrm3DScene* scene3d = (nrm3DScene*)apMOMedia;
		OnDestroyWorldScene(scene3d);
		MP_DELETE(scene3d);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_LIGHT)
	{
		nrmLight* light = (nrmLight*)apMOMedia;
		MP_DELETE(light);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_INPUT)
	{
		nrmEdit* edit = (nrmEdit*)apMOMedia;
		MP_DELETE(edit);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_CLOUD)
	{
		nrmCloud* cloud = (nrmCloud*)apMOMedia;
		MP_DELETE(cloud);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_IMAGE_FRAME)
	{
		nrmImageFrame* imageFrame = (nrmImageFrame*)apMOMedia;
		MP_DELETE(imageFrame);

		return OMS_OK;
	}
	if (apMOMedia->GetType()==MLMT_PARTICLES)
	{
		nrmParticles* particles = (nrmParticles*)apMOMedia;
		MP_DELETE(particles);

		return OMS_OK;
	}

	return OMS_ERROR_NOT_SUPPORTED;
}

omsresult CNRManager::DestroyMO(mlMedia* apMLMedia)
{
	if (apMLMedia==NULL) 
	{
		return OMS_ERROR_INVALID_ARG;
	}

	MLMediaDeleting(apMLMedia);

	moMedia* pMO = apMLMedia->GetSafeMO();

	if (pMO)
	{
		omsresult result = DestroyMO(pMO);

		if (result != OMS_ERROR_NOT_SUPPORTED)
			apMLMedia->SetMO(NULL);
		else
			assert(false);

		return result;
	}

	return OMS_ERROR_NOT_SUPPORTED;
}

// уничтожить плагин, отвязанный ранее от mlMeida
oms::omsresult CNRManager::DestroyPlugin(moMedia* apPlugin){
	nrmPlugin* plugin = (nrmPlugin*)apPlugin;
	MP_DELETE(plugin);
	return OMS_OK;
}

void CNRManager::CreateMapManager()
{
	if (m_context->mpMapMan != NULL)
		return;

	MP_NEW_P2(map_manager, CMapManager, m_context, this);
	m_context->mpMapMan = (cmIMapManager*)map_manager;
	if (m_worldEditor != NULL)
		m_worldEditor->SetMapManager(map_manager);
}

void CNRManager::DestroyMapManager()
{
	if (m_context->mpMapMan == NULL)
		return;

	MP_DELETE_UNSAFE( (CMapManager*)m_context->mpMapMan);
	m_context->mpMapMan = NULL;
}

void CNRManager::OnCreateWorldScene(nrm3DScene* aWorldScene)
{
	if (aWorldScene == NULL)
		return;
	if (m_context == NULL)
		return;
	if (m_context->mpMapMan == NULL)
		return;
	
	mlMedia* pMLMedia = aWorldScene->GetMLMedia();
	if (pMLMedia == NULL)
		return;
	if (pMLMedia->GetIScene3D() == NULL)
		return;
	if (!pMLMedia->GetIScene3D()->GetUseMapManager())
		return;

	aWorldScene->SetMapManager((CMapManager*)m_context->mpMapMan);
	((CMapManager*)m_context->mpMapMan)->SetScene(aWorldScene->m_scene);
}

void CNRManager::OnDestroyWorldScene(nrm3DScene* aWorldScene)
{
	if (aWorldScene == NULL)
		return;
	if (m_context == NULL)
		return;
	if (m_context->mpMapMan == NULL)
		return;
	((CMapManager*)m_context->mpMapMan)->SetScene(NULL);

	mlMedia* pMLMedia = aWorldScene->GetMLMedia();
	if (pMLMedia == NULL)
		return;
	if (pMLMedia->GetIScene3D() == NULL)
		return;
	if (!pMLMedia->GetIScene3D()->GetUseMapManager())
		return;
	aWorldScene->SetMapManager(NULL);
}

bool CNRManager::StencilBufferExists()
{
	return true;
}

ml3DRotation CNRManager::multiplyRotations(ml3DRotation &rot1, ml3DRotation &rot2)
{
	ml3DRotation rotRes;
	m_math3D->quatMult(rot1, rot2, rotRes);
	return rotRes;
}

oms::omsresult CNRManager::SaveMedia(const mlMedia* /*apMLMedia*/, const wchar_t* /*apwcPath*/)
{
	// ??
	/*mlMedia*	pMLMedia = (mlMedia*)apMLMedia;
	int	type = pMLMedia->GetType();
	if (type == MLMT_IMAGE)
	{
		nrmObject*	object = (nrmObject*)pMLMedia->GetSafeMO();
		object->SaveMedia(apwcPath);
		return OMS_OK;
	}
	// try to save file
	USES_CONVERSION;
	mlMedia* mpMLMedia = (mlMedia*)apMLMedia;
	assert(m_resLibrary);
	if (!m_resLibrary)	return OMS_ERROR_FAILURE;
	if (!mpMLMedia->GetILoadable())	return OMS_ERROR_FAILURE;
	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	if (!pwcSrc)	return OMS_ERROR_FAILURE;
	ifile* inputFile = m_resLibrary->get_res_file(CComString(W2A(pwcSrc)));
	if (!inputFile)	return OMS_ERROR_FAILURE;
	ifile* outputFile = m_resLibrary->get_res_file_to_write(CComString(W2A(apwcPath)), FALSE);
	if (!outputFile)
	{
		m_resLibrary->delete_res_file(inputFile);
		return OMS_ERROR_FAILURE;
	}
	DWORD inputSize = inputFile->get_file_size();
	if (inputSize > 0)
	{
		LPVOID	mem = malloc(inputSize);
		assert(mem);
		if (mem)
		{
			inputFile->read((BYTE*)mem, inputSize);
			UINT wrote;
			outputFile->write((const TCHAR*)mem, inputSize, &wrote);
			//assert(wrote == inputSize);
		}
	}
	m_resLibrary->delete_res_file(inputFile);
	m_resLibrary->delete_res_file(outputFile);*/
	return OMS_OK;
}

BOOL IsMinimized(HWND hwnd)
{
	DWORD	style;
	style = ::GetWindowLong(hwnd, GWL_STYLE);
	return (style&WS_MINIMIZE)==WS_MINIMIZE?TRUE:FALSE;
}

void CNRManager::AutoFix16bitDisplays()
{
	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	std::string displayName = "\\\\.\\Display ";
	for (int displayID = 1; displayID <= 3; displayID++)
	{		
		displayName[displayName.size() - 1] = '0' + displayID;
		ZeroMemory(&dm, sizeof(dm));

		BOOL res = EnumDisplaySettings(displayName.c_str(), ENUM_CURRENT_SETTINGS, &dm);	
	
		if ((32 != dm.dmBitsPerPel) && (res))
		{
			dm.dmBitsPerPel = 32;
		
			LONG res = ChangeDisplaySettingsEx(displayName.c_str(), &dm, NULL, 0, NULL);
			if (res == DISP_CHANGE_SUCCESSFUL)
			{
				EnumDisplaySettings(displayName.c_str(), ENUM_CURRENT_SETTINGS, &dm);
				while (32 != dm.dmBitsPerPel)
				{
					Sleep(1);
					EnumDisplaySettings(displayName.c_str(), ENUM_CURRENT_SETTINGS, &dm);
				}
			}
		}
	}
}

bool CNRManager::SetControlWindow(HWND ahWnd, const RECT& arcRect)
{	
	m_context->mpLogWriter->WriteLn("SetControlWindow-1");

	AutoFix16bitDisplays();

	wchar_t wcsLanguage[100] = L"";
	m_context->mpApp->GetLanguage((wchar_t*)wcsLanguage, 99);
	bool isEnglish = (wcscmp(wcsLanguage, L"eng") == 0);
	gRM->lang = (isEnglish) ? LANGUAGE_ENGLISH : LANGUAGE_RUSSIAN;

	m_context->mpLogWriter->WriteLn("SetControlWindow-2");

	InitEngine();

	m_context->mpLogWriter->WriteLn("SetControlWindow-3");

	int sceneID = g->sc2d.AddObject("main_2d_scene");	
	gRM->Set2DScene(g->sc2d.GetObjectPointer(sceneID));

	if (gMP) gMP->SetLogWriter(m_context->mpLogWriter);
	
	g->ne.SetMainThreadCoreID(0);

	m_context->mpLogWriter->SetApplicationDataDirectory( GetApplicationDataDirectory().c_str());
	g->SetLogWriter( m_context->mpLogWriter);

	g->lw.WriteLn("SetControlWindow-4");

	g->pi.EnumerateAllProcesses();

	g->lw.WriteLn("SetControlWindow-5");

	if( m_context->mp3DWS->GetApplicationRoot() != NULL)
	{
		std::wstring sRoot( m_context->mp3DWS->GetApplicationRoot());
		SetApplicationRootDirectory( sRoot);		
	}

	m_userSettings.SetIsOfficial( g->gi.IsOfficialVersion());
	m_userSettings.SetLogWriter( m_context->mpLogWriter);

	g->lw.WriteLn("SetControlWindow-6");

	m_userSettings.LoadGlobal();
	g->rs.SetInt(INTERACTIVE_BOARD_MONITOR_ID, rtl_atoi(m_userSettings.GetSetting(USER_SETTING_DISPLAYID_FOR_INTERACTIVE_BOARD).c_str()));

	g->tp.Init();
	g->tp.Start();
	g->ne.SetLoadedState(true);

	g->lw.WriteLn("SetControlWindow-7");

	stat_openGLInitTime = (int)g->tp.GetTickCount();

	if (!m_isLogEnabled)
	{
		m_context->mpLogWriter->SetMode(m_isLogEnabled);
	}

	assert(m_wnd == 0);
	m_wnd = ahWnd;
	m_wndRect = arcRect;
	m_lastTick = GetTickCount();
	m_context->mp3DWS->SendMessage2Launcher( APP_PERCENT, 18);

	g->SetResLibrary(gRM->resLib);
	char pchValue[5] = "";
	gRM->nrMan->GetContext()->mpApp->GetSettingFromIni("settings", "resloader_time_logging", pchValue, 3);
	if (strcmp(pchValue, "1") == 0)
	{
		gRM->resLib->GetAsynchResMan()->EnableTimeLogging(true);
	}	

	gRM->nrMan->GetContext()->mpApp->GetSettingFromIni("settings", "local_scripts", pchValue, 3);
	if (strcmp(pchValue, "1") == 0)
	{
		gRM->resLib->GetAsynchResMan()->SetLocalScripts(true);
	}	

	m_cave.Init();

	HWND helperWindow = FindWindowEx(ahWnd, NULL, NULL, "3DWS");
	g->ne.helperWnd = helperWindow;
	g->ne.ghWnd = ahWnd;
	g->ne.SetBackgroundColor(0.5f, 0.5f, 0.5f, 1.0f);	

	m_context->mp3DWS->SendMessage2Launcher( APP_PERCENT, 21);
	g->ne.LoadSettings();
	g->ne.SetMouseSettings(false);
	m_context->mp3DWS->SendMessage2Launcher( APP_PERCENT, 26);
	g->stp.SetSize(0, 0, arcRect.right - arcRect.left, arcRect.bottom - arcRect.top);

	g->ne.SetSettings();
	
	g->gi.Analyze();
	g->rs.CheckSettings();

	g->cc.InitFromXML(GetApplicationRootDirectory());

	m_context->mp3DWS->SendMessage2Launcher( APP_PERCENT, 33);

	// hack to avoid "context loss" on Intel, which doesn't exist in OpenGL specifications,
	// but exists in Intel drivers...
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		HMENU hMenuHandle = GetSystemMenu(g->ne.ghWnd, FALSE);
		if (hMenuHandle != NULL) 
		{
			DeleteMenu(hMenuHandle, SC_MINIMIZE, MF_BYCOMMAND);			 

		}
		SendMessage(g->ne.ghWnd, WM_INTEL_VIDEO_FOUND, 0, 0);
		unsigned int style = GetWindowLong(g->ne.ghWnd, GWL_EXSTYLE);
		style += WS_EX_TOPMOST;
		SetWindowLong(g->ne.ghWnd, GWL_EXSTYLE, style);
	}
	else if (g->gi.GetVendorID() == VENDOR_NVIDIA)
	{
		SendMessage(g->ne.ghWnd, WM_NVIDIA_VIDEO_FOUND, 0, 0);
	}

	stat_openGLInitTime = (int)g->tp.GetTickCount() - stat_openGLInitTime;

	if ((g->gi.GetVendorID() == VENDOR_INTEL) || (g->gi.GetVendorID() == VENDOR_SIS) || (!g->cc.AreExtendedAbilitiesSupported()))
	{
		g->rs.SetBool(SHADOWS_ENABLED, false);
	}

	m_context->mp3DWS->SendMessage2Launcher( APP_PERCENT, 38);

	stat_resourcesPreloadTime = (int)g->tp.GetTickCount();

	g->fm.ScanDirectoryForFonts(GetFontsDirectory());

	g->cmm.LoadAll();	
	g->mm.LoadNeighbors();
	m_context->mp3DWS->SendMessage2Launcher( APP_PERCENT, 45);
	g->tm.LoadAllExternal();

	m_context->mp3DWS->SendMessage2Launcher( APP_PERCENT, 50);

	int vendorCheckAnswer = g->cc.IsVendorSupported();
	
	if (vendorCheckAnswer == DRIVER_INVALID)
	{
		SendMessage(g->ne.ghWnd, WM_HIDE_LAUNCHER_NOW, 0, 0);	
		if (isEnglish)
		{
			MessageBox(NULL, "Your video driver is not compatible with the Virtual Academy. Probably, drivers for your videocard are outdated!", "Checking configuration", MB_ICONERROR);
		}
		else
		{
			MessageBox(NULL, "Ваш видеодрайвер не совместим с Виртуальной Академией.", "Проверка конфигурации", MB_ICONERROR);
		}
		return false;
	}

	if (vendorCheckAnswer == VENDOR_OK)
	{
		g->t3dm.LoadAll();
	}
	g->matm.LoadAll();
	
	stat_resourcesPreloadTime = (int)g->tp.GetTickCount() - stat_resourcesPreloadTime;

	g->taskm.Init();

	bool isVSyncEnabled = g->rs.GetBool(VSYNC_ENABLED);
	g->rs.SetBool(VSYNC_ENABLED, false);
	g->ne.UpdateVSync();
	g->rs.SetBool(VSYNC_ENABLED, isVSyncEnabled);

	if (g->rs.GetBool(SHADERS_USING))
	{
		g->fp.LoadAll();
	}

	CreateMapManager();

	m_worldEditor->Run();
	m_context->mp3DWS->SendMessage2Launcher( APP_PERCENT, 55);

	g->cg.LogProcesses(g->lw.GetILogWriter());

	gRM->minSysReq->Init();

	SendMessage(g->ne.ghWnd, WM_CREATE_LOGIN_WND, 0, 0);	

	/*std::string url; unsigned char t;
	g->gi.GetDriverLink(url, t);*/

	//g->rs.SetInt(CAVE_SUPPORT, true);
	
	return true;
}

rm::ICodeEditorSupport* CNRManager::GetCodeEditorSupport()
{
	return &m_codeEditorSupport;
}

bool CNRManager::IsCaveMode()
{
	return g->rs.GetBool(CAVE_SUPPORT);
}

void CNRManager::SetScreenModeForOculus(unsigned long adwWidth, unsigned long adwHeight, unsigned long x, unsigned long y) {
	__int64 t1 =g->tp.GetTickCount();

	HWND hwnd = g->ne.ghWnd;
	RECT rc, border, winRect;
	GetClientRect(hwnd, &rc);
	GetWindowRect(hwnd, &winRect);
	ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rc.left));
	ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rc.right));
	border.top = rc.top - winRect.top;
	border.left = rc.left - winRect.left;
	border.right = winRect.right - rc.right;
	border.bottom = winRect.bottom - rc.bottom;

	SendMessage(hwnd, WM_OCULUS_WANTS_TO_CHANGE_THE_RESOLUTION, 0, 0);
	MoveWindow(hwnd, 
		x - border.left, 
		y - border.top, 
		adwWidth + border.left + border.right,
		adwHeight + border.top + border.bottom, false);

	m_screenWidth = adwWidth;
	m_screenHeight = adwHeight;

	g->ne.Resize(m_screenWidth, m_screenHeight);
	g->stp.SetSize(0, 0, m_screenWidth, m_screenHeight);

	g->ne.BeginFrame(true);
	gRM->scene2d->Draw();
	g->ne.EndFrame();

	m_videoCaptureSupport.OnScreenSizeChanged();

	//////////////////////////////////////////////////////////////////////////
	// WE
	if (m_worldEditor->GetConnectionState())
	{
		m_worldEditor->RelinkWindow();
	}
	//////////////////////////////////////////////////////////////////////////

	if (m_screenWidth != INITIAL_PLAYER_WIDTH)
	{
		gRM->loadingCtrl->OnWindowResized();
	}
    
	__int64 t2 =g->tp.GetTickCount();

	__int64 t3 = t2-t1;
}

bool CNRManager::SetScreenMode(unsigned long adwWidth, unsigned long adwHeight, unsigned long adwBPP, bool isFullScreen)
{
	if (g->rs.GetBool(CAVE_SUPPORT))
	if (adwWidth > 512)
	{
		int offsetX, w, h;
		m_context->mpApp->GetWideDesktopParams(w, h, offsetX);
		adwWidth = w;
		adwHeight = h;
	}

	__int64 t1 =g->tp.GetTickCount();

	int offsetX, desktopWidth, desktopHeight;
	m_context->mpApp->GetDesktopSize(offsetX, desktopWidth, desktopHeight);

	if (!g->rs.GetBool(CAVE_SUPPORT))
	if ((adwWidth > (unsigned int)desktopWidth) || (adwHeight > (unsigned int)desktopHeight))
	{
		return false;
	}

	if ((m_screenWidth == adwWidth) && (m_screenHeight == adwHeight))
	{
		return false;
	}

	g->lw.WriteLn("[SetScreenMode] adwWidth = ", adwWidth, " adwHeight = ", adwHeight);

	HMONITOR hMonitor;
	RECT rct;
	rct.left = offsetX;
	rct.top = 0;
	rct.bottom = desktopHeight;
	rct.right = offsetX + desktopWidth;

    hMonitor = MonitorFromRect(&rct, MONITOR_DEFAULTTONEAREST);

	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	RECT rctAll;
	GetClientRect(g->ne.ghWnd, &rct);
	GetWindowRect(g->ne.ghWnd, &rctAll);
	unsigned int captionHeight = (rctAll.bottom - rctAll.top) - (rct.bottom - rct.top);

	g->lw.WriteLn("[SetScreenMode] mi.rcWork.bottom = ", mi.rcWork.bottom, " mi.rcWork.top = ", mi.rcWork.top);
	g->lw.WriteLn("[SetScreenMode] captionHeight = ", captionHeight, " desktopHeight = ", desktopHeight);

	if (!g->rs.GetBool(CAVE_SUPPORT))
	if (mi.rcWork.bottom - mi.rcWork.top - captionHeight < desktopHeight)
	{
		desktopHeight = mi.rcWork.bottom - mi.rcWork.top - captionHeight;
		g->lw.WriteLn("[SetScreenMode] after (1) desktopHeight = ", desktopHeight);
	}

	if (!g->rs.GetBool(CAVE_SUPPORT))
	if (mi.rcWork.right - mi.rcWork.left < desktopWidth)
	{
		desktopWidth = mi.rcWork.right - mi.rcWork.left;		
	}

	if (!g->rs.GetBool(CAVE_SUPPORT))
	if (adwWidth >= (unsigned int)desktopWidth)
	{		
		adwWidth = mi.rcWork.right - mi.rcWork.left;
		
		g->lw.WriteLn("[SetScreenMode] after (2) adwWidth = ", adwWidth);
	}			

	if (adwHeight >= (unsigned int)desktopHeight)
	{		
		adwHeight = mi.rcWork.bottom - mi.rcWork.top;
		adwHeight -= captionHeight;

		g->lw.WriteLn("[SetScreenMode] after (3) adwHeight = ", adwHeight);
	}

	if ((m_screenWidth > INITIAL_PLAYER_WIDTH) && (adwWidth > INITIAL_PLAYER_WIDTH))
	{
		m_isResolutionChanged = true;
	}

	bool isSizeChanged = ((m_screenWidth != adwWidth) || (m_screenHeight != adwHeight));
	m_screenWidth = adwWidth;
	m_screenHeight = adwHeight;
	m_screenBPP = adwBPP;

	DWORD size = (DWORD)adwWidth + (((DWORD)adwHeight)<<16);
	::SendMessage(g->ne.ghWnd, WM_ADJUST_CONTROL_WND, N3D_MESSAGE_CODE, size);

	g->ne.Resize(m_screenWidth, m_screenHeight);
	g->stp.SetSize(0, 0, m_screenWidth, m_screenHeight);

	g->ne.BeginFrame(true);
	gRM->scene2d->Draw();
	g->ne.EndFrame();

	if (isSizeChanged)
	{
		m_videoCaptureSupport.OnScreenSizeChanged();
	}

	//////////////////////////////////////////////////////////////////////////
	// WE
	if (m_worldEditor->GetConnectionState())
	{
		m_worldEditor->RelinkWindow();
	}
	//////////////////////////////////////////////////////////////////////////

	if (m_screenWidth != INITIAL_PLAYER_WIDTH)
	{
		gRM->loadingCtrl->OnWindowResized();
	}
    
	__int64 t2 =g->tp.GetTickCount();

	__int64 t3 = t2-t1;
    
	return true;
}

void CNRManager::SetSoundManager(rm::rmISoundManager* apSoundManager)
{
	m_soundManager = apSoundManager;
}

void CNRManager::SetCommunicationManager(cm::cmICommunicationManager* apComManager)
{
	m_comManager = apComManager;

	gRM->resLib->set_communication_manager(apComManager);

	assert(!m_query);
	MP_NEW_V(m_query, nrmQuery, apComManager);
}

void CNRManager::SetSceneManager(mlISceneManager* apmlISceneManager)
{
	m_sceneManager = apmlISceneManager;
}

void CNRManager::SetResourceManager(res::resIResourceManager* apResourceManager)
{
	m_resourceManager = apResourceManager;
	assert(gRM->resLib);
	gRM->resLib->GetAsynchResMan()->SetILogWriter(m_context->mpLogWriter);
	gRM->resLib->set_resource_manager(m_resourceManager);
}

void CNRManager::StartLoadingResource()
{
	g->bm.AsyncPreloadBlends();
	g->motm.AsyncPreloadMotions();
	CTexturesPreloader texturesPreloader;
	texturesPreloader.LoadAll();
}

void CNRManager::DeleteDynamicTexture(IDynamicTexture* dynamicTexture)
{
	EnterCriticalSection(&m_deleteDynamicTexturesCS);
	m_dynamicTexturesForDeleting.push_back(dynamicTexture);
	LeaveCriticalSection(&m_deleteDynamicTexturesCS);
}

void CNRManager::RemoveDynamicTextures()
{
	EnterCriticalSection(&m_deleteDynamicTexturesCS);
	
	std::vector<IDynamicTexture *>::iterator it = m_dynamicTexturesForDeleting.begin();
	std::vector<IDynamicTexture *>::iterator itEnd = m_dynamicTexturesForDeleting.end();

	for ( ; it != itEnd; it++)
	{
		g->tm.RemoveDynamicTexture(*it);
	}

	m_dynamicTexturesForDeleting.clear();

	LeaveCriticalSection(&m_deleteDynamicTexturesCS);
}

/************************************************************************/
/* Main rendering cycle
/************************************************************************/

void CNRManager::UpdateAndRender()
{
	g->pm.StartFrame();
	
	UpdateMumble();

	m_context->mpSyncMan->ProcessSyncMessages();

	g->pm.FrameBlockPerformed(FRAME_BLOCK_CHECK_MAP_MESSAGES);

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-1");
#endif

	g->ne.BeginFrame(true);

	g->pm.FrameBlockPerformed(FRAME_BLOCK_BEGIN_FRAME);

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-2");
#endif

	gRM->mouseCtrl->OnNewFrame();

	m_graphicQualityController->Update();

	unsigned int delta = (unsigned int)(g->ne.time - m_lastUpdateTime);
	std::vector<IController *>::reverse_iterator it = m_controllers.rbegin();
	std::vector<IController *>::reverse_iterator itEnd = m_controllers.rend();	

	for (; it != itEnd; it++)
	{
		(*it)->Update(delta);
	}

	g->pm.FrameBlockPerformed(FRAME_BLOCK_CONTROLLERS_UPDATE);

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-3");
#endif

	RemoveDynamicTextures();

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-4");
#endif

	RenderScene();

	g->pm.FrameBlockPerformed(FRAME_BLOCK_RENDER_SCENE);

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-5");
#endif

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-6");
#endif

	m_lastUpdateTime = g->ne.time;
	
	if ((g->ne.time - m_updateTime > g->rs.GetInt(COMMON_DELTA_TIME)) ||
		(!m_renderingStarted))
	{
		DWORD newTick = 0;
		if (m_nextTick != 0)	
		{
			newTick = m_nextTick;
		}
		else
		{
			newTick = GetTickCount();
		}

		m_lastTick = newTick;
		int iDeltaTime = -1;
		m_nextTick = 0;

		m_sceneManager->Update((__int64)newTick * 1000, iDeltaTime, !gRM->lastScreen->IsVisible());
		m_cave.Update();

		if (m_context->mpSharingMan)
			m_context->mpSharingMan->Update();

		mlMedia* objUnderMouse = m_sceneManager->GetVisibleUnderNouse();
		if (objUnderMouse)
		{
			if ((objUnderMouse->GetI3DObject()) ||
				(objUnderMouse->GetI3DGroup()) ||
				(objUnderMouse->GetIViewport()) ||
				(objUnderMouse->GetICamera()))
			{
				gRM->mouseCtrl->HandleMouse(true);
			}	
			else	
			{
				gRM->mouseCtrl->HandleMouse(false);
			}
		}
		else
		{
			gRM->mouseCtrl->HandleMouse(false);	
		}

		m_updateTime = g->ne.time;
	}

	g->pm.FrameBlockPerformed(FRAME_BLOCK_RMML_UPDATE);

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-7");
#endif

	gRM->mouseCtrl->UpdateMouseInfoIfNeeded();

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-8");
#endif

	g->ne.EndFrame();

	g->pm.FrameBlockPerformed(FRAME_BLOCK_END_FRAME);

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-9");
#endif

	
	m_worldEditor->Update();
	
#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-10");
#endif
	
	if ((g->ne.time - m_updateResTime > (int)(g->rs.GetInt(COMMON_DELTA_TIME) / 4)) ||
		(g->ne.time < m_updateResTime))
	{
		if (gRM->resLib && gRM->resLib->GetAsynchResMan() != NULL)
		{
			gRM->resLib->GetAsynchResMan()->Update();
		}

		g->pm.FrameBlockPerformed(FRAME_BLOCK_RESLOADER_UPDATE);

		m_updateResTime = g->ne.time;
		m_soundMan3D->update();
	}
	else
	{
		g->pm.FrameBlockPerformed(FRAME_BLOCK_RESLOADER_UPDATE);
	}

	g->pm.FrameBlockPerformed(FRAME_BLOCK_SOUNDMAN_UPDATE);

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::UpdateAndRender()-11");
#endif

	LoadUserSettingsIfNeeded();	

	m_videoCaptureSupport.Update();

	g->pm.EndFrame();
}

void CNRManager::LoadUserSettingsIfNeeded()
{
	m_userSettings.Update();
	if (m_isNeedToLoadUserSettings)
	{
		m_userSettings.SetUserStringID( GetContext()->mpSM->GetClientStringID());
		m_userSettings.LoadAll();

		if (m_userSettings.GetSetting(USER_SETTING_QUALITY_MODE) != "")
		{
			m_graphicQualityController->TryToSetQualityMode(rtl_atoi(m_userSettings.GetSetting(USER_SETTING_QUALITY_MODE).c_str()));
		}
		else
		{
			m_graphicQualityController->TryToSetInitialQualityMode();			
		}

		if (m_userSettings.GetSetting(USER_SETTING_SHOW_CLOUDS) == "false")
		{
			g->rs.SetBool(SHOW_AVATAR_CLOUDS, false);
		}

		if (m_userSettings.GetSetting(USER_SETTING_VSYNC_ENABLED) != "")
		{
			g->rs.SetInt(VSYNC_ENABLED, (m_userSettings.GetSetting(USER_SETTING_VSYNC_ENABLED) == "true"));
			g->ne.UpdateVSync();
		}

		if (m_userSettings.GetSetting(USER_SETTING_DISTANCE_KOEF) != "")
		{
			g->rs.SetInt(NORMAL_LOD_SCALE, rtl_atoi(m_userSettings.GetSetting(USER_SETTING_DISTANCE_KOEF).c_str()));
			g->ne.UpdateVSync();
		}

		if (m_userSettings.GetSetting(USER_SETTING_MULTISAMPLE_POWER) != "")
		{
			int power = rtl_atoi(m_userSettings.GetSetting(USER_SETTING_MULTISAMPLE_POWER).c_str());
			if ((power != 1) && (power != 2) && (power != 4) && (power != 8) && (power != 16))
			{
				power = 1;
			}
			if (power > g->gi.GetMaxAntialiasingPower())
			{
				power = g->gi.GetMaxAntialiasingPower();
			}
			power = m_graphicQualityController->AdjustMultisamplePower(power);
			if (m_graphicQualityController->GetPostEffect())
			{
				m_graphicQualityController->GetPostEffect()->SetMultisamplePower(power);
				g->rs.SetInt(MSAA, power);
			}
		}

		if ((m_userSettings.GetSetting(USER_SETTING_KINECT_LEFT_CORNER_X) != "") &&
			(m_userSettings.GetSetting(USER_SETTING_KINECT_LEFT_CORNER_Y) != "") &&
			(m_userSettings.GetSetting(USER_SETTING_KINECT_LEFT_CORNER_Z) != ""))
		{
			CVector3D pnt(StringToFloat(m_userSettings.GetSetting(USER_SETTING_KINECT_LEFT_CORNER_X).c_str()),
				StringToFloat(m_userSettings.GetSetting(USER_SETTING_KINECT_LEFT_CORNER_Y).c_str()),
				StringToFloat(m_userSettings.GetSetting(USER_SETTING_KINECT_LEFT_CORNER_Z).c_str()));
			CGlobalKinect::GetInstance()->SetDeskLeftCorner(pnt);
		}

		if ((m_userSettings.GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_X) != "") &&
			(m_userSettings.GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_Y) != "") &&
			(m_userSettings.GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_Z) != ""))
		{
			CVector3D pnt(StringToFloat(m_userSettings.GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_X).c_str()),
				StringToFloat(m_userSettings.GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_Y).c_str()),
				StringToFloat(m_userSettings.GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_Z).c_str()));
			CGlobalKinect::GetInstance()->SetDeskRightCorner(pnt);
		}

		/* затычка через один два выпущенных релиза убрать*/
		if(m_context->mpVoipMan != NULL)
		{
			m_context->mpVoipMan->CorrectOldSettings();
			m_userSettings.SaveAllExludeGlobalSettings();
		}
		//////////////////////////////////////////////////////////////////////////

		m_isNeedToLoadUserSettings = false;
	}
}

omsresult CNRManager::CopyToLibrary(mlMedia *apMLMedia)
{
	mlMedia*	pScene = apMLMedia->GetScene();
	CRMCompLib*	compLib = GetCompLib(pScene);
	if (compLib)
		return compLib->AddObject(apMLMedia);
	else
		return OMS_ERROR_FAILURE;
}
	
omsresult CNRManager::DeleteFromLibrary(mlMedia *apMLMedia)
{
	std::map<mlMedia*, CRMCompLib*>::iterator	it;
	omsresult	oRes;
	for(it=rmCompositions.begin(); it!=rmCompositions.end(); it++)
		if ((oRes=it->second->RemoveObject(apMLMedia))==OMS_OK || oRes==OMS_ERROR_ILLEGAL_VALUE)
			break;

	if (apMLMedia->GetICloud() == NULL)
		return OMS_OK;

	return OMS_OK;
}

CRMCompLib* CNRManager::GetCompLib(mlMedia *apScene)
{
	if (!apScene)	return NULL;
	CRMCompLib*	pComposition = NULL;
	std::map<mlMedia*, CRMCompLib*>::iterator	it;
	if ((it=rmCompositions.find(apScene)) == rmCompositions.end())
	{
		MP_NEW_V(pComposition, CRMCompLib, this);
		rmCompositions.insert(std::map<mlMedia*, CRMCompLib*>::value_type(apScene, pComposition));
	}else
		pComposition = it->second;
	return pComposition;
}

void CNRManager::ShowHint(nrmViewport *vp, BOOL show)
{
	if (show)
		m_hintViewport = vp;
	else
		if (m_hintViewport == vp)
			m_hintViewport = NULL;
}

void CNRManager::SaveCloudsVisibility()
{
	std::vector<void*>& clouds = GetLinks();
	int cloudCount = clouds.size();

	if (cloudCount > m_cloudsVisibilitySize)
	{
		MP_DELETE_ARR(m_cloudsVisibility);
		m_cloudsVisibility = MP_NEW_ARR(bool, cloudCount * 2);
		m_cloudsVisibilitySize = cloudCount * 2;
	}
	int cloudID = 0;

	std::vector<void*>::iterator it = clouds.begin();
	std::vector<void*>::iterator itEnd = clouds.end();
	for ( ; it != itEnd; it++, cloudID++)
	{
		nrmCloud* cloud = (nrmCloud *)(*it);
		m_cloudsVisibility[cloudID] = cloud->IsVisible();
	}
}

void CNRManager::RestoreCloudsVisibility()
{
	if (!m_cloudsVisibility)
	{
		return;
	}

	std::vector<void*>& clouds = GetLinks();
	int cloudID = 0;

	std::vector<void*>::iterator it = clouds.begin();
	std::vector<void*>::iterator itEnd = clouds.end();
	for ( ; it != itEnd; it++, cloudID++)
	{
		nrmCloud* cloud = (nrmCloud *)(*it);
		cloud->SetVisible(m_cloudsVisibility[cloudID]);
	}
}

void CNRManager::ProcessClouds()
{
	if (g->ne.isIconicAndIntel)
	{
		return;
	}

	bool isCameraChanged = (((m_lastEyePos - g->cm.GetActiveCamera()->GetEyePosition()).GetLengthSq() > 1.0f) ||
		((m_lastLookAt - g->cm.GetActiveCamera()->GetLookAt()).GetLengthSq() > 1.0f));

	if (isCameraChanged)
	{
		m_lastEyePos = g->cm.GetActiveCamera()->GetEyePosition();
		m_lastLookAt = g->cm.GetActiveCamera()->GetLookAt();
	}

	if ((g->ne.time - m_updateTime < g->rs.GetInt(COMMON_DELTA_TIME)) ||
		((isCameraChanged) && (g->ne.time - m_updateTime < g->rs.GetInt(COMMON_DELTA_TIME) * 10)))
	{
		return;
	}

	std::vector<nrmViewport *>::iterator it = m_viewports.begin();
	std::vector<nrmViewport *>::iterator itEnd = m_viewports.end();

	for (; it != itEnd; it++)
	if ((*it)->GetBooleanProp("visible") == true)
	{
		nrmViewport*	prmViewport = *it;

		if (prmViewport)
		if (prmViewport->m_viewport->GetCamera())
		{
			float mainDepth = (float)prmViewport->GetMLMedia()->GetIVisible()->GetAbsDepth();

			std::vector<mlMedia*> mlClouds;
			
			std::vector<void*> clouds = GetLinks();
			mlClouds.reserve(clouds.size());

			std::vector<void*>::iterator it = clouds.begin();
			std::vector<void*>::iterator itEnd = clouds.end();

			for (; it != itEnd; it++)
			{
				nrmCloud* cloud = (nrmCloud*)(*it);
				mlMedia* pMedia = cloud->GetMLMedia();
				if (pMedia == NULL)
				{
					continue;
				}

				mlMedia* pOurViewport = pMedia->GetParentElement();
				if (pOurViewport == NULL)
				{
					continue;
				}

				if ((pOurViewport == prmViewport->GetMLMedia()) || (pOurViewport->GetIViewport() == NULL))
				if (pMedia->GetICloud())	
				{
					ml3DPosition pos = cloud->GetPos();
					float distSq = prmViewport->m_viewport->GetCamera()->GetPointDistSq((float)pos.x, (float)pos.y, (float)pos.z);
					cloud->SetDistSq(distSq);

					cloud->SetActiveViewport(prmViewport);
					cloud->SetScreenSize(m_screenWidth, m_screenHeight);
					cloud->update(0, NULL);

					if (cloud->IsVisible())
					{
						mlClouds.push_back(pMedia);
					}
				}
			}
			
			for (unsigned int x = 0; x < mlClouds.size(); x++)
			{
				int l = x;

				nrmCloud* cloud = (nrmCloud *)(mlClouds[x]->GetSafeMO()->GetICloud());

				for (unsigned int y = x + 1; y < mlClouds.size(); y++)
				{
					nrmCloud* _cloud = (nrmCloud *)(mlClouds[y]->GetSafeMO()->GetICloud());
					
					if (_cloud->GetDistSq() > cloud->GetDistSq())
					{
						l = y;
					}
				}

				mlMedia* tmp = mlClouds[x];
				mlClouds[x] = mlClouds[l];
				mlClouds[l] = tmp;
			}

			std::vector<mlMedia*>::iterator iter = mlClouds.begin();
			std::vector<mlMedia*>::iterator iterEnd = mlClouds.end();

			for ( ; iter != iterEnd; iter++)
			{
				nrmCloud* cloud = (nrmCloud *)((*iter)->GetSafeMO()->GetICloud());

				if (cloud->IsSortOnDepth())
				{
					mainDepth -= 3.0f;

					cloud->SetDepth(mainDepth);
				}
			}
		}
	}
}

bool CNRManager::IsComputerSlow()
{
	static bool res = g->gi.IsLowEndVideocard();

	return res;
}

void CNRManager::RenderScene(bool isPostEffectNeeded)
{	
#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::RenderScene()-1");
#endif

	if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
	{
		UpdateCompLibs((unsigned int)(g->ne.time - m_lastUpdateTime));
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::RenderScene()-2");
#endif

	int qualityMode = m_graphicQualityController->GetQualityMode();
	glColor3ub(255, 255, 255);

	g->cm.SetNearPlane(10.0f);
	g->cm.SetFarPlane(125000.0f);
	g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45));

	CViewport* viewport = NULL;
	
	for (int i = 0; i < g->scm.GetCount(); i++)
	{
		CScene* scene = g->scm.GetObjectPointer(i);
		if (scene->IsDeleted())
		{
			continue;
		}

		m_renderingStarted = true;

		int res = SetActiveCameraToScene(scene);
		if (res != 1)
		{
			if (-1 == res)
			{
				m_isPhysicsInitialized = true;
				if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
				{
					g->phom.Update();
				}
			}
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		if(m_worldEditor->GetConnectionState() && m_worldEditor->DoSelectClickedObject())
		{
			CViewport* viewport = scene->GetViewport();
			if (viewport != NULL)
			{
				if(m_worldEditor->DoPlaceObjectByMouse())
				{
					nrmObject* nrmObj = (nrmObject*)viewport->GetUserData(USER_DATA_NRMOBJECT);
					if (nrmObj)
					{
						mlMedia* mlViewport = nrmObj->GetMLMedia();
						if (mlViewport)
						{
							if (mlViewport->GetSafeMO())
							{	
								nrmViewport* moViewport = (nrmViewport*)mlViewport->GetSafeMO();
								mlPoint aMouseXY;
								POINT tmp;
								GetCursorPos(&tmp);
								ScreenToClient(g->ne.ghWnd, &tmp);
								aMouseXY.x = tmp.x;
								aMouseXY.y = tmp.y;
								ml3DPosition aMouseXYZ;
								moViewport->GetObjectUnderMouse(aMouseXY, false, &aMouseXYZ);
								CVector3D p((float)aMouseXYZ.x, (float)aMouseXYZ.y, (float)aMouseXYZ.z);
								m_worldEditor->CreateNewObjectByMouse(&p);
							}
						}					
					}
				}
				else
				{
					C3DObject* selectedObject = viewport->GetObjectUnderMouse();

					if (selectedObject != NULL)
					{
						m_worldEditor->SendMouseSelectedObject(selectedObject);
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////

		m_graphicQualityController->CreateSkyIfNeeded(scene);

		g->scm.SetActiveScene(i);
		if (isPostEffectNeeded)
		{
			scene->SetPostEffect(m_graphicQualityController->GetPostEffect());
			m_updateTime = g->ne.time - 1000;
		}

		m_isPhysicsInitialized = true;
		gRM->loadingCtrl->OnNew3DFrame();
		if(m_worldEditor->GetConnectionState())
		{
#pragma warning(push)
#pragma warning(disable : 4239)			
			scene->Render(m_worldEditor->GetActiveObjects()->GetObjects());
#pragma warning(pop)

			if(m_worldEditor->IsNeedRefresh())
			{
				m_worldEditor->RefreshObjects();
			}

			m_worldEditor->ProcessCommand();
		}
		else
		{
			if (g->rs.GetBool(CAVE_SUPPORT))
			{
				static int caveFloorWidth = -1;
				static int caveFloorHeight = -1;
				static int caveOffset = -1;

				if ((caveFloorHeight == -1) || (m_isNeedCaveSnapshot))
				{
					std::wstring iniFN = GetApplicationRootDirectory();
					iniFN += L"\\player.ini";
					CIniFile ini(iniFN);
					caveOffset = rtl_atoi(ini.GetStringWithDefault("settings", "cave_floor_offset", "0").c_str());
					caveFloorHeight = rtl_atoi(ini.GetStringWithDefault("settings", "cave_floor_height", "1050").c_str());
					caveFloorWidth = rtl_atoi(ini.GetStringWithDefault("settings", "cave_floor_width", "1100").c_str());
				}

				g->rs.SetInt(FOV, 90);
				viewport = scene->GetViewport();
				int oldViewportHeight = viewport->GetHeight();
				CCamera3D* camera = viewport->GetCamera();
				CCamera3D* saved = camera->SaveState(); 
				int width = viewport->GetWidth();
				for (int i = 0; i < 4; i++)
				{										
					if (i != 3)
					{
						viewport->SetOffset(i * width / 4, 0);
						viewport->SetSize(width / 4, viewport->GetHeight());
					}
					else
					{									
						viewport->SetOffset(i * width / 4 + caveOffset, 0);
						viewport->SetSize(caveFloorWidth, caveFloorHeight);
					}

					if ((g->phom.GetControlledObject()) && (g->phom.GetControlledObject()->GetObject3D()) && (g->phom.GetControlledObject()->GetObject3D()->GetLODGroup()))
					{
						g->phom.GetControlledObject()->GetObject3D()->GetLODGroup()->SetVisibleAll(false);
					}

					CVector3D delta = camera->GetLookAt() - camera->GetEyePosition();
					delta.z = 0;
					CVector3D pos = camera->GetLookAt();
					if (i == 0)
					{
						delta.RotateZ(90.0f);
						delta.z = 0;
						camera->SetEyePosition(pos);												
						camera->SetLookAt(pos + delta);												
					}
					if (i == 1)
					{
						camera->SetEyePosition(pos);												
						camera->SetLookAt(pos + delta);																					
					}
					if (i == 2) 
					{
						delta.RotateZ(-90.0f);
						delta.z = 0;
						camera->SetEyePosition(pos);												
						camera->SetLookAt(pos + delta);											
					}
					if (i == 3)
					{
						camera->SetEyePosition(pos);												
						camera->SetLookAt(pos + CVector3D(0, 0, -delta.GetLength()));																	
						delta.Normalize();
						camera->SetUp(-delta.x, -delta.y, 0);
					}

					if ((g->phom.GetControlledObject()) && (g->phom.GetControlledObject()->GetObject3D()) && (g->phom.GetControlledObject()->GetObject3D()->GetLODGroup()))
					{
						g->phom.GetControlledObject()->GetObject3D()->GetLODGroup()->SetVisibleAll(false);
					}

					scene->Render();					

					/*if (m_isNeedCaveSnapshot)
					{
						std::wstring fn = GetApplicationDataDirectory();
						fn += L"\\";
						fn += IntToWStr(i);
						fn += L".jpg";
						SCropRect rect;
						rect.x = 0;
						rect.y = 0;
						rect.width = width / 4;
						rect.height = viewport->GetHeight();			

						m_graphicQualityController->GetPostEffect()->SaveScreen(fn, &rect);					
					}*/
					
					camera->RestoreState(saved);		
					camera->SetUp(0, 0, 1);
				}

				if (m_isNeedCaveSnapshot)
				{
					m_isNeedCaveSnapshot = false;
				}

				viewport->SetOffset(0, 0);
				viewport->SetSize(width, oldViewportHeight);
			}
			else
			{ 
				scene->Render();
			}
		}

		viewport = scene->GetViewport();
	}
	 
#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::RenderScene()-3");
#endif

	if (viewport)
	{
		g->stp.SetViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());
	}
	else
	{
		g->stp.SetViewport(0, 0, m_screenWidth, m_screenHeight);
	}
	
	gRM->scene2d->Draw();

	if ((m_isResolutionChanged) && (m_renderingStarted))
	{
		if ((qualityMode == LOW_QUALITY_MODE_ID) || (g->gi.GetVendorID() == VENDOR_INTEL)  || (g->gi.GetVendorID() == VENDOR_SIS))
		{
			gRM->lastScreen->CopyLastScreen();
		}
		gRM->lastScreen->TakeLastScreen();
		m_isResolutionChanged = false;
	}

#ifdef CRASH_SEARCH
	g->lw.WriteLn("CNRManager::RenderScene()-4");
#endif
}

void CNRManager::RegisterController(IController* controller)
{
	m_controllers.push_back(controller);
}

void CNRManager::UnregisterController(IController* controller)
{
	std::vector<IController *>::iterator it = m_controllers.begin();
	std::vector<IController *>::iterator itEnd = m_controllers.end();

	for (; it != itEnd; it++)
	if (*it == controller)
	{
		m_controllers.erase(it);
		break;
	}
}

void CNRManager::RegisterViewport(nrmViewport* viewport)
{
	m_viewports.push_back(viewport);
}

void CNRManager::UnregisterViewport(nrmViewport* viewport)
{
	std::vector<nrmViewport *>::iterator it = m_viewports.begin();
	std::vector<nrmViewport *>::iterator itEnd = m_viewports.end();

	for (; it != itEnd; it++)
	if (*it == viewport)
	{
		m_viewports.erase(it);
		break;
	}
}

void CNRManager::UpdateCompLibs(DWORD delta)
{
	mlMedia*	pUIScene = m_sceneManager->GetUI();
	mlMedia*	pActiveScene = m_sceneManager->GetActiveScene();
	mlMedia*	pAuthScene = m_sceneManager->GetAuthScene();
	CRMCompLib*	pRmLib;
	CRMCompLib*	pUIRmLib;
	CRMCompLib*	pAuthRmLib;

	if (pUIScene && rmUIRenderedComp != pUIScene){
		rmUIRenderedComp = pUIScene;
		pUIRmLib = GetCompLib(rmUIRenderedComp);
		assert(pUIRmLib);
		pUIRmLib->init();
	}else
		pUIRmLib = GetCompLib(rmUIRenderedComp);

	if (pActiveScene && rmRenderedComp != pActiveScene) {
		rmRenderedComp = pActiveScene;
		pRmLib = GetCompLib(rmRenderedComp);
		assert(pRmLib);
		pRmLib->init();
	}else
	{
		if (!pActiveScene)
			rmRenderedComp = NULL;
		pRmLib = GetCompLib(rmRenderedComp);
	}

	if (pAuthScene && rmAuthRenderedComp != pAuthScene){
		rmAuthRenderedComp = pAuthScene;
		pAuthRmLib = GetCompLib(rmAuthRenderedComp);
		assert(pAuthRmLib);
		pAuthRmLib->init();
	}else
		pAuthRmLib = GetCompLib(rmAuthRenderedComp);

	// Update all 3d elements
	if (pUIRmLib)
		pUIRmLib->update(delta);
	if (pRmLib)
		pRmLib->update(delta);
	if (pAuthRmLib)
		pAuthRmLib->update(delta);
}

void CNRManager::SaveBitmap(LPCTSTR /*fileName*/, HBITMAP /*hBitmap*/)
{
}

int	 CNRManager::GetFilesFromCFG(int /*aiType*/, char* /*apszCFGFileName*/, char* /*appszFileNames*/, int /*bufCount*/)
{
	return 0;
}

oms::omsContext* CNRManager::GetContext()
{
	return m_context;
}

void CNRManager::OnExit()
{
	if (m_renderingStarted)
	if ((m_graphicQualityController->GetQualityMode() == LOW_QUALITY_MODE_ID) || (g->gi.GetVendorID() == VENDOR_INTEL) || (g->gi.GetVendorID() == VENDOR_SIS))
	if (g->scm.GetActiveScene())
	{
		gRM->lastScreen->CopyLastScreen();		
	}
}

void CNRManager::PrepareForDestroy()
{
	m_codeEditorSupport.OnDestroy();
	m_videoCaptureSupport.EndCapture();
	if (m_renderingStarted)
	{
		gRM->lastScreen->TakeLastScreen();
	}
	g->ne.PrepareForDestroy();
	m_userSettings.SaveDelayedSettingsImmediately();
}

void* CNRManager::GetCSForWindow(void* ahWnd){
	TCHAR lpcClassName[200] = _T("\0");
	int iRet = GetClassName((HWND)ahWnd, lpcClassName, 195);
	if(iRet > 0 && _tcscmp(lpcClassName, "SWFlash_PlaceholderX") == 0){
		return m_browsersCSForChildWindows;
	}
	return NULL;
}

IWorldEditor* CNRManager::GetWorldEditor()
{
	return m_worldEditor;
}

IMouseController* CNRManager::GetMouseController()
{
	return gRM->mouseCtrl;
}

CNRManager::~CNRManager()
{
	g->ne.SetWorkMode(WORK_MODE_DESTROY);
	DestroyMapManager();

	if (m_graphicQualityController != NULL)
	{
		MP_DELETE(m_graphicQualityController);
		m_graphicQualityController = NULL;
	}

	g->Destroy();

	std::map<mlMedia*, CRMCompLib*>::iterator it;
	for (it = rmCompositions.begin(); it != rmCompositions.end(); it++)
	{
		MP_DELETE(it->second);
	}
	rmCompositions.clear();

	if (gRM->resLib != NULL)
	{
		MP_DELETE(gRM->resLib);
		gRM->resLib = NULL;
	}

	if (m_math3D != NULL)
	{
		MP_DELETE(m_math3D);
		m_math3D = NULL;
	}

	if (m_soundMan3D != NULL)
	{
		MP_DELETE(m_soundMan3D);
		m_soundMan3D = NULL;
	}

	if (m_query != NULL)
	{
		MP_DELETE(m_query);
		m_query = NULL;
	}

	MP_DELETE(m_worldEditor);
	MP_DELETE_ARR(m_cloudsVisibility);
	m_cloudsVisibility = NULL;
}

bool CNRManager::FindFreePlace(ml3DPosition& pos, ml3DBounds& bounds, ml3DRotation& /*rot*/, int maxDist, ml3DPosition& posRes, const wchar_t* location)
{
	if (!m_isPhysicsInitialized)
	{
		return false;
	}

	const float MUL_KOEF = 1.05f;

	bounds.xMin *= MUL_KOEF;
	bounds.xMax *= MUL_KOEF;
	bounds.yMin *= MUL_KOEF;
	bounds.yMax *= MUL_KOEF;
	bounds.zMin *= MUL_KOEF;
	bounds.zMax *= MUL_KOEF;

	USES_CONVERSION;
	std::string locationID;
	if(location != NULL && *location != L'\0')
		locationID = W2A(location);

	float size = (float)(bounds.xMax - bounds.xMin);
	if (size < (float)(bounds.yMax - bounds.yMin))
	{
		size = (float)(bounds.yMax - bounds.yMin);
	}

	const int MAX_POINT_STEPS = 10;
	const int HALF_MAX_POINT_STEPS = (int)(MAX_POINT_STEPS / 2); 
	const int MAX_STEPS = 20;
	const float BASE_RADUIS_STEP = 30.0f;
	const float RADIUS_ADD_VALUE = 3.0f;

	for (int r = 0; r < MAX_STEPS; r++)
	{
		float radius = BASE_RADUIS_STEP * (float)r;
		for (int l = 0; l < r; l++)
		{
			radius += l * RADIUS_ADD_VALUE;
		}

		if(maxDist >=0 && r > 0 && radius > (maxDist + size))
			break;

		int maxAngles = 1;
		if (r != 0)
		{
			maxAngles = 8;
		}

		for (int angleID = 0; angleID < maxAngles; angleID++)
		{
			float testPointX = (float)pos.x + cosf(angleID * 45.0f / 57.0f) * radius;
			float testPointY = (float)pos.y + sinf(angleID * 45.0f / 57.0f) * radius;
			if(locationID[0] != L'\0'){
				SLocationInfo* locInfo = gRM->mapMan->GetLocationByXYZ(testPointX, testPointY, (float)pos.z);
				if(locInfo == NULL || locationID != locInfo->name)
					continue;
			}
			if (g->phom.IsOnlyGroundExistsAtPoint(testPointX, testPointY, true))
			{
				bool res = true;

				float centerX = (float)(bounds.xMax + bounds.xMin) * 0.5f;
				float centerY = (float)(bounds.yMax + bounds.yMin) * 0.5f;

				for (int x = -HALF_MAX_POINT_STEPS; x < HALF_MAX_POINT_STEPS; x++)
				for (int y = -HALF_MAX_POINT_STEPS; y < HALF_MAX_POINT_STEPS; y++)
				{
					CVector3D addP((float)(bounds.xMax - bounds.xMin) * (float)x * 0.5f / HALF_MAX_POINT_STEPS,
						(float)(bounds.yMax - bounds.yMin)* (float)y * 0.5f / HALF_MAX_POINT_STEPS, 0);					

					CVector3D pnt(testPointX + centerX, testPointY + centerY, 0);
					pnt += addP;

					if ((!g->phom.IsOnlyGroundExistsAtPoint(pnt.x, pnt.y, true)) ||
						(!g->phom.IsGroundExistsAtPoint(pnt.x, pnt.y)))
					{
						res = false;
						break;
					}
					
					// если угол границ объекта не попадает в нужную локацию
					if(locationID[0] != L'\0'){
						SLocationInfo* locInfo = gRM->mapMan->GetLocationByXYZ(pnt.x, pnt.y, (float)pos.z);
						if(locInfo == NULL || locationID != locInfo->name){
							res = false; // то считаем размещение неподходящим
							break;
						}
					}
				}

				if (res)
				{
					posRes.x = testPointX;
					posRes.y = testPointY;
					posRes.z = pos.z;

					return true;
				}
			}
		}
	}

	return false;
}
 

// возвращает строковый идентификатор локации, в которой находится 3D-объект
const char* CNRManager::GetObjectLocation(mlMedia* apMLMedia){
	mlI3DObject* pML3DO = apMLMedia->GetI3DObject();
	if(pML3DO == NULL)
		return NULL;
	mlMedia* pML3DScene = pML3DO->GetScene3D();
	if(pML3DScene == NULL)
		return NULL;
	nrm3DScene* scene3d = (nrm3DScene*)pML3DScene->GetSafeMO();
	if(!scene3d || !scene3d->m_mapManager)
		return NULL;
	moMedia* pMOMedia = apMLMedia->GetSafeMO();
	if (!pMOMedia)
		return NULL;
	nrm3DObject* p3DObject = (nrm3DObject*)pMOMedia;
	if(p3DObject->m_obj3d == NULL)
		return NULL;
	SLocationInfo* pLocInfo = scene3d->m_mapManager->GetObjectLocation(p3DObject->m_obj3d);
	if(pLocInfo == NULL)
		return NULL;
	return pLocInfo->name.c_str();
}

// возвращает строковый идентификатор общения (зоны общения), в которой находится 3D-объект
const char* CNRManager::GetObjectCommunicationArea(mlMedia* apMLMedia){
	mlI3DObject* pML3DO = apMLMedia->GetI3DObject();
	if(pML3DO == NULL)
		return NULL;
	mlMedia* pML3DScene = pML3DO->GetScene3D();
	if(pML3DScene == NULL)
		return NULL;
	nrm3DScene* scene3d = (nrm3DScene*)pML3DScene->GetSafeMO();
	if(!scene3d || !scene3d->m_mapManager)
		return NULL;
	std::string sAreaName = scene3d->m_mapManager->GetObjectCommunicationAreaName(apMLMedia);
	return sAreaName.c_str();
}

void CNRManager::SetAudioMode( wchar_t* sType, int iMode)
{
	m_soundMan3D->SetAudioMode( sType, iMode);
}

void CNRManager::SetAudioVolume( wchar_t* sType, float fVolume)
{
	m_soundMan3D->SetAudioVolume( sType, fVolume);
}

void CNRManager::LoadAndApplyUserSettings()
{
	static bool isLoaded = false;
	if (isLoaded)
	{
		return;
	}
	else
	{
		isLoaded = true;
	}

	m_isNeedToLoadUserSettings = true;	
	gRM->loadingCtrl->OnLoginSuccess();
}

void CNRManager::OnMemoryOverflow()
{
	g->lw.WriteLn("OnMemoryOverflow");

	if (gRM->mapMan)
	{
		g->lw.WriteLn("OnMemoryOverflow MapMan resources clearing");
		gRM->mapMan->OnMemoryOverflow();		
		g->lw.WriteLn("OnMemoryOverflow MapMan resources clearing - ok");
	}	

	g->taskm.ExecuteAllTasks();
}

desktop::IDesktopSharingDestination* CNRManager::GetDesktopSharingDestination(char* alpcTarget)
{
	if( !m_context->mpSM)
		return NULL;

	USES_CONVERSION;
	mlMedia* pObject = m_context->mpSM->GetObjectByTarget( A2W( alpcTarget));
	if( !pObject)
		return NULL;

	if( pObject->GetType() != MLMT_IMAGE)
		return NULL;

	nrmImage* pImage = (nrmImage*)pObject->GetSafeMO();
	if( NULL == pImage)
		return NULL;

	return pImage;
}

// код ошибки при отмены авторизации
void CNRManager::OnLogout( int /*iCode*/)
{
}

bool CNRManager::IsMotionPreloadedEnd()
{
	return g->motm.IsMotionsPreloaded();
}

bool CNRManager::IsBlendsPreloadedEnd()
{	
	return g->bm.IsBlendsPreloaded();
}

void CNRManager::GetComputerInfo( std::wstring &compName, std::string &processName, unsigned int &processorCount, unsigned int &realCoreCount, bool &hyperthreadingEnabled, unsigned __int64 &physicalMemorySize, unsigned __int64 &virtualMemorySize, std::string &osVersion, unsigned char &osBits)
{
	USES_CONVERSION;
	compName = A2W(g->ci.GetCompName().c_str());
	processName = g->ci.GetProcessorName();
	processorCount = g->ci.GetProcessorCount();
	realCoreCount = g->ci.GetRealCoreCount();
	hyperthreadingEnabled = g->ci.IsHyperthreadingEnabled();
	physicalMemorySize = g->ci.GetPhysicalMemorySize();
	virtualMemorySize = g->ci.GetVirtualMemorySize();
	osVersion = g->ci.GetOSVersionString();
	osBits = (byte)g->ci.GetOSBits();	
}

void CNRManager::GetDriverLink(std::string& link, std::string& videoURL, unsigned char& linkType)
{
	std::wstring iniFN = GetApplicationRootDirectory();
	iniFN += L"\\player.ini";
	CIniFile ini(iniFN);
	std::string lang = ini.GetString("settings", "language");
	g->gi.GetDriverLink(link, videoURL, lang, linkType);
}

void CNRManager::GetGPUInfo( std::string &vendorName, std::string &deviceName, unsigned __int64 &videoMemorySize, int &major, int &minor)
{
	vendorName = g->gi.GetVendorName();
	//deviceName = g->gi.GetRendererGLName();	
	deviceName = g->gi.GetDeviceName();
	videoMemorySize = g->gi.GetVideoMemory()->GetTotalSize();
	int lead;	
	g->gi.GetDriverVersion(lead, major, minor);
}

void CNRManager::GetLoadingResourcesCoords( resourceLoadStruct* apResLoadStruct, int aResCount, float* pAvatarX, float* pAvatarY, float* pAvatarZ)
{
	apResLoadStruct->coordsSize = 0;

	for( int i=0; i<aResCount; i++, apResLoadStruct++)
	{
		IAsynchResource* asynchRes = apResLoadStruct->pIAsynchResource;
		if( !asynchRes)
			continue;

		int objectsUsingResourceCount = asynchRes->GetObjectCount();

		for (int i = 0; i < objectsUsingResourceCount; i++)
		{
			C3DObject* obj;
			float koef = 1.0f;
			asynchRes->GetObject(i, (void**)(&obj), &koef);

			if( !obj)
				continue;

			obj->GetCoords( &apResLoadStruct->x[apResLoadStruct->coordsSize]
							, &apResLoadStruct->y[apResLoadStruct->coordsSize]
							, &apResLoadStruct->z[apResLoadStruct->coordsSize]);
			apResLoadStruct->coordsSize++;
		}
	}

	if( m_context->mpMapMan != NULL)
	{
		float ax, ay, az;
		m_context->mpMapMan->GetAvatarPosition(ax, ay, az);
		if( pAvatarX) *pAvatarX = ax;
		if( pAvatarY) *pAvatarY = ay;
		if( pAvatarZ) *pAvatarZ = az;
	}
}

bool CNRManager::IsKnownProxyServer(std::string& userName, std::string& password, std::string& host, std::string& port)
{
	CKnownProxyServersList list;
	return list.IsKnownProxyServer(userName, password, host, port);
}

rm::IVideoCaptureSupport* CNRManager::GetVideoCaptureSupport()
{
	return &m_videoCaptureSupport;
}