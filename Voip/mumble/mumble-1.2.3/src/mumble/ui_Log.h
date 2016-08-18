/********************************************************************************
** Form generated from reading UI file 'Log.ui'
**
** Created: Fri 1. Jun 10:05:08 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_LOG_H
#define UI_LOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LogConfig
{
public:
    QVBoxLayout *vboxLayout;
    QTreeWidget *qtwMessages;
    QGroupBox *qgbTTS;
    QGridLayout *gridLayout;
    QLabel *qlVolume;
    QSlider *qsVolume;
    QLabel *qlThreshold;
    QSpinBox *qsbThreshold;
    QSpacerItem *spacerItem;
    QGroupBox *qgbWhisper;
    QVBoxLayout *verticalLayout;
    QCheckBox *qcbWhisperFriends;

    void setupUi(QWidget *LogConfig)
    {
        if (LogConfig->objectName().isEmpty())
            LogConfig->setObjectName(QString::fromUtf8("LogConfig"));
        LogConfig->resize(402, 334);
        vboxLayout = new QVBoxLayout(LogConfig);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        qtwMessages = new QTreeWidget(LogConfig);
        qtwMessages->setObjectName(QString::fromUtf8("qtwMessages"));
        qtwMessages->setAlternatingRowColors(true);
        qtwMessages->setSelectionMode(QAbstractItemView::SingleSelection);
        qtwMessages->setRootIsDecorated(false);
        qtwMessages->setUniformRowHeights(true);
        qtwMessages->setItemsExpandable(false);

        vboxLayout->addWidget(qtwMessages);

        qgbTTS = new QGroupBox(LogConfig);
        qgbTTS->setObjectName(QString::fromUtf8("qgbTTS"));
        gridLayout = new QGridLayout(qgbTTS);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qlVolume = new QLabel(qgbTTS);
        qlVolume->setObjectName(QString::fromUtf8("qlVolume"));

        gridLayout->addWidget(qlVolume, 0, 0, 1, 1);

        qsVolume = new QSlider(qgbTTS);
        qsVolume->setObjectName(QString::fromUtf8("qsVolume"));
        qsVolume->setMaximum(100);
        qsVolume->setSingleStep(5);
        qsVolume->setPageStep(20);
        qsVolume->setOrientation(Qt::Horizontal);
        qsVolume->setTickPosition(QSlider::TicksBelow);
        qsVolume->setTickInterval(5);

        gridLayout->addWidget(qsVolume, 0, 1, 1, 2);

        qlThreshold = new QLabel(qgbTTS);
        qlThreshold->setObjectName(QString::fromUtf8("qlThreshold"));

        gridLayout->addWidget(qlThreshold, 1, 0, 1, 1);

        qsbThreshold = new QSpinBox(qgbTTS);
        qsbThreshold->setObjectName(QString::fromUtf8("qsbThreshold"));
        qsbThreshold->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        qsbThreshold->setMaximum(5000);
        qsbThreshold->setSingleStep(10);

        gridLayout->addWidget(qsbThreshold, 1, 1, 1, 1);

        spacerItem = new QSpacerItem(8, 16, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 1, 2, 1, 1);


        vboxLayout->addWidget(qgbTTS);

        qgbWhisper = new QGroupBox(LogConfig);
        qgbWhisper->setObjectName(QString::fromUtf8("qgbWhisper"));
        verticalLayout = new QVBoxLayout(qgbWhisper);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qcbWhisperFriends = new QCheckBox(qgbWhisper);
        qcbWhisperFriends->setObjectName(QString::fromUtf8("qcbWhisperFriends"));

        verticalLayout->addWidget(qcbWhisperFriends);


        vboxLayout->addWidget(qgbWhisper);

#ifndef QT_NO_SHORTCUT
        qlVolume->setBuddy(qsVolume);
        qlThreshold->setBuddy(qsbThreshold);
#endif // QT_NO_SHORTCUT

        retranslateUi(LogConfig);

        QMetaObject::connectSlotsByName(LogConfig);
    } // setupUi

    void retranslateUi(QWidget *LogConfig)
    {
        LogConfig->setWindowTitle(QApplication::translate("LogConfig", "Messages", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = qtwMessages->headerItem();
        ___qtreewidgetitem->setText(5, QApplication::translate("LogConfig", "Path", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(4, QApplication::translate("LogConfig", "Soundfile", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(3, QApplication::translate("LogConfig", "Text-To-Speech", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(2, QApplication::translate("LogConfig", "Notification", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("LogConfig", "Console", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("LogConfig", "Message", 0, QApplication::UnicodeUTF8));
        qgbTTS->setTitle(QApplication::translate("LogConfig", "Text To Speech", 0, QApplication::UnicodeUTF8));
        qlVolume->setText(QApplication::translate("LogConfig", "Volume", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsVolume->setToolTip(QApplication::translate("LogConfig", "Volume of Text-To-Speech Engine", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsVolume->setWhatsThis(QApplication::translate("LogConfig", "<b>This is the volume used for the speech synthesis.</b>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlThreshold->setText(QApplication::translate("LogConfig", "Length threshold", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsbThreshold->setToolTip(QApplication::translate("LogConfig", "Message length threshold for Text-To-Speech Engine", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsbThreshold->setWhatsThis(QApplication::translate("LogConfig", "<b>This is the length threshold used for the Text-To-Speech Engine.</b><br />Messages longer than this limit will not be read aloud in their full length.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qsbThreshold->setSuffix(QApplication::translate("LogConfig", " Characters", 0, QApplication::UnicodeUTF8));
        qgbWhisper->setTitle(QApplication::translate("LogConfig", "Whisper", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbWhisperFriends->setToolTip(QApplication::translate("LogConfig", "If checked you will only hear whispers from users you added to your friend list.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbWhisperFriends->setText(QApplication::translate("LogConfig", "Only accept whispers from friends", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LogConfig: public Ui_LogConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOG_H

#endif