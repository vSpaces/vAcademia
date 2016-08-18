#pragma once

#include "../lame/lame.h"

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#ifdef UINT_MAX
# define         MAX_U_32_NUM            UINT_MAX
#else
# define         MAX_U_32_NUM            0xFFFFFFFF
#endif

#define         Min(A, B)       ((A) < (B) ? (A) : (B))
#define         Max(A, B)       ((A) > (B) ? (A) : (B))

#define LR  0
#define MS  2


typedef enum sound_file_format_e 
{
	sf_unknown, 
	sf_raw, 
	sf_wave, 
	sf_aiff, 
	sf_mp1,  /* MPEG Layer 1, aka mpg */
	sf_mp2,  /* MPEG Layer 2 */
	sf_mp3,  /* MPEG Layer 3 */
	sf_ogg 
} sound_file_format;

#include "cthread.h"
#include "mutex.h"

class CFrameManager;
class CRecordState;

class CMP3Encoder : public CThread
{
public:
	CMP3Encoder( CComString aUserName);	
	virtual ~CMP3Encoder();

public:
	//void create( CRecordState *recordState);
	// добавить данные
	int addData( void *data, int length);	
	// удалить все данные
	void removeAllData();
	void destroy();
	// блок сформирован? True -да, иначе нет
	BOOL isBlockReady();
	// MP3-кадр (будущий) - число блоков данных равно blockNumber? True - да, иначе нет
	BOOL isMP3FrameReady();

protected: 	
	// инициализация
	int lameInit();
	// установка параметров сэмпла
	int setMP3Params( int channels, int samples_per_sec, int bits_per_sample);
	// открыть файл для записи
	BOOL openMP3File( WCHAR *sFileName);
	// открыть новый файл для записи
	BOOL openNewMP3File( WCHAR *sFileName);
	// кодирование сэмпла
	int encode_sample();
	// установить пустой кадр (сэмпл)
	int setEmptySample();
	// завершить кодирование
	int encode_flush();
	// закрыть файл
	void closeMP3File();
	//
	void lameClose();

private:
	//
	int  lame_set_stream_binary_mode ( FILE* const fp );
	//
	FILE * init_newoutfile( wchar_t *outPath);
	//
	FILE * init_outfile( wchar_t *outPath, int decode);
	// 
	int unpackReadSamples( const int samples_to_read, const int bytes_per_sample,
						const int swap_order, int *sample_buffer);
	//
	int readSamplesPcm( int sample_buffer[ 2304], int frame_size, int samples_to_read);
	//
	int getAudioCommon( int buffer[2][1152], short buffer16[2][1152]);
	//
	int getAudio( int buffer[2][1152]);

public:
	CFrameManager *getFrameManager();

private:	
	// кол-во кадров
	int frameCount;
	// номер текущего кадра
	int currentFrame;
	// менеджер кадров - осуществляет преобразование кадров
	CFrameManager *frameManager;
	// менеджер кадров - осуществляет преобразование кадров
	CFrameManager *frameManager2;
	//
	BOOL bFirst;
	// поток вывода в файл
	FILE *outfile;
	// поток ввода из файл
	FILE *infile;
	// структура параметров (флагов) кодирования
	lame_global_flags *gf;
	// формат файла
	sound_file_format input_format;
	// кол-во бит
	int pcmbitwidth;
	//
	int pcmswapbytes;
	//
	int count_samples_carefully;
	// кол-во прочитанных сэмплов
	int num_samples_read;
	// пустой кадр (сэмпл)
	BOOL bEmpty;
	// mp3-буфер
	unsigned char mp3buffer[ LAME_MAXMP3BUFFER];
	// буфер данных
	int Buffer[ 2][ 1152];
	// число блоков, которые записываются в файл
	int blockNumber;

private:
	//	
	CMutex updateStateMutex;
	// TRUE: запущено на сохранение данных
	BOOL bRunSaver;	

#ifdef _DEBUG
	CFrameManager *sampleFrameManager;
#endif
	// состояние записи: параметры записи
	CRecordState *recordState;
	//
	CComString m_sUserName;
	//
	BOOL bTerminating;
	BOOL bTerminated;
	// указывает, что параметры уже установлены
	BOOL bParamsSetted;

	// битрейт
	int m_iBitBrate;
	// канал: моно или стерео
	int m_iChannelMode;
	// уровень качества
	int m_iQuality;
	// частота
	int m_iSampleRate;

protected:
	int encode();

public:
	BOOL setFileName( WCHAR *sFileName);
	// установка параметров записываемого звука с микрофона
	void setParams( int channels, int sampleRate, int sampleSize);
	// установка параметров режима записи в звуковой файл
	int setMP3FileParams( int bit_brate, int iChannelMode, int iQuality, int iSampleRate);
	BOOL init( int channels, int sampleRate, int sampleSize, WCHAR *sFileName);
	BOOL clearSoundFileLog();
	BOOL run(  BOOL isBufferSaving = FALSE);
	BOOL save();
	BOOL isRecording();
	void setBlockNumber( int blockNumber);	
	WCHAR *getFileName();
	void Main();
	void Terminating();
	void startSaving();

	static BOOL getFileFullNameFromUuid( CWComString &tmpFileFullName, CWComString &tmpFileName, CWComString firstLine = L"", CWComString lastLine = L"");
	static BOOL getPathFromName( char *name, wchar_t *path);

public:
	static void turnOn();
	static void turnOff();
	static BOOL isTurn();
};
