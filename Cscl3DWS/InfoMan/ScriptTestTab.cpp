// ScriptTestTab.cpp : Implementation of CScriptTestTab

#include "stdafx.h"
#include "LogTab.h"
#include "ScriptTestTab.h"
#include ".\scripttesttab.h"
#include "SimpleThreadStarter.h"

CScriptTestTab::~CScriptTestTab()
{
	if( IsWindow())
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
}

void CScriptTestTab::SetContext(oms::omsContext *apContext)
{
	mpOmsContext = apContext;
}

void CScriptTestTab::SelectAllItems()
{
	HTREEITEM rootItem = m_testTree.GetChildItem(TVI_ROOT);
	while (rootItem != NULL)
	{
		HTREEITEM item = m_testTree.GetChildItem(rootItem);
		while( item != NULL)
		{
			m_testTree.SetCheckState(item, true);
			item = m_testTree.GetNextSiblingItem( item);
		}
		m_testTree.SetCheckState(rootItem, true);
		rootItem = m_testTree.GetNextSiblingItem( rootItem);
	}
}
void CScriptTestTab::DeSelectAllItems()
{
	HTREEITEM rootItem = m_testTree.GetChildItem(TVI_ROOT);
	while (rootItem != NULL)
	{
		HTREEITEM item = m_testTree.GetChildItem(rootItem);
		while( item != NULL)
		{
			m_testTree.SetCheckState(item, false);
			item = m_testTree.GetNextSiblingItem( item);
		}
		m_testTree.SetCheckState(rootItem, false);
		rootItem = m_testTree.GetNextSiblingItem( rootItem);
	}
}

LRESULT CScriptTestTab::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

	m_resultsEdit.Attach(GetDlgItem( IDC_TEST_RESULT));
	m_startButton.Attach(GetDlgItem( IDC_BUTTON_START));
	m_stopButton.Attach(GetDlgItem( IDC_BUTTON_STOP));
	m_testTree.Attach(GetDlgItem( IDC_TREE_TEST));
	
	BuildTestTree();

	bHandled = FALSE;
	return 0;
}
// CScriptTestTab

LRESULT CScriptTestTab::OnBnClickedButtonStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LRESULT lLength = 30;

	TCHAR* lpData = MP_NEW_ARR( TCHAR, lLength+1);
	TCHAR* pData = lpData;

	pData = "scene.testCtrl.startTesting();" + '\0';

	wchar_t * lpw = MP_NEW_ARR( wchar_t, lLength+1);
	ZeroMemory(lpw, sizeof(lpw));
	::MultiByteToWideChar(CP_ACP, 0, pData, -1, lpw, strlen(pData)+1);

	omsresult result;
	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript(lpw, strlen(pData));

	MP_DELETE_ARR( lpData);
	MP_DELETE_ARR( lpw);

	m_resultsEdit.SetWindowText("");

	return 0;
}

DWORD WINAPI CScriptTestTab::startAutoTestingThread_(LPVOID param)
{
	Sleep(30000);
	CScriptTestTab* aParams = (CScriptTestTab*)param;
	if(aParams != NULL)
	{
		//aParams->BuildTestTree();
		BOOL bHandled = true;
		WPARAM wParam = MAKEWPARAM( IDC_BUTTON_SELECT, BN_CLICKED);
		aParams->SendMessage( WM_COMMAND, wParam, NULL);
		//aParams->OnBnClickedButtonSelect(0, 0, 0, bHandled);
		Sleep(3000);
		//aParams->OnBnClickedButtonStart( 0, 0, 0, bHandled);
		wParam = MAKEWPARAM( IDC_BUTTON_START, BN_CLICKED);
		aParams->SendMessage( WM_COMMAND, wParam, NULL);
	}
	return 0;
}

void CScriptTestTab::StartAutoTesting()
{
	CSimpleThreadStarter testThreadStarter(__FUNCTION__);
	testThreadStarter.SetRoutine(startAutoTestingThread_);
	testThreadStarter.SetParameter(this);
	HANDLE hStartAutoTestingThread = testThreadStarter.Start();

	BuildTestTree();
}

