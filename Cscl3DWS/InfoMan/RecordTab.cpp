// RecordTab.cpp : Implementation of CRecordTab

#include "stdafx.h"
#include "LogTab.h"
#include "RecordTab.h"
#include "rmml.h"
#include ".\recordtab.h"

CRecordTab::CRecordTab()
:MP_VECTOR_INIT(v_syncRecordExtendedInfo)
{

}

void CRecordTab::SetContext( oms::omsContext *apContext)
{
	mpOmsContext= apContext;
	//currentSession = NULL;
	m_building = false;
	beginRecordRequest = 0;
}

LRESULT CRecordTab::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

	m_ListBox.Attach(GetDlgItem( IDC_LIST_RECORD));
	//m_ListCtrl.Attach(GetDlgItem( IDC_LIST_RECORD2));
	m_resultsEdit.Attach(GetDlgItem( IDC_RECORD_VIEW));

	m_building = false;
	
	bHandled = FALSE;
	return 0;
}

void CRecordTab::GetRecords()
{
	m_building = false;
	m_ListBox.ResetContent();
	/*m_ListCtrl.DeleteAllItems();
	m_ListCtrl.DeleteColumn( 0);*/
	v_syncRecordExtendedInfo.clear();
	mpOmsContext->mpSyncMan->GetEventRecords( 0, 0, -1, L"", L"", this);
}

LRESULT CRecordTab::OnBnClickedButtonUpdateTree(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	GetRecords();
	//BuildTree();
	//currentSession = NULL;
	return 0;
}

LRESULT CRecordTab::OnBnClickedButtonUpdateInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	/*if (currentSession != NULL)
		m_resultsEdit.SetWindowText(mpOmsContext->mpRecordMan->GetSessionInfo(currentSession).c_str());*/
	return 0;
}


LRESULT CRecordTab::OnBnClickedButtonExport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	/*int uSelectedCount = m_ListCtrl.GetSelectedCount();
	if (uSelectedCount > 0)
	{
		for (int i=0; i < uSelectedCount; i++)
		{
			int nItem = m_ListCtrl.GetNextItem(nItem, LVNI_SELECTED);
			//ASSERT(nItem != -1);
			sync::syncRecordExtendedInfo *pData = (sync::syncRecordExtendedInfo *)m_ListCtrl.GetItemData( nItem);
			//m_myListCtrl.Update(nItem); 
		}
	}*/
	int count = m_ListBox.GetSelCount();
	int *pIndex = MP_NEW_ARR( int, count);
	m_ListBox.GetSelItems( count, pIndex);
	for(int i=0;i<count;i++)
	{
		syncRecordExtendedInfo *pData = (syncRecordExtendedInfo *)m_ListBox.GetItemDataPtr( pIndex[i]);
	}
	return 0;
}

LRESULT CRecordTab::OnBnClickedButtonImport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	/*int uSelectedCount = m_ListCtrl.GetSelectedCount();
	if (uSelectedCount > 0)
	{
		int nItem = -1;
		for (int i=0; i < uSelectedCount; i++)
		{
			int nItem = m_ListCtrl.GetNextItem(nItem, LVNI_SELECTED);
			//ASSERT(nItem != -1);
			sync::syncRecordExtendedInfo *pData = (sync::syncRecordExtendedInfo *)m_ListCtrl.GetItemData( nItem);
			//m_myListCtrl.Update(nItem); 
		}
	}*/

	int count = m_ListBox.GetSelCount();
	int *pIndex = MP_NEW_ARR( int, count);
	m_ListBox.GetSelItems( count, pIndex);
	for(int i=0;i<count;i++)
	{
		syncRecordExtendedInfo *pData = (syncRecordExtendedInfo *)m_ListBox.GetItemDataPtr( pIndex[i]);
	}
	return 0;
}

