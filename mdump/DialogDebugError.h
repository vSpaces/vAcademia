
class CDialogDebugError :
	public CDialogImpl<CDialogDebugError>
	//public CUpdateUI<CDialogDebugError>
{
public:
	CDialogDebugError();
	virtual ~CDialogDebugError();

	enum { IDD = IDD_FORMVIEW};

	//const _AtlUpdateUIMap* GetUpdateUIMap(){return NULL;};

	BEGIN_MSG_MAP(CDialogDebugError)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON_YES, BN_CLICKED, OnBnClickedButtonYes)
		COMMAND_HANDLER(IDC_BUTTON_NO, BN_CLICKED, OnBnClickedButtonNo)
		COMMAND_HANDLER(IDC_CHECK1, BN_CLICKED, OnBnClickedCheck1)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedButtonYes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonNo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheck1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	BOOL m_bCreatingDumpAndSending;
	int m_iLanguage;

public:
	void SetAppLanguage( int iLanguage);
	BOOL IsCreatingDumpAndSending();	
};