// InputParameters.cpp: implementation of the CInputParameters class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "fla.h"
#include "InputParameters.h"
#include "FLNFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputParameters::~CInputParameters()
{

}

//void Reverse(CStringList &aSList){
//	if(aSList.GetCount() <= 1) return;
//	CStringList slRSList;
//	POSITION pos=aSList.GetHeadPosition();
//	while(pos){
//		CString sTmp = aSList.GetNext(pos);
//		slRSList.AddHead(sTmp);
//	}
//	aSList.RemoveAll();
//	pos = slRSList.GetHeadPosition();
//	while(pos){
//		CString sTmp = slRSList.GetNext(pos);
//		aSList.AddTail(sTmp);
//	}
//}

int compare( const void *arg1, const void *arg2 ){
   return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
}

ErrorCls* CInputParameters::Parse(LPCSTR alpcsParam){
//	CString sParam(alpcsParam);
//	if(sParam[0]=='-' || sParam[0]=='/'){
//		// parameter
//		// enum {cmdAssemble,cmdDisassemble} eCommand;
//		// BOOL bCompress;
//		// CTransparentColor oTC;
//		// int iBPP;
//		// enum {fmtPNG,fmtTGA} eOutFormat;
//		// int iSpeed;
//		// ??
//		if(sParam[1]=='d' || sParam[1]=='D'){
//			bDebugMode=TRUE;
//		}
//		if(sParam[1]=='r' || sParam[1]=='R'){
//			if(sParam.GetLength()>3 && 
//				(sParam[2]=='e' || sParam[2]=='E') &&
//				(sParam[3]=='v' || sParam[3]=='V')
//			){
//				bReverse = TRUE;
//			}else if(sParam.GetLength()>3 && 
//				(sParam[2]=='n' || sParam[2]=='N') &&
//				(sParam[3]=='s' || sParam[3]=='S')
//			){
//				bRemoveRareNoise = TRUE;
//			}else{
//				// source rectangle
//				CString sCoords=sParam.Right(sParam.GetLength()-2);
//				int iScaned=sscanf(sCoords,"%d,%d,%d,%d",
//									&(rcSrc.left),&(rcSrc.top),&(rcSrc.right),&(rcSrc.bottom));
//				if(iScaned<4){
//					return new ErrorCls(0,
//						"'r'-option must have got all region coordinates");
//				}
//				rcSrc.right+=rcSrc.left;
//				rcSrc.bottom+=rcSrc.top;
//			}
//		}
//		if(sParam[1]=='t' || sParam[1]=='T'){
//			// transparent color
//			if(sParam.GetLength()>2){
//				if(sParam[2]=='a' || sParam[2]=='A'){
//					bTCAuto=TRUE;
//					int iDop;
//					const char* pc=(LPCSTR)sParam; pc+=3;
//					int iScaned=sscanf(pc,"%d",&iDop);
//					if(iScaned==1){
//						oTC.d=iDop;
//					}
//				}else if(sParam[2]=='c' || sParam[2]=='C'){
//					bTrueColor=TRUE;
//					if(sParam.GetLength()>3){
//						char chCM=sParam[3];
//						if(chCM=='0') iTCCompRate=0;
//						if(chCM=='1') iTCCompRate=1;
//						if(chCM=='2') iTCCompRate=2;
//						if(chCM=='3') iTCCompRate=3;
//					}
//				}
//			}else{
//				CString sColor=sParam.Right(sParam.GetLength()-2);
//				int iScaned=sscanf(sColor,"%d,%d,%d,%d",
//									&(oTC.r),&(oTC.g),&(oTC.b),&(oTC.d));
//				if(iScaned<4){
//					if(iScaned<3){
//						return new ErrorCls(0,
//							"'t'-option must have got all color components");
//					}
//					oTC.d=0;
//				}
//				oTC.bDefined=TRUE;
//			}
//		}
//		if(sParam[1]=='f' || sParam[1]=='F'){
//			if(sParam[2]=='n' || sParam[2]=='N'){
//				eOutFormat=fmtPNG;
//			}
//			if(sParam[2]=='t' || sParam[2]=='T'){
//				eOutFormat=fmtTGA;
//			}
//			if(sParam[2]=='b' || sParam[2]=='B'){
//				eOutFormat=fmtBMP;
//			}
//			if(sParam[2]=='p' || sParam[2]=='P'){
//				eOutFormat=fmtPIN;
//			}
//			bOutFormatSpecified=TRUE;
//		}
//		if(sParam[1]=='s' || sParam[1]=='S'){
//			int iSpd=0;
//			const char* pc=(LPCSTR)sParam; pc+=2;
//			int iScaned=sscanf(pc,"%d",&iSpd);
//			if(iScaned==1){
//				iSpeed=iSpd;
//			}
//		}
//		if(sParam[1]=='m' || sParam[1]=='M'){
//			bSrcByEmptyMargs=TRUE;
//		}
//		if(sParam[1]=='p' || sParam[1]=='P'){ // premultiplied input
//			bPreMultiplied=TRUE;
//		}
//		if(sParam[1]=='c' || sParam[1]=='C' || sParam[1]=='с' || sParam[1]=='—'){ //
//			bDontCutAnyMargins=TRUE; bSrcByEmptyMargs=TRUE;
//		}
//		if(sParam[1]=='q' || sParam[1]=='Q'){
//			iQuality=0;
//			if(sParam.GetLength()>2){
//				char chCM=sParam[2];
//				if(chCM>='0' && chCM<='9')
//					iQuality=chCM-'0';
//			}
//		}
//		if(sParam[1]=='z' || sParam[1]=='Z'){
//			bUseZlib=TRUE;
//		}
//		if(sParam[1]=='b' || sParam[1]=='B'){
//			bMakeAssBckBAT=TRUE;
//		}
//		sParam.MakeLower();
//		sParam.Delete(0,1);
//		if(sParam=="asis"){
//			bSaveAsIs=TRUE;
//		}else if(sParam=="v"){
//			bVideoCompression=TRUE;
////		}else if(sParam=="old"){
////			bOldDiffAlg = TRUE;
//		}else if(sParam=="dark"){
//			bDarking = TRUE;
//		}else if(sParam=="af"){
//			bApplyAlphaFilter = TRUE;
//		}
//	}else{
//		// file mask
//		CStringList slFNames;
//		WIN32_FIND_DATA wfd;
//		HANDLE r = FindFirstFile(sParam,&wfd);
//		if (r == INVALID_HANDLE_VALUE){
//			sAnimFileName=sParam;
//			return NULL;
//		}
//		CString sDir;
//		int iSlPos=-1;
//		if((iSlPos=sParam.Find('\\'))>=0){
//			sDir=sParam.Left(iSlPos+1);
//		}
//		while(1){
//			slFNames.AddTail(sDir+wfd.cFileName);
//			if(!FindNextFile(r,&wfd)) break;
//		}
//		FindClose(r);
//		// sort file names
//		int iNumOfFiles=slFNames.GetCount();
//		LPSTR* sArray=new LPSTR[iNumOfFiles];
//		POSITION pos=slFNames.GetHeadPosition();
//		for(int iFNum=0; pos; iFNum++){
//			CString sFName=slFNames.GetNext(pos);
//			sArray[iFNum]=new char[sFName.GetLength()+5];
//			strcpy(sArray[iFNum],sFName);
//		}
//		slFNames.RemoveAll();
//
//		qsort(sArray,iNumOfFiles,sizeof(char*),compare);
//
//		for(int i=0; i<iNumOfFiles; i++){
//			slFileNames.AddTail(sArray[i]);
//			delete (sArray[i]);
//		}
//		delete sArray;
//		//
//		if(bReverse) Reverse(slFileNames);
//	}
	return NULL;
}

