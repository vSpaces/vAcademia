// nrmPreview.cpp: implementation of the nrmMath3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmPreview.h"
//#include <MMSystem.h>
#include "nrmImage.h"
#include "texture.h"
#include "GlobalSingletonStorage.h"
#include "ServiceMan.h"
#include "MapManagers\Protocol\PreviewRecordStopQueryOut.h"
#include "MapManagers\Protocol\PreviewSetAudioTimeQueryOut.h"
#include "MapManagers\Protocol\SetSplashSrcQueryIn.h"
#include "MapManagers\Protocol\DeleteRecordsOut.h"
#include "MapManagers\Protocol\PreviewChatMessageOut.h"
#include "MapManagers\Protocol\PreviewEventOut.h"
#include "MapManagers\Protocol\PreviewStatisticOut.h"
#include "MapManagers\Protocol\SetPreviewDefaultImageOut.h"

#include "DataBuffer2.h"

_RAW_PREVIEW_ITEM::_RAW_PREVIEW_ITEM():
	MP_STRING_INIT(targetName),
	MP_STRING_INIT(cameraName)
{
	frameType = PFT_PREVIEW;
	recordID = frameTime = imageWidth = imageHeight = 0;
	channelsCount = dataSize = 0;
	imageData = NULL;
	isDirectChannelOrder = true;
}

_RAW_PREVIEW_ITEM::_RAW_PREVIEW_ITEM(const _RAW_PREVIEW_ITEM& other):
	MP_STRING_INIT(targetName),
	MP_STRING_INIT(cameraName)
{
	frameType = other.frameType;
	recordID = other.recordID;
	sessionID = other.sessionID;
	frameTime = other.frameTime;
	imageWidth = other.imageWidth;
	imageHeight = other.imageHeight;
	channelsCount = other.channelsCount;
	imageData = other.imageData;
	dataSize = other.dataSize;
	targetName = other.targetName;
	cameraName = other.cameraName;
	countScreenImage = other.countScreenImage;
	isDirectChannelOrder = other.isDirectChannelOrder;
}

_RECORD_SERVER_DESC::_RECORD_SERVER_DESC():
	MP_STRING_INIT(address),
	MP_STRING_INIT(httpHost),
	MP_STRING_INIT(storagePath)
{
}

nrmPreviewRecorder::nrmPreviewRecorder( oms::omsContext* apContext):
	MP_STRING_INIT(sPreviewHttpHost),
	MP_STRING_INIT(sStoragePath),
	MP_WSTRING_INIT(sLastPrintScreenFilePath),
	MP_STRING_INIT(startAudioFileName),
	MP_VECTOR_INIT(rawFrames),
	MP_MAP_INIT(mapSplashes),
	MP_MAP_INIT(mapServers)
{
	ATLASSERT( apContext);
	context = apContext;
	recordID = 0xFFFFFFFF;
	session = NULL;
	//serviceServer = NULL;
	isRecordingNow = false;
	addFrameEvent.Create("FramesEvent", TRUE);
	startAudioRecordID = 0xFFFFFFFF;
	sPreviewHttpHost = "";
	sStoragePath = "";
	bRecordingProgress = false;
	bStatisticRecordingProgress = true;
	startAudioTime = 0;
	recordDuration = 0;
	startRecordTime = GetTickCount();
	startSessionTime = GetTickCount();
}

nrmPreviewRecorder::~nrmPreviewRecorder()
{
	SignalStop();
	addFrameEvent.SetEvent();
	Stop();
	bRecordingProgress = false;
	csFrames.Lock();
	for (VecFramesIt it = rawFrames.begin(); it!=rawFrames.end(); it++)
	{
		RAW_PREVIEW_ITEM*	frameData = &(*it);
		MP_DELETE_ARR(frameData->imageData);
	}
	rawFrames.clear();
	csFrames.Unlock();
}

void nrmPreviewRecorder::GetPreviewCurrentRecordAddress(int aRecordID, wchar_t* &alpPreviewCurrentRecordAddress)
{
	char sRecordID[10];
//	_itoa(recordID, sRecordID, 10);
	_itoa(aRecordID, sRecordID, 10);
	if(sPreviewHttpHost == "")
		return;
	std::string sAddress = sPreviewHttpHost+"/"+sStoragePath+"/Preview"+sRecordID;
	
	StringReplace(sAddress, "\\/", "/");
	StringReplace(sAddress, "//", "/");	
	if(sAddress.find("http://", 0)==-1)
	{
		sAddress.insert(0, "http://");
		StringReplace(sAddress, "///", "//");
	}
	StringTrimRight(sAddress);	
	USES_CONVERSION;
	alpPreviewCurrentRecordAddress = wcsdup(A2W(sAddress.c_str()));
	/*alpPreviewCurrentRecordAddress = sPreviewServer.AllocSysString();*/
}


