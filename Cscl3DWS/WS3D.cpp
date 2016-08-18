// WS3D.cpp : Implementation of CWS3D

#include "stdafx.h"
#include <signal.h>
#include "Cscl3DWS.h"
#include "WS3D.h"
#include "res\res.h"
#include "sm\\sm.h"
#include "common.h"
#include "../mdump/mdump.h"
#include "FileSys.h"
#include "Protocol.h"
#include "TraceRefsDlg.h"
#include "IMouseController.h"
#include "WorldEditor/IWorldEditor.h"
#include <fstream>
#include "InfoMan\InfoMan.h"
#include "config/oms_config.h"
#include "syncMan.h"
#include "ServiceMan.h"
#include "../Logger/Logger.h"
#include "../LogWriter/LogWriter.h"
#include "winuser.h"
#include "DumpParamsSaver.h"
#include <Psapi.h>
#include "ExecCommandThread.h"
#include "GlobalKeyboardHook.h"
#include "mmsystem.h"
#include "Commdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CWS3D
#define LOG_EVENT_ADD_TEXT	1 | 0x8000
#define DSSCL_NORMAL        0x00000001
#define MIN_USER_INACTIVE_TIME	  900000

#include "shlobj.h"
std::wstring GetApplicationDataDirectory()
{
	wchar_t tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

DWORD WINAPI MyThreadProc(LPVOID lpParam)
{
	abort();
	return 0;
}

DWORD WINAPI MyThreadProc3(LPVOID lpParam)
{
	raise(SIGILL);
	return 0;
}


DWORD WINAPI MyThreadProc2(LPVOID lpParam)
{
	CExecCommandThread* thr = (CExecCommandThread*)lpParam;
	thr->Start(L"1.exe", NULL);
	return 0;
}

CWS3D::CWS3D():
	MP_VECTOR_INIT( m_execCommandThreadVec),
	MP_WSTRING_INIT( mwsUIBase),
	MP_WSTRING_INIT( mwsUISkinBase),
	MP_WSTRING_INIT( msSysCommand),
	MP_WSTRING_INIT( msRegKey),
	MP_WSTRING_INIT( msPathIni),
	MP_VECTOR_INIT(m_displayParams)
{
#ifdef _DEBUG
//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//_CrtSetBreakAlloc(64);
#endif
	m_hResourceInstance = NULL;
	//m_bWindowOnly = TRUE;
	//mpIResourceManager = NULL;
	mpICommunicationManager = NULL;
	mpIResourceManager = NULL;
    mhEventGetSetState = CreateEvent( 
        NULL,     // no security attributes
        FALSE,    // auto-reset event
        TRUE,     // initial state is signaled
        NULL);    // object not named
	mpSMState = NULL;
	sMyUserName = "";
	mSMLastMode = rmml::smmInitilization;
	initCounter = 0;
	bFullScreen = FALSE;
	m_hParentWnd = 0;
	pCallbacks = NULL;
	mbMouseCaptured = false;
	nLogInitialized = liLogNotInitialized;
	InitializeCriticalSection( &csQuery);
	mbLangAlreadyGot = FALSE;
	mbVersionAlreadyGot = FALSE;
//	CoInitialize(NULL);
	hVoipModule = 0;
	hDesktopModule = 0;
	hSoundModule = 0;
	hComModule = 0;
	hLoggerModule = 0;
	needExit = false;
	mbAppActive = true;
	m_pLogWriter = NULL;
	m_pLogWriterLastMsgAsync = NULL;

	m_isAllowedDebug = false;

	m_isAlt = false;
	m_isCtrl = false;
	m_isShift = false;

	m_isSleepingUserState = false;
	m_lastUserActivityTime = GetTimeStamp();
	m_windowMovingTime = 0;

	muWindowClientWidth = 0;
	muWindowClientHeight = 0;

	sessionID = 0;

	setlocale(LC_ALL, "Russian");
	m_pDumpParamsSaver = MP_NEW( CDumpParamsSaver);
	m_isDisplayParamsInitialized = false;
	m_pkeybHook = NULL;
}

CWS3D::~CWS3D()
{
	CExecCommandThreadVecIt it = m_execCommandThreadVec.begin();
	CExecCommandThreadVecIt itEnd = m_execCommandThreadVec.end();
	for ( ; it != itEnd; it++)
	{
		// проходим по всем объектам команд
		CExecCommandThread *m_pExecCommandThread = *it;
		if ( m_pExecCommandThread != NULL)
		{					
			MP_DELETE( m_pExecCommandThread);			
		}
	}
	m_execCommandThreadVec.clear();

	DeleteCriticalSection( &csQuery);
	CloseHandle(mhEventGetSetState);
	if(mpSMState!=NULL) GlobalFree(mpSMState);
	std::vector<wchar_t*>::iterator vi = mvArguments.begin();
	for(; vi != mvArguments.end(); vi++){
		wchar_t* pwc = *vi;
		if(pwc != NULL)
			MP_DELETE_ARR( pwc);
	}
	MP_DELETE( m_pDumpParamsSaver);
	m_pDumpParamsSaver = NULL;
	mpIResourceManager = NULL;
	mpICommunicationManager = NULL;
}

unsigned long CWS3D::SetLogAndTracerMode(BOOL bUseLog, int gSendLog, BOOL bUseTracer, BOOL bUseSyncLog)
{
	pOMSContext->mpTracer->setTraceMode(bUseTracer!=0);
	pOMSContext->mpRM->SetLogMode(bUseLog!=0);

	/*if(pOMSContext->mpLogger!=NULL)
		pOMSContext->mpLogger->SetMode( gSendLog>0?true:false);*/

	if(pOMSContext->mpSyncMan!=NULL)
		pOMSContext->mpSyncMan->SetLogSyncMessage( bUseSyncLog!=0);

	return 0;
}

void CWS3D::SetApplicationRoot(LPCWSTR alpcRoot)
{
	if( alpcRoot)
		sApplicationRoot = alpcRoot;
}

LPCWSTR CWS3D::GetApplicationRoot()
{
	return sApplicationRoot;
}

void CWS3D::DestroyContext()
{
	if(pOMSContext.get()!=NULL)
	{
		if( m_pkeybHook != NULL)
		{
			delete  m_pkeybHook;
			m_pkeybHook = NULL;
		}

		Stop();		

		service::DestroyServiceWorldManager( pOMSContext.get());

		service::DestroyServiceManager( pOMSContext.get());

		sync::DestroySyncManager( pOMSContext.get());

		DestroyVoipManagerImpl(pOMSContext.get());
		//DestroyDesktopManagerImpl(pOMSContext.get());
		DestroySceneManager(pOMSContext.get());
		DestroySoundManagerImpl(pOMSContext.get());

		// лог убъется в DestroyRenderManager(pOMSContext.get());
		if(mpIResourceManager!=NULL)
			mpIResourceManager->SetILogWriter(NULL);
		pOMSContext->mpLogger = NULL;
		pOMSContext->mpLogWriter = NULL;

		rm::DestroyRenderManager(pOMSContext.get());

		if(mpIResourceManager!=NULL)
			mpIResourceManager->DestroyResourceManager();

		DestroyDesktopManagerImpl(pOMSContext.get());

		pOMSContext->mpResM = NULL;
		pOMSContext->mpIResM = NULL;
		mpIResourceManager = NULL;
		DestroyComManagerImpl(pOMSContext.get());
		mpICommunicationManager = NULL;
		
		if (pOMSContext.get()->mpInfoMan != NULL)
		{
			info::DestroyInfoManager(pOMSContext.get());
			pOMSContext.get()->mpInfoMan = NULL;
		}
		DestroyLogManagersImpl( pOMSContext.get());
	}
	ZeroMemory( pOMSContext.get(), sizeof( oms::omsContext));
}

using oms::omsUserEvent;

unsigned long CWS3D::PrepareForDestroy()
{
	pOMSContext->mpTracer->StopSendingToServer();
	pOMSContext->mpLogWriter->StopSendingToServer();
	DestroyLoggerImpl(pOMSContext.get());
	
	if ( pOMSContext->mpIPadMan)
		pOMSContext->mpIPadMan->Logout();
	
	if ( pOMSContext->mpVoipMan)
		pOMSContext->mpVoipMan->CloseVoipManServer();

	if(pOMSContext->mpRM!=NULL)
		pOMSContext->mpRM->PrepareForDestroy();

	if ( pOMSContext->mpSndM)
		pOMSContext->mpSndM->UpdateAllSound( SOUNDS_STOP);	
	//DestroyContext();
	return S_OK;
}

bool CWS3D::OnClose(){
	if(pOMSContext.get() && pOMSContext->mpSM)
	{
		return pOMSContext->mpSM->OnClose();
	}
	return true;
}

unsigned long CWS3D::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
//ATLTRACE("%x, %x\n", wParam, lParam);
	/*if(wParam=='H' && (GetKeyState(VK_CONTROL) & 0x8000)){
		// перегрузить интерфейс и удалить модуль и сцену (Home)
		loadUI(NULL);
		return 0;
	}*/

	//  комбинация кнопок для отладки, 
	//	чтобы пользователь случайно не наткнулся на наши отладочные возможности:
	//  Alt-Ctrl-Shift-D

	m_lastUserActivityTime = GetTimeStamp();

	if( (wParam=='V' || wParam=='v' || wParam=='М' || wParam=='м' ) && 
		(GetKeyState(VK_MENU) & 0x8000))
	{
		pOMSContext->mpRM->ToogleRenderState(3321);	
	}

	if( (wParam=='V' || wParam=='v' || wParam=='М' || wParam=='м' ) && 
		(GetKeyState(VK_SHIFT) & 0x8000))
	{
		pOMSContext->mpRM->ToogleRenderState(3322);	
	}

	if( (wParam=='D' || wParam=='d' || wParam=='В' || wParam=='в' ) && 
		(GetKeyState(VK_MENU) & 0x8000) && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000) ){
		m_isAllowedDebug = true;

		if( m_pkeybHook == NULL)
		{
			m_pkeybHook = new CGlobalKeyboardHook( m_hResourceInstance, pOMSContext.get());
			m_pkeybHook->Start();
		}
		return 0;
	}

	// begin cloth_editor
	if ((wParam == 66) && (lParam == 76))
	{
		pOMSContext->mpRM->ToogleRenderState(100);		
	}
	else if ((wParam == 66) && (lParam == 77))
	{
		pOMSContext->mpRM->ToogleRenderState(101);		
	}
	else if ((wParam == 66) && (lParam == 78))
	{
		pOMSContext->mpRM->ToogleRenderState(102);		
	}
	else if ((wParam == 66) && (lParam == 79))
	{
		pOMSContext->mpRM->ToogleRenderState(103);		
	}
	else if ((wParam == 66) && (lParam == 308))
	{
		pOMSContext->mpRM->ToogleRenderState(51);		
	}
	else if ((wParam == 66) && (lParam == 306))
	{
		pOMSContext->mpRM->ToogleRenderState(52);		
	}
	else if ((wParam == 66) && (lParam == 307))
	{
		pOMSContext->mpRM->ToogleRenderState(53);		
	}
	// end cloth_editor

	m_isAlt = (GetKeyState(VK_MENU) & 0x8000)!=0;
	m_isCtrl = (GetKeyState(VK_CONTROL) & 0x8000)!=0;
	m_isShift = (GetKeyState(VK_SHIFT) & 0x8000)!=0;

	////begin  whiteboard и управление шарингом
	//if (pOMSContext->mpRM->GetMouseController())		
	//	{
	//		unsigned short muKeyCode = wParam & 0x0000FFFF;
	//		unsigned short muScanCode = wParam & 0x0000FFFF;

	//		unsigned short muKeyStates;
	//		unsigned short muUnicode;
	//		unsigned short muAscii;

	//		TranslateKeyCode(muKeyCode, muKeyStates, muScanCode, muAscii, muUnicode);
	//		pOMSContext->mpRM->GetMouseController()->OnKeyDown(muAscii, wParam, m_isAlt, m_isCtrl, m_isShift);
	//	}
	////end  whiteboard и управление шарингом


	if (m_isAllowedDebug)
	{
		if(wParam=='1' && (GetKeyState(VK_MENU) & 0x8000))
		{
			CreateThread(NULL, 65000, MyThreadProc, NULL, 0, NULL);			
		}
		if(wParam=='2' && (GetKeyState(VK_MENU) & 0x8000))
		{
			CreateThread(NULL, 65000, MyThreadProc2, NULL, 0, NULL);
		}
		if(wParam=='3' && (GetKeyState(VK_MENU) & 0x8000))
		{
			CreateThread(NULL, 65000, MyThreadProc3, NULL, 0, NULL);			
		}
		if(wParam=='4' && (GetKeyState(VK_MENU) & 0x8000))
		{
			raise(SIGSEGV);
		}
		if(wParam=='5' && (GetKeyState(VK_MENU) & 0x8000))
		{
			raise(SIGTERM);
		}
		if(wParam=='6' && (GetKeyState(VK_MENU) & 0x8000))
		{
			RaiseException(123, EXCEPTION_NONCONTINUABLE, 0, NULL);
		}
		if(wParam=='7' && (GetKeyState(VK_MENU) & 0x8000))
		{
			throw 13;
		}
		if(wParam=='8' && (GetKeyState(VK_MENU) & 0x8000))
		{
			DebugBreak();
		}
		if(wParam=='9' && (GetKeyState(VK_MENU) & 0x8000))
		{
			exit();
		}
		if(wParam=='0' && (GetKeyState(VK_MENU) & 0x8000))
		{
			throw std::runtime_error("123");			
		}
		
	
		/*if(wParam=='R' && (GetKeyState(VK_CONTROL) & 0x8000)){
			// перегрузить сцену
			pOMSContext->mpSM->LoadScene(-1);
			pOMSContext->mpSM->LoadScene((wchar_t*)1);
			return 0;
		}

		if(wParam=='Y' && (GetKeyState(VK_SHIFT) & 0x8000)){
			pOMSContext->mpSM = NULL;
			pOMSContext->mpSM->LoadScene(-1);
			return 0;
		}

		if(wParam=='Y' && (GetKeyState(VK_CONTROL) & 0x8000)){
			Sleep(1000 * 1000); 
			return 0;
		}*/

		if(wParam=='F' && (GetKeyState(VK_SHIFT) & 0x8000)){
			// сохранение всех текстур шрифта	
			pOMSContext->mpRM->ToogleRenderState(4096);
			return 0;
		}

		if(wParam=='M' && (GetKeyState(VK_CONTROL) & 0x8000)){
			//  в лог выводится краткая информация о занятой памяти: только по файлам
			pOMSContext->mpRM->ToogleRenderState(2000);
			return 0;
		}

		if(wParam=='M' && (GetKeyState(VK_MENU) & 0x8000)){
			//  в лог выводится полная информация о занятой памяти: по файлам и номерам строчек.
			pOMSContext->mpRM->ToogleRenderState(2001);
			return 0;
		}

		if(wParam=='S' && (GetKeyState(VK_SHIFT) & 0x8000)){
			//любой вывод информации о занятой памяти выводит как размер занятой памяти (файлом исходником или парой строчка/файл), так и разницу в размере занимаемой памяти между текущим и сохраненным моментом.
			pOMSContext->mpRM->ToogleRenderState(2002);
			return 0;
		}

		if(wParam=='F' && (GetKeyState(VK_MENU) & 0x8000)){
			// вывод информации о занятой памяти по функциям
			pOMSContext->mpRM->ToogleRenderState(2003);
			return 0;
		}

		if(wParam=='E' && (GetKeyState(VK_MENU) & 0x8000)){
			// вывод информации об объектах сравнительно с сохраненной точкой
			pOMSContext->mpRM->ToogleRenderState(2004);
			return 0;
		}

		if(wParam=='1' && (GetKeyState(VK_CONTROL) & 0x8000)){
			// вывести BoundBoxes
			//pOMSContext->mpRM->ToggleDebugRenderItem(DRI_AABB);
			return 0;
		}
		// выводит инфу о ссылках на заданные объекты в trace
		// abLost - выводить инфу только по потерянным RMML-элементам (parent==null)
		// aiType - 0: все, 1: 3D-объекты, 2: 3D-персонажи, 3: 3D-группы, 4: композиции
		// apwcObjName - имя объекта
		//bool mlSceneManager::TraceObjectsRefs(bool abLost, int aiType, const wchar_t* apwcObjName);
		if(wParam=='L' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_MENU) & 0x8000)){
			// найти всех аватаров с parent=null и вывести в trace все ссылки на них
			pOMSContext->mpSM->TraceObjectsRefs(true, 2);
			return 0;
		}
		if(wParam=='A' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_MENU) & 0x8000)){
			// найти всех аватаров и вывести в trace все ссылки на них
			//pOMSContext->mpSM->TraceObjectsRefs(false, 2);
			return 0;
		}
		if(wParam=='N' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_MENU) & 0x8000)){
			// получить через окно имя RMML-объекта
			CTraceRefsDialog dlg;
			int iRes = dlg.DoModal();
			if(iRes == IDOK){
				CComBSTR bstrName = dlg.mbstrName;
				bool bLost = dlg.mbLost?true:false;
				int iType = (int)dlg.mType;
				// найти все соответствующие объекты и вывести в trace все ссылки на них
				pOMSContext->mpSM->TraceObjectsRefs(bLost, iType, (BSTR)bstrName);
			}
			return 0;
		}
		if(wParam=='O' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_MENU) & 0x8000)){
			// вывести в tracer путь к объекту, который под мышкой, а также его свойства
			pOMSContext->mpSM->TraceObjectUnderMouse();
		}
		if(wParam=='G' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_MENU) & 0x8000)){
			// вызвать процедуру добавления объекта под мышкой в галерею объектов
			pOMSContext->mpSM->CopyObjectUnderMouseToGallery();
		}
		if(wParam=='T' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_MENU) & 0x8000)){
			// вывести в файл все дерево RMML-объектов
			//		std::filebuf* buf = new std::filebuf();
			//		buf->open("c:\\rmmltree.txt", std::ios::in | std::ios::out | std::ios::trunc);
			std::ofstream* fTree = MP_NEW( std::ofstream);
			fTree->rdbuf()->open("c:\\rmmltree.txt", std::ios::in | std::ios::out | std::ios::trunc);
			//fwrite("", fLog);
			//(*fTree) << "ddddddd";
			pOMSContext->mpSM->OutRMMLObjectTree(fTree, 0);
			fTree->rdbuf()->close();
		}
		/*	long lKeyStates = 0;
		if(GetKeyState(VK_SHIFT) & 0x8000)	lKeyStates |= 0x1;
		if(GetKeyState(VK_LSHIFT) & 0x8000)	lKeyStates |= 0x2;
		if(GetKeyState(VK_CONTROL) & 0x8000)lKeyStates |= 0x4;
		if(GetKeyState(VK_LCONTROL) & 0x8000)lKeyStates |= 0x8;
		if(GetKeyState(VK_MENU) & 0x8000)	lKeyStates |= 0x10;
		if(GetKeyState(VK_LMENU) & 0x8000)	lKeyStates |= 0x20;*/

		if (((wParam == 'Z') || (wParam == 'z')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			if(!pOMSContext->mpRM->GetWorldEditor()->GetConnectionState())
			{
				pOMSContext->mpSM->ShowZoneEvent( pOMSContext->mpRM->ToogleRenderState(0));
			}		
		}

		if (((wParam == 'J') || (wParam == 'j') || (wParam == 'О') || (wParam == 'о')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(411);		
		}

		if (((wParam == 'U') || (wParam == 'u') || (wParam == 'Г') || (wParam == 'г')) && (GetKeyState(VK_SHIFT) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(2005);		
		}

		if (((wParam == 'A') || (wParam == 'a') || (wParam == 'Ф') || (wParam == 'ф')) && (GetKeyState(VK_SHIFT) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(477);		
		}

		if (((wParam == 'E') || (wParam == 'e') || (wParam == 'У') || (wParam == 'у')) && (GetKeyState(VK_SHIFT) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(488);		
		}

		if (((wParam == 'F') || (wParam == 'f') || (wParam == 'А') || (wParam == 'а')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(800);		
		}

		if (((wParam == 'K') || (wParam == 'k') || (wParam == 'Л') || (wParam == 'л')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(999);		
		}

		if (((wParam == 'K') || (wParam == 'k') || (wParam == 'Л') || (wParam == 'л')) && (GetKeyState(VK_SHIFT) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(998);		
		}

		if ((wParam == 66) && (lParam == 76))
		{
			pOMSContext->mpRM->ToogleRenderState(100);		
		}

		if (((wParam == 'U') || (wParam == 'u')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			if(!pOMSContext->mpRM->GetWorldEditor()->GetConnectionState())
			{
				pOMSContext->mpRM->ToogleRenderState(54);
			}		
		}

		if (((wParam == 'H') || (wParam == 'h')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(1100);			
		}
		if (((wParam == 'O') || (wParam == 'o') || (wParam == 'Щ') || (wParam == 'щ')) && 
			(GetKeyState(VK_SHIFT) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(777);				
		}

		if (((wParam == 'I') || (wParam == 'i')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			if(pOMSContext->mpInfoMan != NULL)
			{
				pOMSContext->mpInfoMan->CreateWnd( (unsigned long)m_hParentWnd);
				pOMSContext->mpInfoMan->ShowWindow( true);
			}
		}

		if (((wParam == 'Q') || (wParam == 'q')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			ATLASSERT( pOMSContext->mpMapMan);
			if(pOMSContext->mpMapMan != NULL)
			{
				pOMSContext->mpMapMan->DumpLoadingQueue();
			}
		}

		if (((wParam == 'W') || (wParam == 'w')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(48);		
		}

		if ((wParam == 'S') && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(55);
		}

		if ((wParam == 'L') && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(49);
		}

		if ((wParam == 'W') && (GetKeyState(VK_MENU) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(5000);
		}

		if (((wParam == 'K') || (wParam == 'k') || (wParam == 'Л') || (wParam == 'л')) && (GetKeyState(VK_MENU) & 0x8000))
		{
			pOMSContext->mpLogWriter->WriteLnLPCSTR("ConnectToKinect button");
			pOMSContext->mpRM->ToogleRenderState(6000);
		}

		if (((wParam == 'O') || (wParam == 'o') ||
			(wParam == 'Щ') || (wParam == 'щ')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(56);
		}

		if (((wParam == 'P') || (wParam == 'p')
			|| (wParam == 'З') || (wParam == 'з')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(57);
		}

		if (((wParam == 'N') || (wParam == 'n')
			|| (wParam == 'Т') || (wParam == 'т')) && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(58);
		}

		if ((wParam == 'D') && (GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(50);
		}
		if(wParam=='D' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_MENU) & 0x8000)){
			// вывести в tracer и лог глубины 2D-объекта под мышкой и его родителей
			pOMSContext->mpSM->TraceUnderMouseObjectDepth();
		}

		for (int k = 0; k < 10; k++)
			if ((wParam == '0' + k) && (GetKeyState(VK_CONTROL) & 0x8000))
			{
				pOMSContext->mpRM->ToogleRenderState(1 + k);
			}

		if (((wParam=='B') || (wParam=='b') ||
			(wParam=='И') || (wParam=='и'))	&& (GetKeyState(VK_CONTROL) & 0x8000))
		{
			pOMSContext->mpRM->ToogleRenderState(300);
			return 0;
		}

		if ((wParam >= VK_F1) && (wParam <= VK_F4))
		{
			pOMSContext->mpRM->ToogleRenderState(1001 + wParam - VK_F1);
		}

		if (wParam == VK_F5)
		{
			pOMSContext->mpRM->ToogleRenderState(1091);
		}	

		if (wParam == VK_F6)
		{
			pOMSContext->mpRM->ToogleRenderState(-1);
		}	

		if (wParam == VK_F7)
		{
			pOMSContext->mpRM->ToogleRenderState(-2);
		}	
	}
	

	// begin world editor
	if (pOMSContext->mpRM->GetWorldEditor()->GetConnectionState())
	{
		pOMSContext->mpRM->GetWorldEditor()->OnChar(wParam);
	}
	// end world editor

	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,lParam));
	return 0;
}

unsigned long CWS3D::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
/*	long lKeyStates = 0;
	if(GetKeyState(VK_SHIFT) & 0x8000)	lKeyStates |= 0x1;
	if(GetKeyState(VK_LSHIFT) & 0x8000)	lKeyStates |= 0x2;
	if(GetKeyState(VK_CONTROL) & 0x8000)lKeyStates |= 0x4;
	if(GetKeyState(VK_LCONTROL) & 0x8000)lKeyStates |= 0x8;
	if(GetKeyState(VK_MENU) & 0x8000)	lKeyStates |= 0x10;
	if(GetKeyState(VK_LMENU) & 0x8000)	lKeyStates |= 0x20;*/
	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));

	m_isAlt = (GetKeyState(VK_MENU) & 0x8000)!=0;
	m_isCtrl = (GetKeyState(VK_CONTROL) & 0x8000)!=0;
	m_isShift = (GetKeyState(VK_SHIFT) & 0x8000)!=0;

	if (pOMSContext->mpRM->GetMouseController())
	{
		pOMSContext->mpRM->GetMouseController()->OnKeyUp(wParam, m_isAlt, m_isCtrl, m_isShift);
	}

	return 0;
}

unsigned long CWS3D::OnLMouseDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();
	if (pOMSContext->mpRM->GetMouseController())
	{
		pOMSContext->mpRM->GetMouseController()->OnLMouseDblClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	if (pOMSContext->mpRM->GetWorldEditor()->GetConnectionState())
	{
		pOMSContext->mpRM->GetWorldEditor()->SetLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));
	return 0;
}

unsigned long CWS3D::OnRMouseDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();
	if (pOMSContext->mpRM->GetMouseController())
	{
		pOMSContext->mpRM->GetMouseController()->OnRMouseDblClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));

	return 0;
}

unsigned long CWS3D::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();

	OnActivateApp(true);
	::GetClientRect(m_FrameWorkWnd, &mrcClient);
	mbMouseCaptured = true;
	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));

	//////////////////////////////////////////////////////////////////////////
	// World Editor
	IWorldEditor* we = pOMSContext->mpRM->GetWorldEditor();
	//if (we->IsConnect())
	if (we->GetConnectionState())
	{
		//we->SelectObject();
		we->SelectClickedObject();
	}
	//////////////////////////////////////////////////////////////////////////


	if (pOMSContext->mpRM->GetMouseController())
	{
		pOMSContext->mpRM->GetMouseController()->OnLMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	if (pOMSContext->mpRM->GetWorldEditor()->GetConnectionState())
	{
		pOMSContext->mpRM->GetWorldEditor()->SetLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

 	return 0;
}

unsigned long CWS3D::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();

	if (pOMSContext->mpRM->GetMouseController())
	{
		pOMSContext->mpRM->GetMouseController()->OnLMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	if(pOMSContext->mpRM->GetWorldEditor()->IsDragObjectByMouse())
	{
		pOMSContext->mpRM->GetWorldEditor()->StopDragObjectByMouse();
	}

	if (pOMSContext->mpRM->GetWorldEditor()->GetConnectionState())
	{
		pOMSContext->mpRM->GetWorldEditor()->SetLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	mbMouseCaptured = false;
	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));

	return 0;
}

unsigned long CWS3D::OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();

	OnActivateApp(true);
	if (pOMSContext->mpRM->GetWorldEditor()->GetConnectionState() && pOMSContext->mpRM->GetWorldEditor()->IsFreeCam())
	{
		pOMSContext->mpRM->GetWorldEditor()->SetMButtonPress(true, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	return 0;
}

unsigned long CWS3D::OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();

	if (pOMSContext->mpRM->GetWorldEditor()->IsMButtonPressed())
	{
		pOMSContext->mpRM->GetWorldEditor()->SetMButtonPress(false);
	}

	return 0;
}

unsigned long CWS3D::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();
	/*if(mbMouseCaptured){
		int xPos = ((int)(short)LOWORD(lParam));	
		int yPos = ((int)(short)HIWORD(lParam));
		if(xPos < 0) xPos = 0;
		if(yPos < 0) yPos = 0;
		if(xPos >= mrcClient.right) xPos = mrcClient.right-1;
		if(yPos >= mrcClient.bottom) yPos = mrcClient.bottom-1;
		lParam = yPos << 16 | xPos;
	}*/

	if (pOMSContext->mpRM->GetMouseController())
	{
		pOMSContext->mpRM->GetMouseController()->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	if(pOMSContext->mpRM->GetWorldEditor()->IsMButtonPressed() || pOMSContext->mpRM->GetWorldEditor()->IsDragObjectByMouse() || pOMSContext->mpRM->GetWorldEditor()->IsLButtonPressed())
	{
		pOMSContext->mpRM->GetWorldEditor()->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	
	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));
	return 0;
}

unsigned long CWS3D::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();
	OnActivateApp(true);
	if (pOMSContext->mpRM->GetMouseController())
	{
		pOMSContext->mpRM->GetMouseController()->OnRMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));
	return 0;
}

unsigned long CWS3D::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();

	if (pOMSContext->mpRM->GetMouseController())
	{
		pOMSContext->mpRM->GetMouseController()->OnRMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));
	return 0;
}

