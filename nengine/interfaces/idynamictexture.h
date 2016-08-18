#pragma once

class IDynamicTexture
{
public:
	// Обновить состояние объекта (не текстуру)
	virtual void Update() = 0;
	// Обновить текстуру
	virtual void UpdateFrame() = 0;

	// Вернуть ссылку на массив с построчным содержимым кадра	
	virtual void* GetFrameData() = 0;

	// Вернуть ширину исходной картинки в пикселях
	virtual unsigned short GetSourceWidth() = 0;
	// Вернуть высоту исходной картинки в пикселях
	virtual unsigned short GetSourceHeight() = 0;
	// Вернуть ширину результирующей текстуры в пикселях
	virtual unsigned short GetTextureWidth() = 0;
	// Вернуть высоту результирующей текстуры в пикселях
	virtual unsigned short GetTextureHeight() = 0;
	// Установить размер результирующей текстуры
	virtual void SetTextureSize(int width, int height) = 0;
	// Вернуть количество цветовых каналов, 3 для RGB, 4 для RGBA
	virtual unsigned char GetColorChannelCount() = 0;
	// Порядок цветовых каналов прямой (RGB) или обратный (BGR)
	virtual bool IsDirectChannelOrder() = 0;

	// Нужно ли обновлять текстуру (или сейчас её содержимое не изменяется?)
	virtual bool IsNeedUpdate() = 0;

	// Уничтожь себя
	// (требуется для того, чтобы nengine мог сам решить когда его безопасно удалить)
	virtual void Destroy() = 0;

	// для обоих случаев реализации IDynamicTexture должны 
	// поддерживать userDataID хотя бы от 0 до 3 включительно
	// получить пользовательские (специфичные для вида динамической текстуры) данные
	virtual void* GetUserData(unsigned int userDataID) = 0;
	// установить пользовательские (специфичные для вида динамической текстуры) данные
	virtual void SetUserData(unsigned int userDataID, void* data) = 0;

	virtual void SetFreezState(bool isFreezed)= 0;
	virtual bool IsFreezed()= 0;
	virtual bool IsFirstFrameSetted()= 0;
};