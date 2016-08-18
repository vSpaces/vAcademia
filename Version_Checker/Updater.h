#include "..\Launcher\StartUpdater.h"

class CUpdater: private CStartUpdater
{
public:
	CUpdater(const HWND &hWnd);
	~CUpdater();
	static wstring GetApplicationDataDirectory();
	void Update()const;
private:
	HWND GetHandle()const;
	HWND m_handle;
};