unsigned long CWS3D::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	m_lastUserActivityTime = GetTimeStamp();

	if (pOMSContext->mpRM->GetMouseController())
	{
		int delta = (short)HIWORD(wParam);
		pOMSContext->mpRM->GetMouseController()->OnMouseWheel(delta);
	}

	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));
	return 0;
}

// управление видом курсора
// 0 - нет курсора (скрыть)
// 1 - нормальный курсор
// 2 - курсор активной зоны
// 3 - курсор ожидания
void CWS3D::SetCursor(int aiType){
//	ATLTRACE( "----------- SetCursor  type = %d\n", aiType);
if(m_pLogWriter){
	static int giPrevCursor;
	if(aiType != giPrevCursor){
		m_pLogWriter->WriteLn("[CWS3D] SetCursor(",aiType,")");
		giPrevCursor = aiType;
	}
}
	if(aiType==0){
		ShowCursor(FALSE);
		return;
	}
	bool bBigCursor = (aiType & CT_BIG_CURSOR_MASK)!=0;
	//aiType &= ~CT_BIG_CURSOR_MASK;
	
	LPSTR resCursor = IDC_ARROW;
	if(bBigCursor)	resCursor = MAKEINTRESOURCE(IDC_BIG_ARROW);
	else			resCursor = IDC_ARROW;
	switch(aiType){
	case oms::ActiveCursor:
		if(bBigCursor)	resCursor = MAKEINTRESOURCE(IDC_BIG_HAND);
		else			resCursor = MAKEINTRESOURCE(32649);
		break;
	case oms::WaitCursor:
		if(bBigCursor)	resCursor = MAKEINTRESOURCE(IDC_BIG_WAIT);
		else			resCursor = IDC_WAIT;
		break;
	case oms::BeamCursor:
		if(bBigCursor)	resCursor = MAKEINTRESOURCE(IDC_BIG_BEAM);
		else			resCursor = MAKEINTRESOURCE(IDC_BEAM);
		break;
	case oms::DragCursor:
		resCursor = MAKEINTRESOURCE(IDC_DRAG);
		break;
	case oms::Size1Cursor:
		resCursor = MAKEINTRESOURCE(IDC_SIZE1);
		break;
	case oms::Size2Cursor:
		resCursor = MAKEINTRESOURCE(IDC_SIZE2);
		break;
	case oms::Size3Cursor:
		resCursor = MAKEINTRESOURCE(IDC_SIZE3);
		break;
	case oms::Size4Cursor:
		resCursor = MAKEINTRESOURCE(IDC_SIZE4);
		break;
	case oms::CreateCursor:
		resCursor = MAKEINTRESOURCE(IDC_POINTER_CREATE);
		break;
	case oms::CopyCursor:
		resCursor = MAKEINTRESOURCE(IDC_POINTER_COPY);
		break;
	case oms::DeleteCursor:
		resCursor = MAKEINTRESOURCE(IDC_POINTER_DELETE);
		break;
	case oms::Move3DPlaneCursor:
		resCursor = MAKEINTRESOURCE(IDC_MOVE_3D_PLANE);
		break;
	case oms::Move3DPlaneImpossibilityCursor:
		resCursor = MAKEINTRESOURCE(IDC_MOVE_3D_PLANE_IMPOSSIBILITY);
		break;
	case oms::Move3DCursor:
		resCursor = MAKEINTRESOURCE(IDC_MOVE_3D);
		break;
	case oms::Rotate3DPlaneCursor:
		resCursor = MAKEINTRESOURCE(IDC_ROTATE_3D_PLANE);
		break;
	case oms::Rotate3DCursor:
		resCursor = MAKEINTRESOURCE(IDC_ROTATE_3D);
		break;
	case oms::Rotate3DImpossibilityCursor:
		resCursor = MAKEINTRESOURCE(IDC_ROTATE_3D_IMPOSSIBILITY);
		break;
	case oms::Select3DCursor:
		resCursor = MAKEINTRESOURCE(IDC_SELECT_3D);
		break;
	case oms::Place3DCursor:
		resCursor = MAKEINTRESOURCE(IDC_PLACE_3D);
		break;
	case oms::Place3DImpossibilityCursor:
		resCursor = MAKEINTRESOURCE(IDC_PLACE_3D_IMPOSSIBILITY);
		break;
	case oms::Copy3DCursor:
		resCursor = MAKEINTRESOURCE(IDC_COPY_3D);
		break;
	case oms::Scale3DCursor:
		resCursor = MAKEINTRESOURCE(IDC_SCALE_3D);
		break;
	case oms::Scale3DImpossibilityCursor:
		resCursor = MAKEINTRESOURCE(IDC_SCALE_3D_IMPOSSIBILITY);
		break;
	case oms::SitplaceUpDown3DCursor:
		resCursor = MAKEINTRESOURCE(IDC_UPDOWN);
		break;
	case oms::PenCursor:
		resCursor = MAKEINTRESOURCE(IDC_PEN);
		break;
	case oms::EraseCursorSize2:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_2);
		break;
	case oms::EraseCursorSize4:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_4);
		break;
	case oms::EraseCursorSize6:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_6);
		break;
	case oms::EraseCursorSize8:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_8);
		break;
	case oms::EraseCursorSize10:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_10);
		break;
	case oms::EraseCursorSize12:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_12);
		break;
	case oms::EraseCursorSize16:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_16);
		break;
	case oms::EraseCursorSize20:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_20);
		break;
	case oms::EraseCursorSize26:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_26);
		break;
	case oms::EraseCursorSize32:
		resCursor = MAKEINTRESOURCE(IDC_ERASE_32);
		break;
	case oms::TextStickerCursor1:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER1);
		break;
	case oms::TextStickerCursor2:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER2);
		break;
	case oms::TextStickerCursor3:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER3);
		break;
	case oms::TextStickerCursor4:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER4);
		break;
	case oms::TextStickerCursor5:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER5);
		break;
	case oms::TextStickerCursor6:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER6);
		break;
	case oms::TextStickerCursor7:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER7);
		break;
	case oms::TextStickerCursor8:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER8);
		break;
	case oms::TextStickerCursor9:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER9);
		break;
	case oms::TextStickerCursor10:
		resCursor = MAKEINTRESOURCE(IDC_TEXTSTICKER10);
		break;
	case oms::ImageStickerCursor1:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER1);	
		break;
	case oms::ImageStickerCursor2:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER2);	
		break;
	case oms::ImageStickerCursor3:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER3);	
		break;
	case oms::ImageStickerCursor4:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER4);	
		break;
	case oms::ImageStickerCursor5:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER5);	
		break;
	case oms::ImageStickerCursor6:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER6);	
		break;
	case oms::ImageStickerCursor7:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER7);	
		break;
	case oms::ImageStickerCursor8:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER8);	
		break;
	case oms::ImageStickerCursor9:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER9);	
		break;
	case oms::ImageStickerCursor10:
		resCursor = MAKEINTRESOURCE(IDC_IMAGESTICKER10);	
		break;
	case oms::HealCursor:
		resCursor = MAKEINTRESOURCE(IDC_HEALCURSOR);	
		break;	
	case oms::FireOffCursor:
		resCursor = MAKEINTRESOURCE(IDC_FIREOFFCURSOR);	
		break;
	case oms::RemoveBoxCursor:
		resCursor = MAKEINTRESOURCE(IDC_REMOVEBOXCURSOR);	
		break;	
	case oms::StretcherCursor:
		resCursor = MAKEINTRESOURCE(IDC_STRETCHERCURSOR);	
		break;	
	case oms::DragPeople:
		resCursor = MAKEINTRESOURCE(IDC_PEOPLEDRAGCURSOR);	
		break;
	case oms::TriageCursor:
		resCursor = MAKEINTRESOURCE(IDC_TRIAGECURSOR);
		break;
	}
