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
#include "DirectSound.h"
#include "MainWindow.h"
#include "Plugins.h"
#include "User.h"
#include "Global.h"
#include "../SDKInterface/MumbleWndMessage.h"
#include "../SDKInterface/MumbleErrorCodes.h"
#include <uuids.h>

#undef FAILED
#define FAILED(Status) (static_cast<HRESULT>(Status)<0)

// #define MY_DEFERRED DS3D_DEFERRED
#define MY_DEFERRED DS3D_IMMEDIATE


#define NBLOCKS 50
#define MAX(a,b)        ( (a) > (b) ? (a) : (b) )
#define MIN(a,b)        ( (a) < (b) ? (a) : (b) )

class DXAudioOutputRegistrar : public AudioOutputRegistrar {
	public:
		DXAudioOutputRegistrar();
		virtual AudioOutput *create();
		virtual const QList<audioDevice> getDeviceChoices( bool aForce);
		virtual void setDeviceChoice(const QVariant &, Settings &);

};

class DXAudioInputRegistrar : public AudioInputRegistrar {
	public:
		DXAudioInputRegistrar();
		virtual AudioInput *create();
		virtual AudioInput *create(QString aDeviceName, QVariant aDeviceUID);
		virtual const QList<audioDevice> getDeviceChoices( bool aForce);
		virtual void setDeviceChoice(const QVariant &, Settings &);
		virtual bool canEcho(const QString &) const;

};

class DirectSoundInit : public DeferInit {
		DXAudioInputRegistrar *airReg;
		DXAudioOutputRegistrar *aorReg;
	public:
		void initialize();
		void destroy();
};

static DirectSoundInit dsinit;

void DirectSoundInit::initialize() {
	airReg = NULL;
	aorReg = NULL;

#ifdef USE_WASAPI
	OSVERSIONINFOEXW ovi;
	memset(&ovi, 0, sizeof(ovi));

	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionEx(reinterpret_cast<OSVERSIONINFOW *>(&ovi));

#ifdef QT_NO_DEBUG
	if ((ovi.dwMajorVersion > 6) || ((ovi.dwMajorVersion == 6) && (ovi.dwBuildNumber >= 6001))) {
		HMODULE hLib = LoadLibrary(L"AVRT.DLL");
		if (hLib != NULL) {
			FreeLibrary(hLib);
			qWarning("DirectSound: Disabled as WASAPI is available");
			return;
		}
	}
#endif
#endif

	airReg = new DXAudioInputRegistrar();
	aorReg = new DXAudioOutputRegistrar();
}

void DirectSoundInit::destroy() {
	delete airReg;
	delete aorReg;
}


DXAudioOutputRegistrar::DXAudioOutputRegistrar() : AudioOutputRegistrar(QLatin1String("DirectSound")) {
}

AudioOutput *DXAudioOutputRegistrar::create() {
	return new DXAudioOutput();
}

typedef QPair<QString, QString> dsDevice;

static BOOL CALLBACK DSEnumProc(LPGUID lpGUID, const WCHAR* lpszDesc,
                                const WCHAR*, void *ctx) {
	if (lpGUID) {
		QList<dsDevice> *l =reinterpret_cast<QList<dsDevice> *>(ctx);
		wchar_t wsGUID[ 512];
		wsGUID[ StringFromGUID2( *lpGUID, &wsGUID[0], 512)] = 0;
		*l << dsDevice(QString::fromUtf16(reinterpret_cast<const ushort*>(lpszDesc)), QString::fromWCharArray(wsGUID));
	}

	return(true);
}

