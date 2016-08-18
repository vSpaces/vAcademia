
#pragma once

#include <windows.h>
#pragma warning(push)
#pragma warning(disable : 4201)
	#include <vfw.h>
#pragma warning(pop)
#include "IVideo.h"
#include "IDynamicTexture.h"

#define STATUS_NOT_OPEN		0
#define STATUS_OPENED		1
#define STATUS_PLAYING		2
#define STATUS_STOPPED		3
#define STATUS_PAUSED		4

class CVideo : public IVideo, public IDynamicTexture
{
public:
	CVideo();
	~CVideo();

public:
	// Начать воспроизведение
	void Start();
	// Остановить воспроизведение
	void Stop();
	// Пауза
	void Pause();
	// Продолжить играть после паузы
	void Play();

	// Возвратить продолжительность анимации в миллисекундах
	unsigned int GetDuration();
	// Установить/снять зацикленное воспроизведение
	// По умолчанию зацикленности нет
	void SetLoop(bool isLooped);

	// Вернуть текущую позицию в миллисекундах
	int GetPosition();
	// Установить текущую позицию в миллисекундах
	void SetPosition(unsigned int pos);

	// Вернуть текущий номер кадра
	int GetFrameNum();
	// Установить текущий номер кадра
	void SetFrameNum(unsigned int num);

	// Возвращает true, если зацикленность включена
	bool IsLooped();
	// Возвращает true, если идет проигрывание
	bool IsPlayed();
	// Возвращает true, если поставлена пауза
	bool IsPaused();

	// Возвращает динамическую текстуру
	IDynamicTexture* GetDynamicTexture();

		// Открыть файл и возвратить успешность открытия
	bool Open(char* fileName);
	// Уничтожить видео
	void Close();

public:
	// Обновить состояние объекта
	void Update();
	// Обновить текстуру
	void UpdateFrame();

	// Вернуть ссылку на массив с построчным содержимым кадра	
	void* GetFrameData();

	// Вернуть ширину исходной картинки в пикселях
	unsigned short GetSourceWidth();
	// Вернуть высоту исходной картинки в пикселях
	unsigned short GetSourceHeight();
	// Вернуть ширину результирующей текстуры в пикселях
	unsigned short GetTextureWidth();
	// Вернуть высоту результирующей текстуры в пикселях
	unsigned short GetTextureHeight();
	// Установить размер результирующей текстуры
	void SetTextureSize(int width, int height);
	// Вернуть количество цветовых каналов, 3 для RGB, 4 для RGBA
	unsigned char GetColorChannelCount();
	// Порядок цветовых каналов прямой (RGB) или обратный (BGR)
	bool IsDirectChannelOrder();

	bool IsNeedUpdate();

	void Destroy();

	void* GetUserData(unsigned int userDataID);
	void SetUserData(unsigned int userDataID, void* data);

	void SetFreezState(bool /*isFreezed*/){};
	bool IsFreezed(){return false;};
	bool IsFirstFrameSetted(){return false;};

private:
	void UpdateTime();

	unsigned char m_currentStatus;
	unsigned char m_colorChannelCount;

	unsigned short m_width;
	unsigned short m_height;
	unsigned short m_textureWidth;
	unsigned short m_textureHeight;

	bool m_isLooped;
	bool m_isDirectChannelOrder;

	unsigned int m_duration;
	unsigned int m_position;
	unsigned int m_lastUpdateTime;
	unsigned int m_frameCount;

	unsigned int m_updateWidth;
	unsigned int m_updateHeight;
	unsigned int m_updateTextureWidth;
	unsigned int m_updateTextureHeight;
	unsigned int m_updateFrameNum;
	HDC m_updateHDC;
	HBITMAP m_updateBitmap;
	PGETFRAME m_updateGetFrame;

	int m_lastFrameNum;

	unsigned char* m_frameData;

	AVISTREAMINFO m_aviStreamInfo;
	PAVISTREAM m_aviStream;
	PGETFRAME m_getFrame; 
	HBITMAP m_bitmap;
	HDRAWDIB m_hdd;
	HDC m_hdc;

	void* m_userData[4];
};