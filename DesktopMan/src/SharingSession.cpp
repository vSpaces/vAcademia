#include "StdAfx.h"
#include "..\include\SharingSession.h"
#include <mmsystem.h>
#include "../../VASEncoder/vase.h"
#include "../../VASEncoder/VASEBitmap.h"
#include "../../VASEncoder/VASEFramesDifPack.h"
#include "../protocol/FrameOut.h"
#include "../protocol/SessionStartOut.h"
#include "../Include/SharingViewSession.h"
#include ".././nengine/ncommon/ThreadAffinity.h"
#include "..\include\SharingMan.h"
#include <math.h>
#include "..\..\Cscl3DWS\rm\rm.h"
#include "../include/MemLeaks.h"

#define FPS_TO_DELAY(x) (x > 0) ? 1000 / x : 1000;
BOOL SaveToFile(LPCSTR fn, HBITMAP hBitmap);
HBITMAP CreateDIBSection(unsigned int auWidth, unsigned int auHeight, 
						 unsigned int auBPP, void** lppBits, bool abReverseHMode,  void* bmp);
bool SaveBitmapToFile(HBITMAP *pHBmp);


//#define CAPTURE_TIME_LOG

CSharingSession::CSharingSession( SHARING_CONTEXT* apContext) : CSharingSessionBase( apContext)
{
	desktopSelector = NULL;
	needKeyFrame = false;
	fps = 10;
	quality = 50;
	fpsSleepDelay = FPS_TO_DELAY(fps);
	previousFrame = NULL;
	encoder = MP_NEW( VASEEncoder);
	MP_NEW_V2( waveletEncoder, VASWaveletEncoder, apContext->gcontext->mpRM->GetTaskManager(), apContext->gcontext);
	viewSession = NULL;
	selectorChanged = false;
	awarenessWnd = NULL;
	encoder->SetEncodeQuality(quality);
	waveletEncoder->SetEncoderQuality(quality);
	useNewCodec = false;
	csViewSession.Init();
	csDestroy.Init();
	
	
	m_isDestroy = false;
	m_isThreadEnd = true;
	m_deleteResultGiven = false;
	m_isDeleted = false;

	m_createThreadID = GetCurrentThreadId();

	m_wmMessageHandler.SetContext(apContext);
}

CSharingSession::~CSharingSession()
{
	if (awarenessWnd)
	{
		awarenessWnd->DestroyWindow();
		SAFE_DELETE(awarenessWnd);
	}

	if (waveletEncoder != NULL)
		waveletEncoder->StopPacking();
	Stop(); // при живом потоке захвата деструктор может вызваться только при закрытии академии, ждем его завершения работы
	CleanUp();

	csViewSession.Term();
	csDestroy.Term();
}

void CSharingSession::SetUseNewCodec()
{
	useNewCodec = true;
}

void	CSharingSession::SetViewSession(CSharingViewSession* apSession)
{
	csViewSession.Lock();
	viewSession = apSession;
	csViewSession.Unlock();
}

//! Установить качество изображения (по сути параметр сжатия для JPEG)
void CSharingSession::SetQuality( unsigned int auQuality)
{
	quality = auQuality;
	encoder->SetEncodeQuality(quality);
	waveletEncoder->SetEncoderQuality(quality);
}

//! Установить максимальный FPS
void CSharingSession::SetMaxFPS( unsigned int auFPS)
{
	// ??
}

void CSharingSession::SetSelector( IDesktopSelector* apSelector)
{
	SAFE_RELEASE( desktopSelector);

	ATLASSERT( apSelector);
	desktopSelector = apSelector;
	selectorChanged = true;
	m_wmMessageHandler.SetSharingSelector(desktopSelector);
}

IDesktopSelector* CSharingSession::GetSelector()
{
	return desktopSelector;
}

void CSharingSession::NeedKeyFrame()
{
	needKeyFrame = true;
}

