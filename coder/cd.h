#pragma once

#define	CODE_VALUE		0xEAD5

class CVipCoder
{
	CVipCoder(){}
	~CVipCoder(){}

public:
	static void	decompMem(void* src, void* dest, int count)
	{
		int wrote=0;
		while( count > 0)
		{
			int bt = sizeof(DWORD);
			if( count < bt)
			{
				bt = count;
			}
			DWORD	val;
			memcpy(&val, (byte*)((DWORD)src+wrote), bt);
			val ^= CODE_VALUE;
			memcpy((byte*)((DWORD)dest+wrote), &val, bt);
			count -= bt;
			wrote+=bt;
		}
	}
};