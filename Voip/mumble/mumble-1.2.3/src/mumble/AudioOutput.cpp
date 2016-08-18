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
#include "AudioOutput.h"
#include "AudioInput.h"
#include "AudioOutputSpeech.h"
#include "User.h"
#include "Global.h"
#include "Message.h"
#include "Plugins.h"
#include "PacketDataStream.h"
#include "ServerHandler.h"
#include "../SDKInterface/MumbleWndMessage.h"
#ifdef USE_VOICE_REC
#include "VoiceRecorder.h"
#endif
#include "./AudioInFromStream/SharedMemoryStreamSound.h"

// Remember that we cannot use static member classes that are not pointers, as the constructor
// for AudioOutputRegistrar() might be called before they are initialized, as the constructor
// is called from global initialization.
// Hence, we allocate upon first call.

QMap<QString, AudioOutputRegistrar *> *AudioOutputRegistrar::qmNew;
QString AudioOutputRegistrar::current = QString();

AudioOutputRegistrar::AudioOutputRegistrar(const QString &n, int p) : name(n), priority(p) {
	if (! qmNew)
		qmNew = new QMap<QString, AudioOutputRegistrar *>();
	qmNew->insert(name,this);
}

AudioOutputRegistrar::~AudioOutputRegistrar() {
	qmNew->remove(name);
}

AudioOutputPtr AudioOutputRegistrar::newFromChoice(QString choice) {
	if (! qmNew)
		return AudioOutputPtr();

	if (!choice.isEmpty() && qmNew->contains(choice)) {
		g.s.qsAudioOutput = choice;
		current = choice;
		return AudioOutputPtr(qmNew->value(choice)->create());
	}
	choice = g.s.qsAudioOutput;
	if (qmNew->contains(choice)) {
		current = choice;
		return AudioOutputPtr(qmNew->value(choice)->create());
	}

	AudioOutputRegistrar *r = NULL;
	foreach(AudioOutputRegistrar *aor, *qmNew)
		if (!r || (aor->priority > r->priority))
			r = aor;
	if (r) {
		current = r->name;
		return AudioOutputPtr(r->create());
	}
	return AudioOutputPtr();
}

bool AudioOutputRegistrar::canMuteOthers() const {
	return false;
}

bool AudioOutputRegistrar::usesOutputDelay() const {
	return true;
}

bool AudioOutputRegistrar::canExclusive() const {
	return false;
}

#ifdef USE_VOICE_REC
SoundFile::SoundFile(const QString &fname) {
	siInfo.frames = 0 ;
	siInfo.channels = 1 ;
	siInfo.samplerate = SAMPLE_RATE ;
	siInfo.samplerate = 0 ;
	siInfo.sections = 0 ;
	siInfo.seekable = 0 ;

	sfFile = NULL;

	qfFile.setFileName(fname);

	if (qfFile.open(QIODevice::ReadOnly)) {
		static SF_VIRTUAL_IO svi = {&SoundFile::vio_get_filelen, &SoundFile::vio_seek, &SoundFile::vio_read, &SoundFile::vio_write, &SoundFile::vio_tell};
#ifdef USE_VOICE_REC
		sfFile = sf_open_virtual(&svi, SFM_READ, &siInfo, this) ;
#endif
	}
}

SoundFile::~SoundFile() {
#ifdef USE_VOICE_REC
	if (sfFile)
		sf_close(sfFile);
#endif
}

bool SoundFile::isOpen() const {
	return (sfFile != NULL) && qfFile.isOpen();
}

int SoundFile::channels() const {
	return siInfo.channels;
}

int SoundFile::samplerate() const {
	return siInfo.samplerate;
}

int SoundFile::error() const {
#ifdef USE_VOICE_REC
	return sf_error(sfFile);
#else
	return 0;
#endif
}

QString SoundFile::strError() const {
#ifdef USE_VOICE_REC
	return QLatin1String(sf_strerror(sfFile));
#else
	return QString();
#endif
}

sf_count_t SoundFile::seek(sf_count_t frames, int whence) {
#ifdef USE_VOICE_REC
	return sf_seek(sfFile, frames, whence);
#else
	return 0;
#endif
}

sf_count_t SoundFile::read(float *ptr, sf_count_t items) {
#ifdef USE_VOICE_REC
	return sf_read_float(sfFile, ptr, items);
#else
	return 0;
#endif
}

