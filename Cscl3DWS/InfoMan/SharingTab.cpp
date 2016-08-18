// SharingTab.cpp : Implementation of CSharingTab

#include "stdafx.h"
#include "LogTab.h"
#include "SharingTab.h"
#include "IDesktopMan.h"
#include ".\sharingtab.h"

void CSharingTab::SetContext( oms::omsContext *apContext)
{
	mpOmsContext= apContext;
	currentSession = NULL;
	m_building = false;
}

LRESULT CSharingTab::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

	m_testTree.Attach(GetDlgItem( IDC_TREE_SHARING_SETTINGS));
	m_resultsEdit.Attach(GetDlgItem( IDC_SESSION_VIEW));

	m_building = true;
	BuildTree();
	m_building = false;
	
	bHandled = FALSE;
	return 0;
}

void CSharingTab::BuildTree()
{
	m_testTree.DeleteAllItems();

	if (!mpOmsContext)
	{
		return;
	}

	if (!mpOmsContext->mpSharingMan)
	{
		return;
	}

	std::string result = mpOmsContext->mpSharingMan->GetAllViewSession();
	result += " ";

	std::string parsingStr;
	parsingStr.clear();
	std::string root = "Шаринги";

	std::string begin = "scene.objects.";
	std::string end = ".pscreenObj.sharingCtrl.sharingImg";
	std::string nameStr = "";
	std::string tmpStr = "";

//	HTREEITEM hRoot = AddRoot("Шаринги");
	for (int i=0; i < result.length(); i++)
	{
		if (result[i] == ' ')
		{
			if (parsingStr.empty())
				continue;
			char* str = MP_NEW_ARR( char, parsingStr.length()+1);
			memcpy(str, parsingStr.c_str(), parsingStr.length());
			str[parsingStr.length()] = 0;

			int pos = 0;

			if (pos = parsingStr.find(begin) >= 0)
			{
				tmpStr = parsingStr.substr(begin.length());
			}
			
			if ((pos = tmpStr.find(end)) >= 0)
			{
				nameStr = tmpStr.substr(0, pos);
			}
			char* name = MP_NEW_ARR( char, nameStr.length()+1);
			memcpy(name, nameStr.c_str(), nameStr.length());
			name[nameStr.length()] = 0;

			AddItem(str, name, NULL);
			//AddItem(NULL, parsingStr.c_str(), hRoot);
			
			
			parsingStr.clear();
		}
		else
			parsingStr += result[i];
	}
}

HTREEITEM CSharingTab::AddRoot(const char* txt)
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

HTREEITEM CSharingTab::AddItem(char* data, char* txt, HTREEITEM hRoot)
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
// CSharingTab

LRESULT CSharingTab::OnTvnSelchangedTreeSharingSettings(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (m_building)
		return 0;
	// TODO: Add your control notification handler code here
	char* target = (char *)m_testTree.GetItemData(pNMTreeView->itemNew.hItem);
	//mpOmsContext->mpSharingMan->GetSessionInfo(target);
	if (target != NULL)
		m_resultsEdit.SetWindowText(mpOmsContext->mpSharingMan->GetSessionInfo(target).c_str());

	currentSession = target;
	return 0;
}

LRESULT CSharingTab::OnBnClickedButtonUpdateTree(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	m_building = true;
	BuildTree();
	m_building = false;
	currentSession = NULL;
	return 0;
}

LRESULT CSharingTab::OnBnClickedButtonUpdateInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if (currentSession != NULL)
		m_resultsEdit.SetWindowText(mpOmsContext->mpSharingMan->GetSessionInfo(currentSession).c_str());
	return 0;
}

LRESULT CSharingTab::OnBnClickedButtonShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if (currentSession != NULL)
		mpOmsContext->mpSharingMan->ShowLastFrame(currentSession);
	return 0;
}
