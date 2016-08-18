#include "stdafx.h"
#include "UrlBuilder.h"

#define FORMAT_BUFFER_LENGTH 1024
wchar_t urlBuilderFormatBuffer[ FORMAT_BUFFER_LENGTH];

// vacademia:///?x=-40852.4&y=-4734.5&z=462.5&rn=2506&ra=0&rx=0&ry=0&rz=0
CUrlBuilder::CUrlBuilder()
{
	x = y = z = 0.0f;
	ra = rx = ry = rz = 0.0f;
	realityId = 0;
	coordsIsSet = realityIdIsSet = locationNameIsSet = false;
}

CUrlBuilder::~CUrlBuilder()
{
	
}

// return location string
std::wstring	CUrlBuilder::toString()
{
	setlocale( LC_ALL, "English_Britain" );

	bool exceptOneParamExists = false;
	std::wstring sUrl = L"vacademia:///?";

	if( locationNameIsSet)
	{
		/*sUrl += L"location=";
		sUrl += locationName;
		exceptOneParamExists = true;*/
	}

	if( coordsIsSet)
	{
		if( exceptOneParamExists)
			sUrl += L"&";

		swprintf( urlBuilderFormatBuffer, L"x=%.2f&y=%.2f&z=%.2f", x, y, z);
		sUrl += urlBuilderFormatBuffer;
		exceptOneParamExists = true;
	}

	if( realityIdIsSet)
	{
		if( exceptOneParamExists)
			sUrl += L"&";

		swprintf( urlBuilderFormatBuffer, L"rn=%d", realityId);
		sUrl += urlBuilderFormatBuffer;
		exceptOneParamExists = true;
	}

	if( rotationsIsSet)
	{
		if( exceptOneParamExists)
			sUrl += L"&";

		swprintf( urlBuilderFormatBuffer, L"ra=%.2f&rx=%.2f&ry=%.2f&rz=%.2f", ra, rx, ry, rz);
		sUrl += urlBuilderFormatBuffer;
		exceptOneParamExists = true;
	}	

	setlocale(LC_ALL, "Russian");

	return sUrl;
}

// set URL params
/*void	CUrlBuilder::setLocation( wchar_t* apwcLocationName)
{
	if( !apwcLocationName)
		return;
	locationName = apwcLocationName;
	locationNameIsSet = true;
}*/

void	CUrlBuilder::setCoords( float ax, float ay, float az)
{
	x = ax;
	y = ay;
	z = az;
	coordsIsSet = true;
}

void	CUrlBuilder::setRotations( float ara, float arx, float ary, float arz)
{
	ra = ara;
	rx = arx;
	ry = ary;
	rz = arz;
	rotationsIsSet = true;
}

void	CUrlBuilder::setReality( unsigned int auRealityId)
{
	realityId = auRealityId;
	realityIdIsSet = true;
}