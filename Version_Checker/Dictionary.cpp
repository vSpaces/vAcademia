#include "stdafx.h"
#include "Dictionary.h"
#include <Shellapi.h>
#include <stdio.h>
#include "AssistantModule.h"

#define LANG_CAPACITY 19

CDictionary:: CDictionary() {}

CDictionary:: ~CDictionary() {}

CDictionary* CDictionary:: GetInstance()
{
	static CDictionary* dict = NULL;
	if (!dict)
	{
		dict = new CDictionary();
	}
	return dict;
}

void CDictionary:: SetLanguage(const wstring &lang)
{
	if(lang == L"rus")
	{
			m_language = L"[Russian]";
			setlocale(LC_ALL, "Russian");
	}else
	{
		m_language = L"[English]";
	}
}

wstring CDictionary::GetText(const int &key)const
{
	vector<wstring> tmp = m_dictionary;
	return m_dictionary[key];
}

bool CDictionary::FillDictionary()
{
	const unsigned int maxLen = 512;
	wstring rootDir = GetRootDir();
	rootDir += L"\\dict.txt";
	
	FILE *file;
	_wfopen_s(&file, rootDir.c_str(), L"r");
	if(file)
	{
		wchar_t buf[maxLen];
		wstring str;
		while(str != m_language)
		{
			wchar_t* a = fgetws(buf, maxLen, file);
			buf[wcslen(buf) - 1] = '\0';
			str = buf;			
		}
		for(int k = 0; k < LANG_CAPACITY; k++)
		{
			fgetws(buf, maxLen, file);
			buf[wcslen(buf) - 1] = '\0';
			str = buf;
			m_dictionary.push_back(str);
		}
		fclose(file);
	}
	return(m_dictionary.size() != 0);
}