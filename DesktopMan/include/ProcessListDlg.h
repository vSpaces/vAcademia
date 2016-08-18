#pragma once
#include "UI/ColoredControls.h"
#include "UI/coloredVAControls.h"
#include "UI/WTButton.h"
#include "UI/wtGroupBox.h"
#include <vector>
using namespace std;
//#include "atlstr.h"
class CWndSelectorDialog;

class CProcessListDlg : public CAxDialogImpl<CProcessListDlg>,
				 	    public CColoredDialog<CProcessListDlg>
{ 
public: 
	CProcessListDlg(BOOL,CWndSelectorDialog*);
	~CProcessListDlg(void);


public:
	enum { IDD = IDD_PROCESS_LIST_DLG };

public:
	BEGIN_MSG_MAP(CProcessListDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//MESSAGE_HANDLER(WM_CLOSE,OnDestroy)
		//MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnBnClickedCancel)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnBnClickedOk)
		NOTIFY_HANDLER(IDC_TREE_PROCESSES, TVN_SELCHANGED, OnTvnSelchangedTreeProcesses)
		NOTIFY_HANDLER(IDC_TREE_PROCESSES, NM_DBLCLK, OnDBKlickTreeProcesses)
		CHAIN_MSG_MAP(CAxDialogImpl<CProcessListDlg>)
		CHAIN_MSG_MAP(CColoredDialog<CProcessListDlg>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()


	LRESULT OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//CTreeCtrlEx	m_ctvProcess;

	HICON GetAssociatedFileIcon( LPCTSTR lpctszFilePath_i );
	DWORD GetProcessID(char* lpNameProcess);
	HWND  FindWnd( DWORD hProcessIDToFind );



protected:
	MP_VECTOR<MP_STRING> list;
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CListBox	m_processesListBox;
	//CColoredTreeViewCtrl
    CTreeViewCtrl m_treeView;

	CWTButtonCtrl m_Btn_Cancel;
	CWTButtonCtrl m_Btn_Ok; 
	CVAColoredStaticCtrl m_Label; //IDC_STATIC
	int m_nInitialImageCount;
    CImageList m_ImageList;
	BOOL m_englishVersion;
    CWndSelectorDialog* m_WndSelectorDialog1;
   //CTreeCtrlEx	m_ctvProcess;

public:
	HWND m_MainWndProc;
	LRESULT OnSelectedItemConfirmed();
	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTvnSelchangedTreeProcesses(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnDBKlickTreeProcesses(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT CompileList();
	
};
