#pragma once

#include <vector>

class CFrame;
//PLIST(CFrameList, CFrame);

// менеджер кадров - осуществляет преобразование кадров
class CFrameManager
{
public:
	CFrameManager( int mode = NONE, BOOL autoDeleteObject = TRUE);
	virtual ~CFrameManager( void);

	enum { NONE, EQUAL_BY_SIZE};	

private: 	
	// добавить кадр
	virtual BOOL addFrame( void *data, int &size, BOOL bUseBlock, int dataOffset = 0);
	// обновить кадр
	virtual BOOL updateFrame( int index, void *data, int size, BOOL bUseBlock = FALSE, int destOffset = 0, int sourceOffset = 0);

public:
	// обновить кадр
	virtual BOOL updateFrames( CFrame *frame);
	// обновить кадр
	virtual BOOL updateFrames( void *data, int length, BOOL bUseBlock = FALSE);
	// получить кадр
	virtual CFrame *getFrame( int index);
	// удалить кадр под номером index
	virtual CFrame *removeFrameAt( int index);
	// удалить все кадры
	virtual void removeAllFrames();
	virtual void clear();
	// возвращает TRUE, если список пустой
	BOOL isEmpty();
	// получить число кадров
	int getFrameCount();
	// получить общий размер всех кадров
	long getTotalSize();
	// получить размер заголовка
	int getHeadSize();
	// установить размер полного кадра
	void setFrameSize( int frameSize);
	// получить размер полного кадра
	int getFrameSize();
	// получить данные для пересылки (создать пакет)
	virtual CFrame *createPacket();
	// распарсивание пакета
	virtual void parser( void *data, int size);
	// распарсивание пакета
	virtual void parser( CFrame *frame);

public:
	// блок сформирован, если true, то да, иначе нет
	virtual BOOL isBlockReady();
	//
	virtual int getBlockFrameCount();
	//
	virtual int getMode();
	//
	CFrame *getData();
	//
	CFrame *getDubleData();

private:
	// кол-во кадров
	int frameCount;
	// номер текущего кадра
	//int currentFrame;
	// размер последнего сэмпла
	int lastFrameSize;
	// общий размер данных
	long totalSize;
	// кол-во кадров в одном целом блоке
	int blockFrameCount;
	// размер полного кадра (полностью заполненного кадра данными)
	int frameSize;
	// размер заголовка
	int headSize;
	// режим работы обновления кадров
	int mode;
	// режим уадления объектов
	BOOL autoDeleteObject;

	void add( CFrame* obj)
	{ objects.push_back( obj);};

	CFrame* get( int index)
	{ return objects[ index];};	

	std::vector< CFrame*> &get_list()
	{ return objects;};

	BOOL is_empty()
	{ return objects.empty();};

	int get_size()
	{ return objects.size();};

	//	std::vector< Object>::iterator begin()
	//	{ return objects.begin();};
	//	std::vector< Object>::iterator end()
	//	{ return objects.end();};	

protected:
	MP_VECTOR<CFrame*> objects;
};
