#pragma once

#include "nrmObject.h"

class nrmImage;

//////////////////////////////////////////////////////////////////////////
class nrmMOIVisibleStub :	public moIVisible
{
public:
	nrmMOIVisibleStub();
	~nrmMOIVisibleStub();

public:
	// реализация moIVisible
	// Свойства
	virtual void AbsXYChanged(); // изменилось абсолютное положение 
	virtual void SizeChanged();
	virtual void AbsDepthChanged(); // изменилась абсолютная глубина
	virtual void AbsVisibleChanged(); // изменилась абсолютная видимость
	virtual void AbsOpacityChanged(); // изменилась абсолютная видимость
	// Прочие
	virtual mlSize GetMediaSize(); 
	virtual void Crop(short awX, short awY, MLWORD awWidth, MLWORD awHeight); 
	virtual mlRect GetCropRect();
	virtual void SetMask(mlMedia* aMask, int aiX=0, int aiY=0);
	virtual void SetMask(int aiX, int aiY, int aiW, int aiH);
	virtual int IsPointIn(const mlPoint aPnt); // -1 - not handled, 0 - point is out, 1 - point is in

protected:
	virtual void	UpdateMask( nrmImage* apMask);
	mlMedia*		GetMask();
	mlMedia*		m_pMaskImage;
};