ErrorCls* CInputParameters::DefWhatToDo(){
	//if(slFileNames.GetCount()==0){
	//	eCommand=cmdGiveHelp;
	//	return NULL;
	//}
	//// »щем им€ анимации
	//POSITION pos=slFileNames.GetHeadPosition();
	//for(int iFNum=0; pos; iFNum++){
	//	CString sFName=slFileNames.GetNext(pos);
	//	BOOL bAnim=FALSE;
	//	int iIdx=sFName.ReverseFind('.');
	//	if(iIdx<0) continue;
	//	CString sExt;
	//	sExt=sFName.Right(sFName.GetLength()-iIdx-1);
	//	sExt.MakeLower();
	//	if(sExt=="fln" || sExt=="fla" || sExt=="flz" || 
	//		sExt=="flh" || sExt=="flc" || sExt=="pin")
	//		bAnim=TRUE;
	//	if(bAnim){
	//		if(iFNum==0){
	//			if(sExt=="pin" && slFileNames.GetCount()>1) break;
	//			sAnimFileName=sFName;
	//			slFileNames.RemoveHead();
	//			break;
	//		}else{
	//			if(sExt!="pin"){
	//				sprintf(sErrMes,"Animation file %s cannot be among picture files",sFName);
	//				return new ErrorCls(0,sErrMes);
	//			}
	//		}
	//	}
	//}
	//// ≈сли им€ анимации не задано, то...
	//if(sAnimFileName==""){
	//	// если указана только одна картинка, то...
	//	if(slFileNames.GetCount()==1){
	//		// им€ анимации = им€ картинки + .fln
	//		sAnimFileName=slFileNames.GetHead();
	//		int iDotPos=-1;
	//		if((iDotPos=sAnimFileName.Find('.'))>0)
	//			sAnimFileName=sAnimFileName.Left(iDotPos);
	//		sAnimFileName+=".fln";
	//	}else{
	//		// ищем общее начало названий картинок
	//		CString sAFNBeg="";
	//		POSITION pos=slFileNames.GetHeadPosition();
	//		CString sFName=slFileNames.GetNext(pos);
	//		if(pos){
	//			sAFNBeg=sFName;
	//			int iDotPos=-1;
	//			if((iDotPos=sAFNBeg.Find('.'))>0)
	//				sAFNBeg=sAFNBeg.Left(iDotPos);
	//		}
	//		while(pos){
	//			sFName=slFileNames.GetNext(pos);
	//			for(int ic=0; ic<sAFNBeg.GetLength(); ic++){
	//				if(sAFNBeg[ic]!=sFName[ic]){
	//					sAFNBeg=sAFNBeg.Left(ic);
	//					break;
	//				}
	//			}
	//		}
	//		// убираем цифры в конце общего начала
	//		for(int ic=sAFNBeg.GetLength()-1; ic>0; ic--){
	//			char ch=sAFNBeg[ic];
	//			if(ch>='0' && ch<='9'){
	//				sAFNBeg=sAFNBeg.Left(ic);
	//			}else break;
	//		}
	//		if(sAFNBeg=="") sAFNBeg="anim";
	//		// им€ анимации = общее начало + .fln
	//		sAnimFileName=sAFNBeg+".fln";
	//	}
	//}
	//// if animation is got only then disassemble it
	//if(slFileNames.GetCount()==0){
	//	eCommand=cmdDisassemble;
	//	return NULL;
	//}
	//eCommand=cmdAssemble;

	////
	//if(sAnimFileName.ReverseFind('.') < 0) 
	//	sAnimFileName+=".fln";
	return NULL;
}

ErrorCls* CInputParameters::UpdateByFrame(CFLNFrame &aoFrame){
	iBPP=aoFrame.bmp.GetBPP();
	if(iBPP==32 && !aoFrame.bmp.HasAlpha()) iBPP=24;
	return NULL;
}
