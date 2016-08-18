#include <string>
#include <fstream>

class CCfgFile
{
public:
	CCfgFile(const string &iniFileName);
	~CCfgFile();
	string GetVersion()const;
private:
	string m_version;
};