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
#include "WASAPI.h"
#include "User.h"
#include "Global.h"
#include "MainWindow.h"
#include "Timer.h"
#include "../SDKInterface/MumbleWndMessage.h"
#include "../SDKInterface/MumbleErrorCodes.h"

#include "./AudioInFromStream/SharedMemoryStreamSound.h"

const PROPERTYKEY PKEY_AudioEndpoint_FormFactor = { { 0x1da5d803, 0xd492, 0x4edd, { 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e } }, 0 };

// Now that Win7 is published, which includes public versions of these
// interfaces, we simply inherit from those but use the "old" IIDs.

MIDL_INTERFACE("33969B1D-D06F-4281-B837-7EAAFD21A9C0")
IVistaAudioSessionControl2 :
public IAudioSessionControl2 {
};

MIDL_INTERFACE("94BE9D30-53AC-4802-829C-F13E5AD34775")
IAudioSessionQuery :
public IUnknown {
	virtual HRESULT STDMETHODCALLTYPE GetQueryInterface(IAudioSessionEnumerator **) = 0;
};

QMutex WASAPISystem::mutex;
QList<QPair<QString,QString>> WASAPISystem::deviceInputHash;
QList<QPair<QString,QString>> WASAPISystem::deviceOutputHash;

class WASAPIInputRegistrar : public AudioInputRegistrar {
	public:
		WASAPIInputRegistrar();
		virtual AudioInput *create();
		virtual AudioInput *create(QString aDeviceName, QVariant aDeviceUID);
		virtual const QList<audioDevice> getDeviceChoices( bool aForce);
		virtual void setDeviceChoice(const QVariant &, Settings &);
		virtual bool canEcho(const QString &) const;
		virtual bool canExclusive() const;
};

class WASAPIOutputRegistrar : public AudioOutputRegistrar {
	public:
		WASAPIOutputRegistrar();
		virtual AudioOutput *create();
		virtual const QList<audioDevice> getDeviceChoices( bool aForce);
		virtual void setDeviceChoice(const QVariant &, Settings &);
		bool canMuteOthers() const;
		virtual bool canExclusive() const;
};

class WASAPIInit : public DeferInit {
		WASAPIInputRegistrar *wirReg;
		WASAPIOutputRegistrar *worReg;
	public:
		void initialize();
		void destroy();
};

static WASAPIInit wasapiinit;

extern bool bIsWin7, bIsVistaSP1;

void WASAPIInit::initialize() {
	wirReg = NULL;
	worReg = NULL;

	if (! bIsVistaSP1) {
		qWarning("WASAPIInit: Requires Vista SP1");
		return;
	}

	HMODULE hLib = LoadLibrary(L"AVRT.DLL");
	if (hLib == NULL) {
		qWarning("WASAPIInit: Failed to load avrt.dll");
		return;
	}
	FreeLibrary(hLib);

	wirReg = new WASAPIInputRegistrar();
	worReg = new WASAPIOutputRegistrar();
}

void WASAPIInit::destroy() {
	delete wirReg;
	delete worReg;
}


WASAPIInputRegistrar::WASAPIInputRegistrar() : AudioInputRegistrar(QLatin1String("WASAPI"), 10) {
}

AudioInput *WASAPIInputRegistrar::create() {
	return new WASAPIInput(QLatin1String(""), QVariant(QLatin1String("")));
}

AudioInput *WASAPIInputRegistrar::create(QString aDeviceName, QVariant aDeviceUID) {
	return new WASAPIInput(aDeviceName, aDeviceUID);
}

const QList<audioDevice> WASAPIInputRegistrar::getDeviceChoices( bool aForce) {
	return WASAPISystem::mapToDevice(WASAPISystem::getInputDevices( aForce), g.s.qsWASAPIInput, aForce);
}

void WASAPIInputRegistrar::setDeviceChoice(const QVariant &choice, Settings &s) {
	s.qsWASAPIInput = choice.toString();
}

bool WASAPIInputRegistrar::canEcho(const QString &outputsys) const {
	return (outputsys == name);
}

bool WASAPIInputRegistrar::canExclusive() const {
	return true;
}

WASAPIOutputRegistrar::WASAPIOutputRegistrar() : AudioOutputRegistrar(QLatin1String("WASAPI"), 10) {
}

AudioOutput *WASAPIOutputRegistrar::create() {
	return new WASAPIOutput();
}

const QList<audioDevice> WASAPIOutputRegistrar::getDeviceChoices( bool aForce) {
	return WASAPISystem::mapToDevice(WASAPISystem::getOutputDevices( aForce), g.s.qsWASAPIOutput, aForce);
}

void WASAPIOutputRegistrar::setDeviceChoice(const QVariant &choice, Settings &s) {
	s.qsWASAPIOutput = choice.toString();
}

bool WASAPIOutputRegistrar::canMuteOthers() const {
	return true;
}

bool WASAPIOutputRegistrar::canExclusive() const {
	return true;
}

const QList<QPair<QString,QString>> WASAPISystem::getInputDevices( bool aForce) {
	if (deviceInputHash.count() == 0 || aForce == true) {
		mutex.lock();
		deviceInputHash.clear();
		QList<QPair<QString,QString>> local = getDevices(eCapture);
		QListIterator<QPair<QString,QString>> i(local);
		while (i.hasNext()) {
			QPair<QString,QString> pair = i.next();
			deviceInputHash.append(pair);	
		}
		mutex.unlock();
	}

	return deviceInputHash;
}

const QList<QPair<QString,QString>> WASAPISystem::getOutputDevices( bool aForce) {
	if (deviceOutputHash.count() == 0 || aForce == true) {
		mutex.lock();
		deviceOutputHash.clear();
		QList<QPair<QString,QString>> local = getDevices(eRender);
		QListIterator<QPair<QString,QString>> i(local);
		while (i.hasNext()) {
			QPair<QString,QString> pair = i.next();
			deviceOutputHash.append(pair);	
		}
		mutex.unlock();
	}

	return deviceOutputHash;
}