sf_count_t SoundFile::vio_get_filelen(void *user_data) {
	SoundFile *sf = reinterpret_cast<SoundFile *>(user_data);

	if (! sf->qfFile.isOpen())
		return -1;

	return (sf->qfFile.size());
}

sf_count_t SoundFile::vio_seek(sf_count_t offset, int whence, void *user_data) {
	SoundFile *sf = reinterpret_cast<SoundFile *>(user_data);

	if (! sf->qfFile.isOpen())
		return -1;

	if (whence == SEEK_SET) {
		sf->qfFile.seek(offset);
	} else if (whence == SEEK_END) {
		sf->qfFile.seek(sf->qfFile.size() - offset);
	} else {
		sf->qfFile.seek(sf->qfFile.pos() + offset);
	}
	return sf->qfFile.pos();
}

sf_count_t SoundFile::vio_read(void *ptr, sf_count_t count, void *user_data) {
	SoundFile *sf = reinterpret_cast<SoundFile *>(user_data);

	if (! sf->qfFile.isOpen())
		return -1;

	return sf->qfFile.read(reinterpret_cast<char *>(ptr), count);
}

sf_count_t SoundFile::vio_write(const void *ptr, sf_count_t count, void *user_data) {
	SoundFile *sf = reinterpret_cast<SoundFile *>(user_data);

	if (! sf->qfFile.isOpen())
		return -1;

	return sf->qfFile.write(reinterpret_cast<const char *>(ptr), count);
}

sf_count_t SoundFile::vio_tell(void *user_data) {
	SoundFile *sf = reinterpret_cast<SoundFile *>(user_data);

	if (! sf->qfFile.isOpen())
		return -1;

	return sf->qfFile.pos();
}
#endif

#ifdef USE_VOICE_REC
AudioOutputSample::AudioOutputSample(const QString &name, SoundFile *psndfile, bool loop, unsigned int freq) : AudioOutputUser(name) {
	int err;

	sfHandle = psndfile;
	iOutSampleRate = freq;

	// Check if the file is good
	if (sfHandle->channels() <= 0 || sfHandle->channels() > 2) {
		sfHandle = NULL;
		return;
	}

	/* qWarning() << "Channels: " << sfHandle->channels();
	qWarning() << "Samplerate: " << sfHandle->samplerate();
	qWarning() << "Target Sr.: " << freq;
	qWarning() << "Format: " << sfHandle->format() << endl; */

	// If the frequencies don't match initialize the resampler
	if (sfHandle->samplerate() != static_cast<int>(freq)) {
		srs = speex_resampler_init(1, sfHandle->samplerate(), iOutSampleRate, 3, &err);
		if (err != RESAMPLER_ERR_SUCCESS) {
			qWarning() << "Initialize " << sfHandle->samplerate() << " to " << iOutSampleRate << " resampler failed!";
			srs = NULL;
			sfHandle = NULL;
			return;
		}
	} else {
		srs = NULL;
	}

	iLastConsume = iBufferFilled = 0;
	bLoop = loop;
	bEof = false;
}

AudioOutputSample::~AudioOutputSample() {
	if (srs)
		speex_resampler_destroy(srs);

	delete sfHandle;
	sfHandle = NULL;
}

SoundFile* AudioOutputSample::loadSndfile(const QString &filename) {
	SoundFile *sf;

	// Create the filehandle and do a quick check if everything is ok
	sf = new SoundFile(filename);

	if (! sf->isOpen()) {
		qWarning() << "File " << filename << " failed to open";
		delete sf;
		return NULL;
	}

	if (sf->error() != SF_ERR_NO_ERROR) {
		qWarning() << "File " << filename << " couldn't be loaded: " << sf->strError();
		delete sf;
		return NULL;
	}

	if (sf->channels() <= 0 || sf->channels() > 2) {
		qWarning() << "File " << filename << " contains " << sf->channels() << " Channels, only 1 or 2 are supported.";
		delete sf;
		return NULL;
	}
	return sf;
}

QString AudioOutputSample::browseForSndfile() {
	SoundFile *sf = NULL;
	QString file = QFileDialog::getOpenFileName(NULL, tr("Choose sound file"), QString(), QLatin1String("*.wav *.ogg *.ogv *.oga *.flac"));
	if (! file.isEmpty()) {
		if ((sf = AudioOutputSample::loadSndfile(file)) == NULL) {
			QMessageBox::critical(NULL,
			                      tr("Invalid sound file"),
			                      tr("The file '%1' cannot be used by Mumble. Please select a file with a compatible format and encoding.").arg(file));
			return QString();
		}
		delete sf;
	}
	return file;
}

