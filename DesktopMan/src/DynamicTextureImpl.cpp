#include "StdAfx.h"
#include "..\include\DynamicTextureImpl.h"
#include "..\..\VASEncoder\VASE.h"
#include "..\..\VASEncoder\vasebitmap.h"
#include "..\..\VASEncoder\VASEFramesDifPack.h"
#include "../include/MemLeaks.h"
#include <math.h>

#include <Mmsystem.h>

//for log
#include "..\..\Cscl3DWS\rm\rm.h"
#include "ilogwriter.h"

CDynamicTextureImpl::CDynamicTextureImpl(SHARING_CONTEXT* apContext):
	MP_VECTOR_INIT(frames)
{
	isDestroyed = false;
	isOneKeyReceived = false;
	keyFrameSize = CSize(256, 256);
	textureRequestSize = CSize(256, 256);
	backBufferBits = NULL;
	backBackBits = NULL;
	normalModeEnabled = false;
	csFrames.Init();
	csDestroy.Init();
	isChanged = true;
	isDestroyedInEngine = false;
	isDestroyedInDesktopMan = false;

	totalBytesReceived = 0;
	lastFrameBytesReceived = 0;
	updateFPS = 0;
	receivedFPS = 0;

	useNewCodec = false;
	isUnpackingNow = false;
	m_deleteResultGiven = false;
	context = apContext;
	textureLoader.InitNewCodec(context->gcontext->mpRM->GetTaskManager(), context->gcontext);

	destination = "";

	for (int k = 0; k < 4; k++)
	{
		m_userData[k] = NULL;
	}

	m_userData[1] = MP_NEW_ARR( char, 19);
	//strcpy_s((char *)m_userData[1], "wavelet_depack.xml");

	viewWindow = NULL;	
	m_countDrawFrames = 0;
	m_countRecvFrames = 0;
	m_timeFromLastRedraw = 0;
	m_timeFromLastRecvFrame = 0;

	m_textureHeigthBeforeResize = -1;
	m_textureWidthBeforeResize = -1;

	m_isFreezed = false;
	m_isNeedToSetKeyFrameSize = true;
	m_totalSizeFrame = 0;
	m_lostDestination = false;
}

void* 	CDynamicTextureImpl::GetUserData(unsigned int userDataID)
{
	if (userDataID < 4)
	{
		return m_userData[userDataID];
	}
	else
	{
		return NULL;
	}
}

void 	CDynamicTextureImpl::SetUserData(unsigned int userDataID, void* data)
{
	if (userDataID < 4)
	{
		m_userData[userDataID] = data;
	}
}

CDynamicTextureImpl::~CDynamicTextureImpl()
{
	// ??
	Clear();
}

void CDynamicTextureImpl::Clear()
{
	csFrames.Lock();
	CleanUpFrames();
	csFrames.Unlock();
	backBufferDC.DeleteDC();
	backBackBufferDC.DeleteDC();

	if (m_lostDestination)
	{//иначе удал€тс€ как подобъекты
		if (backBufferBitmap.m_hBitmap != NULL)
			backBufferBitmap.DeleteObject();
		if (backBackBitmap.m_hBitmap != NULL)
			backBackBitmap.DeleteObject();
		
		textureLoader.CleanUp();
	}
	csFrames.Term();
	csDestroy.Term();
}

void	CDynamicTextureImpl::AddVASEPack(VASEFramesDifPack* aPack, bool abKeyFrame)
{
	ATLASSERT( aPack);

	if((!abKeyFrame && !isOneKeyReceived) || !IsLive())
	{
		SAFE_DELETE( aPack);
		return;
	}

	if( abKeyFrame)
	{
		// если пришел ключевой кадр, то все остальные можно убрать
		csFrames.Lock();
		CleanUpFrames();
		csFrames.Unlock();
		isOneKeyReceived = true;
	}

	csFrames.Lock();
	VASE_FRAME	frame;
	frame.pack = aPack;
	frame.isKeyframe = abKeyFrame;
	frames.push_back( frame);
	isChanged = true;
	csFrames.Unlock();

	if (abKeyFrame)
	{
		if (context->gcontext->mpLogWriter)
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]Frame saved in dynamic texture "+destination);
	}
	m_countRecvFrames++;
	m_timeFromLastRecvFrame = timeGetTime();
}

// ќбновить состо€ние объекта (не текстуру);
void CDynamicTextureImpl::Update()
{
	// текстура добавлена в поток распаковки - он вызовет UpdateFrame
	isUnpackingNow = true;
}