const QList<QPair<QString,QString>> WASAPISystem::getDevices(EDataFlow dataflow) {
	QList<QPair<QString,QString>> devices;

	HRESULT hr;

	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDeviceCollection *pCollection = NULL;

	CoInitialize( NULL);

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&pEnumerator));

	if (! pEnumerator || FAILED(hr)) {
		qWarning("WASAPI: Failed to instatiate enumerator: hr=0x%08lx", hr);
	} else {
		
		IMMDevice *pDevDevice = NULL;
		if( dataflow == EDataFlow::eCapture)
			hr = pEnumerator->GetDefaultAudioEndpoint(dataflow, eCommunications, &pDevDevice);
		else
			hr = pEnumerator->GetDefaultAudioEndpoint(dataflow, eMultimedia, &pDevDevice);
		wchar_t *devID = NULL;	
		bool defaultAdded = false;
		if (!FAILED(hr))		
		{
			devices.append(qMakePair(QString(), tr("Default Device")));
			defaultAdded = true;
			hr = pDevDevice->GetId(&devID);
			PROPVARIANT varDevName;
			IPropertyStore *pStore = NULL;
			pDevDevice->OpenPropertyStore(STGM_READ, &pStore);
			PropVariantInit(&varDevName);
			pStore->GetValue(PKEY_Device_FriendlyName, &varDevName);

			devices.append(qMakePair(QString::fromWCharArray(devID), QString::fromWCharArray(varDevName.pwszVal)));
			PropVariantClear(&varDevName);			
			pStore->Release();
			pDevDevice->Release();
		}

		hr = pEnumerator->EnumAudioEndpoints(dataflow, DEVICE_STATE_ACTIVE, &pCollection);
		if (! pCollection || FAILED(hr)) {
			qWarning("WASAPI: Failed to enumerate: hr=0x%08lx", hr);
		} else {
			UINT ndev = 0;
			pCollection->GetCount(&ndev);		
			if (!defaultAdded && ndev>0)
				devices.append(qMakePair(QString(), tr("Default Device")));
			//qWarning() << "WASAPI: getDevices::ndev==" << ndev;
			for (unsigned int idx=0;idx<ndev;++idx) {
				IMMDevice *pDevice = NULL;
				IPropertyStore *pStore = NULL;

				pCollection->Item(idx, &pDevice);
				pDevice->OpenPropertyStore(STGM_READ, &pStore);

				LPWSTR strid = NULL;
				pDevice->GetId(&strid);

				PROPVARIANT varName;
				PropVariantInit(&varName);

				pStore->GetValue(PKEY_Device_FriendlyName, &varName);
				//qWarning() << (QString::fromWCharArray(varName.pwszVal));

				if( devID == NULL || wcscmp(devID, strid))
				{
					devices.append(qMakePair(QString::fromWCharArray(strid), QString::fromWCharArray(varName.pwszVal)));
				}

				PropVariantClear(&varName);
				CoTaskMemFree(strid);

				pStore->Release();
				pDevice->Release();
			}
			pCollection->Release();
		}
		if( devID != NULL)
			CoTaskMemFree(devID);
		pEnumerator->Release();
	}

	CoUninitialize();

	return devices;
}

const QList<audioDevice> WASAPISystem::mapToDevice(const QList<QPair<QString,QString>> devs, const QString &match, bool aFullUpdate) {
	QList<audioDevice> qlReturn;
	QListIterator<QPair<QString,QString>> i(devs);
	while (i.hasNext()) {
		QPair<QString,QString> pair = i.next();
		qlReturn.append(audioDevice(pair.second, pair.first));
	}
	return qlReturn;
}

WASAPIInput::WASAPIInput(QString aDeviceName, QVariant aDeviceUID) {
	m_deviceName = aDeviceName;
	m_deviceUID = aDeviceUID;
};

WASAPIInput::~WASAPIInput() {
	bRunning = false;
	wait();
}

void WASAPIInput::notifyInitEnd()
{
	HANDLE hEventFromSDK; 

	hEventFromSDK =	g.hInitWASAPIInputEvent;
	if (hEventFromSDK)
		SetEvent(hEventFromSDK);
}

static IMMDevice *openNamedOrDefaultDevice(const QString& name, EDataFlow dataFlow, ERole role, bool useDefault=true) {
	HRESULT hr;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&pEnumerator));
	if (!pEnumerator || FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_INSTATIATE_ENUMERATOR_FAILED), (LPARAM)(hr));
		qWarning("WASAPI: Failed to instantiate enumerator: hr=0x%08lx", hr);
		return NULL;
	}

	// Try to find a device pointer for |name|.
	if (!name.isEmpty()) {
		STACKVAR(wchar_t, devname, name.length() + 1);
		int len = name.toWCharArray(devname);
		devname[len] = 0;
		hr = pEnumerator->GetDevice(devname, &pDevice);
		if (FAILED(hr)) {
			if( dataFlow == eRender)
			{
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_OPEN_CHOSEN_DEVICE_FAILED), (LPARAM)(hr));
				qWarning("WASAPI: Failed to open selected output device %s %ls (df=%d, e=%d, hr=0x%08lx), falling back to default", qPrintable(name), devname, dataFlow, role, hr);	
			}
			else
			{
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_OPEN_CHOSEN_DEVICE_FAILED), (LPARAM)(hr));
				qWarning("WASAPI: Failed to open selected input device %s %ls (df=%d, e=%d, hr=0x%08lx), falling back to default", qPrintable(name), devname, dataFlow, role, hr);	
			}
		}
	}

	// Use the default device if |pDevice| is still NULL.
	// We retrieve the actual device name for the currently selected default device and
	// open the device by it's real name to work around triggering the automatic
	// ducking behavior.
	if (!pDevice && useDefault) {
		hr = pEnumerator->GetDefaultAudioEndpoint(dataFlow, role, &pDevice);
		if (FAILED(hr)) {
			if( dataFlow == eRender)
			{
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_OPEN_DEVICE_FAILED), (LPARAM)(hr));
				qWarning("WASAPI: Failed to open output device: df=%d, e=%d, hr=0x%08lx", dataFlow, role, hr);		
			}
			else
			{
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_OPEN_DEVICE_FAILED), (LPARAM)(hr));
				qWarning("WASAPI: Failed to open input device: df=%d, e=%d, hr=0x%08lx", dataFlow, role, hr);	
			}
			goto cleanup;
		}
		wchar_t *devname = NULL;
		hr = pDevice->GetId(&devname);
		if (FAILED(hr)) {
			if( dataFlow == eRender)
			{
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_QUERY_DEVICE_FAILED), (LPARAM)(hr));
				qWarning("WASAPI: Failed to query output device: df=%d, e=%d, hr=0x%08lx", dataFlow, role, hr);
			}
			else
			{
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_QUERY_DEVICE_FAILED), (LPARAM)(hr));
				qWarning("WASAPI: Failed to query input device: df=%d, e=%d, hr=0x%08lx", dataFlow, role, hr);
			}
			goto cleanup;
		}
		pDevice->Release();
		hr = pEnumerator->GetDevice(devname, &pDevice);
		if (FAILED(hr)) {
			if( dataFlow == eRender)
			{
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_REOPEN_DEFAULT_DEVICE_FAILED), (LPARAM)(hr));
				qWarning("WASAPI: Failed to reopen default output device: df=%d, e=%d, hr=0x%08lx", dataFlow, role, hr);
			}
			else
			{
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_REOPEN_DEFAULT_DEVICE_FAILED), (LPARAM)(hr));
				qWarning("WASAPI: Failed to reopen default input device: df=%d, e=%d, hr=0x%08lx", dataFlow, role, hr);
			}
			goto cleanup;
		}
		//WASAPINotificationClient::get().enlistDefaultDeviceAsUsed(devname);
		CoTaskMemFree(devname);
	}