void nrmPreviewRecorder::SetPreviewServerAddress( unsigned int aiRecordID, char *lpServerAddress, unsigned int aiServerPort, char *lpPreviewHttpHost, char *lpStoragePath)
{
	ATLASSERT( lpServerAddress);
	ATLASSERT( lpPreviewHttpHost);
	ATLASSERT( lpStoragePath);
	if( !lpServerAddress)
		return;
	if( !lpPreviewHttpHost)
		return;
	if( !lpStoragePath)
		return;
	
	CComString	sLog;
	sLog.Format( "Preview server address was registered: %u, %s, %u\n", aiRecordID, lpServerAddress, aiServerPort);
	g->lw.WriteLn(sLog.GetBuffer());

	// все ок
	RegisterRecordServer( aiRecordID, lpServerAddress, aiServerPort, lpPreviewHttpHost, lpStoragePath);
}

/*void nrmPreviewRecorder::SetPreviewServersAddress( unsigned int aiRecordID, char *lpServerAddress, unsigned int aiServerPort, char *lpPreviewHttpHost, char *lpStoragePath)
{
}*/

bool nrmPreviewRecorder::SetPreviewServerAddressImpl( char *lpServerAddress, unsigned int aiServerPort)
{
	ATLASSERT( lpServerAddress);
	if( !lpServerAddress)
		return false;

	/*CComString	sLog;
	sLog.Format( "Preview server address was set: %s, %u\n", lpServerAddress, aiServerPort);
	g->lw.WriteLn(sLog.GetBuffer());*/

	if( serverInfo.sIP == (char*)lpServerAddress
		&& (unsigned int)serverInfo.tcpPort == aiServerPort
		&& session != NULL)
	{
		// все ок
		return false;
	}

	if( session != NULL)
	{
		context->mpComMan->DestroyServerSession( &session);
		session = NULL;
	}

	serverInfo.sIP = (char*)lpServerAddress;
	serverInfo.tcpPort = aiServerPort;

	if( session == NULL)
	{
		context->mpComMan->CreatePreviewServerSession( &session, &serverInfo);
		ATLASSERT( session);
	}
	session->SetMessageHandler( this);
	return true;
}

void nrmPreviewRecorder::RegisterRecordServer( unsigned int aiRecordID, LPCSTR alpServerAddress, unsigned int recordServerPort
											  , LPCSTR alpHttpHost, LPCSTR alpStoragePath)
{
	MapRecordsServersIt it = mapServers.find( aiRecordID);
	if( it == mapServers.end())
	{
		RECORD_SERVER_DESC desc;
		desc.address = alpServerAddress;
		desc.port = recordServerPort;
		desc.httpHost = alpHttpHost;
		desc.storagePath = alpStoragePath;
		mapServers.insert( MapRecordsServers::value_type( aiRecordID, desc));

		// Выставим эвент готовности кадров, как пришел сервер. 
		// Если есть кадры, которые ждут сервера, то они попробуют отправиться
		csFrames.Lock();
			if( rawFrames.size() != 0)
				addFrameEvent.SetEvent();
		csFrames.Unlock();
	}
}

bool nrmPreviewRecorder::DeleteRecord(unsigned int aRecordID)
{
	if( session)
	{
		csSession.Lock();
		if( SetRecordServerByRecordID( aRecordID))
		{
			// оставим сообщение от Stop
			MapManagerProtocol::PreviewRecordStopQueryOut	queryOut(aRecordID, 0);
			session->Send( ST_DeleteRecord, queryOut.GetData(), queryOut.GetDataSize());
		}
		csSession.Unlock();
		return true;
	}
	return false;
}

bool nrmPreviewRecorder::DeleteRecords( unsigned int* /*pRecIDs*/, unsigned int /*count*/)
{
	//context->mpServiceMan->SendPreviewServersRequest();
	/*if( session)
	{
		csSession.Lock();
		MapManagerProtocol::CDeleteRecordsOut deleteRecords( pRecIDs, count);
		session->Send( ST_DeleteRecords, deleteRecords.GetData(), deleteRecords.GetDataSize());
		csSession.Unlock();
		return true;
	}*/
	return false;
}

bool nrmPreviewRecorder::SetRecordServerByRecordID( unsigned int aiRecordID)
{
	MapRecordsServersIt it = mapServers.find( aiRecordID);
	if( it == mapServers.end())
		return false;
	
	RECORD_SERVER_DESC desc;
	desc.address = it->second.address;
	desc.port = it->second.port;
	desc.httpHost = it->second.httpHost;
	desc.storagePath = it->second.storagePath;

	sPreviewHttpHost = desc.httpHost;
	sStoragePath = desc.storagePath;

	if( SetPreviewServerAddressImpl( (char*)desc.address.c_str(), desc.port))
	{
		if( startAudioTime != 0 && startAudioRecordID == aiRecordID)
		{
			SendPreviewAudioStartTimeImpl( startAudioTime, startAudioFileName.c_str());
		}
	}

	return true;
}

