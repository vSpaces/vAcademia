/********************************************************************************
** Form generated from reading UI file 'AudioOutput.ui'
**
** Created: Fri 1. Jun 10:05:07 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOOUTPUT_H
#define UI_AUDIOOUTPUT_H

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
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AudioOutput
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *qgbInterfaces;
    QGridLayout *gridLayout_3;
    QLabel *qliSystem;
    QComboBox *qcbSystem;
    QSpacerItem *spacerItem;
    QLabel *qliDevice;
    QComboBox *qcbDevice;
    QCheckBox *qcbExclusive;
    QCheckBox *qcbPositional;
    QGroupBox *qgbOutput;
    QGridLayout *gridLayout;
    QLabel *qliJitter;
    QSlider *qsJitter;
    QLabel *qlJitter;
    QLabel *qliVolume;
    QSlider *qsVolume;
    QLabel *qlVolume;
    QLabel *qliDelay;
    QSlider *qsDelay;
    QLabel *qlDelay;
    QLabel *qliOtherVolume;
    QSlider *qsOtherVolume;
    QLabel *qlOtherVolume;
    QHBoxLayout *horizontalLayout;
    QCheckBox *qcbAttenuateOthers;
    QCheckBox *qcbAttenuateOthersOnTalk;
    QGroupBox *qgbVolume;
    QGridLayout *gridLayout_2;
    QCheckBox *qcbHeadphones;
    QLabel *qliMinDistancce;
    QSlider *qsMinDistance;
    QLabel *qlMinDistance;
    QLabel *qliMaxDistancce;
    QSlider *qsMaxDistance;
    QLabel *qlMaxDistance;
    QLabel *qliMaxDistVolume;
    QSlider *qsMaxDistVolume;
    QLabel *qlMaxDistVolume;
    QLabel *qliBloom;
    QLabel *qlBloom;
    QSlider *qsBloom;
    QGroupBox *qgbLoopback;
    QGridLayout *gridLayout1;
    QLabel *qliPacketDelay;
    QSlider *qsPacketDelay;
    QLabel *qlPacketDelay;
    QLabel *qliPacketLoss;
    QSlider *qsPacketLoss;
    QLabel *qlPacketLoss;
    QLabel *qliLoopback;
    QComboBox *qcbLoopback;
    QSpacerItem *spacerItem1;

    void setupUi(QWidget *AudioOutput)
    {
        if (AudioOutput->objectName().isEmpty())
            AudioOutput->setObjectName(QString::fromUtf8("AudioOutput"));
        AudioOutput->resize(435, 544);
        verticalLayout = new QVBoxLayout(AudioOutput);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qgbInterfaces = new QGroupBox(AudioOutput);
        qgbInterfaces->setObjectName(QString::fromUtf8("qgbInterfaces"));
        gridLayout_3 = new QGridLayout(qgbInterfaces);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        qliSystem = new QLabel(qgbInterfaces);
        qliSystem->setObjectName(QString::fromUtf8("qliSystem"));

        gridLayout_3->addWidget(qliSystem, 0, 0, 1, 1);

        qcbSystem = new QComboBox(qgbInterfaces);
        qcbSystem->setObjectName(QString::fromUtf8("qcbSystem"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qcbSystem->sizePolicy().hasHeightForWidth());
        qcbSystem->setSizePolicy(sizePolicy);

        gridLayout_3->addWidget(qcbSystem, 0, 1, 1, 1);

        spacerItem = new QSpacerItem(24, 16, QSizePolicy::Maximum, QSizePolicy::Minimum);

        gridLayout_3->addItem(spacerItem, 0, 2, 1, 1);

        qliDevice = new QLabel(qgbInterfaces);
        qliDevice->setObjectName(QString::fromUtf8("qliDevice"));

        gridLayout_3->addWidget(qliDevice, 0, 3, 1, 1);

        qcbDevice = new QComboBox(qgbInterfaces);
        qcbDevice->setObjectName(QString::fromUtf8("qcbDevice"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qcbDevice->sizePolicy().hasHeightForWidth());
        qcbDevice->setSizePolicy(sizePolicy1);
        qcbDevice->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
        qcbDevice->setMinimumContentsLength(16);

        gridLayout_3->addWidget(qcbDevice, 0, 4, 1, 1);

        qcbExclusive = new QCheckBox(qgbInterfaces);
        qcbExclusive->setObjectName(QString::fromUtf8("qcbExclusive"));

        gridLayout_3->addWidget(qcbExclusive, 1, 1, 1, 2);

        qcbPositional = new QCheckBox(qgbInterfaces);
        qcbPositional->setObjectName(QString::fromUtf8("qcbPositional"));

        gridLayout_3->addWidget(qcbPositional, 1, 4, 1, 1);


        verticalLayout->addWidget(qgbInterfaces);

        qgbOutput = new QGroupBox(AudioOutput);
        qgbOutput->setObjectName(QString::fromUtf8("qgbOutput"));
        gridLayout = new QGridLayout(qgbOutput);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qliJitter = new QLabel(qgbOutput);
        qliJitter->setObjectName(QString::fromUtf8("qliJitter"));

        gridLayout->addWidget(qliJitter, 0, 0, 1, 1);

        qsJitter = new QSlider(qgbOutput);
        qsJitter->setObjectName(QString::fromUtf8("qsJitter"));
        qsJitter->setMinimum(1);
        qsJitter->setMaximum(5);
        qsJitter->setPageStep(2);
        qsJitter->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(qsJitter, 0, 1, 1, 1);

        qlJitter = new QLabel(qgbOutput);
        qlJitter->setObjectName(QString::fromUtf8("qlJitter"));
        qlJitter->setMinimumSize(QSize(40, 0));
        qlJitter->setText(QString::fromUtf8("jb"));

        gridLayout->addWidget(qlJitter, 0, 2, 1, 1);

        qliVolume = new QLabel(qgbOutput);
        qliVolume->setObjectName(QString::fromUtf8("qliVolume"));

        gridLayout->addWidget(qliVolume, 1, 0, 1, 1);

        qsVolume = new QSlider(qgbOutput);
        qsVolume->setObjectName(QString::fromUtf8("qsVolume"));
        qsVolume->setMinimum(0);
        qsVolume->setMaximum(200);
        qsVolume->setValue(100);
        qsVolume->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(qsVolume, 1, 1, 1, 1);

        qlVolume = new QLabel(qgbOutput);
        qlVolume->setObjectName(QString::fromUtf8("qlVolume"));
        qlVolume->setText(QString::fromUtf8("vol"));

        gridLayout->addWidget(qlVolume, 1, 2, 1, 1);

        qliDelay = new QLabel(qgbOutput);
        qliDelay->setObjectName(QString::fromUtf8("qliDelay"));

        gridLayout->addWidget(qliDelay, 2, 0, 1, 1);

        qsDelay = new QSlider(qgbOutput);
        qsDelay->setObjectName(QString::fromUtf8("qsDelay"));
        qsDelay->setMinimum(1);
        qsDelay->setMaximum(10);
        qsDelay->setPageStep(3);
        qsDelay->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(qsDelay, 2, 1, 1, 1);

        qlDelay = new QLabel(qgbOutput);
        qlDelay->setObjectName(QString::fromUtf8("qlDelay"));
        qlDelay->setText(QString::fromUtf8("od"));

        gridLayout->addWidget(qlDelay, 2, 2, 1, 1);

        qliOtherVolume = new QLabel(qgbOutput);
        qliOtherVolume->setObjectName(QString::fromUtf8("qliOtherVolume"));
        qliOtherVolume->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        qliOtherVolume->setWordWrap(true);

        gridLayout->addWidget(qliOtherVolume, 3, 0, 1, 1);

        qsOtherVolume = new QSlider(qgbOutput);
        qsOtherVolume->setObjectName(QString::fromUtf8("qsOtherVolume"));
        qsOtherVolume->setMinimum(0);
        qsOtherVolume->setMaximum(100);
        qsOtherVolume->setValue(0);
        qsOtherVolume->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(qsOtherVolume, 3, 1, 1, 1);

        qlOtherVolume = new QLabel(qgbOutput);
        qlOtherVolume->setObjectName(QString::fromUtf8("qlOtherVolume"));
        qlOtherVolume->setText(QString::fromUtf8("at"));

        gridLayout->addWidget(qlOtherVolume, 3, 2, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        qcbAttenuateOthers = new QCheckBox(qgbOutput);
        qcbAttenuateOthers->setObjectName(QString::fromUtf8("qcbAttenuateOthers"));

        horizontalLayout->addWidget(qcbAttenuateOthers);

        qcbAttenuateOthersOnTalk = new QCheckBox(qgbOutput);
        qcbAttenuateOthersOnTalk->setObjectName(QString::fromUtf8("qcbAttenuateOthersOnTalk"));

        horizontalLayout->addWidget(qcbAttenuateOthersOnTalk);


        gridLayout->addLayout(horizontalLayout, 5, 1, 1, 1);


        verticalLayout->addWidget(qgbOutput);

        qgbVolume = new QGroupBox(AudioOutput);
        qgbVolume->setObjectName(QString::fromUtf8("qgbVolume"));
        qgbVolume->setEnabled(true);
        gridLayout_2 = new QGridLayout(qgbVolume);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        qcbHeadphones = new QCheckBox(qgbVolume);
        qcbHeadphones->setObjectName(QString::fromUtf8("qcbHeadphones"));

        gridLayout_2->addWidget(qcbHeadphones, 0, 3, 1, 3);

        qliMinDistancce = new QLabel(qgbVolume);
        qliMinDistancce->setObjectName(QString::fromUtf8("qliMinDistancce"));

        gridLayout_2->addWidget(qliMinDistancce, 1, 1, 1, 1);

        qsMinDistance = new QSlider(qgbVolume);
        qsMinDistance->setObjectName(QString::fromUtf8("qsMinDistance"));
        qsMinDistance->setMinimum(10);
        qsMinDistance->setMaximum(200);
        qsMinDistance->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(qsMinDistance, 1, 3, 1, 1);

        qlMinDistance = new QLabel(qgbVolume);
        qlMinDistance->setObjectName(QString::fromUtf8("qlMinDistance"));
        qlMinDistance->setMinimumSize(QSize(40, 0));
        qlMinDistance->setText(QString::fromUtf8("md"));

        gridLayout_2->addWidget(qlMinDistance, 1, 5, 1, 1);

        qliMaxDistancce = new QLabel(qgbVolume);
        qliMaxDistancce->setObjectName(QString::fromUtf8("qliMaxDistancce"));

        gridLayout_2->addWidget(qliMaxDistancce, 3, 1, 1, 1);

        qsMaxDistance = new QSlider(qgbVolume);
        qsMaxDistance->setObjectName(QString::fromUtf8("qsMaxDistance"));
        qsMaxDistance->setMinimum(10);
        qsMaxDistance->setMaximum(1000);
        qsMaxDistance->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(qsMaxDistance, 3, 3, 1, 1);

        qlMaxDistance = new QLabel(qgbVolume);
        qlMaxDistance->setObjectName(QString::fromUtf8("qlMaxDistance"));
        qlMaxDistance->setMinimumSize(QSize(40, 0));
        qlMaxDistance->setText(QString::fromUtf8("md"));

        gridLayout_2->addWidget(qlMaxDistance, 3, 5, 1, 1);

        qliMaxDistVolume = new QLabel(qgbVolume);
        qliMaxDistVolume->setObjectName(QString::fromUtf8("qliMaxDistVolume"));

        gridLayout_2->addWidget(qliMaxDistVolume, 4, 1, 1, 1);

        qsMaxDistVolume = new QSlider(qgbVolume);
        qsMaxDistVolume->setObjectName(QString::fromUtf8("qsMaxDistVolume"));
        qsMaxDistVolume->setMaximum(100);
        qsMaxDistVolume->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(qsMaxDistVolume, 4, 3, 1, 1);

        qlMaxDistVolume = new QLabel(qgbVolume);
        qlMaxDistVolume->setObjectName(QString::fromUtf8("qlMaxDistVolume"));
        qlMaxDistVolume->setMinimumSize(QSize(40, 0));
        qlMaxDistVolume->setText(QString::fromUtf8("mv"));

        gridLayout_2->addWidget(qlMaxDistVolume, 4, 5, 1, 1);

        qliBloom = new QLabel(qgbVolume);
        qliBloom->setObjectName(QString::fromUtf8("qliBloom"));

        gridLayout_2->addWidget(qliBloom, 2, 1, 1, 1);

        qlBloom = new QLabel(qgbVolume);
        qlBloom->setObjectName(QString::fromUtf8("qlBloom"));
        qlBloom->setMinimumSize(QSize(40, 0));
        qlBloom->setText(QString::fromUtf8("bl"));

        gridLayout_2->addWidget(qlBloom, 2, 5, 1, 1);

        qsBloom = new QSlider(qgbVolume);
        qsBloom->setObjectName(QString::fromUtf8("qsBloom"));
        qsBloom->setMinimum(0);
        qsBloom->setMaximum(75);
        qsBloom->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(qsBloom, 2, 3, 1, 1);


        verticalLayout->addWidget(qgbVolume);

        qgbLoopback = new QGroupBox(AudioOutput);
        qgbLoopback->setObjectName(QString::fromUtf8("qgbLoopback"));
        gridLayout1 = new QGridLayout(qgbLoopback);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        qliPacketDelay = new QLabel(qgbLoopback);
        qliPacketDelay->setObjectName(QString::fromUtf8("qliPacketDelay"));

        gridLayout1->addWidget(qliPacketDelay, 1, 0, 1, 1);

        qsPacketDelay = new QSlider(qgbLoopback);
        qsPacketDelay->setObjectName(QString::fromUtf8("qsPacketDelay"));
        qsPacketDelay->setMaximum(100);
        qsPacketDelay->setOrientation(Qt::Horizontal);

        gridLayout1->addWidget(qsPacketDelay, 1, 1, 1, 1);

        qlPacketDelay = new QLabel(qgbLoopback);
        qlPacketDelay->setObjectName(QString::fromUtf8("qlPacketDelay"));
        qlPacketDelay->setMinimumSize(QSize(40, 0));
        qlPacketDelay->setText(QString::fromUtf8("dv"));

        gridLayout1->addWidget(qlPacketDelay, 1, 2, 1, 1);

        qliPacketLoss = new QLabel(qgbLoopback);
        qliPacketLoss->setObjectName(QString::fromUtf8("qliPacketLoss"));

        gridLayout1->addWidget(qliPacketLoss, 2, 0, 1, 1);

        qsPacketLoss = new QSlider(qgbLoopback);
        qsPacketLoss->setObjectName(QString::fromUtf8("qsPacketLoss"));
        qsPacketLoss->setMaximum(50);
        qsPacketLoss->setSingleStep(5);
        qsPacketLoss->setPageStep(20);
        qsPacketLoss->setOrientation(Qt::Horizontal);

        gridLayout1->addWidget(qsPacketLoss, 2, 1, 1, 1);

        qlPacketLoss = new QLabel(qgbLoopback);
        qlPacketLoss->setObjectName(QString::fromUtf8("qlPacketLoss"));
        qlPacketLoss->setMinimumSize(QSize(40, 0));
        qlPacketLoss->setText(QString::fromUtf8("pl"));

        gridLayout1->addWidget(qlPacketLoss, 2, 2, 1, 1);

        qliLoopback = new QLabel(qgbLoopback);
        qliLoopback->setObjectName(QString::fromUtf8("qliLoopback"));

        gridLayout1->addWidget(qliLoopback, 0, 0, 1, 1);

        qcbLoopback = new QComboBox(qgbLoopback);
        qcbLoopback->setObjectName(QString::fromUtf8("qcbLoopback"));

        gridLayout1->addWidget(qcbLoopback, 0, 1, 1, 2);


        verticalLayout->addWidget(qgbLoopback);

        spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(spacerItem1);

#ifndef QT_NO_SHORTCUT
        qliSystem->setBuddy(qcbSystem);
        qliDevice->setBuddy(qcbDevice);
        qliJitter->setBuddy(qsJitter);
        qliVolume->setBuddy(qsVolume);
        qliDelay->setBuddy(qsDelay);
        qliOtherVolume->setBuddy(qsOtherVolume);
        qliMinDistancce->setBuddy(qsMinDistance);
        qliMaxDistancce->setBuddy(qsMaxDistance);
        qliMaxDistVolume->setBuddy(qsMaxDistVolume);
        qliBloom->setBuddy(qsMaxDistVolume);
        qliPacketDelay->setBuddy(qsPacketDelay);
        qliPacketLoss->setBuddy(qsPacketLoss);
        qliLoopback->setBuddy(qcbLoopback);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(qcbSystem, qcbDevice);
        QWidget::setTabOrder(qcbDevice, qcbPositional);
        QWidget::setTabOrder(qcbPositional, qsJitter);
        QWidget::setTabOrder(qsJitter, qsVolume);
        QWidget::setTabOrder(qsVolume, qsDelay);
        QWidget::setTabOrder(qsDelay, qcbHeadphones);
        QWidget::setTabOrder(qcbHeadphones, qsMinDistance);
        QWidget::setTabOrder(qsMinDistance, qsBloom);
        QWidget::setTabOrder(qsBloom, qsMaxDistance);
        QWidget::setTabOrder(qsMaxDistance, qsMaxDistVolume);
        QWidget::setTabOrder(qsMaxDistVolume, qcbLoopback);
        QWidget::setTabOrder(qcbLoopback, qsPacketDelay);
        QWidget::setTabOrder(qsPacketDelay, qsPacketLoss);

        retranslateUi(AudioOutput);

        QMetaObject::connectSlotsByName(AudioOutput);
    } // setupUi

    void retranslateUi(QWidget *AudioOutput)
    {
        AudioOutput->setWindowTitle(QApplication::translate("AudioOutput", "Form", 0, QApplication::UnicodeUTF8));
        qgbInterfaces->setTitle(QApplication::translate("AudioOutput", "Interface", 0, QApplication::UnicodeUTF8));
        qliSystem->setText(QApplication::translate("AudioOutput", "System", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbSystem->setToolTip(QApplication::translate("AudioOutput", "Output method for audio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbSystem->setWhatsThis(QApplication::translate("AudioOutput", "<b>This is the output method to use for audio.</b><br />Most likely you want to use DirectSound.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliDevice->setText(QApplication::translate("AudioOutput", "Device", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbDevice->setToolTip(QApplication::translate("AudioOutput", "Output device for audio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbDevice->setWhatsThis(QApplication::translate("AudioOutput", "<b>This is the output device to use for audio.</b>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbExclusive->setToolTip(QApplication::translate("AudioOutput", "Exclusive mode, not recommended.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbExclusive->setWhatsThis(QApplication::translate("AudioOutput", "<b>This opens the device in exclusive mode.</b><br />No other application will be able to use the device.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbExclusive->setText(QApplication::translate("AudioOutput", "Exclusive", 0, QApplication::UnicodeUTF8));
        qcbPositional->setText(QApplication::translate("AudioOutput", "Positional Audio", 0, QApplication::UnicodeUTF8));
        qgbOutput->setTitle(QApplication::translate("AudioOutput", "Audio Output", 0, QApplication::UnicodeUTF8));
        qliJitter->setText(QApplication::translate("AudioOutput", "Default &Jitter Buffer", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsJitter->setToolTip(QApplication::translate("AudioOutput", "Safety margin for jitter buffer", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsJitter->setWhatsThis(QApplication::translate("AudioOutput", "<b>This sets the minimum safety margin for the jitter buffer.</b><br />All incoming audio is buffered, and the jitter buffer continually tries to push the buffer to the minimum sustainable by your network, so latency can be as low as possible. This sets the minimum buffer size to use. If the start of sentences you hear is very jittery, increase this value.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliVolume->setText(QApplication::translate("AudioOutput", "Volume", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsVolume->setToolTip(QApplication::translate("AudioOutput", "Volume of incoming speech", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsVolume->setWhatsThis(QApplication::translate("AudioOutput", "<b>This adjusts the volume of incoming speech.</b><br />Note that if you increase this beyond 100%, audio will be distorted.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliDelay->setText(QApplication::translate("AudioOutput", "Output Delay", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsDelay->setToolTip(QApplication::translate("AudioOutput", "Amount of data to buffer", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsDelay->setWhatsThis(QApplication::translate("AudioOutput", "This sets the amount of data to pre-buffer in the output buffer. Experiment with different values and set it to the lowest which doesn't cause rapid jitter in the sound.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliOtherVolume->setText(QApplication::translate("AudioOutput", "Attenuate applications by...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsOtherVolume->setToolTip(QApplication::translate("AudioOutput", "Attenuation of other applications during speech", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsOtherVolume->setWhatsThis(QApplication::translate("AudioOutput", "<b>Attenuate volume of other applications during speech</b><br />Mumble supports decreasing the volume of other applications during incoming and/or outgoing speech. This sets the attenuation of other applications if the feature is enabled.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbAttenuateOthers->setToolTip(QApplication::translate("AudioOutput", "If checked Mumble lowers the volume of other applications while other users talk", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbAttenuateOthers->setWhatsThis(QApplication::translate("AudioOutput", "<b>Attenuate applications while other users talk</b><br />Mumble supports decreasing the volume of other applications during incoming and/or outgoing speech. This makes mumble activate the feature while other users talk to you.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbAttenuateOthers->setText(QApplication::translate("AudioOutput", "while other users talk", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbAttenuateOthersOnTalk->setToolTip(QApplication::translate("AudioOutput", "If checked Mumble lowers the volume of other applications while you talk", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbAttenuateOthersOnTalk->setWhatsThis(QApplication::translate("AudioOutput", "<b>Attenuate applications while you talk</b><br />Mumble supports decreasing the volume of other applications during incoming and/or outgoing speech. This makes mumble activate the feature while you talk.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbAttenuateOthersOnTalk->setText(QApplication::translate("AudioOutput", "while you talk", 0, QApplication::UnicodeUTF8));
        qgbVolume->setTitle(QApplication::translate("AudioOutput", "Positional Audio", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbHeadphones->setToolTip(QApplication::translate("AudioOutput", "The connected \"speakers\" are actually headphones", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbHeadphones->setWhatsThis(QApplication::translate("AudioOutput", "Checking this indicates that you don't have speakers connected, just headphones. This is important, as speakers are usually in front of you, while headphones are directly to your left/right.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbHeadphones->setText(QApplication::translate("AudioOutput", "Headphones", 0, QApplication::UnicodeUTF8));
        qliMinDistancce->setText(QApplication::translate("AudioOutput", "Minimum Distance", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsMinDistance->setToolTip(QApplication::translate("AudioOutput", "Minimum distance to user before sound volume decreases", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        qsMinDistance->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        qsMinDistance->setWhatsThis(QApplication::translate("AudioOutput", "This sets the minimum distance for sound calculations. The volume of other users' speech will not decrease until they are at least this far away from you.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliMaxDistancce->setText(QApplication::translate("AudioOutput", "Maximum Distance", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsMaxDistance->setToolTip(QApplication::translate("AudioOutput", "Maximum distance, beyond which speech volume won't decrease", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsMaxDistance->setWhatsThis(QApplication::translate("AudioOutput", "This sets the maximum distance for sound calculations. When farther away than this, other users' speech volume will not decrease any further.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliMaxDistVolume->setText(QApplication::translate("AudioOutput", "Minimum Volume", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsMaxDistVolume->setToolTip(QApplication::translate("AudioOutput", "Factor for sound volume decrease", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsMaxDistVolume->setWhatsThis(QApplication::translate("AudioOutput", "What should the volume be at the maximum distance?", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliBloom->setText(QApplication::translate("AudioOutput", "Bloom", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsBloom->setToolTip(QApplication::translate("AudioOutput", "Factor for sound volume increase", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsBloom->setWhatsThis(QApplication::translate("AudioOutput", "How much should sound volume increase for sources that are really close?", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qgbLoopback->setTitle(QApplication::translate("AudioOutput", "Loopback Test", 0, QApplication::UnicodeUTF8));
        qliPacketDelay->setText(QApplication::translate("AudioOutput", "Delay Variance", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsPacketDelay->setToolTip(QApplication::translate("AudioOutput", "Variance in packet latency", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsPacketDelay->setWhatsThis(QApplication::translate("AudioOutput", "<b>This sets the packet latency variance for loopback testing.</b><br />Most audio paths contain some variable latency. This allows you to set that variance for loopback mode testing. For example, if you set this to 15ms, this will emulate a network with 20-35ms ping latency or one with 80-95ms latency. Most domestic net connections have a variance of about 5ms.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliPacketLoss->setText(QApplication::translate("AudioOutput", "Packet Loss", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsPacketLoss->setToolTip(QApplication::translate("AudioOutput", "Packet loss for loopback mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsPacketLoss->setWhatsThis(QApplication::translate("AudioOutput", "<b>This sets the packet loss for loopback mode.</b><br />This will be the ratio of packets lost. Unless your outgoing bandwidth is peaked or there's something wrong with your network connection, this will be 0%", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliLoopback->setText(QApplication::translate("AudioOutput", "&Loopback", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbLoopback->setToolTip(QApplication::translate("AudioOutput", "Desired loopback mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbLoopback->setWhatsThis(QApplication::translate("AudioOutput", "<b>This enables one of the loopback test modes.</b><br /><i>None</i> - Loopback disabled<br /><i>Local</i> - Emulate a local server.<br /><i>Server</i> - Request loopback from server.<br />Please note than when loopback is enabled, no other users will hear your voice. This setting is not saved on application exit.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
    } // retranslateUi

};

namespace Ui {
    class AudioOutput: public Ui_AudioOutput {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOOUTPUT_H