const QList<audioDevice> DXAudioOutputRegistrar::getDeviceChoices( bool aForce) {
	QList<dsDevice> qlOutput;

	wchar_t wsGUID[ 512];
	wsGUID[ StringFromGUID2( DSDEVID_DefaultPlayback, &wsGUID[0], 512)] = 0;
	qlOutput << dsDevice(DXAudioOutput::tr("Default DirectSound Voice Output"), QString::fromWCharArray(wsGUID));
	DirectSoundEnumerateW(DSEnumProc, reinterpret_cast<void *>(&qlOutput));

	QList<audioDevice> qlReturn;

	const GUID *lpguid = NULL;

	if (! g.s.qbaDXOutput.isEmpty()) {
		lpguid = reinterpret_cast<LPGUID>(g.s.qbaDXOutput.data());
	} else {
		lpguid = &DSDEVID_DefaultPlayback;
	}

	foreach(dsDevice d, qlOutput) {
		//qlReturn.append( audioDevice(d.first, QByteArray(reinterpret_cast<const char *>(&d.second), sizeof(GUID))));
		qlReturn.append( audioDevice(d.first, QByteArray( d.second.toLatin1())));
	}	
	return qlReturn;
}

void DXAudioOutputRegistrar::setDeviceChoice(const QVariant &choice, Settings &s) {
	s.qbaDXOutput = choice.toByteArray();
}

DXAudioInputRegistrar::DXAudioInputRegistrar() : AudioInputRegistrar(QLatin1String("DirectSound")) {
}

AudioInput *DXAudioInputRegistrar::create() {
	return new DXAudioInput(QLatin1String(""), QVariant(QLatin1String("")));
}

AudioInput *DXAudioInputRegistrar::create(QString aDeviceName, QVariant aDeviceUID) {
	return new DXAudioInput(aDeviceName, aDeviceUID);
}

const QList<audioDevice> DXAudioInputRegistrar::getDeviceChoices( bool aForce) {
	QList<dsDevice> qlInput;

	wchar_t wsGUID[ 512];
	wsGUID[ StringFromGUID2( DSDEVID_DefaultVoiceCapture, &wsGUID[0], 512)] = 0;
	//qlInput << dsDevice(DXAudioInput::tr("Default DirectSound Voice Input"), DSDEVID_DefaultVoiceCapture);
	qlInput << dsDevice(DXAudioInput::tr("Default DirectSound Voice Input"), QString::fromWCharArray(wsGUID));
	DirectSoundCaptureEnumerateW(DSEnumProc, reinterpret_cast<void *>(&qlInput));

	QList<audioDevice> qlReturn;

	const GUID *lpguid = NULL;

	if (! g.s.qbaDXInput.isEmpty()) {
		lpguid = reinterpret_cast<LPGUID>(g.s.qbaDXInput.data());
	} else {
		lpguid = &DSDEVID_DefaultVoiceCapture;
	}

	/*foreach(dsDevice d, qlInput) {
		if (d.second == *lpguid) {
			qlReturn << audioDevice(d.first, QByteArray(reinterpret_cast<const char *>(&d.second), sizeof(GUID)));
		}
	}
	foreach(dsDevice d, qlInput) {
		if (d.second != *lpguid) {
			qlReturn << audioDevice(d.first, QByteArray(reinterpret_cast<const char *>(&d.second), sizeof(GUID)));
		}
	}*/
	foreach(dsDevice d, qlInput) {		
		qlReturn << audioDevice(d.first, QByteArray( d.second.toLatin1()));
		//qlReturn << audioDevice(d.first, QByteArray(reinterpret_cast<const char *>(&d.second), sizeof(GUID)));
	}
	return qlReturn;
}

void DXAudioInputRegistrar::setDeviceChoice(const QVariant &choice, Settings &s) {
	s.qbaDXInput = choice.toByteArray();
}

bool DXAudioInputRegistrar::canEcho(const QString &) const {
	return false;
}

DXAudioOutput::DXAudioOutput() {
}

DXAudioOutput::~DXAudioOutput() {
	bRunning = false;
	wait();
}

