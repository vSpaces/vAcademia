#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "objectTree.h"
#include "ILogWriter.h"
#include <map>

class CSynchObjectPropTab :
	public CBaseDialogTab,
	public CDialogImpl<CSynchObjectPropTab>,
	public CUpdateUI<CSynchObjectPropTab>,
	public CDialogLayout<CSynchObjectPropTab>
{
public:
	CSynchObjectPropTab();
	virtual ~CSynchObjectPropTab();

	enum { IDD = IDD_OBJECT_SYNC_PROP};

	BEGIN_LAYOUT_MAP()
		LAYOUT_CONTROL(IDC_EDIT_PROP, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP)
	END_LAYOUT_MAP()

public:
	BEGIN_UPDATE_UI_MAP(CSynchObjectPropTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CSynchObjectPropTab)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CDialogLayout<CSynchObjectPropTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
	void SetTextProp( const std::wstring& info);

protected:
	CEdit	m_EditProp;

private:

};