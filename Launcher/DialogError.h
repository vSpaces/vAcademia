#include "resource.h"

class CDialogError :
	public CDialogImpl<CDialogError>
	//public CUpdateUI<CDialogError>
{
public:
	CDialogError();
	virtual ~CDialogError();

	enum { IDD = IDD_FORMVIEW};

	//const _AtlUpdateUIMap* GetUpdateUIMap(){return NULL;};

	BEGIN_MSG_MAP(CDialogError)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)		
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDC_BTN_OK, BN_CLICKED, OnBnClickedOk)
		COMMAND_HANDLER(IDC_BTN_NO, BN_CLICKED, OnBnClickedNo)		
		COMMAND_HANDLER(IDC_CHECK_HIDEWND, BN_CLICKED, OnBnClickedCheckHidewnd)
	END_MSG_MAP()

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedNo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	BOOL m_bCreatingDumpAndSending;	
	int m_iLanguage;

public:
	void SetAppLanguage( int iLanguage);
	BOOL IsCreatingDumpAndSending();	
	void CloseDialog();
	LRESULT OnBnClickedCheckHidewnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};