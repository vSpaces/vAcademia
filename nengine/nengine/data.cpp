
#include "stdafx.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include "Data.h"
#include "FileFunctions.h"
#include "cal3d/memory_leak.h"

Data::Data(void* ptr, int len)
{
	bits   = (byte *) ptr;
	length = len;
	pos    = 0;
}

Data::Data(const wchar_t* fileName)
{			
	bits = NULL;
	length = 0;
	pos = 0;
	
	FILE* fd = FileOpen(fileName, L"rb");	

	if (fd == NULL)
	{
		return;
	}

	fseek(fd, 0, 2);
	long len = ftell(fd);
	fseek(fd, 0, 0);

	if (len == -1)
	{
		fclose(fd);

		return;
	}

	bits = MP_NEW_ARR(byte, len);

	length = fread(bits, len, 1, fd);

	fclose(fd);
}

bool Data::IsOk()const
{
	return (bits != NULL);
}

void* Data::GetPtr(int offs)const
{
	if (offs < 0 || offs >= length)
	{
		return NULL;
	}

	return (bits + offs);
}

int	Data::GetBytes(void* ptr, int len)
{
	if (pos >= length)
	{
		return -1;
	}

	if (pos + len > length)
	{
		len = length - pos;
	}

	memcpy(ptr, bits + pos, len);

	pos += len;

	return len;
}

bool Data::GetString(string& str, char term)
{
	if (pos >= length)
	{
		return false;
	}

	str = "";

	while ((pos < length) && (bits[pos] != term))
	{
		str += bits[pos++];
	}

	if ((pos < length) && (bits [pos] == term))
	{
		pos++;
	}
	
	if ((term == '\r') && (pos + 1 < length) && (bits [pos+1] == '\n'))
	{
		pos++;
	}

	return true;
}

Data::~Data()
{
	if (bits)
	{
		MP_DELETE_ARR(bits);
	}
}
