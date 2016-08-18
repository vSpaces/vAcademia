// Conection.h: interface for the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONECTION_H__2B0EBE03_B1D4_44E0_AA80_A41B2A91F145__INCLUDED_)
#define AFX_CONECTION_H__2B0EBE03_B1D4_44E0_AA80_A41B2A91F145__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock.h>
#include "client_data.h"
#include "rtp.h"
#include <queue>


#ifndef SOUND_CONNECT_H
#include "SoundConnect.h"
#define SOUND_CONNECT_H
#endif


//  Socket-specific types
typedef WORD SOCKEVENT;					// An asynchronous socket event

typedef struct _NEW_HOST_PARAMS {
    LPSTR  pszHostName;					// Name
    LPIN_ADDR paddr;					// Address
    unsigned short *port_no;			// Port
    ULONG laddr;
    BYTE bBuffer[MAXGETHOSTSTRUCT];		// For saving HOSTENT information
} NEW_HOST_PARAMS, *LPNEW_HOST_PARAMS;


typedef class CClient_Data FAR *LPCLIENT_DATA;
typedef class CClient_Data CLIENT_DATA;
 
class CSoundConnect;
class CConnection  
{
public:
	BOOL bCommandRelease, bControlRelease;
	unsigned short free_port;
	CHTTP* pHTTP;

	HANDLE hThread;							// Потоки для Command
	HANDLE hThread2;						// Потоки для Control		
	SOCKET sCommand;						//Command socket
	int OutputEnd;
	DWORD dwThreadId;						// Поток для слушания данных с сокета
	DWORD dwThreadIdCtrl;						// Поток для слушания данных с сокета
	int stop_thread;

	LPCLIENT_DATA pClientData;				// Date of the clients
//    CRTP rtp_p;								// RTP class variable

private:
	int num_receive_packet;					 // The number of receive messages;
	SOCKET sControl;						//RTP control port
	
	NEW_HOST_PARAMS NewHostParams;          // Parameters for NewHost dialog
	char ourSendingHost[16];
    struct sockaddr_in name;			// Target system address

	int outputTimeout;			// Output release timeout
	long outputPending;                 // Output buffers in queue

protected:
	void Init_session_key();							// Generate a random session key for RTP

public:
	SOCKERR ReleaseSocket(SOCKET *psock);
	void InitThread(CSoundConnect *sconnect_);
	int Init_sockets(unsigned short port); 
	void Listen();
	void ListenCtrl();
	void PlaySoundFile();
	BOOLEAN InitOut;
	void TimerReactionPlay();
	int readWaveInit(LPSTR szFileName);
	void startSoundFile(LPSTR pszFile);
	void SendSoundFile();
	unsigned short findPort(SOCKET s);
	BOOL loop_control_port(LPCLIENT_DATA d);
	short get_pClientData_SHORT(int id);
	int loop_samples(LPCLIENT_DATA d);
	int get_pClientData_INT(int);
	LPCLIENT_DATA get_pClientData();
	void SetData(int id, int val);
	void loop_flush(LPCLIENT_DATA d);
	int CheckpClientDataBOOL(int id);
	int CheckPointer(int id);
	//Functions
	SOCKERR CreateSocket(SOCKET *psock, INT type, ULONG address, PORT port);
	void UnInit();										// UnInitialize socket mode
	
	//Constructor
	CConnection(CHTTP *aHTTP);
	virtual ~CConnection();
	CSoundConnect *sconnect;

	BOOLEAN SOCKET_STATE;			// Инициализированы ли сокеты в конструкторе 
};

#endif // !defined(AFX_CONECTION_H__2B0EBE03_B1D4_44E0_AA80_A41B2A91F145__INCLUDED_)
