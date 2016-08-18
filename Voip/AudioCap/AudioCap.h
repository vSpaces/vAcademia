// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AUDIOCAP_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AUDIOCAP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef AUDIOCAP_EXPORTS
#define AUDIOCAP_API __declspec(dllexport)
#else
#define AUDIOCAP_API __declspec(dllimport)
#endif

// This class is exported from the AudioCap.dll
class AUDIOCAP_API CAudioCap {
public:
	CAudioCap(void);
	// TODO: add your methods here.
};

extern AUDIOCAP_API int nAudioCap;

AUDIOCAP_API int fnAudioCap(void);