bool nrmPreviewRecorder::SetRecordServerByFrame(RAW_PREVIEW_ITEM * frame)
{
	MapRecordsServersIt it;
	if (frame->frameType == PFT_STATISTIC)
	{
		it = mapServers.find( (unsigned int)(-2));
	}
	else
	{
		it = mapServers.find( frame->recordID);
	}
	if( it == mapServers.end())
		return false;

	RECORD_SERVER_DESC desc;
	desc.address = it->second.address;
	desc.port = it->second.port;
	desc.httpHost = it->second.httpHost;
	desc.storagePath = it->second.storagePath;

	sPreviewHttpHost = desc.httpHost;
	sStoragePath = desc.storagePath;

	if( SetPreviewServerAddressImpl( (char*)desc.address.c_str(), desc.port))
	{
		if( startAudioTime != 0 && startAudioRecordID == frame->recordID)
		{
			SendPreviewAudioStartTimeImpl( startAudioTime, startAudioFileName.c_str());
		}
	}
	return true;
}

unsigned int nrmPreviewRecorder::getStatisticsTime()
{
	unsigned long statisticsTime = GetTickCount() - startSessionTime;
	return (unsigned int)statisticsTime;
}

unsigned int nrmPreviewRecorder::getRecordingDuration()
{
	return recordDuration + (GetTickCount() - startRecordTime);
}

void nrmPreviewRecorder::StartRecording(unsigned int aRecordID, unsigned int aDuration)
{
	if( !context->mpServiceMan)
		return;

	firstFrame = true;

	startAudioTime = 0;
	startRecordTime = GetTickCount();
	recordDuration = aDuration;
	recordID = aRecordID;
	bRecordingProgress = true;

	// получить сервер
	CComString	sLog;
	sLog.Format( "Preview server request was send: recID = %u\n", aRecordID);
	g->lw.WriteLn(sLog.GetBuffer());

	context->mpServiceMan->SendPreviewServerRequest( aRecordID);
	if( !IsRunning())
		Start();
}

void nrmPreviewRecorder::AddEvent( wchar_t * alpcEvent, wchar_t * alpcCameraName)
{
	CComString	sLog;
	sLog.Format( "nrmPreviewRecorder::Add event received");
	g->lw.WriteLn(sLog.GetBuffer());
	if( !bRecordingProgress)
		return;

	SendPreviewEvent( alpcEvent, alpcCameraName);
}

void nrmPreviewRecorder::AddStatistic( wchar_t * alpcStatistic)
{
	CComString	sLog;
	sLog.Format( "nrmPreviewRecorder::Add statistic received");
	g->lw.WriteLn(sLog.GetBuffer());
	if( !bStatisticRecordingProgress)
		return;

	SendPreviewStatistic( alpcStatistic);
}

bool nrmPreviewRecorder::AddFrame( rmml::mlMedia* apObject, wchar_t * alpcCameraName, char countScreenImage)
{
	CComString	sLog;
	sLog.Format( "nrmPreviewRecorder::Add frame received");
	g->lw.WriteLn(sLog.GetBuffer());
	if( !apObject)
		return false;
	if( !bRecordingProgress)
		return false;
	sLog.Format( "nrmPreviewRecorder::recordingIsGoing");
	g->lw.WriteLn(sLog.GetBuffer());

	ATLASSERT( apObject->GetType() == MLMT_IMAGE);
	nrmImage*	image = (nrmImage*)apObject->GetSafeMO();
	if( !image)
		return false;
	if( !image->m_sprite)
		return false;

	RAW_PREVIEW_ITEM	frame;
	if( !FillRecordFrameData( frame, image, 90, alpcCameraName, getRecordingDuration(), true, countScreenImage))
	{
		return false;
	}
	AddFrameToQueue( frame);
	return true;
}

bool nrmPreviewRecorder::AddStatisticFrame( rmml::mlMedia* apObject)
{
	if( !IsRunning())
		Start();
	CComString	sLog;
	sLog.Format( "nrmPreviewRecorder::Add statistic frame received");
	g->lw.WriteLn(sLog.GetBuffer());
	if( !apObject)
		return false;

	ATLASSERT( apObject->GetType() == MLMT_IMAGE);
	nrmImage*	image = (nrmImage*)apObject->GetSafeMO();
	if( !image)
		return false;
	if( !image->m_sprite)
		return false;

	static unsigned int startTime = 0;
	if (0 == startTime)
	{
		startTime = g->ne.time;
	}
	
	RAW_PREVIEW_ITEM	frame;
	if( !FillStatisticFrameData( frame, image, 15, (int)((g->ne.time - startTime) / 1000), true))
	{
		return false;
	}
	AddFrameToQueue( frame);
	return true;
}

void nrmPreviewRecorder::AddChatMessage( int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled)
{
	CComString	sLog;
	sLog.Format( "nrmPreviewRecorder::Add frame received");
	g->lw.WriteLn(sLog.GetBuffer());
	if( wsLogin == NULL || wsText == NULL)
		return;
	if( !bRecordingProgress)
		return;
	sLog.Format( "nrmPreviewRecorder::recordingIsGoing the message chat...");
	g->lw.WriteLn(sLog.GetBuffer());

	SendPreviewChatMessage( aType, wsLogin, wsText, abCloudedEnabled);
}

