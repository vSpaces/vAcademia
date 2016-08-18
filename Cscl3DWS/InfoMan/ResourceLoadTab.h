#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "resource.h"       // main symbols
#include "ILogWriter.h"
#include "iasynchresourcemanager.h"
#include "..\SyncMan\SyncMan.h"
#include <vector>

struct CFloatPoint
{
	float x;
	float y;
};

struct viewResOnBitmap
{
	int id;
	int type;
	CFloatPoint sPoint;
};

class CResourceLoadTab:
	public CBaseDialogTab,
	public CDialogImpl<CResourceLoadTab>,
	public CUpdateUI<CResourceLoadTab>,
	public CDialogLayout<CResourceLoadTab>
{
public:
	CResourceLoadTab();
	~CResourceLoadTab()
	{
	}

	enum {IDD = IDD_RESOURCELOAD};

	BEGIN_LAYOUT_MAP()
		LAYOUT_CONTROL(IDC_RESOURCE_CHECK, LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_LEFT)
		//LAYOUT_CONTROL(IDC_IMAGE_BOX, LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_EDIT_OUTPUT, LAYOUT_ANCHOR_ALL)
	END_LAYOUT_MAP()

	BEGIN_UPDATE_UI_MAP(CResourceLoadTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CResourceLoadTab)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		//COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
		//COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
		COMMAND_HANDLER(IDC_RESOURCE_CHECK, BN_CLICKED, OnBnClickedCheckResourceLoad)
		CHAIN_MSG_MAP(CDialogLayout<CResourceLoadTab>)
	END_MSG_MAP()

	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/*LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//	EndDialog(wID);
		return 0;
	}*/
private:
	oms::omsContext *mpOmsContext;
	char* currentSession;
	bool m_building;
	bool mWorkFlag;
	CEdit mEditResourcesLoadingLog;
	CBitmap mBitmap;
	//CStatic mImageBox;
	resourceLoadStruct* mpResLoadStruct;
	MP_VECTOR<viewResOnBitmap> mResPoint;
	int mMapWidth, mMapHeight;
	float rd_x, rd_y, lu_x, lu_y;
	float koefXToMap, koefYToMap;

	void SetDownloadResourceText();

public:
	void SetContext( oms::omsContext *apContext);
	LRESULT OnBnClickedCheckResourceLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
};