void DXAudioOutput::run() {
	HRESULT hr;
	DSBUFFERDESC        dsbdesc;
	WAVEFORMATEXTENSIBLE wfx;
	WAVEFORMATEXTENSIBLE wfxSet;
	int ns = 0;
	unsigned int chanmasks[32];

	LPDIRECTSOUND8             pDS = NULL;
	LPDIRECTSOUNDBUFFER       pDSBPrimary = NULL;
	LPDIRECTSOUNDBUFFER       pDSBOutput = NULL;
	LPDIRECTSOUNDNOTIFY8       pDSNotify = NULL;

	DWORD	dwBufferSize;
	DWORD	dwLastWritePos;
	DWORD	dwLastPlayPos;
	DWORD	dwTotalPlayPos;
	int iLastwriteblock;
	LPVOID aptr1, aptr2;
	DWORD nbytes1, nbytes2;

	int playblock;
	int nowriteblock;
	DWORD dwPlayPosition, dwWritePosition;

	unsigned int iByteSize;

	bool bOk;
	DWORD dwSpeakerConfig;

	bool failed = false;

	bOk = false;
	DWORD dwMask = 0;
	bool bHead = false;

	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize  = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	if (! g.s.qbaDXOutput.isEmpty()) {
		LPGUID lpguid = reinterpret_cast<LPGUID>(g.s.qbaDXOutput.data());
		if (FAILED(hr = DirectSoundCreate8(lpguid, &pDS, NULL))) {
			failed = true;
		}
	}

	if (! pDS && FAILED(hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &pDS, NULL))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_CREATE_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutput: DirectSoundCreate failed: hr=0x%08lx", hr);
		goto cleanup;
	} else if (FAILED(hr = pDS->SetCooperativeLevel(g.mw->winId(), DSSCL_PRIORITY))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_SETCOOPERATIVELEVEL_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutput: SetCooperativeLevel failed: hr=0x%08lx", hr);
		goto cleanup;
	}else if (FAILED(hr = pDS->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_CREATESOUNDBUFFER_PRIMARY_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutput: CreateSoundBuffer (Primary) failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	pDS->GetSpeakerConfig(&dwSpeakerConfig);

	switch (DSSPEAKER_CONFIG(dwSpeakerConfig)) {
		case DSSPEAKER_HEADPHONE:
			dwMask = KSAUDIO_SPEAKER_STEREO;
			bHead = true;
			break;
		case DSSPEAKER_MONO:
			dwMask = KSAUDIO_SPEAKER_MONO;
			break;
		case DSSPEAKER_QUAD:
			dwMask = KSAUDIO_SPEAKER_QUAD;
			break;
		case DSSPEAKER_STEREO:
			dwMask = KSAUDIO_SPEAKER_STEREO;
			break;
		case DSSPEAKER_SURROUND:
			dwMask = KSAUDIO_SPEAKER_SURROUND;
			break;
		case DSSPEAKER_5POINT1:
			dwMask = KSAUDIO_SPEAKER_5POINT1;
			break;
		case DSSPEAKER_7POINT1:
			dwMask = KSAUDIO_SPEAKER_7POINT1;
			break;
		case DSSPEAKER_7POINT1_SURROUND:
			dwMask = KSAUDIO_SPEAKER_7POINT1_SURROUND;
			break;
		case DSSPEAKER_5POINT1_SURROUND:
			dwMask = KSAUDIO_SPEAKER_5POINT1_SURROUND;
			break;
		default:
			dwMask = 0;
			break;
	}

	if (! g.s.doPositionalAudio())
		dwMask = KSAUDIO_SPEAKER_MONO;

	for (int i=0;i<32;i++) {
		if (dwMask & (1 << i)) {
			chanmasks[ns++] = 1 << i;
		}
	}

	iMixerFreq = SAMPLE_RATE;
	iChannels = ns;
	eSampleFormat = SampleShort;

	iByteSize = iFrameSize * sizeof(short) * ns;

	ZeroMemory(&wfxSet, sizeof(wfxSet));
	wfxSet.Format.wFormatTag = WAVE_FORMAT_PCM;

	ZeroMemory(&wfx, sizeof(wfx));
	wfx.Format.wFormatTag = WAVE_FORMAT_PCM;
	wfx.Format.nChannels = qMax(ns, 1);
	wfx.Format.nSamplesPerSec = SAMPLE_RATE;
	wfx.Format.nBlockAlign = sizeof(short) * wfx.Format.nChannels;
	wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
	wfx.Format.wBitsPerSample = 16;

	if (FAILED(hr = pDSBPrimary->SetFormat(reinterpret_cast<WAVEFORMATEX *>(&wfx)))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_SETFORMAT_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutput: SetFormat failed: hr=0x%08lx", hr);
		goto cleanup;
	}
	if (FAILED(hr = pDSBPrimary->GetFormat(reinterpret_cast<WAVEFORMATEX *>(&wfxSet), sizeof(wfxSet), NULL))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_GETFORMAT_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutput: GetFormat failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	qWarning("DXAudioOutput: Primary buffer of %ld Hz, %d channels, %d bits",wfxSet.Format.nSamplesPerSec,wfxSet.Format.nChannels,wfxSet.Format.wBitsPerSample);

	ZeroMemory(&wfx, sizeof(wfx));
	wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	wfx.Format.nChannels = ns;
	wfx.Format.nSamplesPerSec = SAMPLE_RATE;
	wfx.Format.nBlockAlign = sizeof(short) * wfx.Format.nChannels;
	wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
	wfx.Format.wBitsPerSample = 16;
	wfx.Format.cbSize = 32;
	wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;
	//wfx.dwChannelMask = dwMask;
	wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize          = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags         = DSBCAPS_GLOBALFOCUS|DSBCAPS_GETCURRENTPOSITION2;
	dsbdesc.dwFlags	 |= DSBCAPS_CTRLPOSITIONNOTIFY;
	dsbdesc.dwBufferBytes = wfx.Format.nChannels * iFrameSize * sizeof(short) * NBLOCKS;
	dsbdesc.lpwfxFormat     = reinterpret_cast<WAVEFORMATEX *>(&wfx);

	if (FAILED(hr = pDS->CreateSoundBuffer(&dsbdesc, &pDSBOutput, NULL))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_CREATESOUNDBUFFER_SECONDARY_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutputUser: CreateSoundBuffer (Secondary) failed: hr=0x%08lx", hr);
		goto cleanup;
	}


	if (FAILED(hr = pDSBOutput->QueryInterface(IID_IDirectSoundNotify, reinterpret_cast<void **>(&pDSNotify)))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_QUERYINTERFACE_NOTIFY_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutputUser: QueryInterface (Notify) failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	qWarning("DXAudioOutputUser: New %dHz output buffer of %ld bytes", SAMPLE_RATE, dsbdesc.dwBufferBytes);

	if (failed)
		g.mw->msgBox(tr("Opening chosen DirectSound Output failed. Default device will be used."));

	initializeMixer(chanmasks, bHead);

	if (FAILED(hr = pDSBOutput->Lock(0, 0, &aptr1, &nbytes1, &aptr2, &nbytes2, DSBLOCK_ENTIREBUFFER))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_INITIAL_LOCK_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutputUser: Initial Lock failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	dwBufferSize = nbytes1 + nbytes2;
	if (aptr1)
		ZeroMemory(aptr1, nbytes1);
	if (aptr2)
		ZeroMemory(aptr2, nbytes2);

	if (FAILED(hr = pDSBOutput->Unlock(aptr1, nbytes1, aptr2, nbytes2))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_INITIAL_UNLOCK_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutputUser: Initial Unlock failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	if (FAILED(hr = pDSBOutput->Play(0, 0, DSBPLAY_LOOPING))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_PLAY_FAILED), (LPARAM)(hr));
		qWarning("DXAudioOutputUser: Play failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	dwLastWritePos = 0;
	dwLastPlayPos = 0;
	dwTotalPlayPos = 0;

	iLastwriteblock = (NBLOCKS - 1 + g.s.iOutputDelay) % NBLOCKS;

	bOk = true;
	bool firstCycle = true;

	while (bRunning && ! FAILED(hr)) {
		if (FAILED(hr = pDSBOutput->GetCurrentPosition(&dwPlayPosition, &dwWritePosition))) {
			qWarning("DXAudioOutputUser: GetCurrentPosition failed: hr=0x%08lx", hr);
			break;
		}

		playblock = dwWritePosition / iByteSize;
		nowriteblock = (playblock + g.s.iOutputDelay + 1) % NBLOCKS;

		for (int block=(iLastwriteblock + 1) % NBLOCKS;(!FAILED(hr)) && (block!=nowriteblock);block=(block + 1) % NBLOCKS) {
			iLastwriteblock = block;

			if (FAILED(hr = pDSBOutput->Lock(block * iByteSize, iByteSize, &aptr1, &nbytes1, &aptr2, &nbytes2, 0))) {
				qWarning("DXAudioOutput: Lock block %u (%d bytes) failed: hr=0x%08lx",block, iByteSize, hr);
				break;
			}
			if (aptr2 || nbytes2) {
				qWarning("DXAudioOutput: Split buffer");
				break;
			}
			if (!aptr1 || ! nbytes1) {
				qWarning("DXAudioOutput: Zerolock");
				break;
			}
			if (! mix(reinterpret_cast<short *>(aptr1), iFrameSize))
			{
				ZeroMemory(aptr1, iByteSize);
			}

			if (FAILED(hr = pDSBOutput->Unlock(aptr1, nbytes1, aptr2, nbytes2))) {
				qWarning("DXAudioOutput: Unlock %p(%lu) %p(%lu) failed: hr=0x%08lx",aptr1,nbytes1,aptr2,nbytes2,hr);
				break;
			}

			if (FAILED(hr = pDSBOutput->GetCurrentPosition(&dwPlayPosition, &dwWritePosition))) {
				qWarning("DXAudioOutputUser: GetCurrentPosition failed: hr=0x%08lx", hr);
				break;
			}

			playblock = dwWritePosition / iByteSize;
			nowriteblock = (playblock + g.s.iOutputDelay + 1) % NBLOCKS;
		}
		if (! FAILED(hr))
			msleep(5);
		if (firstCycle) {
			firstCycle = false;
			if (g.hCallbackWnd)
				::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(MIC_NO_ERROR), (LPARAM)(0));
		}
		bNotHang = true;
	}

	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_OUT_LOST_DEVICE), (LPARAM)(hr));

		g.mw->msgBox(tr("Lost DirectSound output device."));
	}
cleanup:
	if (! bOk) {
		g.mw->msgBox(tr("Opening chosen DirectSound Output failed. No audio will be heard."));
		return;
	}

	if (pDSNotify)
		pDSNotify->Release();
	if (pDSBOutput) {
		pDSBOutput->Stop();
		pDSBOutput->Release();
	}
	if (pDSBPrimary)
		pDSBPrimary->Release();
	if (pDS)
		pDS->Release();
}

#define NBUFFBLOCKS 50

DXAudioInput::DXAudioInput(QString aDeviceName, QVariant aDeviceUID) {
	m_deviceName = aDeviceName;
	m_deviceUID = aDeviceUID;
}

DXAudioInput::~DXAudioInput() {
	bRunning = false;
	wait();
}

void DXAudioInput::run() {
	LPDIRECTSOUNDCAPTURE8      pDSCapture;
	LPDIRECTSOUNDCAPTUREBUFFER pDSCaptureBuffer;
	LPDIRECTSOUNDNOTIFY8       pDSNotify;

	DWORD dwBufferSize;
	bool bOk;
	DWORD dwReadyBytes = 0;
	DWORD dwLastReadPos = 0;
	DWORD dwReadPosition;
	DWORD dwCapturePosition;

	LPVOID aptr1, aptr2;
	DWORD nbytes1, nbytes2;

	HRESULT       hr;
	WAVEFORMATEX  wfx;
	DSCBUFFERDESC dscbd;

	pDSCapture = NULL;
	pDSCaptureBuffer = NULL;
	pDSNotify = NULL;

	bOk = false;

	bool failed = false;
	float safety = 2.0f;
	bool didsleep = false;
	bool firstsleep = false;

	Timer t;

	ZeroMemory(&wfx, sizeof(wfx));
	wfx.wFormatTag = WAVE_FORMAT_PCM;

	ZeroMemory(&dscbd, sizeof(dscbd));
	dscbd.dwSize = sizeof(dscbd);

	dscbd.dwBufferBytes = dwBufferSize = iFrameSize * sizeof(short) * NBUFFBLOCKS;
	dscbd.lpwfxFormat = &wfx;

	wfx.nChannels = 1;
	wfx.nSamplesPerSec = iSampleRate;
	wfx.nBlockAlign = 2;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.wBitsPerSample = 16;

	if (!m_deviceUID.toByteArray().isEmpty()) {
		LPGUID lpguid = reinterpret_cast<LPGUID>(m_deviceUID.toByteArray().data());
		if (FAILED(hr = DirectSoundCaptureCreate8(lpguid, &pDSCapture, NULL))) {
			failed = true;
		}
	}
	// Create IDirectSoundCapture using the preferred capture device
	if (! g.s.qbaDXInput.isEmpty() && failed) {
		LPGUID lpguid = reinterpret_cast<LPGUID>(g.s.qbaDXInput.data());
		if (FAILED(hr = DirectSoundCaptureCreate8(lpguid, &pDSCapture, NULL))) {
			failed = true;
		}
	}

	if (! pDSCapture && FAILED(hr = DirectSoundCaptureCreate8(&DSDEVID_DefaultVoiceCapture, &pDSCapture, NULL))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_IN_CAPTURE_CREATE_FAILED), (LPARAM)(hr));
		qWarning("DXAudioInput: DirectSoundCaptureCreate failed: hr=0x%08lx", hr);
	}
	else if (FAILED(hr = pDSCapture->CreateCaptureBuffer(&dscbd, &pDSCaptureBuffer, NULL))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_IN_CREATE_CAPTURE_BUFFER_FAILED), (LPARAM)(hr));
		qWarning("DXAudioInput: CreateCaptureBuffer failed: hr=0x%08lx", hr);
	}
	else if (FAILED(hr = pDSCaptureBuffer->QueryInterface(IID_IDirectSoundNotify, reinterpret_cast<void **>(&pDSNotify)))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_IN_QUERYINTERFACE_NOTIFY_FAILED), (LPARAM)(hr));
		qWarning("DXAudioInput: QueryInterface (Notify) failed: hr=0x%08lx", hr);
	}
	else
		bOk = true;



	if (failed) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_IN_OPEN_CHOSEN_DEVICE_FAILED), (LPARAM)(hr));
		g.mw->msgBox(tr("Opening chosen DirectSound Input failed. Default device will be used."));
	}

	qWarning("DXAudioInput: Initialized");

	if (! bOk)
		goto cleanup;

	if (FAILED(hr = pDSCaptureBuffer->Start(DSCBSTART_LOOPING))) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_IN_START_FAILED), (LPARAM)(hr));
		qWarning("DXAudioInput: Start failed: hr=0x%08lx", hr);
	} else {
		while (bRunning) {
			firstsleep = true;
			didsleep = false;

			do {
				if (FAILED(hr = pDSCaptureBuffer->GetCurrentPosition(&dwCapturePosition, &dwReadPosition))) {
					qWarning("DXAudioInput: GetCurrentPosition failed: hr=0x%08lx", hr);
					bRunning = false;
					break;
				}
				if (dwReadPosition < dwLastReadPos)
					dwReadyBytes = (dwBufferSize - dwLastReadPos) + dwReadPosition;
				else
					dwReadyBytes = dwReadPosition - dwLastReadPos;

				if (static_cast<int>(dwReadyBytes) < sizeof(short) * iFrameSize) {
					double msecleft = 20.0 - (dwReadyBytes * 20.0) / (sizeof(short) * iFrameSize);

					if (didsleep)
						safety *= 1.1f;
					else if (firstsleep)
						safety *= 0.998f;

					int msec = static_cast<int>(msecleft + (firstsleep ? safety : 0.0));
					msleep(msec);

					didsleep = true;
					firstsleep = false;
				}
			} while ((static_cast<int>(dwReadyBytes) < sizeof(short) * iFrameSize) && bRunning);

			// Desynchonized?
			if (dwReadyBytes > (dwBufferSize / 2)) {
				qWarning("DXAudioInput: Lost synchronization");
				dwLastReadPos = dwReadPosition;
			} else if (bRunning) {
				if (FAILED(hr = pDSCaptureBuffer->Lock(dwLastReadPos, sizeof(short) * iFrameSize, &aptr1, &nbytes1, &aptr2, &nbytes2, 0))) {
					qWarning("DXAudioInput: Lock from %ld (%d bytes) failed: hr=0x%08lx",dwLastReadPos, sizeof(short) * iFrameSize, hr);
					bRunning = false;
					break;
				}

				if (aptr1 && nbytes1)
					CopyMemory(psMic, aptr1, nbytes1);

				if (aptr2 && nbytes2)
					CopyMemory(psMic+nbytes1/2, aptr2, nbytes2);

				if (FAILED(hr = pDSCaptureBuffer->Unlock(aptr1, nbytes1, aptr2, nbytes2))) {
					qWarning("DXAudioInput: Unlock failed: hr=0x%08lx", hr);
					bRunning = false;
					break;
				}

				dwLastReadPos = (dwLastReadPos + sizeof(short) * iFrameSize) % dwBufferSize;

				encodeAudioFrame();
			}
			bNotHang = true;
		}
		if (! FAILED(hr))
			pDSCaptureBuffer->Stop();
	}

	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(DIRECTSOUND_IN_LOST_DEVICE), (LPARAM)(hr));

		g.mw->msgBox(tr("Lost DirectSound input device."));
	}

