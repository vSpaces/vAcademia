
#pragma once

#include <oms_context.h>

class CWindowPrintSupport
{
public:
	CWindowPrintSupport(oms::omsContext* context);
	~CWindowPrintSupport();

	void HandlePrintRequest(unsigned int wParam);

private:
	void HandleSimpleCaptureRequest(unsigned char* data, unsigned int size, unsigned int width, unsigned int height, unsigned int wParam);
	void HandleVideoCaptureRequest(unsigned char* data, unsigned int size, unsigned int width, unsigned int height);

	void FreeGlobalMemoryObjectIfNeeded();
	void CreateGlobalMemoryObjectIfNeeded(unsigned int size);

	void ConvertRGBToBGR(unsigned char* data, unsigned int size);

	oms::omsContext* m_context;
	HANDLE m_globalMemoryObject;
	unsigned int m_globalMemorySize;
};