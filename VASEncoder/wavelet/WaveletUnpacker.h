
#pragma once

#include "QualitySettings.h"
#include "WaveletUnpackRoutines.h"

class CWaveletUnpacker
{
public:
	CWaveletUnpacker();
	~CWaveletUnpacker();

	void SetQuality(unsigned char quality);
	unsigned char GetQuality()const;

	void SetOptions(unsigned int passCount, unsigned int uvInvScale, unsigned int quantizeKoef);
	void GetOptions(unsigned int& passCount, unsigned int& uvInvScale, unsigned int& quantizeKoef);

	void SetImageData(unsigned char* data, unsigned int width, unsigned int height);
	virtual bool Unpack(bool abEasyPacked = false);

	unsigned char* GetRGB()const;
	unsigned int GetDataSize()const;

	unsigned int GetWidth()const;
	unsigned int GetHeight()const;

private:
	void AllocateBufferIfNeeded(int bufferSize);
	void CleanUp();

	unsigned char* m_data;
	unsigned int m_width;
	unsigned int m_height;

	unsigned int m_passCount;
	unsigned int m_uvInvScale;
	unsigned int m_quantizeKoef;

	unsigned char* m_rgb;
	unsigned int m_lastBufferSize;

	CWaveletUnpackRoutines wlUnpackRoutines;

	CQualitySettings m_qualitySettings;
};