//! Подписка на события
void CSharingSession::Subscribe( IDesktopSharingSessionCallback* apCallback)
{
	CEventSource<IDesktopSharingSessionCallback>::Subscribe( apCallback);
}

void CSharingSession::Unsubscribe( IDesktopSharingSessionCallback* apCallback)
{
	CEventSource<IDesktopSharingSessionCallback>::Unsubscribe( apCallback);
}

void CSharingSession::OnSessionStarted()
{
	if( !desktopSelector)
		return;

	if( IsRunning())
		return;

	DWORD dwCode = 0;
	::GetExitCodeThread(m_hThread, &dwCode);

	SendStartSessionRequest();

	ViskoeThread::CThreadImpl<CSharingSession>::Start();
	m_isThreadEnd = false;
}

void CSharingSession::HandleMessage( BYTE aType, BYTE* aData, int aDataSize)
{
	switch( aType)
	{
	case RT_RemoteCommand:
		m_wmMessageHandler.OnRemoteCommandReceived(aData, aDataSize);
		break;
	default:
		ATLASSERT( false);
	}
}


DWORD GetCPUIndex()
{
	DWORD result;

	__asm
	{
		mov eax,1
		cpuid
		shr ebx,24
		and ebx,0xf
		mov result,ebx
	} 

	return result;
}

