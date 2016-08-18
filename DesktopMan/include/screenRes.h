#pragma once

bool IsLargeFonts()
{
	HDC hDC = GetDC(0);
	int dpp = GetDeviceCaps( hDC, LOGPIXELSY);
	ReleaseDC( 0, hDC);
	if( dpp > 96)
		return true;
	return false;
}