#pragma once

#include "../CommonRenderManagerHeader.h"

class CMapServerPacketOutBase
{
public:
	CMapServerPacketOutBase();
	CMapServerPacketOutBase(int reserveSize);
	virtual ~CMapServerPacketOutBase();

	//void SetPackageID(int packageID);

	void Add(unsigned long value);
	void Add(unsigned int value);
	void Add(unsigned short value);
	void Add(unsigned char value);
	void Add(float value);
	void Add(int value);
	void Add(short value);
	void Add(char value);
	void Add(std::string& value);
	void Add(CComString& value);
	void Add(const wchar_t* apwcValue);
	void Add(char* pBuffer, int size);
	void Add(unsigned char* pBuffer, int size);

	const unsigned char* GetData() const;
	int GetDataSize() const;

private:
	void Init(int packageID);

	MP_VECTOR<unsigned char> m_buffer;
	int m_count;
};