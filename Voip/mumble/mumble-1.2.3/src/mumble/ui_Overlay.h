/********************************************************************************
** Form generated from reading UI file 'Overlay.ui'
**
** Created: Fri 1. Jun 10:08:04 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OVERLAY_H
#define UI_OVERLAY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGraphicsView>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OverlayConfig
{
public:
    QVBoxLayout *vboxLayout;
    QStackedWidget *qswOverlayPage;
    QWidget *qwOverlayConfig;
    QVBoxLayout *verticalLayout_4;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_9;
    QGroupBox *qgbOptions;
    QVBoxLayout *verticalLayout_15;
    QCheckBox *qcbEnable;
    QSpacerItem *horizontalSpacer_6;
    QVBoxLayout *verticalLayout_14;
    QPushButton *qpbLoadPreset;
    QPushButton *qpbSavePreset;
    QPushButton *qpbUninstall;
    QGroupBox *qgpFps;
    QHBoxLayout *horizontalLayout_8;
    QVBoxLayout *verticalLayout_9;
    QCheckBox *qcbShowFps;
    QPushButton *qpbFpsFont;
    QPushButton *qpbFpsColor;
    QSpacerItem *verticalSpacer_3;
    QGraphicsView *qgvFpsPreview;
    QTabWidget *qtwSetup;
    QWidget *qwLayoutTab;
    QVBoxLayout *verticalLayout_13;
    QGraphicsView *qgvView;
    QWidget *qwExceptions;
    QVBoxLayout *verticalLayout_16;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *qrbBlacklist;
    QRadioButton *qrbWhitelist;
    QStackedWidget *qswBlackWhiteList;
    QWidget *qwBlack;
    QVBoxLayout *verticalLayout_2;
    QListWidget *qlwBlacklist;
    QWidget *qwWhite;
    QVBoxLayout *verticalLayout_3;
    QListWidget *qlwWhitelist;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *qpbAdd;
    QPushButton *qpbRemove;
    QWidget *qwOverlayInstall;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_11;
    QHBoxLayout *horizontalLayout_5;
    QLabel *qlInstallIcon;
    QVBoxLayout *verticalLayout_6;
    QLabel *qlInstallText;
    QLabel *qlInstallValidityText;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *qpbInstall;
    QPushButton *qpbShowCerts;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_2;
    QWidget *qwOverlayUpgrade;
    QVBoxLayout *verticalLayout_7;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_3;
    QLabel *qlUpgradeIcon;
    QVBoxLayout *verticalLayout_8;
    QLabel *qlUpgradeText;
    QLabel *qlUpgradeValidityText;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *qpbUpgrade;
    QPushButton *qpbUpgradeShowCerts;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *OverlayConfig)
    {
        if (OverlayConfig->objectName().isEmpty())
            OverlayConfig->setObjectName(QString::fromUtf8("OverlayConfig"));
        OverlayConfig->resize(719, 881);
        OverlayConfig->setWindowTitle(QString::fromUtf8("Form"));
        vboxLayout = new QVBoxLayout(OverlayConfig);
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        qswOverlayPage = new QStackedWidget(OverlayConfig);
        qswOverlayPage->setObjectName(QString::fromUtf8("qswOverlayPage"));
        qwOverlayConfig = new QWidget();
        qwOverlayConfig->setObjectName(QString::fromUtf8("qwOverlayConfig"));
        verticalLayout_4 = new QVBoxLayout(qwOverlayConfig);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        widget = new QWidget(qwOverlayConfig);
        widget->setObjectName(QString::fromUtf8("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        horizontalLayout_9 = new QHBoxLayout(widget);
        horizontalLayout_9->setSpacing(-1);
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        horizontalLayout_9->setSizeConstraint(QLayout::SetDefaultConstraint);
        qgbOptions = new QGroupBox(widget);
        qgbOptions->setObjectName(QString::fromUtf8("qgbOptions"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qgbOptions->sizePolicy().hasHeightForWidth());
        qgbOptions->setSizePolicy(sizePolicy1);
        qgbOptions->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        verticalLayout_15 = new QVBoxLayout(qgbOptions);
        verticalLayout_15->setObjectName(QString::fromUtf8("verticalLayout_15"));
        qcbEnable = new QCheckBox(qgbOptions);
        qcbEnable->setObjectName(QString::fromUtf8("qcbEnable"));

        verticalLayout_15->addWidget(qcbEnable);

        horizontalSpacer_6 = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_15->addItem(horizontalSpacer_6);

        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        verticalLayout_14->setSizeConstraint(QLayout::SetDefaultConstraint);
        qpbLoadPreset = new QPushButton(qgbOptions);
        qpbLoadPreset->setObjectName(QString::fromUtf8("qpbLoadPreset"));

        verticalLayout_14->addWidget(qpbLoadPreset);

        qpbSavePreset = new QPushButton(qgbOptions);
        qpbSavePreset->setObjectName(QString::fromUtf8("qpbSavePreset"));

        verticalLayout_14->addWidget(qpbSavePreset);

        qpbUninstall = new QPushButton(qgbOptions);
        qpbUninstall->setObjectName(QString::fromUtf8("qpbUninstall"));

        verticalLayout_14->addWidget(qpbUninstall);


        verticalLayout_15->addLayout(verticalLayout_14);


        horizontalLayout_9->addWidget(qgbOptions);

        qgpFps = new QGroupBox(widget);
        qgpFps->setObjectName(QString::fromUtf8("qgpFps"));
        qgpFps->setEnabled(true);
        sizePolicy1.setHeightForWidth(qgpFps->sizePolicy().hasHeightForWidth());
        qgpFps->setSizePolicy(sizePolicy1);
        horizontalLayout_8 = new QHBoxLayout(qgpFps);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setSizeConstraint(QLayout::SetDefaultConstraint);
        qcbShowFps = new QCheckBox(qgpFps);
        qcbShowFps->setObjectName(QString::fromUtf8("qcbShowFps"));

        verticalLayout_9->addWidget(qcbShowFps);

        qpbFpsFont = new QPushButton(qgpFps);
        qpbFpsFont->setObjectName(QString::fromUtf8("qpbFpsFont"));

        verticalLayout_9->addWidget(qpbFpsFont);

        qpbFpsColor = new QPushButton(qgpFps);
        qpbFpsColor->setObjectName(QString::fromUtf8("qpbFpsColor"));

        verticalLayout_9->addWidget(qpbFpsColor);

        verticalSpacer_3 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_3);


        horizontalLayout_8->addLayout(verticalLayout_9);

        qgvFpsPreview = new QGraphicsView(qgpFps);
        qgvFpsPreview->setObjectName(QString::fromUtf8("qgvFpsPreview"));
        qgvFpsPreview->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(qgvFpsPreview->sizePolicy().hasHeightForWidth());
        qgvFpsPreview->setSizePolicy(sizePolicy2);
        qgvFpsPreview->setMaximumSize(QSize(16777215, 100));
        qgvFpsPreview->setBaseSize(QSize(0, 40));
        qgvFpsPreview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        qgvFpsPreview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QBrush brush(QColor(128, 128, 128, 255));
        brush.setStyle(Qt::SolidPattern);
        qgvFpsPreview->setBackgroundBrush(brush);
        QBrush brush1(QColor(0, 0, 0, 255));
        brush1.setStyle(Qt::NoBrush);
        qgvFpsPreview->setForegroundBrush(brush1);
        qgvFpsPreview->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

        horizontalLayout_8->addWidget(qgvFpsPreview);


        horizontalLayout_9->addWidget(qgpFps);


        verticalLayout_4->addWidget(widget);

        qtwSetup = new QTabWidget(qwOverlayConfig);
        qtwSetup->setObjectName(QString::fromUtf8("qtwSetup"));
        qwLayoutTab = new QWidget();
        qwLayoutTab->setObjectName(QString::fromUtf8("qwLayoutTab"));
        verticalLayout_13 = new QVBoxLayout(qwLayoutTab);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        qgvView = new QGraphicsView(qwLayoutTab);
        qgvView->setObjectName(QString::fromUtf8("qgvView"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(qgvView->sizePolicy().hasHeightForWidth());
        qgvView->setSizePolicy(sizePolicy3);
        qgvView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        qgvView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        qgvView->setBackgroundBrush(brush);
        qgvView->setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing|QPainter::SmoothPixmapTransform|QPainter::TextAntialiasing);

        verticalLayout_13->addWidget(qgvView);

        qtwSetup->addTab(qwLayoutTab, QString());
        qwExceptions = new QWidget();
        qwExceptions->setObjectName(QString::fromUtf8("qwExceptions"));
        verticalLayout_16 = new QVBoxLayout(qwExceptions);
        verticalLayout_16->setObjectName(QString::fromUtf8("verticalLayout_16"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        qrbBlacklist = new QRadioButton(qwExceptions);
        qrbBlacklist->setObjectName(QString::fromUtf8("qrbBlacklist"));
        qrbBlacklist->setChecked(true);

        horizontalLayout_2->addWidget(qrbBlacklist);

        qrbWhitelist = new QRadioButton(qwExceptions);
        qrbWhitelist->setObjectName(QString::fromUtf8("qrbWhitelist"));

        horizontalLayout_2->addWidget(qrbWhitelist);


        verticalLayout_16->addLayout(horizontalLayout_2);

        qswBlackWhiteList = new QStackedWidget(qwExceptions);
        qswBlackWhiteList->setObjectName(QString::fromUtf8("qswBlackWhiteList"));
        sizePolicy2.setHeightForWidth(qswBlackWhiteList->sizePolicy().hasHeightForWidth());
        qswBlackWhiteList->setSizePolicy(sizePolicy2);
        qwBlack = new QWidget();
        qwBlack->setObjectName(QString::fromUtf8("qwBlack"));
        sizePolicy2.setHeightForWidth(qwBlack->sizePolicy().hasHeightForWidth());
        qwBlack->setSizePolicy(sizePolicy2);
        verticalLayout_2 = new QVBoxLayout(qwBlack);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        qlwBlacklist = new QListWidget(qwBlack);
        qlwBlacklist->setObjectName(QString::fromUtf8("qlwBlacklist"));

        verticalLayout_2->addWidget(qlwBlacklist);

        qswBlackWhiteList->addWidget(qwBlack);
        qwWhite = new QWidget();
        qwWhite->setObjectName(QString::fromUtf8("qwWhite"));
        sizePolicy2.setHeightForWidth(qwWhite->sizePolicy().hasHeightForWidth());
        qwWhite->setSizePolicy(sizePolicy2);
        verticalLayout_3 = new QVBoxLayout(qwWhite);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        qlwWhitelist = new QListWidget(qwWhite);
        qlwWhitelist->setObjectName(QString::fromUtf8("qlwWhitelist"));

        verticalLayout_3->addWidget(qlwWhitelist);

        qswBlackWhiteList->addWidget(qwWhite);

        verticalLayout_16->addWidget(qswBlackWhiteList);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        qpbAdd = new QPushButton(qwExceptions);
        qpbAdd->setObjectName(QString::fromUtf8("qpbAdd"));

        horizontalLayout->addWidget(qpbAdd);

        qpbRemove = new QPushButton(qwExceptions);
        qpbRemove->setObjectName(QString::fromUtf8("qpbRemove"));

        horizontalLayout->addWidget(qpbRemove);


        verticalLayout_16->addLayout(horizontalLayout);

        qtwSetup->addTab(qwExceptions, QString());
        qswBlackWhiteList->raise();

        verticalLayout_4->addWidget(qtwSetup);

        qswOverlayPage->addWidget(qwOverlayConfig);
        qwOverlayInstall = new QWidget();
        qwOverlayInstall->setObjectName(QString::fromUtf8("qwOverlayInstall"));
        verticalLayout_5 = new QVBoxLayout(qwOverlayInstall);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        groupBox = new QGroupBox(qwOverlayInstall);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_11 = new QVBoxLayout(groupBox);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        qlInstallIcon = new QLabel(groupBox);
        qlInstallIcon->setObjectName(QString::fromUtf8("qlInstallIcon"));
        qlInstallIcon->setPixmap(QPixmap(QString::fromUtf8(":/config_osd.png")));

        horizontalLayout_5->addWidget(qlInstallIcon);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        qlInstallText = new QLabel(groupBox);
        qlInstallText->setObjectName(QString::fromUtf8("qlInstallText"));
        qlInstallText->setWordWrap(true);

        verticalLayout_6->addWidget(qlInstallText);

        qlInstallValidityText = new QLabel(groupBox);
        qlInstallValidityText->setObjectName(QString::fromUtf8("qlInstallValidityText"));

        verticalLayout_6->addWidget(qlInstallValidityText);


        horizontalLayout_5->addLayout(verticalLayout_6);


        verticalLayout_11->addLayout(horizontalLayout_5);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        qpbInstall = new QPushButton(groupBox);
        qpbInstall->setObjectName(QString::fromUtf8("qpbInstall"));

        horizontalLayout_4->addWidget(qpbInstall);

        qpbShowCerts = new QPushButton(groupBox);
        qpbShowCerts->setObjectName(QString::fromUtf8("qpbShowCerts"));

        horizontalLayout_4->addWidget(qpbShowCerts);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout_11->addLayout(horizontalLayout_4);


        verticalLayout_5->addWidget(groupBox);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_2);

        qswOverlayPage->addWidget(qwOverlayInstall);
        qwOverlayUpgrade = new QWidget();
        qwOverlayUpgrade->setObjectName(QString::fromUtf8("qwOverlayUpgrade"));
        verticalLayout_7 = new QVBoxLayout(qwOverlayUpgrade);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        groupBox_2 = new QGroupBox(qwOverlayUpgrade);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_12 = new QVBoxLayout(groupBox_2);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        qlUpgradeIcon = new QLabel(groupBox_2);
        qlUpgradeIcon->setObjectName(QString::fromUtf8("qlUpgradeIcon"));
        qlUpgradeIcon->setPixmap(QPixmap(QString::fromUtf8(":/config_osd.png")));

        horizontalLayout_3->addWidget(qlUpgradeIcon);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        qlUpgradeText = new QLabel(groupBox_2);
        qlUpgradeText->setObjectName(QString::fromUtf8("qlUpgradeText"));
        qlUpgradeText->setWordWrap(true);

        verticalLayout_8->addWidget(qlUpgradeText);

        qlUpgradeValidityText = new QLabel(groupBox_2);
        qlUpgradeValidityText->setObjectName(QString::fromUtf8("qlUpgradeValidityText"));

        verticalLayout_8->addWidget(qlUpgradeValidityText);


        horizontalLayout_3->addLayout(verticalLayout_8);


        verticalLayout_12->addLayout(horizontalLayout_3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        qpbUpgrade = new QPushButton(groupBox_2);
        qpbUpgrade->setObjectName(QString::fromUtf8("qpbUpgrade"));

        horizontalLayout_6->addWidget(qpbUpgrade);

        qpbUpgradeShowCerts = new QPushButton(groupBox_2);
        qpbUpgradeShowCerts->setObjectName(QString::fromUtf8("qpbUpgradeShowCerts"));

        horizontalLayout_6->addWidget(qpbUpgradeShowCerts);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);


        verticalLayout_12->addLayout(horizontalLayout_6);


        verticalLayout_7->addWidget(groupBox_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer);

        qswOverlayPage->addWidget(qwOverlayUpgrade);

        vboxLayout->addWidget(qswOverlayPage);


        retranslateUi(OverlayConfig);

        qswOverlayPage->setCurrentIndex(0);
        qtwSetup->setCurrentIndex(0);
        qswBlackWhiteList->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(OverlayConfig);
    } // setupUi

    void retranslateUi(QWidget *OverlayConfig)
    {
        qgbOptions->setTitle(QApplication::translate("OverlayConfig", "Options", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbEnable->setToolTip(QApplication::translate("OverlayConfig", "Enable overlay.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbEnable->setWhatsThis(QApplication::translate("OverlayConfig", "This sets whether the overlay is enabled or not. This setting is only checked when applications are started, so make sure Mumble is running and this option is on before you start the application.<br />Please note that if you start Mumble after starting the application, or if you disable the overlay while the application is running, there is no safe way to restart the overlay without also restarting the application.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbEnable->setText(QApplication::translate("OverlayConfig", "Enable Overlay", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbLoadPreset->setToolTip(QApplication::translate("OverlayConfig", "Load an overlay preset from file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qpbLoadPreset->setText(QApplication::translate("OverlayConfig", "Load\342\200\246", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbSavePreset->setToolTip(QApplication::translate("OverlayConfig", "Save your overlay settings to file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qpbSavePreset->setText(QApplication::translate("OverlayConfig", "Save\342\200\246", 0, QApplication::UnicodeUTF8));
        qpbUninstall->setText(QApplication::translate("OverlayConfig", "Uninstall Overlay", 0, QApplication::UnicodeUTF8));
        qgpFps->setTitle(QApplication::translate("OverlayConfig", "FPS Display", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbShowFps->setToolTip(QApplication::translate("OverlayConfig", "Display a frame counter in the overlay", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbShowFps->setText(QApplication::translate("OverlayConfig", "Show FPS counter", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbFpsFont->setToolTip(QApplication::translate("OverlayConfig", "Set the overlay font.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbFpsFont->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        qpbFpsFont->setText(QApplication::translate("OverlayConfig", "Font", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbFpsColor->setToolTip(QApplication::translate("OverlayConfig", "Set the overlay text color.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbFpsColor->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        qpbFpsColor->setText(QApplication::translate("OverlayConfig", "Color", 0, QApplication::UnicodeUTF8));
        qtwSetup->setTabText(qtwSetup->indexOf(qwLayoutTab), QApplication::translate("OverlayConfig", "Layout", 0, QApplication::UnicodeUTF8));
        qrbBlacklist->setText(QApplication::translate("OverlayConfig", "Blacklist", 0, QApplication::UnicodeUTF8));
        qrbWhitelist->setText(QApplication::translate("OverlayConfig", "Whitelist", 0, QApplication::UnicodeUTF8));
        qpbAdd->setText(QApplication::translate("OverlayConfig", "Add...", 0, QApplication::UnicodeUTF8));
        qpbRemove->setText(QApplication::translate("OverlayConfig", "Remove", 0, QApplication::UnicodeUTF8));
        qtwSetup->setTabText(qtwSetup->indexOf(qwExceptions), QApplication::translate("OverlayConfig", "Overlay exceptions", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("OverlayConfig", "Overlay Installation", 0, QApplication::UnicodeUTF8));
        qlInstallIcon->setText(QString());
        qlInstallText->setText(QApplication::translate("OverlayConfig", "Mumble has detected that you do not have the Mumble Overlay installed.\n"
"\n"
"Click the button below to install the overlay.", 0, QApplication::UnicodeUTF8));
        qlInstallValidityText->setText(QString());
        qpbInstall->setText(QApplication::translate("OverlayConfig", "Install Mumble Overlay", 0, QApplication::UnicodeUTF8));
        qpbShowCerts->setText(QApplication::translate("OverlayConfig", "View Certificates", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("OverlayConfig", "Overlay Upgrade", 0, QApplication::UnicodeUTF8));
        qlUpgradeIcon->setText(QString());
        qlUpgradeText->setText(QApplication::translate("OverlayConfig", "Mumble has detected an old version of the overlay support files installed on your computer.\n"
"\n"
"To upgrade these files to their latest versions, click the button below.", 0, QApplication::UnicodeUTF8));
        qlUpgradeValidityText->setText(QString());
        qpbUpgrade->setText(QApplication::translate("OverlayConfig", "Upgrade Mumble Overlay", 0, QApplication::UnicodeUTF8));
        qpbUpgradeShowCerts->setText(QApplication::translate("OverlayConfig", "View Certificates", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(OverlayConfig);
    } // retranslateUi

};

namespace Ui {
    class OverlayConfig: public Ui_OverlayConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OVERLAY_H
