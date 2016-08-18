/********************************************************************************
** Form generated from reading UI file 'LookConfig.ui'
**
** Created: Fri 1. Jun 10:05:07 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS
#ifndef UI_LOOKCONFIG_H
#define UI_LOOKCONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LookConfig
{
public:
    QGridLayout *gridLayout_4;
    QGroupBox *qgbLayout;
    QGridLayout *gridLayoutLayout;
    QRadioButton *qrbLClassic;
    QRadioButton *qrbLStacked;
    QLabel *qlLStacked;
    QLabel *qlLHybrid;
    QRadioButton *qrbLHybrid;
    QRadioButton *qrbLCustom;
    QLabel *qlLCustom;
    QLabel *qlLClassic;
    QGroupBox *qgbLookFeel;
    QGridLayout *gridLayout;
    QLabel *qliStyle;
    QComboBox *qcbStyle;
    QLabel *qliCSS;
    QLineEdit *qleCSS;
    QPushButton *qpbSkinFile;
    QLabel *qliLanguage;
    QComboBox *qcbLanguage;
    QCheckBox *qcbHighContrast;
    QSpacerItem *verticalSpacer;
    QGroupBox *qgbApplication;
    QGridLayout *gridLayout_3;
    QLabel *qliAlwaysOnTop;
    QComboBox *qcbAlwaysOnTop;
    QCheckBox *qcbShowContextMenuInMenuBar;
    QCheckBox *qcbAskOnQuit;
    QGroupBox *qgbTray;
    QVBoxLayout *verticalLayout;
    QCheckBox *qcbHideTray;
    QCheckBox *qcbStateInTray;
    QGroupBox *qgbChannel;
    QGridLayout *gridLayout_2;
    QLabel *qliChannelDrag;
    QComboBox *qcbChannelDrag;
    QLabel *qliExpand;
    QComboBox *qcbExpand;
    QCheckBox *qcbUsersTop;
    QCheckBox *qcbShowUserCount;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *LookConfig)
    {
        if (LookConfig->objectName().isEmpty())
            LookConfig->setObjectName(QString::fromUtf8("LookConfig"));
        LookConfig->resize(700, 530);
        LookConfig->setWindowTitle(QString::fromUtf8("Form"));
        gridLayout_4 = new QGridLayout(LookConfig);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        qgbLayout = new QGroupBox(LookConfig);
        qgbLayout->setObjectName(QString::fromUtf8("qgbLayout"));
        gridLayoutLayout = new QGridLayout(qgbLayout);
        gridLayoutLayout->setObjectName(QString::fromUtf8("gridLayoutLayout"));
        qrbLClassic = new QRadioButton(qgbLayout);
        qrbLClassic->setObjectName(QString::fromUtf8("qrbLClassic"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qrbLClassic->sizePolicy().hasHeightForWidth());
        qrbLClassic->setSizePolicy(sizePolicy);
        qrbLClassic->setIconSize(QSize(64, 64));

        gridLayoutLayout->addWidget(qrbLClassic, 1, 0, 1, 1);

        qrbLStacked = new QRadioButton(qgbLayout);
        qrbLStacked->setObjectName(QString::fromUtf8("qrbLStacked"));
        sizePolicy.setHeightForWidth(qrbLStacked->sizePolicy().hasHeightForWidth());
        qrbLStacked->setSizePolicy(sizePolicy);
        qrbLStacked->setIconSize(QSize(64, 64));

        gridLayoutLayout->addWidget(qrbLStacked, 1, 2, 1, 1);

        qlLStacked = new QLabel(qgbLayout);
        qlLStacked->setObjectName(QString::fromUtf8("qlLStacked"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qlLStacked->sizePolicy().hasHeightForWidth());
        qlLStacked->setSizePolicy(sizePolicy1);
#ifdef USE_QTSVG
        qlLStacked->setPixmap(QPixmap(QString::fromUtf8("skin:layout_stacked.svg")));
#endif
        gridLayoutLayout->addWidget(qlLStacked, 0, 2, 1, 1);

        qlLHybrid = new QLabel(qgbLayout);
        qlLHybrid->setObjectName(QString::fromUtf8("qlLHybrid"));
        sizePolicy1.setHeightForWidth(qlLHybrid->sizePolicy().hasHeightForWidth());
        qlLHybrid->setSizePolicy(sizePolicy1);
#ifdef USE_QTSVG
        qlLHybrid->setPixmap(QPixmap(QString::fromUtf8("skin:layout_hybrid.svg")));
#endif
        qlLHybrid->setScaledContents(false);

        gridLayoutLayout->addWidget(qlLHybrid, 0, 3, 1, 1);

        qrbLHybrid = new QRadioButton(qgbLayout);
        qrbLHybrid->setObjectName(QString::fromUtf8("qrbLHybrid"));
        sizePolicy.setHeightForWidth(qrbLHybrid->sizePolicy().hasHeightForWidth());
        qrbLHybrid->setSizePolicy(sizePolicy);
        qrbLHybrid->setIconSize(QSize(64, 64));

        gridLayoutLayout->addWidget(qrbLHybrid, 1, 3, 1, 1);

        qrbLCustom = new QRadioButton(qgbLayout);
        qrbLCustom->setObjectName(QString::fromUtf8("qrbLCustom"));
        sizePolicy.setHeightForWidth(qrbLCustom->sizePolicy().hasHeightForWidth());
        qrbLCustom->setSizePolicy(sizePolicy);

        gridLayoutLayout->addWidget(qrbLCustom, 1, 4, 1, 1);

        qlLCustom = new QLabel(qgbLayout);
        qlLCustom->setObjectName(QString::fromUtf8("qlLCustom"));
#ifdef USE_QTSVG
        qlLCustom->setPixmap(QPixmap(QString::fromUtf8("skin:layout_custom.svg")));
#endif

        gridLayoutLayout->addWidget(qlLCustom, 0, 4, 1, 1);

        qlLClassic = new QLabel(qgbLayout);
        qlLClassic->setObjectName(QString::fromUtf8("qlLClassic"));
        sizePolicy1.setHeightForWidth(qlLClassic->sizePolicy().hasHeightForWidth());
        qlLClassic->setSizePolicy(sizePolicy1);
#ifdef USE_QTSVG
        qlLClassic->setPixmap(QPixmap(QString::fromUtf8("skin:layout_classic.svg")));
#endif
        gridLayoutLayout->addWidget(qlLClassic, 0, 0, 1, 1);


        gridLayout_4->addWidget(qgbLayout, 0, 0, 1, 2);

        qgbLookFeel = new QGroupBox(LookConfig);
        qgbLookFeel->setObjectName(QString::fromUtf8("qgbLookFeel"));
        gridLayout = new QGridLayout(qgbLookFeel);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qliStyle = new QLabel(qgbLookFeel);
        qliStyle->setObjectName(QString::fromUtf8("qliStyle"));

        gridLayout->addWidget(qliStyle, 0, 0, 1, 1);

        qcbStyle = new QComboBox(qgbLookFeel);
        qcbStyle->setObjectName(QString::fromUtf8("qcbStyle"));

        gridLayout->addWidget(qcbStyle, 1, 0, 1, 2);

        qliCSS = new QLabel(qgbLookFeel);
        qliCSS->setObjectName(QString::fromUtf8("qliCSS"));

        gridLayout->addWidget(qliCSS, 2, 0, 1, 1);

        qleCSS = new QLineEdit(qgbLookFeel);
        qleCSS->setObjectName(QString::fromUtf8("qleCSS"));

        gridLayout->addWidget(qleCSS, 3, 0, 1, 1);

        qpbSkinFile = new QPushButton(qgbLookFeel);
        qpbSkinFile->setObjectName(QString::fromUtf8("qpbSkinFile"));

        gridLayout->addWidget(qpbSkinFile, 3, 1, 1, 1);

        qliLanguage = new QLabel(qgbLookFeel);
        qliLanguage->setObjectName(QString::fromUtf8("qliLanguage"));

        gridLayout->addWidget(qliLanguage, 4, 0, 1, 1);

        qcbLanguage = new QComboBox(qgbLookFeel);
        qcbLanguage->setObjectName(QString::fromUtf8("qcbLanguage"));

        gridLayout->addWidget(qcbLanguage, 5, 0, 1, 2);

        qcbHighContrast = new QCheckBox(qgbLookFeel);
        qcbHighContrast->setObjectName(QString::fromUtf8("qcbHighContrast"));

        gridLayout->addWidget(qcbHighContrast, 6, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 7, 0, 1, 1);


        gridLayout_4->addWidget(qgbLookFeel, 1, 0, 3, 1);

        qgbApplication = new QGroupBox(LookConfig);
        qgbApplication->setObjectName(QString::fromUtf8("qgbApplication"));
        gridLayout_3 = new QGridLayout(qgbApplication);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        qliAlwaysOnTop = new QLabel(qgbApplication);
        qliAlwaysOnTop->setObjectName(QString::fromUtf8("qliAlwaysOnTop"));

        gridLayout_3->addWidget(qliAlwaysOnTop, 0, 0, 1, 1);

        qcbAlwaysOnTop = new QComboBox(qgbApplication);
        qcbAlwaysOnTop->setObjectName(QString::fromUtf8("qcbAlwaysOnTop"));

        gridLayout_3->addWidget(qcbAlwaysOnTop, 0, 1, 1, 1);

        qcbShowContextMenuInMenuBar = new QCheckBox(qgbApplication);
        qcbShowContextMenuInMenuBar->setObjectName(QString::fromUtf8("qcbShowContextMenuInMenuBar"));

        gridLayout_3->addWidget(qcbShowContextMenuInMenuBar, 1, 0, 1, 2);

        qcbAskOnQuit = new QCheckBox(qgbApplication);
        qcbAskOnQuit->setObjectName(QString::fromUtf8("qcbAskOnQuit"));

        gridLayout_3->addWidget(qcbAskOnQuit, 2, 0, 1, 2);


        gridLayout_4->addWidget(qgbApplication, 1, 1, 1, 1);

        qgbTray = new QGroupBox(LookConfig);
        qgbTray->setObjectName(QString::fromUtf8("qgbTray"));
        verticalLayout = new QVBoxLayout(qgbTray);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qcbHideTray = new QCheckBox(qgbTray);
        qcbHideTray->setObjectName(QString::fromUtf8("qcbHideTray"));

        verticalLayout->addWidget(qcbHideTray);

        qcbStateInTray = new QCheckBox(qgbTray);
        qcbStateInTray->setObjectName(QString::fromUtf8("qcbStateInTray"));

        verticalLayout->addWidget(qcbStateInTray);


        gridLayout_4->addWidget(qgbTray, 2, 1, 1, 1);

        qgbChannel = new QGroupBox(LookConfig);
        qgbChannel->setObjectName(QString::fromUtf8("qgbChannel"));
        gridLayout_2 = new QGridLayout(qgbChannel);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        qliChannelDrag = new QLabel(qgbChannel);
        qliChannelDrag->setObjectName(QString::fromUtf8("qliChannelDrag"));

        gridLayout_2->addWidget(qliChannelDrag, 0, 0, 1, 1);

        qcbChannelDrag = new QComboBox(qgbChannel);
        qcbChannelDrag->setObjectName(QString::fromUtf8("qcbChannelDrag"));

        gridLayout_2->addWidget(qcbChannelDrag, 0, 1, 1, 1);

        qliExpand = new QLabel(qgbChannel);
        qliExpand->setObjectName(QString::fromUtf8("qliExpand"));

        gridLayout_2->addWidget(qliExpand, 1, 0, 1, 1);

        qcbExpand = new QComboBox(qgbChannel);
        qcbExpand->setObjectName(QString::fromUtf8("qcbExpand"));

        gridLayout_2->addWidget(qcbExpand, 1, 1, 1, 1);

        qcbUsersTop = new QCheckBox(qgbChannel);
        qcbUsersTop->setObjectName(QString::fromUtf8("qcbUsersTop"));

        gridLayout_2->addWidget(qcbUsersTop, 2, 0, 1, 2);

        qcbShowUserCount = new QCheckBox(qgbChannel);
        qcbShowUserCount->setObjectName(QString::fromUtf8("qcbShowUserCount"));

        gridLayout_2->addWidget(qcbShowUserCount, 3, 0, 1, 2);

        gridLayout_2->setColumnStretch(1, 1);

        gridLayout_4->addWidget(qgbChannel, 3, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_2, 5, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer, 4, 1, 1, 1);

#ifndef QT_NO_SHORTCUT
        qliLanguage->setBuddy(qcbLanguage);
#endif // QT_NO_SHORTCUT

        retranslateUi(LookConfig);

        QMetaObject::connectSlotsByName(LookConfig);
    } // setupUi

    void retranslateUi(QWidget *LookConfig)
    {
        qgbLayout->setTitle(QApplication::translate("LookConfig", "Layout", 0, QApplication::UnicodeUTF8));
        qrbLClassic->setText(QApplication::translate("LookConfig", "Classic", 0, QApplication::UnicodeUTF8));
        qrbLStacked->setText(QApplication::translate("LookConfig", "Stacked", 0, QApplication::UnicodeUTF8));
        qlLStacked->setText(QString());
        qlLHybrid->setText(QString());
        qrbLHybrid->setText(QApplication::translate("LookConfig", "Hybrid", 0, QApplication::UnicodeUTF8));
        qrbLCustom->setText(QApplication::translate("LookConfig", "Custom", 0, QApplication::UnicodeUTF8));
        qlLCustom->setText(QString());
        qgbLookFeel->setTitle(QApplication::translate("LookConfig", "Look and Feel", 0, QApplication::UnicodeUTF8));
        qliStyle->setText(QApplication::translate("LookConfig", "Style", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbStyle->setToolTip(QApplication::translate("LookConfig", "Basic widget style", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbStyle->setWhatsThis(QApplication::translate("LookConfig", "<b>This sets the basic look and feel to use.</b>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliCSS->setText(QApplication::translate("LookConfig", "Skin", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleCSS->setToolTip(QApplication::translate("LookConfig", "Skin file to use", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qleCSS->setWhatsThis(QApplication::translate("LookConfig", "<b>This sets which skin Mumble should use.</b><br />The skin is a style file applied on top of the basic widget style. If there are icons in the same directory as the style sheet, those will replace the default icons.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbSkinFile->setText(QApplication::translate("LookConfig", "&Browse...", 0, QApplication::UnicodeUTF8));
        qliLanguage->setText(QApplication::translate("LookConfig", "Language", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbLanguage->setToolTip(QApplication::translate("LookConfig", "Language to use (requires restart)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbLanguage->setWhatsThis(QApplication::translate("LookConfig", "<b>This sets which language Mumble should use.</b><br />You have to restart Mumble to use the new language.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbHighContrast->setToolTip(QApplication::translate("LookConfig", "Apply some high contrast optimizations for visually impaired users", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbHighContrast->setText(QApplication::translate("LookConfig", "Optimize for high contrast", 0, QApplication::UnicodeUTF8));
        qgbApplication->setTitle(QApplication::translate("LookConfig", "Application", 0, QApplication::UnicodeUTF8));
        qliAlwaysOnTop->setText(QApplication::translate("LookConfig", "Always On Top", 0, QApplication::UnicodeUTF8));
        qcbAlwaysOnTop->clear();
        qcbAlwaysOnTop->insertItems(0, QStringList()
         << QApplication::translate("LookConfig", "Never", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LookConfig", "Always", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LookConfig", "In minimal view", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("LookConfig", "In normal view", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        qcbAlwaysOnTop->setToolTip(QApplication::translate("LookConfig", "This setting controls when the application will be always on top.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbAlwaysOnTop->setWhatsThis(QApplication::translate("LookConfig", "This setting controls in which situations the application will stay always on top. If you select <i>Never</i> the application will not stay on top. <i>Always</i> will always keep the application on top. <i>In minimal view</i> / <i>In normal view</i> will only keep the application always on top when minimal view is activated / deactivated.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbShowContextMenuInMenuBar->setToolTip(QApplication::translate("LookConfig", "Adds user and channel context menus into the menu bar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbShowContextMenuInMenuBar->setText(QApplication::translate("LookConfig", "Show context menu in menu bar", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbAskOnQuit->setToolTip(QApplication::translate("LookConfig", "Ask whether to close or minimize when quitting Mumble.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbAskOnQuit->setWhatsThis(QApplication::translate("LookConfig", "<b>If set, will verify you want to quit if connected.</b>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbAskOnQuit->setText(QApplication::translate("LookConfig", "Ask on quit while connected", 0, QApplication::UnicodeUTF8));
        qgbTray->setTitle(QApplication::translate("LookConfig", "Tray Icon", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbHideTray->setToolTip(QApplication::translate("LookConfig", "Hide the main Mumble window in the tray when it is minimized.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbHideTray->setWhatsThis(QApplication::translate("LookConfig", "<b>If set, minimizing the Mumble main window will cause it to be hidden and accessible only from the tray. Otherwise, it will be minimized as a window normally would.</b>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbHideTray->setText(QApplication::translate("LookConfig", "Hide in tray when minimized", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbStateInTray->setToolTip(QApplication::translate("LookConfig", "Displays talking status in system tray", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbStateInTray->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        qcbStateInTray->setText(QApplication::translate("LookConfig", "Show talking status in tray icon", 0, QApplication::UnicodeUTF8));
        qgbChannel->setTitle(QApplication::translate("LookConfig", "Channel Tree", 0, QApplication::UnicodeUTF8));
        qliChannelDrag->setText(QApplication::translate("LookConfig", "Channel Dragging", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbChannelDrag->setToolTip(QApplication::translate("LookConfig", "This changes the behavior when moving channels.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbChannelDrag->setWhatsThis(QApplication::translate("LookConfig", "This sets the behavior of channel drags; it can be used to prevent accidental dragging. <i>Move</i> moves the channel without prompting. <i>Do Nothing</i> does nothing and prints an error message. <i>Ask</i> uses a message box to confirm if you really wanted to move the channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliExpand->setText(QApplication::translate("LookConfig", "Expand", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbExpand->setToolTip(QApplication::translate("LookConfig", "When to automatically expand channels", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbExpand->setWhatsThis(QApplication::translate("LookConfig", "This sets which channels to automatically expand. <i>None</i> and <i>All</i> will expand no or all channels, while <i>Only with users</i> will expand and collapse channels as users join and leave them.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbUsersTop->setToolTip(QApplication::translate("LookConfig", "List users above subchannels (requires restart).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbUsersTop->setWhatsThis(QApplication::translate("LookConfig", "<b>If set, users will be shown above subchannels in the channel view.</b><br />A restart of Mumble is required to see the change.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbUsersTop->setText(QApplication::translate("LookConfig", "Users above Channels", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbShowUserCount->setToolTip(QApplication::translate("LookConfig", "Show number of users in each channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbShowUserCount->setText(QApplication::translate("LookConfig", "Show channel user count", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(LookConfig);
    } // retranslateUi

};

namespace Ui {
    class LookConfig: public Ui_LookConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOOKCONFIG_H
#endif