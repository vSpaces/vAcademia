// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AUDIOMIXER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AUDIOMIXER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include "IAudioMixer.h"

namespace vacademia
{
	// This class is exported from the AudioMixer.dll
	class AUDIOMIXER_API CAudioMixer : public IAudioMixer {
	public:
		CAudioMixer();
		errorMixer createMixChannel( MIX_CHANNEL_PARAMS*,  unsigned int *channedlID);
		errorMixer deleteMixChannel(  unsigned int *channedlID);
		errorMixer addSamples( unsigned int *channelID, BYTE *data, unsigned int dataSize/*bytes*/, unsigned int* samplesWrote);
		// TODO: add your methods here.
	};		

AUDIOMIXER_API IAudioMixer* getIAudioMixer();

}