cleanup:
	if (pEnumerator)
		pEnumerator->Release();

	return pDevice;
}

void WASAPIInput::runForCaptureSound()
{
	HRESULT hr;
	IMMDevice *pMicDevice = NULL;
	IAudioClient *pMicAudioClient = NULL;
	IAudioCaptureClient *pMicCaptureClient = NULL;
	WAVEFORMATEX *micpwfx = NULL;
	WAVEFORMATEXTENSIBLE *micpwfxe = NULL;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	UINT32 numFramesLeft;
	UINT32 micPacketLength = 0, echoPacketLength = 0;
	UINT32 allocLength;
	UINT64 devicePosition;
	UINT64 qpcPosition;
	HANDLE hEvent;
	BYTE *pData;
	DWORD flags;
	DWORD dwTaskIndex = 0;
	HANDLE hMmThread;
	float *tbuff = NULL;
	short *sbuff = NULL;
	bool doecho = g.s.doEcho();
	REFERENCE_TIME def, min, latency, want;
	bool exclusive = false;
	bool particleEcho = false;
	SharedMemoryStreamSound* outSound = NULL; 
	short* streamBuffer = NULL;

	CoInitialize(NULL);

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hMmThread = AvSetMmThreadCharacteristics(L"Pro Audio", &dwTaskIndex);
	if (hMmThread == NULL) {
		qWarning("WASAPIInput: Failed to set Pro Audio thread priority");
	}

	// Open mic device.
	if (!pMicDevice)
		pMicDevice = openNamedOrDefaultDevice(g.s.qsWASAPIOutput, eRender, eCommunications);
	if (!pMicDevice) {
		notifyInitEnd();
		goto cleanup;
	}
	updateDeviceFormFactor(pMicDevice);



	hr = pMicDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **) &pMicAudioClient);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_ACTIVATE_MIC_AUDIOCLIENT_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Activate Mic AudioClient failed: hr=0x%08lx", hr);
		notifyInitEnd();
		goto cleanup;
	}

	def = min = latency = 0;

	pMicAudioClient->GetDevicePeriod(&def, &min);

	want = qMax<REFERENCE_TIME>(min, 100000);
	qWarning("WASAPIInput: Latencies %lld %lld => %lld", def, min, want);


	hr = pMicAudioClient->GetMixFormat(&micpwfx);
	micpwfxe = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(micpwfx);

	if (FAILED(hr) || (micpwfx->wBitsPerSample != (sizeof(float) * 8)) || (micpwfxe->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_SUBFORMAT_IS_NOT_IEEE_FORMAT), (LPARAM)(hr));
		qWarning("WASAPIInput: Mic Subformat is not IEEE Float: hr=0x%08lx", hr);
		goto cleanup;
	}

	hr = pMicAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, micpwfx, NULL);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_MIC_INITIALIZE_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Mic Initialize failed: hr=0x%08lx", hr);
		if (hr == AUDCLNT_E_DEVICE_IN_USE)
			notifyInitEnd();
		goto cleanup;
	}

	pMicAudioClient->GetStreamLatency(&latency);

	hr = pMicAudioClient->GetBufferSize(&bufferFrameCount);
	qWarning("WASAPIInput: Stream Latency %lld (%d)", latency, bufferFrameCount);

	hr = pMicAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pMicCaptureClient);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_MIC_GETSERVICE_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Mic GetService failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	pMicAudioClient->SetEventHandle(hEvent);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_SET_MIC_EVENT_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Failed to set mic event: hr=0x%08lx", hr);
		goto cleanup;
	}

	hr = pMicAudioClient->Start();
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_START_MIC_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Failed to start mic: hr=0x%08lx", hr);
		goto cleanup;
	}

	iMicChannels = micpwfx->nChannels;
	iMicFreq = micpwfx->nSamplesPerSec;
	
	//если получение эха устройства провалилось берем его входящий звук мамбл
	//или сейчас захват звука
	if (g.s.doEcho()) {
		outSound = new SharedMemoryStreamSound(QLatin1String("vacademiaVoiceEcho"));
		iEchoFreq = SAMPLE_RATE;
		iEchoChannels = 1;
		particleEcho = true;
	}
	initializeMixer();

	allocLength = (iMicLength / 2) * micpwfx->nChannels;


	WASAPISystem::getInputDevices( false);

	notifyInitEnd();
	bool firstCycle = true;


	tbuff = new float[allocLength];
	streamBuffer =  new short[allocLength];
	bool createTwoFrameShift = true;

	while (bRunning && ! FAILED(hr)) {
		hr = pMicCaptureClient->GetNextPacketSize(&micPacketLength);
		if (FAILED(hr)) {
			qWarning("WASAPIInput: GetNextPacketSize failed: hr=0x%08lx", hr);
			if (g.hCallbackWnd)
				::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_GET_NEXT_PACKET_FAILED), (LPARAM)(hr));
			goto cleanup;
		}
		if (particleEcho && outSound) {
			echoPacketLength = outSound->GetAvailableSize() / 2;
		}
		
		while ((micPacketLength > 0) || (echoPacketLength > 0)) {
			if (echoPacketLength > 0) {
				if (particleEcho && outSound ) {
					if (echoPacketLength > allocLength) {
						delete [] tbuff;
						allocLength = echoPacketLength;
						tbuff = new float[echoPacketLength];

						delete [] streamBuffer;
						streamBuffer =  new short[echoPacketLength];
					}
					createTwoFrameShift = false;
					particleEchoCancellation(outSound, streamBuffer, tbuff, echoPacketLength, createTwoFrameShift);
					createTwoFrameShift = false;
				}
			} else if (micPacketLength > 0) {
				hr = pMicCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, &devicePosition, &qpcPosition);
				numFramesLeft = numFramesAvailable;
				if (FAILED(hr)) {
					qWarning("WASAPIInput: GetBuffer failed: hr=0x%08lx", hr);
					if (g.hCallbackWnd)
						::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_GET_NEXT_PACKET_FAILED), (LPARAM)(hr));
					goto cleanup;
				}

				UINT32 nFrames = numFramesAvailable * micpwfx->nChannels;
				if (nFrames > allocLength) {
					delete [] tbuff;
					allocLength = nFrames;
					tbuff = new float[allocLength];
				}
				memcpy(tbuff, pData, nFrames * sizeof(float));
				hr = pMicCaptureClient->ReleaseBuffer(numFramesAvailable);
				if (FAILED(hr)) {
					qWarning("WASAPIInput: ReleaseBuffer failed: hr=0x%08lx", hr);
					if (g.hCallbackWnd)
						::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_GET_NEXT_PACKET_FAILED), (LPARAM)(hr));
					goto cleanup;
				}
				addMic(tbuff, numFramesAvailable);
			}
			hr = pMicCaptureClient->GetNextPacketSize(&micPacketLength);
			if (! FAILED(hr) ) {
				if (particleEcho && outSound) {
					echoPacketLength = outSound->GetAvailableSize() / 2;
				} 
			}
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


