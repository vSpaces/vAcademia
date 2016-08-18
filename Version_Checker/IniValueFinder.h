#include <string>

class CIniValueFinder
{
public:
	CIniValueFinder(const wstring &iniFileName);
	~CIniValueFinder();

	void SetPath(const wstring &iniFileName);
	void SetValue(const wchar_t* section, const wchar_t* key, const wchar_t* value);

	string GetValue(const wchar_t* section, const wchar_t* key)const;
	wstring GetPath()const;

private:
	wstring m_path;
};