bool AudioOutputSample::needSamples(unsigned int snum) {
	// Forward the buffer
	for (unsigned int i=iLastConsume;i<iBufferFilled;++i)
		pfBuffer[i-iLastConsume]=pfBuffer[i];
	iBufferFilled -= iLastConsume;
	iLastConsume = snum;


	// Check if we can satisfy request with current buffer
	if (iBufferFilled >= snum)
		return true;

	// Calculate the required buffersize to hold the results
	unsigned int iInputFrames = static_cast<unsigned int>(ceilf(static_cast<float>(snum * sfHandle->samplerate()) / static_cast<float>(iOutSampleRate)));
	unsigned int iInputSamples = iInputFrames * sfHandle->channels();

	float *pOut;
	bool mix = sfHandle->channels() > 1;
	STACKVAR(float, fOut, iInputSamples);
	STACKVAR(float, fMix, iInputFrames);

	bool eof = false;
	sf_count_t read;
	do {
		resizeBuffer(iBufferFilled + snum);

		// If we need to resample or mix write to the buffer on stack
		pOut = (srs || mix) ? fOut : pfBuffer + iBufferFilled;

		// Try to read all samples needed to satifsy this request
		if ((read = sfHandle->read(pOut, iInputSamples)) < iInputSamples) {
			if (sfHandle->error() != SF_ERR_NO_ERROR || !bLoop) {
				// We reached the eof or encountered an error, stuff with zeroes
				memset(pOut, 0, sizeof(float) * (iInputSamples - read));
				read = iInputSamples;
				eof = true;
			} else {
				sfHandle->seek(SEEK_SET, 0);
			}
		}

		if (mix) { // Mix the channels (only two channels)
			read /= 2;
			// If we need to resample after this write to extra buffer
			pOut = srs ? fMix : pfBuffer + iBufferFilled;
			for (unsigned int i = 0; i < read; i++)
				pOut[i] = (fOut[i*2] + fOut[i*2 + 1]) * 0.5f;

		}

		spx_uint32_t inlen = static_cast<unsigned int>(read);
		spx_uint32_t outlen = snum;
		if (srs) // If necessary resample
			speex_resampler_process_float(srs, 0, pOut, &inlen, pfBuffer + iBufferFilled, &outlen);

		iBufferFilled += outlen;
	} while (iBufferFilled < snum);

	if (eof && !bEof) {
		emit playbackFinished();
		bEof = true;
	}

	return !eof;
}
#endif

AudioOutput::AudioOutput() {
	QThread::setStackSize(1024*1024);
	iFrameSize = SAMPLE_RATE / 100;
	bRunning = true;
	bNotHang = false;
	bSuccessMix = false;

	iChannels = 0;
	fSpeakers = NULL;
	fSpeakerVolume = NULL;
	bSpeakerPositional = NULL;

	iMixerFreq = 0;
	eSampleFormat = SampleFloat;
	iSampleSize = 0;

	m_sharedMemoryStreamSound = new SharedMemoryStreamSound(QLatin1String("vacademiaVoiceOut"));
	m_sharedMemoryStreamSound->CreateCaptureBuffer(iFrameSize * sizeof(short) * 500);

	m_sharedEchoStreamSound = new SharedMemoryStreamSound(QLatin1String("vacademiaVoiceEcho"));
	m_sharedEchoStreamSound->CreateCaptureBuffer(iFrameSize * sizeof(short) * 1);
	m_sharedResampler = NULL;
}

AudioOutput::~AudioOutput() {
	bRunning = false;
	wait();
	wipe();

	delete [] fSpeakers;
	delete [] fSpeakerVolume;
	delete [] bSpeakerPositional;

	delete m_sharedMemoryStreamSound;
	delete m_sharedEchoStreamSound;

	if (m_sharedResampler)
		speex_resampler_destroy(m_sharedResampler);
}

// Here's the theory.
// We support sound "bloom"ing. That is, if sound comes directly from the left, if it is sufficiently
// close, we'll hear it full intensity from the left side, and "bloom" intensity from the right side.