void nrmPreviewRecorder::SendPreviewEvent( wchar_t *wsEvent, wchar_t *wsObjectName)
{
	if( session != NULL)
	{
		csSession.Lock();	// чтобы сесcия не поменялась
		SendPreviewEventImpl( wsEvent, wsObjectName);
		csSession.Unlock();
	}
}

void nrmPreviewRecorder::SendPreviewStatistic( wchar_t *wsStatistic)
{
	if( session != NULL)
	{
		csSession.Lock();	// чтобы сесcия не поменялась
		SendPreviewStatisticImpl( wsStatistic);
		csSession.Unlock();
	}
}

void nrmPreviewRecorder::SendPreviewEventImpl( wchar_t *wsEvent, wchar_t *wsObjectName)
{
	if( session != NULL)
	{
		PreviewEventOut queryOut( recordID, getRecordingDuration(), wsEvent, wsObjectName);
		session->Send( ST_SetAddEvent, queryOut.GetData(), queryOut.GetDataSize());
	}
}

void nrmPreviewRecorder::SendPreviewStatisticImpl(wchar_t *wsStatistic)
{
	if( session != NULL)
	{
		PreviewStatisticOut queryOut( recordID, getStatisticsTime(), wsStatistic);
		session->Send( ST_SetAddStatistic, queryOut.GetData(), queryOut.GetDataSize());
	}
}

void nrmPreviewRecorder::SendPreviewChatMessage( int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled)
{
	if( session != NULL)
	{
		csSession.Lock();	// чтобы сесcия не поменялась
		/*if( SetRecordServerByRecordID( recordID))
		{*/
		SendPreviewChatMessageImpl( aType, wsLogin, wsText, abCloudedEnabled);
		//}
		csSession.Unlock();
	}
}

void nrmPreviewRecorder::SendPreviewChatMessageImpl( int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled)
{
	if( session != NULL)
	{
		PreviewChatMessageOut queryOut( recordID, getRecordingDuration(), aType, wsLogin, wsText, abCloudedEnabled);
		session->Send( ST_SetAddChatMessage, queryOut.GetData(), queryOut.GetDataSize());
	}
}

void nrmPreviewRecorder::AddFrameToQueue(RAW_PREVIEW_ITEM& frameData)
{
	csFrames.Lock();
		if (rawFrames.size() > 9)
		{
			MP_DELETE_ARR(rawFrames.begin()->imageData);
			rawFrames.erase(rawFrames.begin());
		}
	
		rawFrames.push_back( frameData);
		addFrameEvent.SetEvent();
	csFrames.Unlock();
}

void nrmPreviewRecorder::StopRecording(unsigned int aRecordID)
{
	if( session)
	{
		csSession.Lock();
		if( SetRecordServerByRecordID( aRecordID))
		{
			MapManagerProtocol::PreviewRecordStopQueryOut	queryOut(aRecordID, getRecordingDuration());
			session->Send( ST_StopRecord, queryOut.GetData(), queryOut.GetDataSize());
		}
		csSession.Unlock();
	}
	// получить сервер
	CComString	sLog;
	sLog.Format( "Preview nrmPreviewRecorder::StopRecording(): recID = %u\n", aRecordID);
	g->lw.WriteLn(sLog.GetBuffer());

	if (aRecordID == recordID)
	{
		bRecordingProgress = false;
	}
}

/*
Посылка сообщений работает следующим образом:
1. При начале новой записи определяется сервер, на который будет производиться запись превью.
2. Каждый кадр помещается в конец очереди кадров
3. Из очереди берется последний кадр и для него определяется сервер, на который нужно послать кадр
4. Если связь с сервером установлена, то кадр посылается на сервер
	иначе он посылается в начало очереди кадров и осуществляется подключение к серверу
*/

bool nrmPreviewRecorder::GetNextFrame( RAW_PREVIEW_ITEM& frame)
{
	bool ret = false;
	csFrames.Lock();
	if( rawFrames.size() != 0)
	{
		VecFramesIt printScreenFrame = rawFrames.end();
		for( VecFramesIt it = rawFrames.begin(); it != rawFrames.end(); it++)
		{
			if( it->frameType == PFT_PRINTSCREEN)
			{
				printScreenFrame = it;
				break;
			}
		}
		if( printScreenFrame == rawFrames.end())
		{
			frame = rawFrames.back();
			rawFrames.pop_back();
		}
		else
		{
			frame = *printScreenFrame;
			rawFrames.erase( printScreenFrame);
		}
		ret = true;
	}
	if( rawFrames.size() == 0)
		addFrameEvent.ResetEvent();
	csFrames.Unlock();
	return ret;
}

