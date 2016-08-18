/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Fri 1. Jun 10:05:06 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolBar>
#ifdef ADDITIONAL_WINDOWS
#include "CustomElements.h"
#endif
#include "UserView.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *qaQuit;
    QAction *qaServerConnect;
    QAction *qaServerDisconnect;
    QAction *qaServerBanList;
    QAction *qaServerInformation;
    QAction *qaUserKick;
    QAction *qaUserMute;
    QAction *qaUserBan;
    QAction *qaUserDeaf;
    QAction *qaUserLocalMute;
    QAction *qaUserTextMessage;
    QAction *qaChannelAdd;
    QAction *qaChannelRemove;
    QAction *qaChannelACL;
    QAction *qaChannelLink;
    QAction *qaChannelUnlink;
    QAction *qaChannelUnlinkAll;
    QAction *qaAudioReset;
    QAction *qaAudioMute;
    QAction *qaAudioDeaf;
    QAction *qaAudioTTS;
    QAction *qaAudioStats;
    QAction *qaAudioUnlink;
    QAction *qaConfigDialog;
    QAction *qaAudioWizard;
    QAction *qaHelpWhatsThis;
    QAction *qaHelpAbout;
    QAction *qaHelpAboutSpeex;
    QAction *qaHelpAboutQt;
    QAction *qaHelpVersionCheck;
    QAction *qaChannelSendMessage;
    QAction *qaConfigMinimal;
    QAction *qaConfigHideFrame;
    QAction *qaConfigCert;
    QAction *qaUserRegister;
    QAction *qaUserFriendAdd;
    QAction *qaUserFriendRemove;
    QAction *qaUserFriendUpdate;
    QAction *qaServerUserList;
    QAction *qaServerTexture;
    QAction *qaServerTokens;
    QAction *qaServerTextureRemove;
    QAction *qaUserCommentReset;
    QAction *qaChannelJoin;
    QAction *qaUserCommentView;
    QAction *qaUserInformation;
    QAction *qaSelfComment;
    QAction *qaSelfRegister;
    QAction *qaUserPrioritySpeaker;
    QAction *qaSelfPrioritySpeaker;
    QAction *qaRecording;
    UserView *qtvUsers;
    QMenuBar *menubar;
    QMenu *qmConfig;
    QMenu *qmHelp;
    QMenu *qmServer;
    QMenu *qmSelf;
    QDockWidget *qdwLog;
    QTextBrowser *qteLog;
#ifdef ADDITIONAL_WINDOWS
    QDockWidget *qdwChat;
    ChatbarTextEdit *qteChat;
#endif
    QToolBar *qtIconToolbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(671, 435);
        MainWindow->setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AnimatedDocks);
        MainWindow->setUnifiedTitleAndToolBarOnMac(true);
        qaQuit = new QAction(MainWindow);
        qaQuit->setObjectName(QString::fromUtf8("qaQuit"));
        qaServerConnect = new QAction(MainWindow);
        qaServerConnect->setObjectName(QString::fromUtf8("qaServerConnect"));
        QIcon icon;
