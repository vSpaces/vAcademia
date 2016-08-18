#include "StdAfx.h"
#include "include/ProcessListDlg.h"
#include "tlhelp32.h"
#include "shlwapi.h"
#include "shellapi.h"
#include <string>
#include <psapi.h>
#include "..\include\selectors\WndSelectorDialog.h"

using namespace std;
#define SETWTEXT(x,y) ::SetWindowTextW(x.m_hWnd,y);

CProcessListDlg::CProcessListDlg(BOOL a_englishVersion, CWndSelectorDialog* a_WndSelectorDialog):
MP_VECTOR_INIT(list)
{
	m_englishVersion = a_englishVersion;
	m_WndSelectorDialog1=a_WndSelectorDialog;
}

CProcessListDlg::~CProcessListDlg(void)
{
}

LRESULT CProcessListDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	m_treeView.Attach(GetDlgItem(IDC_TREE_PROCESSES));

	m_Btn_Cancel.SubclassWindow(GetDlgItem(IDCANCEL));
	 //m_Btn_Cancel.SetTextColor( RGB(0,35,99));
	m_Btn_Ok.SubclassWindow(GetDlgItem(IDOK));
	m_Label.SubclassWindow(GetDlgItem(IDC_STATIC));


	 if(m_englishVersion)
	 {
		SETWTEXT(m_Btn_Cancel, L"Cancel");
		::SetWindowText(m_hWnd, "Selection of applications");
		//SETWTEXT(m_hWnd, L"Selection of applications");
		SETWTEXT(m_Btn_Ok, L"Ok");
		SETWTEXT(m_Label, L"Select an application to capture images");
		 
	/*		 
		 m_Btn_Cancel.SetWindowText("Cancel");
		 SetWindowText("Selection of applications");
		 m_Btn_Ok.SetWindowText("Ok");
		 m_Label.SetWindowText("Select an application to capture images");
	*/
		 //select an application to capture images
	 
	 }
	 else
	 {
		 ::SetWindowText(m_hWnd, "Выбор приложения");
		 SETWTEXT(m_Btn_Cancel, L"Отмена");
		// SETWTEXT(m_hWnd, L"Выбор приложения");
		 SETWTEXT(m_Btn_Ok, L"Ok");
		 SETWTEXT(m_Label, L"Выберите приложения для захвата изображения");

	 }
	 
	 m_ImageList.Create( GetSystemMetrics( SM_CXSMICON ), GetSystemMetrics( SM_CYSMICON ), ILC_COLOR32 | ILC_MASK, 0, 20 );
	 return 1;  // Let the system set the focus
}



LRESULT CProcessListDlg::CompileList()
{

	SetBitmap( IDB_BACKTEXTURE);
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == NULL) 
	{
		MessageBox("Error Load ToolHelp");
	}
	PROCESSENTRY32 proc = {0};
	proc.dwSize = sizeof(PROCESSENTRY32);

	TCHAR   csFullPath[MAX_PATH] ;  
	
	m_treeView.SetImageList( m_ImageList);

	list.clear();
	m_treeView.DeleteAllItems();
	for (int i =0;i<m_ImageList.GetImageCount();i++ )
	{
		m_ImageList.Remove(i);
	}


	if (Process32First(hSnap, &proc))
	{
		//m_processesListBox.AddString(proc.szExeFile);

		while (Process32Next(hSnap, &proc)) 
		{
			//	m_processesListBox.AddString(proc.szExeFile);	
			
			int MAX_MODULES = 1024;
			// Module array
			HMODULE hModules[1024] = { 0 };

			// Name variable
			TCHAR szName[MAX_PATH] = { 0 };

			HANDLE hProces = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_DUP_HANDLE, 
				FALSE, 
				proc.th32ProcessID );

			GetModuleFileNameEx(hProces, hModules[0], csFullPath, MAX_PATH);

			if (hProces)
				CloseHandle(hProces);

			HWND h_wnd_ = FindWnd(proc.th32ProcessID);
			if((h_wnd_!=NULL) &&(m_WndSelectorDialog1->m_hWnd!=h_wnd_))
			{			
				HICON hIcon = GetAssociatedFileIcon( csFullPath );
				int nIndex = 20; 
				if( hIcon  )
				{
					nIndex = m_ImageList.AddIcon( hIcon );
					int iii = GetLastError();
					if( nIndex  == -1 )
					{
						//ASSERT( FALSE );
						nIndex = 20;
					}

				}// End if

				char text[100];
				//char m_nameEditStr[100];
				//				m_nameEditStr = "";
				CWindow myWindow = h_wnd_;

				if(myWindow.GetWindowText(text, 100))
				{
					//					m_nameEditStr = text;
					string str = proc.szExeFile;
					list.push_back(MAKE_MP_STRING(str));
					
					int* procesIDData = MP_NEW_ARR( int, 1);
					memcpy(procesIDData, &proc.th32ProcessID, sizeof(int));
					m_treeView.SetItemData(m_treeView.InsertItem(text, nIndex, nIndex, 0, 0),(DWORD_PTR)procesIDData);
				}

			}
		}
	}

	m_MainWndProc = NULL;

	return 0;
}
HICON CProcessListDlg::GetAssociatedFileIcon( LPCTSTR lpctszFilePath_i )
{
	SHFILEINFO shFileInfo = { 0 };

	// For retrieving icon
	if( SHGetFileInfo( lpctszFilePath_i, 
		FILE_ATTRIBUTE_NORMAL, 
		&shFileInfo, 
		sizeof( shFileInfo ), 
		SHGFI_SMALLICON | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES ))
	{
	
	}

	// Do we have an icon, then store this icon handle
	// for destruction later on
	if( shFileInfo.hIcon )
	{
		// Add icon to list for destroying later on
		//m_lstExtraIcons.AddTail( shFileInfo.hIcon );
	}

	// Icon to return
	return shFileInfo.hIcon;
}