//resCursor = MAKEINTRESOURCE(IDC_TESTTRANSP);
	//mhCursor = LoadCursor(_Module.GetResourceInstance(),resCursor);
	mhCursor = LoadCursor(NULL,resCursor); // пытаемся загрузить стандартный курсор
	if( mhCursor == NULL)
		mhCursor = LoadCursor(m_hResourceInstance,resCursor);
	::SetCursor(mhCursor);
	//::SetClassLong(m_hWnd,GCL_HCURSOR,(long)mhCursor);
	::SetClassLong(m_FrameWorkWnd,GCL_HCURSOR,(long)mhCursor);
	/*for (std::list< CChildWnd>::iterator it = oChildrenWnd.begin();  it != oChildrenWnd.end();  it++)
	{
		::SendMessage( it->hWnd, WM_CHILD_CURSOR, 0, (LONG)mhCursor);
	}*/
		//::SetClassLong(it->hWnd,GCL_HCURSOR,(long)mhCursor);
}

unsigned long CWS3D::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	// TODO: Add your message handler code here and/or call default
	::SetCursor(mhCursor);

	return 0;
}

int CWS3D::GetClientWidth()
{
	RECT rc;
	::GetClientRect(m_hParentWnd, &rc);
	if((rc.right-rc.left) > 0)
		muWindowClientWidth = (rc.right-rc.left);
	return muWindowClientWidth;
}

int CWS3D::GetClientHeight()
{
	RECT rc;
	::GetClientRect(m_hParentWnd, &rc);
	if((rc.bottom-rc.top) > 0)
		muWindowClientHeight = rc.bottom-rc.top;
	return muWindowClientHeight;
}

unsigned int CWS3D::GetMainWindowHandle()
{
	return (unsigned int)m_hParentWnd;
}

void CWS3D::MouseMoveToPos( short aX, short aY)
{
	RECT frameWorkRECT = {0,0,0,0};
	RECT rcClient = {0,0,0,0};
	::GetClientRect(m_FrameWorkWnd, &rcClient);
	::GetWindowRect(m_FrameWorkWnd, &frameWorkRECT);
	int windowX = frameWorkRECT.left;
	int windowY = frameWorkRECT.top + (frameWorkRECT.bottom - frameWorkRECT.top) - (rcClient.bottom - rcClient.top);
	// 1.1. смещаем абсциссу, т.к. у окошка есть бордюр
	windowX += 6;
	windowX += aX;
	windowY += aY;	
	POINT pt;
	::GetCursorPos(&pt);
	if( pt.x != windowX || pt.y != windowY)
		::SetCursorPos( windowX, windowY);
}

void CWS3D::SendMouseMessage( unsigned int msg, unsigned int wParam, short aX, short aY)
{
	/*if( msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN)
	{
		//::SetActiveWindow( m_FrameWorkWnd);
		//::SetCursorPos( windowX, windowY);
	}
	else
		int ff = 0;	*/
	//long lParam = (windowY << 16) + windowX;
	long lParam = (aY << 16) + aX;
	::SendMessage( m_FrameWorkWnd, msg, wParam, lParam);
	//::PostMessage(NULL , msg, wParam, lParam);
	//::SendMessage( HWND_BROADCAST, msg, wParam, lParam);
}

inline bool isEqual(const wchar_t* apch1, const wchar_t* apch2){
	for(;;){
		if(*apch1!=*apch2) return false;
		if(*apch1==L'\0') return true;
		apch1++, apch2++;
	}
}

void CWS3D::ExternCommand( const wchar_t* apwcCmd, const wchar_t* apwcPar){
	/*if(isEqual(apwcCmd,L"_i!TLE")){
		// взять EventID
		unsigned short uEventID=*apwcPar++;
		// взять имя (если есть)
		int iNameLen=0;
		const wchar_t* pwc=apwcPar;
		for(; *pwc!=L'\0'; pwc++){
			if(*pwc==L'|'){
				iNameLen=pwc-apwcPar;
				break;
			}
		}
		wchar_t* pwcName=NULL;
		if(iNameLen > 0){
			pwcName=new wchar_t[iNameLen+1];
			memcpy(pwcName,apwcPar,iNameLen*sizeof(wchar_t));
			pwcName[iNameLen]=L'\0';
			apwcPar=pwc+1;
		}
		//
		Fire_OnLogEvent((BSTR)pwcName, CComVariant(uEventID, VT_I4), CComVariant(1, VT_I4), (BSTR)apwcPar);
		if(pwcName!=NULL) delete pwcName;
	}else
		Fire_OnCommand( (const BSTR)apwcCmd, (const BSTR)apwcPar);*/
}

UINT CWS3D::Thread()
{
	return 0;
}

HRESULT CWS3D::ExecScript(BSTR sScript, VARIANT abImmediately)
{
	// TODO: Add your implementation code here
	wchar_t* pwcRetVal;
	CComVariant vImmediately(abImmediately);
	vImmediately.ChangeType(VT_BOOL);
	if(vImmediately.vt == VT_BOOL && vImmediately.bVal)
		pOMSContext->mpSM->ExecScript(sScript, -1, &pwcRetVal);
	else
		pOMSContext->mpSM->ExecScript(sScript);

//	mInput.AddEvent(omsUserEvent(uMsg,(unsigned int)wParam,(long)lParam));
	return S_OK;
}

/*
 * реализация rmml::mlISinchronizer
 */
omsresult CWS3D::CallEvent(unsigned int auID, MLBYTE* apData, MLWORD auDataLength){
	return OMS_ERROR_NOT_INITIALIZED;
}

// маска локального идентификатора (т.е. не зарегистрированного на сервере)
#define LOC_ID_MASK 0x40000000

omsresult CWS3D::GetObjectID(const wchar_t* apwcStringID, unsigned int &aiID){

	if(mpSynch.get()==NULL)
		return OMS_ERROR_NOT_INITIALIZED;
	mpSynch->GetObjectID(apwcStringID, aiID);
	aiID |= LOC_ID_MASK;
	return OMS_OK;
}

omsresult CWS3D::GetObjectStringID( unsigned int auID, rmml::mlOutString &asStringID){
	if(auID & LOC_ID_MASK){
		auID &= ~LOC_ID_MASK;
		if(mpSynch.get()==NULL)
			return OMS_ERROR_NOT_INITIALIZED;
		mpSynch->GetObjectStringID(auID, asStringID);
		return OMS_OK;
	}
	return OMS_ERROR_NOT_INITIALIZED;
	/*if(spLogMan==NULL) return OMS_ERROR_NOT_INITIALIZED;
	CComBSTR bstrStringID;
	if(FAILED(spLogMan->findObject(WS3D_LOG_CHANNEL, auID, &bstrStringID)))
		return OMS_ERROR_FAILURE;
	asStringID=bstrStringID;
	return OMS_OK;*/
}

void CWS3D::NotifyGetState(MLBYTE* apData, MLDWORD alSize){
return;
	mpSMState=(unsigned char*)GlobalAlloc(GMEM_FIXED,alSize+1);
	memcpy(mpSMState,apData,alSize);
	mSMStateSize=alSize;
	::SetEvent(mhEventGetSetState);
}

