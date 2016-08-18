/********************************************************************************
** Form generated from reading UI file 'AudioInput.ui'
**
** Created: Fri 1. Jun 10:05:07 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOINPUT_H
#define UI_AUDIOINPUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "AudioStats.h"

QT_BEGIN_NAMESPACE

class Ui_AudioInput
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *qgbInterfaces;
    QGridLayout *gridLayout;
    QLabel *qliSystem;
    QComboBox *qcbSystem;
    QSpacerItem *spacerItem;
    QLabel *qliDevice;
    QComboBox *qcbDevice;
    QComboBox *qcbEcho;
    QLabel *qliEcho;
    QCheckBox *qcbExclusive;
    QGroupBox *qgbTransmission;
    QGridLayout *gridLayout1;
    QLabel *qliTransmit;
    QComboBox *qcbTransmit;
    QLabel *qliIdle;
    QSlider *qsIdle;
    QLabel *qlIdle;
    QStackedWidget *qswTransmit;
    QWidget *qwPTT;
    QGridLayout *gridLayout2;
    QLabel *qliDoublePush;
    QSlider *qsDoublePush;
    QLabel *qlDoublePush;
    QLineEdit *qlePushClickPathOn;
    QLineEdit *qlePushClickPathOff;
    QPushButton *qpbPushClickReset;
    QPushButton *qpbPushClickBrowseOn;
    QPushButton *qpbPushClickBrowseOff;
    QLabel *qlPushClickOff;
    QLabel *qlPushClickOn;
    QPushButton *qpbPushClickPreview;
    QCheckBox *qcbPushClick;
    QWidget *qwVAD;
    QGridLayout *gridLayout3;
    QHBoxLayout *hboxLayout;
    QRadioButton *qrbAmplitude;
    QRadioButton *qrbSNR;
    QLabel *qliTransmitHold;
    QSlider *qsTransmitHold;
    QLabel *qlTransmitHold;
    AudioBar *abSpeech;
    QLabel *qliTransmitMin;
    QSlider *qsTransmitMin;
    QLabel *qliTransmitMax;
    QSlider *qsTransmitMax;
    QWidget *qwContinuous;
    QGroupBox *qgbCompression;
    QGridLayout *gridLayout4;
    QLabel *qliQuality;
    QSlider *qsQuality;
    QLabel *qlQuality;
    QLabel *qliFrames;
    QSlider *qsFrames;
    QLabel *qlFrames;
    QLabel *qlBitrate;
    QGroupBox *qgbAudio;
    QGridLayout *gridLayout5;
    QLabel *qliNoise;
    QSlider *qsNoise;
    QLabel *qlNoise;
    QLabel *qliAmp;
    QSlider *qsAmp;
    QLabel *qlAmp;
    QSpacerItem *spacerItem1;

    void setupUi(QWidget *AudioInput)
    {
        if (AudioInput->objectName().isEmpty())
            AudioInput->setObjectName(QString::fromUtf8("AudioInput"));
        AudioInput->resize(436, 512);
        vboxLayout = new QVBoxLayout(AudioInput);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        qgbInterfaces = new QGroupBox(AudioInput);
        qgbInterfaces->setObjectName(QString::fromUtf8("qgbInterfaces"));
        gridLayout = new QGridLayout(qgbInterfaces);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qliSystem = new QLabel(qgbInterfaces);
        qliSystem->setObjectName(QString::fromUtf8("qliSystem"));

        gridLayout->addWidget(qliSystem, 0, 0, 1, 1);

        qcbSystem = new QComboBox(qgbInterfaces);
        qcbSystem->setObjectName(QString::fromUtf8("qcbSystem"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qcbSystem->sizePolicy().hasHeightForWidth());
        qcbSystem->setSizePolicy(sizePolicy);

        gridLayout->addWidget(qcbSystem, 0, 1, 1, 1);

        spacerItem = new QSpacerItem(24, 16, QSizePolicy::Maximum, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 0, 2, 1, 1);

        qliDevice = new QLabel(qgbInterfaces);
        qliDevice->setObjectName(QString::fromUtf8("qliDevice"));

        gridLayout->addWidget(qliDevice, 0, 3, 1, 1);

        qcbDevice = new QComboBox(qgbInterfaces);
        qcbDevice->setObjectName(QString::fromUtf8("qcbDevice"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qcbDevice->sizePolicy().hasHeightForWidth());
        qcbDevice->setSizePolicy(sizePolicy1);
        qcbDevice->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
        qcbDevice->setMinimumContentsLength(16);

        gridLayout->addWidget(qcbDevice, 0, 4, 1, 1);

        qcbEcho = new QComboBox(qgbInterfaces);
        qcbEcho->setObjectName(QString::fromUtf8("qcbEcho"));

        gridLayout->addWidget(qcbEcho, 1, 4, 1, 1);

        qliEcho = new QLabel(qgbInterfaces);
        qliEcho->setObjectName(QString::fromUtf8("qliEcho"));

        gridLayout->addWidget(qliEcho, 1, 3, 1, 1);

        qcbExclusive = new QCheckBox(qgbInterfaces);
        qcbExclusive->setObjectName(QString::fromUtf8("qcbExclusive"));

        gridLayout->addWidget(qcbExclusive, 1, 1, 1, 1);


        vboxLayout->addWidget(qgbInterfaces);

        qgbTransmission = new QGroupBox(AudioInput);
        qgbTransmission->setObjectName(QString::fromUtf8("qgbTransmission"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(qgbTransmission->sizePolicy().hasHeightForWidth());
        qgbTransmission->setSizePolicy(sizePolicy2);
        gridLayout1 = new QGridLayout(qgbTransmission);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        qliTransmit = new QLabel(qgbTransmission);
        qliTransmit->setObjectName(QString::fromUtf8("qliTransmit"));

        gridLayout1->addWidget(qliTransmit, 0, 0, 1, 1);

        qcbTransmit = new QComboBox(qgbTransmission);
        qcbTransmit->setObjectName(QString::fromUtf8("qcbTransmit"));

        gridLayout1->addWidget(qcbTransmit, 0, 1, 1, 2);

        qliIdle = new QLabel(qgbTransmission);
        qliIdle->setObjectName(QString::fromUtf8("qliIdle"));

        gridLayout1->addWidget(qliIdle, 2, 0, 1, 1);

        qsIdle = new QSlider(qgbTransmission);
        qsIdle->setObjectName(QString::fromUtf8("qsIdle"));
        qsIdle->setMaximum(1800);
        qsIdle->setSingleStep(60);
        qsIdle->setPageStep(300);
        qsIdle->setValue(1800);
        qsIdle->setOrientation(Qt::Horizontal);

        gridLayout1->addWidget(qsIdle, 2, 1, 1, 1);

        qlIdle = new QLabel(qgbTransmission);
        qlIdle->setObjectName(QString::fromUtf8("qlIdle"));
        qlIdle->setMinimumSize(QSize(30, 0));

        gridLayout1->addWidget(qlIdle, 2, 2, 1, 1);

        qswTransmit = new QStackedWidget(qgbTransmission);
        qswTransmit->setObjectName(QString::fromUtf8("qswTransmit"));
        qwPTT = new QWidget();
        qwPTT->setObjectName(QString::fromUtf8("qwPTT"));
        gridLayout2 = new QGridLayout(qwPTT);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        qliDoublePush = new QLabel(qwPTT);
        qliDoublePush->setObjectName(QString::fromUtf8("qliDoublePush"));

        gridLayout2->addWidget(qliDoublePush, 0, 0, 1, 1);

        qsDoublePush = new QSlider(qwPTT);
        qsDoublePush->setObjectName(QString::fromUtf8("qsDoublePush"));
        qsDoublePush->setMaximum(1000);
        qsDoublePush->setSingleStep(10);
        qsDoublePush->setPageStep(100);
        qsDoublePush->setOrientation(Qt::Horizontal);

        gridLayout2->addWidget(qsDoublePush, 0, 1, 1, 2);

        qlDoublePush = new QLabel(qwPTT);
        qlDoublePush->setObjectName(QString::fromUtf8("qlDoublePush"));
        qlDoublePush->setText(QString::fromUtf8("TextLabel"));

        gridLayout2->addWidget(qlDoublePush, 0, 3, 1, 1);

        qlePushClickPathOn = new QLineEdit(qwPTT);
        qlePushClickPathOn->setObjectName(QString::fromUtf8("qlePushClickPathOn"));

        gridLayout2->addWidget(qlePushClickPathOn, 3, 1, 1, 2);

        qlePushClickPathOff = new QLineEdit(qwPTT);
        qlePushClickPathOff->setObjectName(QString::fromUtf8("qlePushClickPathOff"));

        gridLayout2->addWidget(qlePushClickPathOff, 4, 1, 1, 2);

        qpbPushClickReset = new QPushButton(qwPTT);
        qpbPushClickReset->setObjectName(QString::fromUtf8("qpbPushClickReset"));

        gridLayout2->addWidget(qpbPushClickReset, 2, 3, 1, 1);

        qpbPushClickBrowseOn = new QPushButton(qwPTT);
        qpbPushClickBrowseOn->setObjectName(QString::fromUtf8("qpbPushClickBrowseOn"));

        gridLayout2->addWidget(qpbPushClickBrowseOn, 3, 3, 1, 1);

        qpbPushClickBrowseOff = new QPushButton(qwPTT);
        qpbPushClickBrowseOff->setObjectName(QString::fromUtf8("qpbPushClickBrowseOff"));

        gridLayout2->addWidget(qpbPushClickBrowseOff, 4, 3, 1, 1);

        qlPushClickOff = new QLabel(qwPTT);
        qlPushClickOff->setObjectName(QString::fromUtf8("qlPushClickOff"));
        qlPushClickOff->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout2->addWidget(qlPushClickOff, 4, 0, 1, 1);

        qlPushClickOn = new QLabel(qwPTT);
        qlPushClickOn->setObjectName(QString::fromUtf8("qlPushClickOn"));
        qlPushClickOn->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout2->addWidget(qlPushClickOn, 3, 0, 1, 1);

        qpbPushClickPreview = new QPushButton(qwPTT);
        qpbPushClickPreview->setObjectName(QString::fromUtf8("qpbPushClickPreview"));

        gridLayout2->addWidget(qpbPushClickPreview, 2, 2, 1, 1);

        qcbPushClick = new QCheckBox(qwPTT);
        qcbPushClick->setObjectName(QString::fromUtf8("qcbPushClick"));

        gridLayout2->addWidget(qcbPushClick, 2, 1, 1, 1);

        qswTransmit->addWidget(qwPTT);
        qwVAD = new QWidget();
        qwVAD->setObjectName(QString::fromUtf8("qwVAD"));
        gridLayout3 = new QGridLayout(qwVAD);
        gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        qrbAmplitude = new QRadioButton(qwVAD);
        qrbAmplitude->setObjectName(QString::fromUtf8("qrbAmplitude"));

        hboxLayout->addWidget(qrbAmplitude);

        qrbSNR = new QRadioButton(qwVAD);
        qrbSNR->setObjectName(QString::fromUtf8("qrbSNR"));

        hboxLayout->addWidget(qrbSNR);


        gridLayout3->addLayout(hboxLayout, 0, 0, 1, 3);

        qliTransmitHold = new QLabel(qwVAD);
        qliTransmitHold->setObjectName(QString::fromUtf8("qliTransmitHold"));

        gridLayout3->addWidget(qliTransmitHold, 1, 0, 1, 1);

        qsTransmitHold = new QSlider(qwVAD);
        qsTransmitHold->setObjectName(QString::fromUtf8("qsTransmitHold"));
        qsTransmitHold->setMinimum(20);
        qsTransmitHold->setMaximum(250);
        qsTransmitHold->setOrientation(Qt::Horizontal);

        gridLayout3->addWidget(qsTransmitHold, 1, 1, 1, 1);

        qlTransmitHold = new QLabel(qwVAD);
        qlTransmitHold->setObjectName(QString::fromUtf8("qlTransmitHold"));
        qlTransmitHold->setMinimumSize(QSize(40, 0));

        gridLayout3->addWidget(qlTransmitHold, 1, 2, 1, 1);

        abSpeech = new AudioBar(qwVAD);
        abSpeech->setObjectName(QString::fromUtf8("abSpeech"));
        abSpeech->setMaximumSize(QSize(16777215, 10));

        gridLayout3->addWidget(abSpeech, 2, 1, 1, 1);

        qliTransmitMin = new QLabel(qwVAD);
        qliTransmitMin->setObjectName(QString::fromUtf8("qliTransmitMin"));

        gridLayout3->addWidget(qliTransmitMin, 3, 0, 1, 1);

        qsTransmitMin = new QSlider(qwVAD);
        qsTransmitMin->setObjectName(QString::fromUtf8("qsTransmitMin"));
        qsTransmitMin->setMinimum(1);
        qsTransmitMin->setMaximum(32767);
        qsTransmitMin->setSingleStep(100);
        qsTransmitMin->setPageStep(1000);
        qsTransmitMin->setOrientation(Qt::Horizontal);

        gridLayout3->addWidget(qsTransmitMin, 3, 1, 1, 1);

        qliTransmitMax = new QLabel(qwVAD);
        qliTransmitMax->setObjectName(QString::fromUtf8("qliTransmitMax"));

        gridLayout3->addWidget(qliTransmitMax, 4, 0, 1, 1);

        qsTransmitMax = new QSlider(qwVAD);
        qsTransmitMax->setObjectName(QString::fromUtf8("qsTransmitMax"));
        qsTransmitMax->setMinimum(1);
        qsTransmitMax->setMaximum(32767);
        qsTransmitMax->setSingleStep(100);
        qsTransmitMax->setPageStep(1000);
        qsTransmitMax->setOrientation(Qt::Horizontal);

        gridLayout3->addWidget(qsTransmitMax, 4, 1, 1, 1);

        qswTransmit->addWidget(qwVAD);
        qwContinuous = new QWidget();
        qwContinuous->setObjectName(QString::fromUtf8("qwContinuous"));
        qswTransmit->addWidget(qwContinuous);

        gridLayout1->addWidget(qswTransmit, 1, 0, 1, 3);


        vboxLayout->addWidget(qgbTransmission);

        qgbCompression = new QGroupBox(AudioInput);
        qgbCompression->setObjectName(QString::fromUtf8("qgbCompression"));
        gridLayout4 = new QGridLayout(qgbCompression);
        gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
        qliQuality = new QLabel(qgbCompression);
        qliQuality->setObjectName(QString::fromUtf8("qliQuality"));

        gridLayout4->addWidget(qliQuality, 0, 0, 1, 1);

        qsQuality = new QSlider(qgbCompression);
        qsQuality->setObjectName(QString::fromUtf8("qsQuality"));
        qsQuality->setMinimum(8000);
        qsQuality->setMaximum(96000);
        qsQuality->setSingleStep(1000);
        qsQuality->setPageStep(5000);
        qsQuality->setValue(32000);
        qsQuality->setOrientation(Qt::Horizontal);

        gridLayout4->addWidget(qsQuality, 0, 1, 1, 1);

        qlQuality = new QLabel(qgbCompression);
        qlQuality->setObjectName(QString::fromUtf8("qlQuality"));
        qlQuality->setMinimumSize(QSize(30, 0));

        gridLayout4->addWidget(qlQuality, 0, 2, 1, 1);

        qliFrames = new QLabel(qgbCompression);
        qliFrames->setObjectName(QString::fromUtf8("qliFrames"));

        gridLayout4->addWidget(qliFrames, 1, 0, 1, 1);

        qsFrames = new QSlider(qgbCompression);
        qsFrames->setObjectName(QString::fromUtf8("qsFrames"));
        qsFrames->setMinimum(1);
        qsFrames->setMaximum(4);
        qsFrames->setPageStep(2);
        qsFrames->setOrientation(Qt::Horizontal);

        gridLayout4->addWidget(qsFrames, 1, 1, 1, 1);

        qlFrames = new QLabel(qgbCompression);
        qlFrames->setObjectName(QString::fromUtf8("qlFrames"));
        qlFrames->setMinimumSize(QSize(40, 0));

        gridLayout4->addWidget(qlFrames, 1, 2, 1, 1);

        qlBitrate = new QLabel(qgbCompression);
        qlBitrate->setObjectName(QString::fromUtf8("qlBitrate"));
        QFont font;
        font.setItalic(true);
        qlBitrate->setFont(font);
        qlBitrate->setAlignment(Qt::AlignCenter);

        gridLayout4->addWidget(qlBitrate, 2, 0, 1, 3);


        vboxLayout->addWidget(qgbCompression);

        qgbAudio = new QGroupBox(AudioInput);
        qgbAudio->setObjectName(QString::fromUtf8("qgbAudio"));
        gridLayout5 = new QGridLayout(qgbAudio);
        gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
        qliNoise = new QLabel(qgbAudio);
        qliNoise->setObjectName(QString::fromUtf8("qliNoise"));

        gridLayout5->addWidget(qliNoise, 0, 0, 1, 1);

        qsNoise = new QSlider(qgbAudio);
        qsNoise->setObjectName(QString::fromUtf8("qsNoise"));
        qsNoise->setMinimum(14);
        qsNoise->setMaximum(60);
        qsNoise->setPageStep(5);
        qsNoise->setOrientation(Qt::Horizontal);

        gridLayout5->addWidget(qsNoise, 0, 1, 1, 1);

        qlNoise = new QLabel(qgbAudio);
        qlNoise->setObjectName(QString::fromUtf8("qlNoise"));
        qlNoise->setMinimumSize(QSize(30, 0));

        gridLayout5->addWidget(qlNoise, 0, 2, 1, 1);

        qliAmp = new QLabel(qgbAudio);
        qliAmp->setObjectName(QString::fromUtf8("qliAmp"));

        gridLayout5->addWidget(qliAmp, 1, 0, 1, 1);

        qsAmp = new QSlider(qgbAudio);
        qsAmp->setObjectName(QString::fromUtf8("qsAmp"));
        qsAmp->setMaximum(19500);
        qsAmp->setSingleStep(500);
        qsAmp->setPageStep(2000);
        qsAmp->setOrientation(Qt::Horizontal);

        gridLayout5->addWidget(qsAmp, 1, 1, 1, 1);

        qlAmp = new QLabel(qgbAudio);
        qlAmp->setObjectName(QString::fromUtf8("qlAmp"));
        qlAmp->setMinimumSize(QSize(30, 0));

        gridLayout5->addWidget(qlAmp, 1, 2, 1, 1);


        vboxLayout->addWidget(qgbAudio);

        spacerItem1 = new QSpacerItem(1, 151, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem1);

#ifndef QT_NO_SHORTCUT
        qliSystem->setBuddy(qcbSystem);
        qliDevice->setBuddy(qcbDevice);
        qliTransmit->setBuddy(qcbTransmit);
        qliDoublePush->setBuddy(qsDoublePush);
        qliTransmitHold->setBuddy(qsTransmitHold);
        qliQuality->setBuddy(qsQuality);
        qliFrames->setBuddy(qsFrames);
        qliNoise->setBuddy(qsNoise);
        qliAmp->setBuddy(qsAmp);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(qcbSystem, qcbDevice);
        QWidget::setTabOrder(qcbDevice, qcbEcho);
        QWidget::setTabOrder(qcbEcho, qcbTransmit);
        QWidget::setTabOrder(qcbTransmit, qsDoublePush);
        QWidget::setTabOrder(qsDoublePush, qcbPushClick);
        QWidget::setTabOrder(qcbPushClick, qpbPushClickPreview);
        QWidget::setTabOrder(qpbPushClickPreview, qpbPushClickReset);
        QWidget::setTabOrder(qpbPushClickReset, qlePushClickPathOn);
        QWidget::setTabOrder(qlePushClickPathOn, qpbPushClickBrowseOn);
        QWidget::setTabOrder(qpbPushClickBrowseOn, qlePushClickPathOff);
        QWidget::setTabOrder(qlePushClickPathOff, qpbPushClickBrowseOff);
        QWidget::setTabOrder(qpbPushClickBrowseOff, qrbSNR);
        QWidget::setTabOrder(qrbSNR, qsTransmitHold);
        QWidget::setTabOrder(qsTransmitHold, qsTransmitMin);
        QWidget::setTabOrder(qsTransmitMin, qsTransmitMax);
        QWidget::setTabOrder(qsTransmitMax, qsIdle);
        QWidget::setTabOrder(qsIdle, qsQuality);
        QWidget::setTabOrder(qsQuality, qsFrames);
        QWidget::setTabOrder(qsFrames, qsNoise);
        QWidget::setTabOrder(qsNoise, qsAmp);

        retranslateUi(AudioInput);

        qswTransmit->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AudioInput);
    } // setupUi

    void retranslateUi(QWidget *AudioInput)
    {
        AudioInput->setWindowTitle(QApplication::translate("AudioInput", "Form", 0, QApplication::UnicodeUTF8));
        qgbInterfaces->setTitle(QApplication::translate("AudioInput", "Interface", 0, QApplication::UnicodeUTF8));
        qliSystem->setText(QApplication::translate("AudioInput", "System", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbSystem->setToolTip(QApplication::translate("AudioInput", "Input method for audio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbSystem->setWhatsThis(QApplication::translate("AudioInput", "<b>This is the input method to use for audio.</b><br />Most likely you want to use DirectSound.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliDevice->setText(QApplication::translate("AudioInput", "Device", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbDevice->setToolTip(QApplication::translate("AudioInput", "Input device for audio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbDevice->setWhatsThis(QApplication::translate("AudioInput", "<b>This is the input device to use for audio.</b>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbEcho->clear();
        qcbEcho->insertItems(0, QStringList()
         << QApplication::translate("AudioInput", "Disabled", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("AudioInput", "Mixed", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("AudioInput", "Multichannel", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        qcbEcho->setToolTip(QApplication::translate("AudioInput", "Cancel echo from speakers", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbEcho->setWhatsThis(QApplication::translate("AudioInput", "Enabling this will cancel the echo from your speakers. Mixed has low CPU impact, but only works well if your speakers are equally loud and equidistant from the microphone. Multichannel echo cancellation provides much better echo cancellation, but at a higher CPU cost.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliEcho->setText(QApplication::translate("AudioInput", "Echo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbExclusive->setToolTip(QApplication::translate("AudioInput", "Exclusive mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbExclusive->setWhatsThis(QApplication::translate("AudioInput", "<b>This opens the device in exclusive mode.</b><br />No other application will be able to use the device.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbExclusive->setText(QApplication::translate("AudioInput", "Exclusive", 0, QApplication::UnicodeUTF8));
        qgbTransmission->setTitle(QApplication::translate("AudioInput", "Transmission", 0, QApplication::UnicodeUTF8));
        qliTransmit->setText(QApplication::translate("AudioInput", "&Transmit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbTransmit->setToolTip(QApplication::translate("AudioInput", "When to transmit your speech", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbTransmit->setWhatsThis(QApplication::translate("AudioInput", "<b>This sets when speech should be transmitted.</b><br /><i>Continuous</i> - All the time<br /><i>Voice Activity</i> - When you are speaking clearly.<br /><i>Push To Talk</i> - When you hold down the hotkey set under <i>Shortcuts</i>.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliIdle->setText(QApplication::translate("AudioInput", "Idle AutoDeafen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsIdle->setToolTip(QApplication::translate("AudioInput", "How long to remain idle before auto-deafen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsIdle->setWhatsThis(QApplication::translate("AudioInput", "This sets an idle timer. If the timer expires without any audio being sent to the server, you are muted and deafened.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlIdle->setText(QString());
        qliDoublePush->setText(QApplication::translate("AudioInput", "DoublePush Time", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsDoublePush->setToolTip(QApplication::translate("AudioInput", "If you press the PTT key twice in this time it will get locked.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsDoublePush->setWhatsThis(QApplication::translate("AudioInput", "<b>DoublePush Time</b><br />If you press the push-to-talk key twice during the configured interval of time it will be locked. Mumble will keep transmitting until you hit the key once more to unlock PTT again.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qlePushClickPathOn->setToolTip(QApplication::translate("AudioInput", "Gets played when the PTT button is pressed", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        qlePushClickPathOff->setToolTip(QApplication::translate("AudioInput", "Gets played when the PTT button is released", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        qpbPushClickReset->setToolTip(QApplication::translate("AudioInput", "Reset audio cue to default", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbPushClickReset->setWhatsThis(QApplication::translate("AudioInput", "<b>Reset</b><br/>Reset the paths for the files to their default.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbPushClickReset->setText(QApplication::translate("AudioInput", "R&eset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbPushClickBrowseOn->setToolTip(QApplication::translate("AudioInput", "Browse for on audio file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qpbPushClickBrowseOn->setText(QApplication::translate("AudioInput", "&Browse...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbPushClickBrowseOff->setToolTip(QApplication::translate("AudioInput", "Browse for off audio file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qpbPushClickBrowseOff->setText(QApplication::translate("AudioInput", "B&rowse...", 0, QApplication::UnicodeUTF8));
        qlPushClickOff->setText(QApplication::translate("AudioInput", "Off", 0, QApplication::UnicodeUTF8));
        qlPushClickOn->setText(QApplication::translate("AudioInput", "On", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbPushClickPreview->setToolTip(QApplication::translate("AudioInput", "Preview the audio cues", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbPushClickPreview->setWhatsThis(QApplication::translate("AudioInput", "<b>Preview</b><br/>Plays the current <i>on</i> sound followed by the current <i>off</i> sound.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbPushClickPreview->setText(QApplication::translate("AudioInput", "&Preview", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbPushClick->setToolTip(QApplication::translate("AudioInput", "Audible audio cue when push-to-talk is activated or deactivated", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbPushClick->setWhatsThis(QApplication::translate("AudioInput", "<b>This enables the audio cues for push to talk.</b><br />Setting this will give you a short audio beep when push to talk is pressed and released.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbPushClick->setText(QApplication::translate("AudioInput", "PTT Audio cue", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qrbAmplitude->setToolTip(QApplication::translate("AudioInput", "Use Amplitude based speech detection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qrbAmplitude->setWhatsThis(QApplication::translate("AudioInput", "<b>This sets speech detection to use Amplitude.</b><br />In this mode, the raw strength of the input signal is used to detect speech.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qrbAmplitude->setText(QApplication::translate("AudioInput", "Amplitude", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qrbSNR->setToolTip(QApplication::translate("AudioInput", "Use SNR based speech detection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qrbSNR->setWhatsThis(QApplication::translate("AudioInput", "<b>This sets speech detection to use Signal to Noise ratio.</b><br />In this mode, the input is analyzed for something resembling a clear signal, and the clarity of that signal is used to trigger speech detection.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qrbSNR->setText(QApplication::translate("AudioInput", "Signal to Noise", 0, QApplication::UnicodeUTF8));
        qliTransmitHold->setText(QApplication::translate("AudioInput", "Voice &Hold", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsTransmitHold->setToolTip(QApplication::translate("AudioInput", "How long to keep transmitting after silence", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsTransmitHold->setWhatsThis(QApplication::translate("AudioInput", "<b>This selects how long after a perceived stop in speech transmission should continue.</b><br />Set this higher if your voice breaks up when you speak (seen by a rapidly blinking voice icon next to your name).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlTransmitHold->setText(QString());
#ifndef QT_NO_TOOLTIP
        abSpeech->setToolTip(QApplication::translate("AudioInput", "Current speech detection chance", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        abSpeech->setWhatsThis(QApplication::translate("AudioInput", "<b>This shows the current speech detection settings.</b><br />You can change the settings from the Settings dialog or from the Audio Wizard.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliTransmitMin->setText(QApplication::translate("AudioInput", "Silence Below", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsTransmitMin->setToolTip(QApplication::translate("AudioInput", "Signal values below this count as silence", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsTransmitMin->setWhatsThis(QApplication::translate("AudioInput", "<b>This sets the trigger values for voice detection.</b><br />Use this together with the Audio Statistics window to manually tune the trigger values for detecting speech. Input values below \"Silence Below\" always count as silence. Values above \"Speech Above\" always count as voice. Values in between will count as voice if you're already talking, but will not trigger a new detection.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliTransmitMax->setText(QApplication::translate("AudioInput", "Speech Above", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsTransmitMax->setToolTip(QApplication::translate("AudioInput", "Signal values above this count as voice", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsTransmitMax->setWhatsThis(QApplication::translate("AudioInput", "<b>This sets the trigger values for voice detection.</b><br />Use this together with the Audio Statistics window to manually tune the trigger values for detecting speech. Input values below \"Silence Below\" always count as silence. Values above \"Speech Above\" always count as voice. Values in between will count as voice if you're already talking, but will not trigger a new detection.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qgbCompression->setTitle(QApplication::translate("AudioInput", "Compression", 0, QApplication::UnicodeUTF8));
        qliQuality->setText(QApplication::translate("AudioInput", "&Quality", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsQuality->setToolTip(QApplication::translate("AudioInput", "Quality of compression (peak bandwidth)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsQuality->setWhatsThis(QApplication::translate("AudioInput", "<b>This sets the quality of compression.</b><br />This determines how much bandwidth Mumble is allowed to use for outgoing audio.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlQuality->setText(QString());
        qliFrames->setText(QApplication::translate("AudioInput", "Audio per packet", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsFrames->setToolTip(QApplication::translate("AudioInput", "How many audio frames to send per packet", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsFrames->setWhatsThis(QApplication::translate("AudioInput", "<b>This selects how many audio frames should be put in one packet.</b><br />Increasing this will increase the latency of your voice, but will also reduce bandwidth requirements.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlFrames->setText(QString());
#ifndef QT_NO_TOOLTIP
        qlBitrate->setToolTip(QApplication::translate("AudioInput", "Maximum bandwidth used for sending audio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlBitrate->setWhatsThis(QApplication::translate("AudioInput", "<b>This shows peak outgoing bandwidth used.</b><br />This shows the peak amount of bandwidth sent out from your machine. Audio bitrate is the maximum bitrate (as we use VBR) for the audio data alone. Position is the bitrate used for positional information. Overhead is our framing and the IP packet headers (IP and UDP is 75% of this overhead).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlBitrate->setText(QString());
        qgbAudio->setTitle(QApplication::translate("AudioInput", "Audio Processing", 0, QApplication::UnicodeUTF8));
        qliNoise->setText(QApplication::translate("AudioInput", "Noise Suppression", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsNoise->setToolTip(QApplication::translate("AudioInput", "Noise suppression", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsNoise->setWhatsThis(QApplication::translate("AudioInput", "<b>This sets the amount of noise suppression to apply.</b><br />The higher this value, the more aggressively stationary noise will be suppressed.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlNoise->setText(QString());
        qliAmp->setText(QApplication::translate("AudioInput", "Amplification", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsAmp->setToolTip(QApplication::translate("AudioInput", "Maximum amplification of input sound", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsAmp->setWhatsThis(QApplication::translate("AudioInput", "<b>Maximum amplification of input.</b><br />Mumble normalizes the input volume before compressing, and this sets how much it's allowed to amplify.<br />The actual level is continually updated based on your current speech pattern, but it will never go above the level specified here.<br />If the <i>Microphone loudness</i> level of the audio statistics hover around 100%, you probably want to set this to 2.0 or so, but if, like most people, you are unable to reach 100%, set this to something much higher.<br />Ideally, set it so <i>Microphone Loudness * Amplification Factor >= 100</i>, even when you're speaking really soft.<br /><br />Note that there is no harm in setting this to maximum, but Mumble will start picking up other conversations if you leave it to auto-tune to that level.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlAmp->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class AudioInput: public Ui_AudioInput {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOINPUT_H
