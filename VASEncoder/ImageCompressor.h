#pragma once

#define USE_FREE_IMAGE 1

class VASE_API CImageCompressor
{
public:
	CImageCompressor();
	~CImageCompressor();

	bool CompressImageData(int pictureWidth, int pictureHeight, int channelsCount,  void* pictureData, int quality, VASEBitmap* apBitmap);
	void CleanUp();
	
private:
	void* m_dib;
};