// отправить на сервер изменения в состояниях (synch-объектов) синхронизируемых объектов
omsresult CWS3D::SendStateChanges(MLBYTE* apData, int aiDataLength){
	return OMS_ERROR_NOT_INITIALIZED;
}

/**
 *
 */

omsresult CWS3D::setSoundManager( sm::smISoundManager *aSoundMan)
{
	// TODO: Add your implementation code here
	sWrapper.setSoundManager( aSoundMan);
	if ( pOMSContext->mpRM)
	{
		pOMSContext->mpRM->GetExtendedRM()->SetSoundManager((rm::rmISoundManager*)&sWrapper);
	}
	return OMS_OK;
}

unsigned long CWS3D::loadUI(BSTR sSrc)
{
	// TODO: Add your implementation code here
	if(pOMSContext.get()==NULL) return E_FAIL;
	if(pOMSContext->mpSM==NULL) return E_FAIL;
	if(sSrc==NULL || sSrc[0]==L'\0'){
		wchar_t* pwcUIScript=L"ui:ui.xml";
//		if(pOMSContext->mpResM->ResourceExists(pwcUIScript)){
		pOMSContext->mpSM->LoadUI(pwcUIScript);
		SendMessage2Launcher( APP_PERCENT, 97);
		pOMSContext->mpResM->SetModuleBase(L"");
		SendMessage2Launcher( APP_PERCENT, 98);
		/*pOMSContext->mpSM->LoadModuleMeta();
		pOMSContext->mpSM->LoadScene( 0);*/
//		}
	}else{
		pOMSContext->mpSM->LoadUI(sSrc);
	}
	// если приложение после запуска оказалось вдруг неактивным
	WINDOWINFO winf;
	winf.cbSize = sizeof(WINDOWINFO);
	if(GetWindowInfo(m_hParentWnd, &winf)){
		if(!(winf.dwWindowStatus & WS_ACTIVECAPTION))
			OnActivateApp(false);	// то надо уведомить об этом SceneManager
	}
	return S_OK;
}

HRESULT CWS3D::loadSceneN(UINT numScene)
{
	if(pOMSContext.get()==NULL) return E_FAIL;
	if(pOMSContext->mpSM==NULL) return E_FAIL;
	// если moduleBase не установлен
	if(!mpIResourceManager) return E_FAIL;

	pOMSContext->mpSM->LoadScene(numScene);
	/*if(pOMSContext->mpSM->GetMode()!=rmml::smmLoading) 	
		Fire_OnSceneLoaded();*/
	return S_OK;
}

unsigned long CWS3D::loadScene(BSTR sSrc)
{
	if(pOMSContext.get()==NULL) return E_FAIL;
	if(pOMSContext->mpSM==NULL) return E_FAIL;
	// если moduleBase не установлен
	if(mpIResourceManager){
//		BSTR psModuleBase;
//		res::gpResourceManagerWrapper->getModuleBase(psModuleBase);
//		"META-INF"
		wchar_t* pwcFullPath=mpIResourceManager->GetFullPath(sSrc);
		CComString str(pwcFullPath);
		str.Replace('/','\\');
		CComString sModuleBase;
		for(;;){
			str.Delete(str.GetLength()-1);
			int iSl=str.ReverseFind('\\');
			if(iSl <= 0) break;
			iSl++; // оставляем '\\'
			str.Delete(iSl, str.GetLength()-iSl);
			CComString strMeta=str;
			strMeta+="META-INF";
			BSTR bstrMeta=strMeta.AllocSysString();
			unsigned int codeError = 0;
			if(mpIResourceManager->ResourceExists(bstrMeta, codeError)){
				sModuleBase=str; 
			}
			SysFreeString(bstrMeta);
			if(!sModuleBase.IsEmpty()){
				BSTR bstrModuleBase=sModuleBase.AllocSysString();
				mpIResourceManager->SetModuleBase(bstrModuleBase);
				SysFreeString(bstrModuleBase);
				break;
			}
		}
	}
	//
	pOMSContext->mpSM->LoadScene(sSrc);
	mSMLastMode=pOMSContext->mpSM->GetMode();
	if(mSMLastMode!=rmml::smmLoading)
	{
		//Fire_OnSceneLoaded();
	}
	else
		mSMLastMode=rmml::smmLoading;

	return S_OK;
}

/*HRESULT CWS3D::saveScene(BSTR sFilePath)
{
	// TODO: Add your implementation code here
	rmmld::mlIAuthoring* pAuth=GetMLAuthoring();
	if(pAuth==NULL) return E_FAIL;
	pAuth->SaveRMML(sFilePath);
	return S_OK;
}*/

unsigned long CWS3D::OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
	COPYDATASTRUCT* pCopyData=(COPYDATASTRUCT*)lParam;
	HWND hwndDebugger=(HWND)wParam;

	if(pCopyData->dwData <= 1)
	{
		mTracer.Send(hwndDebugger,pCopyData->dwData==0?true:false);
	}
	return S_OK;
}

bool CWS3D::OnActivateApp(bool abActivated){
//if(pOMSContext->mpLogWriter)
//pOMSContext->mpLogWriter->WriteLn("app activated: \n",CLogValue(abActivated?1:0));
	if(mbAppActive == abActivated)
		return false;
	if(pOMSContext->mpSM != NULL){
		if(abActivated)
			pOMSContext->mpSM->OnAppActivated();
		else
			pOMSContext->mpSM->OnAppDeactivated();
	}
	mbAppActive = abActivated;
	return true;
}

// фактический размер клиентской части окна изменился
bool CWS3D::OnWindowSizeChanged(unsigned int auiWidth, unsigned int auiHeight)
{
	muWindowClientWidth = auiWidth;
	muWindowClientHeight = auiHeight;

	if( pOMSContext->mpSM)
	{
		pOMSContext->mpSM->OnWindowSizeChanged( muWindowClientWidth, muWindowClientHeight);
	}
	return true;
}

void CWS3D::SendDebugInfo(unsigned char acInfoType, void* ahwndDebugger
						  , unsigned char* apData, unsigned int auDataLength)
{
	HWND hwndDebugger=(HWND)ahwndDebugger;
	COPYDATASTRUCT CopyData;
	CopyData.dwData=acInfoType;
	CopyData.cbData=auDataLength;
	CopyData.lpData=apData;

	::SendMessage(hwndDebugger, WM_COPYDATA, NULL, (LPARAM)(LPVOID) &CopyData);
}

HRESULT CWS3D::SetAvatarID(UINT aid)
{
	// TODO: Add your implementation code here
	avatarID = aid;
	return S_OK;
}

unsigned long CWS3D::initResManager()
{
	ATLASSERT(pOMSContext->mpComMan);
	mpIResourceManager = pOMSContext->mpResM;
	mpIResourceManager->InitResourceManager();
	//mpIResourceManager->setResourceManager(aResMan);
	SendMessage2Launcher( APP_PERCENT, 11);
	if( pOMSContext->mpRM)
	{
		pOMSContext->mpRM->GetExtendedRM()->SetResourceManager(mpIResourceManager);
		SendMessage2Launcher( APP_PERCENT, 12);
		pOMSContext->mpRM->GetExtendedRM()->SetCommunicationManager(mpICommunicationManager);
		SendMessage2Launcher( APP_PERCENT, 13);
		pOMSContext->mpRM->GetExtendedRM()->SetSceneManager(pOMSContext.get()->mpSM);
		pOMSContext->mpRM->GetExtendedRM()->SetSoundManager((rm::rmISoundManager*)&sWrapper);
		sWrapper.SetResourceMan( pOMSContext->mpIResM);
	}
	return S_OK;
}

HRESULT CWS3D::get_UISkinBase(BSTR *pVal)
{
	if(mpIResourceManager)
	{
		wchar_t *p = NULL; 
		mpIResourceManager->GetUISkinBase((const wchar_t**)&p);
		*pVal = p;
	}
	return S_OK;
}

HRESULT CWS3D::put_UISkinBase(BSTR newVal)
{
	mwsUISkinBase=newVal;
	if(!mpIResourceManager) return S_OK;
	mpIResourceManager->SetUISkinBase(mwsUISkinBase.data());
	// если надо, перегружаем UI
	// ??
	return S_OK;
}

unsigned long CWS3D::put_PathIni(BSTR newVal)
{
	if( !newVal)	return S_FALSE;
	CComString	str( newVal);	str.MakeLower();
	msPathIni = newVal;
	return S_OK;
}

HRESULT CWS3D::GetCSForWindow(HWND ahWnd, IUnknown** apCS)
{
	if(apCS == NULL)
		return E_INVALIDARG;
	*apCS = (IUnknown*)pOMSContext->mpRM->GetCSForWindow(ahWnd);
	return S_OK;
}

void CWS3D::Refresh()
{
	Tick();
}

// взять имена файлов, прописанных в конфигурации трехмерного объекта
int CWS3D::GetFilesFromCFG(int iType, char* apszCFGFileName, char* appszFileNames, int bufCount)
{
	if( pOMSContext->mpRM)
		return pOMSContext->mpRM->GetExtendedRM()->GetFilesFromCFG(iType, apszCFGFileName, appszFileNames, bufCount);
	return 0;
}

/*oms::omsIWSWrapper*		CWS3D::GetIWSWrapper()
{
	return this;
}*/

HRESULT CWS3D::setAvatarName(BSTR sName)
{
	// TODO: Add your implementation code here
	sMyUserName = cLPCSTR(sName);
	return S_OK;
}