// ќбновить текстуру
void CDynamicTextureImpl::UpdateFrame()
{
	bool backBufferChanged = false;

	// текстура еще не создана или удалена
	if (backBufferDC == 0 || !IsLive())
	{
		if (DestroyFromUnpacing())
			Release();
		return;
	}
	std::vector<VASE_FRAME>	framesCopy;
	
	csFrames.Lock();
		framesCopy.clear();
		for (unsigned int i=0; i < frames.size(); i++)
		{
			framesCopy.push_back(frames[i]);
		}
		frames.clear();
		isChanged = false;
	csFrames.Unlock();
	
	// скопируем кадры на текстуру
	VecFramesIt it = framesCopy.begin(), end = framesCopy.end();
	for (; it!=end; it++)
	{
		VASEFramesDifPack* pack = (*it).pack;
		if( it->isKeyframe)
		{
			if (context->gcontext->mpLogWriter)
				context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]UpdateFrame "+destination);
			OnNewKeyframeReceived( pack);
		}
		if( normalModeEnabled)
		{
			for (unsigned int i=0; i<pack->GetBitmapsCount(); i++)
			{
				VASEBitmap* bitmap = pack->GetBitmap( i);
				ATLASSERT( bitmap);

				const void* jpegData = NULL;
				unsigned int jpegDataSize = 0;
				bitmap->GetBuffer( &jpegData, &jpegDataSize);
				m_totalSizeFrame += jpegDataSize;
				if( NULL != jpegData)
				{
					if (useNewCodec)
					{
						if (textureLoader.LoadFromMemoryNewCodec((void*)jpegData, jpegDataSize, bitmap->GetSize(), pack->IsEasyPacked()))
						{
							CopyFrameToBackbuffer( textureLoader.GetWaveletsWidth(), textureLoader.GetWaveletsHeight()
							, textureLoader.GetWaveletsData(), textureLoader.GetWaveletsDataSize()
							, bitmap->GetPosition(), false);
						}
					}
					else
					{
						if( textureLoader.LoadFromMemory( (void*)jpegData, jpegDataSize))
						{
							CopyFrameToBackbuffer( textureLoader.GetWidth(), textureLoader.GetHeight()
													, textureLoader.GetData(), textureLoader.GetDataSize()
													, bitmap->GetPosition(), true);

						}
					}
					backBufferChanged = true;
				}
			}
		}
	}
	if (backBufferChanged)
	{
		if (backBufferSize.cx != keyFrameSize.cx || backBufferSize.cy != keyFrameSize.cy)
		{
			backBufferDC.SetStretchBltMode( HALFTONE);
			backBufferDC.StretchBlt( 0, 0, backBufferSize.cx, backBufferSize.cy, backBackBufferDC, 0, 0, keyFrameSize.cx, keyFrameSize.cy, SRCCOPY);
		}
		else
		{
			backBufferDC.SetStretchBltMode( COLORONCOLOR);
			backBufferDC.BitBlt( 0, 0, backBufferSize.cx, backBufferSize.cy, backBackBufferDC, 0, 0, SRCCOPY);
		}
	}
	while( !framesCopy.empty())
	{
		VASEFramesDifPack* frame = (*framesCopy.begin()).pack;
		SAFE_DELETE( frame);
		framesCopy.erase( framesCopy.begin());
	}
	// если движок и desktopman дали разрешение на удаление
	if (DestroyFromUnpacing())
		Release();
}

void CDynamicTextureImpl::CopyFrameToBackbuffer( unsigned int auWidth, unsigned int auHeight
												, void* lpData, unsigned int auSize
												, CPoint position,bool abReverseHMode)
{
	// create
	void* pBits = 0;
	HDC screenDC = GetDC(0);
	BITMAPINFO info;
	HBITMAP hbm = CreateDIBSection( auWidth, auHeight
									, 24, &pBits, info, abReverseHMode);
	if (pBits == NULL)
	{
		return;
	}
	memcpy( pBits, lpData, auSize);
//	::SetDIBits(screenDC, hbm, 0, auHeight, lpData, &info, DIB_RGB_COLORS);
	
	CDC hTempDC;
	hTempDC.CreateCompatibleDC( screenDC);
	HBITMAP hBitmapOld = hTempDC.SelectBitmap( hbm);

	backBackBufferDC.BitBlt(position.x, position.y, auWidth, auHeight, hTempDC, 0, 0, SRCCOPY);
/*	if (viewWindow)
	{
		BITMAP myBMP;
		CBitmap cbmp;
		cbmp = backBufferDC.GetCurrentBitmap();
		cbmp.GetBitmap(&myBMP);
	
		viewWindow->Update(myBMP.bmWidth, myBMP.bmHeight, myBMP.bmBits, auSize, abReverseHMode);
	}
*/
//	DrawStatistics( backBufferDC);

	//destroy
	hTempDC.SelectBitmap( hBitmapOld);
	hTempDC.DeleteDC();
	DeleteObject( hbm);
	ReleaseDC( 0, screenDC);
};