cleanup:
	if (! bOk) {
		g.mw->msgBox(tr("Opening chosen DirectSound Input device failed. No microphone capture will be done."));
	}
	if (pDSNotify)
		pDSNotify->Release();
	if (pDSCaptureBuffer)
		pDSCaptureBuffer->Release();
	if (pDSCapture)
		pDSCapture->Release();
}



std::vector<std::wstring> DXAudioInput::GetMicLinesOnDevice(const wchar_t* awsDeviceName) {
	std::vector<std::wstring>	wsNames;
	unsigned int mixerID = -1;
	HMIXER hMixer;
	HRESULT hr;

	int deviceCount = waveInGetNumDevs();
	for( int i=0; i<deviceCount; i++)
	{
		WAVEINCAPSW caps;
		::waveInGetDevCapsW(i, &caps, sizeof(caps));

		if( wcscmp(caps.szPname, awsDeviceName) == 0)
		{
			MMRESULT err = mixerGetID((HMIXEROBJ)i, &mixerID, MIXER_OBJECTF_WAVEIN);
			if( err != MMSYSERR_NOERROR)
				mixerID = -1;
			break;
		}
	}
	if( mixerID == -1) {
		return wsNames;
	}

	hr = mixerOpen(&hMixer, mixerID, 0, 0, 0);
	if (FAILED(hr))
	{
		return wsNames;
	}

	MIXERLINEW mxl;
	DWORD kind, count;

	kind = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;

	hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return wsNames;
	}

	count = mxl.cConnections;

	
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if (mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
		{
			std::wstring wsMicrophoneName = mxl.szName;
			wsNames.push_back( wsMicrophoneName);
		}
	}
	mixerClose(hMixer);

	return wsNames;
}


