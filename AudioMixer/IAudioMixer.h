#ifndef __AUDIOMIXER_H_
#define __AUDIOMIXER_H_

#ifdef AUDIOMIXER_EXPORTS
#define AUDIOMIXER_API __declspec(dllexport)
#else
#define AUDIOMIXER_API __declspec(dllimport)
#endif

#ifdef WIN32
#define AUDIOMIXER_NO_VTABLE __declspec(novtable)
#else
#define AUDIOMIXER_NO_VTABLE
#endif

namespace vacademia
{
	typedef int errorMixer;	
	
	const static errorMixer NO_MIXER_ERROR = 0;
	const static errorMixer MIXER_ERROR1 = 1;
	const static errorMixer MIXER_ERROR2 = 2;
	const static errorMixer MIXER_ERROR3 = 3;

	struct MIX_CHANNEL_PARAMS
	{
		int param1;
	};

	struct /*AUDIOMIXER_NO_VTABLE*/ IAudioMixer
	{
		virtual errorMixer createMixChannel( MIX_CHANNEL_PARAMS*,  unsigned int *channedlID) = 0;
		virtual errorMixer deleteMixChannel(  unsigned int *channedlID) = 0;
		virtual errorMixer addSamples( unsigned int *channelID, BYTE *data, unsigned int dataSize/*bytes*/, unsigned int* samplesWrote) = 0;
	};	
}

#endif