DWORD CSharingSession::Run()
{
	HANDLE hndl = GetCurrentThread();
	SetSecondaryThreadAffinity( hndl);
	unsigned int keyFrameCounter = 0;
	unsigned long currentTime = 0;
	unsigned long frameTime = 0;
	unsigned long sendTime = 0;
	unsigned long processTime = 0;
	unsigned int messageType = 0;
	unsigned long startTime = timeGetTime();
	unsigned int keyFrameSize = 0;
	unsigned int diffFrameSize = 0;

	bool wasDiffBetweenKeyFrames = false;
	int totalDiffSize = 0;
	void * lpMaskBits = NULL;
	int lpMaskBitSize = 0;
	int sizeMask = 0;

	bool isKeyFrame = 0;
	IDesktopFrame*	frame;

	int lastWidth = 0;
	int lastHeight = 0;
	
	while( !ShouldStop())
	{
		//  Ждем пока с сервера придет идентификатор сессии
		if( GetSessionID() == INCORRECT_SESSION_ID || !viewSession)
		{
			Sleep(fpsSleepDelay);
			continue;
		}

		if( selectorChanged)
		{
			SAFE_RELEASE( previousFrame);
			keyFrameCounter = 0;
			selectorChanged = false;
		}
		
		if (!desktopSelector->IsCaptureAvailable())
		{
			LPCTSTR targ = GetSessionName();
			context->sharingMan->SessionClosedEvent(targ);
		}

		currentTime = timeGetTime();
		sendTime = 0;
		frameTime = currentTime - startTime;
		desktopSelector->RecapFrame();
#ifdef CAPTURE_TIME_LOG
		int difftime = timeGetTime() - currentTime; 
		char buf[100];
		itoa(difftime,buf,10);
		if (context->gcontext->mpLogWriter)
		{
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]RecapFrame frame time = ");
			context->gcontext->mpLogWriter->WriteLnLPCSTR(buf);
		}
#endif

		isKeyFrame = 0;
		frame = desktopSelector->GetCurrentFrame();
		if( !frame)
		{
			Sleep( fpsSleepDelay);
			continue;
		}
		
		// проверяем на смену размеров захватываемого окна (др селекторы не меняют размер)
		if (lastWidth != frame->GetWidth() && lastHeight != frame->GetHeight())
			needKeyFrame = true;
		lastWidth = frame->GetWidth();
		lastHeight = frame->GetHeight();

		if (!useNewCodec) {
			sizeMask = (frame->GetWidth() * frame->GetHeight()) >> 1;
			if (sizeMask > lpMaskBitSize)
			{
				if (lpMaskBits != NULL)
					SAFE_DELETE_ARRAY(lpMaskBits);
				lpMaskBitSize = sizeMask;
				lpMaskBits = MP_NEW_ARR( BYTE, lpMaskBitSize);
			}
		}

		VASEFramesDifPack*	pack = MP_NEW( VASEFramesDifPack);
		VASEFramesDifPack*	packEasyEncode = MP_NEW( VASEFramesDifPack);
		packEasyEncode->SetEasyPacked();

		if( !previousFrame || needKeyFrame)
		{
			//??
			if (needKeyFrame && previousFrame)
			{
				SAFE_RELEASE( previousFrame);
				keyFrameCounter = 0;
			}
			
			VASEBitmap*	bitmap = pack->AddBitmap();
			VASEBitmap*	easyEncodedBitmap = packEasyEncode->AddBitmap();
#ifdef CAPTURE_TIME_LOG
			int time = timeGetTime();
#endif
			if (useNewCodec)
			{
				if( !waveletEncoder->EncodeDIBBitmap(frame->GetData(), frame->GetWidth(), frame->GetHeight(), 32, bitmap, easyEncodedBitmap))
				{
					SAFE_DELETE( pack);
					SAFE_DELETE( packEasyEncode);
				}
			}
			else
			{
				if( !encoder->EncodeDIBBitmap( frame->GetData(), frame->GetWidth(), frame->GetHeight(), 32, bitmap, lpMaskBits))
				{
					SAFE_DELETE( pack);
					SAFE_DELETE( packEasyEncode);
				}
			}
#ifdef CAPTURE_TIME_LOG
			int difftime = timeGetTime() - time; 
			char buf[100];
			itoa(difftime,buf,10);
			if (context->gcontext->mpLogWriter)
			{
					context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]capture key frame time = ");
				context->gcontext->mpLogWriter->WriteLnLPCSTR(buf);
			}
#endif
			diffFrameSize = 0;
			messageType = ST_KeyFrameOut;
			isKeyFrame = true;

			needKeyFrame = false;
			wasDiffBetweenKeyFrames = false;
		}
		else
		{
#ifdef CAPTURE_TIME_LOG
			int time = timeGetTime();
#endif
			if (useNewCodec)
			{
				if( !waveletEncoder->EncodeDifference( previousFrame->GetData(), previousFrame->GetWidth(), previousFrame->GetHeight(), 32
					, frame->GetData(), frame->GetWidth(), frame->GetHeight(), 32, pack, packEasyEncode))
				{
					diffFrameSize = 0;
					SAFE_DELETE( pack);
					SAFE_DELETE( packEasyEncode);
				}
				else
					wasDiffBetweenKeyFrames = true;
			}
			else 
			{
				if( !encoder->EncodeDifference( previousFrame->GetData(), previousFrame->GetWidth(), previousFrame->GetHeight(), 32
												, frame->GetData(), frame->GetWidth(), frame->GetHeight(), 32, pack, lpMaskBits))
				{
					diffFrameSize = 0;
					SAFE_DELETE( pack);
					SAFE_DELETE( packEasyEncode);
				}
				else
					wasDiffBetweenKeyFrames = true;
			}
#ifdef CAPTURE_TIME_LOG
			int difftime = timeGetTime() - time; 
			if (pack)
			{
				char buf[100];
				itoa(difftime,buf,10);
				if (context->gcontext->mpLogWriter)
				{
					context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]capture diff frame time = ");
					context->gcontext->mpLogWriter->WriteLnLPCSTR(buf);	
				}
			}
#endif
			messageType = ST_FrameOut;
			isKeyFrame = false;
		}
		// send data to server
		if( pack)
		{
			SharingManProtocol::CFrameOut	frameOut( GetSessionID(), frameTime, pack, isKeyFrame, useNewCodec);
			//ATLTRACE("Sended data size: %u\n", frameOut.GetDataSize());

#ifdef CAPTURE_TIME_LOG
			itoa(frameOut.GetDataSize(),buf,10);
			if (context->gcontext->mpLogWriter)
			{
				context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]send frameOut.GetDataSize() = ");
				context->gcontext->mpLogWriter->WriteLnLPCSTR(buf);	
			}
