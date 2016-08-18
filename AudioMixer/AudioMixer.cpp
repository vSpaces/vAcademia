// AudioMixer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "AudioMixer.h"

using namespace vacademia;

#pragma data_seg("Shared")
#pragma data_seg()

__declspec(allocate("Shared")) CAudioMixer audioMixer;

#pragma comment(linker, "/Section.Shared,RWS")

// This is an example of an exported variable
//AUDIOMIXER_API int nAudioMixer=0;

// This is an example of an exported function.
AUDIOMIXER_API IAudioMixer* getIAudioMixer()
{
	return (IAudioMixer*)&audioMixer;
}

CAudioMixer::CAudioMixer()
{
}

errorMixer CAudioMixer::createMixChannel( MIX_CHANNEL_PARAMS*, unsigned int* channedlID)
{
	return 0;
}

errorMixer CAudioMixer::deleteMixChannel( unsigned int *channedlID)
{
	return 0;
}

errorMixer CAudioMixer::addSamples( unsigned int *channelID, BYTE *data, unsigned int dataSize/*bytes*/, unsigned int* samplesWrote)
{
	return 0;
}
