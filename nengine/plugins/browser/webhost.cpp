#include "stdafx.h"

#include "browser.h"
#ifndef CComPtr
#include "comptr.h"
#endif
#include <string>

typedef enum {
	CURLE_OK = 0,
	CURLE_UNSUPPORTED_PROTOCOL,    // 1
	CURLE_FAILED_INIT,             // 2
	CURLE_URL_MALFORMAT,           // 3
	CURLE_OBSOLETE4,               // 4 - NOT USED
	CURLE_COULDNT_RESOLVE_PROXY,   // 5 
	CURLE_COULDNT_RESOLVE_HOST,    // 6 
	CURLE_COULDNT_CONNECT,         // 7 
	CURLE_FTP_WEIRD_SERVER_REPLY,  // 8 
	CURLE_REMOTE_ACCESS_DENIED,    // 9 a service was denied by the server
								   // due to lack of access - when login fails
								   // this is not returned. 
	CURLE_OBSOLETE10,              // 10 - NOT USED 
	CURLE_FTP_WEIRD_PASS_REPLY,    // 11 
	CURLE_OBSOLETE12,              // 12 - NOT USED 
	CURLE_FTP_WEIRD_PASV_REPLY,    // 13 
	CURLE_FTP_WEIRD_227_FORMAT,    // 14 
	CURLE_FTP_CANT_GET_HOST,       // 15 
	CURLE_OBSOLETE16,              // 16 - NOT USED 
	CURLE_FTP_COULDNT_SET_TYPE,    // 17 
	CURLE_PARTIAL_FILE,            // 18 
	CURLE_FTP_COULDNT_RETR_FILE,   // 19 
	CURLE_OBSOLETE20,              // 20 - NOT USED 
	CURLE_QUOTE_ERROR,             // 21 - quote command failure 
	CURLE_HTTP_RETURNED_ERROR,     // 22 
	CURLE_WRITE_ERROR,             // 23 
	CURLE_OBSOLETE24,              // 24 - NOT USED 
	CURLE_UPLOAD_FAILED,           // 25 - failed upload "command" 
	CURLE_READ_ERROR,              // 26 - could open/read from file 
	CURLE_OUT_OF_MEMORY,           // 27 
	// Note: CURLE_OUT_OF_MEMORY may sometimes indicate a conversion error
	// instead of a memory allocation error if CURL_DOES_CONVERSIONS is defined
	CURLE_OPERATION_TIMEDOUT,      // 28 - the timeout time was reached 
	CURLE_OBSOLETE29,              // 29 - NOT USED 
	CURLE_FTP_PORT_FAILED,         // 30 - FTP PORT operation failed 
	CURLE_FTP_COULDNT_USE_REST,    // 31 - the REST command failed 
	CURLE_OBSOLETE32,              // 32 - NOT USED 
	CURLE_RANGE_ERROR,             // 33 - RANGE "command" didn't work 
	CURLE_HTTP_POST_ERROR,         // 34 
	CURLE_SSL_CONNECT_ERROR,       // 35 - wrong when connecting with SSL 
	CURLE_BAD_DOWNLOAD_RESUME,     // 36 - couldn't resume download 
	CURLE_FILE_COULDNT_READ_FILE,  // 37 
	CURLE_LDAP_CANNOT_BIND,        // 38 
	CURLE_LDAP_SEARCH_FAILED,      // 39 
	CURLE_OBSOLETE40,              // 40 - NOT USED 
	CURLE_FUNCTION_NOT_FOUND,      // 41 
	CURLE_ABORTED_BY_CALLBACK,     // 42 
	CURLE_BAD_FUNCTION_ARGUMENT,   // 43 
	CURLE_OBSOLETE44,              // 44 - NOT USED 
	CURLE_INTERFACE_FAILED,        // 45 - CURLOPT_INTERFACE failed 
	CURLE_OBSOLETE46,              // 46 - NOT USED 
	CURLE_TOO_MANY_REDIRECTS ,     // 47 - catch endless re-direct loops 
	CURLE_UNKNOWN_TELNET_OPTION,   // 48 - User specified an unknown option 
	CURLE_TELNET_OPTION_SYNTAX ,   // 49 - Malformed telnet option 
	CURLE_OBSOLETE50,              // 50 - NOT USED 
	CURLE_PEER_FAILED_VERIFICATION, // 51 - peer's certificate or fingerprint
									// wasn't verified fine 
	CURLE_GOT_NOTHING,             // 52 - when this is a specific error 
	CURLE_SSL_ENGINE_NOTFOUND,     // 53 - SSL crypto engine not found 
	CURLE_SSL_ENGINE_SETFAILED,    // 54 - can not set SSL crypto engine as
									// default 
	CURLE_SEND_ERROR,              // 55 - failed sending network data 
	CURLE_RECV_ERROR,              // 56 - failure in receiving network data 
	CURLE_OBSOLETE57,              // 57 - NOT IN USE 
	CURLE_SSL_CERTPROBLEM,         // 58 - problem with the local certificate 
	CURLE_SSL_CIPHER,              // 59 - couldn't use specified cipher 
	CURLE_SSL_CACERT,              // 60 - problem with the CA cert (path?) 
	CURLE_BAD_CONTENT_ENCODING,    // 61 - Unrecognized transfer encoding 
	CURLE_LDAP_INVALID_URL,        // 62 - Invalid LDAP URL 
	CURLE_FILESIZE_EXCEEDED,       // 63 - Maximum file size exceeded 
	CURLE_USE_SSL_FAILED,          // 64 - Requested FTP SSL level failed 
	CURLE_SEND_FAIL_REWIND,        // 65 - Sending the data requires a rewind
									// that failed 
	CURLE_SSL_ENGINE_INITFAILED,   // 66 - failed to initialise ENGINE 
	CURLE_LOGIN_DENIED,            // 67 - user, password or similar was not
									// accepted and we failed to login 
	CURLE_TFTP_NOTFOUND,           // 68 - file not found on server 
	CURLE_TFTP_PERM,               // 69 - permission problem on server 
	CURLE_REMOTE_DISK_FULL,        // 70 - out of disk space on server 
	CURLE_TFTP_ILLEGAL,            // 71 - Illegal TFTP operation 
	CURLE_TFTP_UNKNOWNID,          // 72 - Unknown transfer ID 
	CURLE_REMOTE_FILE_EXISTS,      // 73 - File already exists 
	CURLE_TFTP_NOSUCHUSER,         // 74 - No such user 
	CURLE_CONV_FAILED,             // 75 - conversion failed 
	CURLE_CONV_REQD,               // 76 - caller must register conversion
									// callbacks using curl_easy_setopt options
									// CURLOPT_CONV_FROM_NETWORK_FUNCTION,
									// CURLOPT_CONV_TO_NETWORK_FUNCTION, and
									// CURLOPT_CONV_FROM_UTF8_FUNCTION 
	CURLE_SSL_CACERT_BADFILE,      // 77 - could not load CACERT file, missing
									// or wrong format 
	CURLE_REMOTE_FILE_NOT_FOUND,   // 78 - remote file not found 
	CURLE_SSH,                     // 79 - error from the SSH layer, somewhat
									// generic so the error message will be of
									// interest when this has happened 
	CURLE_SSL_SHUTDOWN_FAILED,     // 80 - Failed to shut down the SSL
									// connection 
	CURL_LAST // never use! 
};


