
#include <streams.h>
#include "ball.h"
#include "windows.h"
#include "winuser.h"

#define NEW_GRAB_METHOD	1

CVAcademiaCapture::CVAcademiaCapture(HWND hWnd, int iImageWidth, int iImageHeight) :
	m_hWnd(hWnd),
	m_globalMemoryObject(NULL),
    m_iImageWidth(iImageWidth),	
    m_iImageHeight(iImageHeight)  
{
} 

bool CVAcademiaCapture::CreateGlobalMemoryObjectIfNeeded(unsigned int size)
{
	if (m_globalMemoryObject == NULL)
	{
		m_globalMemoryObject = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, L"vAcademiaCaptureSharedMemory");

		if (m_globalMemoryObject == NULL)
		{
			return false;
		}
	}					

	return true;
}

void CVAcademiaCapture::DestroyGlobalMemoryObjectIfNeeded()
{
	if (m_globalMemoryObject)
	{
		CloseHandle(m_globalMemoryObject);
		m_globalMemoryObject = NULL;
	}
}

unsigned char* CVAcademiaCapture::GetGlobalMemoryArrayPointer(unsigned int size)
{
	if (!m_globalMemoryObject)
	{
		return NULL;
	}

	return (unsigned char *)MapViewOfFile(m_globalMemoryObject, FILE_MAP_ALL_ACCESS, 0, 0, size);
}

void CVAcademiaCapture::GetAllSizes(unsigned int& width, unsigned int& height, unsigned int& sizeRGB,
						unsigned int& sizeRGBA)
{
	RECT clientRect;
	GetClientRect(m_hWnd, &clientRect);
	width = clientRect.right - clientRect.left;
	height = clientRect.bottom - clientRect.top;

	int sizeOfOneChannel = width * height;
	sizeRGB = sizeOfOneChannel * 3;
	sizeRGBA = sizeOfOneChannel * 4;
}

void CVAcademiaCapture::ConvertRGBToRGBA(unsigned char* destData, unsigned int destSize,
							 unsigned char* srcData, unsigned int srcSize)
{
	int k = srcSize - 3;

	for (int i = destSize - 4; i >= 0; i -= 4, k -= 3)
	{
		destData[i] = srcData[k + 2];
		destData[i + 1] = srcData[k + 1];
		destData[i + 2] = srcData[k];
		destData[i + 3] = 255;
	}
}

void CVAcademiaCapture::FreeGlobalMemoryArrayPointer(unsigned char* arrayPointer)
{
	UnmapViewOfFile(arrayPointer);
}

bool CVAcademiaCapture::GetImage(unsigned char* data, unsigned int size)
{
	if (!m_hWnd)
	{
		memset(data, 0, size);
		return false;
	}

#ifdef NEW_GRAB_METHOD
	unsigned int width, height, sizeRGB, sizeRGBA;
	GetAllSizes(width, height, sizeRGB, sizeRGBA);

	SendMessage(m_hWnd, WM_PRINT, (WPARAM)0, PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT | PRF_OWNED);

	if (!CreateGlobalMemoryObjectIfNeeded(sizeRGB))
	{
		return false;
	}

	unsigned char* imageCaptureData = NULL;
	imageCaptureData = GetGlobalMemoryArrayPointer(sizeRGB);							
	if (imageCaptureData == NULL) 
	{	
		DestroyGlobalMemoryObjectIfNeeded();
		return false;
	}	

	char noImageString[8] = "NOIMAGE";
	if (memcmp(imageCaptureData, &noImageString[0], strlen(noImageString)) == 0)
	{
		FreeGlobalMemoryArrayPointer(imageCaptureData);
		return false;
	}

	ConvertRGBToRGBA(data, sizeRGBA, imageCaptureData, sizeRGB);

	FreeGlobalMemoryArrayPointer(imageCaptureData);

	return true;
#else
	HDC hDC = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hDC);

	RECT rct;
	GetWindowRect(m_hWnd, &rct);
	RECT clientRct;
	GetClientRect(m_hWnd, &clientRct);

	int captionHeight = (rct.bottom - rct.top) - (clientRct.bottom - clientRct.top);
	int borderWidth = ((rct.right - rct.left) - (clientRct.right - clientRct.left)) / 2;

	int startX = rct.left + borderWidth;
	int startY = rct.top + captionHeight - borderWidth;

	int width = clientRct.right - clientRct.left;
	int height = clientRct.bottom - clientRct.top;
			
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1; 
	bmi.bmiHeader.biBitCount = 24; 
	bmi.bmiHeader.biCompression = 0; 
	bmi.bmiHeader.biSizeImage = height * width * 3; 
	bmi.bmiHeader.biXPelsPerMeter = 0; 
	bmi.bmiHeader.biYPelsPerMeter = 0; 
	bmi.bmiHeader.biClrUsed = 0; 
	bmi.bmiHeader.biClrImportant = 0; 
	
	HBITMAP hBmpNew = CreateCompatibleBitmap(hDC, width, height);
	SelectObject(hMemDC, hBmpNew);
	BitBlt(hMemDC, 0, 0, width, height, hDC, startX, startY, SRCCOPY);

	GetDIBits(hMemDC, hBmpNew, 0, height, data, &bmi, DIB_RGB_COLORS);
			
	ReleaseDC(m_hWnd, hMemDC);
	DeleteDC(hMemDC);
	DeleteObject(hBmpNew);

	int _size = width * height;
	int size3 = _size * 3;
	int size4 = _size * 4;

	int k = size3 - 3;

	for (int i = size4 - 4; i >= 0; i -= 4, k -= 3)
	{
		data[i] = data[k + 0];
		data[i + 1] = data[k + 1];
		data[i + 2] = data[k + 2];
		data[i + 3] = 255;
	}

	return true;
#endif
}

CVAcademiaCapture::~CVAcademiaCapture()
{
	DestroyGlobalMemoryObjectIfNeeded();
}