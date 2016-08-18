
#pragma once

#ifdef	_WIN32
	#pragma	warning (disable:4786)
#endif

#include	<string>

using namespace std;

class Data
{
public:
	Data (const wchar_t *fileName);
	Data (void *ptr, int len);
	~Data();

	const string& GetFileName ()const
	{
		return file;
	}
	
	bool IsEmpty()const
	{
		return pos >= length;
	}

	int	GetLength()const
	{
		return length;
	}

	int	GetPos()const
	{
		return pos;
	}

	int	GetByte()
	{
		if (pos < length)
		{
			return bits[pos++];
		}
		else
		{
			return -1;
		}
	}

	signed short GetShort()
	{
		if (pos + 1 >= length)
		{
			return -1;
		}

		signed short v = *(signed short*)(bits + pos);

		pos += 2;

		return v;
	}

	unsigned short GetUnsignedShort()
	{
		if (pos + 1 >= length)
		{
			return 0xFFFF;
		}

		unsigned short v = *(unsigned short *)(bits + pos);

		pos += 2;

		return v;
	}

	long GetLong()
	{
		if (pos + 3 >= length)
		{
			return -1;
		}

		long v = *(long*)(bits + pos);

		pos += 4;

		return v;
	}

	unsigned long GetUnsignedLong()
	{
		if (pos + 3 >= length)
		{
			return 0xFFFFFFFF;
		}

		unsigned long v = *(unsigned long*)(bits + pos);

		pos += 4;

		return v;
	}

	float GetFloat()
	{
		if (pos + 3 >= length)
		{
			return 0;
		}

		float f = *(float*)(bits + pos);

		pos += 4;

		return f;
	}

	double GetDouble()
	{
		if (pos + 7 >= length)
		{
			return 0;
		}

		double d = *(double*)(bits + pos);

		pos += 8;

		return d;
	}

	void* GetPtr()const
	{
		return (bits + pos);
	}

	int	SeekCur(int delta)
	{
		pos += delta;

		if (pos > length)
		{
			pos = length;
		}

		if (pos < 0)
		{
			pos = 0;
		}

		return pos;
	}

	int	SeekAbs(int offs)
	{
		pos = offs;

		if (pos > length)
		{
			pos = length;
		}

		if (pos < 0)
		{
			pos = 0;
		}

		return pos;
	}

	void* GetPtr(int offs)const;
	bool IsOk()const;
	int GetBytes(void* ptr, int len);
	bool GetString(string& str, char term);

private:
	byte  *bits;
	int	length;
	int	pos;
	string file;						
};
