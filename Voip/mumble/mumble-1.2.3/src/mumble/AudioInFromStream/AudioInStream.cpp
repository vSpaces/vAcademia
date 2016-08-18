#include "mumble_pch.hpp"

#include "AudioInStream.h"
#include "Global.h"
#include "AudioOutputSpeech.h"
#include "../../SDKInterface/MumbleWndMessage.h"
#include "Timer.h"

class SSAudioOutputRegistrar : public AudioOutputRegistrar {
public:
	SSAudioOutputRegistrar();
	virtual AudioOutput *create();
	virtual const QList<audioDevice> getDeviceChoices( bool aForce);
	virtual void setDeviceChoice(const QVariant &, Settings &);

};

SSAudioOutputRegistrar::SSAudioOutputRegistrar() : AudioOutputRegistrar(QLatin1String("SoundStream")) {
}

AudioOutput *SSAudioOutputRegistrar::create() {
	return new AudioOutStream();
}


const QList<audioDevice> SSAudioOutputRegistrar::getDeviceChoices( bool aForce) {
	QList<audioDevice> qlReturn;

	return qlReturn;
}

void SSAudioOutputRegistrar::setDeviceChoice(const QVariant &choice, Settings &s) {
}

class SSAudioInputRegistrar : public AudioInputRegistrar {
public:
	SSAudioInputRegistrar();
	virtual AudioInput *create();
	virtual AudioInput *create(QString aDeviceName, QVariant aDeviceUID);
	virtual const QList<audioDevice> getDeviceChoices( bool aForce);
	virtual void setDeviceChoice(const QVariant &, Settings &);
	virtual bool canEcho(const QString &) const;

};

SSAudioInputRegistrar::SSAudioInputRegistrar() : AudioInputRegistrar(QLatin1String("SoundStream")) {
}

AudioInput *SSAudioInputRegistrar::create() {
	return new AudioInStream();
}
AudioInput *SSAudioInputRegistrar::create(QString aDeviceName, QVariant aDeviceUID) {
	return new AudioInStream();
}

const QList<audioDevice> SSAudioInputRegistrar::getDeviceChoices( bool aForce) {
	QList<audioDevice> qlReturn;
	return qlReturn;
}

void SSAudioInputRegistrar::setDeviceChoice(const QVariant &choice, Settings &s) {
	
}

bool SSAudioInputRegistrar::canEcho(const QString &) const {
	return false;
}


class StreamSoundInit : public DeferInit {
	SSAudioInputRegistrar *airReg;
	SSAudioOutputRegistrar *aorReg; 
public:
	void initialize();
	void destroy();
};

static StreamSoundInit dsinit;

void StreamSoundInit::initialize() {
	airReg = NULL;
	aorReg = NULL;
	airReg = new SSAudioInputRegistrar();
	aorReg = new SSAudioOutputRegistrar();
}