unsigned long CWS3D::initialize(HWND wnd)
{
	m_hParentWnd = wnd;
	m_FrameWorkWnd = wnd;
	::GetWindowRect(wnd, &m_FrameWorkRECT);
	if (createManagers())
	{
		return	S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT CWS3D::getSysCommand(BSTR *psCmd)
{
	*psCmd=(BSTR)msSysCommand.c_str();
	return S_OK;
}

HRESULT CWS3D::Resize(short aiWidth, short aiHeight)
{
	// ??
	return S_OK;
}

unsigned long CWS3D::Update(DWORD adwTime)
{
	if (needExit)
	{
		return 1;
	}

	// ??
	Tick();

	UpdateSleepingState();

	static unsigned int lastKeyboardStateUpdateTime = GetTimeStamp();
	const unsigned int MIN_KEYBOARD_UPDATE_TIME = 1000;

	if ((GetTimeStamp() - lastKeyboardStateUpdateTime > MIN_KEYBOARD_UPDATE_TIME) ||
		(lastKeyboardStateUpdateTime - GetTimeStamp() > MIN_KEYBOARD_UPDATE_TIME))
	{
		lastKeyboardStateUpdateTime = GetTimeStamp();
		bool isKeyboardStateChanged = false;

		bool currIsAlt = (GetKeyState(VK_MENU) & 0x8000)!=0;
		bool currIsCtrl = (GetKeyState(VK_CONTROL) & 0x8000)!=0;
		bool currIsShift = (GetKeyState(VK_SHIFT) & 0x8000)!=0;

		if (m_isAlt != currIsAlt)
		{
			m_isAlt = !m_isAlt;
			isKeyboardStateChanged = true;
		}
		if (m_isCtrl != currIsCtrl)
		{
			m_isCtrl = !m_isCtrl;
			isKeyboardStateChanged = true;
		}
		if (m_isShift != currIsShift)
		{
			m_isShift = !m_isShift;
			isKeyboardStateChanged = true;
		}

		if (isKeyboardStateChanged)
		{
			if (pOMSContext->mpRM->GetMouseController())
			{
				pOMSContext->mpRM->GetMouseController()->OnKeyDown(0, 0, m_isAlt, m_isCtrl, m_isShift);
			}
		}
	}

	return 0;
}

// проверяем активность пользователя по нажатию на клавишу или на события от мыши
// или свернуто ли окно
void CWS3D::UpdateSleepingState()
{
	if (pOMSContext->mpSM)
	{
		unsigned int curTime = 	GetTimeStamp();
		BOOL isIc = IsIconic(m_FrameWorkWnd);

		unsigned int diff = curTime - m_lastUserActivityTime;

		bool isActiveByActions = (GetTimeStamp() - m_lastUserActivityTime < MIN_USER_INACTIVE_TIME);
		bool isActiveNow = (isActiveByActions && !isIc);

		if ( isActiveNow != m_isSleepingUserState) 
		{
			if (isActiveNow)
			{
				pOMSContext->mpSM->OnUserWakeUp();
			}
			else
			{
				pOMSContext->mpSM->OnUserSleep();
			}			
			
			m_isSleepingUserState = isActiveNow;
		}	

		if ((m_windowMovingTime != 0) && (timeGetTime() - m_windowMovingTime > 500))
		{
			// считаем, что перемещение окна завершилось
			pOMSContext->mpSM->OnWindowMoved();
			m_windowMovingTime = 0;
		}
	}

}

BOOL CWS3D::initOMSContext()
{
	pOMSContext = std::auto_ptr<oms::omsContext>(new oms::omsContext);
	mpSynch = std::auto_ptr<wsSynchronizer>(new wsSynchronizer(pOMSContext.get()));
	//
	pOMSContext->mpInput=&mInput;
	pOMSContext->mpWindow=this;
	pOMSContext->mpTracer=&mTracer;
	pOMSContext->mpSynch=this;
	pOMSContext->mpApp=this;
	pOMSContext->mp3DWS=this;
	return true;
}

omsContext *CWS3D::getOMSContext()
{
	return pOMSContext.get();
}

BOOL CWS3D::createManagers()
{	
	SendMessage2Launcher( APP_PERCENT, 7);
	info::CreateInfoManager( pOMSContext.get());
	CreateLogManagersImpl( pOMSContext.get());
	// VOIP должен быть первым, так как на него вешается Callback в сценаменеджере
	CreateVoipManagerImpl( pOMSContext.get());
	CreateDesktopManagerImpl( pOMSContext.get());

	rmml::CreateSceneManager( pOMSContext.get());
	sWrapper.setOMSContext( pOMSContext.get());
	SendMessage2Launcher( APP_PERCENT, 11);
	CreateSoundManagerImpl( pOMSContext.get());	

	CreateComManagerImpl( pOMSContext.get());
	ATLASSERT(pOMSContext->mpComMan);
	mpICommunicationManager = pOMSContext->mpComMan;

	rm::CreateRenderManager( pOMSContext.get());

	ATLASSERT(pOMSContext->mpRM);
	initResManager();
	if( pOMSContext->mpRM)
	{
		rm::rmIRenderManagerAX*	pRMExt = pOMSContext->mpRM->GetExtendedRM();
		SendMessage2Launcher( APP_PERCENT, 15);

		// надо передать флаг gbCave в RM до SetControlWindow

		if( !pRMExt->SetControlWindow( m_FrameWorkWnd, m_FrameWorkRECT))
		{
			exit();
			return FALSE;
		}

		//pOMSContext->mpLogWriter = pOMSContext->mpRM->GetLogWriter();
		pRMExt->SetSceneManager(pOMSContext->mpSM);
	}

	SendMessage2Launcher( APP_PERCENT, 55);

	mtStartTime=timeGetTime();
	mhCursor=LoadCursor(NULL,IDC_ARROW);
	SetEditMode(FALSE);		

	sync::CreateSyncManager( pOMSContext.get());

	service::CreateServiceManager( pOMSContext.get());

	service::CreateServiceWorldManager( pOMSContext.get());

	SendMessage2Launcher( APP_PERCENT, 60);	

	return TRUE;
}

void CWS3D::OnAuthorizated( int aiTypeSendLog)
{
	if ( pOMSContext->mpSyncMan != NULL)
	{
		pOMSContext->mpSM->SetSyncCallbacks();
		pOMSContext->mpSyncMan->Connect();
	}
	if (pOMSContext->mpServiceMan != NULL)
	{
		pOMSContext->mpSM->SetServiceCallbacks();
		pOMSContext->mpServiceMan->Connect();
	}

	if(pOMSContext->mpLogger != NULL)
	{
		pOMSContext->mpLogger->SetMode( aiTypeSendLog>0?true:false);
		if(aiTypeSendLog>0)
		{
			if((aiTypeSendLog & SEND_LOGWRITER_LOG) != 0)
			{
				//pOMSContext->mpRM->SetLogMode(true);
				pOMSContext->mpLogWriter->StartSendingToServer( (lgs::ILoggerBase*)pOMSContext->mpLogger);
			}
			else
				pOMSContext->mpLogWriter->StopSendingToServer();
			if((aiTypeSendLog & SEND_TRACER_LOG) != 0)
			{
				pOMSContext->mpTracer->StartSendingToServer( (lgs::ILoggerBase*)pOMSContext->mpLogger);
				//pOMSContext->mpTracer->setTraceMode(true);
			}	
			else
				pOMSContext->mpTracer->StopSendingToServer();
		}
	}
}

// активно ли окно приложения на данный момент?
bool CWS3D::IsActive(){
	return mbAppActive;
}

void CWS3D::Destroy()
{
	DestroyContext();
}

void CWS3D::SetEditMode(BOOL abEdit)
{
	if(abEdit)
	{
		//KillTimer(1);
		//SetTimer(1,1000);
	}
	else
	{
		//KillTimer(1);
		//SetTimer(1,1000/24); // 24 fps
	}
}

unsigned int CWS3D::GetError()
{
	LPVOID lpMsgBuf;
	int ff = GetLastError();
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
		// Handle the error.
		return ff;
	}

	//MessageBox( 0, (char*)lpMsgBuf, "", MB_OK);
	// to tracer
	pOMSContext->mpTracer->trace("execSysCommand ERROR: ");
	pOMSContext->mpTracer->trace((const char *)lpMsgBuf);

	// Display the string.
//	//MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );
	return ff;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <float.h>
#include <signal.h>

void sigfpe_test()
{ 
	// Code taken from http://www.devx.com/cplus/Article/34993/1954

	//Set the x86 floating-point control word according to what
	//exceptions you want to trap. 
	_clearfp(); //Always call _clearfp before setting the control
	  
	//_controlfp_s(&cwOriginal, cw, MCW_EM); //Set it.
	unsigned int control_word = 0;
	_controlfp_s(&control_word, EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|
				EM_DENORMAL|EM_INVALID|EM_INEXACT, MCW_EM); //Get the default control
	//MCW_EM is defined in float.h.
	//Restore the original value when done:
	//_controlfp(cwOriginal, MCW_EM);

	// Divide by zero

	float a = 1;
	float b = 0;
	float c = a/b;
	c; 
}

class CDerived;
class CBase
{
public:
	CBase(CDerived *derived): m_pDerived(derived) {};
	~CBase();
	virtual void function(void) = 0;

	CDerived * m_pDerived;
};

#pragma warning(disable:4355)
class CDerived : public CBase
{
public:
	CDerived() : CBase(this) {};   // C4355
	virtual void function(void) {};
};

CBase::~CBase()
{
	m_pDerived -> function();
}

#define BIG_NUMBER 0x00ffffff

class CTestFailed
{
public:
	CTestFailed(){ x = 0;};
	~CTestFailed(){};

	void run()
	{
		x++;
	}

	void recure_alloc()
	{
		int *pi = new int[ BIG_NUMBER];
		int ax = 0;
		recure_alloc();
	}

	void run_access_violation()
	{
		// Access violation
		x++;		
		CTestFailed *t = new CTestFailed();
		delete t;
		t = NULL;
		t->run();
		x++;
	}

	void run_terminate()
	{
		// terminate. Each new thread needs to install its own 	
		// Call terminate
		terminate();
	}

	void run_unexpected()
	{
		// unexpected. Each new thread needs to install its own 	
		// Call unexpected
		unexpected();
	}

	void run_invalid_parameter()
	{
		// invalid parameter
		char* formatString;
		// Call printf_s with invalid parameters.
		formatString = NULL;
#pragma warning(disable : 6387)   // warning C6387: 'argument 1' might be '0': this does not adhere to the specification for the function 'printf'
		printf(formatString);
#pragma warning(default : 6387) 		
	}

	void _pure_virtual()
	{
		int xa = 1;
		xa *= 20;
		CDerived derived;
		xa++;
	}

	void run_pure_virtual()
	{
		// pure virtual method call. Each new thread needs to install its own
		// pure virtual method call	
		int sa = 0;
		int awe = 10;
		sa = awe + sa;
		if ( sa > 0)
		{
			_pure_virtual();
		}		
		awe = sa + awe;
		int s = awe;
	}

	void run_recure_alloc()
	{
		 //new operator fault
		recure_alloc();
	}

	void run_abort()
	{
		// SIGABRT. Each new thread needs to install its own
		// Call abort
		abort();
	}

	void run_sigfpe()
	{
		// SIGFPE. Each new thread needs to install its own
		// floating point exception ( /fp:except compiler option)
		sigfpe_test();
	}

	void run_sigill()
	{
		// SIGILL. Each new thread needs to install its own
		raise(SIGILL);
	}

	void run_sigint()
	{
		// SIGINT 
		raise(SIGINT);
	}

	void run_sigsecgv()
	{
		// SIGSEGV. Each new thread needs to install its own 	
		raise(SIGSEGV);
	}

	void run_sigterm()
	{
		// SIGTERM. Each new thread needs to install its own
		raise(SIGTERM);
	}

	void run_raiseException()
	{
		// RaiseException 
		// Raise noncontinuable software exception
		RaiseException(123, EXCEPTION_NONCONTINUABLE, 0, NULL);
	}

	void run_throw()
	{
		// throw 
		// Throw typed C++ exception.
		throw 13;
	}

protected:
	int x;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Возвращает true, если командная строка содержит в себе подстроку .exe
// Считаем, что это запуск приложения.
bool CWS3D::execSysCommandExeOnly(const wchar_t* apwcCommandLine, unsigned int* apErrorCode)
{
	if( !apwcCommandLine)
	{
		if( pOMSContext->mpLogWriter != NULL)
			pOMSContext->mpLogWriter->WriteLn("execSysCommandExeOnly apwcCommandLine == NULL");
		return false;
	}

	CWComString sPath = apwcCommandLine;
	sPath.MakeLower();

	bool abResult = false;
	unsigned int auErrorCode = 0;

	if( sPath.Find( L"updater.exe", 0) >-1 && sPath.Find( L"http://", 0) == -1)
	{
		if( pOMSContext->mpLogWriter != NULL)
			pOMSContext->mpLogWriter->WriteLn("execSysCommandExeOnly execute program");
		abResult = true;

		STARTUPINFOW si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si,sizeof(si));
		si.cb = sizeof(si);

		ZeroMemory(&pi,sizeof(pi));

		BOOL res = CreateProcessW(NULL, sPath.GetBuffer(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
		if(!res)
		{
			auErrorCode = GetError();
			if( pOMSContext->mpLogWriter != NULL)
				pOMSContext->mpLogWriter->WriteLn("execSysCommandExeOnly ErrorCode==", auErrorCode);
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	if( apErrorCode)
		*apErrorCode = auErrorCode;

	return abResult;
}

bool CWS3D::execSysCommand(const wchar_t* apwcCommandLine, unsigned int* apErrorCode)
{
	if( !apwcCommandLine)
		return false;

	msSysCommand = apwcCommandLine;

	if( execSysCommandExeOnly( apwcCommandLine, apErrorCode))
		return true;

	CExecCommandThreadVecIt it = m_execCommandThreadVec.begin();	
	for ( ; it != m_execCommandThreadVec.end();)
	{
		// проходим по всем объектам команд
		CExecCommandThread *m_pExecCommandThread = *it;
		if ( m_pExecCommandThread != NULL)
		{		
			if ( m_pExecCommandThread->GetStatus() == CExecCommandThread::FINISHED)
			{
				// команда завершила работу удаляем объект
				MP_DELETE( m_pExecCommandThread);				
				it = m_execCommandThreadVec.erase( it);
			}
			else
				it++;
		}
		else
			it++;
	}

	CExecCommandThread *m_pExecCommandThread = MP_NEW( CExecCommandThread);	
	m_pExecCommandThread->Start( apwcCommandLine, this);
	m_execCommandThreadVec.push_back( m_pExecCommandThread);

	return 0;
}

bool CWS3D::IsExecAnySysCommand()
{
	CExecCommandThreadVecIt it = m_execCommandThreadVec.begin();
	CExecCommandThreadVecIt itEnd = m_execCommandThreadVec.end();
	for ( ; it != itEnd; it++)
	{
		// проходим по всем объектам команд
		CExecCommandThread *m_pExecCommandThread = *it;
		if ( m_pExecCommandThread != NULL)
		{		
			if ( m_pExecCommandThread->GetStatus() != CExecCommandThread::FINISHED)
			{
				return true;
			}			
		}
	}
	return false;
}


//#define N3D_RELEASE
bool CWS3D::GetDebugVersion()
{
#ifdef AAAA
	return false;
#else
	return true;
#endif
/*#ifndef DEBUG
	return false;
#else
	#ifdef N3D_RELEASE
		return false;
	#else
		return true;
	#endif
#endif*/
}

void CWS3D::minimize(){
	//Fire_OnMinimize();
}

void CWS3D::exit()
{
	needExit = true;	
}

// загрузить dll
unsigned long CWS3D::LoadLibrary(const wchar_t* apcDllName){
	if(apcDllName == NULL) return 0;
	WCHAR lpcCurDir[MAX_PATH+200];
	::GetCurrentDirectoryW(MAX_PATH+150, lpcCurDir);
	WCHAR lpcModuleFileName[MAX_PATH+200];
	//::GetModuleFileName( _Module.GetModuleInstance(), lpcModuleFileName, MAX_PATH+150);
	::GetModuleFileNameW( NULL, lpcModuleFileName, MAX_PATH+150);
	WCHAR lpcDllFullPathName[MAX_PATH+200];
	LPWSTR lpFileName=NULL;
	::GetFullPathNameW(lpcModuleFileName,MAX_PATH+150,lpcDllFullPathName,&lpFileName);

	//MessageBox(0,"apcDllName",apcDllName,1);

	//MessageBox(0,"lpFileName",lpFileName,1);
	if(lpFileName!=NULL){
		*lpFileName=L'\0';
		::SetCurrentDirectoryW(lpcDllFullPathName);
	}

	//MessageBox(0,"lpcDllFullPathName",lpcDllFullPathName,1);

	HMODULE hModule = ::LoadLibraryW(lpcDllFullPathName);
	::SetCurrentDirectoryW(lpcCurDir);
	if(hModule == NULL)
		hModule = ::LoadLibraryW(apcDllName);
	return (unsigned long)hModule;
}

// освободить dll
void CWS3D::FreeLibrary(unsigned long auHModule){
	::FreeLibrary((HMODULE)auHModule);
}

void* CWS3D::GetProcAddress(unsigned long auHModule, const char* apcProcName){
	if(auHModule == 0)
		return NULL;
	return ::GetProcAddress((HMODULE)auHModule,apcProcName);
}

void CWS3D::GetStringGUID( wchar_t* apwcGUID, unsigned short aSizeGUID)
{
	apwcGUID[ 0] = 0;
	GUID guid;
	if (SUCCEEDED( CoCreateGuid( &guid)))
		apwcGUID[ StringFromGUID2( guid, apwcGUID, aSizeGUID)] = 0;
}

bool CWS3D::ResizeWindow(unsigned auClientWidth, unsigned auClientHeight, int aiFullScreen)
{
	muWindowClientWidth = auClientWidth;
	muWindowClientHeight = auClientHeight;
	// Перерендериваем только, если разрешение окна реально поменялось - nap
	return SetScreenMode( auClientWidth, auClientHeight, 32, aiFullScreen);
}

void CWS3D::ResizeWindowForOculus(unsigned auClientWidth, unsigned auClientHeight, unsigned x, unsigned y) {
	muWindowClientWidth = auClientWidth;
	muWindowClientHeight = auClientHeight;
	SetScreenModeForOculus( auClientWidth, auClientHeight, x, y);
}

// получить текущее время (timeGetTime для отладки)
unsigned long CWS3D::GetTimeStamp()
{
	return timeGetTime();
}

// получить версию плеера в строковом виде
bool CWS3D::GetVersion(wchar_t* apwcVersion, unsigned short aSizeVersion){
	wstring currentVersion = GetRegVersion();
	if( currentVersion.empty())	return false;
	ATLASSERT( aSizeVersion > currentVersion.length());
	byte requiredBufferLength = currentVersion.length()+1;
	if( aSizeVersion <= requiredBufferLength)	return false;
	// функция добавит 0 в конце строки, так передается длина на символ больше
	wcsncpy_s( apwcVersion, aSizeVersion, currentVersion.c_str(), requiredBufferLength);
	return true;
}

// получить язык в строковом виде, на котором следует выводить все сообщения 
bool CWS3D::GetLanguage(wchar_t* apwcLanguage, unsigned short aSizeLanguage){
	if(apwcLanguage == NULL || aSizeLanguage < 2) return false;
	wcscpy_s(apwcLanguage, aSizeLanguage, L"en");
	wstring currentLanguage = GetRegLanguage();
	if( currentLanguage.empty()) return true; // если не указан, то английский
	_wcslwr_s((wchar_t*)(currentLanguage.c_str()), currentLanguage.length()+1);
	int iStrLen = currentLanguage.size();
	if((aSizeLanguage - 1) < iStrLen)
		iStrLen = aSizeLanguage - 1;
	wcsncpy_s( apwcLanguage, aSizeLanguage, currentLanguage.c_str(), iStrLen);
	apwcLanguage[iStrLen] = L'\0';
	return true;
}

// сохранить уставновку в реестре
bool CWS3D::SetSetting(const wchar_t* apwcName, const wchar_t* apwcValue){
	HKEY	hKey;
	USES_CONVERSION;
	if(msPathIni.size()>1)
	{
		CComString sValue; 
		if(WritePrivateProfileStringW(L"user", apwcName, apwcValue, msPathIni.c_str()))
		{
			return true;
		}
	}
	// открыть в реестре msRegKey
	else if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, W2A(msRegKey.c_str()), 0, KEY_READ, &hKey)==ERROR_SUCCESS && hKey!=0)
	{
		CComString sName = (wchar_t*)apwcName;
		CComString sValue = (wchar_t*)apwcValue;
		if(RegSetValueEx(hKey, sName.GetString(), 0, REG_SZ, (BYTE*)sValue.GetString(), sValue.GetLength())==ERROR_SUCCESS)
			return true;
	}
	return false;
}

// получить установку из ini
bool CWS3D::GetSettingFromIni(const char* apwcSection, const char* apwcName, char* apwcValue, unsigned short aSizeValue)
{
	USES_CONVERSION;
	CComString sValue = GetValueStrFromIni( apwcSection, apwcName);
	if( sValue.IsEmpty())
		return false;

	if(sValue.GetLength() < aSizeValue){
		strcpy_s(apwcValue, aSizeValue, sValue.GetBuffer());
	}else{
		strncpy_s(apwcValue, aSizeValue, sValue.GetBuffer(), aSizeValue - 1);
		apwcValue[aSizeValue - 1] = '\0';
	}
	return true;
}

// получить установку из реестра
bool CWS3D::GetSetting(const wchar_t* apwcName, wchar_t* apwcValue, unsigned short aSizeValue, const wchar_t* apwcSectionName){
	HKEY	hKey;
	USES_CONVERSION;
	// если параметр пустая строка
	if (apwcSectionName == NULL || apwcSectionName[0] == 0)
		apwcSectionName = L"user";
	if(msPathIni.size()>1)
	{
		CWComString sValue; 
		if(GetPrivateProfileStringW( apwcSectionName, apwcName, L"", sValue.GetBuffer(MAX_PATH+1), MAX_PATH, msPathIni.c_str()))
		{
			 //apwcValue = sValue.AllocSysString();
			wstring wsValue = sValue.GetBuffer();
			if(wsValue.size() < aSizeValue){
				wcscpy_s(apwcValue, aSizeValue, wsValue.c_str());
			}else{
				wcsncpy_s(apwcValue, aSizeValue, wsValue.c_str(), aSizeValue - 1);
				apwcValue[aSizeValue - 1] = L'\0';
			}
			 return true;
		}
	}
	else if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, W2A(msRegKey.c_str()), 0, KEY_READ, &hKey)==ERROR_SUCCESS && hKey!=0)
	{
		// получить длину значения
		CComString sName = (wchar_t*)apwcName;
		CComString value;
		DWORD dwType = REG_SZ;
		DWORD dwSize;
		LONG lRes = RegQueryValueEx( hKey, sName.GetString(), 0, &dwType, NULL, &dwSize);
		if( dwSize > 0)
		{
			lRes = RegQueryValueEx( hKey, sName.GetString(), 0, &dwType, (BYTE*)value.GetBuffer( dwSize), &dwSize);
			RegCloseKey( hKey);
			if (lRes == ERROR_SUCCESS)
			{
				// скопировать полученное значение в apwcValue
				wstring wsValue = wstring( A2W(value.GetBuffer()));
				if(wsValue.size() < aSizeValue){
					wcscpy_s(apwcValue, aSizeValue, wsValue.c_str());
				}else{
					wcsncpy_s(apwcValue, aSizeValue, wsValue.c_str(), aSizeValue - 1);
					apwcValue[aSizeValue - 1] = L'\0';
				}
				return true;
			}
		}
		RegCloseKey( hKey);
	}
	return false;
}


void CWS3D::GetFileFilter(CWComString &sFilter, CWComString &sExt)
{
	wchar_t pwcLang[20] = L"eng";
	GetLanguage(pwcLang, 19);
	if( sExt == L"pdf")
		sFilter = L"PDF (*.pdf)|*.pdf";
	else if( sExt == L"ppt" || sExt == L"pptx")
	{
		if(pwcLang[0]==L'r' && pwcLang[1]==L'u') // russian
			sFilter = L"PowerPoint презентации (*.ppt;*.pptx)|*.ppt;*.pptx";
		else
			sFilter = L"PowerPoint presentations (*.ppt;*.pptx)|*.ppt;*.pptx";
	}
	
	else if( sExt == L"jpg" || sExt == L"jpeg" || sExt == L"jpe")
	{
		sFilter = L"JPEG (*.jpg;*.jpeg;*.jpe)|*.jpg;*.jpeg;*.jpe";
	}
	else
	{
		CWComString sName = sExt;
		sName.MakeUpper();
		sFilter = sName + L" (*" + sExt + L")" + L"|*." + sExt;
	}
}

// через стандартное диалоговое окно получить путь к файлу на локальном компьютере 
bool CWS3D::GetLocalFilePath(const wchar_t* apwcFilter, const wchar_t* apwcStartPath, bool abExists, wchar_t* pwcPath, unsigned short aSizePath, const wchar_t* apszDefaultName)
{
	CWComString sCurDir;
	GetCurrentDirectoryW(MAX_PATH, sCurDir.GetBuffer( MAX_PATH+1));
	pwcPath[0] = 0;

	OPENFILENAMEW ofn;
	wchar_t szFile[MAX_PATH*4];       // buffer for file name
	szFile[0] = '\0';

	CWComString sFilter = apwcFilter;
	wchar_t pwcLang[20] = L"eng";
	GetLanguage(pwcLang, 19);

	CWComString sExt = sFilter;	
	if(sFilter.GetLength() > 0)
	{
		if( sFilter.Find(L"|") == -1 && sFilter != L"*.*")
		{
			if( sFilter.Find(L"|") == -1)
			{
				sExt.Replace(L"*.", L"");
				GetFileFilter(sFilter, sExt);		
			}
			else
				sExt = L"";
			sFilter += L'|';
		}
		else if( sFilter == L"*.*")
			sFilter = L"";
		else
			sFilter += L'|';
		sFilter.Replace( L"||", L"|");
	}
	if(pwcLang[0]==L'r' && pwcLang[1]==L'u') // russian
		sFilter += L"Все файлы (*.*)|*.*";
	else
		sFilter += L"All (*.*)|*.*";
	sFilter += L'|';	
	
	int iFilterLen = sFilter.GetLength();
	wchar_t* pszFilter = sFilter.AllocSysString();
	for(int ii = 0; ii < iFilterLen; ii++){
		if(pszFilter[ii] == L'|')
			pszFilter[ii] = L'\0';
	}

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_FrameWorkWnd;
	ofn.lpstrFile = szFile;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	wcsncpy_s(szFile, MAX_PATH*4, apszDefaultName, wcslen(apszDefaultName));
	szFile[wcslen(apszDefaultName)] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = pszFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	CWComString sFileName = L"";

	if(abExists)
	{
		if (!GetOpenFileNameW(&ofn))
			return false;
		sFileName = ofn.lpstrFile;
	}else
	{
		sExt.Insert(0, L".");
		while( true)
		{			
			if (!GetSaveFileNameW(&ofn))
				return false;

			sFileName = ofn.lpstrFile;
			if ( ofn.nFilterIndex == 1)
			{								
				int pos = sFileName.ReverseFind(L'.');
				CWComString sExtFile = sExt;
				if( pos>-1 && pos > sFileName.GetLength() - 6)
				{
					sExtFile = sFileName.Right( sFileName.GetLength() - pos - 1);
					if( sFilter.Find(sExtFile, 0) == -1)
						sFileName += sExt;
				}
				else
					sFileName += sExt;
			}
			else 
				sExt = L"";

			HANDLE hFile = CreateFileW( sFileName.GetBuffer(), 0, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(hFile);
				CComString sTitle, sText;
				if(pwcLang[0]==L'r' && pwcLang[1]==L'u') 
				{
					sTitle = "Подтверждение";
					sText = "Перезаписать файл?";
				}
				else
				{
					sTitle = "Confirm";
					sText = "Overwrite file?";
				}
				int res = MessageBox( m_hParentWnd, sText.GetBuffer(), sTitle.GetBuffer(), MB_YESNO | MB_ICONINFORMATION);
				if( res == IDNO)
				{
					continue;
				}			
			}
			break;	
		}
	}
	SetCurrentDirectoryW(sCurDir.GetBuffer());
	if( sFileName.GetLength() == 0)
		return false;

	int iStrLen = sFileName.GetLength();
	if(aSizePath <= iStrLen)
		iStrLen = aSizePath - 1;
	if(iStrLen < 0) iStrLen = 0;
	wcsncpy_s(pwcPath, aSizePath, sFileName.GetBuffer(), iStrLen);
	pwcPath[iStrLen] = L'\0';

	return true;
}

// (если aiX < 0, то используются текущие координаты указателя мыши)
bool CWS3D::FireMouseDown(int aiX, int aiY)
{
	// ??
	ATLASSERT( FALSE);
	return false;
}


// получить аргументы процесса (заданные в командной строке, например)
wchar_t** CWS3D::GetArguments(){
	if(mvArguments.size() == 0){
		mvArguments.push_back(NULL);
	}else{
		wchar_t* pwc = *mvArguments.rbegin();
		if(pwc != NULL){
			mvArguments.push_back(NULL);
		}
	}
	return (wchar_t**)&(*mvArguments.begin());
}

BYTE aKeyState[256];
bool bKeyStateInitialized = false;

// провести трансляцию скан-кодов в ascii и unicode
void CWS3D::TranslateKeyCode(unsigned short muKeyCode, unsigned short& muKeyStates, unsigned short muScanCode, unsigned short& muAscii, unsigned short& muUnicode, bool& mbAlt, bool& mbCtrl, bool& mbShift){
	if(!bKeyStateInitialized){
		for(int i = 0; i< 256; i++) aKeyState[i] = 0;
		bKeyStateInitialized = true;
	}

// muKeyCode // wParam
// muScanCode // scan code - младший байт
	UINT uScanCode = muScanCode & 0xFF;
	GetKeyboardState(aKeyState);

  aKeyState[VK_CAPITAL] = LOBYTE(::GetKeyState(VK_CAPITAL));

// актуализируем состояние нажатия shift-а на момент нажатия кнопки
  aKeyState[VK_SHIFT]  = HIBYTE(::GetKeyState(VK_SHIFT)); //(muKeyStates & 0x1)?0x81:0;
	aKeyState[VK_LSHIFT] = HIBYTE(::GetKeyState(VK_LSHIFT)); //(muKeyStates & 0x2)?0x81:0;

	aKeyState[VK_CONTROL]  = HIBYTE(::GetKeyState(VK_CONTROL));
	aKeyState[VK_LCONTROL] = HIBYTE(::GetKeyState(VK_LCONTROL));

	aKeyState[VK_MENU]  = HIBYTE(::GetKeyState(VK_MENU));
	aKeyState[VK_LMENU] = HIBYTE(::GetKeyState(VK_LMENU));

  muKeyStates &= 0xFE | (aKeyState[VK_SHIFT]  ? 0x1 : 0x0);
  muKeyStates &= 0xFD | (aKeyState[VK_LSHIFT] ? 0x2 : 0x0);

  muKeyStates &= 0xFB | (aKeyState[VK_CONTROL]  ? 0x4 : 0x0);
  muKeyStates &= 0xF7 | (aKeyState[VK_LCONTROL] ? 0x8 : 0x0);

  muKeyStates &= 0xEF | (aKeyState[VK_MENU]  ? 0x10 : 0x0);
  muKeyStates &= 0xDF | (aKeyState[VK_LMENU] ? 0x20 : 0x0);

	WORD awChar[4]; awChar[0] = awChar[1] = awChar[2] = awChar[3] = 0;
	int iRet = ToAscii(muKeyCode,
			uScanCode,
			aKeyState,
			awChar,
			0
			);
	muAscii = 0;
	if(iRet > 0)
		muAscii = awChar[0];
	WCHAR awcUnicode[4]; awcUnicode[0] = awcUnicode[1] = awcUnicode[2] = 0;
	iRet = ToUnicode(muKeyCode,
			  uScanCode,
			  aKeyState,
			  awcUnicode,
			  4,
			  0
			  );
	muUnicode = 0;
	if(iRet > 0)
		muUnicode = awcUnicode[0];

	mbAlt = m_isAlt;
	mbShift = m_isShift;
	mbCtrl = m_isCtrl;
}

// проверить, нажата ли указанная клавиша на клавиатуре?
bool CWS3D::IsKeyDown(unsigned short muKeyCode, unsigned short muScanCode){
	BYTE sKeyState = HIBYTE(::GetKeyState(muKeyCode));
	return (sKeyState != 0);
}

// показать диалоговое окно с сообщением
int CWS3D::ShowMessageBox(const wchar_t* apwcMessage, const wchar_t* apwcTitle){
	CComString sMessage = cLPCSTR(apwcMessage);
	CComString sTitle;
	if(apwcTitle != NULL)
		sTitle = cLPCSTR(apwcTitle);

	//MessageBox( m_hParentWnd, sMessage.GetString(), sTitle.GetString(), MB_OK | MB_ICONINFORMATION );
	return 1;
}

HRESULT CWS3D::ToggleFullScreen(UINT auOn)
{
	// TODO: Add your implementation code here
	bFullScreen=(auOn==1)?TRUE:FALSE;
	return S_OK;
}

bool IsUnsIntNumber(const wchar_t* apwcStr){
	for(; *apwcStr != L'\0'; apwcStr++){
		if((*apwcStr < L'0') || (*apwcStr > L'9'))
			return false;
	}
	return true;
}

unsigned long CWS3D::loadModule(BSTR sModuleID, BSTR sScene)
{
	if(!mpIResourceManager) return E_FAIL;

	// TODO: Add your implementation code here
	if(pOMSContext.get() != NULL && pOMSContext->mpResM != NULL){
		pOMSContext->mpResM->SetModuleID(sModuleID);
		pOMSContext->mpSM->LoadModuleMeta();
		if(sScene == NULL || *sScene == '\0'){
			loadSceneN(0);
		}else{
			int iSceneNum = -1;
			if(IsUnsIntNumber(sScene)){
				iSceneNum = 0;
				for(const wchar_t* pwc = sScene; *pwc != L'\0'; pwc++){
					iSceneNum = iSceneNum * 10 + (*pwc - '0');
				}
				loadSceneN(iSceneNum-1);
			}else{
				loadScene(sScene);
			}
		}
	}
	return S_OK;
}

void CWS3D::Tick()
{
	if(pOMSContext.get()!=NULL && pOMSContext->mpSM){
		rmml::mlEModes mlMode=pOMSContext->mpSM->GetMode();
		if(mlMode != mSMLastMode){
			if(mSMLastMode==rmml::smmLoading){
				mSMLastMode=mlMode;
				//Fire_OnSceneLoaded();
			}
			mSMLastMode=mlMode;
		}
	}

	if (nLogInitialized == liLogInitialized)
	{
		nLogInitialized = liLogInitHandled;
		//Fire_OnLogInitialized();
	}

	if( pOMSContext->mpRM &&
		pOMSContext->mpRM->GetExtendedRM())
	{
		pOMSContext->mpRM->GetExtendedRM()->UpdateAndRender();
	}

	if(pOMSContext->mpInfoMan != NULL)
		pOMSContext->mpInfoMan->Update();
	if( pOMSContext->mpComMan)
	{
		pOMSContext->mpComMan->Update();
	}
}

bool CWS3D::SetScreenMode(DWORD adwWidth, DWORD adwHeight, DWORD adwBPP, BOOL abFullscreen)
{
	ATLASSERT(pOMSContext->mpRM);
	rm::rmIRenderManagerAX*	pRMExt = pOMSContext->mpRM->GetExtendedRM();
	ATLASSERT(pRMExt);
	return pRMExt->SetScreenMode(adwWidth,adwHeight,adwBPP,abFullscreen!=0);
}

void CWS3D::SetScreenModeForOculus(DWORD adwWidth, DWORD adwHeight, DWORD x, DWORD y)
{
	ATLASSERT(pOMSContext->mpRM);
	rm::rmIRenderManagerAX*	pRMExt = pOMSContext->mpRM->GetExtendedRM();
	ATLASSERT(pRMExt);
	pRMExt->SetScreenModeForOculus(adwWidth,adwHeight, x, y);
}

bool CWS3D::SetArguments(const wchar_t* apwcArgs){
	if ( apwcArgs == NULL)
		return false;
	wchar_t* pwcArgs = MP_NEW_ARR( wchar_t, wcslen( apwcArgs)+1);
	wcscpy_s(pwcArgs, wcslen( apwcArgs)+1, apwcArgs);
	mvArguments.push_back( pwcArgs);

	return true;
}

/*HRESULT CWS3D::OnActivate( BOOL bActivated)
{
	sWrapper.UpdateAllSound( bActivated);
	return 0;
}*/

CComString	CWS3D::GetRegKeyValueStr( LPCTSTR lpKey)
{
	HKEY	hKey;
	USES_CONVERSION;
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, W2A(msRegKey.c_str()), 0, KEY_READ, &hKey)==ERROR_SUCCESS && hKey!=0)
	{
		CComString value;
		DWORD dwType = REG_SZ;
		DWORD dwSize;
		LONG lRes = RegQueryValueEx( hKey, lpKey, 0, &dwType, NULL, &dwSize);
		if( dwSize > 0)
		{
			lRes = RegQueryValueEx( hKey, lpKey, 0, &dwType, (BYTE*)value.GetBufferSetLength( dwSize), &dwSize);
			RegCloseKey( hKey);
			if (lRes == ERROR_SUCCESS)
			{
				return value;
			}
		}
		RegCloseKey( hKey);
	}
	return "";
}