cleanup:
	if (micpwfx && !exclusive)
		CoTaskMemFree(micpwfx);

	if (pMicAudioClient) {
		pMicAudioClient->Stop();
		pMicAudioClient->Release();
	}
	if (pMicCaptureClient)
		pMicCaptureClient->Release();
	if (pMicDevice)
		pMicDevice->Release();

	if (hMmThread != NULL)
		AvRevertMmThreadCharacteristics(hMmThread);

	if (hEvent != NULL)
		CloseHandle(hEvent);

	delete [] tbuff;
	delete [] sbuff;
	if (streamBuffer)
		delete[] streamBuffer;
}

void WASAPIInput::run() {
	if (g.s.bEchoForSystemMixer && !m_findMicMode) {
		qWarning("runForCaptureSound");
		runForCaptureSound();
	}
	else {
		qWarning("runForMic");
		runForMic();
	}
}

void WASAPIInput::runForMic() {
	HRESULT hr;
	IMMDevice *pMicDevice = NULL;
	IAudioClient *pMicAudioClient = NULL;
	IAudioCaptureClient *pMicCaptureClient = NULL;
	IMMDevice *pEchoDevice = NULL;
	IAudioClient *pEchoAudioClient = NULL;
	IAudioCaptureClient *pEchoCaptureClient = NULL;
	WAVEFORMATEX *micpwfx = NULL, *echopwfx = NULL;
	WAVEFORMATEXTENSIBLE *micpwfxe = NULL, *echopwfxe = NULL;
	WAVEFORMATEXTENSIBLE wfe;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	UINT32 numFramesLeft;
	UINT32 micPacketLength = 0, echoPacketLength = 0;
	UINT32 allocLength;
	UINT64 devicePosition;
	UINT64 qpcPosition;
	HANDLE hEvent;
	BYTE *pData;
	DWORD flags;
	DWORD dwTaskIndex = 0;
	HANDLE hMmThread;
	float *tbuff = NULL;
	short *sbuff = NULL;
	bool doecho = g.s.doEcho();
	REFERENCE_TIME def, min, latency, want;
	bool exclusive = false;
	bool particleEcho = false;
	SharedMemoryStreamSound* outSound = NULL; 
	short* streamBuffer = NULL;

	CoInitialize(NULL);

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hMmThread = AvSetMmThreadCharacteristics(L"Pro Audio", &dwTaskIndex);
	if (hMmThread == NULL) {
		qWarning("WASAPIInput: Failed to set Pro Audio thread priority");
	}

	if (!m_deviceUID.toString().isEmpty()) {
		pMicDevice = openNamedOrDefaultDevice(m_deviceUID.toString(), eCapture, eCommunications, false);
	}
	// Open mic device.
	if (!pMicDevice)
		pMicDevice = openNamedOrDefaultDevice(g.s.qsWASAPIInput, eCapture, eCommunications);
	if (!pMicDevice) {
		notifyInitEnd();
		goto cleanup;
	}
	updateDeviceFormFactor(pMicDevice);
	
	if (!m_deviceUID.toString().isEmpty() && m_findMicMode) {
		//при поиске самого громкого микрофона микшер пропускаем
		if (getDeviceFormFator() != Microphone) {
			notifyInitEnd();
			goto cleanup;
		}
	}
	qWarning() << "m_deviceName = " << m_deviceName;
	qWarning() << "m_deviceUID.toString() = " << m_deviceUID.toString();

	if (doecho) {
		pEchoDevice = openNamedOrDefaultDevice(g.s.qsWASAPIOutput, eRender, eCommunications);
		if (!pEchoDevice)
			doecho = false;
	}
	

	hr = pMicDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **) &pMicAudioClient);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_ACTIVATE_MIC_AUDIOCLIENT_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Activate Mic AudioClient failed: hr=0x%08lx", hr);
		notifyInitEnd();
		goto cleanup;
	}

	def = min = latency = 0;

	pMicAudioClient->GetDevicePeriod(&def, &min);

	want = qMax<REFERENCE_TIME>(min, 100000);
	qWarning("WASAPIInput: Latencies %lld %lld => %lld", def, min, want);

	if (g.s.bExclusiveInput && ! doecho) {
		for (int channels = 1; channels<=2; ++channels) {
			ZeroMemory(&wfe, sizeof(wfe));
			wfe.Format.cbSize = 0;
			wfe.Format.wFormatTag = WAVE_FORMAT_PCM;
			wfe.Format.nChannels = channels;
			wfe.Format.nSamplesPerSec = SAMPLE_RATE;
			wfe.Format.wBitsPerSample = 16;
			wfe.Format.nBlockAlign = wfe.Format.nChannels * wfe.Format.wBitsPerSample / 8;
			wfe.Format.nAvgBytesPerSec = wfe.Format.nBlockAlign * wfe.Format.nSamplesPerSec;

			micpwfxe = &wfe;
			micpwfx = reinterpret_cast<WAVEFORMATEX *>(&wfe);

			hr = pMicAudioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, want, want, micpwfx, NULL);
			if (SUCCEEDED(hr)) {
				eMicFormat = SampleShort;
				exclusive = true;
				qWarning("WASAPIInput: Successfully opened exclusive mode");
				break;
			}

			micpwfxe = NULL;
			micpwfx = NULL;
		}
	}

	if (!  micpwfxe) {
		if (g.s.bExclusiveInput)
			qWarning("WASAPIInput: Failed to open exclusive mode.");

		hr = pMicAudioClient->GetMixFormat(&micpwfx);
		micpwfxe = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(micpwfx);

		if (FAILED(hr) || (micpwfx->wBitsPerSample != (sizeof(float) * 8)) || (micpwfxe->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
			if (g.hCallbackWnd)
				::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_SUBFORMAT_IS_NOT_IEEE_FORMAT), (LPARAM)(hr));
			qWarning("WASAPIInput: Mic Subformat is not IEEE Float: hr=0x%08lx", hr);
			goto cleanup;
		}

		hr = pMicAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, micpwfx, NULL);
		if (FAILED(hr)) {
			if (g.hCallbackWnd)
				::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_MIC_INITIALIZE_FAILED), (LPARAM)(hr));
			qWarning("WASAPIInput: Mic Initialize failed: hr=0x%08lx", hr);
			if (hr == AUDCLNT_E_DEVICE_IN_USE)
				notifyInitEnd();
			goto cleanup;
		}
	}

	pMicAudioClient->GetStreamLatency(&latency);
	hr = pMicAudioClient->GetBufferSize(&bufferFrameCount);
	qWarning("WASAPIInput: Stream Latency %lld (%d)", latency, bufferFrameCount);

	hr = pMicAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pMicCaptureClient);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_MIC_GETSERVICE_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Mic GetService failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	pMicAudioClient->SetEventHandle(hEvent);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_SET_MIC_EVENT_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Failed to set mic event: hr=0x%08lx", hr);
		goto cleanup;
	}

	hr = pMicAudioClient->Start();
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_START_MIC_FAILED), (LPARAM)(hr));
		qWarning("WASAPIInput: Failed to start mic: hr=0x%08lx", hr);
		goto cleanup;
	}

	iMicChannels = micpwfx->nChannels;
	iMicFreq = micpwfx->nSamplesPerSec;
	bool echoError = false;

	if (doecho) {
		for (int i=0; i < 1; i++) {
			hr = pEchoDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **) &pEchoAudioClient);
			if (FAILED(hr)) {
				qWarning("WASAPIInput: Activate Echo AudioClient failed: hr=0x%08lx", hr);
				echoError = true;
				break;
			}

			hr = pEchoAudioClient->GetMixFormat(&echopwfx);
			echopwfxe = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(echopwfx);

			if (FAILED(hr) || (echopwfx->wBitsPerSample != (sizeof(float) * 8)) || (echopwfxe->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
				qWarning("WASAPIInput: Echo Subformat is not IEEE Float: hr=0x%08lx", hr);
				echoError = true;
				break;
			}

			hr = pEchoAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, echopwfx, NULL);
			if (FAILED(hr)) {
				qWarning("WASAPIInput: Echo Initialize failed: hr=0x%08lx", hr);
				echoError = true;
				break;
			}

			hr = pEchoAudioClient->GetBufferSize(&bufferFrameCount);
			hr = pEchoAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pEchoCaptureClient);
			if (FAILED(hr)) {
				qWarning("WASAPIInput: Echo GetService failed: hr=0x%08lx", hr);
				echoError = true;
				break;
			}

			pEchoAudioClient->SetEventHandle(hEvent);
			if (FAILED(hr)) {
				qWarning("WASAPIInput: Failed to set echo event: hr=0x%08lx", hr);
				echoError = true;
				break;
			}

			hr = pEchoAudioClient->Start();
			if (FAILED(hr)) {
				qWarning("WASAPIInput: Failed to start Echo: hr=0x%08lx", hr);
				echoError = true;
				break;
			}

			iEchoChannels = echopwfx->nChannels;
			iEchoFreq = echopwfx->nSamplesPerSec;
		} 
	}
	//сейчас захват звука давим частичное эхо
	if (g.s.bEchoForSystemMixer && g.s.doEcho())
		echoError = true;

	if (echoError) 
	{
		if (pEchoAudioClient) {
			pEchoAudioClient->Stop();
			pEchoAudioClient->Release();
		}
		if (pEchoCaptureClient)
			pEchoCaptureClient->Release();
		if (pEchoDevice)
			pEchoDevice->Release();

		pEchoAudioClient = NULL;
		pEchoCaptureClient = NULL;
		pEchoDevice = NULL;

		doecho = false;
		iEchoChannels = 0;
	}

	
	//если получение эха устройства провалилось берем его входящий звук мамбл
	//или сейчас захват звука
	if (!doecho && g.s.doEcho()) {
		outSound = new SharedMemoryStreamSound(QLatin1String("vacademiaVoiceEcho"));
		iEchoFreq = SAMPLE_RATE;
		iEchoChannels = 1;
		particleEcho = true;
	}
	initializeMixer();

	allocLength = (iMicLength / 2) * micpwfx->nChannels;


	WASAPISystem::getInputDevices( false);


	notifyInitEnd();
	bool firstCycle = true;

	if (exclusive) {
		sbuff = new short[allocLength];
		while (bRunning && ! FAILED(hr)) {
			hr = pMicCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, &devicePosition, &qpcPosition);
			if (hr != AUDCLNT_S_BUFFER_EMPTY) {
				if (FAILED(hr))
					goto cleanup;

				numFramesLeft = numFramesAvailable;

				UINT32 nFrames = numFramesAvailable * micpwfx->nChannels;
				if (nFrames > allocLength) {
					delete [] sbuff;
					allocLength = nFrames;
					sbuff = new short[allocLength];
				}

				memcpy(sbuff, pData, nFrames * sizeof(short));
				hr = pMicCaptureClient->ReleaseBuffer(numFramesAvailable);
				if (FAILED(hr))
					goto cleanup;
				addMic(sbuff, numFramesAvailable);
			}
			if (! FAILED(hr))
				WaitForSingleObject(hEvent, 100);
			bNotHang = true;
		}
	} else {
		tbuff = new float[allocLength];
		streamBuffer =  new short[allocLength];
		bool createTwoFrameShift = true;
		while (bRunning && ! FAILED(hr)) {
			hr = pMicCaptureClient->GetNextPacketSize(&micPacketLength);
			if (FAILED(hr)) {
				qWarning("WASAPIInput: GetNextPacketSize failed: hr=0x%08lx", hr);
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_GET_NEXT_PACKET_FAILED), (LPARAM)(hr));
				goto cleanup;
			}
			if (particleEcho && outSound) {
				echoPacketLength = outSound->GetAvailableSize() / 2;
			}
			else if (iEchoChannels) {
				hr = pEchoCaptureClient->GetNextPacketSize(&echoPacketLength);
				if (FAILED(hr)) {
					iEchoChannels = 0;
					echoPacketLength = 0;
					doecho = false;
				}
			}

			while ((micPacketLength > 0) || (echoPacketLength > 0)) {
				if (echoPacketLength > 0) {
					if (particleEcho && outSound ) {
						if (echoPacketLength > allocLength) {
							delete [] tbuff;
							allocLength = echoPacketLength;
							tbuff = new float[echoPacketLength];

							delete [] streamBuffer;
							streamBuffer =  new short[echoPacketLength];
						}
						createTwoFrameShift = false;
						particleEchoCancellation(outSound, streamBuffer, tbuff, echoPacketLength, createTwoFrameShift);
						createTwoFrameShift = false;
					}
					else {
						hr = pEchoCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, &devicePosition, &qpcPosition);
						numFramesLeft = numFramesAvailable;
						if (FAILED(hr)) {
							qWarning("WASAPIInput: GetBuffer failed: hr=0x%08lx", hr);
							echoPacketLength = 0;
							continue;
						}

						UINT32 nFrames = numFramesAvailable * echopwfx->nChannels;
						if (nFrames > allocLength) {
							delete [] tbuff;
							allocLength = nFrames;
							tbuff = new float[allocLength];
						}
						memcpy(tbuff, pData, nFrames * sizeof(float));
						hr = pEchoCaptureClient->ReleaseBuffer(numFramesAvailable);
						if (FAILED(hr)) {
							qWarning("WASAPIInput: ReleaseBuffer failed: hr=0x%08lx", hr);
							echoPacketLength = 0;
							continue;
						}

						addEcho(tbuff, numFramesAvailable, false);
					}
					
				} else if (micPacketLength > 0) {
					hr = pMicCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, &devicePosition, &qpcPosition);
					numFramesLeft = numFramesAvailable;
					if (FAILED(hr)) {
						qWarning("WASAPIInput: GetBuffer failed: hr=0x%08lx", hr);
						if (g.hCallbackWnd)
							::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_GET_NEXT_PACKET_FAILED), (LPARAM)(hr));
						goto cleanup;
					}

					UINT32 nFrames = numFramesAvailable * micpwfx->nChannels;
					if (nFrames > allocLength) {
						delete [] tbuff;
						allocLength = nFrames;
						tbuff = new float[allocLength];
					}
					memcpy(tbuff, pData, nFrames * sizeof(float));
					hr = pMicCaptureClient->ReleaseBuffer(numFramesAvailable);
					if (FAILED(hr)) {
						qWarning("WASAPIInput: ReleaseBuffer failed: hr=0x%08lx", hr);
						if (g.hCallbackWnd)
							::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_IN_GET_NEXT_PACKET_FAILED), (LPARAM)(hr));
						goto cleanup;
					}
					addMic(tbuff, numFramesAvailable);
				}
				hr = pMicCaptureClient->GetNextPacketSize(&micPacketLength);
				if (! FAILED(hr) ) {
					if (particleEcho && outSound) {
						echoPacketLength = outSound->GetAvailableSize() / 2;
					} 
					else if (iEchoChannels) {
						hr = pEchoCaptureClient->GetNextPacketSize(&echoPacketLength);
					}
				}
			}
			if (! FAILED(hr))
				WaitForSingleObject(hEvent, 2000);
			if (firstCycle) {
				firstCycle = false;
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(MIC_NO_ERROR), (LPARAM)(0));
			}
			bNotHang = true;
		}
	}

