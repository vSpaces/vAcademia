#include "stdafx.h"
#include "protocol.h"


CProtocol::CProtocol()
{
}

CProtocol::~CProtocol()
{
}

// Разбор URL
// vacademia://island1/#location_name?x=10&y=10&z=10&rx=0&ry=0&rz=0&ra=0&rn=234
bool	CProtocol::parse( LPCTSTR alpUrl)
{
	if( !alpUrl)	return false;
	
	CComString lower(alpUrl);	lower.MakeLower();
	if( (lower.Left( strlen( PROTOCOL_PREFIX)) == PROTOCOL_PREFIX) || lower.Left( strlen( PROTOCOL_PREFIX_ALT)) == PROTOCOL_PREFIX_ALT)
	{
		return true;	// не найден префикс протокола (vacademia://)
	}

	return false;
}

