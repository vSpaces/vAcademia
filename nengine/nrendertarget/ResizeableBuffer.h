
#pragma once

#define BUFFER_SIZE_DELTA	10000

class CResizeableBuffer
{
public:
	CResizeableBuffer();
	~CResizeableBuffer();

	void AddData(void* data, int size);
	bool PopData(void* data, int size);
	void Clear();
	void Truncate(int size);

	int GetSize()const;
	void* GetPointer()const;

private:
	void* m_data;
	int m_size;
	int m_dataSize;
};