#endif

			if( isKeyFrame)
			{
				keyFrameSize = frameOut.GetDataSize();
				totalDiffSize = 0;
			}
			else
				diffFrameSize = frameOut.GetDataSize();

			totalDiffSize += diffFrameSize;

			csViewSession.Lock();				
			if( viewSession)
			{
				viewSession->UpdateTextureSize(frame->GetWidth(), frame->GetHeight());
				viewSession->useNewCodec = useNewCodec;
				//для безопасности при многопоточности
				
				if (useNewCodec)
				{
					SAFE_DELETE(pack);
					if (!ShouldStop())
						context->sharingMan->AddVASEPack(packEasyEncode, isKeyFrame, viewSession);
				}
				else
				{
					SAFE_DELETE(packEasyEncode);
					if (!ShouldStop())
						context->sharingMan->AddVASEPack(pack, isKeyFrame, viewSession);
				}
			}
			else
			{
				SAFE_DELETE( pack);
				SAFE_DELETE( packEasyEncode);
			}
			csViewSession.Unlock();

			if (ShouldStop())
				continue;
			
			unsigned long startSendTime = timeGetTime();
			while (!ShouldStop())
			{
				if (SendWithWait( isKeyFrame ? ST_KeyFrameOut : ST_FrameOut, frameOut.GetData(), frameOut.GetDataSize()))
					break;
				else {
					Sleep(5 + (rand()*20)/RAND_MAX);
				}
			}
			sendTime = timeGetTime() - startSendTime;
		}

		keyFrameCounter++;
#ifdef CAPTURE_TIME_LOG
		itoa(sendTime,buf,10);
		if (context->gcontext->mpLogWriter)
		{
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]sendTime frame time = ");
			context->gcontext->mpLogWriter->WriteLnLPCSTR(buf);	
		}
#endif
		SAFE_RELEASE( previousFrame);
		// избегаем при остановке потока sleep-а
		if (ShouldStop())
			continue;

		if( keyFrameCounter >= 25)
		{
			if (!wasDiffBetweenKeyFrames && keyFrameCounter <= 36000*fps) // нет изменений кадр не отправляем(раз в 10ч)
				previousFrame = frame->Clone();
			else if (totalDiffSize/keyFrameCounter < 0.7*keyFrameSize && keyFrameCounter <= 30*fps)
			{ 
				//средний размер дифов меньше 70% размера ключевого - раз в 30сек обновляем ключевой 
				//чтоб на сервере не хранить слишком много кадров
				if (diffFrameSize > 0.8*keyFrameSize)
					keyFrameCounter += 10*fps;			
				previousFrame = frame->Clone();
			}
			else
				keyFrameCounter = 0;
		}
		else
		{
			previousFrame = frame->Clone();
		}
		processTime = timeGetTime() - currentTime;
#ifdef CAPTURE_TIME_LOG
		itoa(processTime,buf,10);
		if (context->gcontext->mpLogWriter)
		{
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]total frame time = ");
			context->gcontext->mpLogWriter->WriteLnLPCSTR(buf);	
		}
#endif
		Sleep( processTime < fpsSleepDelay ? (fpsSleepDelay - processTime) : 0.1*(processTime-sendTime) < 500 ? processTime-sendTime : 500);
		// кадр не укладывается в фпс - все равно делаем Sleep, чтобы дать др потокам поработать	
}
	SAFE_DELETE_ARRAY(lpMaskBits);
	ThreadEnded();
	return 0;
}

void CSharingSession::OnSessionStopped()
{
//	waveletEncoder->StopPacking();
//	Stop();	// CThread<CSharingSession>::Stop()
	SignalStop();
}

// реализация ISharingConnectionEvents
void CSharingSession::OnConnectLost()
{
	SetSessionID( INCORRECT_SESSION_ID);
	SetViewSession(NULL);
}