const wchar_t* GetCURLErrorDesc(int aiErrorCode){
	const wchar_t* pwcError = NULL;
	switch(aiErrorCode){
	case CURLE_OK: // (0)
		pwcError = L"All fine. Proceed as usual.";
		break;
	case CURLE_UNSUPPORTED_PROTOCOL: // (1)
    	pwcError = L"The URL you passed to libcurl used a protocol that this libcurl does not support. The support might be a compile-time option that you didn't use, it can be a misspelled protocol string or just a protocol libcurl has no code for.";
		break;
	case CURLE_FAILED_INIT: // (2)
    	pwcError = L"Very early initialization code failed. This is likely to be an internal error or problem.";
		break;
	case CURLE_URL_MALFORMAT: // (3)
    	pwcError = L"The URL was not properly formatted.";
		break;
//	case CURLE_URL_MALFORMAT_USER: // (4)
//    	pwcError = L"This is never returned by current libcurl.";
		break;
	case CURLE_COULDNT_RESOLVE_PROXY: // (5)
    	pwcError = L"Couldn't resolve proxy. The given proxy host could not be resolved.";
		break;
	case CURLE_COULDNT_RESOLVE_HOST: // (6)
    	pwcError = L"Couldn't resolve host. The given remote host was not resolved.";
		break;
	case CURLE_COULDNT_CONNECT: // (7)
    	pwcError = L"Failed to connect() to host or proxy.";
		break;
	case CURLE_FTP_WEIRD_SERVER_REPLY: // (8)
    	pwcError = L"After connecting to an FTP server, libcurl expects to get a certain reply back. This error code implies that it got a strange or bad reply. The given remote server is probably not an OK FTP server.";
		break;
	case CURLE_REMOTE_ACCESS_DENIED: // (9)
    	pwcError = L"We were denied access when trying to login to an FTP server or when trying to change working directory to the one given in the URL.";
		break;
//	case CURLE_FTP_USER_PASSWORD_INCORRECT: // (10)
//    	pwcError = L"This is never returned by current libcurl.";
		break;
	case CURLE_FTP_WEIRD_PASS_REPLY: // (11)
    	pwcError = L"After having sent the FTP password to the server, libcurl expects a proper reply. This error code indicates that an unexpected code was returned.";
		break;
//	case CURLE_FTP_WEIRD_USER_REPLY: // (12)
//    	pwcError = L"After having sent user name to the FTP server, libcurl expects a proper reply. This error code indicates that an unexpected code was returned.";
		break;
	case CURLE_FTP_WEIRD_PASV_REPLY: // (13)
    	pwcError = L"libcurl failed to get a sensible result back from the server as a response to either a PASV or a EPSV command. The server is flawed.";
		break;
	case CURLE_FTP_WEIRD_227_FORMAT: // (14)
    	pwcError = L"FTP servers return a 227-line as a response to a PASV command. If libcurl fails to parse that line, this return code is passed back.";
		break;
	case CURLE_FTP_CANT_GET_HOST: // (15)
    	pwcError = L"An internal failure to lookup the host used for the new connection.";
		break;
//	case CURLE_FTP_CANT_RECONNECT: // (16)
//    	pwcError = L"A bad return code on either PASV or EPSV was sent by the FTP server, preventing libcurl from being able to continue.";
		break;
	case CURLE_FTP_COULDNT_SET_TYPE: // (17)
    	pwcError = L"Received an error when trying to set the transfer mode to binary.";
		break;
	case CURLE_PARTIAL_FILE: // (18)
    	pwcError = L"A file transfer was shorter or larger than expected. This happens when the server first reports an expected transfer size, and then delivers data that doesn't match the previously given size.";
		break;
	case CURLE_FTP_COULDNT_RETR_FILE: // (19)
    	pwcError = L"This was either a weird reply to a 'RETR' command or a zero byte transfer complete.";
		break;
//	case CURLE_FTP_WRITE_ERROR: // (20)
//    	pwcError = L"After a completed file transfer, the FTP server did not respond a proper";
		break;
	case CURLE_QUOTE_ERROR: // (21)
    	pwcError = L"When sending custom 'QUOTE' commands to the remote server, one of the commands returned an error code that was 400 or higher.";
		break;
	case CURLE_HTTP_RETURNED_ERROR: // (22)
    	pwcError = L"This is returned if CURLOPT_FAILONERROR is set TRUE and the HTTP server returns an error code that is >= 400.";
		break;
	case CURLE_WRITE_ERROR: // (23)
    	pwcError = L"An error occurred when writing received data to a local file, or an error was returned to libcurl from a write callback.";
		break;
//	case CURLE_MALFORMAT_USER: // (24)
//    	pwcError = L"This is never returned by current libcurl.";
		break;
	case CURLE_UPLOAD_FAILED: // (25)
    	pwcError = L"Failed starting the upload. For FTP, the server typcially denied the STOR command. The error buffer usually contains the server's explanation to this. (This error code was formerly known as CURLE_FTP_COULDNT_STOR_FILE.)";
		break;
	case CURLE_READ_ERROR: // (26)
    	pwcError = L"There was a problem reading a local file or an error returned by the read callback.";
		break;
	case CURLE_OUT_OF_MEMORY: // (27)
    	pwcError = L"Out of memory. A memory allocation request failed. This is serious badness and things are severely screwed up if this ever occur.";
		break;
	case CURLE_OPERATION_TIMEDOUT: // (28)
    	pwcError = L"Operation timeout. The specified time-out period was reached according to the conditions.";
		break;
//	case CURLE_FTP_COULDNT_SET_ASCII: // (29)
//    	pwcError = L"libcurl failed to set ASCII transfer type (TYPE A).";
		break;
	case CURLE_FTP_PORT_FAILED: // (30)
    	pwcError = L"The FTP PORT command returned error. This mostly happen when you haven't specified a good enough address for libcurl to use. See CURLOPT_FTPPORT.";
		break;
	case CURLE_FTP_COULDNT_USE_REST: // (31)
    	pwcError = L"The FTP REST command returned error. This should never happen if the server is sane.";
		break;
//	case CURLE_FTP_COULDNT_GET_SIZE: // (32)
//    	pwcError = L"The FTP SIZE command returned error. SIZE is not a kosher FTP command, it is an extension and not all servers support it. This is not a surprising error.";
		break;
	case CURLE_RANGE_ERROR: // (33)
    	pwcError = L"The HTTP server does not support or accept range requests.";
		break;
	case CURLE_HTTP_POST_ERROR: // (34)
    	pwcError = L"This is an odd error that mainly occurs due to internal confusion.";
		break;
	case CURLE_SSL_CONNECT_ERROR: // (35)
    	pwcError = L"A problem occurred somewhere in the SSL/TLS handshake. You really want the error buffer and read the message there as it pinpoints the problem slightly more. Could be certificates (file formats, paths, permissions), passwords, and others.";
		break;
	case CURLE_BAD_DOWNLOAD_RESUME: // (36)
    	pwcError = L"Attempting FTP resume beyond file size.";
		break;
	case CURLE_FILE_COULDNT_READ_FILE: // (37)
    	pwcError = L"A file given with FILE:// couldn't be opened. Most likely because the file path doesn't identify an existing file. Did you check file permissions?";
		break;
	case CURLE_LDAP_CANNOT_BIND: // (38)
    	pwcError = L"LDAP cannot bind. LDAP bind operation failed.";
		break;
	case CURLE_LDAP_SEARCH_FAILED: // (39)
    	pwcError = L"LDAP search failed.";
		break;
//	case CURLE_LIBRARY_NOT_FOUND: // (40)
//    	pwcError = L"Library not found. The LDAP library was not found.";
		break;
	case CURLE_FUNCTION_NOT_FOUND: // (41)
    	pwcError = L"Function not found. A required LDAP function was not found.";
		break;
	case CURLE_ABORTED_BY_CALLBACK: // (42)
    	pwcError = L"Aborted by callback. A callback returned 'abort' to libcurl.";
		break;
	case CURLE_BAD_FUNCTION_ARGUMENT: // (43)
    	pwcError = L"Internal error. A function was called with a bad parameter.";
		break;
//	case CURLE_BAD_CALLING_ORDER: // (44)
//    	pwcError = L"This is never returned by current libcurl.";
		break;
	case CURLE_INTERFACE_FAILED: // (45)
    	pwcError = L"Interface error. A specified outgoing interface could not be used. Set which interface to use for outgoing connections' source IP address with CURLOPT_INTERFACE.";
		break;
//	case CURLE_BAD_PASSWORD_ENTERED: // (46)
//    	pwcError = L"This is never returned by current libcurl.";
		break;
	case CURLE_TOO_MANY_REDIRECTS: // (47)
    	pwcError = L"Too many redirects. When following redirects, libcurl hit the maximum amount. Set your limit with CURLOPT_MAXREDIRS.";
		break;
	case CURLE_UNKNOWN_TELNET_OPTION: // (48)
    	pwcError = L"An option set with CURLOPT_TELNETOPTIONS was not recognized/known. Refer to the appropriate documentation.";
		break;
	case CURLE_TELNET_OPTION_SYNTAX: // (49)
    	pwcError = L"A telnet option string was Illegally formatted.";
		break;
/*
	case CURLE_OBSOLETE: // (50)
    	pwcError = L"This is not an error. This used to be another error code in an old libcurl version and is currently unused.";
	case CURLE_SSL_PEER_CERTIFICATE: // (51)
    	pwcError = L"The remote server's SSL certificate was deemed not OK.";
	case CURLE_GOT_NOTHING: // (52)
    	pwcError = L"Nothing was returned from the server, and under the circumstances, getting nothing is considered an error.";
	case CURLE_SSL_ENGINE_NOTFOUND: // (53)
    	pwcError = L"The specified crypto engine wasn't found.";
CURLE_SSL_ENGINE_SETFAILED (54)
    Failed setting the selected SSL crypto engine as default!
CURLE_SEND_ERROR (55)
    Failed sending network data.
CURLE_RECV_ERROR (56)
    Failure with receiving network data.
CURLE_SHARE_IN_USE (57)
    Share is in use
CURLE_SSL_CERTPROBLEM (58)
    problem with the local client certificate
CURLE_SSL_CIPHER (59)
    couldn't use specified cipher
CURLE_SSL_CACERT (60)
    peer certificate cannot be authenticated with known CA certificates
CURLE_BAD_CONTENT_ENCODING (61)
    Unrecognized transfer encoding
CURLE_LDAP_INVALID_URL (62)
    Invalid LDAP URL
CURLE_FILESIZE_EXCEEDED (63)
    Maximum file size exceeded
CURLE_FTP_SSL_FAILED (64)
    Requested FTP SSL level failed
CURLE_SEND_FAIL_REWIND (65)
    When doing a send operation curl had to rewind the data to retransmit, but the rewinding operation failed
CURLE_SSL_ENGINE_INITFAILED (66)
    Initiating the SSL Engine failed
CURLE_LOGIN_DENIED (67)
    The remote server denied curl to login (Added in 7.13.1)
CURLE_TFTP_NOTFOUND (68)
    File not found on TFTP server
CURLE_TFTP_PERM (69
    Permission problem on TFTP server
CURLE_TFTP_DISKFULL (70)
    Out of disk space on TFTP server
CURLE_TFTP_ILLEGAL (71)
    Illegal TFTP operation
CURLE_TFTP_UNKNOWNID (72)
    Unknown TFTP transfer ID
CURLE_TFTP_EXISTS (73)
    TFTP File already exists
CURLE_TFTP_NOSUCHUSER (74)
    No such TFTP user
CURLE_CONV_FAILED (75)
    Character conversion failed
CURLE_CONV_REQD (76)
    Caller must register conversion callbacks
CURLE_SSL_CACERT_BADFILE (77)
    Problem with reading the SSL CA cert (path? access rights?)

	*/
	}
	return pwcError;
}

