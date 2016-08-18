// DataBuffer.h: interface for the CDataBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATABUFFER_H__99EE1391_6C9F_4B88_8B63_3E77B54C0876__INCLUDED_)
#define AFX_DATABUFFER_H__99EE1391_6C9F_4B88_8B63_3E77B54C0876__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "UtilTypes.h"

using namespace std;
class CDataBuffer
{
public:
	CDataBuffer( DWORD aSize=0);
	~CDataBuffer();
	BYTE* addData( DWORD aDataSize, BYTE* aData);
	// jedi
	BYTE *addData(const CByteVector& aData);
	BYTE *setData(BYTE* data, DWORD aSize, DWORD destOffset = 0, DWORD sourceOffset = 0);
	BYTE *add(unsigned short aDataSize, BYTE *aData);
	BYTE *add(unsigned int aDataSize, BYTE *aData);
	BYTE *add(DWORD aDataSize, BYTE *aData);
	BYTE *add(const CByteVector& aData);
	template<typename T> BYTE *add(T aData);
	BYTE *addStringUnicode(unsigned short aDataSize, BYTE *aData);
	//
	inline operator BYTE*() { return pData;};
	inline DWORD getSize() { return dwDataSize;};
	inline DWORD getLength() { return dwDataLength;};
	inline void clear() { dwDataLength = 0;};
	int getPos()const{ return (dwDataLength == 0 && miGetIdx > 0)?miGetIdx:dwDataLength; }
	void setPos(int aiPos=-1){
		miGetIdx=miSetIdx=aiPos; 
		if(aiPos==getPos())	miSetIdx=-1;
		if(miGetIdx < 0) miGetIdx=0;
	}
	bool growFromBegin(DWORD aNewSize);
protected:
	// Увеличивает буфер
	virtual bool grow( DWORD aNewSize);
	// jedi
	BOOL isValidParams( const void *data, DWORD aSize, DWORD destOffset = 0, DWORD sourceOffset = 0);	
	//

	// Буфер данных
	BYTE* pData;
	// Размер буфера данных
	DWORD dwDataSize;
	// Длина данных в буфере
	DWORD dwDataLength;
	int miGetIdx;	// позиция, с которой считывать следующие данные
	int miSetIdx;	// используется для того, чтобы можно было помещать значения не в конец буфера как обычно, 

	int dataGrowSize;
	int growRequestsCount;

};

template<typename T>
BYTE* CDataBuffer::add(T aData)
{
	return addData(sizeof( aData), (BYTE*)&aData);
}

class CGlobalDataBuffer : public CDataBuffer
{
protected:
	// Увеличивает буфер
	virtual bool grow( DWORD aNewSize);
};

class CBaseInfoOut
{
protected: 
	CDataBuffer data;
	unsigned short iDataSize;
public:
	inline unsigned char* GetData() { return (BYTE*) data;}; 
	inline unsigned short GetDataSize() {return iDataSize; }

};

#endif // !defined(AFX_DATABUFFER_H__99EE1391_6C9F_4B88_8B63_3E77B54C0876__INCLUDED_)
