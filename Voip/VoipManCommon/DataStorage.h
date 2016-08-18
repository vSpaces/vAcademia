#pragma once

class CDataStorage
{
public:	
	CDataStorage(unsigned int size);
	CDataStorage( BYTE *aData, unsigned int aSize);
	~CDataStorage();

	template<class Type>
		bool Add(Type& val)
		{
			if (!IsEnoughSpace(m_position, sizeof(val)))
			{
				ResizeStorage(2*(m_size + sizeof(val)));
			}

			memcpy(&m_data[m_position], &val, sizeof(val));
			m_position += sizeof(val);

			return true;
		}

	bool Add( const char *val);
	bool Add( char *val);
	bool Add(std::string& val);
	bool Add( const wchar_t *val);
	bool Add( wchar_t *val);
	bool Add(std::wstring& val);

	template<class Type>
		bool Read(Type& val)
		{
			if (!IsEnoughSpace(m_position, sizeof(val)))
			{
				return false;
			}

			memcpy(&val, &m_data[m_position], sizeof(val));
			m_position += sizeof(val);

			return true;
		}

	bool Read(std::string& val);
	bool Read(std::wstring& val);
#ifdef	MEMORY_PROFILING
	bool Read(MP_STRING& val);
	bool Read(MP_WSTRING& val);
#endif
	BYTE* GetData();

	unsigned int GetSize();
	unsigned int GetRealDataSize();

	void Reset(int newPos = 0);
	
	void Create(unsigned int size);

	void SetObjectName(std::string name);
	std::string GetObjectName()const;

	unsigned char GetCommandID()const;

private:
	bool IsEnoughSpace(unsigned int position, int dataSize);	
	void ResizeStorage(unsigned int newSize);
	void Destroy();

	MP_STRING m_name;

	BYTE* m_data;
	unsigned int m_position;	
	unsigned int m_size;
};