void CRecordTab::Update()
{
	if(m_building && v_syncRecordExtendedInfo.size() != NULL)
	{
		mutex.lock();
		//m_ListCtrl.InsertColumn(0, "records", LVCFMT_LEFT, 150);
		syncRecordExtendedInfoVecIter iter = v_syncRecordExtendedInfo.begin();
		for(;iter!=v_syncRecordExtendedInfo.end();iter++)
		{
			//char ff[7];
			//_itoa(iter->recID, ff, 10);
#if _MSC_VER >= 1600
			syncRecordExtendedInfo* pData = (syncRecordExtendedInfo*)iter._Ptr;
#else
			syncRecordExtendedInfo* pData = (syncRecordExtendedInfo*)iter._Myptr;
#endif
			CComString s = (BSTR)pData->name.c_str();
			int index = m_ListBox.AddString( s.GetBuffer());			
			m_ListBox.SetItemDataPtr( index, (void*)pData);
			/*LVITEM item = { 0 };
			item.mask = LVIF_TEXT;
			item.iItem = -1;
			item.i2SubItem = 0;
			item.pszText = (LPTSTR)s.GetBuffer();
			item.state = 0;
			item.stateMask = 0
			item.iImage = 0;
			item.lParam = 0;
			int index = m_ListCtrl.InsertItem(&item);
			//int index = m_ListCtrl.InsertItem(-1, s.GetBuffer());
			m_ListCtrl.SetItemData( index, (DWORD_PTR)pData);*/
		}
		m_ListBox.SetColumnWidth( 150);
		//m_ListCtrl.SetWindowPos(NULL, )
		m_building = false;
		mutex.unlock();
	}
}

void CRecordTab::onReceivedRecordsExtendedInfo( unsigned int totalCount, unsigned int auRecordCount, syncRecordExtendedInfo** appRecordsInfo, unsigned int auID, int aiErrorCode)
{
	mutex.lock();
	/*if (beginRecordRequest + auRecordCount < totalCount)
	{
		beginRecordRequest += auRecordCount;
		mpOmsContext->mpSyncMan->GetEventRecords(0, beginRecordRequest, 50, L"", L"", this);
	}
	else*/
		m_building = true;
	for (unsigned int i = 0;  i < auRecordCount;  i++)
	{
		syncRecordExtendedInfo syncRecordExtendedInfo;
		syncRecordExtendedInfo.recID = (*appRecordsInfo)->recID;
		syncRecordExtendedInfo.recRealityID = (*appRecordsInfo)->recRealityID;
		syncRecordExtendedInfo.eventDate = (*appRecordsInfo)->eventDate;
		syncRecordExtendedInfo.locationURL = (*appRecordsInfo)->locationURL;
		syncRecordExtendedInfo.language = (*appRecordsInfo)->language;
		syncRecordExtendedInfo.eventRecordID = (*appRecordsInfo)->eventRecordID;
		syncRecordExtendedInfo.name = (*appRecordsInfo)->name;
		syncRecordExtendedInfo.description = (*appRecordsInfo)->description;
		syncRecordExtendedInfo.expected_length = (*appRecordsInfo)->expected_length;
		syncRecordExtendedInfo.login = (*appRecordsInfo)->login;
		syncRecordExtendedInfo.firstname = (*appRecordsInfo)->firstname;
		syncRecordExtendedInfo.rFirstname = (*appRecordsInfo)->rFirstname;
		syncRecordExtendedInfo.lastname = (*appRecordsInfo)->lastname;
		syncRecordExtendedInfo.rLastname = (*appRecordsInfo)->rLastname;
		syncRecordExtendedInfo.post = (*appRecordsInfo)->post;
		syncRecordExtendedInfo.courseName = (*appRecordsInfo)->courseName;
		syncRecordExtendedInfo.open = (*appRecordsInfo)->open;
		syncRecordExtendedInfo.paid = (*appRecordsInfo)->paid;
		syncRecordExtendedInfo.depth = (*appRecordsInfo)->depth;
		syncRecordExtendedInfo.previewImage = (*appRecordsInfo)->previewImage;
		syncRecordExtendedInfo.locationDescription = (*appRecordsInfo)->locationDescription;
		syncRecordExtendedInfo.loc_preview = (*appRecordsInfo)->loc_preview;
		syncRecordExtendedInfo.courseEventsCount = (*appRecordsInfo)->courseEventsCount;
		syncRecordExtendedInfo.courseEventNum = (*appRecordsInfo)->courseEventNum;
		syncRecordExtendedInfo.strTags = (*appRecordsInfo)->strTags;

		// поля просмотра
		syncRecordExtendedInfo.brRecID = (*appRecordsInfo)->brRecID;
		syncRecordExtendedInfo.brDepth = (*appRecordsInfo)->brDepth;
		syncRecordExtendedInfo.seanceID = (*appRecordsInfo)->seanceID;
		syncRecordExtendedInfo.realityID = (*appRecordsInfo)->realityID;
		syncRecordExtendedInfo.recordDuration = (*appRecordsInfo)->recordDuration;
		syncRecordExtendedInfo.recordName = (*appRecordsInfo)->recordName;
		syncRecordExtendedInfo.recordDescription = (*appRecordsInfo)->recordDescription;
		syncRecordExtendedInfo.brRealityID = (*appRecordsInfo)->brRealityID;
		syncRecordExtendedInfo.brRecordName = (*appRecordsInfo)->brRecordName;
		syncRecordExtendedInfo.brRecordDescription = (*appRecordsInfo)->brRecordDescription;
		syncRecordExtendedInfo.brDuration = (*appRecordsInfo)->brDuration;
		syncRecordExtendedInfo.brPreviewImage = (*appRecordsInfo)->brPreviewImage;
		syncRecordExtendedInfo.fromStartDuration = (*appRecordsInfo)->fromStartDuration;
		*appRecordsInfo++;
		v_syncRecordExtendedInfo.push_back(syncRecordExtendedInfo);
	}
	mutex.unlock();
}

