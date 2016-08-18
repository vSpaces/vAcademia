// MRU.h: interface for the CMRU class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MRU_H__66D02B3C_2C16_43D5_B478_01624BB7EFF5__INCLUDED_)
#define AFX_MRU_H__66D02B3C_2C16_43D5_B478_01624BB7EFF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMRU : public CStringList  
{
	CString m_sFullFileName;
public:
	CMRU();
	virtual ~CMRU();
	void Load(LPCSTR alpcName);
	void UpdateAndSave(LPCSTR alpcLastUsedItem);
};

#endif // !defined(AFX_MRU_H__66D02B3C_2C16_43D5_B478_01624BB7EFF5__INCLUDED_)
