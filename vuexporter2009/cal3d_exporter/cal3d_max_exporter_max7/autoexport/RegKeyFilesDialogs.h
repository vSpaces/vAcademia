#pragma once

//#include "ComString.h"
#include <vector>

class CRegManager
{
	public:
		CRegManager();
		~CRegManager();
		BOOL ReadValue(LPCTSTR pName, CString& ret);
		void WriteValue(LPCTSTR pName,LPCTSTR pValue);
	private:
		struct Sreglist
		{
			CString Name;
			CString Value;
		};
		std::vector <Sreglist> reglist;
		int FindInVector(CString Name);
};

