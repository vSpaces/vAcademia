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
#include "ClientUser.h"
#include "Channel.h"
#include "Global.h"
#include "AudioOutput.h"

#include "../SDKInterface/MumbleWndMessage.h"

QHash<unsigned int, ClientUser *> ClientUser::c_qmUsers;
QReadWriteLock ClientUser::c_qrwlUsers;

QList<ClientUser *> ClientUser::c_qlTalking;
QReadWriteLock ClientUser::c_qrwlTalking;

ClientUser::ClientUser(QObject *p) : QObject(p) {
	tsState = Settings::Passive;
	bLocalMute = false;
	fPowerMin = fPowerMax = 0.0f;
	fAverageAvailable = 0.0f;
	iFrames = 0;
	iSequence = 0;
	bTalkedFlag = false;

	refCnt = 1;
}

ClientUser *ClientUser::get(unsigned int uiSession) {
	QWriteLocker lock(&c_qrwlUsers);
	ClientUser *p = c_qmUsers.value(uiSession);
	if (p)
		p->addRef();
	return p;
}

ClientUser *ClientUser::getByHash(const QString &hash) {
	QWriteLocker lock(&c_qrwlUsers);

	ClientUser *cu;
	foreach(cu, c_qmUsers) {
		if (cu->qsHash == hash) {
			cu->addRef();
			return cu;
		}
	}

	return NULL;
}

QList<ClientUser *> ClientUser::getTalking() {
	QReadLocker lock(&c_qrwlTalking);
	return c_qlTalking;
}

bool ClientUser::isValid(unsigned int uiSession) {
	QReadLocker lock(&c_qrwlUsers);

	return c_qmUsers.contains(uiSession);
}

ClientUser *ClientUser::add(unsigned int uiSession, QObject *po) {
	QWriteLocker lock(&c_qrwlUsers);

	ClientUser *p = new ClientUser(po);
	p->uiSession = uiSession;
	c_qmUsers[uiSession] = p;
	return p;
}

ClientUser *ClientUser::match(const ClientUser *other, bool matchname) {
	QWriteLocker lock(&c_qrwlUsers);

	ClientUser *p;
	foreach(p, c_qmUsers) {
		if (p == other)
			continue;
		if ((p->iId >= 0) && (p->iId == other->iId)) {
			p->addRef();
			return p;
		}
		if (matchname && (p->qsName == other->qsName)) {
			p->addRef();
			return p;
		}
	}
	return NULL;
}

void ClientUser::remove(unsigned int uiSession) {
	c_qrwlUsers.lockForWrite();

	bool needSendMessage = false;
	bool needSendLeftChannel = false;
	unsigned int sessionID = 0;
	int channelID = 0;

	ClientUser *p = c_qmUsers.take(uiSession);
	if (p) {
		if (p->cChannel)
			p->cChannel->removeUser(p);

		AudioOutputPtr ao = g.ao;
		if (ao)
			ao->removeBuffer(p);

		if (p->tsState != Settings::Passive) {
			QWriteLocker lock(&c_qrwlTalking);
			c_qlTalking.removeAll(p);
		} 

		if (p->cChannel) {
			if (p->uiSession == g.uiSession)//это я
			{
				needSendLeftChannel = true;
			}
			needSendMessage = true;
			sessionID = p->uiSession;
			channelID = p->cChannel->iId;
			
		}
	}
	c_qrwlUsers.unlock();

	if (needSendMessage)
	{
		if (needSendLeftChannel)//это я
		{
			if (g.hCallbackWnd)
				::SendMessage(g.hCallbackWnd, WM_MUMBLE_LEFTCHANNEL, (WPARAM)(channelID), NULL);
			if (g.pOnLeftChannelCallback)
				((IntParamCallback*)g.pOnLeftChannelCallback)((int)sessionID);
		}
		if (g.hCallbackWnd)
			::SendMessage(g.hCallbackWnd, WM_MUMBLE_REMOVEUSER, (WPARAM)(sessionID), (LPARAM)(channelID));
		if (g.pOnRemoveUserCallback)
			((TwoIntParamCallback*)g.pOnRemoveUserCallback)((int)sessionID, (int)channelID);
	}
}

void ClientUser::remove(ClientUser *p) {
	remove(p->uiSession);
}

QString ClientUser::getFlagsString() const {
	QStringList flags;

	if (! qsFriendName.isEmpty())
		flags << ClientUser::tr("Friend");
	if (iId >= 0)
		flags << ClientUser::tr("Authenticated");
	if (bPrioritySpeaker)
		flags << ClientUser::tr("Priority speaker");
	if (bRecording)
		flags << ClientUser::tr("Recording");
	if (bMute)
		flags << ClientUser::tr("Muted (server)");
	if (bDeaf)
		flags << ClientUser::tr("Deafened (server)");
	if (bLocalMute)
		flags << ClientUser::tr("Local Mute");
	if (bSelfMute)
		flags << ClientUser::tr("Muted (self)");
	if (bSelfDeaf)
		flags << ClientUser::tr("Deafened (self)");

	return flags.join(QLatin1String(", "));
}