float AudioOutput::calcGain(float dotproduct, float distance) {

	float dotfactor = (dotproduct + 1.0f) / 2.0f;
	float att;


	// No distance attenuation
	if (g.s.fAudioMaxDistVolume > 0.99f) {
		att = qMin(1.0f, dotfactor + g.s.fAudioBloom);
	} else if (distance < g.s.fAudioMinDistance) {
		float bloomfac = g.s.fAudioBloom * (1.0f - distance/g.s.fAudioMinDistance);

		att = qMin(1.0f, bloomfac + dotfactor);
	} else {
		float datt;

		if (distance >= g.s.fAudioMaxDistance) {
			datt = g.s.fAudioMaxDistVolume;
		} else {
			float mvol = g.s.fAudioMaxDistVolume;
			if (mvol < 0.01f)
				mvol = 0.01f;

			float drel = (distance-g.s.fAudioMinDistance) / (g.s.fAudioMaxDistance - g.s.fAudioMinDistance);
			datt = powf(10.0f, log10f(mvol) * drel);
		}

		att = datt * dotfactor;
	}
	return att;
}

void AudioOutput::wipe() {
	foreach(AudioOutputUser *aop, qmOutputs)
		removeBuffer(aop);
}

const float *AudioOutput::getSpeakerPos(unsigned int &speakers) {
	if ((iChannels > 0) && fSpeakers) {
		speakers = iChannels;
		return fSpeakers;
	}
	return NULL;
}
#ifdef USE_VOICE_REC
AudioOutputSample *AudioOutput::playSample(const QString &filename, bool loop) {
	SoundFile *handle;
	handle = AudioOutputSample::loadSndfile(filename);
	if (handle == NULL)
		return NULL;

	while ((iMixerFreq == 0) && isAlive()) {
#if QT_VERSION >= 0x040500
		QThread::yieldCurrentThread();
#endif
	}

	if (! iMixerFreq)
		return NULL;

	qrwlOutputs.lockForWrite();
	AudioOutputSample *aos = new AudioOutputSample(filename, handle, loop, iMixerFreq);
	qmOutputs.insert(NULL, aos);
	qrwlOutputs.unlock();

	return aos;

}
#endif
void AudioOutput::addFrameToBuffer(ClientUser *user, const QByteArray &qbaPacket, unsigned int iSeq, MessageHandler::UDPMessageType type) {
	if (iChannels == 0)
		return;
	qrwlOutputs.lockForRead();
	AudioOutputSpeech *aop = dynamic_cast<AudioOutputSpeech *>(qmOutputs.value(user));

	if (! aop || (aop->umtType != type)) {
		qrwlOutputs.unlock();

		if (aop)
			removeBuffer(aop);

		while ((iMixerFreq == 0) && isAlive()) {
#if QT_VERSION >= 0x040500
			QThread::yieldCurrentThread();
#endif
		}

		if (! iMixerFreq)
			return;

		qrwlOutputs.lockForWrite();
		aop = new AudioOutputSpeech(user, iMixerFreq, type);
		qmOutputs.replace(user, aop);
	}

	aop->addFrameToBuffer(qbaPacket, iSeq);

	qrwlOutputs.unlock();
}

void AudioOutput::removeBuffer(const ClientUser *user) {
	removeBuffer(qmOutputs.value(user));
}

void AudioOutput::removeBuffer(AudioOutputUser *aop) {
	QWriteLocker locker(&qrwlOutputs);
	QMultiHash<const ClientUser *, AudioOutputUser *>::iterator i;
	for (i=qmOutputs.begin(); i != qmOutputs.end(); ++i) {
		if (i.value() == aop) {
			qmOutputs.erase(i);
			delete aop;
			break;
		}
	}
}

