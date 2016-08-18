#if !defined(AFX_SERVERINFO)
#define AFX_SERVERINFO

namespace cs
{
	class CServerInfo
	{
	public:
		CComString sServerName;
		CComString sIP;
		CComString sBuild;
		short int tcpPort;
		short int udpSendPort;
		short int udpRecvPort;
		CServerInfo()
		{
			sServerName = "";
			sIP = "";
			tcpPort = 0;
			udpRecvPort = 0;
			udpSendPort = 0;
		}
	};
};

#endif