wchar_t* CScriptTestTab::GetModuleNames()
{
	wchar_t* lpwResultTemp = NULL;
	CWComString s = L"scene.LoadTests();";
	omsresult result;

	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript( s.GetBuffer(), s.GetLength(), &lpwResultTemp);
	LRESULT lLength = 32;

	TCHAR* lpData = MP_NEW_ARR( TCHAR, lLength+1);
	TCHAR* pData = lpData;

	wchar_t* lpwResult = NULL;
	pData = "scene.testCtrl.getModulesName();" + '\0';

	wchar_t * lpw = MP_NEW_ARR( wchar_t, lLength+1);
	ZeroMemory(lpw, sizeof(lpw));
	::MultiByteToWideChar(CP_ACP, 0, pData, -1, lpw, strlen(pData)+1);	
	
	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript(lpw, strlen(pData), &lpwResult);

	MP_DELETE_ARR( lpData);
	MP_DELETE_ARR( lpw);

	return lpwResult;
}
wchar_t* CScriptTestTab::GetTestsByModule(const char* moduleName)
{
	std::string str = "scene.testCtrl.getTestsName(\"";
	str += moduleName;
	str += "\");"+ '\0';

	wchar_t * lpw = MP_NEW_ARR( wchar_t, str.length());
	ZeroMemory(lpw, sizeof(lpw));
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, lpw, str.length());
	wchar_t* lpwResult = NULL;

	omsresult result;
	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript(lpw, str.length(), &lpwResult);

	MP_DELETE_ARR( lpw);

	return lpwResult;
}

void CScriptTestTab::BuildTestTree()
{
	m_testTree.DeleteAllItems();

	wchar_t* lpwResult = GetModuleNames();
	if (!lpwResult)
		return;
	std::wstring parsingTestStr;
	std::wstring parsingStr;
	parsingStr.clear();
	std::string root = "Тесты";
//	HTREEITEM hMainRoot = AddRoot(root.c_str());
	for (int i=0; i < strlen(WC2MB(lpwResult)); i++)
	{
		if (lpwResult[i] == ' ')
		{
			HTREEITEM hRoot = AddRoot(WC2MB(parsingStr.c_str()));
			wchar_t* lpwTests = GetTestsByModule(WC2MB(parsingStr.c_str()));
			if (!lpwTests)
				continue;
			for (int j=0; j < strlen(WC2MB(lpwTests)); j++)
			{
				if (lpwTests[j] == ' ')
				{
					std::wstring str = L"\"" + parsingStr + L"\", \"" + parsingTestStr + L"\"";
					AddItem(WC2MB(str.c_str()), WC2MB(parsingTestStr.c_str()), hRoot);
					parsingTestStr.clear();
				} 
				else
					parsingTestStr += lpwTests[j];
			}
			parsingStr.clear();
		}
		else
			parsingStr += lpwResult[i];
	}
}

HTREEITEM CScriptTestTab::AddRoot(const char* txt)
{
	TVINSERTSTRUCT tvis;
	tvis.hParent = TVI_ROOT;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT;
	tvis.item.pszText = (LPTSTR) txt;
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 0;
	tvis.item.state = 0;
	tvis.item.stateMask = 0;
	tvis.item.lParam = 0;
	HTREEITEM hItem = m_testTree.InsertItem( &tvis);
	m_testTree.SetCheckState(hItem, false);
	DWORD_PTR ptr = (unsigned long)txt;
	m_testTree.SetItemData(hItem, ptr);
	return hItem;
}

HTREEITEM CScriptTestTab::AddItem(char* data, char* txt, HTREEITEM hRoot)
{
	TVINSERTSTRUCT tvis;
	tvis.hParent = hRoot;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT;
	tvis.item.pszText = (LPTSTR) txt;
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 0;
	tvis.item.state = 0;
	tvis.item.stateMask = 0;
	tvis.item.lParam = 0;
	HTREEITEM hItem = m_testTree.InsertItem( &tvis);
	m_testTree.SetCheckState(hItem, false);
	DWORD_PTR ptr = (unsigned long)data;  
	m_testTree.SetItemData(hItem, ptr);
	return hItem;
}

