#pragma once

#define  JPEG_DEFAULT_NAME			"1.jpg"
#define  PNG_DEFAULT_NAME			"1.png"

class VASWaveletEncoder;


class CTextureLoader
{
public:
	CTextureLoader();
	~CTextureLoader();

	bool LoadFromMemory(void* pictureData, unsigned int dataSize, std::string fileName = JPEG_DEFAULT_NAME);
#ifdef VAS_ENCODER
	bool LoadFromMemoryNewCodec(void* pictureData, unsigned int dataSize, CSize pictureSize, bool abEasyPacked = false);
#endif
	void CleanUp();
#ifdef VAS_ENCODER
	void InitNewCodec(void* taskManager, oms::omsContext* aContext);
#endif
	void Resize(int width, int height);

	unsigned char GetChannelCount()const;
	unsigned int GetWidth()const;
	unsigned int GetHeight()const;

	void* GetData()const;
	unsigned int GetDataSize()const;

	bool IsDirectChannelOrder()const;
	bool IsInverseChannelOrder()const;


	unsigned int GetWaveletsWidth()const;
	unsigned int GetWaveletsHeight()const;

	void* GetWaveletsData()const;
	unsigned int GetWaveletsDataSize()const;

private:
#ifdef VAS_ENCODER
	VASWaveletEncoder* waveletEncoder;
#endif
protected:
	void OnLoaded(bool isBmp);

	void* m_dib;
	bool reverseRGB;

	void* m_waveletsDIB;
	unsigned int m_waveletsWidth;
	unsigned int m_waveletsHeight;
	unsigned int m_waveletsPictureSize;
};