#pragma once

#include "StdAfx.h"

struct SamplesBuffer
{
	BYTE* data;
	int size;

	SamplesBuffer::SamplesBuffer()
	{
		data = NULL;
		size = 0;
	}

	SamplesBuffer::SamplesBuffer(void* data, int size)
	{
		data = data;
		size = size;
	}

};