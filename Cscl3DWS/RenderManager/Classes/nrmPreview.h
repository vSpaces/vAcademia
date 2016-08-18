// nrmPreview.h: interface for the nrmMath3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NRMPREVIEW_INCLUDED)
#define NRMPREVIEW_INCLUDED

#include "../CommonRenderManagerHeader.h"

#define	ST_AddFrame					1
#define	ST_StopRecord				2
#define	ST_DeleteRecord				3
#define	ST_AddPrintScreen			4
#define	ST_SetPreviewAudioTime		5
#define	ST_SplashSrcReceived		6
#define	ST_DeleteRecords			7
#define	ST_SetAddChatMessage		8
#define	ST_SetPreviewDefaultImage	9
#define	ST_SetAddEvent				10
#define	ST_SetAddStatistic		11
#define	ST_AddStatisticFrame		15

#include "iclientsession.h"
#include "serverInfo.h"
#include "imessagehandler.h"
#include <map>
//#include "iclientsession.h"

class nrmImage;

//! supported frame formats
#define	FF_JPG	1

typedef struct _RECORD_SERVER_DESC
{
	MP_STRING address;
	unsigned int port;
	MP_STRING httpHost;
	MP_STRING storagePath;

	_RECORD_SERVER_DESC();
} RECORD_SERVER_DESC;

enum	PREVIEW_FRAME_TYPE { PFT_PREVIEW, PFT_PRINTSCREEN, PFT_STATISTIC};

/*
Структура хранит данные об одном кадре превью
*/
typedef struct _RAW_PREVIEW_ITEM
{
	PREVIEW_FRAME_TYPE	frameType;		// тип источника фрейма
	union
	{
		unsigned long		recordID;		// номер сессии к которой относится этот кадр
		unsigned long		sessionID;		// номер записи к которой относится этот кадр
	};
	unsigned long		frameTime;		// время кадра в мс
	unsigned int		imageWidth;		// ширина хранящейся картинки
	unsigned int		imageHeight;	// высота хранящейся картинки
	unsigned int		channelsCount;	// количество каналов
	byte*				imageData;		// пиксели хранящейся картинки
	unsigned long		dataSize;		// размер данных
	MP_STRING			targetName;		// 
	MP_STRING			cameraName;		// название камеры
	byte				countScreenImage; // rjkbxtcndj
	bool				isDirectChannelOrder;

	_RAW_PREVIEW_ITEM();
	_RAW_PREVIEW_ITEM(const _RAW_PREVIEW_ITEM& other);	
} RAW_PREVIEW_ITEM;

typedef struct _SEND_HEADER
{
	union
	{
		unsigned long		recordID;		// номер сессии к которой относится этот кадр
		unsigned long		sessionID;		// номер записи к которой относится этот кадр
	};
	unsigned long	frameTime;		// время кадра в мс
	unsigned int	imageWidth;		// ширина хранящейся картинки
	unsigned int	imageHeight;	// высота хранящейся картинки
	unsigned int	imageFormat;	// формат хранящейся картинки
	unsigned long	dataSize;		// размер данных

	_SEND_HEADER()
	{
		recordID = frameTime = imageWidth = imageHeight = 0;
		imageFormat = dataSize = 0;
	}
}	SEND_HEADER;

#define RT_SetSplashSrc	1

