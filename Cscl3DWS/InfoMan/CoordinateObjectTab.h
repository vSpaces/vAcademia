#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "objectTree.h"
#include "ILogWriter.h"
#include <map>
//#include <afxrich.h>

class CCoordinateObjectPropTab :
	public CBaseDialogTab,
	public CDialogImpl<CCoordinateObjectPropTab>,
	public CUpdateUI<CCoordinateObjectPropTab>,
	public CDialogLayout<CCoordinateObjectPropTab>
{
public:
	CCoordinateObjectPropTab();
	virtual ~CCoordinateObjectPropTab();

	enum { IDD = IDD_OBJECT_COORDINATE_PROP};

	BEGIN_LAYOUT_MAP()
		LAYOUT_CONTROL(IDC_EDIT_PROP, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP)
	END_LAYOUT_MAP()

public:
	BEGIN_UPDATE_UI_MAP(CCoordinateObjectPropTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CCoordinateObjectPropTab)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
	void SetTextProp( const ml3DPosition& info);

protected:
	CEdit	mX;
	CEdit	mY;
	CEdit	mZ;

private:

};