CComString	CWS3D::GetValueStrFromIni( LPCTSTR lpSection, LPCTSTR lpKey)
{
	USES_CONVERSION;

	if (pOMSContext.get())
	{
		if (pOMSContext->mpLogWriter)
			pOMSContext->mpLogWriter->WriteLn("GetValueStrFromIni section ", lpSection, " key ", lpKey, " fullPath ", W2A(msPathIni.c_str()));
	}

	CWComString sValue;
	if(GetPrivateProfileStringW( A2W( lpSection), A2W( lpKey), L"", sValue.GetBuffer(MAX_PATH+1), MAX_PATH, msPathIni.c_str()))
	{
		if (pOMSContext.get())
		{
			if (pOMSContext->mpLogWriter)
				pOMSContext->mpLogWriter->WriteLn("success value ", sValue.GetBuffer());
		}
		CComString resultStr(sValue.GetString());
		return resultStr;
	}
	int errorCode = GetLastError();
	if (pOMSContext.get())
			{
				if (pOMSContext->mpLogWriter)
					pOMSContext->mpLogWriter->WriteLn("failed errorCode ", errorCode);
			}
	return "";
}

unsigned long CWS3D::getSettingValue(BSTR sName, BSTR* psVal)
{
	wchar_t pwcValue[1024] = L"";
	if( GetSetting( CComBSTR(sName),  pwcValue, sizeof(pwcValue)/sizeof(pwcValue[0])))
	{
		CComString sValue( pwcValue);
		*psVal = sValue.AllocSysString();
		return S_OK;
	}
	return E_FAIL;
}

