#include <winreg.h>
#pragma once

class CProtocol
{
public:
	CProtocol();
	~CProtocol();

	// Разбор URL
	bool	parse( LPCTSTR alpUrl);

};
