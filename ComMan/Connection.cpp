// Conection.cpp: implementation of the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Connection.h"
#include <fstream>
#include <string.h>
#include "ThreadAffinity.h"
#include "CrashHandlerInThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LPSTR pszClientClass;

DWORD WINAPI ListenThread(PVOID pvParam) { 
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	CoInitialize(NULL);
	CConnection *new_connection=(CConnection *) pvParam;
    while(1)
	{ if (new_connection->stop_thread==TRUE) break; 
	  new_connection->Listen();
	}
	new_connection->bCommandRelease=TRUE;
	CoUninitialize();
	
	return(0); 
} 

DWORD WINAPI ListenThreadCtrl(PVOID pvParam) { 
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	CConnection *new_connection=(CConnection *) pvParam;
	CoInitialize(NULL);
    while(1)
	{ if (new_connection->stop_thread==TRUE) break; 
	  new_connection->ListenCtrl();
	}
	new_connection->bControlRelease=TRUE;
	CoUninitialize();
	return(0); 
} 

typedef class CClient_Data CLIENT_DATA;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CConnection::CConnection(CHTTP *aHTTP) : pHTTP( aHTTP), bCommandRelease(FALSE), bControlRelease(FALSE) 
{
	outputPending=0; SOCKET_STATE = TRUE;
	outputTimeout = 0;
	InitOut=FALSE; 	OutputEnd=FALSE;
	stop_thread=FALSE;
	bool flag_free_port=FALSE;
	free_port=COMMAND_PORT;
	long res;
	bool connection_OK = TRUE;
	pClientData = NULL;
	
	while (!flag_free_port)
	{
		res=Init_sockets(free_port);
		if( res > 0) 
		{
			flag_free_port = TRUE; 
		}
		else
		{ 
			if (res==0)	
			{ 
				free_port+=2;
			}
			else 
			{ 
				connection_OK=FALSE; break; 
			}
		}
	}

    if (!connection_OK) return;
	wchar_t sQuery[11];
	wcscpy_s(sQuery, 11, L"get_IP.php");
	CComString sParams;
	sParams.Format( "port=%hu", free_port);
	//CComBSTR bsParams(sParams);
	DWORD dwNewDataSize = 0;
	BYTE* pNewData = NULL;
	if (pHTTP != NULL)
	{
		int res=pHTTP->sendQueryData( (wchar_t*)sQuery, sParams.AllocSysString(), &dwNewDataSize, &pNewData);
		if (pNewData != NULL && res == qeNoError)
		{
			int result;
			unsigned char *pdest=_mbsstr(pNewData,(const unsigned char*)"HTML");
			result = pdest - pNewData + 1;
			if (result>0)
			{
				pClientData = MP_NEW(CClient_Data);
			}
			else
			{
				MP_NEW_V2(pClientData, CClient_Data, dwNewDataSize,pNewData);	
			}
			pClientData->Send_Command("IN");		  
		}  
		else pClientData = MP_NEW(CClient_Data);
	}
}


CConnection::~CConnection()
{
	stop_thread=TRUE; 
	ReleaseSocket(&sCommand);
	ReleaseSocket(&sControl);

	while (bCommandRelease==FALSE && bControlRelease==FALSE)
		Sleep(100); 

	pClientData->Send_Command("OUT");
	MP_DELETE( pClientData);
}

SOCKERR CConnection::ReleaseSocket(SOCKET *psock)
{
    if (closesocket(*psock)!=0)
		return WSAGetLastError();
    
	*psock = INVALID_SOCKET;
	return 0;

}

