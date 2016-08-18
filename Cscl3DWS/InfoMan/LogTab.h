#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "objectTree.h"
#include "ILogWriter.h"
#include <map>

class CLogTab :
	public CBaseDialogTab,
	public CDialogImpl<CLogTab>,
	public CUpdateUI<CLogTab>,
	public CDialogLayout<CLogTab>
{
public:
	CLogTab();
	virtual ~CLogTab();

	enum { IDD = IDD_LOGVIEW};

	BEGIN_LAYOUT_MAP()
		LAYOUT_CONTROL(IDC_GROUP_FILTERS, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_RICHEDIT_LOG, LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_BUTTON_FIND, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_RIGHT)
		LAYOUT_CONTROL(IDC_CHECK_AUTOUPDATE, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_EDIT_FIND, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_RIGHT)

		LAYOUT_CONTROL(IDC_CHECK_DEFAULT, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_SOUND_MAN, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_RENDER_MAN, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_COM_MAN, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_AVATAR_EDITOR, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)

		LAYOUT_CONTROL(IDC_CHECK_INFO_MAN, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_MDUMP, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_NTEXTURE, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_OMSPLAYER, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_PATH_REFINER, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_RESLOADER, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_RMML, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_SERVICE_MAN, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_SYNC_MAN, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_VASENCODER, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_VIDEO, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_NPHYSICS, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_VOIP_MAN, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)

		LAYOUT_CONTROL(IDC_CHECK_SOME_LOG_TYPE1, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_SOME_LOG_TYPE2, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)
		LAYOUT_CONTROL(IDC_CHECK_SOME_LOG_TYPE3, LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT)

	END_LAYOUT_MAP()

public:
	BEGIN_UPDATE_UI_MAP(CLogTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CLogTab)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON_FIND, BN_CLICKED, OnBnClickedButtonFind)
		COMMAND_HANDLER(IDC_CHECK_AUTOUPDATE, BN_CLICKED, OnBnClickedCheckAutoUpdate)
		COMMAND_HANDLER(IDC_GROUP_FILTERS, BN_CLICKED, OnBnClickedGroupFilters)
		COMMAND_HANDLER(IDC_CHECK_SOUND_MAN, BN_CLICKED, OnBnClickedCheckSoundMan)
		COMMAND_HANDLER(IDC_CHECK_RENDER_MAN, BN_CLICKED, OnBnClickedCheckRenderMan)
		COMMAND_HANDLER(IDC_CHECK_COM_MAN, BN_CLICKED, OnBnClickedCheckComMan)
		COMMAND_HANDLER(IDC_CHECK_AVATAR_EDITOR, BN_CLICKED, OnBnClickedCheckAvatarEditor)
		COMMAND_HANDLER(IDC_CHECK_DEFAULT, BN_CLICKED, OnBnClickedCheckDefault)
		COMMAND_HANDLER(IDC_CHECK_INFO_MAN, BN_CLICKED, OnBnClickedCheckInfoMan)
		COMMAND_HANDLER(IDC_CHECK_MDUMP, BN_CLICKED, OnBnClickedCheckMdump)
		COMMAND_HANDLER(IDC_CHECK_NTEXTURE, BN_CLICKED, OnBnClickedCheckNtexture)
		COMMAND_HANDLER(IDC_CHECK_OMSPLAYER, BN_CLICKED, OnBnClickedCheckOmsplayer)
		COMMAND_HANDLER(IDC_CHECK_PATH_REFINER, BN_CLICKED, OnBnClickedCheckPathRefiner)
		COMMAND_HANDLER(IDC_CHECK_RESLOADER, BN_CLICKED, OnBnClickedCheckResloader)
		COMMAND_HANDLER(IDC_CHECK_RMML, BN_CLICKED, OnBnClickedCheckRmml)
		COMMAND_HANDLER(IDC_CHECK_SERVICE_MAN, BN_CLICKED, OnBnClickedCheckServiceMan)
		COMMAND_HANDLER(IDC_CHECK_SYNC_MAN, BN_CLICKED, OnBnClickedCheckSyncMan)
		COMMAND_HANDLER(IDC_CHECK_VASENCODER, BN_CLICKED, OnBnClickedCheckVasencoder)
		COMMAND_HANDLER(IDC_CHECK_VIDEO, BN_CLICKED, OnBnClickedCheckVideo)
		COMMAND_HANDLER(IDC_CHECK_VOIP_MAN, BN_CLICKED, OnBnClickedCheckVoipMan)
		COMMAND_HANDLER(IDC_CHECK_NPHYSICS, BN_CLICKED, OnBnClickedCheckNphysics)
		COMMAND_HANDLER(IDC_CHECK_SOME_LOG_TYPE1, BN_CLICKED, OnBnClickedCheckSomeLogType1)
		COMMAND_HANDLER(IDC_CHECK_SOME_LOG_TYPE2, BN_CLICKED, OnBnClickedCheckSomeLogType2)
		COMMAND_HANDLER(IDC_CHECK_SOME_LOG_TYPE3, BN_CLICKED, OnBnClickedCheckSomeLogType3)
		CHAIN_MSG_MAP(CDialogLayout<CLogTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ return TRUE;};
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ return TRUE;};

public:
	void SetContext( oms::omsContext *apContext)
	{
		mpOmsContext = apContext;
	}

	void SetInfoManager( IExternalLogWriter* apInfoManager)
	{
		mpInfoManager = apInfoManager;
	}

	void Trace(const std::string& text, LOG_TYPE logType);

protected:
	void TraceImpl(const std::string& text, LOG_TYPE logType);
	//CRichEditCtrl	m_EditLog;
	CEdit	m_EditLog;
	CEdit	m_EditSearch;
	CButton	n_ButtonSearch;

	oms::omsContext * mpOmsContext;
	IExternalLogWriter* mpInfoManager;

	MP_MAP<LOG_TYPE, bool> mFiltersMap;

private:
	void UpdateEditFilters();
	void ScrollToSelected();
	void LoadLogFromFile();

	unsigned int mLastFindPos;
	
public:
	LRESULT OnBnClickedButtonFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckAutoUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedGroupFilters(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckSoundMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckRenderMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckComMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckAvatarEditor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckInfoMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckMdump(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckNtexture(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckOmsplayer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckPathRefiner(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckResloader(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckRmml(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckServiceMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckSyncMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckVasencoder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckVoipMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckNphysics(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckSomeLogType1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckSomeLogType2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckSomeLogType3(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};