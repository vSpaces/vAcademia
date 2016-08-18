#pragma once

#define USE_FREE_IMAGE 1

class VASE_API CTextureSaver
{
public:
	CTextureSaver();
	~CTextureSaver();

	bool CompressImageData(int pictureWidth, int pictureHeight, int channelsCount,  void* pictureData, int quality, VASEBitmap* apBitmap);
	void CleanUp();
	
private:
	int m_pictureID;
	void* m_dib;
};