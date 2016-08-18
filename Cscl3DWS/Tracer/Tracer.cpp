// Tracer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <windowsx.h>
//#include "..\oms\oms_context.h"
//#include "..\rmml\rmml.h"
//#include "..\rmml\rmmld.h"
#include "Tree.h"
#include "textColor.h"

#define WCDC_GET_JSTREE 4

#define MAX_LOADSTRING 100

enum ViewMode{
	TRVM_TRACE,
	TRVM_TREE
}geViewMode=TRVM_TRACE;

bool gbFindMode = false;
bool gbFilterMode = false;
DWORD gdwFindUpPos = 0;
DWORD gdwFindDownPos = 0;

struct SFilter
{
	std::string sFilter;
	bool bUsed;
};

struct SSender
{
	std::string sName;
	HWND hWnd;
};

std::vector<SSender> gvSenders;
std::vector<SFilter> gvFilters;


// Global Variables:
HINSTANCE hInst;											// current instance
HINSTANCE g_hRichEditLib = NULL;
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];						// The title bar text

HWND ghWnd;
HWND ghwndFind=NULL;
HWND ghwndFilter=NULL;
//HWND ghwndSelectSender=NULL;
WNDPROC hwndTraceEditProc;
WNDPROC hwndFindDialogProc;
WNDPROC hwndFilterDialogProc;

bool globalTracer = false;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndTraceEditProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndFindDialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndFilterDialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndSelectSenderDialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


#define _countof(array) (sizeof(array)/sizeof(array[0]))

inline void TrcTrace(LPCTSTR lpszFormat, ...){
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[2000];

	nBuf = _vsnprintf(szBuffer, _countof(szBuffer), lpszFormat, args);

	::OutputDebugString(szBuffer);

	va_end(args);
}

