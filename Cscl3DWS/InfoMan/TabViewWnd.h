#pragma once
//#include "infoman.h"
//#include "infoManager.h"
#include "WTLTabViewCtrl.h"

class CTabViewWnd : public CWTLTabViewCtrl
{
public:
	CTabViewWnd();
	virtual ~CTabViewWnd();

	DECLARE_WND_SUPERCLASS(NULL, CWTLTabViewCtrl::GetWndClassName())
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CTabViewWnd)
		CHAIN_MSG_MAP(CWTLTabViewCtrl)
	END_MSG_MAP()
};