void nrmPreviewRecorder::GetBackMissedServerFrame( RAW_PREVIEW_ITEM& frame)
{
	csFrames.Lock();
	rawFrames.insert(rawFrames.begin(), frame);
	csFrames.Unlock();
}

DWORD nrmPreviewRecorder::Run()
{
	CComString	recordServer;
//	unsigned	recordServerPort;

	while( !ShouldStop() )
	{
		//! Дождемся кадра
		addFrameEvent.WaitForEvent();

		//! Получим новый кадр
		RAW_PREVIEW_ITEM	frame;
		if( !GetNextFrame( frame))
		{
			CComString	sLog;
			sLog.Format( "nrmPreviewRecorder::Run() NO frames found, recID=%u", recordID);
			g->lw.WriteLn(sLog.GetBuffer());
			Sleep(10);
			continue;
		}

		csSession.Lock();	// чтобы сесиия не поменялась
		//! Если не найден сервер для этого кадра (что сцуко очень странно..) 
		//! то вернем кадр на место, но в начало очереди.
		//! Эта ситуация возникает когда ответ на запрос сервера для данной записи еще не пришел
		//! Переподключим сервер если надо
		//! Если подключиться не удалось, то ждем.. это сцуко страшная ошибка.. ее не исправить
		//! потому что часть кадров ушла в сеть и остаток можно только дописать на тот же самый сервер
		if( !SetRecordServerByFrame( &frame))
		{
			CComString	sLog;
			sLog.Format( "nrmPreviewRecorder::Run() NO GetRecordServer found, recID=%u", frame.recordID);
			g->lw.WriteLn(sLog.GetBuffer());

			//ATLASSERT( FALSE);
			GetBackMissedServerFrame( frame);
			//! нужно сделать запрос на следующий сервер
			if (context->mpServiceMan)
				context->mpServiceMan->SendPreviewServerRequest( (frame.frameType == PFT_STATISTIC) ? (unsigned int)(-2) : frame.recordID);
			csSession.Unlock();
			Sleep( 100);
			continue;
		}

		if( !session || session->GetStatus() == 0)
		{
			CComString	sLog;
			if (session->GetStatus() == 0)
			{
				//sLog.Format( "nrmPreviewRecorder::Run() session status  == NOT CONNECTED");
			}
			else if (frame.frameType == PFT_STATISTIC)
			{
				sLog.Format( "nrmPreviewRecorder::Run() NO Session found, client sessionID=-2");
			}
			else
			{
				sLog.Format( "nrmPreviewRecorder::Run() NO Session found, recID=%u", frame.recordID);
			}
			if (sLog.GetLength() > 0)
			{
				g->lw.WriteLn(sLog.GetBuffer());
			}

			//ATLASSERT( FALSE);
			GetBackMissedServerFrame( frame);
			//! нужно бы сделать запрос на следующий сервер
			csSession.Unlock();
			Sleep( 100);
			continue;
		}

		//! 0. resample frame
		// ??

		//! 1. pack frame to jpeg
		// ?? packed already because of Devil single thread nature

		//! 2. send frame to server
		ATLASSERT( session);
		SEND_HEADER header;
		header.recordID = frame.recordID;
		header.frameTime = frame.frameTime;
		header.imageWidth = frame.imageWidth;
		header.imageHeight = frame.imageHeight;
		header.imageFormat = FF_JPG;
		header.dataSize = frame.dataSize;

		if (firstFrame)
		{
			header.frameTime = 0;
			firstFrame = false;
		}

		if( frame.frameType == PFT_PREVIEW  || frame.frameType == PFT_STATISTIC)
		{
			CDataBuffer2	data;
			if (frame.frameType == PFT_STATISTIC)
			{
				header.sessionID = frame.sessionID;
			}
			else
			{
				data.addString( frame.cameraName);
				data.add(frame.countScreenImage);
			}

			unsigned int messageDataSize = sizeof( SEND_HEADER) + header.dataSize + data.getSize();

			byte*	temp = temp = MP_NEW_ARR(byte, messageDataSize);
			int offset = 0;
			memcpy( (void*)temp, (void*)&header, sizeof(header)); offset += sizeof(header);
			memcpy( (void*)((DWORD)temp + sizeof(header)), (void*)frame.imageData, header.dataSize); offset += header.dataSize;
			memcpy( (void*)((DWORD)temp + offset), (void*)data.getData(), data.getSize());	offset += data.getSize();
			session->Send( (frame.frameType == PFT_STATISTIC) ? ST_AddStatisticFrame : ST_AddFrame, temp, messageDataSize);
			MP_DELETE_ARR(temp);
		}
		else
		{
			ATLASSERT( FALSE);
			CDataBuffer2	data;
			data.addString( frame.targetName);

			unsigned int messageDataSize = data.getSize() + sizeof( SEND_HEADER) + header.dataSize;
			byte*	temp = MP_NEW_ARR(byte, messageDataSize);
			int offset = 0;
			memcpy( (void*)temp, (void*)data.getData(), data.getSize());	offset += data.getSize();
			memcpy( (void*)temp, (void*)&header, sizeof(header));	offset += sizeof(header);
			memcpy( (void*)((DWORD)temp + offset), (void*)frame.imageData, header.dataSize);
			session->Send( ST_AddPrintScreen, temp, messageDataSize);
			MP_DELETE_ARR(temp);
		}

		MP_DELETE_ARR(frame.imageData);
		csSession.Unlock();

	}
	return 0;
}