/************************************************************************/
/* Version:  0.0.0.0 [releaseVersion.maxVersion.minVersion.build]
//
Обновление клиента необходимо если на сервере releaseVersion больше текущей версии releaseVersion на клиенте,
или при совпадении releaseVersion на сервере maxVersion больше текущей версии maxVersion на клиенте.
В случае различия minVersion и build обновление идет по желанию.
/************************************************************************/
wstring	CWS3D::GetRegVersion()
{
	if(!mbVersionAlreadyGot)
	{
		/*msVersion = GetRegKeyValueStr("version");
		if(msVersion.IsEmpty())
		{*/
			msVersion = GetValueStrFromIni("settings", "version");
		//}
		mbVersionAlreadyGot = TRUE;
	}
	USES_CONVERSION;
	return wstring( A2W(msVersion.GetBuffer()));
}

void CWS3D::GetToolbarParams(int& toolbarOffsetX, int& toolbarOffsetY, int& toolbarWidth, int& toolbarHeight)
{
	int offsetX, desktopWidth, desktopHeight;
	GetDesktopSize(offsetX, desktopWidth, desktopHeight);

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

	toolbarOffsetX = mi.rcWork.left - offsetX;
	toolbarOffsetY = mi.rcWork.top;
    
	int clientHeight = mi.rcWork.bottom - mi.rcWork.top;
	int clientWidth = mi.rcWork.right - mi.rcWork.left;

	if (clientWidth != desktopWidth)
	{
		toolbarWidth = desktopWidth - clientWidth;
		toolbarHeight = 0;
	}
	else if (clientHeight != desktopHeight)
	{
		toolbarWidth = 0;
		toolbarHeight = desktopHeight - clientHeight;
	}
	else
	{
		toolbarWidth = 0;
		toolbarHeight = 0;
	}
}

void CWS3D::GetDisplayParamsIfNeeded()
{
	if (m_isDisplayParamsInitialized)
	{
		return;
	}

	m_isDisplayParamsInitialized = true;

	DEVMODE dm;
	std::string displayName = "\\\\.\\Display ";
	for (int displayID = 1; displayID <= 3; displayID++)
	{		
		displayName[displayName.size() - 1] = '0' + displayID;
		ZeroMemory(&dm, sizeof(dm));

		EnumDisplaySettings(displayName.c_str(), ENUM_CURRENT_SETTINGS, &dm);		
		m_displayParams.push_back(dm);
	}

	ZeroMemory(&m_defaultDM, sizeof(m_defaultDM));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &m_defaultDM);
}

void CWS3D::GetWideDesktopParams(int& width, int& height, int& startOffset)
{
	GetDisplayParamsIfNeeded();

	int minX = 1000000;
	int maxX = -minX;
	height = 0;

	MP_VECTOR<DEVMODE>::iterator it = m_displayParams.begin();
	MP_VECTOR<DEVMODE>::iterator itEnd = m_displayParams.end();
	
	for ( ; it != itEnd; it++)
	{
		if (0 == height)
		{
			height = (*it).dmPelsHeight;
		}
		if ((*it).dmPosition.x < minX)
		{
			minX = (*it).dmPosition.x;
		}
		int _maxX = (*it).dmPosition.x + (*it).dmPelsWidth;
		if (_maxX > maxX)
		{
			maxX = _maxX;
		}
	}

	startOffset = minX;
	width = maxX - minX;
}

void CWS3D::GetDesktopSize(int& offsetX, int& width, int& height)
{
	// 1. берем абсциссу окошка	 
	::GetWindowRect(m_FrameWorkWnd, &m_FrameWorkRECT);
	int windowX = m_FrameWorkRECT.left;
	// 1.1. смещаем абсциссу, т.к. у окошка есть бордюр
	windowX += 15;

	// 2. ищем по асбциссе монитор, на котором находится окошко
	// считаем, что в системе не будет более трех мониторов
	DEVMODE dm;
	DEVMODE dmMinX = m_defaultDM;					
	bool isDisplayFound = false;

	GetDisplayParamsIfNeeded();

	MP_VECTOR<DEVMODE>::iterator it = m_displayParams.begin();
	MP_VECTOR<DEVMODE>::iterator itEnd = m_displayParams.end();
	
	for ( ; it != itEnd; it++)
	{
		if ((windowX >= (*it).dmPosition.x) && (windowX < ((*it).dmPosition.x + (int)(*it).dmPelsWidth)))
		{
			isDisplayFound = true;
			dm = *it;
			break;
		}		

		if ((*it).dmPosition.x < dmMinX.dmPosition.x)
		{
			dmMinX = *it;
		}
	}

	// 3. Если не нашли (?), то возьмем параметры основного монитора
	if (!isDisplayFound)
	{
		if (m_displayParams.size() > 1)
		{
			if (windowX < dmMinX.dmPosition.x) 
			{
				dm = dmMinX;
			}
			else
			{
				dm = m_defaultDM;
			}
		}
		else
		{
			dm = m_defaultDM;
		}
	}


	// 4. возвращаем параметры найденного монитора
	width = dm.dmPelsWidth;
	height = dm.dmPelsHeight;
	offsetX = dm.dmPosition.x;
}

wstring	CWS3D::GetRegLanguage()
{
	if(!mbLangAlreadyGot)
	{
		/*msLanguage = GetRegKeyValueStr("language");
		if(msLanguage.IsEmpty())
		{*/
			msLanguage = GetValueStrFromIni("settings", "language");
		//}
		mbLangAlreadyGot = TRUE;
	}
	USES_CONVERSION;
	return wstring( A2W(msLanguage.GetBuffer()));
}

HMODULE LoadDLL( LPCWSTR alpcRelPath, LPCWSTR alpcName)
{
	CWComString sModuleDirectory;
	GetModuleFileNameW( NULL, sModuleDirectory.GetBufferSetLength(MAX_PATH), MAX_PATH);

	CWComString sCurrentDirectory;
	GetCurrentDirectoryW( MAX_PATH, sCurrentDirectory.GetBufferSetLength(MAX_PATH));

	SECFileSystem fs;

	/*if (gbCave)
	{
		CComString path =sCurrentDirectory;
		path+= "\\";
		path+=alpcRelPath;
		SetCurrentDirectory(path);
	}
	else*/
	SetCurrentDirectoryW( fs.GetPath(sModuleDirectory) + alpcRelPath);

	HMODULE handle = ::LoadLibraryW( alpcName);
	SetCurrentDirectoryW( sCurrentDirectory);
	return handle;
}

void CWS3D::CreateLogManagersImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;
	ATLASSERT( !context->mpLogWriter);
	if( context->mpLogWriter)
		return;

	ATLASSERT( !m_pLogWriter);
	m_pLogWriter = MP_NEW( CLogWriter);
	context->mpLogWriter = m_pLogWriter;
	context->mpLogWriter->SetApplicationDataDirectory( GetApplicationDataDirectory().c_str());
	
	ATLASSERT( !context->mpLogWriterLastMsgAsync);
	if( context->mpLogWriterLastMsgAsync)
		return;
	ATLASSERT( !m_pLogWriterLastMsgAsync);
	m_pLogWriterLastMsgAsync = MP_NEW( CLogWriterLastMsgAsync);
	context->mpLogWriterLastMsgAsync = m_pLogWriterLastMsgAsync;
	context->mpLogWriterLastMsgAsync->SetApplicationDataDirectory( GetApplicationDataDirectory().c_str());
	USES_CONVERSION;	
	
	m_pDumpParamsSaver->set( L"scriptLog", (WCHAR*)context->mpLogWriterLastMsgAsync->GetLogFilePath().c_str());	

	CreateLoggerImpl(pOMSContext.get());

	if (context->mpLogger!=NULL)
		context->mpLogger->SetMode(true);

	pOMSContext->mpLogWriter->StartSendingToServer( (lgs::ILoggerBase*)pOMSContext->mpLogger);
	pOMSContext->mpTracer->StartSendingToServer( (lgs::ILoggerBase*)pOMSContext->mpLogger);
}

void CWS3D::DestroyLogManagersImpl( oms::omsContext* context)
{
	context->mpLogWriter = NULL;
	MP_DELETE( m_pLogWriter);
	context->mpLogWriterLastMsgAsync = NULL;
	MP_DELETE( m_pLogWriterLastMsgAsync);
}

void CWS3D::CreateVoipManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;
	ATLASSERT( !context->mpVoipMan);
	if( context->mpVoipMan)
		return;
	//voip::CreateVoipManagerClient( context);
#ifdef DEBUG
	#define	VOIP_MANAGER_DLL	L"VoipManClient.dll"
#else
	#define	VOIP_MANAGER_DLL	L"VoipManClient.dll"
#endif

	if( !hVoipModule)
		hVoipModule = ::LoadDLL( L"", VOIP_MANAGER_DLL);

	if( hVoipModule)
	{
		omsresult (*lpCreateVoipManagerClient)( oms::omsContext* aContext);
		(FARPROC&)lpCreateVoipManagerClient = (FARPROC)GetProcAddress( (unsigned long)hVoipModule, "CreateVoipManagerClient");
		if( lpCreateVoipManagerClient)
		{
			lpCreateVoipManagerClient( context);
		}
		else
		{
			HandleOnManagerCreateFunctionNotFound( VOIP_MANAGER_DLL, "CreateVoipManagerClient");
		}

		if( !context->mpVoipMan)
		{
			HandleOnManagerCreateFunctionFailed( VOIP_MANAGER_DLL, "CreateVoipManagerClient");

			FreeLibrary((unsigned long)hVoipModule);
			hVoipModule = 0;
		}
		else
		{
			HandleOnManagerModuleInitialized( VOIP_MANAGER_DLL);
		}
	}
	else
	{
		HandleOnManagerModuleNotLoaded( VOIP_MANAGER_DLL);
	}
}

void	CWS3D::HandleOnManagerModuleInitialized( const  wchar_t* alpcModuleName)
{
	if( alpcModuleName == NULL)
		return;

	omsContext* context = pOMSContext.get();
	if( context == NULL)
		return;

	if( context->mpLogWriter == NULL)
		return;

	unsigned int auErrorCode = GetLastError();
	context->mpLogWriter->WriteLn("Module ", alpcModuleName, " initialized properly");
}

