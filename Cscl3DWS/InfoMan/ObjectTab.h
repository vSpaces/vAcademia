#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "objectTree.h"
#include "TabViewWnd.h"
#include "SynchObjectTab.h"
#include "CoordinateObjectTab.h"


class CObjectTab :
	public CBaseDialogTab,
	public CDialogImpl<CObjectTab>,
	public CUpdateUI<CObjectTab>,
	public CDialogLayout<CObjectTab>
{
public:
	CObjectTab();
	virtual ~CObjectTab();

	enum { IDD = IDD_OBJECTVIEW};

	BEGIN_LAYOUT_MAP()
		BEGIN_LAYOUT_CONTAINER( ABS(4), ABS(4), ABS(-4), ABS(-4))
		
			BEGIN_LAYOUT_CONTAINER( RATIO(0), RATIO(0), ABS(250), RATIO(1))
				LAYOUT_CONTROL(IDC_TREE_OBJECT, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_VERTICAL)
				LAYOUT_CONTROL(IDC_BUTTON_UPDATE, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_CHECK_SYNCH, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_CHECK_CHARACTER, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_CHECK_CLASSES, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_STATIC_FILTER, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_EDIT_FIND, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_BUTTON_FIND, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_STATIC_FIND, LAYOUT_ANCHOR_BOTTOM_LEFT)
			END_LAYOUT_CONTAINER()

			BEGIN_LAYOUT_CONTAINER( RIGHT_OF_PAD(IDC_TREE_OBJECT, 4), RATIO(0), RATIO(1), RATIO(1))
				LAYOUT_CONTROL(IDC_STATIC_PROPERTIES, LAYOUT_ANCHOR_ALL)
				LAYOUT_CONTROL(IDC_EDIT_PROPERTIES, LAYOUT_ANCHOR_ALL)
				LAYOUT_CONTROL(IDC_PROP_UPDATE_BUTTON, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_BUTTON_OPEN_SRC, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_UPDATE_BINSTATE, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_EMULATE_NETWORK_BREAKS, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_BREAK_OBJECT_SERVER_CONNECTION, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_BREAK_RES_SERVER_CONNECTION, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_BREAK_VOIP_SERVER_CONNECTION, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_BREAK_SERVICE_SERVER_CONNECTION, LAYOUT_ANCHOR_BOTTOM_LEFT)
				LAYOUT_CONTROL(IDC_BUTTON_COMPINFO_TO_BUFFER, LAYOUT_ANCHOR_BOTTOM_LEFT)
			END_LAYOUT_CONTAINER()

		END_LAYOUT_CONTAINER()
	END_LAYOUT_MAP()

public:
	BEGIN_UPDATE_UI_MAP(CObjectTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CObjectTab)
		COMMAND_HANDLER(IDC_BUTTON_UPDATE, BN_CLICKED, OnBnClickedButtonUpdate)
		NOTIFY_HANDLER(IDC_TREE_OBJECT, TVN_SELCHANGED, OnTvnSelchangedTreeObject)
		COMMAND_HANDLER(IDC_CHECK_CHARACTER, BN_CLICKED, OnBnClickedCheckCharacter)
		COMMAND_HANDLER(IDC_CHECK_SYNCH, BN_CLICKED, OnBnClickedCheckSynch)
		COMMAND_HANDLER(IDC_BUTTON_FIND, BN_CLICKED, OnBnClickedButtonFind)
		COMMAND_HANDLER(IDC_PROP_UPDATE_BUTTON, BN_CLICKED, OnBnClickedPropUpdateButton)
		COMMAND_HANDLER(IDC_PROP_UPDATE_BUTTON2, BN_CLICKED, OnBnClickedPropUpdateButton2)
		COMMAND_HANDLER(IDC_RICH_EDIT_LOG, BN_CLICKED, OnBnClickedRichEditLog)
		COMMAND_HANDLER(IDC_UPDATE_BINSTATE, BN_CLICKED, OnBnClickedUpdateBinState)
		COMMAND_HANDLER(IDC_EMULATE_NETWORK_BREAKS, BN_CLICKED, OnBnClickedEnumateNWBs)
		COMMAND_HANDLER(IDC_BREAK_OBJECT_SERVER_CONNECTION, BN_CLICKED, OnBnClickedFreezeObjectServerConnection)
		COMMAND_HANDLER(IDC_BREAK_RES_SERVER_CONNECTION, BN_CLICKED, OnBnClickedFreezeResServerConnection)
		COMMAND_HANDLER(IDC_BREAK_VOIP_SERVER_CONNECTION, BN_CLICKED, OnBnClickedFreezeVoipServerConnection)
		COMMAND_HANDLER(IDC_BREAK_SERVICE_SERVER_CONNECTION, BN_CLICKED, OnBnClickedFreezeServiceServerConnection)
		COMMAND_HANDLER(IDC_BUTTON_OPEN_SRC, BN_CLICKED, OnBnClickedButtonOpenSrc)
		COMMAND_HANDLER(IDC_BUTTON_COMPINFO_TO_BUFFER, BN_CLICKED, OnBnClickedButtonCompInfoToBuffer)
		CHAIN_MSG_MAP(CUpdateUI<CObjectTab>)
		CHAIN_MSG_MAP(CDialogLayout<CObjectTab>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ return TRUE;};
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ return TRUE;};
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ return TRUE;};
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
	void AddObject();

	void SetContext( oms::omsContext *apContext)
	{
		objectTree.SetContext( apContext);
		pContext = apContext;
	}
	inline void ShowObject(const rmml::mlString& aTarget) { objectTree.ShowObject( aTarget);}


private:
	oms::omsContext * pContext;
	mlObjectInfo* mpObjectInfo;
	CButton compInfoToBufferButton;
	CButton updatePropBitton;
	CButton openSrcBitton;
	CButton updateBinStatePropBitton;
	CButton enumateNetworksBreaks;
	CObjectTree objectTree;
	CEdit	mProperties;
	bool	mBuilding;
	bool	objectServerConnectionIsFreezed;
	bool	resServerConnectionIsFreezed;
	bool	voipServerConnectionIsFreezed;
	bool	serviceServerConnectionIsFreezed;

	void SetSynchProperties(const std::wstring& info);

	LRESULT OnBnClickedButtonUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckCharacter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckSynch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTvnSelchangedTreeObject(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedPropUpdateButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
public:
	LRESULT OnBnClickedPropUpdateButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRichEditLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedUpdateBinState(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedEnumateNWBs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedFreezeObjectServerConnection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedFreezeResServerConnection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedFreezeVoipServerConnection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedFreezeServiceServerConnection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonOpenSrc(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonCompInfoToBuffer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};