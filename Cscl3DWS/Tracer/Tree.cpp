
#include "stdafx.h"
#include "commctrl.h"
#include "Tree.h"
#include "mlSynchData.h"
#include "MyString.h"
#include <stdio.h>

HWND ghwndTree=NULL;

void CreateTreeCtrl(HINSTANCE ahInst,HWND hWndParent){
	RECT rt;
	GetClientRect(hWndParent, &rt);
	ghwndTree=CreateWindow(WC_TREEVIEW,NULL,
		TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL // | WS_BORDER
		,
		0,//5, //    int x,
		0,//3, //    int y,
		rt.right,//rt.right-10, // rt.right, //    int nWidth,
		rt.bottom,//rt.bottom-6, // rt.bottom, //    int nHeight,
		hWndParent, //    HWND hWndParent,
		(HMENU)1, //    HMENU hMenu,
		ahInst, //    HINSTANCE hInstance,
		NULL //    LPVOID lpParam
	);
//	if(ghwndTree==NULL) return;
//	TVINSERTSTRUCT is;
//	is.hParent=NULL;
//	is.hInsertAfter=TVI_ROOT;
//	is.item.mask=TVIF_TEXT;
//	is.item.pszText="test";
//	is.item.cchTextMax=strlen(is.item.pszText);
//	TreeView_InsertItem(ghwndTree,&is);
}

void DestroyTreeCtrl(){
	if(ghwndTree!=NULL)
		DestroyWindow(ghwndTree);
	ghwndTree=NULL;
}

HTREEITEM InsertItem(HTREEITEM ahParent, const char* apszItemText){
	TVINSERTSTRUCT is;
	if(ahParent==NULL){
		is.hParent=NULL;
		is.hInsertAfter=TVI_ROOT;
	}else{
		is.hParent=ahParent;
		is.hInsertAfter=TVI_LAST;
	}
	is.item.mask=TVIF_TEXT;
	is.item.pszText=(char*)apszItemText;
	is.item.cchTextMax=strlen(is.item.pszText);
	return TreeView_InsertItem(ghwndTree,&is);
}

inline bool isEqual(const char* apch1, const char* apch2){
	for(;;){
		if(*apch1!=*apch2) return false;
		if(*apch1=='\0') return true;
		apch1++, apch2++;
	}
}

inline bool isEqual(const wchar_t* apwc1, const wchar_t* apwc2){
	for(;;){
		if(*apwc1!=*apwc2) return false;
		if(*apwc1==L'\0') return true;
		apwc1++, apwc2++;
	}
}

// надо сделать мапку int2int для перевод id объекта в его HTREEITEM
// ??

#define PJSVT_NO		(char)0
#define PJSVT_NULL		(char)1
#define PJSVT_OBJECT	(char)2
#define PJSVT_OBJREF	(char)3
#define PJSVT_FUNCTION	(char)4
#define PJSVT_STRING	(char)5
#define PJSVT_INT8		(char)6
#define PJSVT_INT16		(char)7
#define PJSVT_INT31		(char)8
#define PJSVT_DOUBLE	(char)9
#define PJSVT_BOOLEAN	(char)10
#define PJSVT_GO		(char)11

void RebuildTreeInternal(mlSynchData &data, HTREEITEM ahParent=NULL){
	int id;
	data >> id;
	unsigned char iPropCnt;
	data >> iPropCnt;
	for(int iProp=0; iProp<iPropCnt; iProp++){
		wchar_t* jscProp;
		data >> jscProp;
//		wstring wsPropName=jscProp;
		CMyWString wsPropName(jscProp);
		CMyString sItemText(wsPropName);
		if(*jscProp==L'\1'){
			// элемент массива
			sItemText="";
			char pszNum[20];
			sprintf(pszNum,"%d",iProp);
			sItemText+=pszNum;
		}
		char chPropType;
		data >> chPropType;
		switch(chPropType){
		case PJSVT_NO:
			continue;
			break;
		case PJSVT_GO:
			// ??
			break;
		case PJSVT_INT8:{
			unsigned char iVal;
			data >> iVal;
			sItemText+=": ";
			sItemText+=(int)iVal;
			InsertItem(ahParent,sItemText);
			}break;
		case PJSVT_INT16:{
			unsigned short iVal;
			data >> iVal;
			sItemText+=": ";
			sItemText+=(int)iVal;
			InsertItem(ahParent,sItemText);
			}break;
		case PJSVT_INT31:{
			int iVal;
			data >> iVal;
			sItemText+=": ";
			sItemText+=(int)iVal;
			InsertItem(ahParent,sItemText);
			}break;
		case PJSVT_BOOLEAN:{
			bool bVal;
			data >> bVal;
			sItemText+=": ";
			if(bVal) sItemText+="true"; else sItemText+="false";
			InsertItem(ahParent,sItemText);
			}break;
		case PJSVT_STRING:{
			wchar_t* jscVal;
			data >> jscVal;
			sItemText+=": \"";
			sItemText+=jscVal;
			sItemText+="\"";
			InsertItem(ahParent,sItemText);
			}break;
		case PJSVT_NULL:
			sItemText+=": null";
			InsertItem(ahParent,sItemText);
			break;
		case PJSVT_DOUBLE:{
			double jsdProp;
			data >> jsdProp;
			sItemText+=": ";
			sItemText+=jsdProp;
			InsertItem(ahParent,sItemText);
			}break;
		case PJSVT_FUNCTION:{
			wchar_t* jscFun;
			data >> jscFun;
//			sItemText+=" (function)";
			sItemText=jscFun;
			sItemText=sItemText.Left(65);
			sItemText.Replace("\n","");
			sItemText.Replace("  "," ");
			sItemText.Replace("  "," ");
			sItemText+="...";
			InsertItem(ahParent,sItemText);
			}break;
		case PJSVT_OBJECT:{
			char* pszClassName=NULL;
			data >> pszClassName;
//			if(jsoProp==NULL){
				if(isEqual(pszClassName,"global")){
//					jsoProp = JS_NewObject(mcx,NULL,NULL,NULL);
				}else{
//					jsoProp=gpSM->CreateElementByClassName(pszClassName);
				}
//			}
//			data >> jsoProp;
			sItemText+=" (";
			sItemText+=pszClassName;
			sItemText+=")";
			HTREEITEM ahTreeItem=InsertItem(ahParent,sItemText);
			RebuildTreeInternal(data,ahTreeItem);
			}break;
		case PJSVT_OBJREF:{
			int iID=0; 
			data >> iID;
			sItemText+=": ??";
			sItemText+="(reference)";
			InsertItem(ahParent,sItemText);
//			JSObject* jsoProp=getJsoById(iID);
//			if(jsoProp!=NULL){
//				vProp=OBJECT_TO_JSVAL(jsoProp);
//				bSet=true;
//			}else{
//				addRef(jso,wsPropName.c_str(),iID);
//			}
			}break;
		default:
			break;
		}
		// ??
	}
	// ??
}

void RebuildTree(void* apData, unsigned int auDataLength){
	mlSynchData data(apData,(int)auDataLength);
	if(TreeView_GetCount(ghwndTree) > 4) return; //??
	TreeView_DeleteAllItems(ghwndTree);
	RebuildTreeInternal(data);
}
