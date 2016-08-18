
#pragma once

#define MAX_BUF_SIZE		16384

class CSynchBuffer
{
public:
	CSynchBuffer();
	~CSynchBuffer();

	void Clear();
	void AddData(void* data, int size);
	void AddUnsignedInt(unsigned int a);
	void AddUnsignedChar(unsigned char a);
	void AddUnsignedShort(unsigned short a);

	unsigned char* GetData();
	int GetDataSize();

private:
	unsigned char m_buf[MAX_BUF_SIZE];
	int m_size;
};