void StreamSoundInit::destroy() {
	delete airReg;
	delete aorReg;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

AudioInStream::AudioInStream(void)
{
}

AudioInStream::~AudioInStream(void)
{
	bRunning = false;
	wait();
}

#define NBUFFBLOCKS 2000

void AudioInStream::run() {
	int dwBufferSize;
	
	int dwReadyBytes = 0;
	int dwLastReadPos = 0;
	int dwReadPosition;
	int dwCapturePosition;

	dwBufferSize = iFrameSize * sizeof(short) * NBUFFBLOCKS;

	LPVOID aptr1, aptr2;
	int nbytes1, nbytes2;

	HRESULT hr;
	quint64 timeStepsBeforeSleep = 0;
	unsigned int sampleTime = iFrameSize*1000 / (SAMPLE_RATE);
	Timer timer;
	timer.restart();
	
	int sendPacketWithoutSleep = 0;
	bool resetTime = true;
	while (bRunning) {
		g.streamSound_lock();
		std::vector<StreamSound*> copyOfStreamSoundBuffers = g.pStreamSoundBuffers;
		g.streamSound_unlock(); //получили копию буферов

//идея в том, что на каждого пользователя от имени которого играется файл - свой буфер

		std::vector<short *> arrOfMic; //массив данных для на frame от каждого буфера
		std::vector<StreamSound*> enoughDataStreamSoundBuffers; //буферы с достаточным кол-м данных
		if (copyOfStreamSoundBuffers.size() == 0)
		{
			msleep(50);
			continue;
		}
		for (unsigned int i=0; i < copyOfStreamSoundBuffers.size(); i++)
		{
			//пробегаем по буферам и выделяем память если нужно
			if (!copyOfStreamSoundBuffers[i]->IsCaptureBufferCreated())
			{
				copyOfStreamSoundBuffers[i]->CreateCaptureBuffer(dwBufferSize);
			}
		}

		bool haveEnoughForCapture = false;
		do { 
			//пробегаемся по буферам и выбираем те, в  которых достаточно данных
			for (unsigned int i=0; i < copyOfStreamSoundBuffers.size(); i++)
			{
				if (FAILED(hr = copyOfStreamSoundBuffers[i]->GetCurrentPosition(&dwCapturePosition, &dwReadPosition))) {
					//get from StreamSound
					qWarning("AudioStreamInput: GetCurrentPosition failed");
					continue;
				}
				if (dwReadPosition < copyOfStreamSoundBuffers[i]->GetLastreadPos())
					dwReadyBytes = (dwBufferSize - copyOfStreamSoundBuffers[i]->GetLastreadPos()) + dwReadPosition;
				else
					dwReadyBytes = dwReadPosition - copyOfStreamSoundBuffers[i]->GetLastreadPos();

				if (static_cast<int>(dwReadyBytes) >= (int)sizeof(short) * iFrameSize) {
					enoughDataStreamSoundBuffers.push_back(copyOfStreamSoundBuffers[i]);
					haveEnoughForCapture = true;				
				}
			}
			if (!haveEnoughForCapture)
			{
				break;
			}
			
		} while (!haveEnoughForCapture);

		if (!haveEnoughForCapture)
		{
			msleep(5);
			continue;
		}
	
		//пробегаемся по буферам с достаточным объемом данных и забираем эти данные в arrOfMic
		for (unsigned int i=0; i < enoughDataStreamSoundBuffers.size(); i++)
		{
			StreamSound* streamSound = enoughDataStreamSoundBuffers[i];
			if (bRunning) {
				if (FAILED(hr = streamSound->Lock(streamSound->GetLastreadPos(), sizeof(short) * iFrameSize, &aptr1, &nbytes1, &aptr2, &nbytes2))) {
					//qWarning("AudioStreamInput: Lock failed");
					continue;
				}

				short *psMicLocal = new short[iFrameSize]; 

				if (aptr1 && nbytes1)
					CopyMemory(psMicLocal, aptr1, nbytes1);

				if (aptr2 && nbytes2)
					CopyMemory(psMicLocal+nbytes1/2, aptr2, nbytes2);

				arrOfMic.push_back(psMicLocal);

				if (FAILED(hr = streamSound->Unlock(aptr1, nbytes1, aptr2, nbytes2))) {
					qWarning("AudioStreamInput: Unlock failed");
					continue;
				}

				streamSound->SetLastReadPos((streamSound->GetLastreadPos() + sizeof(short) * iFrameSize) % dwBufferSize);
			}
			
		}
		if (bRunning)
		{
			if (arrOfMic.size() > 0)
				CopyMemory(psMic, arrOfMic[0], sizeof(short) * iFrameSize);
			
			//микшируем данные из arrOfMic в psMic
			if (arrOfMic.size() > 1)
			{
				for (int j =0 ; j < iFrameSize; j++)
				{
					long avr = psMic[j];
					for (unsigned int i=1; i<arrOfMic.size(); i++ )
					{
						avr += arrOfMic[i][j];						
					//	div++;
					}
					psMic[j] = static_cast<short>(avr < -32768 ? -32768 : (avr > 32767 ? 32767 : avr));
				}
			}
			for (unsigned int i=0; i<arrOfMic.size(); i++ )
			{
				delete[] arrOfMic[i];
			}

			//кодируем и отправляем звук
			if (arrOfMic.size() > 0) {
				encodeAudioFrame();
				sendPacketWithoutSleep++;
			}

			if (arrOfMic.size() == 0) {
				//Lock не отдал данные возможно идет прелоадинг или стоим на паузе
				msleep(5);
			}

			arrOfMic.clear();
		}
		
		//отправляем N пакетов без слипа, затем спим время N самплов
		if (sendPacketWithoutSleep >= 30)
		{
			timeStepsBeforeSleep = timer.elapsed();

			timer.restart();
			quint64 waitTime = 0;

			if (timeStepsBeforeSleep < sampleTime*(sendPacketWithoutSleep)*1000)
				waitTime = sampleTime*(sendPacketWithoutSleep)*1000-timeStepsBeforeSleep;
			sendPacketWithoutSleep = 0;

			while (timer.elapsed() < waitTime)
			{
				//if (waitTime - timer.elapsed() > 2000)
					msleep(1);//при воспроизведении точность расчета в timer 1мс думаю не страшно если задержа между сериями пакетов будет чуть больше
				//else //msleep(1) может выполняться и больше
				//	msleep(0);
			}

			timeStepsBeforeSleep = 0;
			timer.restart();
		}

	}
}


AudioOutStream::AudioOutStream(void)
{
}

AudioOutStream::~AudioOutStream(void)
{
	bRunning = false;
	wait();
}

void AudioOutStream::run() {
	AudioOutputUser *aop;

	iMixerFreq = SAMPLE_RATE;
	iChannels = 1;
	eSampleFormat = SampleShort;

	quint64 timeTick = 0;
	unsigned int nsamp = iFrameSize;

	unsigned int sampleTime = iFrameSize*1000 / (SAMPLE_RATE);

	Timer timer;
	timer.restart();

	while (bRunning ) {
		QList<AudioOutputUser *> qlMix;
		QList<AudioOutputUser *> qlDel;
		qrwlOutputs.lockForRead();
		bool needAdjustment = false;
		QMultiHash<const ClientUser *, AudioOutputUser *>::const_iterator i = qmOutputs.constBegin();

		while (i != qmOutputs.constEnd()) {
			AudioOutputUser *aop = i.value();
			if (! aop->needSamples(nsamp)) {
				qlDel.append(aop);
			} else {
				qlMix.append(aop);
				// Set a flag if there is a priority speaker
				if (i.key() && i.key()->bPrioritySpeaker)
					needAdjustment = true;
			}
			++i;
		}

		bool noData = false;
		if (! qlMix.isEmpty()) {
			foreach(aop, qlMix) {
				const float * RESTRICT pfBuffer = aop->pfBuffer;

				if (g.pTalkingCallback2)
				{
					AudioOutputSpeech *aos = qobject_cast<AudioOutputSpeech *>(aop);

					 
					if (aos && aos->p) {
						short* recbuff = (short*)::CoTaskMemAlloc(sizeof(short)*nsamp);
						memset(recbuff, 0, sizeof(short) * nsamp);
						bool isZeroArr = true;
						
						for (unsigned int i = 0; i < nsamp; ++i) {
							float adjBuf = pfBuffer[i] * 1;
							
							if (adjBuf != 0) {
								isZeroArr = false;
								recbuff[i] = static_cast<short>(32768.f * (adjBuf < -1.0f ? -1.0f : (adjBuf > 1.0f ? 1.0f : adjBuf)));
							}
						}
						(((TalkingCallback2*)g.pTalkingCallback2))((int)(aos->p->uiSession), (int)SAMPLE_RATE, (void*)recbuff, nsamp, g.pCallbackUserData);

					}				
				}
							
			}
		}
		else {
			noData = true;
		}


		qrwlOutputs.unlock();
		
		foreach(aop, qlDel)
			removeBuffer(aop);

		timeTick = timer.elapsed();

		timer.restart();
		quint64 waitTime = 0;

		if (timeTick < sampleTime*1000)
			waitTime = sampleTime*1000-timeTick;

		while (timer.elapsed() < waitTime)
		{
			if (noData || !g.pTalkingCallback2)
				msleep(1);
			else
			{
				if (waitTime - timer.elapsed() > 2000)
					msleep(1);
				else //msleep(1) может выполняться и больше
					msleep(0);
			}
		}
		timer.restart();
}
}