void CSharingSession::OnConnectRestored()
{
	if (!m_isDestroy)
		SendStartSessionRequest();
}

void CSharingSession::SendStartSessionRequest()
{
	ATLASSERT( GetSessionName() != "");
	if (!IsFileSharingSession())
	{
		SharingManProtocol::CSessionStartOut	queryOut( GetSessionName(), GetRemoteAccessMode());
		SendQuery(ST_StartSession, queryOut.GetData(), queryOut.GetDataSize());
	}
	/*else if (GetFileName() != NULL)
	{
		SharingManProtocol::CSessionStartOut	queryOut( GetUnicSessionName(), GetFileName(), GetRecordID(), IsPaused(), GetSeekFileTime());
		SendQuery(ST_StartFilePlay, queryOut.GetData(), queryOut.GetDataSize());	
	}*/
}

void CSharingSession::SendTerminateSessionRequest()
{
	ATLASSERT( GetSessionName() != "");
	SharingManProtocol::CSessionStartOut	queryOut( GetSessionName(), false);
	Send(ST_StopSession, queryOut.GetData(), queryOut.GetDataSize());
}

void CSharingSession::ShowAwarenessWnd()
{
	if (!desktopSelector->CanShowCaptureRect() || m_isDestroy)
		return;
	if (!awarenessWnd)
		MP_NEW_V( awarenessWnd, CRectAwarenessWnd, desktopSelector);
	awarenessWnd->ShowAwarenessWindow();
}

void CSharingSession::HideAwarenessWnd()
{
	if (awarenessWnd)
	{
		awarenessWnd->HideAwarenessWindow();		
	}
}
bool CSharingSession::CanDelete(bool aWillBeDeleteAfterThis)
{
	bool result = false;
	csDestroy.Lock();
//создаваться должен главным потоком им же пусть удаляется - 
//тк awarenessWnd - окно создается главным потоком удалить можно только в том же потоке что оно было создано
// aWillBeDeleteAfterThis = true по умолчанию   false - если мы хотим узнать можно ли удалять из не главного потока 
//(!!!) удалять должен главный поток
	if (aWillBeDeleteAfterThis && m_createThreadID != GetCurrentThreadId())
		return false;
	//другой поток уже получил результат, что можно удалять, пусть он и удалит
	if (m_deleteResultGiven)
	{
		csDestroy.Unlock();
		return false;
	}
	if (m_isThreadEnd && m_isDestroy)
	{
		m_deleteResultGiven = true;
		result = true;
	}
	csDestroy.Unlock();
	return result;
}

bool CSharingSession::Destroy()
{
	SignalStop();


	if (awarenessWnd)
	{
		awarenessWnd->HideAwarenessWindow();		
	}

	csDestroy.Lock();
	m_isDestroy = true;
	csDestroy.Unlock();

	if (CanDelete())
	{
		MP_DELETE_THIS;
		return true;
	}
	return false;
}
void CSharingSession::ThreadEnded()
{
	csDestroy.Lock();
	m_isThreadEnd = true;
	csDestroy.Unlock();

	if (CanDelete(false))
	{
//		CleanUp();
		m_isDeleted = true;
	}
}

void CSharingSession::CleanUp()
{
	if (context->gcontext->mpLogWriter)
		context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]SS deleting "+CComString(GetSessionName()));
	m_wmMessageHandler.SetSharingSelector(NULL);
	SAFE_RELEASE( desktopSelector);
	SAFE_DELETE( encoder);
	SAFE_DELETE( waveletEncoder);
	SAFE_RELEASE( previousFrame);
}

bool CSharingSession::IsLive()
{
	return !m_isDeleted;
}

void CSharingSession::SetRemoteControlMode(int aFullControl)
{
	m_wmMessageHandler.SetRemoteControlMode(aFullControl);
}
//////////////////////////////////////////////////////////////////////////

