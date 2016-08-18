#pragma once

class VASEBitmap;

/************************************************************************/
/* Хранит разницу между кадрами в виде набора пакованных картинок
/************************************************************************/
class VASE_API VASEFramesDifPack
{
public:
	VASEFramesDifPack(void);
	~VASEFramesDifPack(void);

public:
	//! Создает картинку и возвращает на нее указатель
	VASEBitmap*	AddBitmap();
	//! Создает картинку и возвращает на нее указатель
	bool	DeleteBitmap(VASEBitmap* apBitmap);
	//! возвращает количество картинок
	unsigned int	GetBitmapsCount();
	//! возвращает картинку по индексу
	VASEBitmap*		GetBitmap( unsigned int auIndex);
	//! устанавливает флаг того что картинка не сжималась после wavelet-преобразования
	void SetEasyPacked();
	//! 
	bool IsEasyPacked();

private:
	MP_VECTOR<VASEBitmap*> bitmaps;
	CRITICAL_SECTION	csFactory;
	bool easyPacked;
};
