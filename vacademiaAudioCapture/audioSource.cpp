
#include <streams.h>
#include "audioSource.h"
#include "windows.h"
#include "winuser.h"

#define NEW_GRAB_METHOD	1

CVAcademiaCapture::CVAcademiaCapture(HWND hWnd) :
	m_hWnd(hWnd),
	m_globalMemoryObject(NULL)
{
} 

bool CVAcademiaCapture::CreateGlobalMemoryObjectIfNeeded(unsigned int size)
{
	if (m_globalMemoryObject == NULL)
	{
		m_globalMemoryObject = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, L"vAcademiaAudioCaptureSharedMemory");

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
void CVAcademiaCapture::FreeGlobalMemoryArrayPointer(unsigned char* arrayPointer)
{
	UnmapViewOfFile(arrayPointer);
}

CVAcademiaCapture::~CVAcademiaCapture()
{
	DestroyGlobalMemoryObjectIfNeeded();
}