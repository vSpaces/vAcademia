
#pragma once

class CWaveletHeader
{
public:
	CWaveletHeader();
	~CWaveletHeader();

	void SetDataPointer(unsigned char* header);

	void SetYPartCount(unsigned char partCount);
	unsigned char GetYPartCount()const;

	void SetYPartSize(unsigned char partID, unsigned int partSize);
	unsigned int GetYPartSize(unsigned char partID)const;

	void SetUSize(unsigned int uSize);
	unsigned int GetUSize()const;

	void SetYUVSize(unsigned int yuvSize);
	unsigned int GetYUVSize()const;

	void SetWidth(unsigned short width);
	unsigned short GetWidth()const;

	void SetHeight(unsigned short height);
	unsigned short GetHeight()const;

	void SetQuality(unsigned short quality);
	unsigned short GetQuality()const;

	unsigned int GetHeaderSize()const;
    
private:
	unsigned char* m_data;
	unsigned char m_yPartCount;
};