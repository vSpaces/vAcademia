
#include "mlRMML.h"
#include "ILogWriter.h"

#include "jsprf.h"
#if JS_VERSION >= 170
#else
#include "jsstddef.h"
#endif
#include <iostream>
#include <sstream>
#include <rmmlsafe.h>

//using namespace std;
namespace rmml{

int GetFormattedStringLength(const char* lpszFormat, va_list argList);
string gsErrorMessage;
bool	gbSynchTracesEnabled = false;

void mlSetSynchTracesEnabled( bool abEnabled, unsigned int auLevel)
{
	gbSynchTracesEnabled = abEnabled;
}

bool mlIsSynchTracesEnabled(){
	return gbSynchTracesEnabled;
}

void mlConcat(JSContext* cx, std::string& aDest, const char* pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	int len = GetFormattedStringLength(pszFormat, ptr);
	unsigned int bufferSize = aDest.size() + len + 200;
	char* sTemp = MP_NEW_ARR( char, bufferSize);
	//strcpy( sTemp, aDest.c_str());
	rmmlsafe_strcpy( sTemp, bufferSize, 0, aDest.c_str());
	//vsprintf( sTemp + aDest.size(), pszFormat, ptr);
	rmmlsafe_vsprintf( sTemp, bufferSize, aDest.size(), pszFormat, ptr);
	va_end(ptr);
	aDest = sTemp;
	MP_DELETE_ARR( sTemp);
}

void mlTraceImpl(JSContext* cx, const char* pszFormat, va_list ptr){
	int len = GetFormattedStringLength(pszFormat, ptr);
	int bufferLength = len + 200;
	gsErrorMessage.resize(len + 200);
	//strcpy(((char*)gsErrorMessage.c_str()), "SM: ");
	rmmlsafe_strcpy((char*)gsErrorMessage.c_str(), bufferLength, 0, "SM: ");
	//vsprintf ((char*)(gsErrorMessage.c_str() + 4), pszFormat, ptr);
	rmmlsafe_vsprintf((char*)gsErrorMessage.c_str(), bufferLength, 4, pszFormat, ptr);
	trc::trcITracer* pTracer = GPSM(cx)->GetContext()->mpTracer;
	if(pTracer)
		pTracer->trace("%s", gsErrorMessage.c_str());
	const char* sMessage = gsErrorMessage.c_str() + 4;
	int sMessageSize = gsErrorMessage.size() - 4;
	int sSmallMessageSize = (sMessageSize > 20) ? 20 : sMessageSize;

	int pos = -1;

	for (int k = 0; k < sSmallMessageSize; k++)
	if (sMessage[k] == '[')
	{
		pos = k;
		if (pos != 0)
		{
			break;
		}
	}

	if (pos != -1)
	if (sMessage[pos + 0] == '[' && sMessage[pos + 1] == 'R' && sMessage[pos + 2] == 'M' &&
		sMessage[pos + 3] == 'M' && sMessage[pos + 4] == 'L')
	{
		ILogWriter* pLogWriter = GPSM( cx)->GetContext()->mpLogWriter;
		CLogValue logValue( sMessage);
		pLogWriter->Write( logValue);
	}
	else if (sMessage[pos + 0] == '[' && sMessage[pos + 1] == 'E' && sMessage[pos + 2] == 'R' &&
		sMessage[pos + 3] == 'R' && sMessage[pos + 4] == 'O' && sMessage[pos + 5] == 'R')
	{
		ILogWriter* pLogWriter = GPSM( cx)->GetContext()->mpLogWriter;
		CLogValue logValue( sMessage);
		pLogWriter->Write( logValue);
	}
	else if (sMessage[pos + 0] == '[' && sMessage[pos + 1] == 'J' && sMessage[pos + 2] == 'S')
	{
		ILogWriter* pLogWriter = GPSM( cx)->GetContext()->mpLogWriter;
		CLogValue logValue( sMessage);
		pLogWriter->Write( logValue);
	}
}

void mlTrace(JSContext* cx, const char* pszFormat, ...){
	va_list ptr;
	va_start(ptr, pszFormat);
	mlTraceImpl(cx, pszFormat, ptr); //made for sychronization debug
	va_end(ptr);
}

void mlTraceNew(JSContext* cx, const char* pszFormat, ...){ //made for sychronization debug
	va_list ptr;
	va_start(ptr, pszFormat);
	mlTraceImpl(cx, pszFormat, ptr);
	va_end(ptr);
}

void mlTraceSyncMode3(JSContext* cx, const char* pszFormat, ...)
{
	if( gbSynchTracesEnabled)
	{
		va_list ptr;
		va_start(ptr, pszFormat);
		mlTraceImpl( cx, pszFormat, ptr);
		va_end(ptr);
	}
}

void mlTraceError(JSContext* cx, const char* pszFormat, ...){
	va_list ptr;
	va_start(ptr, pszFormat);
	char sErrorMessage[ 250000] = "SM:E ";
	//vsprintf( &sErrorMessage[strlen(sErrorMessage)], pszFormat, ptr);
	if( strstr(pszFormat, "%S") != NULL)
	{
		wchar_t* v1 = va_arg(ptr, wchar_t*);
		wchar_t* v2 = va_arg(ptr, wchar_t*);
		va_end(ptr);

		va_start(ptr, pszFormat);
	}
	rmmlsafe_vsprintf( sErrorMessage, sizeof(sErrorMessage), strlen(sErrorMessage), pszFormat, ptr);
	trc::trcITracer* pTracer = GPSM(cx)->GetContext()->mpTracer;
	if(pTracer)
		pTracer->trace(sErrorMessage); //for debug

	ILogWriter* pLogWriter = GPSM( cx)->GetContext()->mpLogWriter;
	CLogValue logValue( sErrorMessage);
	pLogWriter->WriteLnLPCSTR( logValue.GetData());

	va_end(ptr);
}

void mlTraceWarning(JSContext* cx, const char* pszFormat, ...){
	va_list ptr;
	va_start(ptr, pszFormat);
	char sErrorMessage[ 2500] = "SM:W ";
	//vsprintf( &sErrorMessage[strlen(sErrorMessage)], pszFormat, ptr);
	rmmlsafe_vsprintf( sErrorMessage, sizeof(sErrorMessage), strlen(sErrorMessage), pszFormat, ptr);
	trc::trcITracer* pTracer = GPSM(cx)->GetContext()->mpTracer;
	if(pTracer)
		pTracer->trace(sErrorMessage); //for debug
	va_end(ptr);
}

#ifndef TRACE
void TRACE(JSContext* cx, const char* pszFormat, ...){
	va_list ptr;
	va_start(ptr, pszFormat);
	char sErrorMessage[ 1500] = "";
	//vsprintf( sErrorMessage, pszFormat, ptr);
	rmmlsafe_vsprintf( sErrorMessage, sizeof(sErrorMessage), 0, pszFormat, ptr);

	trc::trcITracer* pTracer = GPSM(cx)->GetContext()->mpTracer;
	if(pTracer)
		pTracer->trace(sErrorMessage); //for debug

	va_end(ptr);
}
#endif

#ifdef _DEBUG

void mlDebug::Abort(JSContext* cx, const char* aFile, int aLine)
{     
TRACE(cx, "Abort (%s,%d)\n",aFile,aLine);
}     

void mlDebug::Break(JSContext* cx, const char* aFile, int aLine)
{ 
TRACE(cx, "Break (%s,%d)\n",aFile,aLine);
}

void mlDebug::Warning(JSContext* cx, const char* aStr,
                             const char* aFile, 
                             int aLine)
{ 
TRACE(cx, "Warning:\'%s\' (%s,%d)\n",aStr,aFile,aLine);
}

void mlDebug::Assertion(JSContext* cx, const char* aStr, const char* aExpr,
                               const char* aFile, int aLine)
{ 
TRACE(cx, "Assertion:\'%s\' %s (%s,%d)\n",aStr,aExpr,aFile,aLine);
}

#endif

#ifndef PTRDIFF
#define PTRDIFF(p1, p2, type)                                 \
	((p1) - (p2))
#endif

//static 
JSString* mlFormatJSError(JSContext *cx, const char *message, JSErrorReport *report, JSBool reportWarnings) {

//	JSString* sRet=JS_NewString(cx,"",1);
//	const int ciMaxErrSize=1000;
//	char* sErrorBuf=new char[ciMaxErrSize];

	if(isEqual(message, "out of memory"))
		return (JSString*)1;

	std::ostringstream sout;

    int i, j, k, n;
    char *prefix, *tmp;
    const char *ctmp;

    if (!report) {
		char* st=JS_smprintf("%s\n", message);
		JSString* sRet=JS_NewStringCopyZ(cx,st);
		//JS_free(cx, st);
		JS_smprintf_free(st);
        return sRet;
    }

    /* Conditionally ignore reported warnings. */
    if (JSREPORT_IS_WARNING(report->flags) && !reportWarnings)
        return JS_NewStringCopyZ(cx,"");

    prefix = NULL;
    if (report->filename)
        prefix = JS_smprintf("%s:", report->filename);
    if (report->lineno) {
        tmp = prefix;
        prefix = JS_smprintf("%s%u: ", tmp ? tmp : "", report->lineno);
        //JS_free(cx, tmp);
		JS_smprintf_free(tmp);
    }
    if (JSREPORT_IS_WARNING(report->flags)) {
        tmp = prefix;
        prefix = JS_smprintf("%s%swarning: ",
                             tmp ? tmp : "",
                             JSREPORT_IS_STRICT(report->flags) ? "strict " : "");
        //JS_free(cx, tmp);
		JS_smprintf_free(tmp);
    }

    /* embedded newlines -- argh! */
    while ((ctmp = strchr(message, '\n')) != 0) {
        ctmp++;
		if (prefix){
//            fputs(prefix, gErrFile);
			sout << prefix;
		}
//        fwrite(message, 1, ctmp - message, gErrFile);
		for(int ii=0; ii<(ctmp - message); ii++){
			char ch=message[ii];
			sout << ch;
		}
        message = ctmp;
    }

    /* If there were no filename or lineno, the prefix might be empty */
	if (prefix){
//        fputs(prefix, gErrFile);
		sout << prefix;
	}
//    fputs(message, gErrFile);
	sout << message;

    if (!report->linebuf) {
//        fputc('\n', gErrFile);
		sout << '\n';
        goto out;
    }

    /* report->linebuf usually ends with a newline. */
    n = (int)strlen(report->linebuf);
	sout << ":\n" << prefix << report->linebuf;
	if(!(n > 0 && report->linebuf[n-1] == '\n')) sout << '\n';
	sout << prefix;
//    fprintf(gErrFile, ":\n%s%s%s%s",
//            prefix,
//            report->linebuf,
//            (n > 0 && report->linebuf[n-1] == '\n') ? "" : "\n",
//            prefix);
#if JS_VERSION >= 170
	n = (int)PTRDIFF(report->tokenptr, report->linebuf, char);
#else
    n = (int)PTRDIFF(report->tokenptr, report->linebuf, char);
#endif
    for (i = j = 0; i < n; i++) {
        if (report->linebuf[i] == '\t') {
            for (k = (j + 8) & ~7; j < k; j++) {
//                fputc('.', gErrFile);
				sout << '.';
            }
            continue;
        }
//        fputc('.', gErrFile);
		sout << '.';
        j++;
    }
//    fputs("^\n", gErrFile);
	sout << "^\n";
 out:
//    if (!JSREPORT_IS_WARNING(report->flags))
//        gExitCode = EXITCODE_RUNTIME_ERROR;
    //JS_free(cx, prefix);
	JS_smprintf_free(prefix);
//	string s=sout.str();
	return JS_NewStringCopyZ(cx, sout.str().c_str());
}

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

int GetFormattedStringLength(const char* lpszFormat, va_list argList){
	va_list argListSave = argList;

	int nMaxLen = 0;
	for (const char* lpsz = lpszFormat; *lpsz != '\0'; lpsz++){
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(++lpsz) == '%'){
			nMaxLen++;
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz++){
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' || *lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0){
			// width indicated by
			nWidth = rtl_atoi(lpsz);
			for (; *lpsz != '\0' && isdigit(*lpsz); lpsz++);
		}

		int nPrecision = 0;
		if (*lpsz == '.'){
			// skip past '.' separator (width.precision)
			lpsz++;

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz++;
			}
			else
			{
				nPrecision = rtl_atoi(lpsz);
				for (; *lpsz != '\0' && isdigit(*lpsz); lpsz++);
			}
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (isEqual(lpsz, "I64", 3)){
			lpsz += 3;
			nModifier = FORCE_INT64;
		}else{
			switch (*lpsz){
			// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz++;
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz++;
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz++;
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier){
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, char);
			break;
		case 'c'|FORCE_ANSI:
		case 'C'|FORCE_ANSI:
			nItemLen = 2;
			va_arg(argList, char);
			break;
		case 'c'|FORCE_UNICODE:
		case 'C'|FORCE_UNICODE:
			nItemLen = 2;
			va_arg(argList, wchar_t);
			break;

		// strings
		case 's':{
			const char* pstrNextArg = va_arg(argList, const char*);
			if (pstrNextArg == NULL){
			   nItemLen = 6;  // "(null)"
			}else{
			   nItemLen = strlen(pstrNextArg);
			   nItemLen = max(1, nItemLen);
			}
			}break;
		case 'S':{
			wchar_t* pstrNextArg = va_arg(argList, wchar_t*);
			if (pstrNextArg == NULL){
			   nItemLen = 6;  // "(null)"
			}else{
			   nItemLen = wcslen(pstrNextArg);
			   nItemLen = max(1, nItemLen);
			}
			}break;
		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
			{
			const char* pstrNextArg = va_arg(argList, const char*);
			if (pstrNextArg == NULL){
			   nItemLen = 6; // "(null)"
			}else{
			   nItemLen = strlen(pstrNextArg);
			   nItemLen = max(1, nItemLen);
			}
			}break;
		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
			{
			wchar_t* pstrNextArg = va_arg(argList, wchar_t*);
			if (pstrNextArg == NULL){
			   nItemLen = 6; // "(null)"
			}else{
			   nItemLen = wcslen(pstrNextArg);
			   nItemLen = max(1, nItemLen);
			}
			}break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0){
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}else{
			switch (*lpsz){
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				if (nModifier & FORCE_INT64)
					va_arg(argList, __int64);
				else
					va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;
			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;
			case 'f':
				va_arg(argList, double);
				nItemLen = 128; // width isn't truncated
				// 312 == strlen("-1+(309 zeroes).")
				// 309 zeroes == max precision of a double
				nItemLen = max(nItemLen, 312+nPrecision);
				break;
			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;
			// no output
			case 'n':
				va_arg(argList, int*);
				break;
			default:;
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	va_end(argListSave);
	return nMaxLen + 10;
}

}