cleanup:
	if (micpwfx && !exclusive)
		CoTaskMemFree(micpwfx);
	if (echopwfx)
		CoTaskMemFree(echopwfx);

	if (pMicAudioClient) {
		pMicAudioClient->Stop();
		pMicAudioClient->Release();
	}
	if (pMicCaptureClient)
		pMicCaptureClient->Release();
	if (pMicDevice)
		pMicDevice->Release();

	if (pEchoAudioClient) {
		pEchoAudioClient->Stop();
		pEchoAudioClient->Release();
	}
	if (pEchoCaptureClient)
		pEchoCaptureClient->Release();
	if (pEchoDevice)
		pEchoDevice->Release();

	if (hMmThread != NULL)
		AvRevertMmThreadCharacteristics(hMmThread);

	if (hEvent != NULL)
		CloseHandle(hEvent);

	delete [] tbuff;
	delete [] sbuff;
	if (streamBuffer)
		delete[] streamBuffer;
}

void WASAPIInput::updateDeviceFormFactor(IMMDevice* pDevice)
{
	IPropertyStore *pStore = NULL;
	pDevice->OpenPropertyStore(STGM_READ, &pStore);
	if (!pStore)
		return;
	PROPVARIANT varFormFactor;
	PropVariantInit(&varFormFactor);
	HRESULT hr = pStore->GetValue(PKEY_AudioEndpoint_FormFactor, &varFormFactor);
	if( hr == S_OK)
	{
		m_formFactorUsedDevice = varFormFactor.uintVal;
	}
	PropVariantClear(&varFormFactor);
}