//	CREATESOCKET  --  Create a new socket
SOCKERR CConnection::CreateSocket(SOCKET *psock, INT type, ULONG address, PORT port)
{
    SOCKET  sNew;
    SOCKERR serr;

    //  Create the socket

    sNew = socket(AF_INET, type, 0);
    serr = (sNew == INVALID_SOCKET) ? WSAGetLastError(): 0;

    if (serr == 0)
	{
        SOCKADDR_IN sockAddr;
       //  Bind an address to the socket

        sockAddr.sin_family = AF_INET;
        sockAddr.sin_addr.s_addr = address;
        sockAddr.sin_port = port;

        if (bind(sNew, (SOCKADDR *) &sockAddr, sizeof(sockAddr)) != 0) 
		{ serr = WSAGetLastError(); }
    }

    if (serr != 0) {
        closesocket(sNew);
        sNew = INVALID_SOCKET;
    }

    *psock = sNew;
    return serr;
}

void CConnection::Listen()
{
   static int ab;
   SOCKADDR_IN addrClient;
   INT cbAddrClient=sizeof(addrClient);
   Sbuffer received;
   Sbuffer *buf=&received;
   int rll = recvfrom(sCommand, (CHAR *)buf , sizeof(Sbuffer), 0, (SOCKADDR *) &addrClient, &cbAddrClient);
   static int sch;
   if (rll == SOCKET_ERROR)  
	{	//int err= WSAGetLastError();
	} // Endif

   if (rll!=0)
   { 
	if (rll== -1) {return; }
	if (buf->buffer_len == 0) { return; } // Nothing
	// Передача буфера для проигрывания SoundMan
	//buf->id=pClientData->ReturnIndexFromAddress(addrClient);
	STransfer_info info;
	info.id = pClientData->ReturnIndexFromAddress(addrClient);
	buf->id = (WORD)info.id;	
	info.command = PUSH_SB;
	info.data_length = buf->buffer_len;
	info.num_packets = buf->num_packet;
	//info.id = buf->id;
	rtl_memcpy(info.data, va_arrsize_in_bytes( info.data), buf->buffer_val,buf->buffer_len);
	struct STransfer_info *pinfo = &info;
	int pktlen=buf->buffer_len+3*sizeof(int);
	ATLTRACE("Sound\n");
	sconnect->HandleData(pktlen, (BYTE *) pinfo);
  }

}

void CConnection::ListenCtrl()
{
   char buff[1024];
 
   SOCKADDR_IN addrClient;
   INT cbAddrClient=sizeof(addrClient);
   int rll = recvfrom(sControl, &buff[0], sizeof(buff)-1, 0, (SOCKADDR *) &addrClient, &cbAddrClient);
   if (rll == SOCKET_ERROR) 
	{	//int err= WSAGetLastError();
	} // Endif

   if (rll!=0)
   { 
	if (rll== -1) {return; }
 	num_receive_packet++;

	// 
	int result = strcmp( buff, "IN");
	if (result==0) 
	{ pClientData->Add_Client(addrClient); 
	  LPSTR st;
      char str[10];
	  STransfer_info info;
	  struct STransfer_info *pinfo = &info;
	  info.command=NEW_CLIENT;
	  _itoa_s(pClientData->REAL_GROUP_SIZE,str, 10, 10);	
	  st=str;
	  info.data_length=strlen(str);
	  rtl_memcpy(info.data, va_arrsize_in_bytes(info.data), st,info.data_length);
	  int size=info.data_length + sizeof(int) + sizeof(LONG);
	  if (sconnect!=NULL)
		sconnect->HandleData(size, (BYTE *) pinfo);
	  return; 
	}
	result = strcmp( buff, "OUT");
	if (result==0) { pClientData->Delete_Client(addrClient); return; }
	int num=rtl_atoi(buff);
	if (num!=0) 
	{ STransfer_info info;
	  info.command = NUM_AVATAR;
	  info.data_length = rll;
	  info.id = pClientData->ReturnIndexFromAddress(addrClient);
	  rtl_memcpy(info.data, va_arrsize_in_bytes(info.data), buff,rll);
	  ATLTRACE("Number avatar = %d",num);
	  struct STransfer_info *pinfo = &info;
	  sconnect->HandleData(rll+sizeof(int), (BYTE *) pinfo);
	}
  }
}

