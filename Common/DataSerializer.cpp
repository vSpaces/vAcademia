#include "StdAfx.h"
#include ".\dataserializer.h"

CDataSerializerIn::CDataSerializerIn(CDataBuffer2& aData) : data( aData)
{
}

CDataSerializerIn::~CDataSerializerIn()
{
}

bool CDataSerializerIn::Serialize( char& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( unsigned char& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( short& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( unsigned short& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( __int32& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( unsigned __int32& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( __int64& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( unsigned __int64& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( float& number)
{
	return data.read( number);
}

bool CDataSerializerIn::Serialize( double& number)
{
	return data.read( number);
}

bool CDataSerializerIn::SerializeString( string& str )
{
	return data.readString( str);
}

bool CDataSerializerIn::SerializeString( wstring& str )
{
	return data.readString( str);
}

bool CDataSerializerIn::SerializeLargeString( string& str )
{
	return data.readLargeString( str);
}

CDataSerializerOut::CDataSerializerOut( CDataBuffer2& aData ) : data( aData)
{

}

CDataSerializerOut::~CDataSerializerOut()
{

}

bool CDataSerializerOut::Serialize( char& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( unsigned char& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( short& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( unsigned short& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( __int32& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( unsigned __int32& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( __int64& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( unsigned __int64& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( float& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::Serialize( double& number )
{
	data.add( number);
	return true;
}

bool CDataSerializerOut::SerializeString( string& str )
{
	data.addString( str);
	return true;
}

bool CDataSerializerOut::SerializeString( wstring& str )
{
	data.addString( str);
	return true;
}

bool CDataSerializerOut::SerializeLargeString( string& str )
{
	data.addLargeString( str);
	return true;
}