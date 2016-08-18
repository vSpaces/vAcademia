
#include "StdAfx.h"
#include "WindowPrintSupport.h"
#include <assert.h>
#include <rm\rm.h>

CWindowPrintSupport::CWindowPrintSupport(oms::omsContext* context):
	m_context(context),
	m_globalMemorySize(0),
	m_globalMemoryObject(NULL)
{
	assert(m_context);
}

void CWindowPrintSupport::CreateGlobalMemoryObjectIfNeeded(unsigned int size)
{
	if ((m_globalMemoryObject == NULL) || (m_globalMemorySize != size))
	{
		FreeGlobalMemoryObjectIfNeeded();
		m_globalMemoryObject = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, L"vAcademiaCaptureSharedMemory");

		if (m_globalMemoryObject == NULL)
		{
			return;
		}
		else
		{
			m_globalMemorySize = size;
		}
	}
}

void CWindowPrintSupport::FreeGlobalMemoryObjectIfNeeded()
{
	if (m_globalMemoryObject)
	{
		CloseHandle(m_globalMemoryObject);
		m_globalMemoryObject = NULL;
	}
}

void CWindowPrintSupport::HandleVideoCaptureRequest(unsigned char* data, unsigned int size, unsigned int width, unsigned int height)
{
	CreateGlobalMemoryObjectIfNeeded(size);
	if (!m_globalMemoryObject)
	{
		return;
	}

	unsigned char* globalArr = (unsigned char *)MapViewOfFile(m_globalMemoryObject, FILE_MAP_ALL_ACCESS, 0, 0, size);							
	if (globalArr == NULL) 
	{	
		FreeGlobalMemoryObjectIfNeeded();
		return;
	}

	memcpy(globalArr, data, size);
	UnmapViewOfFile(globalArr);
}

void CWindowPrintSupport::ConvertRGBToBGR(unsigned char* data, unsigned int size)
{
	for (unsigned int i = 0; i < size; i += 3)
	{
		unsigned char tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}
}

void CWindowPrintSupport::HandleSimpleCaptureRequest(unsigned char* data, unsigned int size, unsigned int width, unsigned int height,
													 unsigned int wParam)
{
	HDC hDC = (HDC)wParam;
	HDC hMemDC = CreateCompatibleDC(hDC);

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
			
	unsigned char* buffer;
	HBITMAP hBmpNew = ::CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS,(void **)&buffer, NULL, 0);
	SelectObject(hMemDC, hBmpNew);

	memcpy(buffer, data, size); 
	ConvertRGBToBGR(buffer, size);

	BitBlt(hDC, 0, 0, width, height, hMemDC, 0, 0, SRCCOPY);
	
	DeleteObject(hBmpNew);
	DeleteDC(hMemDC);					
}

void CWindowPrintSupport::HandlePrintRequest(unsigned int wParam)
{
	if ((m_context) && (m_context->mpRM))
	{
		unsigned int width, height, size;
		unsigned char* data;

		if (m_context->mpRM->GetVideoCaptureSupport()->GetPictureData((void **)&data, size, width, height))
		{
			if (wParam == 0)
			{
				HandleVideoCaptureRequest(data, size, width, height);				
			}
			else
			{
				HandleSimpleCaptureRequest(data, size, width, height, wParam);
			}

			m_context->mpRM->GetVideoCaptureSupport()->FreePictureData(data);	
		}
	}
}

CWindowPrintSupport::~CWindowPrintSupport()
{
	FreeGlobalMemoryObjectIfNeeded();
}