void CConnection::UnInit()
{  WSACleanup(); 
}



void CConnection::Init_session_key()
{
//	rtp_p.Init();
}

int CConnection::CheckPointer(int id)
{
	if (id==CLIENTDATA)
	{ if (pClientData==NULL) return FALSE;
	   return TRUE;	
	}
	
  return 2;
}

int CConnection::CheckpClientDataBOOL(int id)
{
	return pClientData->CheckBOOL(id);
}



//	LOOP_FLUSH  --  Flush any queued loopback packets
void CConnection::loop_flush(LPCLIENT_DATA d)
{
	while (d->llhead != NULL) 
	{
		LPSTR b = (LPSTR) d->llhead;
		
		d->llhead = d->llhead->llnext;
		//GlobalFreePtr(b); 
		MP_DELETE_ARR(b);
	}
	d->llhead = d->lltail = NULL;
	d->localLoopback &= ~LOOPBACK_PLAYING;

}


void CConnection::SetData(int id, int val)
{
	pClientData->SetData(id,val)	;
}

LPCLIENT_DATA CConnection::get_pClientData()
{
	return pClientData;
}

int CConnection::get_pClientData_INT(int id)
{
	return pClientData->get_INT(id);
}

/*	LOOP_SAMPLES  --  Return number of samples in next loopback
					  packet.  If none is queued, return zero.  */
int CConnection::loop_samples(LPCLIENT_DATA d)
{
	struct localLoop *p = d->llhead;	
	return (p == NULL) ? 0 : p->llsamples;
}

short CConnection::get_pClientData_SHORT(int id)
{
	return pClientData->get_SHORT(id);
}

/*  LOOP_CONTROL_PORT  --  Returns TRUE if a loopback packet is
						   queued and it is destined for the control
						   port.  */
BOOL CConnection::loop_control_port(LPCLIENT_DATA d)
{
	struct localLoop *p = d->llhead;	

	return (p == NULL) ? 0 : (ntohs(p->lladdr.sin_port) & 1);

}



unsigned short CConnection::findPort(SOCKET s)
{
	if( s == 0)
		return 0;
	return COMMAND_PORT;
}



//Sending sound file
void CConnection::SendSoundFile()
{
//	pClientData->szFile="linda.wav";
	startSoundFile("linda2.wav");
}

//  REVLONG  --  Reverse bytes in a long

void revlong(long *l)
{
	long l1 = *l;
	LPSTR ip = (LPSTR) &l1, op = (LPSTR) l;
	
	op[0] = ip[3];
	op[1] = ip[2];
	op[2] = ip[1];
	op[3] = ip[0]; 
}


// Send wave from file .wav
void CConnection::startSoundFile(LPSTR pszFile)
{
    HFILE hFile = HFILE_ERROR;
    char magic[4];

    if (pClientData->timeout > 0) {
    	pClientData->timeout = 0;
    }

    //  Try to open it
    hFile = _lopen(pszFile, OF_READ | OF_SHARE_DENY_WRITE);

    if (hFile == HFILE_ERROR) 
	{  //  Error opening file
		::MessageBox(NULL,"Error opening this file","Error", MB_ICONSTOP | MB_OK);
        goto FatalError;
    }

    _lread(hFile, magic, sizeof(long));
    
    /* See if it's a chunky, wavy RIFF.  If so, delegate
       handling of the file to the multimedia I/O package. */
       
	if (memcmp(magic, "RIFF", 4) == 0) {
		_lclose(hFile);
		if (!readWaveInit(pszFile)) {
			return;
		}
	} else {
	    /* If the file has a Sun .au file header, skip it.
	       Note that we still blithely assume the file is
	       8-bit ISDN u-law encoded at 8000 samples per
	       second. */
	
	    if (memcmp(magic, ".snd", 4) == 0) {
	        long startpos;
	
	        _lread(hFile, &startpos, sizeof(long));
	        revlong(&startpos);
	        _llseek(hFile, startpos, 0);
	    } else {
	        _llseek(hFile, 0L, 0);
	    }
    }
    
    pClientData->state = Transferring;
    if (pClientData->timeout > 0) {
    	pClientData->timeout = 0;
    }
	TimerReactionPlay();

    return;

FatalError:
    if (hFile != HFILE_ERROR) {
        _lclose(hFile);
    }

}


