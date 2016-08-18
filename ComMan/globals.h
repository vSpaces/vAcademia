
#define DESIRED_WINSOCK_VERSION 0x0101  // we'd like winsock ver 1.1...
#define MINIMUM_WINSOCK_VERSION 0x0001  // ...but we'll take ver 1.0
#define COMMAND_PORT           2074

// Timer constants
#define FRAME_TIMER_ID                  1
#define TIMEOUT_CONNECTION				30				// Remote connection timeout
#define TIMEOUT_AUDIO_OUTPUT			4				// Release audio output timeout
#define TIMEOUT_RESEND_LWL				(119)			// Resend Look Who's Listening listing
#define TIMEOUT_RESEND_SDES				10				// Reset RTP ID interval


#define LOOPBACK_ENABLED	1		// Enabled flag in localLoopback 
#define LOOPBACK_PLAYING	2		// Flag in localLoopback for play active

typedef INT	SOCKERR;        			// A socket error code

#define EXCHANGE_SAMPLE_RATE			8000		// Samples per second in data exchanged over the network
#define BUFL	1000							// Sound buffer maximum length (actually less)

extern LPSTR pszClientClass;					
extern int waProtUseLargerRTCPackets;

#define fCompGSM    32		      // GSM compression
#define fProtocol   0x40000000    // Speak Freely protocol flag

enum {CLIENTDATA,RTP,GSM};
enum {WANTSINPUT,TIMEOUT,STATE,SDESTIMER,LOCALLOOPBACK,OUTPUTSOCKETBUSY};

#define MAX_HOST                        256
#define GWL_CLIENT                      0               // Client-side data
#define CLIENTPTR(w)            ((LPCLIENT_DATA)GetWindowLong((w), GWL_CLIENT))
#define IS_LOCALHOST(i) (ntohl((long(i)) == 0x7F000001)

typedef WORD PORT;           			// A socket port number

