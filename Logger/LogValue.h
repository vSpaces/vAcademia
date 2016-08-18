#pragma once
#include <string>
//#include "HelperFunctions.h"
#define MAX_INT_LENGTH 12
#define MAX_DOUBLE_LENGTH 24

class CLogValue
{
public:
	CLogValue::CLogValue()
	{
		m_string = "";
	}

	CLogValue::CLogValue(int intType)
	{
		char buf[MAX_INT_LENGTH];
		sprintf(&buf[0], "%d\0", intType);
		m_string = buf;
	}

	CLogValue::CLogValue(unsigned int uintType)
	{
		char buf[MAX_INT_LENGTH];
		sprintf(&buf[0], "%u\0", uintType);
		m_string = buf;
	}

	CLogValue::CLogValue(void* ptrType)
	{
		char buf[MAX_INT_LENGTH];
		m_string = "0x";
		m_string += itoa((int)ptrType, buf, 16);
	}

	CLogValue::CLogValue(double doubleType)
	{
		char buf[MAX_DOUBLE_LENGTH];
		sprintf(&buf[0], "%f\0", doubleType);
		m_string = &buf[0];
	}

	CLogValue::CLogValue(std::string stringType)
	{
		m_string = stringType;
	}

	CLogValue::CLogValue(char charType)
	{
		m_string = charType;
	}

	CLogValue::CLogValue(char * pszType)
	{
		m_string = pszType;
	}

	CLogValue::CLogValue(const char * pcszType)
	{
		m_string = pcszType;
	}

	CLogValue::CLogValue(unsigned char charType)
	{
		m_string = charType;
	}

	CLogValue::CLogValue(unsigned char * pszType)
	{
		m_string = (char *)pszType;
	}

	CLogValue::CLogValue(const unsigned char* aData, unsigned int aDataSize)
	{
		const char s1[] = "===========\n";
		const char s2[] = "Binary data size: %d\n";

		unsigned int bufferLength = aDataSize * 3 
									+ aDataSize / 8 
									+ aDataSize / 40 
									+ 2/*ошибки округления*/ 
									+ 2 * strlen(s1) 
									+ strlen(s2) 
									+ 10/*число байт*/ 
									+ 8/*переводы строки строк s1 и s2*/;

		char* buffer = new char[ bufferLength];
		char* p = buffer;
		*p = 0;

		strcat( p, s1);
		sprintf( p, s2, aDataSize);
		p += strlen( p);

		for (unsigned int i = 0;  i < aDataSize;  i++)
		{
			sprintf( p, "%02X", aData[ i]);
			if ((i % 40) == 0)
				strcat( p, "\n");
			else if ((i % 8) == 0)
				strcat( p, "  ");
			else
				strcat( p, " ");
			p += strlen( p);
		}

		strcat( p, s1);
		m_string = buffer;

		int ii = p - buffer;
		ATLASSERT( ii < bufferLength);

		delete[] buffer;
	}

	const char * CLogValue::GetData()
	{
		return m_string.c_str();
	}

	CLogValue::~CLogValue()
	{
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string + obj9.m_string + obj10.m_string;
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string + obj9.m_string;
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string;
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string;
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string;
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string;
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string;
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2, CLogValue obj3)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string;
	}

	CLogValue::CLogValue(CLogValue obj1, CLogValue obj2)
	{
		m_string = obj1.m_string + obj2.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string + obj9.m_string + obj10.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string + obj9.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2, CLogValue obj3)
	{
		m_string = obj1.m_string + obj2.m_string + obj3.m_string;
	}

	void CLogValue::Reload(CLogValue obj1, CLogValue obj2)
	{
		m_string = obj1.m_string + obj2.m_string;
	}

	std::string CLogValue::GetString()
	{
		return m_string;
	}

private:

	std::string m_string;
};
