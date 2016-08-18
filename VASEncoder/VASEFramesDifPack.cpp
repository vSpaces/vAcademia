#include "StdAfx.h"
#include ".\VASEFramesDifPack.h"
#include ".\VASEBitmap.h"
#include <algorithm>

#include ".\MemLeaks.h"

VASEFramesDifPack::VASEFramesDifPack(void): MP_VECTOR_INIT(bitmaps)
{
	InitializeCriticalSection(&csFactory);
	easyPacked = false;
}

VASEFramesDifPack::~VASEFramesDifPack(void)
{
	std::vector<VASEBitmap*>::iterator it = bitmaps.begin(), end = bitmaps.end();
	for (; it!= end; it++)
	{
		SAFE_DELETE( (*it));
	}
	bitmaps.clear();
	DeleteCriticalSection(&csFactory);
}

//! возвращает количество картинок
unsigned int	VASEFramesDifPack::GetBitmapsCount()
{
	return bitmaps.size();
}

//! возвращает картинку по индексу
VASEBitmap*	VASEFramesDifPack::GetBitmap( unsigned int auIndex)
{
	if( auIndex >= bitmaps.size())
		return NULL;

	return bitmaps[ auIndex];
}

//! Создает картинку и возвращает на нее указатель
VASEBitmap*	VASEFramesDifPack::AddBitmap()
{
	VASEBitmap*	bitmap = MP_NEW( VASEBitmap);
	EnterCriticalSection(&csFactory);
	bitmaps.push_back( bitmap);
	LeaveCriticalSection(&csFactory);
	return bitmap;
}

//! Создает картинку и возвращает на нее указатель
bool	VASEFramesDifPack::DeleteBitmap(VASEBitmap* apBitmap)
{
	std::vector<VASEBitmap*>::iterator it = find( bitmaps.begin(), bitmaps.end(), apBitmap);
	if( it != bitmaps.end())
	{
		SAFE_DELETE( (*it));
		EnterCriticalSection(&csFactory);
		bitmaps.erase( it);
		LeaveCriticalSection(&csFactory);
		return true;
	}
	return false;
}

//! устанавливает флаг того что картинка не сжималась после wavelet-преобразования
void VASEFramesDifPack::SetEasyPacked()
{
	easyPacked = true;
}
//! 
bool VASEFramesDifPack::IsEasyPacked()
{
	return easyPacked;
}