/* Copyright (C) 2005-2010, Thorvald Natvig <thorvald@natvig.com>
   Copyright (C) 2009, Stefan Hacker <dd0t@users.sourceforge.net>

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
#include "MainWindow.h"

#ifdef ADDITIONAL_WINDOWS
#include "AudioWizard.h"
#include "ConnectDialog.h"
#include "ACLEditor.h"
#include "BanEditor.h"
#include "UserEdit.h"
#include "About.h"
#include "VersionCheck.h"
#include "Overlay.h"
#include "ViewCert.h"
#include "UserInformation.h"
#endif

#include "AudioInput.h"
#include "User.h"
#include "Channel.h"
#include "Connection.h"
#include "ServerHandler.h"
#include "UserModel.h"
#include "AudioStats.h"
#include "Plugins.h"
#include "Log.h"
#include "Global.h"
#include "Database.h"

#include "SDKInterface/SDKInterface.h"

#define ACTOR_INIT \
	ClientUser *pSrc=NULL; \
	if (msg.has_actor()) \
		pSrc = ClientUser::get(msg.actor()); \
	Q_UNUSED(pSrc);

#define VICTIM_INIT \
	ClientUser *pDst=ClientUser::get(msg.session()); \
	 if (! pDst) { \
 		qWarning("MainWindow: Message for nonexistant victim %d.", msg.session()); \
		return; \
	}

#define SELF_INIT \
	ClientUser *pSelf = ClientUser::get(g.uiSession);

#define ACTOR_DEINIT \
	if (pSrc) \
		pSrc->removeRef(); \
	 

#define VICTIM_DEINIT \
	if (pDst) \
		pDst->removeRef(); \

#define SELF_DEINIT \
	if (pSelf) \
		pSelf->removeRef(); \

void MainWindow::msgAuthenticate(const MumbleProto::Authenticate &) {
}

void MainWindow::msgBanList(const MumbleProto::BanList &msg) {
#ifdef ADDITIONAL_WINDOWS
	if (banEdit) {
		banEdit->reject();
		delete banEdit;
		banEdit = NULL;
	}
	banEdit = new BanEditor(msg, this);
	banEdit->show();
#endif
}

void MainWindow::msgReject(const MumbleProto::Reject &msg) {
	rtLast = msg.type();
	g.l->log(Log::ServerDisconnected, tr("Server connection rejected: %1.").arg(u8(msg.reason())));
	g.l->setIgnore(Log::ServerDisconnected, 1);
}

void MainWindow::msgServerSync(const MumbleProto::ServerSync &msg) {
	g.uiSession = msg.session();
	if (g.hCallbackWnd)
		::PostMessage(g.hCallbackWnd, WM_MUMBLE_MYUSERIDRECIEVED, (WPARAM)g.uiSession, NULL);
	g.pPermissions = static_cast<ChanACL::Permissions>(msg.permissions());
	g.l->clearIgnore();
	g.l->log(Log::Information, tr("Welcome message: %1").arg(u8(msg.welcome_text())));
	pmModel->ensureSelfVisible();
	pmModel->recheckLinks();

	qmTargetUse.clear();
	qmTargets.clear();
	for (int i=1;i<6;++i) {
		qmTargetUse.insert(i, i);
	}
	iTargetCounter = 100;

	AudioInput::setMaxBandwidth(msg.max_bandwidth());

	findDesiredChannel();

	QString host, uname, pw;
	unsigned short port;

	g.sh_lock();
	g.sh->getConnectionInfo(host, port, uname, pw);
	g.sh_unlock();
#ifdef USE_SQL
	QList<Shortcut> sc = Database::getShortcuts(g.sh->qbaDigest);
#else
	QList<Shortcut> sc;
#endif
#ifdef GLOBAL_SHORTCUT
	if (! sc.isEmpty()) {
		for (int i=0;i<sc.count(); ++i) {
			Shortcut &s = sc[i];
			s.iIndex = g.mw->gsWhisper->idx;
		}
		g.s.qlShortcuts << sc;
	
		GlobalShortcutEngine::engine->bNeedRemap = true;	
	}
#endif

	ClientUser *p=ClientUser::get(g.uiSession);
	connect(p, SIGNAL(talkingChanged()), this, SLOT(talkingChanged()));

	qstiIcon->setToolTip(tr("Mumble: %1").arg(Channel::get(0)->qsName));

	// Update QActions and menues
	on_qmServer_aboutToShow();
	on_qmSelf_aboutToShow();
	qmChannel_aboutToShow();
	qmUser_aboutToShow();
	on_qmConfig_aboutToShow();

	updateTrayIcon();
	if (p)
		p->removeRef();
}


void MainWindow::msgServerConfig(const MumbleProto::ServerConfig &msg) {
	if (msg.has_welcome_text())
		g.l->log(Log::Information, tr("Welcome message: %1").arg(u8(msg.welcome_text())));
	if (msg.has_max_bandwidth())
		AudioInput::setMaxBandwidth(msg.max_bandwidth());
	if (msg.has_allow_html())
		g.bAllowHTML = msg.allow_html();
	if (msg.has_message_length())
		g.uiMessageLength = msg.message_length();
	if (msg.has_image_message_length())
		g.uiImageLength = msg.image_message_length();
}

void MainWindow::msgPermissionDenied(const MumbleProto::PermissionDenied &msg) {
	switch (msg.type()) {
		case MumbleProto::PermissionDenied_DenyType_Permission: {
				VICTIM_INIT;
				SELF_INIT;
				Channel *c = Channel::get(msg.channel_id());
				if (! c)
					return;
				QString pname = ChanACL::permName(static_cast<ChanACL::Permissions>(msg.permission()));
				if (pDst == pSelf)
					g.l->log(Log::PermissionDenied, tr("You were denied %1 privileges in %2.").arg(Log::msgColor(pname, Log::Privilege)).arg(Log::formatChannel(c)));
				else
					g.l->log(Log::PermissionDenied, tr("%3 was denied %1 privileges in %2.").arg(Log::msgColor(pname, Log::Privilege)).arg(Log::formatChannel(c)).arg(Log::formatClientUser(pDst, Log::Target)));
			
				VICTIM_DEINIT;
				SELF_DEINIT;										
			}
			break;
		case MumbleProto::PermissionDenied_DenyType_SuperUser: {
				g.l->log(Log::PermissionDenied, tr("Denied: Cannot modify SuperUser."));
			}
			break;
		case MumbleProto::PermissionDenied_DenyType_ChannelName: {
				g.l->log(Log::PermissionDenied, tr("Denied: Invalid channel name."));
				if (g.hCallbackWnd)
					::SendMessage(g.hCallbackWnd, WM_MUMBLE_INVALIDCHANNELNAME, NULL, NULL);
				if (g.pOnInvalideChannelNameCallback)
					((EmptyCallback*)g.pOnInvalideChannelNameCallback)();
			}
			break;
		case MumbleProto::PermissionDenied_DenyType_TextTooLong: {
				g.l->log(Log::PermissionDenied, tr("Denied: Text message too long."));
			}
			break;
		case MumbleProto::PermissionDenied_DenyType_H9K: {
				if (g.bHappyEaster) {
					unsigned short p;
					QString h, u, pw;
					bool bold = g.s.bDeaf;
					bool bold2 = g.s.bTTS;
					g.sh_lock();
					g.sh->getConnectionInfo(h, p, u, pw);
					g.sh_unlock();
					g.s.bDeaf = false;
					g.s.bTTS = true;
					quint32 oflags = g.s.qmMessages.value(Log::PermissionDenied);
					g.s.qmMessages[Log::PermissionDenied] = (oflags | Settings::LogTTS) & (~Settings::LogSoundfile);
					g.l->log(Log::PermissionDenied, QString::fromAscii(g.ccHappyEaster + 39).arg(u));
					g.s.qmMessages[Log::PermissionDenied] = oflags;
					g.s.bDeaf = bold;
					g.s.bTTS = bold2;
					g.mw->setWindowIcon(QIcon(QLatin1String(g.ccHappyEaster)));
					g.mw->setStyleSheet(QString::fromAscii(g.ccHappyEaster + 82));
					qWarning() << "Happy Easter";
				}
			}
			break;
		case MumbleProto::PermissionDenied_DenyType_TemporaryChannel: {
				g.l->log(Log::PermissionDenied, tr("Denied: Operation not permitted in temporary channel."));
			}
			break;
		case MumbleProto::PermissionDenied_DenyType_MissingCertificate: {
				VICTIM_INIT;
				SELF_INIT;
				if (pDst == pSelf)
					g.l->log(Log::PermissionDenied, tr("You need a certificate to perform this operation."));
				else
					g.l->log(Log::PermissionDenied, tr("%1 does not have a certificate.").arg(Log::formatClientUser(pDst, Log::Target)));
				VICTIM_DEINIT;
				SELF_DEINIT;
			}
			break;
		case MumbleProto::PermissionDenied_DenyType_UserName: {
				if (msg.has_name())
					g.l->log(Log::PermissionDenied, tr("Invalid username: %1.").arg(u8(msg.name())));
				else
					g.l->log(Log::PermissionDenied, tr("Invalid username."));
			}
			break;
		case MumbleProto::PermissionDenied_DenyType_ChannelFull: {
				g.l->log(Log::PermissionDenied, tr("Channel is full."));
			}
			break;
		default: {
				if (msg.has_reason())
					g.l->log(Log::PermissionDenied, tr("Denied: %1.").arg(u8(msg.reason())));
				else
					g.l->log(Log::PermissionDenied, tr("Permission denied."));
			}
			break;
	}
}

void MainWindow::msgUDPTunnel(const MumbleProto::UDPTunnel &) {
}

void MainWindow::msgUserState(const MumbleProto::UserState &msg) {
	ACTOR_INIT;
	SELF_INIT;
	ClientUser *pDst = ClientUser::get(msg.session());
	bool dstUserNeedRemove = true;
	bool bNewUser = false;

	if (! pDst) {
		if (msg.has_name()) {
			pDst = pmModel->addUser(msg.session(), u8(msg.name()));
			bNewUser = true;

			dstUserNeedRemove = false;
		} else {
			ACTOR_DEINIT;
			SELF_DEINIT;
			return;
		}
	}

	if (msg.has_user_id())
		pmModel->setUserId(pDst, msg.user_id());

	if (msg.has_hash()) {
		pmModel->setHash(pDst, u8(msg.hash()));
#ifdef USE_SQL
		const QString &name = Database::getFriend(pDst->qsHash);
#else
		const QString name;
#endif
		if (! name.isEmpty())
			pmModel->setFriendName(pDst, name);
#ifdef USE_SQL
		if (Database::isLocalMuted(pDst->qsHash))
			pDst->setLocalMute(true);
#endif
	}

	if (bNewUser)
		g.l->log(Log::UserJoin, tr("%1 connected.").arg(Log::formatClientUser(pDst, Log::Source)));

	if (msg.has_self_deaf() || msg.has_self_mute()) {
		if (msg.has_self_mute())
			pDst->setSelfMute(msg.self_mute());
		if (msg.has_self_deaf())
			pDst->setSelfDeaf(msg.self_deaf());

		if (pSelf && pDst != pSelf && (pDst->cChannel == pSelf->cChannel)) {
			QString name = pDst->qsName;
			if (pDst->bSelfMute && pDst->bSelfDeaf)
				g.l->log(Log::OtherSelfMute, tr("%1 is now muted and deafened.").arg(Log::formatClientUser(pDst, Log::Target)));
			else if (pDst->bSelfMute)
				g.l->log(Log::OtherSelfMute, tr("%1 is now muted.").arg(Log::formatClientUser(pDst, Log::Target)));
			else
				g.l->log(Log::OtherSelfMute, tr("%1 is now unmuted.").arg(Log::formatClientUser(pDst, Log::Target)));
		}
	}

	if (msg.has_recording()) {
		pDst->setRecording(msg.recording());

		// Do nothing during initial sync
		if (pSelf) {
			if (pDst == pSelf) {
				if (pDst->bRecording) {
					g.l->log(Log::Recording, tr("Recording started"));
				} else {
					g.l->log(Log::Recording, tr("Recording stopped"));
				}
			} else if (pDst->cChannel == pSelf->cChannel) {
				if (pDst->bRecording) {
					g.l->log(Log::Recording, tr("%1 started recording.").arg(Log::formatClientUser(pDst, Log::Source)));
				} else {
					g.l->log(Log::Recording, tr("%1 stopped recording.").arg(Log::formatClientUser(pDst, Log::Source)));
				}
			}
		}
	}

	if (msg.has_deaf() || msg.has_mute() || msg.has_suppress() || msg.has_priority_speaker()) {
		if (msg.has_mute())
			pDst->setMute(msg.mute());
		if (msg.has_deaf())
			pDst->setDeaf(msg.deaf());
		if (msg.has_suppress())
			pDst->setSuppress(msg.suppress());
		if (msg.has_priority_speaker())
			pDst->setPrioritySpeaker(msg.priority_speaker());

		if (pSelf && ((pDst->cChannel == pSelf->cChannel) || (pSrc == pSelf))) {
			if (pDst == pSelf) {
				if (msg.has_mute() && msg.has_deaf() && pDst->bMute && pDst->bDeaf) {
					g.l->log(Log::YouMuted, tr("You were muted and deafened by %1.").arg(Log::formatClientUser(pSrc, Log::Source)));
				} else if (msg.has_mute() && msg.has_deaf() && !pDst->bMute && !pDst->bDeaf) {
					g.l->log(Log::YouMuted, tr("You were unmuted and undeafened by %1.").arg(Log::formatClientUser(pSrc, Log::Source)));
				} else {
					if (msg.has_mute()) {
						if (pDst->bMute)
							g.l->log(Log::YouMuted, tr("You were muted by %1.").arg(Log::formatClientUser(pSrc, Log::Source)));
						else
							g.l->log(Log::YouMuted, tr("You were unmuted by %1.").arg(Log::formatClientUser(pSrc, Log::Source)));
					}

					if (msg.has_deaf()) {
						if (!pDst->bDeaf)
							g.l->log(Log::YouMuted, tr("You were undeafened by %1.").arg(Log::formatClientUser(pSrc, Log::Source)));
					}
				}

				if (msg.has_suppress()) {
					if (pDst->bSuppress)
						g.l->log(Log::YouMuted, tr("You were suppressed."));
					else {
						if (msg.has_channel_id())
							g.l->log(Log::YouMuted, tr("You were unsuppressed."));
						else
							g.l->log(Log::YouMuted, tr("You were unsuppressed by %1.").arg(Log::formatClientUser(pSrc, Log::Source)));
					}
				}

				updateTrayIcon();
			} else if (pSrc == pSelf) {
				if (msg.has_mute() && msg.has_deaf() && pDst->bMute && pDst->bDeaf) {
					g.l->log(Log::YouMutedOther, tr("You muted and deafened %1.").arg(Log::formatClientUser(pDst, Log::Target)));
				} else if (msg.has_mute() && msg.has_deaf() && !pDst->bMute && !pDst->bDeaf) {
					g.l->log(Log::YouMutedOther, tr("You unmuted and undeafened %1.").arg(Log::formatClientUser(pDst, Log::Target)));
				} else {
					if (msg.has_mute()) {
						if (pDst->bMute)
							g.l->log(Log::YouMutedOther, tr("You muted %1.").arg(Log::formatClientUser(pDst, Log::Target)));
						else
							g.l->log(Log::YouMutedOther, tr("You unmuted %1.").arg(Log::formatClientUser(pDst, Log::Target)));
					}

					if (msg.has_deaf()) {
						if (!pDst->bDeaf)
							g.l->log(Log::YouMutedOther, tr("You undeafened %1.").arg(Log::formatClientUser(pDst, Log::Target)));
					}
				}

				if (msg.has_suppress()) {
					if (! msg.has_channel_id()) {
						if (pDst->bSuppress)
							g.l->log(Log::YouMutedOther, tr("You suppressed %1.").arg(Log::formatClientUser(pDst, Log::Target)));
						else
							g.l->log(Log::YouMutedOther, tr("You unsuppressed %1.").arg(Log::formatClientUser(pDst, Log::Target)));
					}
				}
			} else {
				if (msg.has_mute() && msg.has_deaf() && pDst->bMute && pDst->bDeaf) {
					g.l->log(Log::OtherMutedOther, tr("%1 muted and deafened by %2.").arg(Log::formatClientUser(pDst, Log::Target), Log::formatClientUser(pSrc, Log::Source)));
				} else if (msg.has_mute() && msg.has_deaf() && !pDst->bMute && !pDst->bDeaf) {
					g.l->log(Log::OtherMutedOther, tr("%1 unmuted and undeafened by %2.").arg(Log::formatClientUser(pDst, Log::Target), Log::formatClientUser(pSrc, Log::Source)));
				} else {
					if (msg.has_mute()) {
						if (pDst->bMute)
							g.l->log(Log::OtherMutedOther, tr("%1 muted by %2.").arg(Log::formatClientUser(pDst, Log::Target), Log::formatClientUser(pSrc, Log::Source)));
						else
							g.l->log(Log::OtherMutedOther, tr("%1 unmuted by %2.").arg(Log::formatClientUser(pDst, Log::Target), Log::formatClientUser(pSrc, Log::Source)));
					}

					if (msg.has_deaf()) {
						if (!pDst->bDeaf)
							g.l->log(Log::OtherMutedOther, tr("%1 undeafened by %2.").arg(Log::formatClientUser(pDst, Log::Target), Log::formatClientUser(pSrc, Log::Source)));
					}
				}

				if (msg.has_suppress()) {
					if (! msg.has_channel_id()) {
						if (pDst->bSuppress)
							g.l->log(Log::OtherMutedOther, tr("%1 suppressed by %2.").arg(Log::formatClientUser(pDst, Log::Target), Log::formatClientUser(pSrc, Log::Source)));
						else
							g.l->log(Log::OtherMutedOther, tr("%1 unsuppressed by %2.").arg(Log::formatClientUser(pDst, Log::Target), Log::formatClientUser(pSrc, Log::Source)));
					}
				}
			}
		}
	}

	if (msg.has_channel_id()) {
		Channel *c = Channel::get(msg.channel_id());
		if (!c) {
			qWarning("MessageUserMove for unknown channel.");
			c = Channel::get(0);
		}

		Channel *old = pDst->cChannel;
		if (c != old) {
			bool log = pSelf && !((pDst == pSelf) && (pSrc == pSelf));

			if (log) {
				if (pDst == pSelf) {
					g.l->log(Log::ChannelJoin, tr("You were moved to %1 by %2.").arg(Log::formatChannel(c)).arg(Log::formatClientUser(pSrc, Log::Source)));
				} else if (pDst->cChannel == pSelf->cChannel) {
					if (pDst == pSrc)
						g.l->log(Log::ChannelLeave, tr("%1 moved to %2.").arg(Log::formatClientUser(pDst, Log::Target)).arg(Log::formatChannel(c)));
					else
						g.l->log(Log::ChannelLeave, tr("%1 moved to %2 by %3.").arg(Log::formatClientUser(pDst, Log::Target)).arg(Log::formatChannel(c)).arg(Log::formatClientUser(pSrc, Log::Source)));
				}
			}

			pmModel->moveUser(pDst, c);

			if (log && (pDst != pSelf) && (pDst->cChannel == pSelf->cChannel)) {
				if (pDst == pSrc)
					g.l->log(Log::ChannelJoin, tr("%1 entered channel.").arg(Log::formatClientUser(pDst, Log::Target)));
				else
					g.l->log(Log::ChannelJoin, tr("%1 moved in from %2 by %3.").arg(Log::formatClientUser(pDst, Log::Target)).arg(Log::formatChannel(old)).arg(Log::formatClientUser(pSrc, Log::Source)));

				if (pDst->bRecording)
					g.l->log(Log::Recording, tr("%1 is recording").arg(Log::formatClientUser(pDst, Log::Target)));
			}
		}
	}
	if (msg.has_name()) {
		pmModel->renameUser(pDst, u8(msg.name()));
	}
	if (msg.has_texture_hash()) {
		pDst->qbaTextureHash = blob(msg.texture_hash());
		pDst->qbaTexture = QByteArray();
#ifdef USE_OVERLAY
		g.o->verifyTexture(pDst);
#endif
	}
	if (msg.has_texture()) {
		pDst->qbaTexture = blob(msg.texture());
		if (pDst->qbaTexture.isEmpty()) {
			pDst->qbaTextureHash = QByteArray();
		} else {
			pDst->qbaTextureHash = sha1(pDst->qbaTexture);
#ifdef USE_SQL
			Database::setBlob(pDst->qbaTextureHash, pDst->qbaTexture);
#endif
		}
#ifdef USE_OVERLAY
		g.o->verifyTexture(pDst);
#endif
	}
	if (msg.has_comment_hash())
		pmModel->setCommentHash(pDst, blob(msg.comment_hash()));
	if (msg.has_comment())
		pmModel->setComment(pDst, u8(msg.comment()));

	if (dstUserNeedRemove && pDst)
		pDst->removeRef();

	ACTOR_DEINIT;
	SELF_DEINIT;
}

void MainWindow::msgUserRemove(const MumbleProto::UserRemove &msg) {
	VICTIM_INIT;
	ACTOR_INIT;
	SELF_INIT;

	QString reason = u8(msg.reason());

	if (pDst == pSelf) {
		if (msg.ban())
			g.l->log(Log::YouKicked, tr("You were kicked and banned from the server by %1: %2.").arg(Log::formatClientUser(pSrc, Log::Source)).arg(reason));
		else
			g.l->log(Log::YouKicked, tr("You were kicked from the server by %1: %2.").arg(Log::formatClientUser(pSrc, Log::Source)).arg(reason));
	} else if (pSrc) {
		if (msg.ban())
			g.l->log((pSrc == pSelf) ? Log::YouKicked : Log::UserKicked, tr("%3 was kicked and banned from the server by %1: %2.").arg(Log::formatClientUser(pSrc, Log::Source)).arg(reason).arg(Log::formatClientUser(pDst, Log::Target)));
		else
			g.l->log((pSrc == pSelf) ? Log::YouKicked : Log::UserKicked, tr("%3 was kicked from the server by %1: %2.").arg(Log::formatClientUser(pSrc, Log::Source)).arg(reason).arg(Log::formatClientUser(pDst, Log::Target)));
	} else {
		g.l->log(Log::UserLeave, tr("%1 disconnected.").arg(Log::formatClientUser(pDst, Log::Source)));
	}
	if (pDst != pSelf)
		pmModel->removeUser(pDst);
	VICTIM_DEINIT;
	ACTOR_DEINIT;
	SELF_DEINIT;
}

void MainWindow::msgChannelState(const MumbleProto::ChannelState &msg) {
	if (! msg.has_channel_id())
		return;

	Channel *c = Channel::get(msg.channel_id());
	Channel *p = msg.has_parent() ? Channel::get(msg.parent()) : NULL;

	if (!c) {
		if (msg.has_parent() && p && msg.has_name()) {
			c = pmModel->addChannel(msg.channel_id(), p, u8(msg.name()));
//api
			CSDKInterface* sdkApi = CSDKInterface::sharedSDKInterface();
			sdkApi->onAddChannel(msg.channel_id(), p, u8(msg.name()));
//end api
			c->bTemporary = msg.temporary();
			p = NULL;
		} else {
			return;
		}
	}

	if (p) {
		Channel *pp = p;
		while (pp) {
			if (pp == c)
				return;
			pp = pp->cParent;
		}
		pmModel->moveChannel(c, p);
	}

	if (msg.has_name())
		pmModel->renameChannel(c, u8(msg.name()));

	if (msg.has_description_hash())
		pmModel->setCommentHash(c, blob(msg.description_hash()));
	if (msg.has_description())
		pmModel->setComment(c, u8(msg.description()));

	if (msg.has_position()) {
		pmModel->repositionChannel(c, msg.position());
	}

	if (msg.links_size()) {
		QList<Channel *> ql;
		pmModel->unlinkAll(c);
		for (int i=0;i<msg.links_size();++i) {
			Channel *l = Channel::get(msg.links(i));
			if (l)
				ql << l;
		}
		if (! ql.isEmpty())
			pmModel->linkChannels(c, ql);
	}
	if (msg.links_remove_size()) {
		QList<Channel *> ql;
		for (int i=0;i<msg.links_remove_size();++i) {
			Channel *l = Channel::get(msg.links_remove(i));
			if (l)
				ql << l;
		}
		if (! ql.isEmpty())
			pmModel->unlinkChannels(c, ql);
	}
	if (msg.links_add_size()) {
		QList<Channel *> ql;
		for (int i=0;i<msg.links_add_size();++i) {
			Channel *l = Channel::get(msg.links_add(i));
			if (l)
				ql << l;
		}
		if (! ql.isEmpty())
			pmModel->linkChannels(c, ql);
	}
}

void MainWindow::msgChannelRemove(const MumbleProto::ChannelRemove &msg) {
	Channel *c = Channel::get(msg.channel_id());
	if (c && (c->iId != 0))
		pmModel->removeChannel(c);
}

void MainWindow::msgTextMessage(const MumbleProto::TextMessage &msg) {
	ACTOR_INIT;
	QString target;
	const QString &plainName = pSrc ? pSrc->qsName : tr("Server", "message from");
	const QString &name = pSrc ? Log::formatClientUser(pSrc, Log::Source) : tr("Server", "message from");

	if (msg.tree_id_size() > 0) {
		target += tr("(Tree) ");
	} else if (msg.channel_id_size() > 0) {
		target += tr("(Channel) ");
	}

	g.l->log(Log::TextMessage, tr("%2%1: %3").arg(name).arg(target).arg(u8(msg.message())),
	         tr("Message from %1").arg(plainName));
	ACTOR_DEINIT
}

void MainWindow::msgACL(const MumbleProto::ACL &msg) {
#ifdef ADDITIONAL_WINDOWS
	if (aclEdit) {
		aclEdit->reject();
		delete aclEdit;
		aclEdit = NULL;
	}
	if (Channel::get(msg.channel_id())) {
		aclEdit = new ACLEditor(msg.channel_id(), msg, this);
		aclEdit->show();
	}
#endif
}

void MainWindow::msgQueryUsers(const MumbleProto::QueryUsers &msg) {
#ifdef ADDITIONAL_WINDOWS
	if (aclEdit)
		aclEdit->returnQuery(msg);
#endif
}

void MainWindow::msgPing(const MumbleProto::Ping &) {
}

void MainWindow::msgCryptSetup(const MumbleProto::CryptSetup &msg) {
	g.sh_lock();
	ConnectionPtr c= g.sh->cConnection;
	if (! c) {
		g.sh_unlock();
		return;
	}
#ifdef SSL_SOCKET
	if (msg.has_key() && msg.has_client_nonce() && msg.has_server_nonce()) {
		const std::string &key = msg.key();
		const std::string &client_nonce = msg.client_nonce();
		const std::string &server_nonce = msg.server_nonce();

		if (key.size() == AES_BLOCK_SIZE && client_nonce.size() == AES_BLOCK_SIZE && server_nonce.size() == AES_BLOCK_SIZE)
			c->csCrypt.setKey(reinterpret_cast<const unsigned char *>(key.data()), reinterpret_cast<const unsigned char *>(client_nonce.data()), reinterpret_cast<const unsigned char *>(server_nonce.data()));
	} else if (msg.has_server_nonce()) {
		const std::string &server_nonce = msg.server_nonce();
		if (server_nonce.size() == AES_BLOCK_SIZE) {
			c->csCrypt.uiResync++;
			memcpy(c->csCrypt.decrypt_iv, server_nonce.data(), AES_BLOCK_SIZE);
		}
	} else {
		MumbleProto::CryptSetup mpcs;
		mpcs.set_client_nonce(std::string(reinterpret_cast<const char *>(c->csCrypt.encrypt_iv), AES_BLOCK_SIZE));
		g.sh->sendMessage(mpcs);
	}
#endif
	g.sh_unlock();
}

void MainWindow::msgContextAction(const MumbleProto::ContextAction &) {
}

void MainWindow::msgVersion(const MumbleProto::Version &msg) {
	g.sh_lock();
	if (msg.has_version())
		g.sh->uiVersion = msg.version();
	if (msg.has_release())
		g.sh->qsRelease = u8(msg.release());
	if (msg.has_os()) {
		g.sh->qsOS = u8(msg.os());
#ifdef USE_OVERLAY
		if (msg.has_os_version())
			g.sh->qsOSVersion = u8(msg.os_version());
#endif
	}
	g.sh_unlock();
}

void MainWindow::msgUserList(const MumbleProto::UserList &msg) {
#ifdef ADDITIONAL_WINDOWS
	if (userEdit) {
		userEdit->reject();
		delete userEdit;
		userEdit = NULL;
	}
	userEdit = new UserEdit(msg, this);
	userEdit->show();
#endif
}

void MainWindow::msgVoiceTarget(const MumbleProto::VoiceTarget &) {
}

void MainWindow::msgPermissionQuery(const MumbleProto::PermissionQuery &msg) {
	Channel *current = pmModel->getChannel(qtvUsers->currentIndex());

	if (msg.flush()) {
		foreach(Channel *c, Channel::c_qhChannels)
			c->uiPermissions = 0;

		// We always need the permissions of the current focus channel
		if (current && current->iId != msg.channel_id()) {
			g.sh_lock();
			g.sh->requestChannelPermissions(current->iId);
			g.sh_unlock();

			current->uiPermissions = ChanACL::All;
		}
	}
	Channel *c = Channel::get(msg.channel_id());
	if (c) {
		c->uiPermissions = msg.permissions();
		if (c->iId == 0)
			g.pPermissions = static_cast<ChanACL::Permissions>(c->uiPermissions);
		if (c == current) {
			updateMenuPermissions();
		}
	}
}

void MainWindow::msgCodecVersion(const MumbleProto::CodecVersion &msg) {
	int alpha = msg.has_alpha() ? msg.alpha() : -1;
	int beta = msg.has_beta() ? msg.beta() : -1;
	bool pref = msg.prefer_alpha();

	#ifdef USE_OPUS
		g.bOpus = msg.opus();
	#endif

	// Workaround for broken 1.2.2 servers
	g.sh_lock();
	if (g.sh && g.sh->uiVersion == 0x010202 && alpha != -1 && alpha == beta) {
		if (pref)
			beta = g.iCodecBeta;
		else
			alpha = g.iCodecAlpha;
	}
	g.sh_unlock();

	if ((alpha != -1) && (alpha != g.iCodecAlpha)) {
		g.iCodecAlpha = alpha;
		if (pref && ! g.qmCodecs.contains(alpha))
			pref = ! pref;
	}
	if ((beta != -1) && (beta != g.iCodecBeta)) {
		g.iCodecBeta = beta;
		if (! pref && ! g.qmCodecs.contains(beta))
			pref = ! pref;
	}
	g.bPreferAlpha = pref;

	int willuse = pref ? g.iCodecAlpha : g.iCodecBeta;

	static bool warned = false;

	if (! g.qmCodecs.contains(willuse)) {
		if (! warned) {
			g.l->log(Log::CriticalError, tr("Unable to find matching CELT codecs with other clients. You will not be able to talk to all users."));
			warned = true;
		}
	} else {
		warned = false;
	}
}

void MainWindow::msgUserStats(const MumbleProto::UserStats &msg) {
#ifdef ADDITIONAL_WINDOWS
	UserInformation *ui = qmUserInformations.value(msg.session());
	if (ui) {
		ui->update(msg);
	} else {
#ifdef USE_OVERLAY
		ui = new UserInformation(msg, g.ocIntercept ? g.mw : NULL);
#endif
		ui->setAttribute(Qt::WA_DeleteOnClose, true);
		connect(ui, SIGNAL(destroyed()), this, SLOT(destroyUserInformation()));

		qmUserInformations.insert(msg.session(), ui);
		ui->show();
	}
#endif
}

void MainWindow::msgRequestBlob(const MumbleProto::RequestBlob &) {
}

void MainWindow::msgContextActionModify(const MumbleProto::ContextActionModify &msg) {
	if (msg.has_operation() && msg.operation() == MumbleProto::ContextActionModify_Operation_Remove) {
		removeContextAction(msg);
		return;
	}

	if (msg.has_operation() && msg.operation() != MumbleProto::ContextActionModify_Operation_Add)
		return;

	QAction *a = new QAction(u8(msg.text()), g.mw);
	a->setData(u8(msg.action()));
	connect(a, SIGNAL(triggered()), this, SLOT(context_triggered()));
	unsigned int ctx = msg.context();
	if (ctx & MumbleProto::ContextActionModify_Context_Server)
		qlServerActions.append(a);
	if (ctx & MumbleProto::ContextActionModify_Context_User)
		qlUserActions.append(a);
	if (ctx & MumbleProto::ContextActionModify_Context_Channel)
		qlChannelActions.append(a);
}

void MainWindow::removeContextAction(const MumbleProto::ContextActionModify &msg) {
	QString action = u8(msg.action());

	QSet<QAction *> qs;
	qs += qlServerActions.toSet();
	qs += qlChannelActions.toSet();
	qs += qlUserActions.toSet();

	foreach(QAction *a, qs) {
		if (a->data() == action) {
			qlServerActions.removeOne(a);
			qlChannelActions.removeOne(a);
			qlUserActions.removeOne(a);
			delete a;
		}
	}
}

void MainWindow::msgSuggestConfig(const MumbleProto::SuggestConfig &msg) {
/*	if (msg.has_version() && (msg.version() > MumbleVersion::getRaw())) {
		g.l->log(Log::Warning, tr("The server requests minimum client version %1").arg(MumbleVersion::toString(msg.version())));
	}
	if (msg.has_positional() && (msg.positional() != g.s.doPositionalAudio())) {
		if (msg.positional())
			g.l->log(Log::Warning, tr("The server requests positional audio be enabled."));
		else
			g.l->log(Log::Warning, tr("The server requests positional audio be disabled."));
	}
	if (msg.has_push_to_talk() && (msg.push_to_talk() != (g.s.atTransmit == Settings::PushToTalk))) {
		if (msg.push_to_talk())
			g.l->log(Log::Warning, tr("The server requests Push-to-Talk be enabled."));
		else
			g.l->log(Log::Warning, tr("The server requests Push-to-Talk be disabled."));
	}*/
}