void	CWS3D::HandleOnManagerModuleNotLoaded( const  wchar_t* alpcModuleName)
{
	if( alpcModuleName == NULL)
		return;

	omsContext* context = pOMSContext.get();
	if( context == NULL)
		return;

	if( context->mpLogWriter == NULL)
		return;

	unsigned int auErrorCode = GetLastError();
	context->mpLogWriter->WriteLn("Module ", alpcModuleName, " loading failed. GetLastError code is ", auErrorCode);
}

void	CWS3D::HandleOnManagerCreateFunctionNotFound( const  wchar_t* alpcModuleName, const char* alpcFuncitonName)
{
	if( alpcModuleName == NULL)
		return;

	if( alpcFuncitonName == NULL)
		return;

	omsContext* context = pOMSContext.get();
	if( context == NULL)
		return;

	if( context->mpLogWriter == NULL)
		return;

	unsigned int auErrorCode = GetLastError();
	context->mpLogWriter->WriteLn("Module ", alpcModuleName, " create function '", alpcFuncitonName, "'not found. GetLastError code is ", auErrorCode);
}

void	CWS3D::HandleOnManagerCreateFunctionFailed( const wchar_t* alpcModuleName, const char* alpcFuncitonName)
{
	if( alpcModuleName == NULL)
		return;

	if( alpcFuncitonName == NULL)
		return;

	omsContext* context = pOMSContext.get();
	if( context == NULL)
		return;

	if( context->mpLogWriter == NULL)
		return;

	unsigned int auErrorCode = GetLastError();
	context->mpLogWriter->WriteLn("Module ", alpcModuleName, " create function '", alpcFuncitonName, "'failed to create module. GetLastError code is ", auErrorCode);
}

void CWS3D::DestroyVoipManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;

	if( !hVoipModule)
		return;

	omsresult (*lpDestroyVoipManager)( oms::omsContext* aContext);
	(FARPROC&)lpDestroyVoipManager = (FARPROC)GetProcAddress( (unsigned long)hVoipModule, "DestroyVoipManagerClient");
	if( lpDestroyVoipManager)
		lpDestroyVoipManager( context);
	FreeLibrary( (unsigned long)hVoipModule);
	//voip::DestroyVoipManagerClient( context);
}

void CWS3D::CreateDesktopManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;
	ATLASSERT( !context->mpDesktopMan);
	if( context->mpDesktopMan)
		return;

#ifdef DEBUG
#define	DESKTOP_MANAGER_DLL	L"DesktopMan.dll"
#else
#define	DESKTOP_MANAGER_DLL	L"DesktopMan.dll"
#endif

	if( !hDesktopModule)
		hDesktopModule = ::LoadDLL( L"", DESKTOP_MANAGER_DLL);

	if( hDesktopModule)
	{
		omsresult (*lpCreateDesktopManager)( oms::omsContext* aContext);
		(FARPROC&)lpCreateDesktopManager = (FARPROC)GetProcAddress( (unsigned long)hDesktopModule, "CreateDesktopManager");
		if( lpCreateDesktopManager)
			lpCreateDesktopManager( context);

		omsresult (*lpCreateSharingManager)( oms::omsContext* aContext);
		(FARPROC&)lpCreateSharingManager = (FARPROC)GetProcAddress( (unsigned long)hDesktopModule, "CreateSharingManager");
		if( lpCreateSharingManager)
			lpCreateSharingManager( context);

		if( !context->mpDesktopMan && !context->mpSharingMan)
		{
			FreeLibrary((unsigned long)hDesktopModule);
			hDesktopModule = 0;
		}
	}
}

void CWS3D::DestroyDesktopManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;

	if( !hDesktopModule)
		return;

	omsresult (*lpDestroyDesktopManager)( oms::omsContext* aContext);
	(FARPROC&)lpDestroyDesktopManager = (FARPROC)GetProcAddress( (unsigned long)hDesktopModule, "DestroyDesktopManager");
	if( lpDestroyDesktopManager)
		lpDestroyDesktopManager( context);

	omsresult (*lpDestroySharingManager)( oms::omsContext* aContext);
	(FARPROC&)lpDestroySharingManager = (FARPROC)GetProcAddress( (unsigned long)hDesktopModule, "DestroySharingManager");
	if( lpDestroySharingManager)
		lpDestroySharingManager( context);

	FreeLibrary( (unsigned long)hDesktopModule);
}

void CWS3D::CreateSoundManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if ( !context)
		return;
	ATLASSERT( !context->mpSndM);
	if ( context->mpSndM)
		return;

#ifdef DEBUG
#define	SOUNMAN_MANAGER_DLL	L"SoundMan.dll"
#else
#define	SOUNMAN_MANAGER_DLL	L"SoundMan.dll"
#endif

	if ( !hSoundModule)
		hSoundModule = ::LoadDLL( L"", SOUNMAN_MANAGER_DLL);

	if ( hSoundModule)
	{
		omsresult (*lpCreateSoundManager)( oms::omsContext* aContext);
		(FARPROC&)lpCreateSoundManager = (FARPROC) GetProcAddress( (unsigned long) hSoundModule, "CreateSoundManager");
		if ( lpCreateSoundManager)
			lpCreateSoundManager( context);
		if ( !context->mpSndM)
		{
			FreeLibrary( (unsigned long) hSoundModule);
			hSoundModule = 0;
		}
		else
			setSoundManager( context->mpSndM);
	}
}

void CWS3D::DestroySoundManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;

	if( !hSoundModule)
		return;

	void (*lpDestroySoundManager)( oms::omsContext* aContext);
	(FARPROC&)lpDestroySoundManager = (FARPROC)GetProcAddress( (unsigned long)hSoundModule, "DestroySoundManager");
	if( lpDestroySoundManager)
		lpDestroySoundManager( context);
	FreeLibrary( (unsigned long)hSoundModule);
}

void CWS3D::CreateComManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if ( !context)
		return;
	ATLASSERT( !context->mpComMan);
	if ( context->mpComMan)
		return;

#ifdef DEBUG
#define	COMMAN_MANAGER_DLL	L"ComMan.dll"
#else
#define	COMMAN_MANAGER_DLL	L"ComMan.dll"
#endif

	if ( !hComModule)
		hComModule = ::LoadDLL( L"", COMMAN_MANAGER_DLL);

	if ( hComModule)
	{
		omsresult (*lpCreateComManager)( oms::omsContext* aContext);
		(FARPROC&)lpCreateComManager = (FARPROC) GetProcAddress( (unsigned long) hComModule, "CreateComManager");
		if ( lpCreateComManager)
			lpCreateComManager( context);
		if ( !context->mpComMan)
		{
			FreeLibrary( (unsigned long) hComModule);
			hComModule = 0;
		}
	}
}

void CWS3D::DestroyComManagerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;

	if( !hComModule)
		return;

	void (*lpDestroyComManager)( oms::omsContext* aContext);
	(FARPROC&)lpDestroyComManager = (FARPROC)GetProcAddress( (unsigned long)hComModule, "DestroyComManager");
	if( lpDestroyComManager)
		lpDestroyComManager( context);
	FreeLibrary( (unsigned long)hComModule);
}


void CWS3D::CreateLoggerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if ( !context)
		return;
	ATLASSERT( !context->mpLogger);
	if ( context->mpLogger)
		return;

#ifdef DEBUG
#define	LOGGER_DLL	L"Logger.dll"
#else
#define	LOGGER_DLL	L"Logger.dll"
#endif

	if ( !hLoggerModule)
		hLoggerModule = ::LoadDLL( L"", LOGGER_DLL);

	if ( hLoggerModule)
	{
		omsresult (*lpCreateLogger)( oms::omsContext* aContext);
		(FARPROC&)lpCreateLogger = (FARPROC) GetProcAddress( (unsigned long) hLoggerModule, "CreateLogger");
		if ( lpCreateLogger)
			lpCreateLogger( context);
		if ( !context->mpLogger)
		{
			FreeLibrary( (unsigned long) hLoggerModule);
			hLoggerModule = 0;
		}		
	}
}

void CWS3D::DestroyLoggerImpl( oms::omsContext* context)
{
	ATLASSERT( context);
	if( !context)
		return;

	if( !hLoggerModule)
		return;

	void (*lpDestroyLogger)( oms::omsContext* aContext);
	(FARPROC&)lpDestroyLogger = (FARPROC)GetProcAddress( (unsigned long)hLoggerModule, "DestroyLogger");
	if( lpDestroyLogger)
		lpDestroyLogger( context);
	FreeLibrary( (unsigned long)hLoggerModule);
}

void CWS3D::OnLogout( int iCode)
{
	if(pOMSContext->mpMapMan != NULL)
		pOMSContext->mpMapMan->OnLogout( iCode);
	if(pOMSContext->mpRM != NULL)
		pOMSContext->mpRM->OnLogout( iCode);
	if(pOMSContext->mpSyncMan != NULL)
		pOMSContext->mpSyncMan->OnLogout( iCode);
}

// размер памяти, занятый программой
bool CWS3D::GetConsumedMemorySize(unsigned long& PageFaultCount
									, unsigned long& PeakWorkingSetSize
									, unsigned long& WorkingSetSize
									, unsigned long& QuotaPeakPagedPoolUsage
									, unsigned long& QuotaPagedPoolUsage
									, unsigned long& QuotaPeakNonPagedPoolUsage
									, unsigned long& QuotaNonPagedPoolUsage
									, unsigned long& PagefileUsage
									, unsigned long& PeakPagefileUsage)
{
	PROCESS_MEMORY_COUNTERS memCounters;
	if( GetProcessMemoryInfo( GetCurrentProcess(), &memCounters, sizeof(memCounters)))
	{
		PageFaultCount = memCounters.PageFaultCount;
		PeakWorkingSetSize = memCounters.PeakWorkingSetSize;
		WorkingSetSize = memCounters.WorkingSetSize;
		QuotaPeakPagedPoolUsage = memCounters.QuotaPeakPagedPoolUsage;
		QuotaPagedPoolUsage = memCounters.QuotaPagedPoolUsage;
		QuotaPeakNonPagedPoolUsage = memCounters.QuotaPeakNonPagedPoolUsage;
		QuotaNonPagedPoolUsage = memCounters.QuotaNonPagedPoolUsage;
		PagefileUsage = memCounters.PagefileUsage;
		PeakPagefileUsage = memCounters.PeakPagefileUsage;
		return true;
	}

	PageFaultCount = 0;
	PeakWorkingSetSize = 0;
	WorkingSetSize = 0;
	QuotaPeakPagedPoolUsage = 0;
	QuotaPagedPoolUsage = 0;
	QuotaPeakNonPagedPoolUsage = 0;
	QuotaNonPagedPoolUsage = 0;
	PagefileUsage = 0;
	PeakPagefileUsage = 0;
	return false;
}

// вызывается, когда на окно плеера перетащили файл
void CWS3D::OnFileDrop(LPCWSTR alpcFilePath)
{
	if(m_pLogWriter){
		m_pLogWriter->WriteLn("[CWS3D] OnFileDrop(",alpcFilePath,")");
	}

	if(alpcFilePath == NULL)
		return;
	USES_CONVERSION;
	if(pOMSContext->mpSM)
	{
		POINT pnt;
		GetCursorPos(&pnt);
		RECT rct, crct;		
		GetWindowRect(m_FrameWorkWnd, &rct);
		GetClientRect(m_FrameWorkWnd, &crct);
		int diff = (rct.bottom - rct.top) - (crct.bottom - crct.top);
		pnt.x -= rct.left;
		pnt.y -= rct.top + diff;
		pOMSContext->mpSM->OnWindowFileDrop( alpcFilePath, pnt.x, pnt.y);
	}
}

void CWS3D::onLoginFinished( long sessionID)
{
	this->sessionID = sessionID;

	unsigned short* a = (unsigned short*)&sessionID;
	SendMessage2Launcher( APP_SESSION, a[0]);
	SendMessage2Launcher( APP_SESSION_2, a[1]);

	//if (!pOMSContext->mpServiceWorldMan->IsDebuggerPresent())
	{
		// создаем файл, что запустились с номером сессии и логином

		std::wstring path = pOMSContext->mpLogWriter->GetApplicationDataDirectory()+ L"\\Vacademia\\LauncherLog";
		CreateDirectoryW(path.c_str(),NULL);

		DWORD dwProcessId = GetCurrentProcessId();
		wchar_t ff[30];
		_itow_s(dwProcessId, ff, 30, 10);

		path +=L"\\run_";
		path +=ff;
		path +=L".log";

		FILE* fl = NULL;
		int res = _wfopen_s(&fl, path.c_str(), L"w");

		if (fl && res==0)
		{
			std::wstring login = pOMSContext->mpComMan->GetClientStringID();

			int size = login.size() + 1;
			char* p = MP_NEW_ARR(char, size);
			size_t convertedCnt = 0;
			wcstombs_s(&convertedCnt, p, size, login.c_str(), size);
			p[size - 1] = '\0';

			fprintf( fl,"%s \n", ff);
			fprintf( fl,"%ld \n", sessionID);
			fprintf( fl,"%s \n", p);

			MP_DELETE_ARR(p);
			fclose(fl);
		}
	}	

	//SendMessage2Launcher( APP_SESSION, this->sessionID);
}

long CWS3D::GetSessionID()
{
	return sessionID;
}

//////////////////////////////////////////////////////////////////////////
// Code is written by Vladimir (2011.11.24)
//WS3DMAN_API int launchProcessID = 0;

void CWS3D::SendMessage2Launcher( int id, unsigned long message)
{
	ws3d::SendSignal2Launcher( ws3d::launchProcessID, id, message);
}

void CWS3D::HandleDebugBreakKeyboardRequest()
{
	if( m_isAllowedDebug)
		DebugBreak();
}

// end Vladimir (2011.11.24)

// Заходили под отладкой.
bool CWS3D::IsDebuggerPresent()
{
	HMODULE hDll = ::LoadLibrary( "kernel32.dll");
	bool (*IsDebuggerPresent)();
	(FARPROC&)IsDebuggerPresent = (FARPROC)GetProcAddress((unsigned long)hDll, "IsDebuggerPresent");
	return IsDebuggerPresent();
}

void CWS3D::HandleMakeDumpKeyboardRequest()
{
	if( m_isAllowedDebug) {
		if (IsDebuggerPresent())
			DebugBreak();
		else
			make_dump( ASSERT_MAKE_USER_ID);
	}
}

bool CWS3D::GetKeysShortcutsEnabled( )
{
	return m_isAllowedDebug;
}
//////////////////////////////////////////////////////////////////////////

void CWS3D::OnWindowMoving()
{
	m_windowMovingTime = timeGetTime();
}