#ifdef USE_QTSVG
        icon.addFile(QString::fromUtf8("skin:categories/applications-internet.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaServerConnect->setIcon(icon);
        qaServerConnect->setIconVisibleInMenu(false);
        qaServerDisconnect = new QAction(MainWindow);
        qaServerDisconnect->setObjectName(QString::fromUtf8("qaServerDisconnect"));
        qaServerDisconnect->setEnabled(false);
        qaServerBanList = new QAction(MainWindow);
        qaServerBanList->setObjectName(QString::fromUtf8("qaServerBanList"));
        qaServerBanList->setEnabled(false);
        qaServerInformation = new QAction(MainWindow);
        qaServerInformation->setObjectName(QString::fromUtf8("qaServerInformation"));
        qaServerInformation->setEnabled(false);
        QIcon icon1;
#ifdef USE_QTSVG
        icon1.addFile(QString::fromUtf8("skin:Information_icon.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaServerInformation->setIcon(icon1);
        qaServerInformation->setIconVisibleInMenu(false);
        qaUserKick = new QAction(MainWindow);
        qaUserKick->setObjectName(QString::fromUtf8("qaUserKick"));
        qaUserMute = new QAction(MainWindow);
        qaUserMute->setObjectName(QString::fromUtf8("qaUserMute"));
        qaUserMute->setCheckable(true);
        qaUserBan = new QAction(MainWindow);
        qaUserBan->setObjectName(QString::fromUtf8("qaUserBan"));
        qaUserDeaf = new QAction(MainWindow);
        qaUserDeaf->setObjectName(QString::fromUtf8("qaUserDeaf"));
        qaUserDeaf->setCheckable(true);
        qaUserLocalMute = new QAction(MainWindow);
        qaUserLocalMute->setObjectName(QString::fromUtf8("qaUserLocalMute"));
        qaUserLocalMute->setCheckable(true);
        qaUserTextMessage = new QAction(MainWindow);
        qaUserTextMessage->setObjectName(QString::fromUtf8("qaUserTextMessage"));
        qaChannelAdd = new QAction(MainWindow);
        qaChannelAdd->setObjectName(QString::fromUtf8("qaChannelAdd"));
        qaChannelRemove = new QAction(MainWindow);
        qaChannelRemove->setObjectName(QString::fromUtf8("qaChannelRemove"));
        qaChannelACL = new QAction(MainWindow);
        qaChannelACL->setObjectName(QString::fromUtf8("qaChannelACL"));
        qaChannelLink = new QAction(MainWindow);
        qaChannelLink->setObjectName(QString::fromUtf8("qaChannelLink"));
        qaChannelUnlink = new QAction(MainWindow);
        qaChannelUnlink->setObjectName(QString::fromUtf8("qaChannelUnlink"));
        qaChannelUnlinkAll = new QAction(MainWindow);
        qaChannelUnlinkAll->setObjectName(QString::fromUtf8("qaChannelUnlinkAll"));
        qaAudioReset = new QAction(MainWindow);
        qaAudioReset->setObjectName(QString::fromUtf8("qaAudioReset"));
        qaAudioMute = new QAction(MainWindow);
        qaAudioMute->setObjectName(QString::fromUtf8("qaAudioMute"));
        qaAudioMute->setCheckable(true);
        QIcon icon2;
#ifdef USE_QTSVG
        icon2.addFile(QString::fromUtf8("skin:actions/audio-input-microphone.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8("skin:actions/audio-input-microphone-muted.svg"), QSize(), QIcon::Normal, QIcon::On);
        icon2.addFile(QString::fromUtf8("skin:actions/audio-input-microphone-muted.svg"), QSize(), QIcon::Active, QIcon::On);
#endif
        qaAudioMute->setIcon(icon2);
        qaAudioMute->setIconVisibleInMenu(false);
        qaAudioDeaf = new QAction(MainWindow);
        qaAudioDeaf->setObjectName(QString::fromUtf8("qaAudioDeaf"));
        qaAudioDeaf->setCheckable(true);
        QIcon icon3;
#ifdef USE_QTSVG
        icon3.addFile(QString::fromUtf8("skin:self_undeafened.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon3.addFile(QString::fromUtf8("skin:deafened_self.svg"), QSize(), QIcon::Normal, QIcon::On);
        icon3.addFile(QString::fromUtf8("skin:deafened_self.svg"), QSize(), QIcon::Active, QIcon::On);
#endif
        qaAudioDeaf->setIcon(icon3);
        qaAudioDeaf->setIconVisibleInMenu(false);
        qaAudioTTS = new QAction(MainWindow);
        qaAudioTTS->setObjectName(QString::fromUtf8("qaAudioTTS"));
        qaAudioTTS->setCheckable(true);
        qaAudioStats = new QAction(MainWindow);
        qaAudioStats->setObjectName(QString::fromUtf8("qaAudioStats"));
        qaAudioUnlink = new QAction(MainWindow);
        qaAudioUnlink->setObjectName(QString::fromUtf8("qaAudioUnlink"));
        qaConfigDialog = new QAction(MainWindow);
        qaConfigDialog->setObjectName(QString::fromUtf8("qaConfigDialog"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("skin:config_basic.png"), QSize(), QIcon::Normal, QIcon::Off);
        qaConfigDialog->setIcon(icon4);
        qaConfigDialog->setMenuRole(QAction::PreferencesRole);
        qaConfigDialog->setIconVisibleInMenu(false);
        qaAudioWizard = new QAction(MainWindow);
        qaAudioWizard->setObjectName(QString::fromUtf8("qaAudioWizard"));
        qaHelpWhatsThis = new QAction(MainWindow);
        qaHelpWhatsThis->setObjectName(QString::fromUtf8("qaHelpWhatsThis"));
        qaHelpAbout = new QAction(MainWindow);
        qaHelpAbout->setObjectName(QString::fromUtf8("qaHelpAbout"));
        qaHelpAbout->setMenuRole(QAction::AboutRole);
        qaHelpAboutSpeex = new QAction(MainWindow);
        qaHelpAboutSpeex->setObjectName(QString::fromUtf8("qaHelpAboutSpeex"));
        qaHelpAboutSpeex->setMenuRole(QAction::ApplicationSpecificRole);
        qaHelpAboutQt = new QAction(MainWindow);
        qaHelpAboutQt->setObjectName(QString::fromUtf8("qaHelpAboutQt"));
        qaHelpAboutQt->setMenuRole(QAction::AboutQtRole);
        qaHelpVersionCheck = new QAction(MainWindow);
        qaHelpVersionCheck->setObjectName(QString::fromUtf8("qaHelpVersionCheck"));
        qaChannelSendMessage = new QAction(MainWindow);
        qaChannelSendMessage->setObjectName(QString::fromUtf8("qaChannelSendMessage"));
        qaConfigMinimal = new QAction(MainWindow);
        qaConfigMinimal->setObjectName(QString::fromUtf8("qaConfigMinimal"));
        qaConfigMinimal->setCheckable(true);
        qaConfigHideFrame = new QAction(MainWindow);
        qaConfigHideFrame->setObjectName(QString::fromUtf8("qaConfigHideFrame"));
        qaConfigHideFrame->setCheckable(true);
        qaConfigCert = new QAction(MainWindow);
        qaConfigCert->setObjectName(QString::fromUtf8("qaConfigCert"));
        qaUserRegister = new QAction(MainWindow);
        qaUserRegister->setObjectName(QString::fromUtf8("qaUserRegister"));
        qaUserFriendAdd = new QAction(MainWindow);
        qaUserFriendAdd->setObjectName(QString::fromUtf8("qaUserFriendAdd"));
        qaUserFriendRemove = new QAction(MainWindow);
        qaUserFriendRemove->setObjectName(QString::fromUtf8("qaUserFriendRemove"));
        qaUserFriendUpdate = new QAction(MainWindow);
        qaUserFriendUpdate->setObjectName(QString::fromUtf8("qaUserFriendUpdate"));
        qaServerUserList = new QAction(MainWindow);
        qaServerUserList->setObjectName(QString::fromUtf8("qaServerUserList"));
        qaServerTexture = new QAction(MainWindow);
        qaServerTexture->setObjectName(QString::fromUtf8("qaServerTexture"));
        qaServerTokens = new QAction(MainWindow);
        qaServerTokens->setObjectName(QString::fromUtf8("qaServerTokens"));
        qaServerTextureRemove = new QAction(MainWindow);
        qaServerTextureRemove->setObjectName(QString::fromUtf8("qaServerTextureRemove"));
        qaUserCommentReset = new QAction(MainWindow);
        qaUserCommentReset->setObjectName(QString::fromUtf8("qaUserCommentReset"));
        qaChannelJoin = new QAction(MainWindow);
        qaChannelJoin->setObjectName(QString::fromUtf8("qaChannelJoin"));
        qaUserCommentView = new QAction(MainWindow);
        qaUserCommentView->setObjectName(QString::fromUtf8("qaUserCommentView"));
        qaUserInformation = new QAction(MainWindow);
        qaUserInformation->setObjectName(QString::fromUtf8("qaUserInformation"));
        qaSelfComment = new QAction(MainWindow);
        qaSelfComment->setObjectName(QString::fromUtf8("qaSelfComment"));
        QIcon icon5;
#ifdef USE_QTSVG
        icon5.addFile(QString::fromUtf8("skin:comment.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaSelfComment->setIcon(icon5);
        qaSelfComment->setIconVisibleInMenu(false);
        qaSelfRegister = new QAction(MainWindow);
        qaSelfRegister->setObjectName(QString::fromUtf8("qaSelfRegister"));
        qaUserPrioritySpeaker = new QAction(MainWindow);
        qaUserPrioritySpeaker->setObjectName(QString::fromUtf8("qaUserPrioritySpeaker"));
        qaUserPrioritySpeaker->setCheckable(true);
        qaSelfPrioritySpeaker = new QAction(MainWindow);
        qaSelfPrioritySpeaker->setObjectName(QString::fromUtf8("qaSelfPrioritySpeaker"));
        qaSelfPrioritySpeaker->setCheckable(true);
        qaRecording = new QAction(MainWindow);
        qaRecording->setObjectName(QString::fromUtf8("qaRecording"));
        QIcon icon6;
#ifdef USE_QTSVG
        icon6.addFile(QString::fromUtf8("skin:actions/media-record.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaRecording->setIcon(icon6);
        qaRecording->setIconVisibleInMenu(false);
        qtvUsers = new UserView(MainWindow);
        qtvUsers->setObjectName(QString::fromUtf8("qtvUsers"));
        qtvUsers->setContextMenuPolicy(Qt::CustomContextMenu);
        qtvUsers->setAcceptDrops(true);
        qtvUsers->setDragEnabled(true);
        qtvUsers->setRootIsDecorated(false);
        qtvUsers->setUniformRowHeights(true);
        qtvUsers->setHeaderHidden(true);
        MainWindow->setCentralWidget(qtvUsers);
        qtvUsers->header()->setVisible(false);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 671, 21));
        qmConfig = new QMenu(menubar);
        qmConfig->setObjectName(QString::fromUtf8("qmConfig"));
        qmHelp = new QMenu(menubar);
        qmHelp->setObjectName(QString::fromUtf8("qmHelp"));
        qmServer = new QMenu(menubar);
        qmServer->setObjectName(QString::fromUtf8("qmServer"));
        qmSelf = new QMenu(menubar);
        qmSelf->setObjectName(QString::fromUtf8("qmSelf"));
        MainWindow->setMenuBar(menubar);
        qdwLog = new QDockWidget(MainWindow);
        qdwLog->setObjectName(QString::fromUtf8("qdwLog"));
        qdwLog->setMinimumSize(QSize(250, 50));
        qdwLog->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable);
        qteLog = new QTextBrowser();
        qteLog->setObjectName(QString::fromUtf8("qteLog"));
        qteLog->setContextMenuPolicy(Qt::CustomContextMenu);
        qteLog->setOpenLinks(false);
        qdwLog->setWidget(qteLog);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), qdwLog);
#ifdef ADDITIONAL_WINDOWS
        qdwChat = new QDockWidget(MainWindow);
        qdwChat->setObjectName(QString::fromUtf8("qdwChat"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qdwChat->sizePolicy().hasHeightForWidth());
        qdwChat->setSizePolicy(sizePolicy);
        qdwChat->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable);

        qteChat = new ChatbarTextEdit();
        qteChat->setObjectName(QString::fromUtf8("qteChat"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qteChat->sizePolicy().hasHeightForWidth());
        qteChat->setSizePolicy(sizePolicy1);
        qteChat->setMaximumSize(QSize(16777215, 16777215));
        qteChat->setAcceptRichText(false);
        qdwChat->setWidget(qteChat);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), qdwChat);
#endif
        qtIconToolbar = new QToolBar(MainWindow);
        qtIconToolbar->setObjectName(QString::fromUtf8("qtIconToolbar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, qtIconToolbar);

        menubar->addAction(qmServer->menuAction());
        menubar->addAction(qmSelf->menuAction());
        menubar->addAction(qmConfig->menuAction());
        menubar->addAction(qmHelp->menuAction());
        qmConfig->addAction(qaConfigDialog);
        qmHelp->addAction(qaHelpWhatsThis);
        qmHelp->addSeparator();
        qmHelp->addAction(qaHelpAbout);
        qmHelp->addAction(qaHelpAboutSpeex);
        qmHelp->addAction(qaHelpAboutQt);
        qmHelp->addSeparator();
        qmHelp->addAction(qaHelpVersionCheck);
        qmSelf->addAction(qaAudioMute);
        qmSelf->addAction(qaAudioDeaf);
        qmSelf->addAction(qaSelfPrioritySpeaker);
        qmSelf->addSeparator();
        qmSelf->addAction(qaRecording);
        qmSelf->addSeparator();
        qmSelf->addAction(qaSelfComment);
        qmSelf->addAction(qaServerTexture);
        qmSelf->addAction(qaServerTextureRemove);
        qmSelf->addSeparator();
        qmSelf->addAction(qaSelfRegister);
        qmSelf->addAction(qaAudioStats);
        qtIconToolbar->addAction(qaServerConnect);
        qtIconToolbar->addAction(qaServerInformation);
        qtIconToolbar->addSeparator();
        qtIconToolbar->addAction(qaAudioMute);
        qtIconToolbar->addAction(qaAudioDeaf);
        qtIconToolbar->addAction(qaRecording);
        qtIconToolbar->addSeparator();
        qtIconToolbar->addAction(qaSelfComment);
        qtIconToolbar->addSeparator();
        qtIconToolbar->addAction(qaConfigDialog);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Mumble", 0, QApplication::UnicodeUTF8));
        qaQuit->setText(QApplication::translate("MainWindow", "&Quit Mumble", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaQuit->setToolTip(QApplication::translate("MainWindow", "Closes the program", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaQuit->setWhatsThis(QApplication::translate("MainWindow", "Exits the application.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        qaServerConnect->setText(QApplication::translate("MainWindow", "&Connect", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaServerConnect->setToolTip(QApplication::translate("MainWindow", "Open the server connection dialog", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaServerConnect->setWhatsThis(QApplication::translate("MainWindow", "Shows a dialog of registered servers, and also allows quick-connect.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaServerDisconnect->setText(QApplication::translate("MainWindow", "&Disconnect", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaServerDisconnect->setToolTip(QApplication::translate("MainWindow", "Disconnect from server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaServerDisconnect->setWhatsThis(QApplication::translate("MainWindow", "Disconnects you from the server.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaServerBanList->setText(QApplication::translate("MainWindow", "&Ban lists", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaServerBanList->setToolTip(QApplication::translate("MainWindow", "Edit ban lists on server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaServerBanList->setWhatsThis(QApplication::translate("MainWindow", "This lets you edit the server-side IP ban lists.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaServerInformation->setText(QApplication::translate("MainWindow", "&Information", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaServerInformation->setToolTip(QApplication::translate("MainWindow", "Show information about the server connection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaServerInformation->setWhatsThis(QApplication::translate("MainWindow", "This will show extended information about the connection to the server.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserKick->setText(QApplication::translate("MainWindow", "&Kick", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserKick->setToolTip(QApplication::translate("MainWindow", "Kick user (with reason)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserKick->setWhatsThis(QApplication::translate("MainWindow", "Kick selected user off server. You'll be asked to specify a reason.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserMute->setText(QApplication::translate("MainWindow", "&Mute", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserMute->setToolTip(QApplication::translate("MainWindow", "Mute user", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserMute->setWhatsThis(QApplication::translate("MainWindow", "Mute or unmute user on server. Unmuting a deafened user will also undeafen them.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserBan->setText(QApplication::translate("MainWindow", "&Ban", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserBan->setToolTip(QApplication::translate("MainWindow", "Kick and ban user (with reason)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserBan->setWhatsThis(QApplication::translate("MainWindow", "Kick and ban selected user from server. You'll be asked to specify a reason.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserDeaf->setText(QApplication::translate("MainWindow", "&Deafen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserDeaf->setToolTip(QApplication::translate("MainWindow", "Deafen user", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserDeaf->setWhatsThis(QApplication::translate("MainWindow", "Deafen or undeafen user on server. Deafening a user will also mute them.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserLocalMute->setText(QApplication::translate("MainWindow", "&Local Mute", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserLocalMute->setToolTip(QApplication::translate("MainWindow", "Mute user locally", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserLocalMute->setWhatsThis(QApplication::translate("MainWindow", "Mute or unmute user locally. Use this on other users in the same room.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserTextMessage->setText(QApplication::translate("MainWindow", "Send &Message", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserTextMessage->setToolTip(QApplication::translate("MainWindow", "Send a Text Message", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserTextMessage->setWhatsThis(QApplication::translate("MainWindow", "Sends a text message to another user.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaChannelAdd->setText(QApplication::translate("MainWindow", "&Add", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaChannelAdd->setToolTip(QApplication::translate("MainWindow", "Add new channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaChannelAdd->setWhatsThis(QApplication::translate("MainWindow", "This adds a new sub-channel to the currently selected channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaChannelRemove->setText(QApplication::translate("MainWindow", "&Remove", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaChannelRemove->setToolTip(QApplication::translate("MainWindow", "Remove channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaChannelRemove->setWhatsThis(QApplication::translate("MainWindow", "This removes a channel and all sub-channels.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaChannelACL->setText(QApplication::translate("MainWindow", "&Edit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaChannelACL->setToolTip(QApplication::translate("MainWindow", "Edit Groups and ACL for channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaChannelACL->setWhatsThis(QApplication::translate("MainWindow", "This opens the Group and ACL dialog for the channel, to control permissions.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaChannelLink->setText(QApplication::translate("MainWindow", "&Link", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaChannelLink->setToolTip(QApplication::translate("MainWindow", "Link your channel to another channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaChannelLink->setWhatsThis(QApplication::translate("MainWindow", "This links your current channel to the selected channel. If users in a channel have permission to speak in the other channel, users can now hear each other. This is a permanent link, and will last until manually unlinked or the server is restarted. Please see the shortcuts for push-to-link.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaChannelUnlink->setText(QApplication::translate("MainWindow", "&Unlink", "Channel", QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaChannelUnlink->setToolTip(QApplication::translate("MainWindow", "Unlink your channel from another channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaChannelUnlink->setWhatsThis(QApplication::translate("MainWindow", "This unlinks your current channel from the selected channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaChannelUnlinkAll->setText(QApplication::translate("MainWindow", "&Unlink All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaChannelUnlinkAll->setToolTip(QApplication::translate("MainWindow", "Unlinks your channel from all linked channels.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaChannelUnlinkAll->setWhatsThis(QApplication::translate("MainWindow", "This unlinks your current channel (not the selected one) from all linked channels.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaAudioReset->setText(QApplication::translate("MainWindow", "&Reset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaAudioReset->setToolTip(QApplication::translate("MainWindow", "Reset audio preprocessor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaAudioReset->setWhatsThis(QApplication::translate("MainWindow", "This will reset the audio preprocessor, including noise cancellation, automatic gain and voice activity detection. If something suddenly worsens the audio environment (like dropping the microphone) and it was temporary, use this to avoid having to wait for the preprocessor to readjust.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaAudioMute->setText(QApplication::translate("MainWindow", "&Mute Self", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaAudioMute->setToolTip(QApplication::translate("MainWindow", "Mute yourself", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaAudioMute->setWhatsThis(QApplication::translate("MainWindow", "Mute or unmute yourself. When muted, you will not send any data to the server. Unmuting while deafened will also undeafen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaAudioDeaf->setText(QApplication::translate("MainWindow", "&Deafen Self", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaAudioDeaf->setToolTip(QApplication::translate("MainWindow", "Deafen yourself", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaAudioDeaf->setWhatsThis(QApplication::translate("MainWindow", "Deafen or undeafen yourself. When deafened, you will not hear anything. Deafening yourself will also mute.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaAudioTTS->setText(QApplication::translate("MainWindow", "&Text-To-Speech", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaAudioTTS->setToolTip(QApplication::translate("MainWindow", "Toggle Text-To-Speech", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaAudioTTS->setWhatsThis(QApplication::translate("MainWindow", "Enable or disable the text-to-speech engine. Only messages enabled for TTS in the Configuration dialog will actually be spoken.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaAudioStats->setText(QApplication::translate("MainWindow", "Audio S&tatistics", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaAudioStats->setToolTip(QApplication::translate("MainWindow", "Display audio statistics", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaAudioStats->setWhatsThis(QApplication::translate("MainWindow", "Pops up a small dialog with information about your current audio input.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaAudioUnlink->setText(QApplication::translate("MainWindow", "&Unlink Plugins", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaAudioUnlink->setToolTip(QApplication::translate("MainWindow", "Forcibly unlink plugin", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaAudioUnlink->setWhatsThis(QApplication::translate("MainWindow", "This forces the current plugin to unlink, which is handy if it is reading completely wrong data.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaConfigDialog->setText(QApplication::translate("MainWindow", "&Settings", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaConfigDialog->setToolTip(QApplication::translate("MainWindow", "Configure Mumble", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaConfigDialog->setWhatsThis(QApplication::translate("MainWindow", "Allows you to change most settings for Mumble.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaAudioWizard->setText(QApplication::translate("MainWindow", "&Audio Wizard", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaAudioWizard->setToolTip(QApplication::translate("MainWindow", "Start the audio configuration wizard", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaAudioWizard->setWhatsThis(QApplication::translate("MainWindow", "This will guide you through the process of configuring your audio hardware.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaHelpWhatsThis->setText(QApplication::translate("MainWindow", "&What's This?", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaHelpWhatsThis->setToolTip(QApplication::translate("MainWindow", "Enter What's This? mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaHelpWhatsThis->setWhatsThis(QApplication::translate("MainWindow", "Click this to enter \"What's This?\" mode. Your cursor will turn into a question mark. Click on any button, menu choice or area to show a description of what it is.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaHelpAbout->setText(QApplication::translate("MainWindow", "&About", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaHelpAbout->setToolTip(QApplication::translate("MainWindow", "Information about Mumble", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaHelpAbout->setWhatsThis(QApplication::translate("MainWindow", "Shows a small dialog with information and license for Mumble.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaHelpAboutSpeex->setText(QApplication::translate("MainWindow", "About &Speex", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaHelpAboutSpeex->setToolTip(QApplication::translate("MainWindow", "Information about Speex", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaHelpAboutSpeex->setWhatsThis(QApplication::translate("MainWindow", "Shows a small dialog with information about Speex.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaHelpAboutQt->setText(QApplication::translate("MainWindow", "About &Qt", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaHelpAboutQt->setToolTip(QApplication::translate("MainWindow", "Information about Qt", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaHelpAboutQt->setWhatsThis(QApplication::translate("MainWindow", "Shows a small dialog with information about Qt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaHelpVersionCheck->setText(QApplication::translate("MainWindow", "Check for &Updates", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaHelpVersionCheck->setToolTip(QApplication::translate("MainWindow", "Check for new version of Mumble", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaHelpVersionCheck->setWhatsThis(QApplication::translate("MainWindow", "Connects to the Mumble webpage to check if a new version is available, and notifies you with an appropriate download URL if this is the case.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaChannelSendMessage->setText(QApplication::translate("MainWindow", "Send &Message", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaChannelSendMessage->setToolTip(QApplication::translate("MainWindow", "Send a Text Message", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaChannelSendMessage->setWhatsThis(QApplication::translate("MainWindow", "Sends a text message to all users in a channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaConfigMinimal->setText(QApplication::translate("MainWindow", "&Minimal View", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaConfigMinimal->setToolTip(QApplication::translate("MainWindow", "Toggle minimal window modes", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaConfigMinimal->setWhatsThis(QApplication::translate("MainWindow", "This will toggle minimal mode, where the log window and menu is hidden.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaConfigHideFrame->setText(QApplication::translate("MainWindow", "Hide Frame", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaConfigHideFrame->setToolTip(QApplication::translate("MainWindow", "Toggle showing frame on minimal window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaConfigHideFrame->setWhatsThis(QApplication::translate("MainWindow", "This will toggle whether the minimal window should have a frame for moving and resizing.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaConfigCert->setText(QApplication::translate("MainWindow", "&Certificate Wizard", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaConfigCert->setToolTip(QApplication::translate("MainWindow", "Configure certificates for strong authentication", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaConfigCert->setWhatsThis(QApplication::translate("MainWindow", "This starts the wizard for creating, importing and exporting certificates for authentication against servers.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserRegister->setText(QApplication::translate("MainWindow", "&Register", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserRegister->setToolTip(QApplication::translate("MainWindow", "Register user on server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserRegister->setWhatsThis(QApplication::translate("MainWindow", "This will permanently register the user on the server.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserFriendAdd->setText(QApplication::translate("MainWindow", "Add &Friend", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserFriendAdd->setToolTip(QApplication::translate("MainWindow", "Adds a user as your friend.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserFriendAdd->setWhatsThis(QApplication::translate("MainWindow", "This will add the user as a friend, so you can recognize him on this and other servers.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserFriendRemove->setText(QApplication::translate("MainWindow", "&Remove Friend", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserFriendRemove->setToolTip(QApplication::translate("MainWindow", "Removes a user from your friends.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserFriendRemove->setWhatsThis(QApplication::translate("MainWindow", "This will remove a user from your friends list.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaUserFriendUpdate->setText(QApplication::translate("MainWindow", "&Update Friend", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserFriendUpdate->setToolTip(QApplication::translate("MainWindow", "Update name of your friend.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaUserFriendUpdate->setWhatsThis(QApplication::translate("MainWindow", "Your friend uses a different name than what is in your database. This will update the name.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaServerUserList->setText(QApplication::translate("MainWindow", "Registered &Users", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaServerUserList->setToolTip(QApplication::translate("MainWindow", "Edit registered users list", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qaServerUserList->setWhatsThis(QApplication::translate("MainWindow", "This opens the editor for registered users, which allow you to change their name or unregister them.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qaServerTexture->setText(QApplication::translate("MainWindow", "Change &Avatar", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaServerTexture->setToolTip(QApplication::translate("MainWindow", "Change your avatar image on this server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaServerTokens->setText(QApplication::translate("MainWindow", "&Access Tokens", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaServerTokens->setToolTip(QApplication::translate("MainWindow", "Add or remove text-based access tokens", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaServerTextureRemove->setText(QApplication::translate("MainWindow", "&Remove Avatar", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaServerTextureRemove->setToolTip(QApplication::translate("MainWindow", "Remove currently defined avatar image.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaUserCommentReset->setText(QApplication::translate("MainWindow", "Reset &Comment", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserCommentReset->setToolTip(QApplication::translate("MainWindow", "Reset the comment of the selected user.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaChannelJoin->setText(QApplication::translate("MainWindow", "&Join Channel", 0, QApplication::UnicodeUTF8));
        qaUserCommentView->setText(QApplication::translate("MainWindow", "View Comment", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserCommentView->setToolTip(QApplication::translate("MainWindow", "View comment in editor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaUserInformation->setText(QApplication::translate("MainWindow", "&Information", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaUserInformation->setToolTip(QApplication::translate("MainWindow", "Query server for connection information for user", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaSelfComment->setText(QApplication::translate("MainWindow", "&Change Comment", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaSelfComment->setToolTip(QApplication::translate("MainWindow", "Change your own comment", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaSelfRegister->setText(QApplication::translate("MainWindow", "R&egister", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaSelfRegister->setToolTip(QApplication::translate("MainWindow", "Register yourself on the server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaUserPrioritySpeaker->setText(QApplication::translate("MainWindow", "Priority Speaker", 0, QApplication::UnicodeUTF8));
        qaSelfPrioritySpeaker->setText(QApplication::translate("MainWindow", "Priority Speaker", 0, QApplication::UnicodeUTF8));
        qaRecording->setText(QApplication::translate("MainWindow", "Recording", 0, QApplication::UnicodeUTF8));
        qmConfig->setTitle(QApplication::translate("MainWindow", "C&onfigure", 0, QApplication::UnicodeUTF8));
        qmHelp->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
        qmServer->setTitle(QApplication::translate("MainWindow", "S&erver", 0, QApplication::UnicodeUTF8));
        qmSelf->setTitle(QApplication::translate("MainWindow", "&Self", 0, QApplication::UnicodeUTF8));
        qdwLog->setWindowTitle(QApplication::translate("MainWindow", "Log", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        qteLog->setWhatsThis(QApplication::translate("MainWindow", "This shows all recent activity. Connecting to servers, errors and information messages all show up here.<br />To configure exactly which messages show up here, use the <b>Settings</b> command from the menu.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifdef ADDITIONAL_WINDOWS
#ifndef QT_NO_TOOLTIP
        qdwChat->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qdwChat->setWhatsThis(QApplication::translate("MainWindow", "This is the chatbar<br />If you enter text here and then press enter the text is sent to the user or channel that was selected. If nothing is selected the message is sent to your current channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qdwChat->setWindowTitle(QApplication::translate("MainWindow", "Chatbar", 0, QApplication::UnicodeUTF8));
#endif
        qtIconToolbar->setWindowTitle(QApplication::translate("MainWindow", "Icon Toolbar", 0, QApplication::UnicodeUTF8));

    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
