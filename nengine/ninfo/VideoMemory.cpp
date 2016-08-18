
#include "StdAfx.h"
#include "VideoMemory.h"
#include <ddraw.h>
#include "GlobalSingletonStorage.h"

CVideoMemory::CVideoMemory():
	m_freeSize(0),
	m_totalSize(0),
	m_isAnalyzed(false)
{
}

unsigned int CVideoMemory::GetTotalSize()
{
	Analyze();

	return m_totalSize;
}

unsigned int CVideoMemory::GetFreeSize()
{
	Analyze();

	return m_freeSize;
}

void CVideoMemory::Analyze()
{
	if (m_isAnalyzed)
	{
		return;
	}

//	CoInitialize(NULL);

	unsigned int local = 0;
	unsigned int total = 0;
	unsigned int localFree = 0;
	unsigned int totalFree = 0;
	unsigned int agp = 0;

	IDirectDraw7* pDirectDraw;
	HRESULT hr = CoCreateInstance(CLSID_DirectDraw7, NULL, CLSCTX_ALL, IID_IDirectDraw7, (void**) &pDirectDraw);
	if(!FAILED(hr))
	{
		hr = IDirectDraw7_Initialize(pDirectDraw, NULL);
		if (!FAILED(hr))
		{
			DDSCAPS2 QueryCaps = {0};
			DWORD dwTotal, dwFree;

			QueryCaps.dwCaps = DDSCAPS_LOCALVIDMEM;
			hr = pDirectDraw->GetAvailableVidMem(&QueryCaps, &dwTotal, &dwFree);
			if (!FAILED(hr))
			{
				local = dwTotal;
				localFree = dwFree;
			}

			QueryCaps.dwCaps = DDSCAPS_TEXTURE;
			hr = pDirectDraw->GetAvailableVidMem(&QueryCaps, &dwTotal, &dwFree);
			if (!FAILED(hr))
			{
				total = dwTotal;
				totalFree = dwFree;
			}

			agp = total - local;
			// либо запросить DDSCAPS_NONLOCALVIDMEM
		}

		pDirectDraw->Release();
	}

	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		m_totalSize = total;
		m_freeSize = totalFree;
	}
	else
	{
		m_totalSize = local;
		m_freeSize = localFree;
	}

	m_isAnalyzed = true;
}

CVideoMemory::~CVideoMemory()
{
}