void AudioOutput::initializeMixer(const unsigned int *chanmasks, bool forceheadphone) {
	delete fSpeakers;
	delete bSpeakerPositional;
	delete fSpeakerVolume;

	fSpeakers = new float[iChannels * 3];
	bSpeakerPositional = new bool[iChannels];
	fSpeakerVolume = new float[iChannels];

	memset(fSpeakers, 0, sizeof(float) * iChannels * 3);
	memset(bSpeakerPositional, 0, sizeof(bool) * iChannels);

	for (unsigned int i=0;i<iChannels;++i)
		fSpeakerVolume[i] = 1.0f;

	if (g.s.bPositionalAudio && (iChannels > 1)) {
		for (unsigned int i=0;i<iChannels;i++) {
			float *s = &fSpeakers[3*i];
			bSpeakerPositional[i] = true;

			switch (chanmasks[i]) {
				case SPEAKER_FRONT_LEFT:
					s[0] = -0.5f;
					s[2] = 1.0f;
					break;
				case SPEAKER_FRONT_RIGHT:
					s[0] = 0.5f;
					s[2] = 1.0f;
					break;
				case SPEAKER_FRONT_CENTER:
					s[2] = 1.0f;
					break;
				case SPEAKER_LOW_FREQUENCY:
					break;
				case SPEAKER_BACK_LEFT:
					s[0] = -0.5f;
					s[2] = -1.0f;
					break;
				case SPEAKER_BACK_RIGHT:
					s[0] = 0.5f;
					s[2] = -1.0f;
					break;
				case SPEAKER_FRONT_LEFT_OF_CENTER:
					s[0] = -0.25;
					s[2] = 1.0f;
					break;
				case SPEAKER_FRONT_RIGHT_OF_CENTER:
					s[0] = 0.25;
					s[2] = 1.0f;
					break;
				case SPEAKER_BACK_CENTER:
					s[2] = -1.0f;
					break;
				case SPEAKER_SIDE_LEFT:
					s[0] = -1.0f;
					break;
				case SPEAKER_SIDE_RIGHT:
					s[0] = 1.0f;
					break;
				case SPEAKER_TOP_CENTER:
					s[1] = 1.0f;
					s[2] = 1.0f;
					break;
				case SPEAKER_TOP_FRONT_LEFT:
					s[0] = -0.5f;
					s[1] = 1.0f;
					s[2] = 1.0f;
					break;
				case SPEAKER_TOP_FRONT_CENTER:
					s[1] = 1.0f;
					s[2] = 1.0f;
					break;
				case SPEAKER_TOP_FRONT_RIGHT:
					s[0] = 0.5f;
					s[1] = 1.0f;
					s[2] = 1.0f;
					break;
				case SPEAKER_TOP_BACK_LEFT:
					s[0] = -0.5f;
					s[1] = 1.0f;
					s[2] = -1.0f;
					break;
				case SPEAKER_TOP_BACK_CENTER:
					s[1] = 1.0f;
					s[2] = -1.0f;
					break;
				case SPEAKER_TOP_BACK_RIGHT:
					s[0] = 0.5f;
					s[1] = 1.0f;
					s[2] = -1.0f;
					break;
				default:
					bSpeakerPositional[i] = false;
					fSpeakerVolume[i] = 0.0f;
					qWarning("AudioOutput: Unknown speaker %d: %08x", i, chanmasks[i]);
					break;
			}
			if (g.s.bPositionalHeadphone || forceheadphone) {
				s[1] = 0.0f;
				s[2] = 0.0f;
				if (s[0] == 0.0f)
					fSpeakerVolume[i] = 0.0f;
			}
		}
		for (unsigned int i=0;i<iChannels;i++) {
			float d = sqrtf(fSpeakers[3*i+0]*fSpeakers[3*i+0] + fSpeakers[3*i+1]*fSpeakers[3*i+1] + fSpeakers[3*i+2]*fSpeakers[3*i+2]);
			if (d > 0.0f) {
				fSpeakers[3*i+0] /= d;
				fSpeakers[3*i+1] /= d;
				fSpeakers[3*i+2] /= d;
			}
		}
	}
	iSampleSize = static_cast<int>(iChannels * ((eSampleFormat == SampleFloat) ? sizeof(float) : sizeof(short)));
	qWarning("AudioOutput: Initialized %d channel %d hz mixer", iChannels, iMixerFreq);
}

bool AudioOutput::mix(void *outbuff, unsigned int nsamp) {
	bool res = mixFromDebug(outbuff, nsamp);

	bSuccessMix = res;
	if (!bSuccessMix)
	{
		short* sampleInShort = new short[nsamp];
		memset(sampleInShort, 0, nsamp*sizeof(short));
		m_sharedEchoStreamSound->AddData((byte*)sampleInShort, nsamp*sizeof(short));
		delete[] sampleInShort;
	}
	return res;
}