#define TRACE              ::TrcTrace
//////////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	if (lpCmdLine)
	if (strcmp(lpCmdLine, "debug") == 0)
	{
		globalTracer = true;
	}
	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TRACER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg) || !TranslateAccelerator(ghWnd, hAccelTable, &msg)) 
		{
			if (!IsWindow(ghwndFind) || !IsDialogMessage(ghwndFind, &msg))
			{
				if (!IsWindow(ghwndFilter) || !IsDialogMessage(ghwndFilter, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
//////////////////////////////////////////////////////////////////////////////////////
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TRACER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_TRACER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
//////////////////////////////////////////////////////////////////////////////////////
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   ghWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!ghWnd)
   {
      return FALSE;
   }

   ShowWindow(ghWnd, nCmdShow);
   UpdateWindow(ghWnd);

   return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////
void CheckError(){
	if(GetLastError()==0) return;
	LPVOID lpMsgBuf;
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
	   // Handle the error.
	   return;
	}

	// Process any inserts in lpMsgBuf.
	// ...

	// Display the string.
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );
}
//////////////////////////////////////////////////////////////////////////////////////

HWND ghwndTraceEdit=NULL;
HFONT ghfEditFont=NULL;
HBRUSH ghbEditBG=NULL;
HBRUSH ghbFindBG=NULL;

HWND ghwndBut=NULL;

HMENU ghmPopupMenu;

//////////////////////////////////////////////////////////////////////////////////////
void CreateTracerList(HWND hWndParent){
	INITCOMMONCONTROLSEX icex;

	// Ensure that the common control DLL is loaded. 

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex); 

	RECT rt;
	GetClientRect(hWndParent, &rt);

	if(g_hRichEditLib == NULL) 
				g_hRichEditLib = LoadLibrary("RICHED20.DLL");
	ghwndTraceEdit=CreateWindow(TEXT(RICHEDIT_CLASS),TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL // | WS_BORDER
		| ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY //    DWORD dwStyle, 
		,
		0,//5, //    int x,
		0,//3, //    int y,
		rt.right,//rt.right-10, // rt.right, //    int nWidth,
		rt.bottom,//rt.bottom-6, // rt.bottom, //    int nHeight,
		hWndParent, //    HWND hWndParent,
		(HMENU)1, //    HMENU hMenu,
		hInst, //    HINSTANCE hInstance,
		NULL //    LPVOID lpParam
	);

	HFONT hf=(HFONT)::SendMessage(ghwndTraceEdit,WM_GETFONT,0,0);
	LOGFONT lf;
	if(hf==NULL){
		hf=(HFONT)GetStockObject(SYSTEM_FONT);
	}
	::GetObject(hf, sizeof(LOGFONT), &lf);
	lf.lfWeight=FW_NORMAL;
	strcpy(lf.lfFaceName,"MS Sans Serif"); // MS Sans Serif
	hf=CreateFontIndirect(&lf);
	::SendMessage(ghwndTraceEdit,WM_SETFONT,(WPARAM)hf,0);
	ghfEditFont=hf;
	LOGBRUSH lb;
	lb.lbStyle=BS_SOLID;
	lb.lbColor=RGB(255,255,255);
	lb.lbHatch=HS_BDIAGONAL;
	ghbEditBG=CreateBrushIndirect(&lb);

	ghmPopupMenu = CreatePopupMenu();

	AppendMenu(ghmPopupMenu, MF_STRING , ID_COPY, "Copy");
	AppendMenu(ghmPopupMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(ghmPopupMenu, MF_STRING , ID_SELECTALL, "Select all");
	AppendMenu(ghmPopupMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(ghmPopupMenu, MF_STRING , ID_EDIT_CLEARTRACER, "Clear trace");
	
	// subclass window
	hwndTraceEditProc = SubclassWindow(ghwndTraceEdit, WndTraceEditProc);
	
	BOOL bRet=ShowWindow(ghwndTraceEdit,SW_SHOW);
}

//////////////////////////////////////////////////////////////////////////////////////

void CreateFindDialog(HWND hWndParent)
{
	ghwndFind = CreateDialog(hInst,
			MAKEINTRESOURCE(IDD_DIALOGFIND), hWndParent, (DLGPROC)WndFindDialogProc);
}

//////////////////////////////////////////////////////////////////////////////////////

void CreateFilterDialog(HWND hWndParent)
{
	ghwndFilter = CreateDialog(hInst,
			MAKEINTRESOURCE(IDD_DIALOGFILTER), hWndParent, (DLGPROC)WndFilterDialogProc);
}

//////////////////////////////////////////////////////////////////////////////////////

/*void CreateSelectSenderDialog(HWND hWndParent)
{
	ghwndSelectSender = CreateDialog(hInst,
			MAKEINTRESOURCE(IDD_DIALOG_SELECT_SENDER), hWndParent, (DLGPROC)WndSelectSenderDialogProc);
}*/

//////////////////////////////////////////////////////////////////////////////////////

std::string objSign = "SM: [!OBJ!]";

void AppendTextImpl(char* apszText,unsigned int auSize){
	if(apszText==NULL || apszText[0]==0 || auSize==0 ) return;

	if (globalTracer == false)
	{
		if (strlen(apszText) < objSign.size())
		{
			return;
		}
		
		if (memcmp(apszText, objSign.c_str(), objSign.size()) != 0)		
		{			
			return;
		}

		apszText += objSign.size();
		auSize -= objSign.size();		
	}

	LRESULT lLineCount=::SendMessage(ghwndTraceEdit,EM_GETLINECOUNT,0,0);
	DWORD dwLastLinePos=::SendMessage(ghwndTraceEdit,EM_LINEINDEX,(WPARAM)lLineCount>0?lLineCount-1:0,0);
	LRESULT lLength=::SendMessage(ghwndTraceEdit,WM_GETTEXTLENGTH,0,0);

	char* buffer = new char[auSize+1];
	char* begin = buffer;

	bool start_hex = false;
	bool was_end_color_tag = false;
	char* start_color_pos = NULL;
	char* end_color_pos = NULL;
	char color[7];
	
	for(unsigned int i=0; i<auSize; i++, apszText++)
	{
		if (start_hex)
		{
			color[0]='#';
			start_hex = false;
			int hex_count = 0;
			
			for (hex_count=1; hex_count<=6 ; hex_count++)
			{
				if ((*apszText >= 'A' && *apszText <= 'F') ||
					(*apszText >= 'a' && *apszText <= 'f') ||
					(*apszText >= '0' && *apszText <= '9'))
				{
					color[hex_count] = *apszText;
					i++;
					apszText++;
				}
				else
				{
					// not color
					break;
				}
			}
			color[hex_count] = '\0';
			hex_count--;
			if (hex_count==6)
			{
				start_color_pos = buffer;
				//read until end of line or next "#"
				while(i<auSize && *apszText!=0xA && *apszText!='#')
				{
					if (*apszText=='\\' && *(apszText+1)=='#')
					{
						*buffer++=_T('#');
						// skip "\"
						apszText++;
						i++;
					}
					else
					{
						*buffer++=*apszText;
					}
					apszText++;
					i++;
				}
				if (*apszText=='#')
				{
					was_end_color_tag = true;
					apszText++;
					i++;
				}
				end_color_pos = buffer;

				STextColor clr;
				
				// parse colors
				char* endptr;
				char byte_string[2];
				// red
				byte_string[0] = color[1];
				byte_string[1] = color[2];
				clr.r = strtol(byte_string, &endptr, 16);
				// green
				byte_string[0] = color[3];
				byte_string[1] = color[4];
				clr.g = strtol(byte_string, &endptr, 16);
				// blue
				byte_string[0] = color[5];
				byte_string[1] = color[6];
				clr.b = strtol(byte_string, &endptr, 16);
				
				clr.start_color_pos = start_color_pos;// - begin + dwLastLinePos;
				clr.end_color_pos = end_color_pos;// - begin + dwLastLinePos;
				gcColors.push_back(clr);
			}
			else
			{
				// if not color restore last characters
				apszText -= hex_count;
				i++;

				// if after first color found "#" without new color, skipping it
				if (was_end_color_tag == false)
				{
					apszText--;
					i--;
				}
				for (int j=0; j<hex_count+1; j++, apszText++)
				{
					*buffer++=*apszText;
				}
			}
		}

		if (*apszText=='#')
		{
			start_hex = true;
			continue;
		}

		was_end_color_tag = false;
		
		if (*apszText=='\\' && *(apszText+1)=='#')
		{
			*buffer++=_T('#');
			apszText++;
			i++;
			continue;
		}

		if(*apszText==0xA)
		{
			*buffer++=_T('\n');
		}
		else
		{
			*buffer++=*apszText;
		}
	}
	*buffer++=_T('\0');

	// start filtering
	std::vector<SFilter>::const_iterator vciFilter = gvFilters.begin();
	std::vector<SFilter>::const_iterator vcieFilter = gvFilters.end();
	for (; vciFilter!=vcieFilter; vciFilter++)
	{
		if (vciFilter->bUsed && vciFilter->sFilter.length()>0)
		{
			std::string sFilter;
			sFilter = vciFilter->sFilter;
			int len = sFilter.length();

			// find first occurrence of filter in buffer
			char* pos = strstr(begin, sFilter.c_str());
			while (pos!=NULL)
			{
			
				// recalc colors
				char* endpos = pos + len;
				std::vector<STextColor>::iterator vci = gcColors.begin();
				std::vector<STextColor>::iterator vce = gcColors.end();
				for (vci; vci != vce; vci++)
				{
					int innerColor = 0;
		
					// check borders of filter and color
					if (endpos < vci->start_color_pos)
					{			// startfilter endfilter startcolor endcolor
						vci->start_color_pos -= len;
						vci->end_color_pos -= len;
					}
					else if (pos <= vci->start_color_pos && vci->start_color_pos <= endpos && endpos <= vci->end_color_pos)
					{			// startfilter startcolor endfilter endcolor
						//vci->end_color_pos -= len;
						vci->start_color_pos = pos;
					}
					else if (pos <= vci->start_color_pos && vci->end_color_pos <= endpos)
					{			// startfilter startcolor endcolor endfilter 
						// delete color
						gcColors.erase(vci);
					}
					else if (vci->start_color_pos <= pos && pos <= vci->end_color_pos && vci->end_color_pos <= endpos)
					{			// startcolor startfilter endcolor endfilter 
						vci->end_color_pos = pos;
					}
					else if (vci->start_color_pos <= pos && endpos <= vci->end_color_pos)
					{			// startcolor startfilter endfilter endcolor
						vci->end_color_pos -= len;
					}
				}
				// filter characters
				int e = strlen(begin)-(endpos-pos);
				memcpy(pos, endpos, strlen(endpos));
				begin[e]='\0';

				// find next occurrence of filter in buffer
				pos = strstr(begin, sFilter.c_str());
			}
		}
	}

	//set selection for insertion
	::SendMessage(ghwndTraceEdit, EM_SETSEL, lLength, lLength);

	// insert text
	::SendMessage(ghwndTraceEdit, EM_REPLACESEL, 0, (LPARAM)begin);

	// set colors
	std::vector<STextColor>::const_iterator vci = gcColors.begin();
	std::vector<STextColor>::const_iterator vce = gcColors.end();
	for (vci; vci != vce; vci++)
	{
		STextColor clr = *vci;
		CHARFORMAT cfmt;
		ZeroMemory(&cfmt, sizeof(cfmt));
		cfmt.cbSize = sizeof(CHARFORMAT);
		cfmt.dwMask = CFM_COLOR;
		cfmt.crTextColor = RGB(clr.r, clr.g, clr.b);
		::SendMessage(ghwndTraceEdit, EM_SETSEL, clr.start_color_pos-begin+dwLastLinePos, clr.end_color_pos-begin+dwLastLinePos);
		::SendMessage(ghwndTraceEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfmt);
	}

	gcColors.clear();
	buffer = NULL;
	delete begin;

}

void AppendText(char* apszText,unsigned int auSize)
{
	if(apszText==NULL || apszText[0]==0 || auSize==0 ) return;
	std::string str = "";
	char tmp[2];
	tmp[1] = 0;	
	int i = 0;
	while (i < auSize)
	{
		tmp[0] = apszText[i];
		str += 	tmp;
		if (apszText[i] == 10)
		{
			AppendTextImpl((char*)str.c_str(), str.size());
			str = "";
		}
		i++;
	}

	if (str.size() > 0)
	{
		AppendTextImpl((char*)str.c_str(), str.size());
	}
}

//////////////////////////////////////////////////////////////////////////////////////

HWND hwndSender=NULL;

//////////////////////////////////////////////////////////////////////////////////////
void UpdateSnedersList()
{
	gvSenders.clear();
	HWND lhwndSender=NULL;
	// перебираем все эксплореры
	HWND  hwndIE = NULL;
	hwndIE=::FindWindowEx(NULL,hwndIE,"IEFrame",NULL);;
	while(hwndIE){
		HWND hwndView=::FindWindowEx(hwndIE,NULL,"Shell DocObject View",NULL);
		if(hwndView!=NULL){
			HWND hwndServer=::FindWindowEx(hwndView,NULL,"Internet Explorer_Server",NULL);
			lhwndSender=::FindWindowEx(hwndServer, NULL, NULL,"3DWS");
			if(lhwndSender!=NULL)
			{
				char buffer[500];
				GetWindowText(lhwndSender, buffer, 500);
				SSender sender;
				sender.hWnd = lhwndSender;
				sender.sName = buffer;
				gvSenders.push_back(sender);
			}
		}
		hwndIE=::FindWindowEx(NULL, hwndIE, "IEFrame", NULL);
	}

	// ищем OMSPlayer
	HWND hwndOMSPlayer=NULL;
	lhwndSender=::FindWindowEx(NULL, hwndOMSPlayer, "VSpaces virtual worlds player", NULL);
	while(lhwndSender)
	{
		char buffer[500];
		GetWindowText(lhwndSender, buffer, 500);
		SSender sender;
		sender.hWnd = lhwndSender;
		sender.sName = buffer;
		gvSenders.push_back(sender);
		lhwndSender=::FindWindowEx(NULL,lhwndSender,"OMSPLAYERRNMC",NULL);
	}

	hwndOMSPlayer=NULL;
	hwndOMSPlayer=::FindWindowEx(NULL,hwndOMSPlayer,"OMSPLAYER",NULL);
	if(hwndOMSPlayer!=NULL)
	{
		lhwndSender=::FindWindowEx(hwndOMSPlayer,NULL,NULL,"3DWS");
		if (lhwndSender)
		{
			char buffer[500];
			GetWindowText(lhwndSender, buffer, 500);
			SSender sender;
			sender.hWnd = lhwndSender;
			sender.sName = buffer;
			gvSenders.push_back(sender);
		}
		hwndOMSPlayer=::FindWindowEx(NULL, hwndOMSPlayer, "OMSPLAYER", NULL);
	}
}

void FindSender()
{
	//UpdateSnedersList();

	if (gvSenders.size() == 0)
	{
		hwndSender = NULL;
	}
	else
	{
		std::vector<SSender>::const_iterator vci = gvSenders.begin();
		std::vector<SSender>::const_iterator vce = gvSenders.end();
		bool found = false;
		for (; vci!=vce; vci++)
		{
			if (hwndSender==vci->hWnd)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			if (gvSenders.size()==1)
			{
				hwndSender=gvSenders[0].hWnd;
			}
			else
			{
				DialogBox(hInst, (LPCTSTR)IDD_DIALOG_SELECT_SENDER, ghWnd, (DLGPROC)WndSelectSenderDialogProc);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void ScrollTraceListToBottom(){
//	ListView_EnsureVisible(ghwndTraceList,ListView_GetItemCount(ghwndTraceList)-1,FALSE);
	/*LRESULT lLineCount=::SendMessage(ghwndTraceEdit,EM_GETLINECOUNT,0,0);
	LRESULT dwLastLinePos=::SendMessage(ghwndTraceEdit,EM_LINEINDEX,(WPARAM)lLineCount>0?lLineCount-1:0,0);*/
	LRESULT lLength=::SendMessage(ghwndTraceEdit,WM_GETTEXTLENGTH,0,0);
	::SendMessage(ghwndTraceEdit,EM_SETSEL,(WPARAM)lLength,(LPARAM)lLength);
	::SendMessage(ghwndTraceEdit,EM_SCROLLCARET,0,0);
}
//////////////////////////////////////////////////////////////////////////////////////
void GetAllData()
{
	COPYDATASTRUCT CopyData;
	CopyData.cbData=0;
	CopyData.lpData=NULL;
	::SendMessage(ghwndTraceEdit, WM_SETTEXT, 0, (LPARAM) "Tracer was connected to vAcademia\r\n"); 
	ScrollTraceListToBottom();
	CopyData.dwData=0; // get all log
	::SendMessage( hwndSender, WM_COPYDATA, (WPARAM)ghWnd, (LPARAM)(LPVOID) &CopyData);
}

void UpdateTraceList(){
	bool bFromBegin=true;
	bFromBegin=false;
	if(hwndSender==NULL || !IsWindow(hwndSender)){
		FindSender();
		bFromBegin=true;
		}
	if(hwndSender==NULL){
		LRESULT lLength=::SendMessage(ghwndTraceEdit, WM_GETTEXTLENGTH, 0, (LPARAM) NULL);
		DWORD dwStartPos=0;
		DWORD dwCarretPos=0;
		::SendMessage(ghwndTraceEdit,EM_GETSEL,(WPARAM)&dwStartPos,(LPARAM)&dwCarretPos);
		::SendMessage(ghwndTraceEdit, WM_SETTEXT, 0, (LPARAM) "vAcademia was not found\r\n");
		if (lLength == 0)
		{
			ScrollTraceListToBottom();
		}
		else
		{
			::SendMessage(ghwndTraceEdit,EM_SETSEL,(WPARAM)dwStartPos,(LPARAM)dwCarretPos);
		}
		return;
	}
	COPYDATASTRUCT CopyData;
	CopyData.cbData=0;
	CopyData.lpData=NULL;

	if(bFromBegin){
		::SendMessage(ghwndTraceEdit, WM_SETTEXT, 0, (LPARAM) "Tracer was connected to vAcademia\r\n"); 
		ScrollTraceListToBottom();
		CopyData.dwData=0; // get all log
	}else{
		// берем только обновление
		CopyData.dwData=1; // get only new data
	}
	::SendMessage( hwndSender, WM_COPYDATA, (WPARAM)ghWnd, (LPARAM)(LPVOID) &CopyData);
}
//////////////////////////////////////////////////////////////////////////////////////
void ClearTraceList()
{
	if(hwndSender==NULL || !IsWindow(hwndSender))
	{
		FindSender();
	}
	if(hwndSender==NULL){
		::SendMessage(ghwndTraceEdit, WM_SETTEXT, 0, (LPARAM) "vAcademia was not found\r\n"); 
		ScrollTraceListToBottom();
		return;
	}
	::SendMessage(ghwndTraceEdit, WM_SETTEXT, 0, (LPARAM) "Tracer was connected to vAcademia\r\n"); 
	ScrollTraceListToBottom();
	//::SendMessage( hwndSender, WM_COPYDATA, (WPARAM)ghWnd, (LPARAM)(LPVOID) &CopyData);
}
void SelectAllTraceList()
{
	DWORD lLineCount=::SendMessage(ghwndTraceEdit,EM_GETLINECOUNT,0,0);
	DWORD dwLastLinePos=::SendMessage(ghwndTraceEdit,EM_LINEINDEX,(WPARAM)lLineCount>0?lLineCount-1:0,0);
	::SendMessage(ghwndTraceEdit , EM_SETSEL, 0, dwLastLinePos);
}
//////////////////////////////////////////////////////////////////////////////////////
void UpdateTree(){
	if(hwndSender==NULL || !IsWindow(hwndSender)){
		FindSender();
	}
	if(hwndSender==NULL){
		// Чистим TreeCtrl
		TreeView_DeleteAllItems(ghwndTree);
		return;
	}
	COPYDATASTRUCT CopyData;
	CopyData.cbData=0;
	CopyData.lpData=NULL;
	CopyData.dwData=WCDC_GET_JSTREE;
	::SendMessage( hwndSender, WM_COPYDATA, (WPARAM)ghWnd, (LPARAM)(LPVOID) &CopyData);
}
//////////////////////////////////////////////////////////////////////////////////////
bool gbPause=false;

void SetPauseMenuItemText(HWND ahWnd, const char* apszText){
	HMENU hMenu=GetMenu(ahWnd);
	MENUITEMINFO MIInfo; MIInfo.cbSize=sizeof(MENUITEMINFO);
	MIInfo.fMask=0x40; //MIIM_STRING;
	MIInfo.dwTypeData=(char*)apszText;
	BOOL bRes=SetMenuItemInfo(hMenu,IDM_PAUSE,FALSE,&MIInfo);
	bRes=RedrawWindow(ahWnd,NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
}
//////////////////////////////////////////////////////////////////////////////////////
void InverseOnTopState(HWND ahWnd){
	HMENU hMenu=GetMenu(ahWnd);
	MENUITEMINFO MIInfo; MIInfo.cbSize=sizeof(MENUITEMINFO);
	MIInfo.fMask=MIIM_STATE; //MIIM_STRING;
	BOOL bRes=GetMenuItemInfo(hMenu,ID_WINDOW_ALWAYSONTOP,FALSE,&MIInfo);
	RECT rc;
	GetWindowRect(ahWnd,&rc);
	if(MIInfo.fState==MFS_CHECKED){
		MIInfo.fState=MFS_UNCHECKED;
		SetWindowPos(ahWnd,HWND_NOTOPMOST,rc.left,rc.top,rc.right-rc.left, rc.bottom-rc.top,0);
	}else{
		MIInfo.fState=MFS_CHECKED;
		SetWindowPos(ahWnd,HWND_TOPMOST,rc.left,rc.top,rc.right-rc.left, rc.bottom-rc.top,0);
	}
	bRes=SetMenuItemInfo(hMenu,ID_WINDOW_ALWAYSONTOP,FALSE,&MIInfo);
}
//////////////////////////////////////////////////////////////////////////////////////
void SetMenuItemCheck(HWND ahWnd,UINT auItemID,bool abChecked){
	HMENU hMenu=GetMenu(ahWnd);
	MENUITEMINFO MIInfo; MIInfo.cbSize=sizeof(MENUITEMINFO);
	MIInfo.fMask=MIIM_STATE; 
	BOOL bRes;
//	bRes=GetMenuItemInfo(hMenu,auItemID,FALSE,&MIInfo);
	MIInfo.fState=abChecked?MFS_CHECKED:MFS_UNCHECKED;
	bRes=SetMenuItemInfo(hMenu,auItemID,FALSE,&MIInfo);
}
//////////////////////////////////////////////////////////////////////////////////////
void SetMenuItemEnabled(HWND ahWnd,UINT auItemID,bool abEnabled){
	HMENU hMenu=GetMenu(ahWnd);
	MENUITEMINFO MIInfo; MIInfo.cbSize=sizeof(MENUITEMINFO);
	MIInfo.fMask=MIIM_STATE; 
	BOOL bRes;
	MIInfo.fState=abEnabled?MFS_ENABLED:MFS_DISABLED;
	bRes=SetMenuItemInfo(hMenu,auItemID,FALSE,&MIInfo);
}
//////////////////////////////////////////////////////////////////////////////////////
void SetViewMode(HWND ahWnd,ViewMode aNewViewMode){
	if(geViewMode==aNewViewMode) return;
	if(geViewMode==TRVM_TRACE){
		// uncheck 'Trace'
		SetMenuItemCheck(ahWnd,ID_VIEW_TRACE,false);
		//
		ShowWindow(ghwndTraceEdit,SW_HIDE);
	}else if(geViewMode==TRVM_TREE){
		SetMenuItemCheck(ahWnd,ID_VIEW_TREE,false);
		//
		ShowWindow(ghwndTree,SW_HIDE);
	}
	if(aNewViewMode==TRVM_TRACE){
		SetMenuItemCheck(ahWnd,ID_VIEW_TRACE,true);
		SetMenuItemEnabled(ahWnd,ID_EDIT_COPY,true);
		SetMenuItemEnabled(ahWnd,ID_EDIT_SELECTALL,true);
		SetMenuItemEnabled(ahWnd,ID_EDIT_CLEARTRACER,true);
		SetMenuItemEnabled(ahWnd,ID_EDIT_FIND,true);
		SetMenuItemEnabled(ahWnd,ID_EDIT_FILTER,true);

		ShowWindow(ghwndTraceEdit,SW_SHOW);
	}else if(aNewViewMode==TRVM_TREE){
		SetMenuItemCheck(ahWnd,ID_VIEW_TREE, true);
		SetMenuItemEnabled(ahWnd,ID_EDIT_COPY, false);
		SetMenuItemEnabled(ahWnd,ID_EDIT_SELECTALL, false);
		SetMenuItemEnabled(ahWnd,ID_EDIT_CLEARTRACER, false);
		SetMenuItemEnabled(ahWnd,ID_EDIT_FIND, false);
		SetMenuItemEnabled(ahWnd,ID_EDIT_FILTER, false);
		TreeView_DeleteAllItems(ghwndTree);
		ShowWindow(ghwndTree, SW_SHOW);
	}
	geViewMode=aNewViewMode;
}

void SaveFilters()
{
	gvFilters.clear();
	//IDC_CHECK_FILTER1 1030
	int res_check_filter = 1017;
	int res_filter = 1030;
	for (int i=0; i<3; i++)
	{
		SFilter filter;
		HWND hwndCheck = GetDlgItem(ghwndFilter, res_check_filter+i);
		DWORD dCheck=::SendMessage(hwndCheck, BM_GETCHECK, 0, 0);
		if (dCheck == BST_CHECKED)
		{
			filter.bUsed = true;
		}
		else
		{
			filter.bUsed = false;
		}

		char sFilter[1000];
		LRESULT lLength=::SendDlgItemMessage(ghwndFilter, res_filter+i, WM_GETTEXTLENGTH, 0, (LPARAM) NULL);
		::SendDlgItemMessage (ghwndFilter, res_filter+i, WM_GETTEXT, lLength+1, (long)sFilter);

		filter.sFilter = sFilter;
		int pos = filter.sFilter.find("\\n");
		while (pos!=-1)
		{
			std::string tmp = filter.sFilter.substr(pos+2, filter.sFilter.length());
			filter.sFilter = filter.sFilter.substr(0, pos);
			filter.sFilter +=  '\n';
			filter.sFilter += tmp;
			pos = filter.sFilter.find("\\n");
		}

		gvFilters.push_back(filter);
	}

}

void LoadFilters()
{
	//IDC_CHECK_FILTER1 1030
	int res_check_filter = 1017;
	int res_filter = 1030;
	int i=0;
	std::vector<SFilter>::const_iterator vciFilter = gvFilters.begin();
	std::vector<SFilter>::const_iterator vcieFilter = gvFilters.end();
	for (; vciFilter!=vcieFilter; vciFilter++)
	{
		SFilter filter = *vciFilter;
		HWND hwndCheck = GetDlgItem(ghwndFilter, res_check_filter+i);
		if (filter.bUsed)
		{
			DWORD dCheck=::SendMessage(hwndCheck, BM_SETCHECK, BST_CHECKED, 0);
		}
		else
		{
			DWORD dCheck=::SendMessage(hwndCheck, BM_SETCHECK, BST_UNCHECKED, 0);
		}
		int pos = filter.sFilter.find('\n');
		while (pos!=-1)
		{
			std::string tmp = filter.sFilter.substr(pos+1, filter.sFilter.length());
			filter.sFilter = filter.sFilter.substr(0, pos);
			filter.sFilter +=  "\\n";
			filter.sFilter += tmp;
			pos = filter.sFilter.find('\n');
		}
		

		::SendDlgItemMessage(ghwndFilter, res_filter+i, WM_SETTEXT, 0, (LPARAM)filter.sFilter.c_str());
		i++;
	}

}

void SetFilterMode(bool mode)
{
	gbFilterMode = mode;
	RECT rt;
	GetClientRect(ghWnd, &rt);
	if (gbFilterMode)
	{
		RECT rtFilter;
		::GetWindowRect(ghwndFilter, &rtFilter);
		// decrease edit control size
		::MoveWindow(ghwndTraceEdit,
			5,
			3,
			rt.right-5,
			rt.bottom-rtFilter.bottom+rtFilter.top-3,
			TRUE);
		// set position of find dialog
		::MoveWindow(ghwndFilter,
			0,
			rt.bottom-rtFilter.bottom+rtFilter.top,
			rt.right,
			rtFilter.bottom-rtFilter.top,
			TRUE);
		SetFocus(ghwndFilter);
		ShowWindow(ghwndFilter, SW_SHOW);
	}
	else
	{
		::MoveWindow(ghwndTraceEdit,5,3,rt.right-5,rt.bottom-3,TRUE);
		ShowWindow(ghwndFilter, SW_HIDE);
	}	
}

//////////////////////////////////////////////////////////////////////////////////////
void SetFindMode(bool mode)
{
	gbFindMode = mode;
	RECT rt;
	GetClientRect(ghWnd, &rt);
	if (gbFindMode)
	{
		RECT rtFind;
		::GetWindowRect(ghwndFind, &rtFind);
		// decrease edit control size
		::MoveWindow(ghwndTraceEdit,
			5,
			3,
			rt.right-5,
			rt.bottom-rtFind.bottom+rtFind.top-3,
			TRUE);
		// set position of find dialog
		::MoveWindow(ghwndFind,
			0,
			rt.bottom-rtFind.bottom+rtFind.top,
			rt.right,
			rtFind.bottom-rtFind.top,
			TRUE);
		SetFocus(ghwndFind);
		ShowWindow(ghwndFind, SW_SHOW);
	}
	else
	{
		::MoveWindow(ghwndTraceEdit,5,3,rt.right-5,rt.bottom-3,TRUE);
		ShowWindow(ghwndFind, SW_HIDE);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void FindInTraceEdit()
{
	char buff[50];
	HWND hwndEdit = GetDlgItem(ghwndFind, IDC_EDITFIND);
	int iLength=GetWindowText(hwndEdit,buff, 50);
	if (iLength!=0)
	{
		// get direction
		HWND hwndDirection = GetDlgItem(ghwndFind, IDC_RADIO2);
		DWORD dFindNext=::SendMessage(hwndDirection, BM_GETCHECK, 0, 0);
		bool bFindNext=0;
		if (dFindNext == BST_CHECKED)
		{
			bFindNext = 1;
		}

		DWORD dwSearchDirection;
		if (bFindNext==1)
		{
			dwSearchDirection = FR_DOWN;
		}
		else
		{
			dwSearchDirection = 0;
		}

		// get case sensetivity
		HWND hwndCheck = GetDlgItem(ghwndFind, IDC_CHECK1);
		DWORD lMatchCaseChecked=::SendMessage(hwndCheck, BM_GETCHECK, 0, 0);
		DWORD dwMatchCase = 0;
		if (lMatchCaseChecked == BST_CHECKED)
		{
			dwMatchCase = FR_MATCHCASE;
		}
		LRESULT lLength=::SendMessage(ghwndTraceEdit, WM_GETTEXTLENGTH, 0, (LPARAM) NULL);
		CHARRANGE cr;
		if (bFindNext)
		{
			cr.cpMin = gdwFindUpPos;
			cr.cpMax = lLength;
		}
		else
		{
			cr.cpMin = gdwFindDownPos;
			cr.cpMax = lLength;
		}
		FINDTEXT ft;
		ft.chrg = cr;
		ft.lpstrText = buff;
		LRESULT lPos = ::SendMessage(ghwndTraceEdit, EM_FINDTEXT, dwSearchDirection|dwMatchCase, (LPARAM)&ft);
		if (lPos==-1)
		{
			gdwFindUpPos = 0;
			gdwFindDownPos = lLength-1;
			if (bFindNext)
			{
				::SendDlgItemMessage(ghwndFind, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)"Passed the end of the file.");
				//MessageBox(ghWnd, "Passed the end of the file", "Find Dialog", MB_ICONINFORMATION);
			}
			else
			{
				::SendDlgItemMessage(ghwndFind, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)"Passed the begin of the file.");
				//MessageBox(ghWnd, "Passed the begin of the file", "Find Dialog", MB_ICONINFORMATION);
			}
			HWND hwndEdit = GetDlgItem(ghwndFind, IDC_EDITFIND);
			SetFocus(hwndEdit);
		}
		else
		{
			SetFocus(ghwndTraceEdit);
			::SendMessage(ghwndTraceEdit, EM_SETSEL, lPos, lPos+iLength);
			char cStatus[30];
			wsprintf(cStatus, "Current position: %d", lPos);
			::SendDlgItemMessage(ghwndFind, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)cStatus);
			gdwFindUpPos = lPos+iLength;
			gdwFindDownPos = lPos;
		}
	}
	else
	{
		HWND hwndEdit = GetDlgItem(ghwndFind, IDC_EDITFIND);
		SetFocus(hwndEdit);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void SetFindPosFromCursor()
{
	DWORD dwStartPos=0;
	DWORD dwEndPos=0;
	::SendMessage(ghwndTraceEdit,EM_GETSEL,(WPARAM)&dwStartPos,(LPARAM)&dwEndPos);
	gdwFindUpPos = dwEndPos;
	gdwFindDownPos = dwEndPos;

	char cStatus[30];
	wsprintf(cStatus, "Current position: %lu", dwEndPos);
	::SendDlgItemMessage(ghwndFind, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)cStatus);
}

bool gbAfterUndo=false;

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
//	TCHAR szHello[MAX_LOADSTRING];
//	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_CREATE:
		{
			CreateTracerList(hWnd);
			CreateTreeCtrl(hInst,hWnd);
			CreateFindDialog(hWnd);
			CreateFilterDialog(hWnd);
			//CreateSelectSenderDialog(hWnd);
			geViewMode=TRVM_TREE;
			SetViewMode(hWnd,TRVM_TRACE);
			::SetTimer(hWnd,1,1000,NULL);
			break;
		}
		case WM_TIMER:
			if(!gbPause)
			{
				UpdateSnedersList();
				if(geViewMode==TRVM_TRACE)
				{
					if (gbFindMode == false && gbFilterMode == false)
						SetFocus(ghwndTraceEdit);
					UpdateTraceList();
				}
				else if(geViewMode==TRVM_TREE)
				{
					SetFocus(ghwndTree);
					UpdateTree();
				}
			}
			break;
		case WM_COPYDATA:
			if(lParam!=NULL)
			{
				COPYDATASTRUCT* pCopyData=(COPYDATASTRUCT*)lParam;
				if(pCopyData->dwData <= 1){
					char* pText=(char*)pCopyData->lpData;
					if (pText)
					if (pText[0] != 0)
					{
						DWORD dwStartPos=0;
						DWORD dwCarretPos=0;
						::SendMessage(ghwndTraceEdit,EM_GETSEL,(WPARAM)&dwStartPos,(LPARAM)&dwCarretPos);
						LRESULT lLineCount=::SendMessage(ghwndTraceEdit,EM_GETLINECOUNT,0,0);
						DWORD dwLastLinePos=::SendMessage(ghwndTraceEdit,EM_LINEINDEX,(WPARAM)lLineCount>0?lLineCount-1:0,0);
						bool bScrollToBottom=false;
						if(dwCarretPos >= dwLastLinePos)
							bScrollToBottom=true;
						LRESULT lFirstVisibleLine=::SendMessage(ghwndTraceEdit,EM_GETFIRSTVISIBLELINE,0,0);
						AppendText(pText,pCopyData->cbData);
						if(pText!=NULL && pCopyData->cbData>0){
							// если курсор на последней строке
							// то прокручиваем все вниз
	//						if(ListView_GetSelectedCount(ghwndTraceList)<=0)
							if(bScrollToBottom){
								ScrollTraceListToBottom();
							}else{
								// восстанавливаем выделение/положение курсора
								::SendMessage(ghwndTraceEdit,EM_SETSEL,(WPARAM)dwStartPos,(LPARAM)dwCarretPos);
								// перематываем на то же место
								::SendMessage(ghwndTraceEdit,WM_VSCROLL,SB_TOP,NULL);
								::SendMessage(ghwndTraceEdit, EM_LINESCROLL,0,lFirstVisibleLine);
							}
						}
					}
				}else if(pCopyData->dwData==WCDC_GET_JSTREE){
					RebuildTree(pCopyData->lpData,pCopyData->cbData);
					// ??
				}else{
//					rmmld::mlIDebugger* pDebugger=(rmmld::mlIDebugger*)pCopyData->lpData;
//					int hh=0;
					// ??
				}
			}
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam);
			switch (wmId)
			{
				case ID_REFRESH:
					// refresh
					if(geViewMode == TRVM_TRACE){
						UpdateTraceList();
					}else if(geViewMode == TRVM_TREE){
						UpdateTree();
					}
					break;
				case ID_SWITCHMODE1:
					{
						if (gbFindMode)
						{
							::SendMessage(ghwndFind, WM_NEXTDLGCTL, FALSE, FALSE);
							break;
						}
						if (gbFilterMode)
						{
							::SendMessage(ghwndFilter, WM_NEXTDLGCTL, FALSE, FALSE);
							break;
						}
					}
				case ID_SWITCHMODE:
					// switch view mode
					if(geViewMode == TRVM_TRACE)
					{
						SetViewMode(ghWnd,TRVM_TREE);
						if (gbFindMode)
						{
							// disable find mode
							SetFindMode(false);
						}
						if (gbFilterMode)
						{
							SetFilterMode(false);
						}
					}
					else if(geViewMode == TRVM_TREE)
					{
						SetViewMode(ghWnd,TRVM_TRACE);
					}
/*					if(geViewMode!=TRVM_TRACE)
					{
						SetViewMode(hWnd,TRVM_TRACE);
						::KillTimer(hWnd,1);
						::SetTimer(hWnd,1,1000,NULL);
					}
					else
					{
						if (gbFindMode)
						{
							// disable find mode
							SetFindMode(false);
						}
						SetViewMode(ghWnd,TRVM_TREE);
						::KillTimer(hWnd,1);
						::SetTimer(hWnd,1,10000,NULL);
					}*/
					break;
				case ID_COPY:
					::SendMessage(ghwndTraceEdit ,WM_COPY, 0, 0);
					break;
				case ID_SELECTALL:
					SelectAllTraceList();
					break;
				case ID_EDIT_CLEARTRACER:
					ClearTraceList();
					break;
				case ID_EDIT_FIND:
					if (gbFilterMode)
					{
						SetFilterMode(false);
					}
					// wm_showwindow message ghwndFind, setting focus to edit control
					ShowWindow(ghwndFind, SW_HIDE);
					// set find mode
					SetFindMode(true);
					break;
				case ID_EDIT_FILTER:
					if (gbFindMode)
					{
						SetFindMode(false);
					}
					ShowWindow(ghwndFilter, SW_HIDE);
					// set filter mode
					SetFilterMode(true);
					break;
				case ID_SELECT_SENDER:
					{
						DialogBox(hInst, (LPCTSTR)IDD_DIALOG_SELECT_SENDER, hWnd, (DLGPROC)WndSelectSenderDialogProc);
					}
					break;
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_PAUSE:
					if(gbPause)
					{
						gbPause=false;
						SetPauseMenuItemText(hWnd,"Pause");
					}
					else
					{
						gbPause=true;
						SetPauseMenuItemText(hWnd,"Resume");
					}
					break;
				case ID_WINDOW_ALWAYSONTOP:
					{
						InverseOnTopState(hWnd);
					}
					break;
				case ID_VIEW_TRACE:
					{
						if(geViewMode!=TRVM_TRACE)
						{
							SetViewMode(hWnd,TRVM_TRACE);
							//::KillTimer(hWnd,1);
							//::SetTimer(hWnd,1,1000,NULL);
						}
					}
					break;
				case ID_VIEW_TREE:
					{
						if(geViewMode!=TRVM_TREE)
						{
							if (gbFindMode)
							{
								// disable find mode
								SetFindMode(false);
							}
							if (gbFilterMode)
							{
								// disable filter mode
								SetFilterMode(false);
							}
							SetViewMode(hWnd,TRVM_TREE);
							//::KillTimer(hWnd,1);
							//::SetTimer(hWnd,1,10000,NULL);
						}
					}
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;

				case IDM_ESCAPE:
					{
						if (gbFindMode)
						{
							//ShowWindow(ghwndFind, SW_HIDE);
							SetFindMode(false);
							SetFocus(ghwndTraceEdit);
						}
						if (gbFilterMode)
						{
							SetFilterMode(false);
							SetFocus(ghwndTraceEdit);
						}
					}
					break;
				case ID_RETURN:
					{
						if (gbFindMode)
						{
							FindInTraceEdit();
						}
						else
						if (gbFilterMode)
						{
							SetFilterMode(false);
							SaveFilters();
						}
						break;
					}
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			if(wmId==1){
				switch(wmEvent){
					case EN_CHANGE:{
							if(!gbAfterUndo)
							{
								gbAfterUndo=true;
								::SendMessage(ghwndTraceEdit,WM_UNDO,0,0);
								::SendMessage(ghwndTraceEdit,EM_EMPTYUNDOBUFFER,0,0);
								gbAfterUndo=false;
							}
						}
						break;
				}
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			::GetClientRect(hWnd, &rt);
//			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			if(geViewMode==TRVM_TRACE){
				WINDOWPLACEMENT wp;
				GetWindowPlacement(ghwndTraceEdit,&wp);
				RECT rc;
				rc.left=0; rc.top=0;
				rc.right=wp.rcNormalPosition.left;
				rc.bottom=rt.bottom;
				::FillRect(hdc,&rc,ghbEditBG);
				rc.right=rt.right;
				rc.bottom=wp.rcNormalPosition.top;
				::FillRect(hdc,&rc,ghbEditBG);
				rc.top=wp.rcNormalPosition.bottom;
				rc.left=0;
				rc.bottom=rt.bottom;
				rc.right=rt.right;
				::FillRect(hdc,&rc,ghbEditBG);
								
			}else if(geViewMode==TRVM_TREE){
				WINDOWPLACEMENT wp;
				GetWindowPlacement(ghwndTree,&wp);
				RECT rc;
				rc.left=0; rc.top=0;
				rc.right=wp.rcNormalPosition.left;
				rc.bottom=rt.bottom;
				::FillRect(hdc,&rc,ghbEditBG);
				rc.right=rt.right;
				rc.bottom=wp.rcNormalPosition.top;
				::FillRect(hdc,&rc,ghbEditBG);
			}
			EndPaint(hWnd, &ps);
			break;
		case WM_INITMENUPOPUP:
			{
				// set menu item ID_EDIT enabled or not
				DWORD lw = LOWORD(lParam);
				// edit menu
				if (lw==1)
				{
					DWORD dwStartPos=0;
					DWORD dwEndPos=0;
					::SendMessage(ghwndTraceEdit, EM_GETSEL, (WPARAM)&dwStartPos, (LPARAM)&dwEndPos);
					if (dwStartPos == dwEndPos)
					{
						SetMenuItemEnabled(hWnd, ID_COPY, false);
					}
					else
					{
						SetMenuItemEnabled(hWnd, ID_COPY, true);
					}
				}
				break;
			}
		case WM_ERASEBKGND:
			return 0;
			break;
		case WM_SIZE:
			{
				RECT rt;
				GetClientRect(hWnd, &rt);
				HWND curWnd=NULL;
				if (gbFindMode)
				{
					curWnd = ghwndFind;
				}
				else
				{
					if (gbFilterMode)
					{
						curWnd = ghwndFilter;
					}
					else
					{
						::MoveWindow(ghwndTraceEdit,5,3,rt.right-5,rt.bottom-3,TRUE);
					}
				}
				if (curWnd)
				{
					RECT rtFind;
					::GetWindowRect(curWnd, &rtFind);
					::MoveWindow(ghwndTraceEdit,
						5,
						3,
						rt.right-5,
						rt.bottom-rtFind.bottom+rtFind.top-3,
						TRUE);
					::MoveWindow(curWnd,
						0,
						rt.bottom-rtFind.bottom+rtFind.top,
						rt.right,
						rtFind.bottom-rtFind.top,
						TRUE);
				}

				::MoveWindow(ghwndTree,5,3,rt.right-5,rt.bottom-3,TRUE);
			}
			break;
		case WM_DESTROY:
			gvFilters.clear();
			gvSenders.clear();
			DestroyTreeCtrl();
			DestroyMenu(ghmPopupMenu);
			DestroyWindow(ghwndTraceEdit);
			DestroyWindow(ghwndFind);
			DestroyWindow(ghwndFilter);
			//DestroyWindow(ghwndSelectSender);
			ghwndTraceEdit=NULL;
			DeleteObject(ghfEditFont);
			DeleteObject(ghbEditBG);
			::KillTimer(hWnd,1);
			if(g_hRichEditLib!=NULL)
				FreeLibrary(g_hRichEditLib);
			g_hRichEditLib = NULL;
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////
// Message handler for trace edit.
LRESULT CALLBACK WndTraceEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch(message)
	{
		case WM_RBUTTONDOWN:
			{
				if (wParam & MK_RBUTTON)
				{
					POINT point;
					GetCursorPos(&point);
					TrackPopupMenu(ghmPopupMenu, TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, 0, ghwndTraceEdit, NULL);
				}
			}
		case WM_LBUTTONUP:
			{
				if (gbFindMode)
					SetFindPosFromCursor();
				break;
			}
		case WM_COMMAND:
			{
				wmId    = LOWORD(wParam); 
				wmEvent = HIWORD(wParam); 
				// message from menu
				if (wmEvent==0)
				{
					switch (wmId)
					{
		 				case ID_COPY:
							::SendMessage(ghwndTraceEdit ,WM_COPY, 0, 0);
							break;
						case ID_SELECTALL:
							SelectAllTraceList();							
							break;
						case ID_EDIT_CLEARTRACER:
							ClearTraceList();
							break;
						default:
						   return DefWindowProc(hWnd, message, wParam, lParam);
					}
				}
				break;
			}
/*		case WM_SIZE:
			{
				LRESULT lFirstVisibleLine=::SendMessage(ghwndTraceEdit,EM_GETFIRSTVISIBLELINE,0,0);
				::SendMessage(ghwndTraceEdit,WM_VSCROLL,SB_TOP,NULL);
				::SendMessage(ghwndTraceEdit, EM_LINESCROLL,0,lFirstVisibleLine);
				break;
			}*/
		case WM_INITMENUPOPUP:
			{
				DWORD dwStartPos=0;
				DWORD dwEndPos=0;
				::SendMessage(ghwndTraceEdit, EM_GETSEL, (WPARAM)&dwStartPos, (LPARAM)&dwEndPos);

				// disable ID_COPY if selection is empty, enable otherwise
				MENUITEMINFO MIInfo; MIInfo.cbSize=sizeof(MENUITEMINFO);
				MIInfo.fMask = MIIM_STATE; 
				if (dwStartPos == dwEndPos)
				{
					MIInfo.fState = MFS_DISABLED;
				}
				else
				{
					MIInfo.fState = MFS_ENABLED;
				}
				SetMenuItemInfo(ghmPopupMenu, ID_COPY, FALSE, &MIInfo);

				HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
				if (hCursor!=NULL)
					SetCursor(hCursor);
				
			}
	}
	return CallWindowProc(hwndTraceEditProc, hWnd, message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////////////////
// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


HANDLE hBmp;
LRESULT CALLBACK WndFindDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (message)
	{
		case WM_INITDIALOG:
			{
				hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BTCLOSE));
				DWORD ret=::SendDlgItemMessage(hWnd, IDC_CLOSE, BM_SETIMAGE, IMAGE_BITMAP, (WPARAM)hBmp);
				::SendDlgItemMessage(hWnd, IDC_RADIO2, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
			}
			break;
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
	 			case IDC_CLOSE:
					SetFindMode(false);
					break;
 				case IDC_FINDNEXT:
					FindInTraceEdit();
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		}
		case WM_SHOWWINDOW:
		{
			if (wParam == TRUE)
			{
				// set find position
				SetFindPosFromCursor();
				// set focus to edit field
				HWND hwndEdit = GetDlgItem(ghwndFind, IDC_EDITFIND);
				SetFocus(hwndEdit);
			}
			break;
		}
		case WM_DESTROY:
			DeleteObject(hBmp);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return FALSE;
}

HANDLE hBmpFilterClose;
HANDLE hBmpFilterOk;
LRESULT CALLBACK WndFilterDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (message)
	{
		case WM_INITDIALOG:
			{
				hBmpFilterClose = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BTCLOSE));
				DWORD ret=::SendDlgItemMessage(hWnd, IDC_CLOSE, BM_SETIMAGE, IMAGE_BITMAP, (WPARAM)hBmpFilterClose);
				hBmpFilterOk = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BTOK));
				ret=::SendDlgItemMessage(hWnd, IDC_OK, BM_SETIMAGE, IMAGE_BITMAP, (WPARAM)hBmpFilterOk);
				::SendDlgItemMessage(hWnd, IDC_CHECK_FILTER1, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
				int res_filter = 1030;
				for (int i=0; i<3; i++)
				{
					::SendDlgItemMessage(hWnd, res_filter+i, WM_SETTEXT, 0, (LPARAM)"");
				}
			}
			break;
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
	 			case IDC_CLOSE:
					SetFilterMode(false);
					break;
				case IDC_OK:
					SetFilterMode(false);
					SaveFilters();
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		}
		case WM_SHOWWINDOW:
		{
			if (wParam == TRUE)
			{
				LoadFilters();

				// set selection to first checked
				int res_filter = 1030;
				int i=0;
				std::vector<SFilter>::const_iterator vciFilter = gvFilters.begin();
				std::vector<SFilter>::const_iterator vcieFilter = gvFilters.end();
				for (; vciFilter!=vcieFilter; vciFilter++)
				{
					if (vciFilter->bUsed==true)
						break;
					i++;
				}
				if (i>3) i=0;
				DWORD lLength=::SendDlgItemMessage(hWnd, res_filter+i, WM_GETTEXTLENGTH,0,0);
				::SendDlgItemMessage(hWnd, res_filter+i, EM_SETSEL, 0, lLength);

				// set focus to edit field
				HWND hwndFilter1 = GetDlgItem(ghwndFilter, res_filter+i);
				SetFocus(hwndFilter1);

			}
			break;
		}
		case WM_DESTROY:
			DeleteObject(hBmpFilterClose);
			DeleteObject(hBmpFilterOk);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return FALSE;
}

void OnSelectSenderOk(HWND hDlg)
{
	DWORD index = SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);
	if (index!=LB_ERR)
	{
		hwndSender = gvSenders[index].hWnd;
		GetAllData();
		ScrollTraceListToBottom();
	}
}

LRESULT CALLBACK WndSelectSenderDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			{
				gbPause = true;
				SetPauseMenuItemText(ghWnd,"Resume");
				RECT rcMain, rc;
				GetWindowRect(ghWnd, &rcMain);
				GetWindowRect(hDlg, &rc);
				SetWindowPos(hDlg, NULL, (rcMain.right - rcMain.left)/2 + rcMain.left - (rc.right - rc.left)/2,
					(rcMain.bottom - rcMain.top)/2 + rcMain.top - (rc.bottom - rc.top)/2, 0, 0, SWP_NOSIZE);

				std::vector<SSender>::const_iterator vci = gvSenders.begin();
				std::vector<SSender>::const_iterator vce = gvSenders.end();
				for (; vci!=vce; vci++)
				{
					SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)vci->sName.c_str());
				}
				if (SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCOUNT, 0, 0)>0)
				{
					SendDlgItemMessage(hDlg, IDC_LIST1, LB_SETCURSEL, 0, 0);
				}
				return TRUE;
			}

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				if (LOWORD(wParam) == IDOK)
				{
					OnSelectSenderOk(hDlg);
				}
				gbPause = false;
				SetPauseMenuItemText(ghWnd,"Pause");
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			else
			{
				if (HIWORD(wParam) == LBN_DBLCLK)
				{
					OnSelectSenderOk(hDlg);
					gbPause = false;
					SetPauseMenuItemText(ghWnd,"Pause");
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
				}
			}
			break;
	}
    return FALSE;
}