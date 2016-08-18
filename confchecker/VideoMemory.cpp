
#include "StdAfx.h"
#include "VideoMemory.h"
#include <ddraw.h>
#include "Globals.h"

CVideoMemory::CVideoMemory():
	m_freeSize(0),
	m_totalSize(0),
	m_isAnalyzed(false)
{
}

unsigned int CVideoMemory::GetTotalSize(int vendorID)
{
	Analyze(vendorID);

	return m_totalSize;
}

unsigned int CVideoMemory::GetFreeSize(int vendorID)
{
	Analyze(vendorID);

	return m_freeSize;
}

void CVideoMemory::Analyze(int vendorID)
{
	if (m_isAnalyzed)
	{
		return;
	}

	CoInitialize(NULL);

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

	if (vendorID == VENDOR_INTEL)
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

	m_ddTotal = total;
	m_ddFree = totalFree;
	m_ddLocal = local;
	m_ddLocalFree = localFree;
	m_ddAGP = agp;
}

unsigned int CVideoMemory::GetDirectDrawTotal()
{
	return m_ddTotal;
}

unsigned int CVideoMemory::GetDirectDrawFree()
{
	return m_ddFree;
}

unsigned int CVideoMemory::GetDirectDrawLocal()
{
	return m_ddLocal;
}


unsigned int CVideoMemory::GetDirectDrawLocalFree()
{
	return m_ddLocalFree;
}

unsigned int CVideoMemory::GetDirectDrawLocalAGP()
{
	return m_ddAGP;
}

CVideoMemory::~CVideoMemory()
{
}