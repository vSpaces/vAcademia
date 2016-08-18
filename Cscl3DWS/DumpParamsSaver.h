#pragma once

class CDumpParamsSaver
{
public:
	CDumpParamsSaver(void);
	virtual ~CDumpParamsSaver(void);
	BOOL set( WCHAR *keyName, WCHAR *sValue);

protected:
	std::wstring getApplicationDataDirectory();

private:
	CWComString m_sFileName;
};
