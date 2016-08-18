#pragma once

#include "DataBuffer2.h"

struct IDataSerializer
{
	virtual bool Serialize(char& number) = 0;
	virtual bool Serialize(unsigned char& number) = 0;
	virtual bool Serialize(short& number) = 0;
	virtual bool Serialize(unsigned short& number) = 0;
	virtual bool Serialize(__int32& number) = 0;
	virtual bool Serialize(unsigned __int32& number) = 0;
	virtual bool Serialize(__int64& number) = 0;
	virtual bool Serialize(unsigned __int64& number) = 0;
	virtual bool Serialize(float& number) = 0;
	virtual bool Serialize(double& number) = 0;
	virtual bool SerializeString( string& str) = 0;
	virtual bool SerializeString( wstring& str) = 0;
	virtual bool SerializeLargeString( string& str) = 0;
};

class CDataSerializerIn : public IDataSerializer
{
public:
	CDataSerializerIn(CDataBuffer2& aData);
	virtual ~CDataSerializerIn();
	virtual bool Serialize(char& number);
	virtual bool Serialize(unsigned char& number);
	virtual bool Serialize(short& number);
	virtual bool Serialize(unsigned short& number);
	virtual bool Serialize(__int32& number);
	virtual bool Serialize(unsigned __int32& number);
	virtual bool Serialize(__int64& number);
	virtual bool Serialize(unsigned __int64& number);
	virtual bool Serialize(float& number);
	virtual bool Serialize(double& number);
	virtual bool SerializeString( string& str);
	virtual bool SerializeString( wstring& str);
	virtual bool SerializeLargeString( string& str);
private:
	CDataBuffer2& data;
};

class CDataSerializerOut : public IDataSerializer
{
public:
	CDataSerializerOut(CDataBuffer2& aData);
	virtual ~CDataSerializerOut();
	virtual bool Serialize(char& number);
	virtual bool Serialize(unsigned char& number);
	virtual bool Serialize(short& number);
	virtual bool Serialize(unsigned short& number);
	virtual bool Serialize(__int32& number);
	virtual bool Serialize(unsigned __int32& number);
	virtual bool Serialize(__int64& number);
	virtual bool Serialize(unsigned __int64& number);
	virtual bool Serialize(float& number);
	virtual bool Serialize(double& number);
	virtual bool SerializeString( string& str);
	virtual bool SerializeString( wstring& str);
	virtual bool SerializeLargeString( string& str);
private:
	CDataBuffer2& data;
};