// ¬ывести статистику
void	CDynamicTextureImpl::DrawStatistics( CDC& cdc)
{
/*	CFont	font;
	font.CreateFont( 14, 14, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
	cdc.SelectFont( font);

	//cdc.DrawText();
	unsigned int	totalBytesReceived;
	// количество полученных на последнем кадре байт
	unsigned int	lastFrameBytesReceived;
	// fps обновлени€ текстуры
	float			updateFPS;
	// fps получени€ сетевых пакетов
	float			receivedFPS;*/
}

// ¬ернуть ссылку на массив с построчным содержимым кадра	
void* CDynamicTextureImpl::GetFrameData()
{
	m_countDrawFrames++;
	m_timeFromLastRedraw = timeGetTime();
	return backBufferBits;
}

// ¬ернуть ширину исходной картинки в пиксел€х
unsigned short CDynamicTextureImpl::GetSourceWidth()
{
	return (unsigned short)(keyFrameSize.cx);
}

// ¬ернуть высоту исходной картинки в пиксел€х
unsigned short CDynamicTextureImpl::GetSourceHeight()
{
	return (unsigned short)(keyFrameSize.cy);
}

// ¬ернуть ширину результирующей текстуры в пиксел€х
unsigned short CDynamicTextureImpl::GetTextureWidth()
{
	return (unsigned short)(textureRequestSize.cx);
}

// ¬ернуть высоту результирующей текстуры в пиксел€х
unsigned short CDynamicTextureImpl::GetTextureHeight()
{
	return (unsigned short)(textureRequestSize.cy);
}

//«апомнить размеры тектуры до их уменьшени€
void CDynamicTextureImpl::SetTextureSizeBeforeResize(int width, int height)
{
	m_textureWidthBeforeResize = width;
	m_textureHeigthBeforeResize = height;
	m_isNeedToSetKeyFrameSize = false;
}

//если уменьшали тек4стуру, вернуть реальные размеры
void CDynamicTextureImpl::ReturnRealSize()
{
	if ((m_textureHeigthBeforeResize!=-1) && (m_textureWidthBeforeResize!=-1))
	{
		SetTextureSize(m_textureWidthBeforeResize, m_textureHeigthBeforeResize);
		m_textureHeigthBeforeResize = -1;
		m_textureWidthBeforeResize = -1;
		m_isNeedToSetKeyFrameSize = true;
	}
}

// ”становить размер результирующей текстуры
void CDynamicTextureImpl::SetTextureSize(int width, int height)
{
	HDC hScreenDC = GetDC(0);
	BITMAPINFO bmpInfo;
	HBITMAP hbmBack = CreateDIBSection( width, height, 24, &backBufferBits, bmpInfo);
	if( INVALID_HANDLE_VALUE != hbmBack && hbmBack != NULL)
	{
		if( backBufferBitmap.m_hBitmap != 0)
		{
			CDC hTempDC;
			hTempDC.CreateCompatibleDC( hScreenDC);
			HBITMAP hBitmapOld = hTempDC.SelectBitmap( hbmBack);

			hTempDC.SetStretchBltMode( HALFTONE);
			
			hTempDC.StretchBlt( 0, 0, width, height
				, backBufferDC, 0, 0, backBufferSize.cx, backBufferSize.cy, SRCCOPY);
			
			hTempDC.SelectBitmap( hBitmapOld);
			hTempDC.DeleteDC();
		}
		backBufferBitmap.Attach( hbmBack);
		if (backBufferSize.cx != width && backBufferSize.cy != height && backBufferSize.cx == 0 && backBufferSize.cy == 0)
		{
			MakeWhiteBackBufferBitmap();
		}
		
		backBufferDC.DeleteDC();
		backBufferDC.CreateCompatibleDC( hScreenDC);
		backBufferDC.SelectBitmap( backBufferBitmap);
	}

	ReleaseDC(0, hScreenDC);
	backBufferSize = CSize( width, height);
	textureRequestSize = backBufferSize;
}
// —делать backBufferBitmap белым
void CDynamicTextureImpl::MakeWhiteBackBufferBitmap()
{
	BITMAP myBMP;
	backBufferBitmap.GetBitmap(&myBMP);
	void *bmp = myBMP.bmBits;
	int size = myBMP.bmWidthBytes*myBMP.bmHeight;
	for (int i=0; i<size; i++)
	{
		((BYTE*)bmp)[i] = 255;	
	}
	backBufferBitmap.SetBitmapBits(size, bmp);
}

