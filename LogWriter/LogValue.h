#pragma once
#include <string>
#define MAX_INT_LENGTH		20
#define MAX_DOUBLE_LENGTH	50

#ifdef LOGWRITER_STATIC_LIB
#define LOGWRITER_API
#else

#ifdef LOGWRITER_EXPORTS
#define LOGWRITER_API __declspec(dllexport)
#else
#define LOGWRITER_API __declspec(dllimport)
#endif

#endif

class LOGWRITER_API CLogValue
{
	friend class CLogWriter;
	friend class CLogWriterLastMsgAsync;
public:
	CLogValue();
	CLogValue(int intType);
	CLogValue(__int64 uint64Type);
	CLogValue(unsigned __int64 uint64Type);
	CLogValue(unsigned int uintType);
	CLogValue(unsigned long ulongType);
	CLogValue(double doubleType);
	CLogValue(std::string stringType);
	CLogValue(std::wstring stringType);
	CLogValue(char charType);
	CLogValue(char * pszType);
	CLogValue(const char * pcszType);
	CLogValue(const wchar_t * pcszType);
	CLogValue(unsigned char charType);
	CLogValue(unsigned char * pszType);
	CLogValue(const char* aMessage, const unsigned char* aData, unsigned int aDataSize);
	CLogValue(void* ptrType);
	CLogValue(bool boolType);
	const char * GetData();
	~CLogValue();
	CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8, const CLogValue& obj9, const CLogValue& obj10);
	CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8, const CLogValue& obj9);
	CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8);

	CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7);
	CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6);
	CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5);
	CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4);
	CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3);
	CLogValue(const CLogValue& obj1, const CLogValue& obj2);
	CLogValue(const CLogValue& obj1);

	void Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8, const CLogValue& obj9, const CLogValue& obj10);
	void Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8, const CLogValue& obj9);
	void Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8);
	void Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7);
	void Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6);
	void Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5);
	void Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4);
	void Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3);
	void Reload(const CLogValue& obj1, const CLogValue& obj2);

private:
	std::string GetString() const;
	std::string m_string;
};
