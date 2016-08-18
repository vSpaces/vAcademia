#if !defined(_SAGECMMN_H_)
#define _SAGECMMN_H_

#include <string.h>
#include <math.h>
//#include <windows.h>

#define ERR_PTR (void far*)0x8FFFFFFD
//enum BOOL{FALSE, TRUE};
#define TRUE 1
#define FALSE 0

#define COPYSTR(str) (strcpy(MP_NEW_ARR( char, strlen(str)+1),str))

class ErrorCls{
	int ErrorNumber;
	char* ErrorComment;
public:
	ErrorCls(int ErrNum, char* comment){
		ErrorNumber=ErrNum;
		ErrorComment=comment;
	}
	int ErrorCode(){ return ErrorNumber; }
	char* ErrorRemark(){ return ErrorComment; }
}; // ErrorCls

extern char* ErrorMessages[];

void OutErrMessage(ErrorCls* pec);
#ifdef NML_ENGINE
void OutErrMessageToMesBox(ErrorCls* pec);
#endif	// NML_ENGINE

ErrorCls* FormErrMessage(int errnum,const char* pstr,int row,int col);

extern char sErrMes[];
extern char sErrMesWith1Param[];

#ifdef NML_COMPILER
extern BOOL bDebug;
#define GEN_ERROR0(PSNT,ERRORMES)	sprintf(sErrMes, "%s\n %s.", \
									common.ErrMesBegin(PSNT),ERRORMES); \
									return MP_NEW_P2( ErrorCls, 0, sErrMes);

#define GEN_ERROR1(PSNT,ERRORMES,PARAM1) \
	sprintf(sErrMesWith1Param,ERRORMES,PARAM1); \
	sprintf(sErrMes, "%s\n %s.", \
	common.ErrMesBegin(PSNT),sErrMesWith1Param); \
	return MP_NEW_P2( ErrorCls, 0, sErrMes);

#define GEN_ERROR2(PSNT,ERRORMES,PARAM1,PARAM2) \
	sprintf(sErrMesWith1Param,ERRORMES,PARAM1,PARAM2); \
	sprintf(sErrMes, "%s\n %s.", \
	common.ErrMesBegin(PSNT),sErrMesWith1Param); \
	return MP_NEW_P2( ErrorCls, 0, sErrMes);
#endif	// NML_COMPILER
#ifdef NML_ENGINE
#define GEN_ERROR0(ERRORMES)		sprintf(sErrMes, "%s\n %s.", \
									common.ErrMesBegin(),ERRORMES); \
									return MP_NEW_P2( ErrorCls, 0, sErrMes);

#define GEN_ERROR1(ERRORMES,PARAM1) \
	sprintf(sErrMesWith1Param,ERRORMES,PARAM1); \
	sprintf(sErrMes, "%s\n %s.", \
	common.ErrMesBegin(),sErrMesWith1Param); \
	return MP_NEW_P2( ErrorCls, 0, sErrMes);
#endif	// NML_ENGINE


char* AppendString(char* *lppzs,const char* string2);

void SetCursorToBeginOfLine();

#define NUMTYPE long double

#define D2R(a) ((((NUMTYPE)a)*M_PI)/180)

#define MEM_ERROR ((ErrorCls*)-4)

#define DELETEP(PTR) if(PTR!=NULL){ MP_DELETE( PTR); }

#define ASSERT_PTR(PTR) \
	  ASSERT(PTR!=NULL); \
	  ASSERT(PTR!=(void*)0xdddddddd); \
	  ASSERT(PTR!=(void*)0xcdcdcdcd); \
	  ASSERT(PTR!=(void*)0xcccccccc); \
	  ASSERT(PTR!=(void*)0xdcdcdcdc);

#endif