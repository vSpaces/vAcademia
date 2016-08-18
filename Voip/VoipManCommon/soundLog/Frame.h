#pragma once

class CFrame
{	
public:
	CFrame();
	CFrame( CFrame *frame);
	CFrame( void* data, int size);
	~CFrame();

public:
	// получить данные
	void *getData();
	// получить данные
	void *getData( int pos);
	// получить данные целого типа
	int getIntData();	
	// получить данные целого типа
	int getIntData( int size, int pos = 0);	
	// получить данные типа с плавающей точкой
	float getFloatData();
	// получить данные типа с плавающей точкой
	float getFloatData( int size, int pos = 0);
	// получить данные
	void *getCopyData();
	// получить данные
	void *getCopyData( int size, int pos = 0);
	// получить данные
	void getCopyData( void *data, int size, int destOffset = 0, int sourceOffset = 0);	
	// получить размер 8*bytes-битных данных
	int getSize( int bytes = 1);

	// создать новые данные
	void createData( int size);

protected:
	// создать новые данные
	void createData( void* data, int size, BOOL bUseBlock = TRUE);
	// создать новые данные
	void createData( CFrame *frame);

public:
	// заполнить поток байтов size нулями
	void createSilence( int size);
	// заполнить поток байтов нулями
	void createSilence();
	// обновить данные
	void updateData( void *data, int size, BOOL bUseBlock = TRUE);
	// обновить данные
	void updateData( void *data, int size, int destOffset, int sourceOffset = 0, BOOL bUseBlock = TRUE);
	// обновить данные
	void updateData( CFrame *frame, int destOffset = 0, int sourceOffset = 0, BOOL bUseBlock = TRUE);
	// копировать данные
	int copyData( const void *data, int size, int destOffset = 0, int sourceOffset = 0);
	// копировать данные
	int copyBytes( const BYTE *data, int size, int destOffset = 0, int sourceOffset = 0);
	// копировать данные
	int copyData( CFrame *frame, int destOffset = 0, int sourceOffset = 0);
	// копировать данные целого типа
	int copyIntData( int data, int size, int destOffset = 0, int sourceOffset = 0);
	// копировать данные типа с плавающей точкой
	int copyFloatData( float data, int size, int destOffset  = 0, int sourceOffset = 0);
	// удалить данные
	void deleteData();
	// определить тип блока
	BOOL getBlockType();
	// установить тип блока
	void setBlockType( BOOL bUseBlock);
	// установить пустые данные: то есть data имеет устаревшие данные. Размер данных считать равным нулю.
	void setEmptyData();	

private:
	char *convertIntToChar( int iData, int size);


private:
	void *data;
	int size;
	BOOL bUseBlock;
};
