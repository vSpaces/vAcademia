#include "StdAfx.h"
#include <time.h>
#include <string>
#include "LogValue.h"
#include <atlbase.h>

#define MAX_INT_LENGTH		20
#define MAX_INT64_LENGTH	35
#define MAX_DOUBLE_LENGTH	50

CLogValue::CLogValue()
{
	m_string = "";
}

CLogValue::CLogValue(bool boolType)
{
	m_string = boolType ? "true" : "false";
}

CLogValue::CLogValue(int intType)
{
	char buf[MAX_INT_LENGTH];
	sprintf_s(&buf[0], MAX_INT_LENGTH, "%d\0", intType);
	m_string = buf;
}

CLogValue::CLogValue(const wchar_t * pcwzType)
{
	USES_CONVERSION;
	m_string = W2A( pcwzType);
}

CLogValue::CLogValue(std::wstring stringType)
{
	USES_CONVERSION;
	m_string = W2A( stringType.c_str());
}

CLogValue::CLogValue(unsigned int uintType)
{
	char buf[MAX_INT_LENGTH];
	sprintf_s(&buf[0], MAX_INT_LENGTH, "%u\0", uintType);
	m_string = buf;
}

CLogValue::CLogValue(__int64 uint64Type)
{
	char buf[MAX_INT64_LENGTH];
	_i64toa_s(uint64Type, buf, MAX_INT64_LENGTH, 10);
	m_string = buf;
}

CLogValue::CLogValue(unsigned __int64 uint64Type)
{
	char buf[MAX_INT64_LENGTH];
	_i64toa_s(uint64Type, buf, MAX_INT64_LENGTH, 10);
	m_string = buf;
}

CLogValue::CLogValue(unsigned long ulongType)
{
	char buf[MAX_INT_LENGTH];
	sprintf_s(&buf[0], MAX_INT_LENGTH, "%u\0", ulongType);
	m_string = buf;
}

CLogValue::CLogValue(double doubleType)
{
	char buf[MAX_DOUBLE_LENGTH];
	sprintf_s(&buf[0], MAX_DOUBLE_LENGTH, "%f\0", doubleType);
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

CLogValue::CLogValue(void* ptrType)
{
	char buf[MAX_INT_LENGTH];
	m_string = "0x";
	m_string += _itoa_s((int)ptrType, buf, MAX_INT_LENGTH, 16);
}

CLogValue::CLogValue(const char* aMessage, const unsigned char* aData, unsigned int aDataSize)
{
	const char s1[] = "\n===========\n";
	const char s2[] = "Binary data size: %d\n";

	unsigned int twoSpaceShift = 8;
	unsigned int newLineShift = 40;

	unsigned int bufferLength = aDataSize * 3 //значение в 2 шестнадцатиричных цифры и пробел(или перевод строки)
		+ aDataSize / twoSpaceShift //по 1 доп пробелу каждые twoSpaceShift
		+ 1/*ошибки округлени€*/ 
		+ strlen(aMessage)
		+ 2 * strlen(s1) 
		+ strlen(s2) 
		+ 10/*число байт - значение aDataSize*/ 
		+1; // завершающий 0

	char* buffer = MP_NEW_ARR( char, bufferLength);
	char* p = buffer;
	memset(buffer, 0, bufferLength);
	int len = 0;
	int elapsedBufferLength = bufferLength;

	strcat_s( p, elapsedBufferLength, aMessage);
	strcat_s( p, elapsedBufferLength, s1);
	len  = strlen( p);
	elapsedBufferLength -= len;
	p += len;
	sprintf_s( p, elapsedBufferLength, s2, aDataSize);
	len  = strlen( p);
	elapsedBufferLength -= len;
	p += len;

	unsigned int i = 0;
	for (unsigned int j = 1, k = 1;  i < aDataSize;  i++, j++, k++)
	{
		sprintf_s( p, elapsedBufferLength, "%02X", aData[ i]);
		if ((j % newLineShift) == 0)
		{
			strcat_s( p, elapsedBufferLength, "\n");
			j = 0;
			k = 0;
		}
		else if ((k % twoSpaceShift) == 0)
		{
			strcat_s( p, elapsedBufferLength, "  ");
			k = 0;
		}
		else
			strcat_s( p, elapsedBufferLength, " ");
		len  = strlen( p);
		elapsedBufferLength -= len;
		p += len;
	}

	strcat_s( p, elapsedBufferLength, s1);
	len  = strlen( p);
	elapsedBufferLength -= len;
	p += len;

	m_string = buffer;

	int realLength = m_string.size();
	int calculatedLength = p - buffer;
	ATLASSERT( calculatedLength < bufferLength);
	ATLASSERT( realLength == calculatedLength);

	MP_DELETE_ARR( buffer);
}

const char * CLogValue::GetData()
{
	return m_string.c_str();
}

CLogValue::~CLogValue()
{
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8, const CLogValue& obj9, const CLogValue& obj10)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string + obj9.m_string + obj10.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8, const CLogValue& obj9)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string + obj9.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1, const CLogValue& obj2)
{
	m_string = obj1.m_string + obj2.m_string;
}

CLogValue::CLogValue(const CLogValue& obj1)
{
	m_string = obj1.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8, const CLogValue& obj9, const CLogValue& obj10)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string + obj9.m_string + obj10.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8, const CLogValue& obj9)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string + obj9.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7, const CLogValue& obj8)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string + obj8.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6, const CLogValue& obj7)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string + obj7.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5, const CLogValue& obj6)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string + obj6.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4, const CLogValue& obj5)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string + obj5.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3, const CLogValue& obj4)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string + obj4.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2, const CLogValue& obj3)
{
	m_string = obj1.m_string + obj2.m_string + obj3.m_string;
}

void CLogValue::Reload(const CLogValue& obj1, const CLogValue& obj2)
{
	m_string = obj1.m_string + obj2.m_string;
}

std::string CLogValue::GetString() const
{
	return m_string;
}