void ClientUser::setTalking(Settings::TalkState ts) {
	if (tsState == ts)
		return;

	bool nstate = false;
	if (ts == Settings::Passive)
		nstate = true;
	else if (tsState == Settings::Passive)
		nstate = true;

	if (ts != Settings::Talking && ts != Settings::Shouting && ts != Settings::Whispering)
	{
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_USERSTOPPEDTALKING, (WPARAM)uiSession, NULL);
		if (g.pOnUserStoppedTalkingCallback)
			((IntParamCallback*)g.pOnUserStoppedTalkingCallback)((int)uiSession);
	}
	else
	{
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_USERTALKING, (WPARAM)uiSession, NULL);
		if (g.pOnUserTalkingCallback)
			((IntParamCallback*)g.pOnUserTalkingCallback)((int)uiSession);
		mutex.lock();
		bTalkedFlag = true;
		mutex.unlock();
	}

	tsState = ts;
	emit talkingChanged();

	if (nstate && cChannel) {
		QWriteLocker lock(&c_qrwlTalking);
		if (ts == Settings::Passive)
			c_qlTalking.removeAll(this);
		else
			c_qlTalking << this;
	}
}

void ClientUser::setMute(bool mute) {
	if (bMute == mute)
		return;
	bMute = mute;
	if (! bMute)
		bDeaf = false;
	emit muteDeafChanged();
}

void ClientUser::setSuppress(bool suppress) {
	if (bSuppress == suppress)
		return;
	bSuppress = suppress;
	emit muteDeafChanged();
}

void ClientUser::setLocalMute(bool mute) {
	if (bLocalMute == mute)
		return;
	bLocalMute = mute;
	emit muteDeafChanged();
}

void ClientUser::setDeaf(bool deaf) {
	bDeaf = deaf;
	if (bDeaf)
		bMute = true;
	emit muteDeafChanged();
}

void ClientUser::setSelfMute(bool mute) {
	bSelfMute = mute;
	if (! mute)
		bSelfDeaf = false;
	emit muteDeafChanged();
}

void ClientUser::setSelfDeaf(bool deaf) {
	bSelfDeaf = deaf;
	if (deaf)
		bSelfMute = true;
	emit muteDeafChanged();
}

void ClientUser::setPrioritySpeaker(bool priority) {
	if (bPrioritySpeaker == priority)
		return;
	bPrioritySpeaker = priority;
	emit muteDeafChanged();
}

void ClientUser::setRecording(bool recording) {
	if (bRecording == recording)
		return;
	bRecording = recording;
	emit muteDeafChanged();
}

bool ClientUser::lessThanOverlay(const ClientUser *first, const ClientUser *second) {
	if (first->cChannel == second->cChannel || first->cChannel == NULL || second->cChannel == NULL)
		return lessThan(first, second);

	// When sorting for the overlay always place the local users
	// channel above the others
	ClientUser *self = c_qmUsers.value(g.uiSession);
	if (self) {
		if (self->cChannel == first->cChannel)
			return true;
		else if (self->cChannel == second->cChannel)
			return false;
	}

	return Channel::lessThan(first->cChannel, second->cChannel);
}



void ClientUser::sortUsersOverlay(QList<ClientUser *> &list) {
	QReadLocker lock(&c_qrwlUsers);

	qSort(list.begin(), list.end(), ClientUser::lessThanOverlay);
}

bool ClientUser::GetTalkedFlag()
{
	return bTalkedFlag;
}

void ClientUser::ClearTalkedFlag()
{
	mutex.lock();
	bTalkedFlag = false;
	mutex.unlock();
}

/* From Channel.h
 */
void Channel::addClientUser(ClientUser *p) {
	addUser(p);
	if (p->uiSession == g.uiSession)//это я
	{
		if (g.hCallbackWnd)
			::SendMessage(g.hCallbackWnd, WM_MUMBLE_JOINEDCHANNEL, (WPARAM)iId, NULL);
		if (g.pOnJoinedChannelCallback)
			((IntParamCallback*)g.pOnJoinedChannelCallback)((int)iId);
	}
	if (g.hCallbackWnd)
		::SendMessage(g.hCallbackWnd, WM_MUMBLE_ADDUSER, (WPARAM)p->uiSession, (LPARAM)iId);
	if (g.pOnAddUserCallback)
		((TwoIntParamCallback*)g.pOnAddUserCallback)((int)p->uiSession, (int)iId);

	if (p->tsState != Settings::Talking && p->tsState != Settings::Shouting && p->tsState != Settings::Whispering)
	{
		if (g.pOnUserStoppedTalkingCallback)
			((IntParamCallback*)g.pOnUserStoppedTalkingCallback)((int)p->uiSession);
	}
	else
	{
		if (g.pOnUserTalkingCallback)
			((IntParamCallback*)g.pOnUserTalkingCallback)((int)p->uiSession);
	}
	p->setParent(this);
}

QDataStream &operator<<(QDataStream &qds, const ClientUser::JitterRecord &jr) {
	qds << static_cast<qint8>(qBound(-128, jr.iSequence, 127));
	qds << static_cast<qint8>(qBound(-128, jr.iFrames, 127));
	qds << static_cast<quint32>(qMin(4294967295ULL, jr.uiElapsed));
	return qds;
}

void ClientUser::addRef() {
	refCnt++;
}

void ClientUser::removeRef() {
	QWriteLocker lock(&c_qrwlUsers);
	if (refCnt > 0)
		refCnt--;
// падает на удалении
	/*if (refCnt == 0)
		delete this;*/ 
}
