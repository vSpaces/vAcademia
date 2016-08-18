
#pragma once

class IDynamicTexture;

class IVideo
{
public:
	// Начать воспроизведение
	virtual void Start() = 0;
	// Остановить воспроизведение
	virtual void Stop() = 0;
	// Пауза
	virtual void Pause() = 0;
	// Продолжить играть после паузы
	virtual void Play() = 0;

	// Возвратить продолжительность анимации в миллисекундах
	virtual unsigned int GetDuration() = 0;
	// Установить/снять зацикленное воспроизведение
	// По умолчанию зацикленности нет
	virtual void SetLoop(bool isLooped) = 0;

	// Вернуть текущую позицию в миллисекундах
	virtual int GetPosition() = 0;
	// Установить текущую позицию в миллисекундах
	virtual void SetPosition(unsigned int pos) = 0;

	// Вернуть текущий номер кадра
	virtual int GetFrameNum() = 0;
	// Установить текущий номер кадра
	virtual void SetFrameNum(unsigned int num) = 0;

	// Возвращает true, если зацикленность включена
	virtual bool IsLooped() = 0;
	// Возвращает true, если идет проигрывание
	virtual bool IsPlayed() = 0;
	// Возвращает true, если поставлена пауза
	virtual bool IsPaused() = 0;

	// Возвращает динамическую текстуру
	virtual IDynamicTexture* GetDynamicTexture() = 0;

	// Открыть файл и возвратить успешность открытия
	virtual bool Open(char* fileName) = 0;
	// Уничтожить видео
	virtual void Close() = 0;
};