/*
using namespace Gdiplus;

HBITMAP CreateDIBSection(unsigned int auWidth, unsigned int auHeight, 
									  unsigned int auBPP, void** lppBits, bool abReverseHMode,  void* bmp)
{
	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = auWidth;
	bmpInfo.bmiHeader.biHeight = abReverseHMode ? auHeight : -auHeight;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = auBPP;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HDC screenDC = ::GetDC(0);
	HBITMAP hbm = ::CreateDIBSection(screenDC, &bmpInfo
		, DIB_RGB_COLORS,(void **)lppBits, NULL, 0);
	SetDIBits(screenDC, hbm, 0, auHeight, bmp, &bmpInfo, DIB_RGB_COLORS);
	::ReleaseDC( 0, screenDC);

	return hbm;
}

int _GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}

bool SaveBitmapToFile(HBITMAP *pHBmp)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	HPALETTE pal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	
	Bitmap bmp(*pHBmp, pal);
	CLSID   encoderClsid;
	_GetEncoderClsid(L"image/bmp", &encoderClsid);
	Status stat = bmp.Save(L"C:\\result.bmp", &encoderClsid, NULL);
	return true;
}

void Test3()
{
	// Создаем главное окно 
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
	
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Bitmap bmp1(L"c:\\bmp1.bmp");
	Bitmap bmp2(L"c:\\bmp2.bmp");

	HBITMAP hBmp1;
	HBITMAP hBmp2;

	bmp1.GetHBITMAP(Color::Black, &hBmp1);
	bmp2.GetHBITMAP(Color::Black, &hBmp2);

	IDesktopFrame*	frame1 = new CDesktopFrame(hBmp1);
	IDesktopFrame*	frame2 = new CDesktopFrame(hBmp2);

    void * lpMaskBits = NULL;
	int lpMaskBitSize = 0;
	int sizeMask = (frame1->GetWidth() * frame1->GetHeight()) >> 1;
	
	if (sizeMask > lpMaskBitSize)
	{
		if (lpMaskBits != NULL)
			free(lpMaskBits);
		lpMaskBitSize = sizeMask;
		lpMaskBits = malloc(lpMaskBitSize);
	}
	VASEEncoder* encoder = new VASEEncoder();

	VASEFramesDifPack*	pack = new VASEFramesDifPack();
	VASEBitmap*	bitmap = pack->AddBitmap();
	if( !encoder->EncodeDIBBitmap( frame1->GetData(), frame1->GetWidth(), 
		frame1->GetHeight(), 32, bitmap, lpMaskBits))
	{
		pack->DeleteBitmap(bitmap);
		SAFE_DELETE(pack);
	}

	VASEBitmap*	bitmap2 = pack->AddBitmap();
	if( !encoder->EncodeDIBBitmap( frame2->GetData(), frame2->GetWidth(), 
		frame2->GetHeight(), 32, bitmap2, lpMaskBits))
	{
		pack->DeleteBitmap(bitmap2);
		SAFE_DELETE(pack);
	}

	if( !encoder->EncodeDifference(frame1->GetData(), frame1->GetWidth(), frame1->GetHeight(), 
		32, frame2->GetData(), frame2->GetWidth(), frame2->GetHeight(), 32, pack, lpMaskBits))
	{
		SAFE_DELETE( pack);
	}

	int count = pack->GetBitmapsCount();
	VASEBitmap* result = pack->GetBitmap(count-1);
	
	const void* jpegData = NULL;
	unsigned int jpegDataSize = 0;

	result->GetBuffer( &jpegData, &jpegDataSize);
	
	CTextureLoader  textureLoader;
	textureLoader.LoadFromMemory( (void*)jpegData, jpegDataSize);
	void* pBits = 0;
	HBITMAP hBitmap = CreateDIBSection (textureLoader.GetWidth(), textureLoader.GetHeight()
		, 24, &pBits, false);
	memcpy( pBits, textureLoader.GetData(), textureLoader.GetDataSize());
	
	SaveBitmapToFile(&hBitmap);

	int nRet = theLoop.Run();
	_Module.RemoveMessageLoop();
}*/