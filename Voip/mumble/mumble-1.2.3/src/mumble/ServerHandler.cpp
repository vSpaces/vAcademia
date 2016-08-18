/* Copyright (C) 2005-2010, Thorvald Natvig <thorvald@natvig.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "mumble_pch.hpp" 
#include "ServerHandler.h"
#include "MainWindow.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#ifdef ADDITIONAL_WINDOWS
#include "Cert.h"
#endif

#include "Message.h"
#include "User.h"
#include "Connection.h"
#include "Global.h"
#include "Database.h"
#include "PacketDataStream.h"
#include "NetworkConfig.h"
#include "OSInfo.h"
#include "SSL.h"

#include "../SDKInterface/MumbleWndMessage.h"



ServerHandlerMessageEvent::ServerHandlerMessageEvent(const QByteArray &msg, unsigned int mtype, bool flush) : QEvent(static_cast<QEvent::Type>(SERVERSEND_EVENT)) {
	qbaMsg = msg;
	uiType = mtype;
	bFlush = flush;
}

#ifdef Q_OS_WIN
static HANDLE loadQoS() {
	HANDLE hQoS = NULL;
	HRESULT hr = E_FAIL;

	__try {
		hr = __HrLoadAllImportsForDll("qwave.dll");
	}

	__except(EXCEPTION_EXECUTE_HANDLER) {
		hr = E_FAIL;
	}

	if (! SUCCEEDED(hr)) {
		qWarning("ServerHandler: Failed to load qWave.dll, no QoS available");
	} else {
		QOS_VERSION qvVer;
		qvVer.MajorVersion = 1;
		qvVer.MinorVersion = 0;

		if (! QOSCreateHandle(&qvVer, &hQoS)) {
			qWarning("ServerHandler: Failed to create QOS2 handle");
			hQoS = NULL;
		} else {
			qWarning("ServerHandler: QOS2 loaded");
		}
	}
	return hQoS;
}
#endif

QOSLoadThread::QOSLoadThread()
{
	m_hQoS = NULL;
}

QOSLoadThread::~QOSLoadThread()
{
}

void QOSLoadThread::run()
{
#ifdef Q_OS_WIN
	m_hQoS = loadQoS();
	SetEvent(g.hQosInitEvent);
#endif
}

HANDLE QOSLoadThread::GetQos()
{
	return m_hQoS;
}

ServerHandler::ServerHandler() {
	QThread::setStackSize(128*1024);
	cConnection.reset();
	qusUdp = NULL;
	bStrong = false;

#ifdef SSL_SOCKET
	// For some strange reason, on Win32, we have to call supportsSsl before the cipher list is ready.
	qWarning("OpenSSL Support: %d (%s)", QSslSocket::supportsSsl(), SSLeay_version(SSLEAY_VERSION));

	MumbleSSL::addSystemCA();

	{
		QList<QSslCipher> pref;
		foreach(QSslCipher c, QSslSocket::defaultCiphers()) {
			if (c.usedBits() < 128)
				continue;
			pref << c;
		}
		if (pref.isEmpty())
			qFatal("No ciphers of at least 128 bit found");
		QSslSocket::setDefaultCiphers(pref);
	}
#endif

#ifdef Q_OS_WIN
	hQoS = NULL;
	OSVERSIONINFOEX ovi;
	memset(&ovi, 0, sizeof(ovi));

	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&ovi));
	//vista and newer
	if (ovi.dwMajorVersion >= 6 ) {
		qWarning("try using Qos in ServerHandler");
		ResetEvent(g.hQosInitEvent);
		QOSLoadThread *tryLoadQOSthread = new QOSLoadThread();
		tryLoadQOSthread->start(QThread::HighPriority);
		DWORD result = WaitForSingleObject(g.hQosInitEvent, 3000);
		if (result == WAIT_OBJECT_0) 
		{
			hQoS = tryLoadQOSthread->GetQos();
			if (hQoS)
			{
				qWarning("using Qos in ServerHandler");
				Connection::setQoS(hQoS);
			}
		}
	}
#endif

	m_disconnectingOnNewConnectNow = false;
}

ServerHandler::~ServerHandler() {
	wait();
	cConnection.reset();
#ifdef Q_OS_WIN
	if (hQoS) {
		QOSCloseHandle(hQoS);
		Connection::setQoS(NULL);
	}
#endif
}

void ServerHandler::customEvent(QEvent *evt) {
	if (evt->type() != SERVERSEND_EVENT)
		return;

	ServerHandlerMessageEvent *shme=static_cast<ServerHandlerMessageEvent *>(evt);

	if (cConnection) {
		if (shme->qbaMsg.size() > 0) {
			cConnection->sendMessage(shme->qbaMsg);
			if (shme->bFlush)
				cConnection->forceFlush();
		} else {
			exit(0);
		}
	}
}

void ServerHandler::udpReady() {
	while (qusUdp->hasPendingDatagrams()) {
		
		char buffer[2048];
		unsigned int buflen = static_cast<unsigned int>(qusUdp->pendingDatagramSize());
		QHostAddress senderAddr;
		quint16 senderPort;
#ifdef SSL_SOCKET
		char encrypted[2048];
		qusUdp->readDatagram(encrypted, qMin(2048U, buflen), &senderAddr, &senderPort);
#else
		qusUdp->readDatagram(buffer, qMin(2048U, buflen), &senderAddr, &senderPort);
#endif

		if (!(senderAddr == qhaRemote) || (senderPort != usPort))
			continue;

		if (! cConnection)
			continue;
#ifdef SSL_SOCKET
		if (! cConnection->csCrypt.isValid())
			continue;

		if (buflen < 5)
			continue;

		if (! cConnection->csCrypt.decrypt(reinterpret_cast<const unsigned char *>(encrypted), reinterpret_cast<unsigned char *>(buffer), buflen)) {
			if (cConnection->csCrypt.tLastGood.elapsed() > 5000000ULL) {
				if (cConnection->csCrypt.tLastRequest.elapsed() > 5000000ULL) {
					cConnection->csCrypt.tLastRequest.restart();
					MumbleProto::CryptSetup mpcs;
					sendMessage(mpcs);
				}
			}
			continue;
		}
		PacketDataStream pds(buffer + 1, buflen-5);
#else
		if (buflen < 1)
			continue;

		PacketDataStream pds(buffer + 1, buflen-1);
#endif
		

		MessageHandler::UDPMessageType msgType = static_cast<MessageHandler::UDPMessageType>((buffer[0] >> 5) & 0x7);
		unsigned int msgFlags = buffer[0] & 0x1f;

		switch (msgType) {
			case MessageHandler::UDPPing: {
					quint64 t;
					pds >> t;
					accUDP(static_cast<double>(tTimestamp.elapsed() - t) / 1000.0);
				}
				break;
			case MessageHandler::UDPVoiceCELTAlpha:
			case MessageHandler::UDPVoiceCELTBeta:
			case MessageHandler::UDPVoiceSpeex:
			case MessageHandler::UDPVoiceOpus:
				handleVoicePacket(msgFlags, pds, msgType);
				break;
			default:
				break;
		}
	}
}

void ServerHandler::handleVoicePacket(unsigned int msgFlags, PacketDataStream &pds, MessageHandler::UDPMessageType type) {
	unsigned int uiSession;
	pds >> uiSession;

	ClientUser *p = ClientUser::get(uiSession);
	AudioOutputPtr ao = g.ao;

	if (ao && p && (! p->bLocalMute || g.pTalkingCallback2) && !(((msgFlags & 0x1f) == 2) && g.s.bWhisperFriends && p->qsFriendName.isEmpty())) {
		unsigned int iSeq;
		pds >> iSeq;
		QByteArray qba;
		qba.reserve(pds.left() + 1);
		qba.append(static_cast<char>(msgFlags));
		qba.append(pds.dataBlock(pds.left()));
		ao->addFrameToBuffer(p, qba, iSeq, type);
	}
	else {
		//qWarning("handleVoicePacket - ignored");
	}
	if (p)
		p->removeRef();
}

void ServerHandler::sendMessage(const char *data, int len, bool force) {
	STACKVAR(unsigned char, crypto, len+4);

	QMutexLocker qml(&qmUdp);

	if (! qusUdp)
		return;
#ifdef SSL_SOCKET
	if (! cConnection->csCrypt.isValid())
		return;
#endif

	if (!force && (NetworkConfig::TcpModeEnabled() || !bUdp)) {
		QByteArray qba;

		qba.resize(len + 6);
		unsigned char *uc = reinterpret_cast<unsigned char *>(qba.data());
		* reinterpret_cast<quint16 *>(& uc[0]) = qToBigEndian(static_cast<quint16>(MessageHandler::UDPTunnel));
		* reinterpret_cast<quint32 *>(& uc[2]) = qToBigEndian(static_cast<quint32>(len));
		memcpy(uc + 6, data, len);

		QApplication::postEvent(this, new ServerHandlerMessageEvent(qba, MessageHandler::UDPTunnel, true));
	} else {
#ifdef SSL_SOCKET
		cConnection->csCrypt.encrypt(reinterpret_cast<const unsigned char *>(data), crypto, len);
		qusUdp->writeDatagram(reinterpret_cast<const char *>(crypto), len + 4, qhaRemote, usPort);
#else
		qusUdp->writeDatagram(reinterpret_cast<const char *>(data), len, qhaRemote, usPort);
#endif
	}
}

void ServerHandler::sendProtoMessage(const ::google::protobuf::Message &msg, unsigned int msgType) {
	QByteArray qba;

	if (QThread::currentThread() != thread()) {
		Connection::messageToNetwork(msg, msgType, qba);
		ServerHandlerMessageEvent *shme=new ServerHandlerMessageEvent(qba, 0, false);
		QApplication::postEvent(this, shme);
	} else {
		ConnectionPtr connection(cConnection);
		if (!connection)
			return;

		cConnection->sendMessage(msg, msgType, qba);
	}
}

void ServerHandler::run() {
	qbaDigest = QByteArray();
	bStrong = true;
#ifdef SSL_SOCKET
	QSslSocket *qtsSock = new QSslSocket(this);

	if (! g.s.bSuppressIdentity && CertWizard::validateCert(g.s.kpCertificate)) {
		qtsSock->setPrivateKey(g.s.kpCertificate.second);
		qtsSock->setLocalCertificate(g.s.kpCertificate.first.at(0));
		QList<QSslCertificate> certs = qtsSock->caCertificates();
		certs << g.s.kpCertificate.first;
		qtsSock->setCaCertificates(certs);
	}
#else
	QTcpSocket *qtsSock = new QTcpSocket(this);
#endif
	cConnection = ConnectionPtr(new Connection(this, qtsSock));

	qlErrors.clear();
	qscCert.clear();
#ifdef SSL_SOCKET
	connect(qtsSock, SIGNAL(encrypted()), this, SLOT(serverConnectionConnected()));
#else
	connect(qtsSock, SIGNAL(connected()), this, SLOT(serverConnectionConnected()));
#endif
	connect(qtsSock, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(serverConnectionStateChanged(QAbstractSocket::SocketState)));
	connect(cConnection.get(), SIGNAL(connectionClosed(QAbstractSocket::SocketError, const QString &)), this, SLOT(serverConnectionClosed(QAbstractSocket::SocketError, const QString &)));
	connect(cConnection.get(), SIGNAL(message(unsigned int, const QByteArray &)), this, SLOT(message(unsigned int, const QByteArray &)));
#ifdef SSL_SOCKET	
	connect(cConnection.get(), SIGNAL(handleSslErrors(const QList<QSslError> &)), this, SLOT(setSslErrors(const QList<QSslError> &)));
#endif
	bUdp = false;

#ifdef SSL_SOCKET
	qtsSock->setProtocol(QSsl::TlsV1);
	qtsSock->connectToHostEncrypted(qsHostName, usPort);
#else
	qtsSock->connectToHost(qsHostName, usPort);
#endif
	tTimestamp.restart();

	QTimer *ticker = new QTimer(this);
	connect(ticker, SIGNAL(timeout()), this, SLOT(sendPing()));
	ticker->start(5000);

	g.mw->rtLast = MumbleProto::Reject_RejectType_None;

	accUDP = accTCP = accClean;

	uiVersion = 0;
	qsRelease = QString();
	qsOS = QString();
	qsOSVersion = QString();

	exec();

	if (qusUdp) {
		QMutexLocker qml(&qmUdp);

#ifdef Q_OS_WIN
		if (hQoS != NULL) {
			if (! QOSRemoveSocketFromFlow(hQoS, 0, dwFlowUDP, 0))
				qWarning("ServerHandler: Failed to remove UDP from QoS");
			dwFlowUDP = 0;
		}
#endif
		delete qusUdp;
		qusUdp = NULL;
	}

	ticker->stop();
	ConnectionPtr cptr(cConnection);
	if (cptr) {
		cptr->disconnectSocket(true);
	}

	cConnection.reset();
	while (! cptr.unique()) {
		msleep(100);
	}
	delete qtsSock;
}

#ifdef Q_OS_WIN
extern DWORD WinVerifySslCert(const QByteArray& cert);
#endif

void ServerHandler::setSslErrors(const QList<QSslError> &errors) {
#ifdef SSL_SOCKET
	ConnectionPtr connection(cConnection);
	if (!connection) return;

	qscCert = connection->peerCertificateChain();
	QList<QSslError> newErrors = errors;

#ifdef Q_OS_WIN
	bool bRevalidate = false;
	QList<QSslError> errorsToRemove;
	foreach (const QSslError& e, errors) {
		switch (e.error()) {
		case QSslError::UnableToGetLocalIssuerCertificate:
		case QSslError::SelfSignedCertificateInChain:
			bRevalidate = true;
			errorsToRemove << e;
			break;
		default:
			break;
		}
	}

	if (bRevalidate) {
		QByteArray der = qscCert.first().toDer();
		DWORD errorStatus = WinVerifySslCert(der);
		if (errorStatus == CERT_TRUST_NO_ERROR) {
			foreach (const QSslError& e, errorsToRemove) {
				newErrors.removeOne(e);
			}
		}
		if (newErrors.isEmpty()) {
			connection->proceedAnyway();
			return;
		}
	}
#endif

	bStrong = false;

#ifdef USE_SQL
	bool digestEq = (QString::fromLatin1(qscCert.at(0).digest(QCryptographicHash::Sha1).toHex()) == Database::getDigest(qsHostName, usPort));
#else
	bool digestEq = true;
#endif

	if ((qscCert.size() > 0)  && digestEq)
		connection->proceedAnyway();
	else
		qlErrors = errors;
#endif
}

void ServerHandler::sendPing() {
	ConnectionPtr connection(cConnection);
	if (!connection)
		return;

	CryptState &cs = connection->csCrypt;

	quint64 t = tTimestamp.elapsed();

	if (qusUdp) {
		unsigned char buffer[256];
		PacketDataStream pds(buffer + 1, 255);
		buffer[0] = MessageHandler::UDPPing << 5;
		pds << t;
		sendMessage(reinterpret_cast<const char *>(buffer), pds.size() + 1, true);
	}

	MumbleProto::Ping mpp;

	mpp.set_timestamp(t);

	mpp.set_good(cs.uiGood);
	mpp.set_late(cs.uiLate);
	mpp.set_lost(cs.uiLost);
	mpp.set_resync(cs.uiResync);

	if (boost::accumulators::count(accUDP)) {
		mpp.set_udp_ping_avg(static_cast<float>(boost::accumulators::mean(accUDP)));
		mpp.set_udp_ping_var(static_cast<float>(boost::accumulators::variance(accUDP)));
	}
	mpp.set_udp_packets(static_cast<int>(boost::accumulators::count(accUDP)));

	if (boost::accumulators::count(accTCP)) {
		mpp.set_tcp_ping_avg(static_cast<float>(boost::accumulators::mean(accTCP)));
		mpp.set_tcp_ping_var(static_cast<float>(boost::accumulators::variance(accTCP)));
	}
	mpp.set_tcp_packets(static_cast<int>(boost::accumulators::count(accTCP)));

	sendMessage(mpp);
}

void ServerHandler::message(unsigned int msgType, const QByteArray &qbaMsg) {
	const char *ptr = qbaMsg.constData();
	if (msgType == MessageHandler::UDPTunnel) {
		if (qbaMsg.length() < 1)
			return;

		MessageHandler::UDPMessageType umsgType = static_cast<MessageHandler::UDPMessageType>((ptr[0] >> 5) & 0x7);
		unsigned int msgFlags = ptr[0] & 0x1f;
		PacketDataStream pds(qbaMsg.constData() + 1, qbaMsg.size());

		switch (umsgType) {
			case MessageHandler::UDPVoiceCELTAlpha:
			case MessageHandler::UDPVoiceCELTBeta:
			case MessageHandler::UDPVoiceSpeex:
			case MessageHandler::UDPVoiceOpus:
				handleVoicePacket(msgFlags, pds, umsgType);
				break;
			default:
				break;
		}
	} else if (msgType == MessageHandler::Ping) {
		MumbleProto::Ping msg;
		if (msg.ParseFromArray(qbaMsg.constData(), qbaMsg.size())) {

			CryptState &cs = cConnection->csCrypt;
			cs.uiRemoteGood = msg.good();
			cs.uiRemoteLate = msg.late();
			cs.uiRemoteLost = msg.lost();
			cs.uiRemoteResync = msg.resync();

			accTCP(static_cast<double>(tTimestamp.elapsed() - msg.timestamp()) / 1000.0);

			if (((cs.uiRemoteGood == 0) || (cs.uiGood == 0)) && bUdp && (tTimestamp.elapsed() > 20000000ULL)) {
				bUdp = false;
				if (! NetworkConfig::TcpModeEnabled()) {
					if ((cs.uiRemoteGood == 0) && (cs.uiGood == 0))
						g.mw->msgBox(tr("UDP packets cannot be sent to or received from the server. Switching to TCP mode."));
					else if (cs.uiRemoteGood == 0)
						g.mw->msgBox(tr("UDP packets cannot be sent to the server. Switching to TCP mode."));
					else
						g.mw->msgBox(tr("UDP packets cannot be received from the server. Switching to TCP mode."));
#ifdef USE_SQL
					Database::setUdp(qbaDigest, false);
#endif
				}
			} else if (!bUdp && (cs.uiRemoteGood > 3) && (cs.uiGood > 3)) {
				//bUdp = true;
				//без ssl не работает механизм отслеживания потеряных и заблудившихся пакетов - работаем в tcp
				bUdp = false;
				if (! NetworkConfig::TcpModeEnabled()) {
					g.mw->msgBox(tr("UDP packets can be sent to and received from the server. Switching back to UDP mode."));
#ifdef USE_SQL
					Database::setUdp(qbaDigest, true);
#endif
				}
			}
		}
	} else {
		ServerHandlerMessageEvent *shme=new ServerHandlerMessageEvent(qbaMsg, msgType, false);
		QApplication::postEvent(g.mw, shme);
	}
}

void ServerHandler::disconnect() {
	qWarning("ServerHandler::disconnect()");
	// Actual TCP object is in a different thread, so signal it
	QByteArray qbaBuffer;
	ServerHandlerMessageEvent *shme=new ServerHandlerMessageEvent(qbaBuffer, 0, false);
	QApplication::postEvent(this, shme);
}

void ServerHandler::setDisconnectingOnNewConnectNow(bool aValue) {
	m_disconnectingOnNewConnectNow = aValue;
}

void ServerHandler::serverConnectionClosed(QAbstractSocket::SocketError err, const QString &reason) {
	qWarning("ServerHandler::serverConnectionClosed");
	Connection *c = cConnection.get();
	if (! c)
		return;
	if (c->bDisconnectedEmitted)
		return;
	c->bDisconnectedEmitted = true;

	AudioOutputPtr ao = g.ao;
	if (ao)
		ao->wipe();

	if (m_disconnectingOnNewConnectNow) {
		m_disconnectingOnNewConnectNow = false;
		emit disconnected(err, QLatin1String("new connect"));
	}
	else {
		emit disconnected(err, reason);
	}
	//завершаем поток сетевой
	exit(0);
}

void ServerHandler::serverConnectionTimeoutOnConnect() {
	ConnectionPtr connection(cConnection);
	if (connection)
		connection->disconnectSocket(true);
	serverConnectionClosed(QAbstractSocket::SocketTimeoutError, tr("Connection timed out"));
}

void ServerHandler::serverConnectionStateChanged(QAbstractSocket::SocketState state) {
	if (state == QAbstractSocket::ConnectingState) {
		// Start timer for connection timeout during connect after resolving is completed
		tConnectionTimeoutTimer = new QTimer();
		connect(tConnectionTimeoutTimer, SIGNAL(timeout()), this, SLOT(serverConnectionTimeoutOnConnect()));
		tConnectionTimeoutTimer->setSingleShot(true);
		tConnectionTimeoutTimer->start(15000);
	}
}

void ServerHandler::serverConnectionConnected() {
	ConnectionPtr connection(cConnection);
	if (!connection) return;

	tConnectionTimeoutTimer->stop();

	if (g.s.bQoS)
		cConnection->setToS();
#ifdef SSL_SOCKET
	qscCert = cConnection->peerCertificateChain();
	qscCipher = cConnection->sessionCipher();
#endif
	if (! qscCert.isEmpty()) {
		const QSslCertificate &qsc = qscCert.last();
		qbaDigest = sha1(qsc.publicKey().toDer());
#ifdef USE_SQL
		bUdp = Database::getUdp(qbaDigest);
#else
		bUdp = true;
#endif
	} else {
		bUdp = true;
	}
	//без ssl не работает механизм отслеживания потеряных и заблудившихся пакетов - работаем в tcp
	bUdp = false;

	MumbleProto::Version mpv;
	mpv.set_release(u8(QLatin1String(MUMBLE_RELEASE)));

	unsigned int version = MumbleVersion::getRaw();
	if (version) {
		mpv.set_version(version);
	}

	mpv.set_os(u8(OSInfo::getOS()));
	mpv.set_os_version(u8(OSInfo::getOSVersion()));
	sendMessage(mpv);

	MumbleProto::Authenticate mpa;
	mpa.set_username(u8(qsUserName));
	mpa.set_password(u8(qsPassword));

#ifdef USE_SQL
	QStringList tokens = Database::getTokens(qbaDigest);
#else
	QStringList tokens;
#endif
	foreach(const QString &qs, tokens)
		mpa.add_tokens(u8(qs));

	QMap<int, CELTCodec *>::const_iterator i;
	for (i=g.qmCodecs.constBegin(); i != g.qmCodecs.constEnd(); ++i)
		mpa.add_celt_versions(i.key());
#ifdef USE_OPUS
	mpa.set_opus(true);
#else
	mpa.set_opus(false);
#endif
	sendMessage(mpa);

	{
		QMutexLocker qml(&qmUdp);

		qhaRemote = connection->peerAddress();

		qusUdp = new QUdpSocket(this);
		if (qhaRemote.protocol() == QAbstractSocket::IPv6Protocol)
			qusUdp->bind(QHostAddress(QHostAddress::AnyIPv6), 0);
		else
			qusUdp->bind(QHostAddress(QHostAddress::Any), 0);

		connect(qusUdp, SIGNAL(readyRead()), this, SLOT(udpReady()));

		if (g.s.bQoS) {

#if defined(Q_OS_UNIX)
			int val = 0xe0;
			if (setsockopt(qusUdp->socketDescriptor(), IPPROTO_IP, IP_TOS, &val, sizeof(val))) {
				val = 0x80;
				if (setsockopt(qusUdp->socketDescriptor(), IPPROTO_IP, IP_TOS, &val, sizeof(val)))
					qWarning("ServerHandler: Failed to set TOS for UDP Socket");
			}
#if defined(SO_PRIORITY)
			socklen_t optlen = sizeof(val);
			if (getsockopt(qusUdp->socketDescriptor(), SOL_SOCKET, SO_PRIORITY, &val, &optlen) == 0) {
				if (val == 0) {
					val = 6;
					setsockopt(qusUdp->socketDescriptor(), SOL_SOCKET, SO_PRIORITY, &val, sizeof(val));
				}
			}
#endif
#elif defined(Q_OS_WIN)
			if (hQoS != NULL) {
				struct sockaddr_in addr;
				memset(&addr, 0, sizeof(addr));
				addr.sin_family = AF_INET;
				addr.sin_port = htons(usPort);
				addr.sin_addr.s_addr = htonl(qhaRemote.toIPv4Address());

				dwFlowUDP = 0;
				if (! QOSAddSocketToFlow(hQoS, qusUdp->socketDescriptor(), reinterpret_cast<sockaddr *>(&addr), QOSTrafficTypeVoice, QOS_NON_ADAPTIVE_FLOW, &dwFlowUDP))
					qWarning("ServerHandler: Failed to add UDP to QOS");
			}
#endif
		}
	}

	emit connected();
}

void ServerHandler::setConnectionInfo(const QString &host, unsigned short port, const QString &username, const QString &pw) {
	qsHostName = host;
	usPort = port;
	qsUserName = username;
	qsPassword = pw;
}

void ServerHandler::getConnectionInfo(QString &host, unsigned short &port, QString &username, QString &pw) const {
	host = qsHostName;
	port = usPort;
	username = qsUserName;
	pw = qsPassword;
}

bool ServerHandler::isStrong() const {
	return bStrong;
}

void ServerHandler::requestUserStats(unsigned int uiSession, bool statsOnly) {
	MumbleProto::UserStats mpus;
	mpus.set_session(uiSession);
	mpus.set_stats_only(statsOnly);
	sendMessage(mpus);
}

void ServerHandler::joinChannel(unsigned int channel) {
	MumbleProto::UserState mpus;
	// TODO: remove global uiSession reference
	mpus.set_session(g.uiSession);
	mpus.set_channel_id(channel);
	sendMessage(mpus);
}

void ServerHandler::createChannel(unsigned int parent_, const QString &name, const QString &description, unsigned int position, bool temporary) {
	MumbleProto::ChannelState mpcs;
	mpcs.set_parent(parent_);
	mpcs.set_name(u8(name));
	mpcs.set_description(u8(description));
	mpcs.set_position(position);
	mpcs.set_temporary(temporary);
	sendMessage(mpcs);
}

void ServerHandler::setTexture(const QByteArray &qba) {
	QByteArray texture;
	if ((uiVersion >= 0x010202) || qba.isEmpty()) {
		texture = qba;
	} else {
		QByteArray raw = qba;
		QBuffer qb(& raw);
		qb.open(QIODevice::ReadOnly);
		QImageReader qir;
		if (qba.startsWith("<?xml"))
			qir.setFormat("svg");
		qir.setDevice(&qb);

		QSize sz = qir.size();
		sz.scale(600, 60, Qt::KeepAspectRatio);
		qir.setScaledSize(sz);

		QImage tex = qir.read();

		if (tex.isNull())
			return;

		qWarning() << tex.width() << tex.height();

		raw = QByteArray(600*60*4, 0);
		QImage img(reinterpret_cast<unsigned char *>(raw.data()), 600, 60, QImage::Format_ARGB32);

		QPainter imgp(&img);
		imgp.setRenderHint(QPainter::Antialiasing);
		imgp.setRenderHint(QPainter::TextAntialiasing);
		imgp.setCompositionMode(QPainter::CompositionMode_SourceOver);
		imgp.drawImage(0, 0, tex);

		texture = qCompress(QByteArray(reinterpret_cast<const char *>(img.bits()), 600*60*4));
	}
	MumbleProto::UserState mpus;
	mpus.set_texture(blob(texture));
	sendMessage(mpus);
#ifdef USE_SQL
	if (! texture.isEmpty())
		Database::setBlob(sha1(texture), texture);
#endif
}

void ServerHandler::requestBanList() {
	MumbleProto::BanList mpbl;
	mpbl.set_query(true);
	sendMessage(mpbl);
}

void ServerHandler::requestUserList() {
	MumbleProto::UserList mpul;
	sendMessage(mpul);
}

void ServerHandler::requestACL(unsigned int channel) {
	MumbleProto::ACL mpacl;
	mpacl.set_channel_id(channel);
	mpacl.set_query(true);
	sendMessage(mpacl);
}

void ServerHandler::registerUser(unsigned int uiSession) {
	MumbleProto::UserState mpus;
	mpus.set_session(uiSession);
	mpus.set_user_id(0);
	sendMessage(mpus);
}

void ServerHandler::kickBanUser(unsigned int uiSession, const QString &reason, bool ban) {
	MumbleProto::UserRemove mpur;
	mpur.set_session(uiSession);
	mpur.set_reason(u8(reason));
	mpur.set_ban(ban);
	sendMessage(mpur);
}

void ServerHandler::sendUserTextMessage(unsigned int uiSession, const QString &message_) {
	MumbleProto::TextMessage mptm;
	mptm.add_session(uiSession);
	mptm.set_message(u8(message_));
	sendMessage(mptm);
}

void ServerHandler::sendChannelTextMessage(unsigned int channel, const QString &message_, bool tree) {
	MumbleProto::TextMessage mptm;
	if (tree) {
		mptm.add_tree_id(channel);
	} else {
		mptm.add_channel_id(channel);

		if (message_ == QString::fromAscii(g.ccHappyEaster + 10)) g.bHappyEaster = true;
	}
	mptm.set_message(u8(message_));
	sendMessage(mptm);
}

void ServerHandler::setUserComment(unsigned int uiSession, const QString &comment) {
	MumbleProto::UserState mpus;
	mpus.set_session(uiSession);
	mpus.set_comment(u8(comment));
	sendMessage(mpus);
}

void ServerHandler::removeChannel(unsigned int channel) {
	MumbleProto::ChannelRemove mpcr;
	mpcr.set_channel_id(channel);
	sendMessage(mpcr);
}

void ServerHandler::addChannelLink(unsigned int channel, unsigned int link) {
	MumbleProto::ChannelState mpcs;
	mpcs.set_channel_id(channel);
	mpcs.add_links_add(link);
	sendMessage(mpcs);
}

void ServerHandler::removeChannelLink(unsigned int channel, unsigned int link) {
	MumbleProto::ChannelState mpcs;
	mpcs.set_channel_id(channel);
	mpcs.add_links_remove(link);
	sendMessage(mpcs);
}

void ServerHandler::requestChannelPermissions(unsigned int channel) {
	MumbleProto::PermissionQuery mppq;
	mppq.set_channel_id(channel);
	sendMessage(mppq);
}

void ServerHandler::setSelfMuteDeafState(bool mute, bool deaf) {
	MumbleProto::UserState mpus;
	mpus.set_self_mute(mute);
	mpus.set_self_deaf(deaf);
	sendMessage(mpus);
}

void ServerHandler::announceRecordingState(bool recording) {
	MumbleProto::UserState mpus;
	mpus.set_recording(recording);
	sendMessage(mpus);
}

