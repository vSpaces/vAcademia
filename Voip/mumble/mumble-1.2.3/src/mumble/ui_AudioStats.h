/********************************************************************************
** Form generated from reading UI file 'AudioStats.ui'
**
** Created: Fri 1. Jun 10:05:08 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_AUDIOSTATS_H
#define UI_AUDIOSTATS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "AudioStats.h"

QT_BEGIN_NAMESPACE

class Ui_AudioStats
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QGroupBox *qgbInput;
    QGridLayout *gridLayout;
    QLabel *qliMicLevel;
    QLabel *qlMicLevel;
    QLabel *qliSpeakerLevel;
    QLabel *qlSpeakerLevel;
    QLabel *qliSignalLevel;
    QLabel *qlSignalLevel;
    QGroupBox *qgbSignal;
    QGridLayout *gridLayout1;
    QLabel *qliMicVolume;
    QLabel *qlMicVolume;
    QLabel *qliMicSNR;
    QLabel *qlMicSNR;
    QLabel *qliSpeechProb;
    QLabel *qlSpeechProb;
    QGroupBox *qgbConfiguration;
    QGridLayout *gridLayout2;
    QLabel *qliBitrate;
    QLabel *qlBitrate;
    QLabel *qliDoublePush;
    QLabel *qlDoublePush;
    QLabel *qliSpeech;
    AudioBar *abSpeech;
    QSpacerItem *spacerItem;
    QGroupBox *qgbSpectrum;
    QVBoxLayout *vboxLayout1;
    AudioNoiseWidget *anwNoise;
    QGroupBox *qgbEcho;
    QVBoxLayout *vboxLayout2;
    AudioEchoWidget *aewEcho;

    void setupUi(QDialog *AudioStats)
    {
        if (AudioStats->objectName().isEmpty())
            AudioStats->setObjectName(QString::fromUtf8("AudioStats"));
        AudioStats->resize(598, 548);
        vboxLayout = new QVBoxLayout(AudioStats);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        qgbInput = new QGroupBox(AudioStats);
        qgbInput->setObjectName(QString::fromUtf8("qgbInput"));
        gridLayout = new QGridLayout(qgbInput);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qliMicLevel = new QLabel(qgbInput);
        qliMicLevel->setObjectName(QString::fromUtf8("qliMicLevel"));

        gridLayout->addWidget(qliMicLevel, 0, 0, 1, 1);

        qlMicLevel = new QLabel(qgbInput);
        qlMicLevel->setObjectName(QString::fromUtf8("qlMicLevel"));

        gridLayout->addWidget(qlMicLevel, 0, 1, 1, 1);

        qliSpeakerLevel = new QLabel(qgbInput);
        qliSpeakerLevel->setObjectName(QString::fromUtf8("qliSpeakerLevel"));

        gridLayout->addWidget(qliSpeakerLevel, 1, 0, 1, 1);

        qlSpeakerLevel = new QLabel(qgbInput);
        qlSpeakerLevel->setObjectName(QString::fromUtf8("qlSpeakerLevel"));

        gridLayout->addWidget(qlSpeakerLevel, 1, 1, 1, 1);

        qliSignalLevel = new QLabel(qgbInput);
        qliSignalLevel->setObjectName(QString::fromUtf8("qliSignalLevel"));

        gridLayout->addWidget(qliSignalLevel, 2, 0, 1, 1);

        qlSignalLevel = new QLabel(qgbInput);
        qlSignalLevel->setObjectName(QString::fromUtf8("qlSignalLevel"));

        gridLayout->addWidget(qlSignalLevel, 2, 1, 1, 1);


        hboxLayout->addWidget(qgbInput);

        qgbSignal = new QGroupBox(AudioStats);
        qgbSignal->setObjectName(QString::fromUtf8("qgbSignal"));
        gridLayout1 = new QGridLayout(qgbSignal);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        qliMicVolume = new QLabel(qgbSignal);
        qliMicVolume->setObjectName(QString::fromUtf8("qliMicVolume"));

        gridLayout1->addWidget(qliMicVolume, 0, 0, 1, 1);

        qlMicVolume = new QLabel(qgbSignal);
        qlMicVolume->setObjectName(QString::fromUtf8("qlMicVolume"));

        gridLayout1->addWidget(qlMicVolume, 0, 1, 1, 1);

        qliMicSNR = new QLabel(qgbSignal);
        qliMicSNR->setObjectName(QString::fromUtf8("qliMicSNR"));

        gridLayout1->addWidget(qliMicSNR, 1, 0, 1, 1);

        qlMicSNR = new QLabel(qgbSignal);
        qlMicSNR->setObjectName(QString::fromUtf8("qlMicSNR"));

        gridLayout1->addWidget(qlMicSNR, 1, 1, 1, 1);

        qliSpeechProb = new QLabel(qgbSignal);
        qliSpeechProb->setObjectName(QString::fromUtf8("qliSpeechProb"));

        gridLayout1->addWidget(qliSpeechProb, 2, 0, 1, 1);

        qlSpeechProb = new QLabel(qgbSignal);
        qlSpeechProb->setObjectName(QString::fromUtf8("qlSpeechProb"));

        gridLayout1->addWidget(qlSpeechProb, 2, 1, 1, 1);


        hboxLayout->addWidget(qgbSignal);


        vboxLayout->addLayout(hboxLayout);

        qgbConfiguration = new QGroupBox(AudioStats);
        qgbConfiguration->setObjectName(QString::fromUtf8("qgbConfiguration"));
        gridLayout2 = new QGridLayout(qgbConfiguration);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        qliBitrate = new QLabel(qgbConfiguration);
        qliBitrate->setObjectName(QString::fromUtf8("qliBitrate"));

        gridLayout2->addWidget(qliBitrate, 0, 0, 1, 1);

        qlBitrate = new QLabel(qgbConfiguration);
        qlBitrate->setObjectName(QString::fromUtf8("qlBitrate"));
        qlBitrate->setMinimumSize(QSize(20, 0));

        gridLayout2->addWidget(qlBitrate, 0, 1, 1, 1);

        qliDoublePush = new QLabel(qgbConfiguration);
        qliDoublePush->setObjectName(QString::fromUtf8("qliDoublePush"));

        gridLayout2->addWidget(qliDoublePush, 0, 3, 1, 1);

        qlDoublePush = new QLabel(qgbConfiguration);
        qlDoublePush->setObjectName(QString::fromUtf8("qlDoublePush"));
        qlDoublePush->setMinimumSize(QSize(20, 0));

        gridLayout2->addWidget(qlDoublePush, 0, 4, 1, 1);

        qliSpeech = new QLabel(qgbConfiguration);
        qliSpeech->setObjectName(QString::fromUtf8("qliSpeech"));

        gridLayout2->addWidget(qliSpeech, 1, 0, 1, 1);

        abSpeech = new AudioBar(qgbConfiguration);
        abSpeech->setObjectName(QString::fromUtf8("abSpeech"));

        gridLayout2->addWidget(abSpeech, 1, 1, 1, 4);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem, 0, 2, 1, 1);


        vboxLayout->addWidget(qgbConfiguration);

        qgbSpectrum = new QGroupBox(AudioStats);
        qgbSpectrum->setObjectName(QString::fromUtf8("qgbSpectrum"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qgbSpectrum->sizePolicy().hasHeightForWidth());
        qgbSpectrum->setSizePolicy(sizePolicy);
        vboxLayout1 = new QVBoxLayout(qgbSpectrum);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        anwNoise = new AudioNoiseWidget(qgbSpectrum);
        anwNoise->setObjectName(QString::fromUtf8("anwNoise"));

        vboxLayout1->addWidget(anwNoise);


        vboxLayout->addWidget(qgbSpectrum);

        qgbEcho = new QGroupBox(AudioStats);
        qgbEcho->setObjectName(QString::fromUtf8("qgbEcho"));
        sizePolicy.setHeightForWidth(qgbEcho->sizePolicy().hasHeightForWidth());
        qgbEcho->setSizePolicy(sizePolicy);
        vboxLayout2 = new QVBoxLayout(qgbEcho);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        aewEcho = new AudioEchoWidget(qgbEcho);
        aewEcho->setObjectName(QString::fromUtf8("aewEcho"));
        sizePolicy.setHeightForWidth(aewEcho->sizePolicy().hasHeightForWidth());
        aewEcho->setSizePolicy(sizePolicy);

        vboxLayout2->addWidget(aewEcho);


        vboxLayout->addWidget(qgbEcho);


        retranslateUi(AudioStats);

        QMetaObject::connectSlotsByName(AudioStats);
    } // setupUi

    void retranslateUi(QDialog *AudioStats)
    {
        AudioStats->setWindowTitle(QApplication::translate("AudioStats", "Audio Statistics", 0, QApplication::UnicodeUTF8));
        qgbInput->setTitle(QApplication::translate("AudioStats", "Input Levels", 0, QApplication::UnicodeUTF8));
        qliMicLevel->setText(QApplication::translate("AudioStats", "Peak microphone level", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlMicLevel->setToolTip(QApplication::translate("AudioStats", "Peak power in last frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlMicLevel->setWhatsThis(QApplication::translate("AudioStats", "This shows the peak power in the last frame (20 ms), and is the same measurement as you would usually find displayed as \"input power\". Please disregard this and look at <b>Microphone power</b> instead, which is much more steady and disregards outliers.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlMicLevel->setText(QString());
        qliSpeakerLevel->setText(QApplication::translate("AudioStats", "Peak speaker level", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlSpeakerLevel->setToolTip(QApplication::translate("AudioStats", "Peak power in last frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlSpeakerLevel->setWhatsThis(QApplication::translate("AudioStats", "This shows the peak power of the speakers in the last frame (20 ms). Unless you are using a multi-channel sampling method (such as ASIO) with speaker channels configured, this will be 0. If you have such a setup configured, and this still shows 0 while you're playing audio from other programs, your setup is not working.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlSpeakerLevel->setText(QString());
        qliSignalLevel->setText(QApplication::translate("AudioStats", "Peak clean level", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlSignalLevel->setToolTip(QApplication::translate("AudioStats", "Peak power in last frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlSignalLevel->setWhatsThis(QApplication::translate("AudioStats", "This shows the peak power in the last frame (20 ms) after all processing. Ideally, this should be -96 dB when you're not talking. In reality, a sound studio should see -60 dB, and you should hopefully see somewhere around -20 dB. When you are talking, this should rise to somewhere between -5 and -10 dB.<br />If you are using echo cancellation, and this rises to more than -15 dB when you're not talking, your setup is not working, and you'll annoy other users with echoes.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlSignalLevel->setText(QString());
        qgbSignal->setTitle(QApplication::translate("AudioStats", "Signal Analysis", 0, QApplication::UnicodeUTF8));
        qliMicVolume->setText(QApplication::translate("AudioStats", "Microphone power", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlMicVolume->setToolTip(QApplication::translate("AudioStats", "How close the current input level is to ideal", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlMicVolume->setWhatsThis(QApplication::translate("AudioStats", "This shows how close your current input volume is to the ideal. To adjust your microphone level, open whatever program you use to adjust the recording volume, and look at the value here while talking.<br /><b>Talk loud, as you would when you're upset over getting fragged by a noob.</b><br />Adjust the volume until this value is close to 100%, but make sure it doesn't go above. If it does go above, you are likely to get clipping in parts of your speech, which will degrade sound quality.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlMicVolume->setText(QString());
        qliMicSNR->setText(QApplication::translate("AudioStats", "Signal-To-Noise ratio", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlMicSNR->setToolTip(QApplication::translate("AudioStats", "Signal-To-Noise ratio from the microphone", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlMicSNR->setWhatsThis(QApplication::translate("AudioStats", "This is the Signal-To-Noise Ratio (SNR) of the microphone in the last frame (20 ms). It shows how much clearer the voice is compared to the noise.<br />If this value is below 1.0, there's more noise than voice in the signal, and so quality is reduced.<br />There is no upper limit to this value, but don't expect to see much above 40-50 without a sound studio.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlMicSNR->setText(QString());
        qliSpeechProb->setText(QApplication::translate("AudioStats", "Speech Probability", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlSpeechProb->setToolTip(QApplication::translate("AudioStats", "Probability of speech", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlSpeechProb->setWhatsThis(QApplication::translate("AudioStats", "This is the probability that the last frame (20 ms) was speech and not environment noise.<br />Voice activity transmission depends on this being right. The trick with this is that the middle of a sentence is always detected as speech; the problem is the pauses between words and the start of speech. It's hard to distinguish a sigh from a word starting with 'h'.<br />If this is in bold font, it means Mumble is currently transmitting (if you're connected).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlSpeechProb->setText(QString());
        qgbConfiguration->setTitle(QApplication::translate("AudioStats", "Configuration feedback", 0, QApplication::UnicodeUTF8));
        qliBitrate->setText(QApplication::translate("AudioStats", "Current audio bitrate", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlBitrate->setToolTip(QApplication::translate("AudioStats", "Bitrate of last frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlBitrate->setWhatsThis(QApplication::translate("AudioStats", "This is the audio bitrate of the last compressed frame (20 ms), and as such will jump up and down as the VBR adjusts the quality. The peak bitrate can be adjusted in the Settings dialog.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlBitrate->setText(QString());
        qliDoublePush->setText(QApplication::translate("AudioStats", "DoublePush interval", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlDoublePush->setToolTip(QApplication::translate("AudioStats", "Time between last two Push-To-Talk presses", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qlDoublePush->setText(QString());
        qliSpeech->setText(QApplication::translate("AudioStats", "Speech Detection", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        abSpeech->setToolTip(QApplication::translate("AudioStats", "Current speech detection chance", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        abSpeech->setWhatsThis(QApplication::translate("AudioStats", "<b>This shows the current speech detection settings.</b><br />You can change the settings from the Settings dialog or from the Audio Wizard.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qgbSpectrum->setTitle(QApplication::translate("AudioStats", "Signal and noise power spectrum", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        anwNoise->setToolTip(QApplication::translate("AudioStats", "Power spectrum of input signal and noise estimate", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        anwNoise->setWhatsThis(QApplication::translate("AudioStats", "This shows the power spectrum of the current input signal (red line) and the current noise estimate (filled blue).<br />All amplitudes are multiplied by 30 to show the interesting parts (how much more signal than noise is present in each waveband).<br />This is probably only of interest if you're trying to fine-tune noise conditions on your microphone. Under good conditions, there should be just a tiny flutter of blue at the bottom. If the blue is more than halfway up on the graph, you have a seriously noisy environment.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qgbEcho->setTitle(QApplication::translate("AudioStats", "Echo Analysis", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        aewEcho->setToolTip(QApplication::translate("AudioStats", "Weights of the echo canceller", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        aewEcho->setWhatsThis(QApplication::translate("AudioStats", "This shows the weights of the echo canceller, with time increasing downwards and frequency increasing to the right.<br />Ideally, this should be black, indicating no echo exists at all. More commonly, you'll have one or more horizontal stripes of bluish color representing time delayed echo. You should be able to see the weights updated in real time.<br />Please note that as long as you have nothing to echo off, you won't see much useful data here. Play some music and things should stabilize. <br />You can choose to view the real or imaginary parts of the frequency-domain weights, or alternately the computed modulus and phase. The most useful of these will likely be modulus, which is the amplitude of the echo, and shows you how much of the outgoing signal is being removed at that time step. The other viewing modes are mostly useful to people who want to tune the echo cancellation algorithms.<br />Please note: If the entire image fluctuates massively while in modulus mode, the echo canceller fails to find any co"
                        "rrelation whatsoever between the two input sources (speakers and microphone). Either you have a very long delay on the echo, or one of the input sources is configured wrong.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
    } // retranslateUi

};

namespace Ui {
    class AudioStats: public Ui_AudioStats {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOSTATS_H
#endif