//		Read .WAV audio files
int CConnection::readWaveInit(LPSTR szFileName)
{
	if( szFileName == NULL)
		return -1;
//	return sound.readWaveInit(szFileName);
	return -1;
}
/*  READWAVENEXT  --  Fill a sound buffer with the next block
					  of data from the WAVE file.  When the end
					  of file is reached, FALSE is returned,
					  otherwise the return is the number of samples
					  stored in the sound buffer.  */


void CConnection::TimerReactionPlay()
{
/*	int bread = sound.readWaveNext_andSend(pClientData,ourSendingHost,rtp_p); 
	if (bread <= 0)		
	{ OutputEnd=TRUE;
 	  sound.readWaveTerm();
	  return;
	} 
	else
	{ TimerReactionPlay();
	}
	*/
}



void CConnection::PlaySoundFile()
{
	LPSTR pszFile="linda2.wav";

    HFILE hFile = HFILE_ERROR;
    char magic[4];

	OutputEnd=FALSE;
    //  Try to open it
    hFile = _lopen(pszFile, OF_READ | OF_SHARE_DENY_WRITE);

    if (hFile == HFILE_ERROR) 
	{  //  Error opening file
		::MessageBox(NULL,"Error opening this file","Error", MB_ICONSTOP | MB_OK);
        goto FatalError;
    }

    _lread(hFile, magic, sizeof(long));
    
      
	if (memcmp(magic, "RIFF", 4) == 0) {
		_lclose(hFile);
		if (!readWaveInit(pszFile)) {
			return;
		}
	} else {
	
	    if (memcmp(magic, ".snd", 4) == 0) {
	        long startpos;
	
	        _lread(hFile, &startpos, sizeof(long));
	        revlong(&startpos);
	        _llseek(hFile, startpos, 0);
	    } else {
	        _llseek(hFile, 0L, 0);
	    }
    }
  
	TimerReactionPlay();

	return;

FatalError: 
	return;

}


int CConnection::Init_sockets(unsigned short port)
{
  SOCKERR serr;
  WSADATA wsadata;

  //  Initialize the sockets library
	serr = WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata);
    if (serr != 0) { ::MessageBox(NULL, "Socket initialization error.", "Error", MB_ICONERROR | MB_OK); return -1; }

	if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) 
	{ ::MessageBox(NULL,"The minimum WinSock version - 1.0", "Error", MB_ICONERROR | MB_OK); return -2; }

	serr = CreateSocket(&sCommand, SOCK_DGRAM, htonl(INADDR_ANY), htons(port));
	if (serr != 0) return FALSE;

    serr = CreateSocket(&sControl, SOCK_DGRAM, htonl(INADDR_ANY),htons(port+1));
	if (serr != 0) return FALSE;
	
	return TRUE;
}

void CConnection::InitThread(CSoundConnect *sconnect_)
{
	sconnect = sconnect_;

	CSimpleThreadStarter listenThreadStarter(__FUNCTION__);	
	listenThreadStarter.SetRoutine(ListenThread);
	listenThreadStarter.SetParameter((PVOID)this);
	hThread = listenThreadStarter.Start();

	CSimpleThreadStarter listenCtrlThreadStarter(__FUNCTION__);	
	listenCtrlThreadStarter.SetRoutine(ListenThreadCtrl);
	listenCtrlThreadStarter.SetParameter((PVOID)this);
	hThread2 = listenCtrlThreadStarter.Start();

//	CloseHandle(hThread2); CloseHandle(hThread); 

}