bool AudioOutput::mixFromDebug(void *outbuff, unsigned int nsamp) {
	QList<AudioOutputUser *> qlMix;
	QList<AudioOutputUser *> qlDel; 

	if (g.s.fVolume < 0.01 && !g.pTalkingCallback2)
		return false;

	const float adjustFactor = std::pow(10, -18.f / 20);
	const float mul = g.s.fVolume;
	const unsigned int nchan = iChannels;
#ifdef USE_VOICE_REC
	ServerHandlerPtr sh = g.sh;
	VoiceRecorderPtr recorder;
	if (sh)
		recorder = g.sh->recorder;
#endif
	qrwlOutputs.lockForRead();
	bool needAdjustment = false;
	QMultiHash<const ClientUser *, AudioOutputUser *>::const_iterator it = qmOutputs.constBegin();
	while (it != qmOutputs.constEnd()) {
		AudioOutputUser *aop = it.value();
		if (! aop->needSamples(nsamp)) {
			qlDel.append(aop);
		} else {
			qlMix.append(aop);
			// Set a flag if there is a priority speaker
			if (it.key() && it.key()->bPrioritySpeaker)
				needAdjustment = true;
		}
		++it;
	} 

	if (! qlMix.isEmpty()) {
		STACKVAR(float, speaker, iChannels*3);
		STACKVAR(float, svol, iChannels);

		STACKVAR(float, fOutput, iChannels * nsamp);
		float *output = (eSampleFormat == SampleFloat) ? reinterpret_cast<float *>(outbuff) : fOutput;
		bool validListener = false;

		memset(output, 0, sizeof(float) * nsamp * iChannels);

		float *mixOutputForShared = new float[nsamp/**2*/];
		memset(mixOutputForShared, 0, sizeof(float) * nsamp/* * 2*/);
#ifdef USE_VOICE_REC
		boost::shared_array<float> recbuff;
		if (recorder) {
			recbuff = boost::shared_array<float>(new float[nsamp]);
			memset(recbuff.get(), 0, sizeof(float) * nsamp);
		}
#endif
		for (unsigned int i=0;i<iChannels;++i)
			svol[i] = mul * fSpeakerVolume[i];

		if (g.s.bPositionalAudio && (iChannels > 1) && g.p->fetch() && (g.bPosTest || g.p->fCameraPosition[0] != 0 || g.p->fCameraPosition[1] != 0 || g.p->fCameraPosition[2] != 0)) {

			float front[3] = { g.p->fCameraFront[0], g.p->fCameraFront[1], g.p->fCameraFront[2] };
			float top[3] = { g.p->fCameraTop[0], g.p->fCameraTop[1], g.p->fCameraTop[2] };

			// Front vector is dominant; if it's zero we presume all is zero.

			float flen = sqrtf(front[0]*front[0]+front[1]*front[1]+front[2]*front[2]);

			if (flen > 0.0f) {
				front[0] *= (1.0f / flen);
				front[1] *= (1.0f / flen);
				front[2] *= (1.0f / flen);

				float tlen = sqrtf(top[0]*top[0]+top[1]*top[1]+top[2]*top[2]);

				if (tlen > 0.0f) {
					top[0] *= (1.0f / tlen);
					top[1] *= (1.0f / tlen);
					top[2] *= (1.0f / tlen);
				} else {
					top[0] = 0.0f;
					top[1] = 1.0f;
					top[2] = 0.0f;
				}

				if (std::abs<float>(front[0] * top[0] + front[1] * top[1] + front[2] * top[2]) > 0.01f) {
					// Not perpendicular. Assume Y up and rotate 90 degrees.

					float azimuth = 0.0f;
					if ((front[0] != 0.0f) || (front[2] != 0.0f))
						azimuth = atan2f(front[2], front[0]);
					float inclination = acosf(front[1]) - static_cast<float>(M_PI) / 2.0f;

					top[0] = sinf(inclination)*cosf(azimuth);
					top[1] = cosf(inclination);
					top[2] = sinf(inclination)*sinf(azimuth);
				}
			} else {
				front[0] = 0.0f;
				front[1] = 0.0f;
				front[2] = 1.0f;

				top[0] = 0.0f;
				top[1] = 1.0f;
				top[2] = 0.0f;
			}

			// Calculate right vector as front X top
			float right[3] = {top[1]*front[2] - top[2]*front[1], top[2]*front[0] - top[0]*front[2], top[0]*front[1] - top[1] * front[0] };

			/*
						qWarning("Front: %f %f %f", front[0], front[1], front[2]);
						qWarning("Top: %f %f %f", top[0], top[1], top[2]);
						qWarning("Right: %f %f %f", right[0], right[1], right[2]);
			*/
			// Rotate speakers to match orientation
			for (unsigned int i=0;i<iChannels;++i) {
				speaker[3*i+0] = fSpeakers[3*i+0] * right[0] + fSpeakers[3*i+1] * top[0] + fSpeakers[3*i+2] * front[0];
				speaker[3*i+1] = fSpeakers[3*i+0] * right[1] + fSpeakers[3*i+1] * top[1] + fSpeakers[3*i+2] * front[1];
				speaker[3*i+2] = fSpeakers[3*i+0] * right[2] + fSpeakers[3*i+1] * top[2] + fSpeakers[3*i+2] * front[2];
			}
			validListener = true;
		}

		foreach(AudioOutputUser *aop, qlMix) {
			const float * RESTRICT pfBuffer = aop->pfBuffer;
			float volumeAdjustment = 1;

			// We have at least one priority speaker
			if (needAdjustment) {
				AudioOutputSpeech *aos = qobject_cast<AudioOutputSpeech *>(aop);
				// Exclude whispering people
				if (aos && (aos->p->tsState == Settings::Talking || aos->p->tsState == Settings::Shouting)) {
					// Adjust all non-priority speakers
					if (!aos->p->bPrioritySpeaker)
						volumeAdjustment = adjustFactor;
				}
			}			

			if (g.pTalkingCallback2)
			{
				AudioOutputSpeech *aos = qobject_cast<AudioOutputSpeech *>(aop);

				if (aos && aos->p) {
					short* recbuff = (short*)::CoTaskMemAlloc(sizeof(short)*nsamp);
					memset(recbuff, 0, sizeof(short) * nsamp);

					for (unsigned int i = 0; i < nsamp; ++i) {
						float adjBuf = pfBuffer[i] * 1;
						
						if (adjBuf != 0) {
							recbuff[i] = static_cast<short>(32768.f * (adjBuf < -1.0f ? -1.0f : (adjBuf > 1.0f ? 1.0f : adjBuf)));
						}
					}
					
					(((TalkingCallback2*)g.pTalkingCallback2))((int)(aos->p->uiSession), (int)SAMPLE_RATE, (void*)recbuff, nsamp, g.pCallbackUserData);
					
				}				
			}		
			else 
			{				
				double modeCurrentTopInputLevel = 3000;
				CVolumeTalkingController::GetVolumeController()->AddUserVolume( (QString)aop->qsName, (void*)pfBuffer, false, nsamp, modeCurrentTopInputLevel);
			}
			////
			
			for (unsigned int sharedChan=0; sharedChan<2; sharedChan++) 
			{
				for (unsigned int i=0; i<nsamp; i++)
				{
					mixOutputForShared[i] += pfBuffer[i];
				}
			}

#ifdef USE_VOICE_REC
			if (recorder) {
				AudioOutputSpeech *aos = qobject_cast<AudioOutputSpeech *>(aop);

				if (aos) {
					for (unsigned int i = 0; i < nsamp; ++i) {
						recbuff[i] += pfBuffer[i] * volumeAdjustment;
					}

					if (!recorder->getMixDown()) {
						if (aos) {
							recorder->addBuffer(aos->p, recbuff, nsamp);
						} else {
							// this should be unreachable
							Q_ASSERT(false);
						}
						recbuff = boost::shared_array<float>(new float[nsamp]);
						memset(recbuff.get(), 0, sizeof(float) * nsamp);
					}

					// Don't add the local audio to the real output
					if (qobject_cast<RecordUser *>(aos->p)) {
						continue;
					}
				}
			}
#endif
			if (validListener && ((aop->fPos[0] != 0.0f) || (aop->fPos[1] != 0.0f) || (aop->fPos[2] != 0.0f))) {
				float dir[3] = { aop->fPos[0] - g.p->fCameraPosition[0], aop->fPos[1] - g.p->fCameraPosition[1], aop->fPos[2] - g.p->fCameraPosition[2] };
				float len = sqrtf(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
				if (len > 0.0f) {
					dir[0] /= len;
					dir[1] /= len;
					dir[2] /= len;
				}
				/*
								qWarning("Voice pos: %f %f %f", aop->fPos[0], aop->fPos[1], aop->fPos[2]);
								qWarning("Voice dir: %f %f %f", dir[0], dir[1], dir[2]);
				*/
				if (! aop->pfVolume) {
					aop->pfVolume = new float[nchan];
					for (unsigned int s=0;s<nchan;++s)
						aop->pfVolume[s] = -1.0;
				}
				for (unsigned int s=0;s<nchan;++s) {
					float * RESTRICT o = output + s;
					//qWarning(QString(QString::number(g.s.fAudioMaxListenDistance) + QLatin1String(" - ") + QString::number(len)).toStdString().c_str());
					if (g.s.fAudioMaxListenDistance == 0.0f || len <= g.s.fAudioMaxListenDistance) {
						const float dot = bSpeakerPositional[s] ? dir[0] * speaker[s*3+0] + dir[1] * speaker[s*3+1] + dir[2] * speaker[s*3+2] : 1.0f;
						const float str = svol[s] * calcGain(dot, len) * volumeAdjustment;
						const float old = (aop->pfVolume[s] >= 0.0f) ? aop->pfVolume[s] : str;
						const float inc = (str - old) / static_cast<float>(nsamp);
						aop->pfVolume[s] = str;
						/*
											qWarning("%d: Pos %f %f %f : Dot %f Len %f Str %f", s, speaker[s*3+0], speaker[s*3+1], speaker[s*3+2], dot, len, str);
						*/
						if ((old >= 0.00000001f) || (str >= 0.00000001f))
							for (unsigned int i=0;i<nsamp;++i)
								o[i*nchan] += pfBuffer[i] * (old + inc*static_cast<float>(i));
					}
					else { // above g.s.fAudioMaxListenDistance distance mute sound another user
						for (unsigned int i=0;i<nsamp;++i)
							o[i*nchan] = 0;
					}

				}
			} else {
				for (unsigned int s=0;s<nchan;++s) {
					const float str = svol[s] * volumeAdjustment;
					float * RESTRICT o = output + s;
					for (unsigned int i=0;i<nsamp;++i)
						o[i*nchan] += pfBuffer[i] * str;
				}
			}
		}
#ifdef USE_VOICE_REC
		if (recorder && recorder->getMixDown()) {
			recorder->addBuffer(NULL, recbuff, nsamp);
		}
#endif
		// Clip
		if (eSampleFormat == SampleFloat) {
			for (unsigned int i=0;i<nsamp*iChannels;i++) {
				output[i] = output[i] < -1.0f ? -1.0f : (output[i] > 1.0f ? 1.0f : output[i]);
			}
		}
		else {
			for (unsigned int i=0;i<nsamp*iChannels;i++) {
				reinterpret_cast<short *>(outbuff)[i] = static_cast<short>(32768.f * (output[i] < -1.0f ? -1.0f : (output[i] > 1.0f ? 1.0f : output[i])));
			}
		}
		

		if (m_sharedResampler == NULL && iMixerFreq != SAMPLE_RATE)
		{
			int error = 0;
			m_sharedResampler = speex_resampler_init(1, iMixerFreq, SAMPLE_RATE, 6, &error);
		}

		spx_uint32_t out_sample_count = 0;
		float* outBuffer = NULL;
		if (m_sharedResampler)
		{
			out_sample_count = int(((float)SAMPLE_RATE / iMixerFreq) * nsamp + 0.5f) + 1;
			
			outBuffer = new float[out_sample_count];
			spx_uint32_t inSampleCnt = nsamp;

			speex_resampler_process_float(m_sharedResampler, 0, mixOutputForShared, &inSampleCnt, outBuffer, &out_sample_count);
		}
		else
		{
			out_sample_count = nsamp;
			outBuffer = mixOutputForShared;
		}

		short* sampleInShort = new short[out_sample_count/*2*/];

		for (unsigned int i=0; i < out_sample_count/**2*/; i++)
		{
			float sampleValue = outBuffer[i];
			sampleInShort[i] = static_cast<short>(32768.f * (sampleValue < -1.0f ? -1.0f : (sampleValue > 1.0f ? 1.0f : sampleValue)));
		}
		m_sharedMemoryStreamSound->AddData((byte*)sampleInShort, out_sample_count*sizeof(short));
		m_sharedEchoStreamSound->AddData((byte*)sampleInShort, out_sample_count*sizeof(short));

		delete [] sampleInShort;
		delete [] mixOutputForShared;
		if (m_sharedResampler)
			delete [] outBuffer;
	}

	qrwlOutputs.unlock();

	foreach(AudioOutputUser *aop, qlDel)
		removeBuffer(aop);

	return (! qlMix.isEmpty());
}

bool AudioOutput::isAlive() const {
	return isRunning();
}

unsigned int AudioOutput::getMixerFreq() const {
	return iMixerFreq;
}

bool AudioOutput::getNotHungFlag() const {
	return bNotHang;
}

void AudioOutput::clearNotHungFlag(){
	bNotHang = false;
}

bool AudioOutput::getSuccessMixFlag() const {
	return bSuccessMix;
}

