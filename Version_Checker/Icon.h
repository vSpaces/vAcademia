#include <shellapi.h>
#include <string>

class CIcon
{
public:
	CIcon(const HWND &hwn, const HICON &hIcon, const UINT &uID, const wstring &text);
	~CIcon();
	void SetMessage(const wstring &text);
private:
	NOTIFYICONDATAW m_nid;
};