// ¬ернуть количество цветовых каналов, 3 дл€ RGB, 4 дл€ RGBA
unsigned char CDynamicTextureImpl::GetColorChannelCount()
{
	return 3/*RGB*/;
}

// ѕор€док цветовых каналов пр€мой (RGB); или обратный (BGR);
bool CDynamicTextureImpl::IsDirectChannelOrder()
{
	return true;
}

// Ќужно ли обновл€ть текстуру (или сейчас еЄ содержимое не измен€етс€?);
bool CDynamicTextureImpl::IsNeedUpdate()
{
	return isChanged;
}

// ”ничтож себ€
// (требуетс€ дл€ того, чтобы nengine мог сам решить когда его безопасно удалить);
void CDynamicTextureImpl::Destroy()
{
	isDestroyed = true;
	IDesktopSharingDestination*	currentDestination = NULL;
	currentDestination = context->gcontext->mpRM->GetDesktopSharingDestination( destination.GetBuffer());
	if (currentDestination) {
		currentDestination->DynamicTextureDeleting(this);
	}
	else {
		m_lostDestination = true;
	}
	if (DestroyFromNengine())
		Release();
}
void CDynamicTextureImpl::DesktopManDestroy()
{
	isDestroyed = true;
	if (DestroyFromDesktopMan())
		Release();
}

void CDynamicTextureImpl::Release()
{
	if (m_lostDestination)
	{
		Clear();
		return;
	}
	MP_DELETE_THIS;
}
bool CDynamicTextureImpl::CanDelete()
{
	csDestroy.Lock();
	bool result = false;
	if (m_deleteResultGiven)
	{
		csDestroy.Unlock();
		return false;
	}
	if (isDestroyedInDesktopMan && isDestroyedInEngine && !isUnpackingNow)
	{
		m_deleteResultGiven = true;
		result = true;
	}
	csDestroy.Unlock();
	return result;
}

bool CDynamicTextureImpl::DestroyFromNengine()
{
	csDestroy.Lock();
	isDestroyedInEngine = true;
	csDestroy.Unlock();

	return CanDelete();
}

bool CDynamicTextureImpl::DestroyFromDesktopMan()
{
	csDestroy.Lock();
	isDestroyedInDesktopMan = true;
	csDestroy.Unlock();
	
	return CanDelete();
}

bool CDynamicTextureImpl::DestroyFromUnpacing()
{
	csDestroy.Lock();
	isUnpackingNow = false;
	csDestroy.Unlock();
	
	return CanDelete();
}

bool	CDynamicTextureImpl::IsLive()
{
	return !isDestroyed;
}

void	CDynamicTextureImpl::OnNewKeyframeReceived(VASEFramesDifPack* aPack)
{
	ATLASSERT( aPack);
	ATLASSERT( aPack->GetBitmapsCount() == 1);
	if( aPack->GetBitmapsCount() == 0)
		return;
    VASEBitmap* bitmap = aPack->GetBitmap( 0);
	
    
	if (keyFrameSize != bitmap->GetSize())
	{
		HDC hScreenDC = GetDC(0);
		if (m_isNeedToSetKeyFrameSize)
		{
			backBufferSize = bitmap->GetSize();

			BITMAPINFO bmpInfo;
			HBITMAP hbmBack = CreateDIBSection( backBufferSize.cx, backBufferSize.cy, 24, &backBufferBits, bmpInfo);
			if (hbmBack == NULL)
				return;
			backBufferBitmap.Attach( hbmBack);
			MakeWhiteBackBufferBitmap();
			backBufferDC.DeleteDC();
			backBufferDC.CreateCompatibleDC( hScreenDC);
			backBufferDC.SelectBitmap( backBufferBitmap);
		}
		
		//textureRequestSize = backBufferSize;

		SetKeyFrameSize( bitmap->GetSize());
		BITMAPINFO info;
		HBITMAP hbmBackBack = CreateDIBSection( keyFrameSize.cx, keyFrameSize.cy, 24, &backBackBits, info);
		if (hbmBackBack == NULL)
			return;
		backBackBitmap.Attach( hbmBackBack);
		backBackBufferDC.DeleteDC();
		backBackBufferDC.CreateCompatibleDC( hScreenDC);
		backBackBufferDC.SelectBitmap( backBackBitmap);

		ReleaseDC(0, hScreenDC);
	}
}

