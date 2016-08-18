
#pragma once

#define PBO_USAGE_READ_TEXTURE	1
#define PBO_USAGE_WRITE_TEXTURE	2

class CPBO
{
public:
	CPBO(unsigned int width, unsigned int height, unsigned char channelCount, bool isDirectChannelOrder, unsigned char usage);
	~CPBO();

	void Capture(int level = 0, int offset = 0);
	void Uncapture();

	void* GetPointer();
	void ReleasePointer();

private:
	unsigned int GetPixelFormat()const;

	unsigned char m_usage;
	unsigned char m_channelCount;

	unsigned int m_bufferID;
	unsigned int m_width;
	unsigned int m_height;

	bool m_isDirectChannelOrder;
};