bool nrmPreviewRecorder::FillRecordFrameData(RAW_PREVIEW_ITEM& frameData, nrmImage* frameImage, unsigned int quality, wchar_t * alpwcCameraName, unsigned long time, bool aDeleteTexture, byte countScreenImage)
{
	ATLASSERT( frameData.imageData == NULL);
	frameData.frameTime = time;
	frameData.recordID = recordID;
	USES_CONVERSION;
	frameData.cameraName = W2A(alpwcCameraName);
	frameData.countScreenImage = countScreenImage;

	return FillFrameData(frameData, frameImage, quality, aDeleteTexture);
}

bool nrmPreviewRecorder::FillStatisticFrameData(RAW_PREVIEW_ITEM& frameData, nrmImage* frameImage, unsigned int quality, unsigned long time, bool aDeleteTexture)
{
	ATLASSERT( frameData.imageData == NULL);

	frameData.frameTime = time;
	frameData.sessionID = context->mpComMan->GetSessionID();
	frameData.frameType = PFT_STATISTIC;

	return FillFrameData(frameData, frameImage, quality, aDeleteTexture, false);
}

bool nrmPreviewRecorder::FillFrameData(RAW_PREVIEW_ITEM& frameData, nrmImage* frameImage, unsigned int quality, bool aDeleteTexture, bool isYInvert)
{
	ATLASSERT( frameData.imageData == NULL);

	int textureID = frameImage->m_sprite->GetTextureID(0);
	if (-1 == textureID)
	{
		return false;
	}

	CTexture* texture = g->tm.GetObjectPointer(textureID);
	if (!texture)
	{
		return false;
	}

	frameData.imageWidth = texture->GetTextureWidth();
	frameData.imageHeight = texture->GetTextureHeight();
	frameData.channelsCount = texture->IsTransparent() ? 4 : 3;

	void* pUncompressedFrameData = texture->GetRawData();
	if (texture->IsKeepInSystemMemory())
	{
		frameData.isDirectChannelOrder = texture->IsRawDirectChannelOrder();
		texture->SetRawData(NULL, 0, true);
	}
	if( !pUncompressedFrameData) 
		return false;

	// #378
	if( frameData.imageWidth != (unsigned int)texture->GetTextureRealWidth() 
		|| frameData.imageHeight != (unsigned int)texture->GetTextureRealHeight())
	{
		void* pTempFrameData = MP_NEW_ARR(unsigned char, frameData.imageWidth * frameData.imageHeight * frameData.channelsCount);
		int sourceStride = texture->GetTextureRealWidth() * frameData.channelsCount;
		int destStride = frameData.imageWidth * frameData.channelsCount;
		byte* psourceline = (byte*)pUncompressedFrameData;
		byte* pdestline = (byte*)pTempFrameData;
		// так как картинка перевернута
		if (isYInvert)
		{
			psourceline += (texture->GetTextureHeight() - 1) * sourceStride;
			pdestline += (frameData.imageHeight - 1) * destStride;
			for( unsigned int h=0; h<frameData.imageHeight; h++)
			{
				memcpy( pdestline, psourceline, destStride);
				psourceline -= sourceStride;
				pdestline -= destStride;
			}
		}
		else
		{
			// statistic frame
			psourceline += (texture->GetTextureRealHeight() - frameData.imageHeight) * sourceStride;
			for( unsigned int h=0; h<frameData.imageHeight; h++)
			{
				memcpy( pdestline, psourceline, destStride);
				psourceline += sourceStride;
				pdestline += destStride;
			}
		}
		unsigned char* tmp = (unsigned char*)pUncompressedFrameData;
		MP_DELETE_ARR(tmp);		
		pUncompressedFrameData = pTempFrameData;
	}
	// end of #378

	unsigned char* data = reinterpret_cast<unsigned char*>(pUncompressedFrameData);		

	if (frameData.channelsCount == 4)
	{
		__int64 uniqueColors[3];
		uniqueColors[1] = -1;
		uniqueColors[2] = -1;

		unsigned int imageSize = frameData.imageHeight * frameData.imageWidth;
		unsigned int offset = 0;
		unsigned int* _data = (unsigned int*)data;
		unsigned int pixel = *_data;
		unsigned int currentPixel;
		uniqueColors[0] = pixel;
		for ( ; offset < imageSize; ++offset)
		{
			currentPixel = *(_data + offset);
			if (pixel != currentPixel)   
			{
				if (uniqueColors[1] == -1)
				{			
					uniqueColors[1] = currentPixel;
					pixel = currentPixel;
				}
				else if (uniqueColors[2] == -1 && currentPixel != uniqueColors[0])
				{			
					uniqueColors[2] = currentPixel;
					break;
				}		
			}
		}
		if (uniqueColors[2] == -1)
		{
			CComString	sLog;
			if (isYInvert)
			{
				sLog.Format( "Preview picture has been disabled. Frame time =  %u\n", frameData.frameTime);
			}
			else
			{
				sLog.Format( "Statistic picture has been disabled. Frame time =  %u\n", frameData.frameTime);
			}
			g->lw.WriteLn(sLog.GetBuffer());

			if( pUncompressedFrameData)
				MP_DELETE_ARR(pUncompressedFrameData);
			return false;
		}
	}
	else if (frameData.channelsCount == 3)
	{
		bool notCorrect = true;

		unsigned char firstColor[3];

		unsigned int imageSize = frameData.imageHeight * frameData.imageWidth * 3;
		unsigned char* _data = (unsigned char*)data;
		memcpy(&firstColor[0], _data, 3);

		for (int i = 0; i < imageSize; i++)
		if (_data[i] != firstColor[i%3])
		{
			notCorrect = false;
			break;
		}

		if (notCorrect)
		{
			if( pUncompressedFrameData)
			{
				MP_DELETE_ARR(pUncompressedFrameData);
			}

			return false;
		}

		if (!frameData.isDirectChannelOrder)
		{
			for (int i = 0; i < imageSize; i += 3)			
			{
				unsigned char temp = _data[i];
				_data[i] = _data[i + 2];
				data[i + 2] = temp;				
			}
		}
	}
	//else
	//{
	//	assert(false);
	//}

	/*bool ret = */CompressImageData( frameData, frameData.imageWidth, frameData.imageHeight, frameData.channelsCount, pUncompressedFrameData, quality);	

 	MP_DELETE_ARR(pUncompressedFrameData);

	if (aDeleteTexture)
		g->tm.DeleteObject(texture->GetID());

	return true;
}
	