WASAPIOutput::WASAPIOutput() {
}

WASAPIOutput::~WASAPIOutput() {
	bRunning = false;
	wait();
}

void WASAPIOutput::setVolumes(IMMDevice *pDevice, bool talking) {
	HRESULT hr;

	if (! talking) {
		QMap<ISimpleAudioVolume *, VolumePair>::const_iterator i;
		for (i=qmVolumes.constBegin(); i != qmVolumes.constEnd(); ++i) {
			float fVolume = 1.0f;
			hr = i.key()->GetMasterVolume(&fVolume);
			if (qFuzzyCompare(i.value().second, fVolume))
				hr = i.key()->SetMasterVolume(i.value().first, NULL);
			i.key()->Release();
		}
		qmVolumes.clear();
		return;
	}

	IAudioSessionManager2 *pAudioSessionManager = NULL;
	int max = 0;
	DWORD dwMumble = GetCurrentProcessId();

	qmVolumes.clear();
	if (qFuzzyCompare(g.s.fOtherVolume, 1.0f))
		return;

	// FIXME: Try to keep the session object around when returning volume.

	if (SUCCEEDED(hr = pDevice->Activate(bIsWin7 ? __uuidof(IAudioSessionManager2) : __uuidof(IAudioSessionManager), CLSCTX_ALL, NULL, (void **) &pAudioSessionManager))) {
		IAudioSessionEnumerator *pEnumerator = NULL;
		IAudioSessionQuery *pMysticQuery = NULL;
		if (! bIsWin7) {
			if (SUCCEEDED(hr = pAudioSessionManager->QueryInterface(__uuidof(IAudioSessionQuery), (void **) &pMysticQuery))) {
				hr = pMysticQuery->GetQueryInterface(&pEnumerator);
			}
		} else {
			hr = pAudioSessionManager->GetSessionEnumerator(&pEnumerator);
		}

		QSet<QUuid> seen;

		if (SUCCEEDED(hr)) {
			if (SUCCEEDED(hr = pEnumerator->GetCount(&max))) {
				for (int i=0;i<max;++i) {
					IAudioSessionControl *pControl = NULL;
					if (SUCCEEDED(hr = pEnumerator->GetSession(i, &pControl))) {
						setVolumeForSessionControl(pControl, dwMumble, seen);
						pControl->Release();
					}
				}
			}
			pEnumerator->Release();
		}
		if (pMysticQuery)
			pMysticQuery->Release();
		pAudioSessionManager->Release();
	}
}

bool WASAPIOutput::setVolumeForSessionControl2(IAudioSessionControl2 *control2, const DWORD mumblePID, QSet<QUuid> &seen) {
	HRESULT hr;
	DWORD pid;
	
	// Don't set the volume for our own control
	if (FAILED(hr = control2->GetProcessId(&pid)) || (pid == mumblePID))
		return true;
	
	// Don't work on expired audio sessions
	AudioSessionState ass;
	if (FAILED(hr = control2->GetState(&ass)) || (ass == AudioSessionStateExpired))
		return false;
	
	// Don't act twice on the same session
	GUID group;
	if (FAILED(hr = control2->GetGroupingParam(&group)))
		return false;
	
	QUuid quuid(group);
	if (seen.contains(quuid))
		return true;
	
	seen.insert(quuid);
	
	// Adjust volume
	ISimpleAudioVolume *pVolume = NULL;
	if (FAILED(hr = control2->QueryInterface(__uuidof(ISimpleAudioVolume), (void **) &pVolume)))
		return false;
	
	BOOL bMute = TRUE;
	bool keep = false;
	if (SUCCEEDED(hr = pVolume->GetMute(&bMute)) && ! bMute) {
		float fVolume = 1.0f;
		if (SUCCEEDED(hr = pVolume->GetMasterVolume(&fVolume)) && ! qFuzzyCompare(fVolume,0.0f)) {
			float fSetVolume = fVolume * g.s.fOtherVolume;
			if (SUCCEEDED(hr = pVolume->SetMasterVolume(fSetVolume, NULL))) {
				hr = pVolume->GetMasterVolume(&fSetVolume);
				qmVolumes.insert(pVolume, VolumePair(fVolume,fSetVolume));
				keep = true;
			}
		}
	}
	
	if (! keep)
		pVolume->Release();
	
	return true;
}