CComQIPtr<IAMAudioInputMixer>	DXAudioInput::GetDeviceVolumeControlImpl(const std::wstring& awsDeviceName, BOOL abInDevice) const
{
	HRESULT hr;
	CComQIPtr<IAMAudioInputMixer>	nullRet;
	ICreateDevEnum *pSysDevEnum = NULL;
	IFileSinkFilter *pSink= NULL;

	if( awsDeviceName.empty())
		return nullRet;

	CoInitialize( NULL);

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		return nullRet;
	}

	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	GUID enumGUID = CLSID_AudioInputDeviceCategory;
	if( !abInDevice)
		enumGUID = CLSID_AudioRendererCategory;
	hr = pSysDevEnum->CreateClassEnumerator(enumGUID, &pEnumCat, 0);


	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					// Display the name in your UI somehow.
					if( awsDeviceName.compare(varName.bstrVal) == 0)
						//if( awsDeviceName.find(varName.bstrVal) != std::wstring::npos)
					{
						// To create an instance of the filter, do the following:
						IBaseFilter *pFilter;
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
						// Now add the filter to the graph. 
						if( pFilter)
						{
							CComQIPtr<IAMAudioInputMixer>	ret = pFilter;

							VariantClear(&varName);
							pFilter->Release();
							pPropBag->Release();
							pMoniker->Release();
							pEnumCat->Release();
							pSysDevEnum->Release();
							return ret;
						}
					}
				}
				VariantClear(&varName);
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();
	CoUninitialize();
	return nullRet;
}