void	CDynamicTextureImpl::SetKeyFrameSize(CSize aSize)
{
	keyFrameSize = aSize;

	if (m_isNeedToSetKeyFrameSize)// когда уменьшили текстуру, делаем 1 раз
	{
		textureRequestSize = keyFrameSize;
	}
}

void CDynamicTextureImpl::CleanUpFrames()
{
	while( !frames.empty())
	{
		VASEFramesDifPack* frame = (*frames.begin()).pack;
		SAFE_DELETE( frame);
		frames.erase( frames.begin());
	}

	isChanged = false;
}

HBITMAP CDynamicTextureImpl::CreateDIBSection( unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, void** lppBits, BITMAPINFO &bmpInfo, bool abReverseHMode)
{
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = auWidth;
	bmpInfo.bmiHeader.biHeight = abReverseHMode ? auHeight : -((int)auHeight);
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = auBPP;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HDC screenDC = GetDC(0);
	HBITMAP hbm = ::CreateDIBSection(screenDC, &bmpInfo
								, DIB_RGB_COLORS,(void **)lppBits, NULL, 0);

	ReleaseDC( 0, screenDC);

	return hbm;
}

void	CDynamicTextureImpl::EnableNormalMode()
{
	normalModeEnabled = true;
}
void CDynamicTextureImpl::SetViewWindow(CRemoteDesktopWindow* window)
{
	viewWindow = window;
}
void CDynamicTextureImpl::SetTarget(LPCSTR alpcName)
{
	if( alpcName)
		destination = alpcName;
}

std::string CDynamicTextureImpl::GetTextureInfo()
{
	std::string str = "";
	char buf[100];
	str += "\r\n Texture Info: \r\n\r\n";
	str += "isDestroyedInEngine: ";
	_itoa_s(isDestroyedInEngine, (char*)buf, 100, 10);
	str += buf;
	str += "\r\n"; 
	str += "count recieved frames: ";
	_itoa_s(m_countRecvFrames, (char*)buf, 100, 10);
	str += buf;
	str += "\r\n"; 
	str += "count drawed frames: ";
	_itoa_s(m_countDrawFrames, (char*)buf, 100, 10);
	str += buf;
	str += "\r\n"; 
	str += "average size frame(don't show real for my own sharing) (Kb): ";
	if (m_countRecvFrames != 0)
	{
		_itoa_s(m_totalSizeFrame/(m_countRecvFrames*1024), (char*)buf, 100, 10);
		str += buf;
	}
	else
		str += "0";
	str += "\r\n"; 
	str += "time from last received frame: ";
	_itoa_s((timeGetTime() - m_timeFromLastRecvFrame), (char*)buf, 100, 10);
	str += buf;
	str += "\r\n"; 
	str += "time from last redraw: ";
	_itoa_s((timeGetTime() - m_timeFromLastRedraw), (char*)buf, 100, 10);
	str += buf;
	str += "\r\n"; 

	return str;
}

void CDynamicTextureImpl::SetFreezState(bool isFreezed)
{
	m_isFreezed = isFreezed;
}

bool CDynamicTextureImpl::IsFreezed()
{
	return m_isFreezed;

}

bool CDynamicTextureImpl::IsFirstFrameSetted()
{
	return m_countDrawFrames > 0;
}

void CDynamicTextureImpl::ShowLastFrame()
{
	if (viewWindow)
	{
		BITMAP myBMP;
		CBitmap cbmp;
		cbmp = backBufferDC.GetCurrentBitmap();
		cbmp.GetBitmap(&myBMP);

		viewWindow->Show();
		viewWindow->Update(myBMP.bmWidth, myBMP.bmHeight, myBMP.bmBits, myBMP.bmWidthBytes*myBMP.bmHeight, true);
		
	}

}