bool WASAPIOutput::setVolumeForSessionControl(IAudioSessionControl *control, const DWORD mumblePID, QSet<QUuid> &seen) {
	HRESULT hr;
	IAudioSessionControl2 *pControl2 = NULL;

	if (!SUCCEEDED(hr = control->QueryInterface(bIsWin7 ? __uuidof(IAudioSessionControl2) : __uuidof(IVistaAudioSessionControl2), (void **) &pControl2)))
		return false;
	
	bool result = setVolumeForSessionControl2(pControl2, mumblePID, seen);
	
	pControl2->Release();
	return result;
}

static void SetDuckingOptOut(IMMDevice *pDevice) {
	if (!bIsWin7)
		return;

	HRESULT hr;
	IAudioSessionManager2 *pSessionManager2 = NULL;
	IAudioSessionControl *pSessionControl = NULL;
	IAudioSessionControl2 *pSessionControl2 = NULL;

	// Get session manager & control1+2 to disable ducking
	hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&pSessionManager2));
	if (FAILED(hr)) {
		qWarning("WASAPIOutput: Activate AudioSessionManager2 failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	hr = pSessionManager2->GetAudioSessionControl(NULL, 0, &pSessionControl);
	if (FAILED(hr)) {
		qWarning("WASAPIOutput: GetAudioSessionControl failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&pSessionControl2));
	if (FAILED(hr)) {
		qWarning("WASAPIOutput: Querying SessionControl2 failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	hr = pSessionControl2->SetDuckingPreference(TRUE);
	if (FAILED(hr)) {
		qWarning("WASAPIOutput: Failed to set ducking preference: hr=0x%08lx", hr);
		goto cleanup;
	}

cleanup:
	if (pSessionControl2)
		pSessionControl2->Release();

	if (pSessionControl)
		pSessionControl->Release();

	if (pSessionManager2)
		pSessionManager2->Release();
}

void WASAPIOutput::notifyInitEnd()
{
	HANDLE hEventFromSDK; 
	hEventFromSDK =	g.hInitWASAPIOutputEvent;
	if (hEventFromSDK)
		SetEvent(hEventFromSDK);
}

void WASAPIOutput::run() {
	HRESULT hr;
	IMMDevice *pDevice = NULL;
	IAudioClient *pAudioClient = NULL;
	IAudioRenderClient *pRenderClient = NULL;
	WAVEFORMATEX *pwfx = NULL;
	WAVEFORMATEXTENSIBLE *pwfxe = NULL;
	UINT32 bufferFrameCount;
	REFERENCE_TIME def, min, latency, want;
	UINT32 numFramesAvailable;
	UINT32 wantLength;
	HANDLE hEvent;
	BYTE *pData;
	DWORD dwTaskIndex = 0;
	HANDLE hMmThread;
	int ns = 0;
	unsigned int chanmasks[32];
	QMap<DWORD, float> qmVolumes;
	bool lastspoke = false;
	REFERENCE_TIME bufferDuration = (g.s.iOutputDelay > 1) ? (g.s.iOutputDelay + 1) * 100000 : 0;
	bool exclusive = false;

	CoInitialize(NULL);

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hMmThread = AvSetMmThreadCharacteristics(L"Pro Audio", &dwTaskIndex);
	if (hMmThread == NULL) {
		qWarning("WASAPIOutput: Failed to set Pro Audio thread priority");
	}

// Open the output device.
	pDevice = openNamedOrDefaultDevice(g.s.qsWASAPIOutput, eRender, eMultimedia);
	if (!pDevice)
	{
		notifyInitEnd();
		goto cleanup;
	}

	// Opt-out of the Windows 7 ducking behavior
	SetDuckingOptOut(pDevice);

	hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **) &pAudioClient);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_ACTIVATE_AUDIOCLIENT_FAILED), (LPARAM)(hr));
		notifyInitEnd();
		qWarning("WASAPIOutput: Activate AudioClient failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	pAudioClient->GetDevicePeriod(&def, &min);
	want = qMax<REFERENCE_TIME>(min, 100000);
	qWarning("WASAPIOutput: Latencies %lld %lld => %lld", def, min, want);

	if (g.s.bExclusiveOutput) {
		hr = pAudioClient->GetMixFormat(&pwfx);
		if (FAILED(hr)) {
			if (g.hCallbackWnd)
				::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_GETMIXFORMAT_FAILED), (LPARAM)(hr));
			qWarning("WASAPIOutput: GetMixFormat failed: hr=0x%08lx", hr);
			goto cleanup;
		}

		pwfxe = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pwfx);

		if (!g.s.bPositionalAudio) {
			pwfx->nChannels = 2;
			pwfxe->dwChannelMask = KSAUDIO_SPEAKER_STEREO;
		}

		pwfx->cbSize = 0;
		pwfx->wFormatTag = WAVE_FORMAT_PCM;
		pwfx->nSamplesPerSec = SAMPLE_RATE;
		pwfx->wBitsPerSample = 16;
		pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
		pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

		pwfxe = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pwfx);

		hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, want, want, pwfx, NULL);
		if (SUCCEEDED(hr)) {
			eSampleFormat = SampleShort;
			exclusive = true;
			qWarning("WASAPIOutput: Successfully opened exclusive mode");
		} else {
			CoTaskMemFree(pwfx);

			pwfxe = NULL;
			pwfx = NULL;
		}
	}

	if (!  pwfxe) {
		if (g.s.bExclusiveOutput)
			qWarning("WASAPIOutput: Failed to open exclusive mode.");

		hr = pAudioClient->GetMixFormat(&pwfx);
		if (FAILED(hr)) {
			if (g.hCallbackWnd)
				::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_GETMIXFORMAT_FAILED), (LPARAM)(hr));
			qWarning("WASAPIOutput: GetMixFormat failed: hr=0x%08lx", hr);
			goto cleanup;
		}
		pwfxe = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pwfx);

		if (FAILED(hr) || (pwfx->wBitsPerSample != (sizeof(float) * 8)) || (pwfxe->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
			if (g.hCallbackWnd)
				::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_SUBFORMAT_IS_NOT_IEEE_FORMAT), (LPARAM)(hr));
			qWarning("WASAPIOutput: Subformat is not IEEE Float: hr=0x%08lx", hr);
			goto cleanup;
		}

		if (!g.s.bPositionalAudio) {
			pwfxe->Format.nChannels = 2;
			pwfxe->Format.nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
			pwfxe->Format.nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
			pwfxe->dwChannelMask = KSAUDIO_SPEAKER_STEREO;

			WAVEFORMATEX *closestFormat = NULL;
			hr = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwfx, &closestFormat);
			if (hr == S_FALSE) {
				qWarning("WASAPIOutput: Driver says no to 2 channel output. Closest format: %d channels @ %d kHz", closestFormat->nChannels, closestFormat->nSamplesPerSec);
				CoTaskMemFree(pwfx);

				pwfx = NULL;
				pAudioClient->GetMixFormat(&pwfx);
				pwfxe = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pwfx);
			} else if (FAILED(hr)) {
				qWarning("WASAPIOutput: IsFormatSupported failed: hr=0x%08lx", hr);
			}

			if (closestFormat)
				CoTaskMemFree(closestFormat);
		}

		hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, bufferDuration, 0, pwfx, NULL);
		if (FAILED(hr)) {
			if (g.hCallbackWnd)
				::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_MIC_INITIALIZE_FAILED), (LPARAM)(hr));
			qWarning("WASAPIOutput: Initialize failed: hr=0x%08lx", hr);
			if (hr == AUDCLNT_E_DEVICE_IN_USE)
				notifyInitEnd();

			goto cleanup;
		}
	}

	pAudioClient->GetStreamLatency(&latency);
	pAudioClient->GetBufferSize(&bufferFrameCount);
	qWarning("WASAPIOutput: Stream Latency %lld (%d)", latency, bufferFrameCount);

	iMixerFreq = pwfx->nSamplesPerSec;

	qWarning("WASAPIOutput: Periods %lldus %lldus (latency %lldus)", def / 10LL, min / 10LL, latency / 10LL);
	qWarning("WASAPIOutput: Buffer is %dus (%d)", (bufferFrameCount * 1000000) / iMixerFreq, g.s.iOutputDelay);

	hr = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pRenderClient);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_GETSERVICE_FAILED), (LPARAM)(hr));
		qWarning("WASAPIOutput: GetService failed: hr=0x%08lx", hr);
		goto cleanup;
	}

	wantLength = lroundf(ceilf((iFrameSize * pwfx->nSamplesPerSec) / (SAMPLE_RATE * 1.0f)));

	pAudioClient->SetEventHandle(hEvent);
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_SET_EVENT_FAILED), (LPARAM)(hr));
		qWarning("WASAPIOutput: Failed to set event: hr=0x%08lx", hr);
		goto cleanup;
	}

	hr = pAudioClient->Start();
	if (FAILED(hr)) {
		if (g.hCallbackWnd)
			::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_START_FAILED), (LPARAM)(hr));
		qWarning("WASAPIOutput: Failed to start: hr=0x%08lx", hr);
		goto cleanup;
	}

	for (int i=0;i<32;i++) {
		if (pwfxe->dwChannelMask & (1 << i)) {
			chanmasks[ns++] = 1 << i;
		}
	}
	if (ns != pwfx->nChannels) {
		qWarning("WASAPIOutput: Chanmask bits doesn't match number of channels.");
	}

	iChannels = pwfx->nChannels;
	initializeMixer(chanmasks);

	WASAPISystem::getOutputDevices( false);

	notifyInitEnd();

	bool mixed = false;
	if (! exclusive) {
		while (bRunning && ! FAILED(hr)) {
			hr = pAudioClient->GetCurrentPadding(&numFramesAvailable);
			if (FAILED(hr)) {
				if (g.hCallbackWnd)
					::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_GET_GETCURRENTPADDING_FAILED), (LPARAM)(hr));
				goto cleanup;
			}

			UINT32 packetLength = bufferFrameCount - numFramesAvailable;

			if (lastspoke != (g.bAttenuateOthers || mixed)) {
				lastspoke = g.bAttenuateOthers || mixed;
				setVolumes(pDevice, lastspoke);
			}

			while (packetLength > 0) {
				hr = pRenderClient->GetBuffer(packetLength, &pData);
				if (FAILED(hr)) {
					if (g.hCallbackWnd)
						::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_GET_GETBUFFER_FAILED), (LPARAM)(hr));
					goto cleanup;
				}

				mixed = mix(reinterpret_cast<float *>(pData), packetLength);
				if (mixed)
					hr = pRenderClient->ReleaseBuffer(packetLength, 0);
				else
					hr = pRenderClient->ReleaseBuffer(packetLength, AUDCLNT_BUFFERFLAGS_SILENT);
				if (FAILED(hr)) {
					if (g.hCallbackWnd)
						::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_GET_GETBUFFER_FAILED), (LPARAM)(hr));
					goto cleanup;
				}

				if (!g.s.bAttenuateOthers && !g.bAttenuateOthers) {
					mixed = false;
				}

				if (lastspoke != (g.bAttenuateOthers || mixed)) {
					lastspoke = g.bAttenuateOthers || mixed;
					setVolumes(pDevice, lastspoke);
				}

				hr = pAudioClient->GetCurrentPadding(&numFramesAvailable);
				if (FAILED(hr)) {
					if (g.hCallbackWnd)
						::PostMessage(g.hCallbackWnd, WM_MUMBLE_ERRORCODE, (WPARAM)(WASAPI_OUT_GET_GETCURRENTPADDING_FAILED), (LPARAM)(hr));
					goto cleanup;
				}

				packetLength = bufferFrameCount - numFramesAvailable;
			}
			if (! FAILED(hr))
				WaitForSingleObject(hEvent, 2000);
			bNotHang = true;
		}
	} else {
		wantLength = bufferFrameCount;

		while (bRunning && ! FAILED(hr)) {
			hr = pRenderClient->GetBuffer(wantLength, &pData);
			if (FAILED(hr))
				goto cleanup;

			mixed = mix(pData, wantLength);

			if (mixed)
				hr = pRenderClient->ReleaseBuffer(wantLength, 0);
			else
				hr = pRenderClient->ReleaseBuffer(wantLength, AUDCLNT_BUFFERFLAGS_SILENT);

			if (FAILED(hr))
				goto cleanup;

			if (! FAILED(hr))
				WaitForSingleObject(hEvent, 100);
			bNotHang = true;
		}
	}

cleanup:
	if (pwfx)
		CoTaskMemFree(pwfx);

	setVolumes(pDevice, false);

	if (pAudioClient) {
		pAudioClient->Stop();
		pAudioClient->Release();
	}
	if (pRenderClient)
		pRenderClient->Release();
	if (pDevice)
		pDevice->Release();

	if (hMmThread != NULL)
		AvRevertMmThreadCharacteristics(hMmThread);

	if (hEvent != NULL)
		CloseHandle(hEvent);
}