LRESULT CProcessListDlg::OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_HIDE);
	m_WndSelectorDialog1->OnProcessListCancelled();
	return 0;
}


LRESULT CProcessListDlg::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OnSelectedItemConfirmed();
	return 0;
}

LRESULT CProcessListDlg::OnSelectedItemConfirmed()
{
	HTREEITEM item =  m_treeView.GetSelectedItem();
	LPTSTR str = (LPTSTR)m_treeView.GetItemData(item);

	#define ITEM_TEXT_SIZE	1024
	LPTSTR strtemp[ITEM_TEXT_SIZE];
	BOOL b = m_treeView.GetItemText(item, (LPTSTR)strtemp, ITEM_TEXT_SIZE);
	int* processIDData = (int*)m_treeView.GetItemData(item);
	if(b)
	{
		DWORD idproc = *processIDData;
		m_MainWndProc = FindWnd(idproc);
//HWND bhWnd;
//bhWnd = FindWindow(NULL, "Безымянный - Блокнот");

		CWindow selectedWindow = m_MainWndProc;
		if (( ::GetWindowLong( m_MainWndProc,GWL_STYLE)& WS_MINIMIZE))
		{
			//selectedWindow.ShowWindow(SW_HIDE);
			selectedWindow.ShowWindow(SW_RESTORE);
			RECT selectedWndRect;
			selectedWindow.GetWindowRect(&selectedWndRect);
			selectedWindow.SetWindowPos(HWND_BOTTOM, &selectedWndRect, SWP_NOACTIVATE);
		}		
		
		m_WndSelectorDialog1->OnProcessWindowSelected(m_MainWndProc); 
		ShowWindow(SW_HIDE);
	}
	return 1;
}

DWORD CProcessListDlg::GetProcessID(char* lpNameProcess) // в параметре - имя процесса для внедрения
{
	HANDLE snap;
	PROCESSENTRY32 pentry32;
	snap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(snap==INVALID_HANDLE_VALUE) return 0;
	pentry32.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(snap,&pentry32)) {CloseHandle(snap);return 0;}
	do
	{
		if(!lstrcmpi(lpNameProcess,&pentry32.szExeFile[0]))
		{
			CloseHandle(snap);
			return pentry32.th32ProcessID;
		}
	}
	while(Process32Next(snap,&pentry32));
	CloseHandle(snap);
	return 0;
}

HWND  CProcessListDlg::FindWnd( DWORD hProcessIDToFind ) 
{
	HWND hWndDesktop = GetDesktopWindow();

	HWND hWnd = ::GetTopWindow(0);
	HWND WindowHandle = NULL;
	while(hWnd)
	{
		DWORD pid;
		DWORD dwTheardId = ::GetWindowThreadProcessId(hWnd, &pid);
		if(pid == hProcessIDToFind) // your process id;
		{
			RECT lp_r;
						
			if (( ::GetWindowLong( hWnd,GWL_STYLE)& WS_VISIBLE)&&::IsWindowVisible(hWnd))	
			{
				if (( ::GetWindowLong( hWnd,GWL_STYLE)& WS_MINIMIZE))//когда окно минимизировано оно имеет отрицательные координаты
				{
					WindowHandle = hWnd;
					break;
				}
				else if(::GetClientRect(hWnd,OUT &lp_r) && (lp_r.bottom>0)&&(lp_r.right>0)) //отсекаем прочие окна за пределами экрана
				{				
					WindowHandle = hWnd;
					break;
				}
			}
		}
		
		hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
	}
	return WindowHandle;
} 

LRESULT CProcessListDlg::OnTvnSelchangedTreeProcesses(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	return 0;
}
LRESULT CProcessListDlg::OnDBKlickTreeProcesses(int , LPNMHDR pNMHDR, BOOL&	)
{
	OnSelectedItemConfirmed();
	return 0;
}


