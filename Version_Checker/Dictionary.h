#pragma once
#include <string>
#include <vector>

#define TITLE 0
#define INCORRECT_DATA 1
#define NEW_UPDATE 2
#define CURR_VER 3
#define Q_UPDATE 4
#define NO_NEW_UPDATES 5
#define DONT_ASK_ME 6
#define CANT_GET_PARAMS 7
#define UPDATE_FROM_SITE 8
#define ERROR_HAS_OCCURED 9
#define FAILED_UPDATER 10
#define YES 11
#define NO 12
#define DESIRED_UPDATE 13
#define REQUIRED_UPDATE 14
#define REQ_FULL_UPDATE 15
#define CURR_VER_CANT_LOGIN 16
#define UPDATE 17
#define NEW_VER 18

class CDictionary
{
public:
	CDictionary();
	~CDictionary();

	static CDictionary* GetInstance();
	void SetLanguage(const wstring &lang);
	bool FillDictionary();
	wstring GetText(const int &key)const;
private:
	vector<wstring> m_dictionary;
	wstring m_language;
};