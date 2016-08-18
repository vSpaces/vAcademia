#pragma once

class CProcessWindowFinder
{
public:
	CProcessWindowFinder();
	~CProcessWindowFinder();

	// Разбор URL
	HWND	get_process_hwnd( const CComString& asExePath);
};
