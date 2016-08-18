
#pragma once

#include "IDynamicTexture.h"
#include "IKinect.h"

class CKinectVideoTexture : public IDynamicTexture
{
public:
	CKinectVideoTexture(IKinect* kinect);	
	~CKinectVideoTexture();

	// Обновить состояние объекта (не текстуру)
	void Update();
	// Обновить текстуру
	virtual void UpdateFrame();

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

	// Нужно ли обновлять текстуру (или сейчас её содержимое не изменяется?)
	bool IsNeedUpdate();

	// Уничтожь себя
	// (требуется для того, чтобы nengine мог сам решить когда его безопасно удалить)
	void Destroy();

	// для обоих случаев реализации IDynamicTexture должны 
	// поддерживать userDataID хотя бы от 0 до 3 включительно
	// получить пользовательские (специфичные для вида динамической текстуры) данные
	void* GetUserData(unsigned int userDataID);
	// установить пользовательские (специфичные для вида динамической текстуры) данные
	void SetUserData(unsigned int userDataID, void* data) ;

	void SetFreezState(bool isFreezed);
	bool IsFreezed();
	bool IsFirstFrameSetted();

	void EnableAnalyzeMode(bool isEnabled);
	bool IsAnalyzeEnabled()const;

	unsigned int GetPixelsWithGivenChannel(char channelID);

private:
	void AllocBufferIfNeeded(unsigned int size);

	IKinect* m_kinect;
	unsigned char* m_data;
	unsigned int m_size;

	void* m_userData[4];

	bool m_isAnalyzeEnabled;
};