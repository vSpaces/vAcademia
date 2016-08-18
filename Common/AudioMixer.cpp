#include "StdAfx.h"
#include "AudioMixer.h"
#include <memory.h>
#include <windef.h>

#define INT16_MIN        (-32767-1)
#define INT16_MAX        +32767

bool CAudioMixer::MixBuffers(const short** buffers, int bufferCount, int bufferSize, short* outputBuffer)
{
	if(buffers == NULL || outputBuffer == NULL || bufferCount <= 0 || bufferSize <= 0)
		return false;
	if (buffers[0] == NULL)
		memset(outputBuffer, 0, bufferSize * sizeof(short));
	else
		memcpy(outputBuffer, buffers[0], bufferSize * sizeof(short));
	for(int iBufIdx = 1; iBufIdx < bufferCount; iBufIdx++){
		const short* buffer = buffers[iBufIdx];
		if(buffer == NULL)
			continue;
		for(int i = 0; i < bufferSize; i++){
			short a = outputBuffer[i];
			short b = buffer[i];
			outputBuffer[i] = 
				// If both samples are negative, mixed signal must have an amplitude between the lesser of A and B, and the minimum permissible negative amplitude
				a < 0 && b < 0 ?
				((int)a + (int)b) - (((int)a * (int)b)/INT16_MIN) :

			// If both samples are positive, mixed signal must have an amplitude between the greater of A and B, and the maximum permissible positive amplitude
			( a > 0 && b > 0 ?
				((int)a + (int)b) - (((int)a * (int)b)/INT16_MAX)

				// If samples are on opposite sides of the 0-crossing, mixed signal should reflect that samples cancel each other out somewhat
				:a + b);
		}
	}
	return true;
}
