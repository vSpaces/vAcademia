#pragma once
#include "CaptureVideo.h"
#include <vector>
using namespace std;

typedef struct _WC_RESOLUTION
{
	unsigned int width;
	unsigned int height;

	bool operator==(const _WC_RESOLUTION& res)
	{
		return ((width==res.width)&&(height==res.height))?true:false;
	}

	bool operator<(const _WC_RESOLUTION& res)
	{
		return (width<res.width)?true:false;
	}

}	WC_RESOLUTION;

typedef MP_VECTOR<WC_RESOLUTION>	vecResolutions;
typedef vecResolutions::iterator	vecResolutionsIt;

class CRTBaseDevice
{
public:
	CRTBaseDevice(void);
	~CRTBaseDevice(void);

	virtual int	StartImpl(HWND ahWnd) = 0;
	virtual void StopImpl() = 0;
	virtual int PreparationForStart() = 0;
	
	// возвращает текущую картинку с устройства
	virtual HBITMAP	GetSnapshotFrame(bool isFirstFrame) = 0;
	// устанавливает размер изображения
	virtual bool	SetFrameSize( unsigned int auWidth, unsigned int auHeight) = 0;
	virtual vecResolutions&	GetSupportedResolutions() = 0;

	// Возвращает имя устройста
	std::string	GetName();
	std::string	GetDisplayDeviceName();

	int Start(HWND ahWnd);
	void Stop();
	void NeedStoping();
	bool IsNeedStoping();
	bool IsStarted();

protected:
	bool				isStarted;
	bool				isStoping;
	HWND				m_hWnd;
	MP_STRING			m_displayName;
	MP_STRING			m_name;
};