bool nrmPreviewRecorder::CompressImageData(RAW_PREVIEW_ITEM& frameData, int aWidth, int aHeight, int aChannels, void* lpUncompressedData, int quality)
{
	void* pCompressedFrameData = NULL;
	unsigned long compressedSize = 0;
	if( g->tm.CompressImageData( aWidth, aHeight, aChannels
		, lpUncompressedData, &pCompressedFrameData, &compressedSize, quality) && compressedSize != 0)
	{
		frameData.channelsCount = aChannels;
		frameData.imageWidth = aWidth;
		frameData.imageHeight = aHeight;
		frameData.imageData = (byte*)pCompressedFrameData;
		frameData.dataSize = compressedSize;
		return true;
	}
	return false;
}

bool nrmPreviewRecorder::StretchCompressedImageData(RAW_PREVIEW_ITEM& frameData, int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight, void* lpUncompressedData, int quality)
{
	void* pCompressedFrameData = NULL;
	unsigned long compressedSize = frameData.dataSize;
	if( g->tm.StretchCompressedImageData( srcX, srcY, srcWidth, srcHeight, destWidth, destHeight
		, lpUncompressedData, &pCompressedFrameData, &compressedSize, quality) && compressedSize != 0)
	{
		frameData.imageWidth = destWidth;
		frameData.imageHeight = destHeight;
		frameData.imageData = (byte*)pCompressedFrameData;
		frameData.dataSize = compressedSize;
		return true;
	}
	return false;
}

// установить кадр на объект
wchar_t* nrmPreviewRecorder::ApplyFrameToTarget( const char* lpcTarget, int bpp, int width, int height, const void* bits, int quality)
{
	RAW_PREVIEW_ITEM	frameData;
	frameData.frameType = PFT_PRINTSCREEN;
	frameData.targetName = lpcTarget;
	if( CompressImageData( frameData, width, height, bpp, (void*)bits, quality))
	{
		if( frameData.imageData)
		{
			// save to file..
			wchar_t szTempDir[ MAX_PATH];
			wchar_t tmpName[ MAX_PATH];

			::GetTempPathW( sizeof(szTempDir)/sizeof(szTempDir[0]), szTempDir);
			USES_CONVERSION;
			::GetTempFileNameW( szTempDir, A2W( lpcTarget), 0, tmpName);

			wcscat( tmpName, L".jpg");
			sLastPrintScreenFilePath = tmpName;

			FILE* fp = _wfopen( sLastPrintScreenFilePath.c_str(), L"wb");
			if( fp)
			{
				bool correct =  (fwrite( frameData.imageData, frameData.dataSize, 1, fp) == 1) ? true : false;
				fclose( fp);
				if( correct)
					return (wchar_t*)sLastPrintScreenFilePath.c_str();
				else
					return NULL;
			}
			else
			{
			}
		}
	}	
	return NULL;
}

