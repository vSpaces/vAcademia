#pragma once

class VASE_API VASEncodBitmap
{
public:
	VASEncodBitmap(void);
	~VASEncodBitmap(void);

public:
	//! возвращает размер
	CSize GetSize();
	//! возвращает позицию
	CPoint	GetPosition();
	//! устанавливает позицию
	void	SetPosition(CPoint aPosition);
	//! устанавливает размер
	void	SetSize(CSize aSize);
	//! возвращает данные картинки
	unsigned int GetBuffer( const void** appData, unsigned int* apDataSize);
	//! выделяет память под картинки
	byte* GetBufferSetLength( unsigned int auBufferSize);
	//! устанавливает размер после сжатия
	void SetEncodedBufferSize( unsigned int auEncodedBufferSize);

private:
	CSize	m_size;
	CPoint	m_position;
	byte*	m_encodedData;
	unsigned int m_allocatedDataSize;
	unsigned int m_encodedDataSize;
};
