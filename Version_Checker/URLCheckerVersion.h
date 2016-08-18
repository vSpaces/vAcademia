#include <string>

class CURLCheckerVersion
{
public:
	CURLCheckerVersion();
	~CURLCheckerVersion();
	bool Check(const string &url, const string &ini, const string &buildType);
	string GetVersion()const;
	unsigned int GetUpdateType()const;
	bool IsServerNotAvailable()const;
private:
	string m_version;
	unsigned int m_update, m_isNotAvailable;
	void ParseData(const string &data); 
};

string GetIP(const string &url);