CComQIPtr<IAMAudioInputMixer>	DXAudioInput::GetLineVolumeControlImpl(const std::wstring& awsDeviceName, const std::wstring& awsLineName, bool abInDevice) const
{
	CComQIPtr<IAMAudioInputMixer>	nullRet;
	CComQIPtr<IAMAudioInputMixer>	devicePtr = GetDeviceVolumeControlImpl( awsDeviceName, abInDevice);
	if( devicePtr == NULL)
		return nullRet;

	CComQIPtr<IBaseFilter>	filterPtr = devicePtr;

	// Now add the filter to the graph. 
	if( filterPtr != NULL)
	{
		IEnumPins*	EnumPins = NULL;
		HRESULT hr = filterPtr->EnumPins(&EnumPins);
		ATLASSERT( SUCCEEDED( hr));
		EnumPins->Reset();
		IPin*		InPin = NULL;	// renderer input
		ULONG		fetched;
		while (EnumPins->Next(1, &InPin, &fetched) == S_OK)
		{
			PIN_INFO	pinfo;
			hr = InPin->QueryPinInfo(&pinfo);
			ATLASSERT( SUCCEEDED( hr));
			pinfo.pFilter->Release();
			InPin->Release();

			if ((abInDevice && pinfo.dir == PINDIR_INPUT)
				|| (!abInDevice && pinfo.dir == PINDIR_OUTPUT))
			{
				if( wcscmp(awsLineName.data(), pinfo.achName) == 0)
				{
					CComQIPtr<IAMAudioInputMixer> pIAMAudioInputMixer = InPin;
					if( pIAMAudioInputMixer)
					{
						EnumPins->Release();
						return pIAMAudioInputMixer;
					}
				}
			}
		}
		EnumPins->Release();
	}
	return nullRet;
}

BOOL	DXAudioInput::SetEnabledInDeviceLine(const std::wstring& awsDeviceName, const std::wstring& awsLineName, BOOL abEnable)
{
	BOOL success = FALSE;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetLineVolumeControlImpl( awsDeviceName, awsLineName, true);
	if( deviceControl)
		if( SUCCEEDED( deviceControl->put_Enable(abEnable)))
			success = TRUE;

	if( abEnable)
	{
		deviceControl = GetDeviceVolumeControlImpl(awsDeviceName, true);
		if( deviceControl)
			if( SUCCEEDED( deviceControl->put_Enable(abEnable)))
				success = TRUE;
	}

	m_lineName.fromWCharArray(awsLineName.c_str());

	return success;
}