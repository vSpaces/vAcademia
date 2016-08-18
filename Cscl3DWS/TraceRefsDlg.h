#pragma once

#include "resource.h"
#include <atlwin.h>

class CTraceRefsDialog : public CDialogImpl<CTraceRefsDialog>
{
public:
	CComBSTR mbstrName;
	BOOL mbLost;
	// aiType - 0: все, 1: 3D-объекты, 2: 3D-персонажи, 3: 3D-группы, 4: композиции
	enum{
		OT_ALL = 0,
		OT_3DOBJECTS,
		OT_CHARACTERS,
		OT_GROUPS,
		OT_COMPOSITIONS
	} mType;
public:
   enum { IDD = IDD_TRACE_REFS };

   BEGIN_MSG_MAP(CTraceRefsDialog)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	  MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
//	  MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      COMMAND_ID_HANDLER(IDOK, OnOK)
	  COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, 
                        LPARAM lParam, BOOL& bHandled)
   {
      // Do some initialization code
	  CenterWindow(GetParent());
	  CheckDlgButton(IDC_RADIO_ALL, BST_CHECKED);
//	  ::SetFocus(GetDlgItem(IDC_EDIT_NAME));
      return 1;
   }
/*
   LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
	   if((HWND)wParam == m_hWnd){
		    ::SetFocus(GetDlgItem(IDC_EDIT_NAME));
			bHandled = true;
	   }
	   return 1;
   }
*/
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
	   BSTR bstrName = NULL;
	   BOOL bRet = GetDlgItemText(IDC_EDIT_NAME, bstrName);
	   mbstrName.Attach(bstrName);
	   bool bLost = IsDlgButtonChecked(IDC_CHK_LOST) == BST_CHECKED;
	   mbLost = bLost?TRUE:FALSE;
	   bool iAll = IsDlgButtonChecked(IDC_RADIO_ALL) == BST_CHECKED;
	   mType = OT_ALL;
	   if(iAll)
		   mType = OT_ALL;
	   bool i3DObject = IsDlgButtonChecked(IDC_RADIO_3DO) == BST_CHECKED;
	   if(i3DObject)
		   mType = OT_3DOBJECTS;
	   bool iCharacter = IsDlgButtonChecked(IDC_RADIO_CHARACTER) == BST_CHECKED;
	   if(iCharacter)
		   mType = OT_CHARACTERS;
	   bool iGroup = IsDlgButtonChecked(IDC_RADIO_GROUP) == BST_CHECKED;
	   if(iGroup)
		   mType = OT_GROUPS;
	   bool iComposition = IsDlgButtonChecked(IDC_RADIO_COMP) == BST_CHECKED;
	   if(iComposition)
		   mType = OT_COMPOSITIONS;
	   return 0;
   } 

   LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   {
     EndDialog(wID);
     return 0;
   }

   LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   {
     EndDialog(wID);
     return 0;
   }
};