/*
Класс пересылает скриншоты для превью на превью-сервер
*/
class nrmPreviewRecorder : public ViskoeThread::CThreadImpl<nrmPreviewRecorder>, 
							public preview::IPreviewRecorder,
							public cs::imessagehandler
{
public:
	nrmPreviewRecorder( oms::omsContext* apContext);
	virtual ~nrmPreviewRecorder();

	// реализация CThreadImpl<nrmPreviewRecorder>
public:
	DWORD Run();

	BEGIN_IN_MESSAGE_MAP()
		IN_MESSAGE2( RT_SetSplashSrc, OnSetSplashSrc)
	END_IN_MESSAGE_MAP()

	// реализация preview::IPreviewRecorder
public:
	// начать запись
	virtual void StartRecording(unsigned int aRecordID, unsigned int aDuration);
	// добавить событие к текущей записи
	virtual void AddEvent( wchar_t * alpcEvent, wchar_t * alpcCameraName);
	// добавить статистику о просмотре экранов
	virtual void AddStatistic( wchar_t * alpcEvent);
	// добавить фрейм к текущей записи
	virtual bool AddFrame( rmml::mlMedia* apObject, wchar_t * alpcCameraName, char countScreenImage);
	// добавить фрейм к текущей сессии
	virtual bool AddStatisticFrame( rmml::mlMedia* apObject);
	// добавить сообщение чата к текущей записи
	virtual void AddChatMessage( int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled);
	// остановить запись
	virtual void StopRecording(unsigned int aRecordID);
	// получить адрес превью текущей записи
	virtual void GetPreviewCurrentRecordAddress( int aRecordID, wchar_t* &alpPreviewCurrentRecordAddress);
	// установить адрес сервера превью
	virtual void SetPreviewServerAddress( unsigned int aiRecordID, char *lpServerAddress, unsigned int aiServerPort, char *lpPreviewHttpHost, char *lpStoragePath);
	// удалить запись (отсылает запрос на сервер)
	virtual bool DeleteRecord(unsigned int aRecordID);
	// удалить записи (отсылает запрос на сервер)
	virtual bool DeleteRecords( unsigned int *pRecIDs, unsigned int count);
	// установить кадр на объект
	virtual wchar_t* ApplyFrameToTarget( const char* lpcTarget, int bpp, int width, int height, const void* bits, int quality);
	// сохранить изображение во временное хранилище(н-р аватара)
	virtual wchar_t* AddImage( rmml::mlMedia* apObject, const char* lpcName, int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight);
	// установить время начала проигрывания звука
	virtual void AddPreviewAudio( unsigned int auiRecordID, const char* alpcFileName);
	// установить картинку по умолчанию из картинки alpcFileName
	virtual void SetPreviewDefaultImage( unsigned int auiRecordID, wchar_t *alpcFileName);
	// получить продолжительность записи
	virtual unsigned int getRecordingDuration();
	// получить продолжительность сессии
	unsigned int getStatisticsTime();

private:
	void AddFrameToQueue(RAW_PREVIEW_ITEM& frameData);
	bool FillFrameData(RAW_PREVIEW_ITEM& frameData, nrmImage* frameImage, unsigned int quality, bool aDeleteTexture, bool isYInvert = true);
	bool FillRecordFrameData(RAW_PREVIEW_ITEM& frameData, nrmImage* frameImage, unsigned int quality, wchar_t * alpcCameraName, unsigned long time, bool aDeleteTexture, byte countScreenImage);
	bool FillStatisticFrameData(RAW_PREVIEW_ITEM& frameData, nrmImage* frameImage, unsigned int quality, unsigned long time, bool aDeleteTexture);
	bool CompressImageData(RAW_PREVIEW_ITEM& frameData, int aWidth, int aHeight, int aChannels, void* lpUncompressedData, int quality);
	bool StretchCompressedImageData(RAW_PREVIEW_ITEM& frameData, int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight, void* lpUncompressedData, int quality);
	void RegisterRecordServer( unsigned int aiRecordID, LPCSTR alpServerAddress, unsigned int recordServerPort, LPCSTR alpHttpHost, LPCSTR alpStoragePath);
	bool SetRecordServerByRecordID( unsigned int aiRecordID);
	bool SetRecordServerByFrame(RAW_PREVIEW_ITEM * frame);
	bool GetNextFrame( RAW_PREVIEW_ITEM& frame);
	void GetBackMissedServerFrame( RAW_PREVIEW_ITEM& frame);
	void SendPreviewAudioStartTime( unsigned int auiAudioTime, const char* alpcFileName);
	void SendPreviewChatMessage( int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled);
	void SendPreviewEvent( wchar_t *wsEvent, wchar_t *wsObjectName);
	void SendPreviewStatistic( wchar_t *wsStatistic);

	void OnSetSplashSrc( BYTE* aData, int aDataSize);
	// установить адрес сервера превью
	bool SetPreviewServerAddressImpl( char *lpServerAddress, unsigned int aiServerPort);
	void SendPreviewAudioStartTimeImpl( unsigned int auiAudioTime, const char* alpcFileName);

	void SendPreviewChatMessageImpl( int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled);
	void SendPreviewEventImpl( wchar_t *wsEvent, wchar_t *wsObjectName);
	void SendPreviewStatisticImpl( wchar_t *wsStatistic);
	void StartStatisticRecording(unsigned int recordId, unsigned int aDuration);
	void StopStatisticRecording();

protected:
	 oms::omsContext* context;
	 unsigned long	startSessionTime;
	 unsigned int	startRecordTime;
	 unsigned int	recordDuration;
	 unsigned int	recordID;

	 bool			bRecordingProgress;
	 bool			bStatisticRecordingProgress;

	 typedef	MP_VECTOR<RAW_PREVIEW_ITEM>	VecFrames;
	 typedef	VecFrames::iterator	VecFramesIt;
	 VecFrames	rawFrames;

	 typedef	MP_MAP<int, bool>	MapSplashes;
	 typedef	MapSplashes::iterator	MapSplashesIt;
	 MapSplashes	mapSplashes;

	 typedef	MP_MAP<unsigned int, RECORD_SERVER_DESC>	MapRecordsServers;
	 typedef	MapRecordsServers::iterator	MapRecordsServersIt;
	 MapRecordsServers	mapServers;

	 CEvent					addFrameEvent;
	 CAutoCriticalSection	csFrames;
	 CAutoCriticalSection	csSession;
	 cs::iclientsession*	session;

	 bool					isRecordingNow;
	 //CComString				sPreviewServer;
	 //int					iPreviewServerPort;
	 cs::CServerInfo		serverInfo;
	 MP_STRING				sPreviewHttpHost;
	 MP_STRING				sStoragePath;

	 MP_WSTRING				sLastPrintScreenFilePath;

	 unsigned int			startAudioRecordID;
	 unsigned int			startAudioTime;
	 MP_STRING				startAudioFileName;

	 bool					firstFrame;

};

#endif // !defined(NRMPREVIEW_INCLUDED)