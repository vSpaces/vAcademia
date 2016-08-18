
#include "StdAfx.h"
#include <stdio.h>
#include "ColorConvert.h"

DWORD	GetDWORDFromRGBV(const std::string& asCommandValue, int& brightness, unsigned char& r,
						 unsigned char& g, unsigned char& b)
{
	unsigned int acolor[3];	float aval=0.0;
	acolor[0] = acolor[1] = acolor[2] = 0;
	int readedCount = sscanf_s( asCommandValue.c_str(), "%u %u %u %f", &acolor[0], &acolor[1], &acolor[2], &aval);
	r = (unsigned char)acolor[0];
	g = (unsigned char)acolor[1];
	b = (unsigned char)acolor[2];
	ATLASSERT( readedCount == 4);
	if( readedCount!=4)	return 0xFF000000;
	brightness = (int)aval;
	aval /= 100.0f;
	if ((acolor[0] != 0) || (acolor[1] != 0) || (acolor[2] != 0))
	for (int i=0; i<sizeof(acolor)/sizeof(acolor[0]); i++)
	{
		if( aval < 0.5f)
		{
			acolor[i] = (unsigned int)(acolor[i] * 2.0f * aval);
		}
		else
		{
			int tmp = (int)(acolor[i] * (1.0f + 2.0f * (aval - 0.5f)));
			if (tmp > 255)
			{
				tmp = 255;
			}
			acolor[i] = /*acolor[i] + (unsigned int)((255.0f - acolor[i]) * 2 * (aval - 0.5))*/tmp;
		}
	}

	return 0xFF000000 + (acolor[0]<<16) + (acolor[1]<<8) + acolor[2];
}