void CScriptTestTab::Trace(char* resStr)
{
	int nInsertAfterChar = m_resultsEdit.GetWindowTextLength();
	m_resultsEdit.SetSel(nInsertAfterChar, nInsertAfterChar, true);
	m_resultsEdit.ReplaceSel(resStr, false);
}

LRESULT CScriptTestTab::OnBnClickedButtonStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	std::string str = "scene.testCtrl.stopTesting();";
	
	wchar_t * lpw = MP_NEW_ARR( wchar_t, str.length());
	ZeroMemory(lpw, sizeof(lpw));
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, lpw, str.length());
	
	omsresult result;
	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript(lpw, str.length());

	MP_DELETE_ARR( lpw);

	return 0;
}

LRESULT CScriptTestTab::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPNMHDR lpnmh = (LPNMHDR) lParam;
	TVHITTESTINFO ht = {0};
	if (NULL == lpnmh)
	{
		bHandled = FALSE;
		return 0;
	}

	if((lpnmh->code  == NM_CLICK) && (lpnmh->idFrom == IDC_TREE_TEST))
	{
		DWORD dwpos = GetMessagePos();

		ht.pt.x = GET_X_LPARAM(dwpos);
		ht.pt.y = GET_Y_LPARAM(dwpos);

		//MapWindowPoints(m_testTree.m_hWnd, &ht.pt, 1);
		RECT rec;
		m_testTree.GetWindowRect(&rec);
		ht.pt.x = ht.pt.x - rec.left;
		ht.pt.y = ht.pt.y - rec.top;

		TreeView_HitTest(lpnmh->hwndFrom, &ht);

		if(TVHT_ONITEMSTATEICON & ht.flags)
		{
			PostMessage(UM_CHECKSTATECHANGE, 0, (LPARAM)ht.hItem);
		}
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CScriptTestTab::OnCheckBoxChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HTREEITEM   hItemChanged = (HTREEITEM)lParam;
	bool selected = m_testTree.GetCheckState(hItemChanged);
	
	HTREEITEM item = m_testTree.GetChildItem(hItemChanged);
	bool isRoot = false;
	std::string param = "";
	if (m_testTree.GetChildItem(hItemChanged) != NULL)
		isRoot = true;
	while( item != NULL)
	{
		bool a = m_testTree.GetCheckState(item);
		//CComString s;
		//m_testTree.GetItemText( item, s.GetBufferSetLength(1024), 1023);
		m_testTree.SetCheckState(item, selected);
		item = m_testTree.GetNextSiblingItem( item);
	}

	std::string str = "";
	param = (char*)m_testTree.GetItemData(hItemChanged);
	if (selected)
		if (isRoot)
			str = "scene.testCtrl.activateTest(\"" + param + "\");";
		else
			str = "scene.testCtrl.activateTest(" + param + ");";
	else
		if (isRoot)
			str = "scene.testCtrl.deactivateTest(\"" + param + "\");";
		else
			str = "scene.testCtrl.deactivateTest(" + param + ");";


	wchar_t * lpw = MP_NEW_ARR( wchar_t, str.length());
	ZeroMemory(lpw, sizeof(lpw));
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, lpw, str.length());

	omsresult result;
	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript(lpw, str.length());

	MP_DELETE_ARR( lpw);

	bHandled = FALSE;
	return 0;
}

LRESULT CScriptTestTab::OnBnClickedButtonSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	std::string str = "scene.testCtrl.activateAll();";

	wchar_t * lpw = MP_NEW_ARR( wchar_t, str.length());
	ZeroMemory(lpw, sizeof(lpw));
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, lpw, str.length());

	omsresult result;
	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript(lpw, str.length());

	MP_DELETE_ARR( lpw);

	SelectAllItems();

	return 0;
}

LRESULT CScriptTestTab::OnBnClickedButtonDeselect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	std::string str = "scene.testCtrl.deActivateAll();";

	wchar_t * lpw = MP_NEW_ARR( wchar_t, str.length());
	ZeroMemory(lpw, sizeof(lpw));
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, lpw, str.length());

	omsresult result;
	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript(lpw, str.length());

	MP_DELETE_ARR( lpw);

	DeSelectAllItems();

	return 0;
}

LRESULT CScriptTestTab::OnBnClickedButtonGetList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BuildTestTree();
	return 0;
}