/*void CRecordTab::onReceivedFullRecordInfo(unsigned int aiRecordObjectID,  syncRecordInfo* appRecordInfo, const wchar_t* aOutsideUrl, int aiErrorCode)
{
}*/
LRESULT CRecordTab::OnLbnSelchangeListRecord(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//m_ListBox.
	return 0;
}

LRESULT CRecordTab::OnLbnDblclkListRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int index = m_ListBox.GetAnchorIndex();
	syncRecordExtendedInfo *pData = (syncRecordExtendedInfo *)m_ListBox.GetItemDataPtr( index);
	if( (unsigned int)pData == 0xffffffff)
		return 0;
	char ff[32];
	_itoa(pData->seanceID, ff, 10);
	CComString s = "seanceID = "; s += ff; 
	ZeroMemory(ff, 32);
	_itoa(pData->recID, ff, 10);
	s = "\r\n recID = "; s += ff; 
	ZeroMemory(ff, 32);
	_itoa(pData->recordID, ff, 10);
	s = "\r\n base recordID = "; s += ff; 
	s += "\r\n name = "; s += (BSTR)pData->name.c_str();
	s += "\r\n location = "; s += (BSTR)pData->locationDescription.c_str();
	s += "\r\n course = "; s += (BSTR)pData->courseName.c_str();
	s += "\r\n author = "; s += (BSTR)pData->firstname.c_str();s += " ";s += (BSTR)pData->lastname.c_str();
	s += "\r\n description = "; s += (BSTR)pData->description.c_str();
	//m_resultsEdit.Clear();
	m_resultsEdit.SetSel(0, -1);
	m_resultsEdit.ReplaceSel( s.GetBuffer());
	return 0;
}

LRESULT CRecordTab::OnNMDblclkListRecord2(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	/*int index = m_ListCtrl.GetHotItem();
	int index2 = m_ListCtrl.GetNextItem(-1, LVNI_FOCUSED);
	sync::syncRecordExtendedInfo *pData = (sync::syncRecordExtendedInfo *)m_ListCtrl.GetItemData( index2);
	if( (unsigned int)pData == 0xffffffff)
		return 0;
	char ff[32];
	_itoa(pData->seanceID, ff, 10);
	CComString s = "seanceID = "; s += ff; 
	ZeroMemory(ff, 32);
	_itoa(pData->recID, ff, 10);
	s = "\r\n recID = "; s += ff; 
	ZeroMemory(ff, 32);
	_itoa(pData->recordID, ff, 10);
	s = "\r\n base recordID = "; s += ff; 
	s += "\r\n name = "; s += (BSTR)pData->name.c_str();
	s += "\r\n location = "; s += (BSTR)pData->locationDescription.c_str();
	s += "\r\n course = "; s += (BSTR)pData->courseName.c_str();
	s += "\r\n author = "; s += (BSTR)pData->firstname.c_str();s += " ";s += (BSTR)pData->lastname.c_str();
	s += "\r\n description = "; s += (BSTR)pData->description.c_str();
	//m_resultsEdit.Clear();
	m_resultsEdit.SetSel(0, -1);
	m_resultsEdit.ReplaceSel( s.GetBuffer());*/
	return 0;
}