const wchar_t* GetWebKitErrorDesc(int aiErrorCode){
	const wchar_t* pwcError = NULL;
	switch(aiErrorCode){
	case WebKitErrorFrameLoadInterruptedByPolicyChange: // 102
		pwcError = L"WebKitErrorFrameLoadInterruptedByPolicyChange";
		break;
	case WebKitErrorCannotShowMIMEType: // 100
		pwcError = L"WebKitErrorCannotShowMIMEType";
		break;
	case WebKitErrorCannotShowURL: // 101
		pwcError = L"WebKitErrorCannotShowURL";
		break;
	case WebKitErrorCannotFindPlugIn: // 200
		pwcError = L"WebKitErrorCannotFindPlugIn";
		break;
	case WebKitErrorCannotLoadPlugIn: // 201
		pwcError = L"WebKitErrorCannotLoadPlugIn";
		break;
	case WebKitErrorJavaUnavailable: // 202
		pwcError = L"WebKitErrorJavaUnavailable";
		break;
	}
	return pwcError;
}

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didStartProvisionalLoadForFrame( 
    /* [in] */ IWebView* webView,
    /* [in] */ IWebFrame* frame) { return S_OK; }

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didReceiveServerRedirectForProvisionalLoadForFrame( 
    /* [in] */ IWebView *webView,
    /* [in] */ IWebFrame *frame) { return S_OK; }

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didFailProvisionalLoadWithError( 
    /* [in] */ IWebView *webView,
    /* [in] */ IWebError *error,
    /* [in] */ IWebFrame *frame
){

	HRESULT hr = S_OK;

	CComPtr<IWebFrame> mainFrame = 0;

	hr = webView->mainFrame(&mainFrame);
	if(SUCCEEDED(hr) && mainFrame != NULL){
		// если ошибка не в главном фрейме
		if(mainFrame != frame){
			// то не будем ее выводить
			return S_OK;
		}
	}

	BSTR bstrError = NULL;
	error->localizedDescription(&bstrError);
	BSTR bstrURL = NULL;
	error->failingURL(&bstrURL);
	int iCode = 0;
	error->code(&iCode);
	const wchar_t* pwcError = L"Unknown error code";
	switch(iCode){
	case -999:
		break; // возможно прервана загрузка данных
	case WebKitErrorFrameLoadInterruptedByPolicyChange: // 102
	case WebKitErrorCannotShowMIMEType: // 100
	case WebKitErrorCannotShowURL: // 101
	case WebKitErrorCannotFindPlugIn: // 200
	case WebKitErrorCannotLoadPlugIn: // 201
	case WebKitErrorJavaUnavailable: // 202
	default:
		const wchar_t* pwcError = GetCURLErrorDesc(iCode);
		if(pwcError == NULL) 
			pwcError = GetWebKitErrorDesc(iCode);
		m_browser->ShowErrorPage(bstrURL, iCode, pwcError);
	}
	if(bstrError != NULL)
		SysFreeString(bstrError);
	if(bstrURL != NULL)
		SysFreeString(bstrURL);
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didCommitLoadForFrame( 
    /* [in] */ IWebView *webView,
    /* [in] */ IWebFrame *frame) { return S_OK; /*updateAddressBar(webView);*/ }

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didReceiveTitle( 
    /* [in] */ IWebView *webView,
    /* [in] */ BSTR title,
    /* [in] */ IWebFrame *frame) { return S_OK; }

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didReceiveIcon( 
    /* [in] */ IWebView *webView,
    /* [in] */ OLE_HANDLE hBitmap,
    /* [in] */ IWebFrame *frame) { return S_OK; }

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didFinishLoadForFrame( 
    /* [in] */ IWebView* webView,
    /* [in] */ IWebFrame* frame) 
{ 
	HRESULT hr = S_OK;

    CComPtr<IWebFrame> mainFrame = 0;

	hr = webView->mainFrame(&mainFrame);
	if(FAILED(hr) || mainFrame == NULL){
		return (hr!=S_OK)?hr:E_FAIL;
	}

	// если документ загрузился не в главный фрейм
	if(mainFrame != frame){
		// то пропускаем это событие
		return S_OK;
	}

	CComPtr<IWebDataSource> dataSource;
	hr = frame->dataSource(&dataSource);
	if(FAILED(hr) || dataSource == NULL){
		return (hr!=S_OK)?hr:E_FAIL;
	}
	CComPtr<IWebURLRequest> request;
	hr = dataSource->initialRequest(&request);
	if(FAILED(hr) || request == NULL){
		return (hr!=S_OK)?hr:E_FAIL;
	}
	BSTR bstrURL = NULL;
    hr = request->mainDocumentURL(&bstrURL);
	if(FAILED(hr) || bstrURL == NULL){
		return (hr!=S_OK)?hr:E_FAIL;
	}

//	request->URL(&bstrURL);
	m_browser->m_callBack->on_document_complete(bstrURL); 
	if(bstrURL != NULL)
		SysFreeString(bstrURL);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didFailLoadWithError( 
    /* [in] */ IWebView *webView,
    /* [in] */ IWebError *error,
    /* [in] */ IWebFrame *forFrame
){
	BSTR bstrError = NULL;
	error->localizedDescription(&bstrError);
	int iCode = 0;
	error->code(&iCode);
	if(bstrError != NULL)
	SysFreeString(bstrError);
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didChangeLocationWithinPageForFrame( 
    /* [in] */ IWebView *webView,
    /* [in] */ IWebFrame *frame) { return S_OK; }

HRESULT STDMETHODCALLTYPE CBrowserWebHost::willPerformClientRedirectToURL( 
    /* [in] */ IWebView *webView,
    /* [in] */ BSTR url,
    /* [in] */ double delaySeconds,
    /* [in] */ DATE fireDate,
    /* [in] */ IWebFrame *frame) { return S_OK; }

HRESULT STDMETHODCALLTYPE CBrowserWebHost::didCancelClientRedirectForFrame( 
    /* [in] */ IWebView *webView,
    /* [in] */ IWebFrame *frame) { return S_OK; }

HRESULT STDMETHODCALLTYPE CBrowserWebHost::willCloseFrame( 
    /* [in] */ IWebView *webView,
    /* [in] */ IWebFrame *frame) { return S_OK; }

#ifndef printf
int printf(const char *, ...){
return 0;
}
#endif

JSClassRef DispatchObjectClassCreate();

JSClassRef DispatchMethodClassCreate();

JSValueRef DispatchObjectGetProperty(JSContextRef, JSObjectRef, JSStringRef,
									 JSValueRef*);

JSValueRef DispatchObjectToString(JSContextRef, JSObjectRef, JSObjectRef,
								  size_t, const JSValueRef*, JSValueRef*);

bool DispatchObjectSetProperty(JSContextRef, JSObjectRef, JSStringRef,
							   JSValueRef, JSValueRef*);

void DispatchObjectFinalize(JSObjectRef);

JSValueRef DispatchMethodCallAsFunction(JSContextRef, JSObjectRef,
										JSObjectRef, size_t,
										const JSValueRef*, JSValueRef*);

JSValueRef DispatchMethodGetToString(JSContextRef, JSObjectRef, JSStringRef,
									 JSValueRef*);

JSValueRef DispatchMethodToString(JSContextRef, JSObjectRef, JSObjectRef,
								  size_t, const JSValueRef*, JSValueRef*);

void DispatchMethodFinalize(JSObjectRef);

static JSClassDefinition _dispatchObjectClassDef = { 0,
kJSClassAttributeNone, "DispatchObject", 0, 0, 0, 0,
DispatchObjectFinalize, 0, DispatchObjectGetProperty,
DispatchObjectSetProperty, 0, 0, 0, 0, 0, 0 };

static JSStaticValue _dispatchMethodStaticValues[] = {
	{ "toString", DispatchMethodGetToString, 0, kJSPropertyAttributeNone },
	{ 0, 0, 0, 0 }
};
static JSClassDefinition _dispatchMethodClassDef = { 0,
kJSClassAttributeNoAutomaticPrototype, "DispatchMethod", 0,
_dispatchMethodStaticValues, 0, 0, DispatchMethodFinalize, 0, 0, 0, 0,
0, DispatchMethodCallAsFunction, 0, 0, 0 };

/*
* The classes used to create DispatchObjects and DispatchMethods.
*/
static JSClassRef _dispatchObjectClass = DispatchObjectClassCreate();
static JSClassRef _dispatchMethodClass = DispatchMethodClassCreate();

JSClassRef GetDispatchObjectClass() {
	return _dispatchObjectClass;
}

JSClassRef GetDispatchMethodClass() {
	return _dispatchMethodClass;
}

JSClassRef DispatchObjectClassCreate() {
	JSClassRef dispClass = JSClassCreate(&_dispatchObjectClassDef);
	JSClassRetain(dispClass);
	return dispClass;
}

JSClassRef DispatchMethodClassCreate() {
	JSClassRef dispClass = JSClassCreate(&_dispatchMethodClassDef);
	JSClassRetain(dispClass);
	return dispClass;
}

JSObjectRef DispatchObjectCreate(JSContextRef jsContext, void* data)  // CBrowserWebHost*
{
	JSObjectRef dispInst = JSObjectMake(jsContext, _dispatchObjectClass, data);
	return dispInst;
}

class DispatchMethodData {
public:
	DispatchMethodData(void* obj, std::string& utf8Name)
		: _obj(obj), _utf8Name(utf8Name) { }
	~DispatchMethodData() {
	}
	void* _obj;
	std::string _utf8Name;
};

void DispatchMethodFinalize(JSObjectRef jsObject) {
	DispatchMethodData* data = reinterpret_cast<DispatchMethodData*>(
		JSObjectGetPrivate(jsObject));
	delete data;
}

void DispatchObjectFinalize(JSObjectRef jsObject) {
	CBrowserWebHost* webHost = (CBrowserWebHost*)(JSObjectGetPrivate(jsObject));
//	ReleaseJavaObject(jObject);
}

JSValueRef GetFunctionPrototype(JSContextRef jsContext, JSValueRef* exception) {
	JSObjectRef globalObject = JSContextGetGlobalObject(jsContext);
	JSStringRef fnPropName= JSStringCreateWithUTF8CString("Function");
	JSValueRef fnCtorValue = JSObjectGetProperty(jsContext, globalObject,
		fnPropName, exception);
	JSStringRelease(fnPropName);
	if (!fnCtorValue) {
		return JSValueMakeUndefined(jsContext);
	}

	JSObjectRef fnCtorObject = JSValueToObject(jsContext, fnCtorValue, exception);
	if (!fnCtorObject) {
		return JSValueMakeUndefined(jsContext);
	}

	JSStringRef protoPropName = JSStringCreateWithUTF8CString("prototype");
	JSValueRef fnPrototype = JSObjectGetProperty(jsContext, fnCtorObject,
		protoPropName, exception);
	JSStringRelease(protoPropName);
	if (!fnPrototype) {
		return JSValueMakeUndefined(jsContext);
	}

	return fnPrototype;
}

JSObjectRef DispatchMethodCreate(JSContextRef jsContext, std::string& name, void* obj) 
{
	JSObjectRef dispInst = JSObjectMake(jsContext, _dispatchMethodClass,
		new DispatchMethodData(obj, name));

	// This could only be cached relative to jsContext.
	JSValueRef fnProtoValue = GetFunctionPrototype(jsContext, NULL);
	JSObjectSetPrototype(jsContext, dispInst, fnProtoValue);
	return dispInst;
}

JSValueRef DispatchObjectGetProperty(JSContextRef jsContext,
									 JSObjectRef jsObject, JSStringRef jsPropertyName,
									 JSValueRef* jsException) 
{
	// If you call toString on a DispatchObject, you should get the results
	// of the java object's toString invcation.
	if (JSStringIsEqualToUTF8CString(jsPropertyName, "toString")) {
		JSObjectRef jsFunction = JSObjectMakeFunctionWithCallback(jsContext,
			jsPropertyName, DispatchObjectToString);
		return jsFunction;
	}

	// The class check is omitted because it should not be possible to tear off
	// a getter.
	CBrowserWebHost* webHost = (CBrowserWebHost*)(JSObjectGetPrivate(jsObject));
	if (!webHost) {
		return JSValueMakeUndefined(jsContext);
	}
	std::wstring wsPropertyName;
	wsPropertyName.assign(JSStringGetCharactersPtr(jsPropertyName), JSStringGetLength(jsPropertyName));

	std::string propertyName = cLPCSTR(wsPropertyName.c_str());

	JSObjectRef oMethod = DispatchMethodCreate(jsContext, propertyName, webHost);
	
//	JSValueRef vMethod = ConvertToType(jsContext, oMethod, kJSTypeObject, NULL);
	JSValueRef vMethod = (JSValueRef)oMethod;
	return vMethod;
/*
	std::string propertyName = static_cast<const jchar*>(JSStringGetCharactersPtr(jsPropertyName);
		// static_cast<jsize>(JSStringGetLength(jsPropertyName)));
	if (!jObject || !jPropertyName) {
		return JSValueMakeUndefined(jsContext);
	}

	JSValueRef jsResult = reinterpret_cast<JSValueRef>(
		_javaEnv->CallIntMethod(jObject, _javaDispatchObjectGetFieldMethod,
		reinterpret_cast<jint>(jsContext),
		jPropertyName));
	if (!jsResult) {
		return JSValueMakeUndefined(jsContext);
	}

	// Java left us an extra reference to eat.
	JSValueUnprotectChecked(jsContext, jsResult);
	return jsResult;
*/
return JSValueMakeUndefined(jsContext);
}

bool DispatchObjectSetProperty(JSContextRef jsContext, JSObjectRef jsObject,
	JSStringRef jsPropertyName, JSValueRef jsValue, JSValueRef* jsException) 
{
	// The class check is omitted because it should not be possible to tear off
	// a getter.
	CBrowserWebHost* webHost = (CBrowserWebHost*)(JSObjectGetPrivate(jsObject));
/*
	jstring jPropertyName = _javaEnv->NewString(
		static_cast<const jchar*>(JSStringGetCharactersPtr(jsPropertyName)),
		static_cast<jsize>(JSStringGetLength(jsPropertyName)));
	if (!jObject || !jPropertyName || _javaEnv->ExceptionCheck()) {
		_javaEnv->ExceptionClear();
		return false;
	}

	JSValueProtectChecked(jsContext, jsValue);

	_javaEnv->CallIntMethod(jObject, _javaDispatchObjectSetFieldMethod,
		reinterpret_cast<jint>(jsContext), jPropertyName,
		reinterpret_cast<jint>(jsValue));
	if (_javaEnv->ExceptionCheck()) {
		_javaEnv->ExceptionClear();
		return false;
	}
*/
	return true;
}

JSValueRef DispatchObjectToString(JSContextRef jsContext, JSObjectRef,
								  JSObjectRef jsThis, size_t, const JSValueRef*, JSValueRef*) 
{
	// This function cannot be torn off and applied to any JSValue. If this does
	// not reference a DispatchObject, return undefined.
	if (!JSValueIsObjectOfClass(jsContext, jsThis, GetDispatchObjectClass())) {
		return JSValueMakeUndefined(jsContext);
	}

	CBrowserWebHost* webHost = (CBrowserWebHost*)(JSObjectGetPrivate(jsThis));
/*
	jstring jResult = reinterpret_cast<jstring>(
		_javaEnv->CallObjectMethod(jObject, _javaDispatchObjectToStringMethod));
	if (_javaEnv->ExceptionCheck()) {
		return JSValueMakeUndefined(jsContext);
	} else if (!jResult) {
		return JSValueMakeNull(jsContext);
	} else {
		JStringWrap result(_javaEnv, jResult);
		JSStringRef resultString = JSStringCreateWithCharacters(
			static_cast<const JSChar*>(result.jstr()),
			static_cast<size_t>(result.length()));
		JSValueRef jsResultString = JSValueMakeString(jsContext, resultString);
		JSStringRelease(resultString);
		return jsResultString;
	}
*/
return JSValueMakeUndefined(jsContext);
}

JSValueRef DispatchMethodCallAsFunction(JSContextRef jsContext,
	JSObjectRef jsFunction, JSObjectRef jsThis, size_t argumentCount,
	const JSValueRef arguments[], JSValueRef* exception) 
{
	// We don't need to check the class here because we take the private
	// data from jsFunction and not jsThis.

	DispatchMethodData* data = reinterpret_cast<DispatchMethodData*>(
		JSObjectGetPrivate(jsFunction));

	CBrowserWebHost* webHost = (CBrowserWebHost*)(data->_obj);

	return webHost->ExternalCall(jsContext, data->_utf8Name.c_str(), argumentCount, arguments);
/*
	jintArray jArguments = _javaEnv->NewIntArray(argumentCount);
	if (!jArguments || _javaEnv->ExceptionCheck()) {
		return JSValueMakeUndefined(jsContext);
	}

	// This single element int array will be passed into the java call to allow the
	// called java method to raise an exception. We will check for a non-null value
	// after the call is dispatched.
	jintArray jException = _javaEnv->NewIntArray(1);
	if (!jException || _javaEnv->ExceptionCheck()) {
		return JNI_FALSE;
	}

	for (size_t i = 0; i < argumentCount; ++i) {
		JSValueRef arg = arguments[i];
		// Java will take ownership of the arguments.
		JSValueProtectChecked(jsContext, arg);
		_javaEnv->SetIntArrayRegion(jArguments, i, 1, reinterpret_cast<jint*>(&arg));
		if (_javaEnv->ExceptionCheck()) {
			return JSValueMakeUndefined(jsContext);
		}
	}

	// Java will take ownership of this.
	JSValueProtectChecked(jsContext, jsThis);

	JSValueRef jsResult = reinterpret_cast<JSValueRef>(_javaEnv->CallIntMethod(jObject,
		_javaDispatchMethodInvokeMethod, reinterpret_cast<jint>(jsContext),
		reinterpret_cast<jint>(jsThis), jArguments, jException));
	if (_javaEnv->ExceptionCheck()) {
		return JSValueMakeUndefined(jsContext);
	}

	JSValueRef jsException = NULL;
	_javaEnv->GetIntArrayRegion(jException, 0, 1, reinterpret_cast<jint*>(&jsException));
	if (!_javaEnv->ExceptionCheck() && jsException) {
		// If the java dispatch set an exception, then we pass it back to our caller.
		if (exception) {
			*exception = jsException;
		}
		// Java left us an extra reference to eat.
		JSValueUnprotectChecked(jsContext, jsException);
	}

	// Java left us an extra reference to eat.
	JSValueUnprotectChecked(jsContext, jsResult);
	return jsResult;
*/
return JSValueMakeUndefined(jsContext);
}

JSValueRef DispatchMethodToString(JSContextRef jsContext, JSObjectRef,
JSObjectRef thisObject, size_t, const JSValueRef*, JSValueRef*) 
{
	// This function cannot be torn off and applied to any JSValue. If this does
	// not reference a DispatchMethod, return undefined.
	if (!JSValueIsObjectOfClass(jsContext, thisObject, GetDispatchMethodClass())) {
		return JSValueMakeUndefined(jsContext);
	}

	DispatchMethodData* data = reinterpret_cast<DispatchMethodData*>(
		JSObjectGetPrivate(thisObject));
	/*
	std::ostringstream ss;
	ss << "function " << data->_utf8Name << "() {\n    [native code]\n}\n";
	JSStringRef stringRep = JSStringCreateWithUTF8CString(ss.str().c_str());
	JSValueRef jsStringRep = JSValueMakeString(jsContext, stringRep);
	JSStringRelease(stringRep);
	return jsStringRep;
*/
return JSValueMakeUndefined(jsContext);
}

JSValueRef DispatchMethodGetToString(JSContextRef jsContext,
	JSObjectRef jsObject, JSStringRef jsPropertyName, JSValueRef* jsException) 
{
	JSObjectRef toStringFn = JSObjectMakeFunctionWithCallback(jsContext,
		jsPropertyName, DispatchMethodToString);
	return toStringFn;
}

JSValueRef js_to_scheme_function(JSContextRef context, JSObjectRef object, JSObjectRef thisObject,
	size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) 
{
	// return (JSValueRef) scheme_js_call_receptor((unsigned int) context,(unsigned int) object,(unsigned int) thisObject,(unsigned int) argumentCount,(unsigned int) *arguments,(unsigned int) exception);
	printf(" *** ! in JS callback.\n");
	return JSValueMakeNumber(context, 0.0);
//	return (JSValueRef) 0;
}

void createGlobalJSFunction(const char* name,
	JSContextRef context,
	JSObjectRef globalObject,
	JSObjectCallAsFunctionCallback functionPointer) 
{
	JSStringRef functionNameString = JSStringCreateWithUTF8CString(name);
	JSObjectSetProperty(context,
		globalObject,
		functionNameString,
		JSObjectMakeFunctionWithCallback(context, functionNameString, functionPointer),
		kJSPropertyAttributeNone,
		NULL);
	JSStringRelease(functionNameString);
}

/* [local] */ HRESULT STDMETHODCALLTYPE CBrowserWebHost::windowScriptObjectAvailable( 
    /* [in] */ IWebView *webView,
    /* [in] */ JSContextRef context,
    /* [in] */ JSObjectRef windowScriptObject)  
{
	JSObjectRef contextGlobalObject = windowScriptObject;

	JSStaticFunction staticFunctions[] = {
		{"c",js_to_scheme_function,kJSPropertyAttributeNone},
		{0, 0, 0}
	};

	JSClassDefinition class_definition = {
		0,
			kJSClassAttributeNone,

			"S",
			NULL,

			NULL,
			staticFunctions,

			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
	};
	JSClassRef c;


	// printf("Creating class in Webkit's JS.");
	c = JSClassCreate(&class_definition);
	// printf("Created, value is %i.",(long) c);

	// printf("extendHost: entered\n");
	JSObjectRef o = JSObjectMake(context,c,NULL);
	// printf("extendHost: a. %i.\n",o);
	JSStringRef s = JSStringCreateWithUTF8CString("S");
	// printf("extendHost: b. %i.\n",s);
	JSObjectSetProperty(context,
		contextGlobalObject,
		s,
		o,
		kJSPropertyAttributeNone,
		NULL);
	// printf("extendHost: c\n");
	JSStringRelease(s);
	// printf("extendHost: d\n");
	createGlobalJSFunction(staticFunctions[0].name,
		context,
		contextGlobalObject,
		staticFunctions[0].callAsFunction);
	// printf("extendHost: ready\n");


	// получить объект window
	JSStringRef sWindow = JSStringCreateWithUTF8CString("window");
	JSValueRef vWindow = JSObjectGetProperty(context, contextGlobalObject, sWindow, NULL);
	JSObjectRef oWindow = JSValueToObject(context, vWindow, NULL);
	JSStringRelease(sWindow);

	// создать JS-класс для window.external
	JSStaticFunction externalStaticFunctions[] = {
		{"meth",js_to_scheme_function,kJSPropertyAttributeNone},
		{0, 0, 0}
	};

	JSClassDefinition externalClassDefinition = {
		0,
			kJSClassAttributeNone,

			"External",
			NULL,

			NULL,
			externalStaticFunctions,

			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
	};
	JSClassRef cExternal;
	cExternal = JSClassCreate(&externalClassDefinition);
	// создаем объект external
//	JSObjectRef oExternal = JSObjectMake(context, cExternal, NULL);
	JSObjectRef oExternal = DispatchObjectCreate(context, this);

	// устанавливаем свойство external у window
	JSStringRef sExternal = JSStringCreateWithUTF8CString("external");
	JSObjectSetProperty(context,
		oWindow,
		sExternal,
		oExternal,
		kJSPropertyAttributeNone,
		NULL);
	JSStringRelease(sWindow);

	return S_OK; 
}

JSValueRef CBrowserWebHost::ExternalCall(JSContextRef context, const char* methodName, unsigned int argumentCount, const JSValueRef arguments[]){
	std::wstring wsMethodName = cLPWCSTR(methodName);
	VARIANT vRes; vRes.vt = VT_ERROR;
	VARIANT* paArgs = new VARIANT[argumentCount];
	// преобразовать аргументы во что-то удобоваримое (VARIANT) и вызывать RMML-метод 
	for(int i = 0; i < (int)argumentCount; i++){
		const JSValueRef jsvArg = arguments[i];
		VARIANT* pArg = &(paArgs[argumentCount - i - 1]);
		pArg->vt = VT_VOID;
		JSType jstype = JSValueGetType(context, jsvArg);
		switch(jstype){
		case kJSTypeUndefined:
			pArg->vt = VT_VOID;
			break;
		case kJSTypeNull:
			pArg->vt = VT_NULL;
			break;
		case kJSTypeBoolean:
			pArg->vt = VT_BOOL;
			pArg->boolVal = (VARIANT_BOOL)JSValueToBoolean(context, jsvArg);
			break;
		case kJSTypeNumber:
			pArg->vt = VT_R8;
			pArg->dblVal = (DOUBLE)JSValueToNumber(context, jsvArg, NULL);
			break;
		case kJSTypeString:{
			JSStringRef jssVal = JSValueToStringCopy(context, jsvArg, NULL);
			pArg->vt = VT_BSTR;
			pArg->bstrVal = SysAllocStringLen(JSStringGetCharactersPtr(jssVal), (UINT)JSStringGetLength(jssVal));
			JSStringRelease(jssVal);
			}break;
		case kJSTypeObject:
			pArg->vt = VT_NULL;
			break;
		}
	}
	m_browser->m_callBack->external_callback(wsMethodName.c_str(), argumentCount, paArgs, &vRes, false);
	for(int i = 0; i < (int)argumentCount; i++){
		VARIANT* pArg = &(paArgs[i]);
		if(pArg->vt == VT_BSTR){
			SysFreeString(pArg->bstrVal);
		}
	}
	delete[] paArgs;
	JSValueRef jsvRes = JSValueMakeUndefined(context);
	switch(vRes.vt){
	case VT_NULL:
		jsvRes = JSValueMakeNull(context);
		break;
	case VT_BOOL:
		jsvRes = JSValueMakeBoolean(context, vRes.boolVal?true:false);
		break;
	case VT_R8:
		jsvRes = JSValueMakeNumber(context, vRes.dblVal);
		break;
	case VT_BSTR:{
		JSStringRef jssRef = JSStringCreateWithCharacters(vRes.bstrVal, wcslen(vRes.bstrVal));
		jsvRes = JSValueMakeString(context, jssRef);
		}break;
	}
	return jsvRes;
}

/*
CURLMcode
This is the generic return code used by functions in the libcurl multi interface. Also consider curl_multi_strerror(3).

CURLM_CALL_MULTI_PERFORM (-1)
This is not really an error. It means you should call curl_multi_perform(3) again without doing select() or similar in between.
CURLM_OK (0)
Things are fine.
CURLM_BAD_HANDLE (1)
The passed-in handle is not a valid CURLM handle.
CURLM_BAD_EASY_HANDLE (2)
An easy handle was not good/valid. It could mean that it isn't an easy handle at all, or possibly that the handle already is in used by this or another multi handle.
CURLM_OUT_OF_MEMORY (3)
You are doomed.
CURLM_INTERNAL_ERROR (4)
This can only be returned if libcurl bugs. Please report it to us!
CURLM_BAD_SOCKET (5)
The passed-in socket is not a valid one that libcurl already knows about. (Added in 7.15.4) 
*/