// сохранить изображение во временное хранилище(н-р аватара)
wchar_t* nrmPreviewRecorder::AddImage( rmml::mlMedia* apObject, const char* lpcName, int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight)
{
	if( !apObject)
		return NULL;

	ATLASSERT( apObject->GetType() == MLMT_IMAGE);
	nrmImage*	frameImage = (nrmImage*)apObject->GetSafeMO();
	if( !frameImage)
		return NULL;
	if( !frameImage->m_sprite)
		return NULL;

	RAW_PREVIEW_ITEM	frameData;
	USES_CONVERSION;
	if( FillRecordFrameData( frameData, frameImage, 90, L"avatar_for_chat", 0, false, 0))
	{
		StretchCompressedImageData(frameData, srcX, srcY, srcWidth, srcHeight, destWidth, destHeight, frameData.imageData, 60);
		// save to file..
		wchar_t szTempDir[ MAX_PATH];
		wchar_t tmpName[ MAX_PATH];

		::GetTempPathW( sizeof(szTempDir)/sizeof(szTempDir[0]), szTempDir);
		::GetTempFileNameW( szTempDir, A2W(lpcName), 0, tmpName);

		wcscat( tmpName, L".jpg");
		sLastPrintScreenFilePath = tmpName;

		FILE* fp = _wfopen( sLastPrintScreenFilePath.c_str(), L"wb");
		if( fp)
		{
			bool correct =  (fwrite( frameData.imageData, frameData.dataSize, 1, fp) == 1) ? true : false;
			MP_DELETE_ARR(frameData.imageData);
			fclose( fp);
			if( correct)
				return (wchar_t*)sLastPrintScreenFilePath.c_str();
		}

		MP_DELETE_ARR(frameData.imageData);
	}
	return NULL;
}

// установить время начала проигрывания звука
void nrmPreviewRecorder::AddPreviewAudio( unsigned int auiRecordID, const char* alpcFileName)
{
	if( auiRecordID == recordID)
	{
		startAudioTime = getRecordingDuration();
		startAudioFileName = alpcFileName;
		startAudioRecordID = auiRecordID;

		SendPreviewAudioStartTime( startAudioTime, alpcFileName);
	}
}

// установить картинку по умолчанию из картинки alpcFileName
void nrmPreviewRecorder::SetPreviewDefaultImage( unsigned int auiRecordID, wchar_t *alpcFileName)
{
	if( session != NULL)
	{
		if( auiRecordID == 0)
			auiRecordID = recordID;	
		if(auiRecordID == 0)
			return;
		csSession.Lock();	
		SetPreviewDefaultImageOut queryOut( auiRecordID, alpcFileName);
		session->Send( ST_SetPreviewDefaultImage, queryOut.GetData(), queryOut.GetDataSize());
		csSession.Unlock();	
	}
}

void nrmPreviewRecorder::SendPreviewAudioStartTime( unsigned int /*auiAudioTime*/, const char* alpcFileName)
{
	if( session != NULL)
	{
		csSession.Lock();	// чтобы сесcия не поменялась
		if( SetRecordServerByRecordID( recordID))
		{
			SendPreviewAudioStartTimeImpl( startAudioTime, alpcFileName);
		}
		csSession.Unlock();
	}
}

void nrmPreviewRecorder::SendPreviewAudioStartTimeImpl( unsigned int /*auiAudioTime*/, const char* alpcFileName)
{
	PreviewSetAudioTimeQueryOut queryOut( recordID, startAudioTime, alpcFileName);
	session->Send( ST_SetPreviewAudioTime, queryOut.GetData(), queryOut.GetDataSize());
}

void nrmPreviewRecorder::OnSetSplashSrc( BYTE* aData, int aDataSize)
{
	CSetSplashSrcQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if(context->mpLogWriter!=NULL)
			context->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CGetQueryPackageIn");
		ATLASSERT( false);
		return;
	}

	ATLASSERT( context->mpServiceMan);
	if( context->mpServiceMan)
		context->mpServiceMan->SetSplashSrc( queryIn.GetRecordID(), queryIn.GetFileName().GetBuffer());

	if( session != NULL)
	{
		csSession.Lock();
		if( SetRecordServerByRecordID( queryIn.GetRecordID()))
		{
			// оставим сообщение от Stop
			MapManagerProtocol::PreviewRecordStopQueryOut	queryOut(queryIn.GetRecordID(), 0);
			session->Send( ST_SplashSrcReceived, queryOut.GetData(), queryOut.GetDataSize());
		}
		csSession.Unlock();
	}
}

void nrmPreviewRecorder::StartStatisticRecording(unsigned int recordId, unsigned int aDuration)
{
	recordID = recordId;
	startRecordTime = GetTickCount();
	recordDuration = aDuration;
	bStatisticRecordingProgress = true;
	context->mpServiceMan->SendPreviewServerRequest( recordId);
}

void nrmPreviewRecorder::StopStatisticRecording()
{
	bStatisticRecordingProgress = false;
}
