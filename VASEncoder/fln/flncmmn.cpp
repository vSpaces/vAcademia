#include "stdafx.h"

#include <stdio.h>
#include "flncmmn.h"

char* AppendString(char* *lppzs,const char* string2){
	int newStringSize = strlen(*lppzs)+strlen(string2)+1;
	char* str = MP_NEW_ARR( char, newStringSize);
	if( str == NULL) return NULL;
	strcpy_s(str, newStringSize, *lppzs);
	MP_DELETE_ARR( *lppzs);
	strcat_s(str, newStringSize, string2);
	return *lppzs=str;
} // AppendString

void OutErrMessage(ErrorCls* pec){
  if(pec==MEM_ERROR){
		fprintf(stderr,"Not enough memory.\n");
		fprintf(stderr,"¥ å¢ â ¥â ¯ ¬ïâ¨.\n");
		fprintf(stderr,"Íå õâàòàåò ïàìÿòè.\n");
	}else{
		switch(pec->ErrorCode()){
		 case 0:
			fprintf(stderr,"Error:\n%s\n",pec->ErrorRemark());
			break;
		 case 1:
			fprintf(stderr,"Undefined error in %s\n"
										 "  Please call to programmers.\n"
										 "¥¯®­ïâ­ ï ®è¨¡ª  ¢ ¬¥â®¤¥ %s\n"
										 "  Ž¡à â¨â¥áì ¯®¦ «ã©áâ  ª à §à ¡®âç¨ª ¬.\n"
										 "Íåïîíÿòíàÿ îøèáêà â ìåòîäå %s\n"
										 "  Îáðàòèòåñü ïîæàëóéñòà ê ðàçðàáîò÷èêàì.\n",
										 pec->ErrorRemark(),
										 pec->ErrorRemark());
			break;
		 default:
			fprintf(stderr,"%d,%s\n",pec->ErrorCode(),pec->ErrorRemark());
			break;
		}
	}
} // OutErrMessage

#ifdef NML_ENGINE
void OutErrMessageToMesBox(ErrorCls* pec){
	if(pec==NULL) return;
	CString sErr;
	if(pec==MEM_ERROR){
		sErr.Format("Not enough memory.");
	}else{
		switch(pec->ErrorCode()){
		 case 0:
			sErr.Format("%s",pec->ErrorRemark());
			break;
		 case 1:
			sErr.Format("Undefined error in %s",pec->ErrorRemark());
			break;
		 default:
			sErr.Format("%d,%s",pec->ErrorCode(),pec->ErrorRemark());
			break;
		}
	}
	//AfxMessageBox(sErr);
} // OutErrMessageToMesBox
#endif	// NML_ENGINE

ErrorCls* FormErrMessage(int errnum,const char* pstr,int row,int col){
  ErrorCls* pec;
  char* perrstr;
  perrstr = MP_NEW_ARR( char, strlen(pstr)+20);
  sprintf_s(perrstr,strlen(pstr)+20,pstr,row,col);
  MP_NEW_V2( pec, ErrorCls, errnum, perrstr);
  return pec;
} // FormErrMessage

char sErrMes[4000]="";

char sErrMesWith1Param[2000]="";

void SetCursorToBeginOfLine(){
	COORD cursorpos;
	CONSOLE_SCREEN_BUFFER_INFO ConsScrnInfo;

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	if (hStdout == INVALID_HANDLE_VALUE) return;
	GetConsoleScreenBufferInfo( hStdout, (PCONSOLE_SCREEN_BUFFER_INFO)&ConsScrnInfo);
	cursorpos = ConsScrnInfo.dwCursorPosition;
	cursorpos.X=0;
	SetConsoleCursorPosition( hStdout, cursorpos);
	fprintf(stdout,"                                                                              ");
	SetConsoleCursorPosition( hStdout, cursorpos);
}