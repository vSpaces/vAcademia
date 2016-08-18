/********************************************************************************
** Form generated from reading UI file 'NetworkConfig.ui'
**
** Created: Fri 1. Jun 10:05:08 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NETWORKCONFIG_H
#define UI_NETWORKCONFIG_H

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
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NetworkConfig
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *qgbConnection;
    QVBoxLayout *vboxLayout;
    QCheckBox *qcbTcpMode;
    QCheckBox *qcbQoS;
    QCheckBox *qcbAutoReconnect;
    QCheckBox *qcbAutoConnect;
    QCheckBox *qcbSuppressIdentity;
    QGroupBox *qgbProxy;
    QGridLayout *gridLayout;
    QLabel *qlType;
    QComboBox *qcbType;
    QLabel *qlHostname;
    QLineEdit *qleHostname;
    QLabel *qlPort;
    QLineEdit *qlePort;
    QLabel *qlUsername;
    QLineEdit *qleUsername;
    QLabel *qlPassword;
    QLineEdit *qlePassword;
    QGroupBox *qgbMisc;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *qcbImageDownload;
    QGroupBox *qgbServices;
    QVBoxLayout *verticalLayout;
    QCheckBox *qcbAutoUpdate;
    QCheckBox *qcbPluginUpdate;
    QCheckBox *qcbUsage;
    QSpacerItem *spacerItem;

    void setupUi(QWidget *NetworkConfig)
    {
        if (NetworkConfig->objectName().isEmpty())
            NetworkConfig->setObjectName(QString::fromUtf8("NetworkConfig"));
        NetworkConfig->resize(576, 567);
        NetworkConfig->setWindowTitle(QString::fromUtf8("Form"));
        verticalLayout_2 = new QVBoxLayout(NetworkConfig);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        qgbConnection = new QGroupBox(NetworkConfig);
        qgbConnection->setObjectName(QString::fromUtf8("qgbConnection"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qgbConnection->sizePolicy().hasHeightForWidth());
        qgbConnection->setSizePolicy(sizePolicy);
        vboxLayout = new QVBoxLayout(qgbConnection);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        qcbTcpMode = new QCheckBox(qgbConnection);
        qcbTcpMode->setObjectName(QString::fromUtf8("qcbTcpMode"));

        vboxLayout->addWidget(qcbTcpMode);

        qcbQoS = new QCheckBox(qgbConnection);
        qcbQoS->setObjectName(QString::fromUtf8("qcbQoS"));

        vboxLayout->addWidget(qcbQoS);

        qcbAutoReconnect = new QCheckBox(qgbConnection);
        qcbAutoReconnect->setObjectName(QString::fromUtf8("qcbAutoReconnect"));

        vboxLayout->addWidget(qcbAutoReconnect);

        qcbAutoConnect = new QCheckBox(qgbConnection);
        qcbAutoConnect->setObjectName(QString::fromUtf8("qcbAutoConnect"));

        vboxLayout->addWidget(qcbAutoConnect);

        qcbSuppressIdentity = new QCheckBox(qgbConnection);
        qcbSuppressIdentity->setObjectName(QString::fromUtf8("qcbSuppressIdentity"));

        vboxLayout->addWidget(qcbSuppressIdentity);


        verticalLayout_2->addWidget(qgbConnection);

        qgbProxy = new QGroupBox(NetworkConfig);
        qgbProxy->setObjectName(QString::fromUtf8("qgbProxy"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qgbProxy->sizePolicy().hasHeightForWidth());
        qgbProxy->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(qgbProxy);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qlType = new QLabel(qgbProxy);
        qlType->setObjectName(QString::fromUtf8("qlType"));

        gridLayout->addWidget(qlType, 0, 0, 1, 1);

        qcbType = new QComboBox(qgbProxy);
        qcbType->setObjectName(QString::fromUtf8("qcbType"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(qcbType->sizePolicy().hasHeightForWidth());
        qcbType->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(qcbType, 0, 1, 1, 3);

        qlHostname = new QLabel(qgbProxy);
        qlHostname->setObjectName(QString::fromUtf8("qlHostname"));
        qlHostname->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(qlHostname, 1, 0, 1, 1);

        qleHostname = new QLineEdit(qgbProxy);
        qleHostname->setObjectName(QString::fromUtf8("qleHostname"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(qleHostname->sizePolicy().hasHeightForWidth());
        qleHostname->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(qleHostname, 1, 1, 1, 1);

        qlPort = new QLabel(qgbProxy);
        qlPort->setObjectName(QString::fromUtf8("qlPort"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(qlPort->sizePolicy().hasHeightForWidth());
        qlPort->setSizePolicy(sizePolicy4);
        qlPort->setMinimumSize(QSize(0, 0));

        gridLayout->addWidget(qlPort, 1, 2, 1, 1);

        qlePort = new QLineEdit(qgbProxy);
        qlePort->setObjectName(QString::fromUtf8("qlePort"));
        QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(qlePort->sizePolicy().hasHeightForWidth());
        qlePort->setSizePolicy(sizePolicy5);
        qlePort->setMinimumSize(QSize(20, 0));
        qlePort->setMaximumSize(QSize(60, 16777215));
        qlePort->setMaxLength(5);

        gridLayout->addWidget(qlePort, 1, 3, 1, 1);

        qlUsername = new QLabel(qgbProxy);
        qlUsername->setObjectName(QString::fromUtf8("qlUsername"));

        gridLayout->addWidget(qlUsername, 2, 0, 1, 1);

        qleUsername = new QLineEdit(qgbProxy);
        qleUsername->setObjectName(QString::fromUtf8("qleUsername"));

        gridLayout->addWidget(qleUsername, 2, 1, 1, 3);

        qlPassword = new QLabel(qgbProxy);
        qlPassword->setObjectName(QString::fromUtf8("qlPassword"));

        gridLayout->addWidget(qlPassword, 3, 0, 1, 1);

        qlePassword = new QLineEdit(qgbProxy);
        qlePassword->setObjectName(QString::fromUtf8("qlePassword"));
        qlePassword->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(qlePassword, 3, 1, 1, 3);


        verticalLayout_2->addWidget(qgbProxy);

        qgbMisc = new QGroupBox(NetworkConfig);
        qgbMisc->setObjectName(QString::fromUtf8("qgbMisc"));
        verticalLayout_3 = new QVBoxLayout(qgbMisc);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        qcbImageDownload = new QCheckBox(qgbMisc);
        qcbImageDownload->setObjectName(QString::fromUtf8("qcbImageDownload"));

        verticalLayout_3->addWidget(qcbImageDownload);


        verticalLayout_2->addWidget(qgbMisc);

        qgbServices = new QGroupBox(NetworkConfig);
        qgbServices->setObjectName(QString::fromUtf8("qgbServices"));
        verticalLayout = new QVBoxLayout(qgbServices);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qcbAutoUpdate = new QCheckBox(qgbServices);
        qcbAutoUpdate->setObjectName(QString::fromUtf8("qcbAutoUpdate"));

        verticalLayout->addWidget(qcbAutoUpdate);

        qcbPluginUpdate = new QCheckBox(qgbServices);
        qcbPluginUpdate->setObjectName(QString::fromUtf8("qcbPluginUpdate"));

        verticalLayout->addWidget(qcbPluginUpdate);

        qcbUsage = new QCheckBox(qgbServices);
        qcbUsage->setObjectName(QString::fromUtf8("qcbUsage"));

        verticalLayout->addWidget(qcbUsage);


        verticalLayout_2->addWidget(qgbServices);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(spacerItem);


        retranslateUi(NetworkConfig);

        QMetaObject::connectSlotsByName(NetworkConfig);
    } // setupUi

    void retranslateUi(QWidget *NetworkConfig)
    {
        qgbConnection->setTitle(QApplication::translate("NetworkConfig", "Connection", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbTcpMode->setToolTip(QApplication::translate("NetworkConfig", "Use TCP compatibility mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbTcpMode->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Enable TCP compatibility mode</b>.<br />This will make Mumble use only TCP when communicating with the server. This will increase overhead and cause lost packets to produce noticeable pauses in communication, so this should only be used if you are unable to use the default (which uses UDP for voice and TCP for control).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbTcpMode->setText(QApplication::translate("NetworkConfig", "Force TCP mode", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbQoS->setToolTip(QApplication::translate("NetworkConfig", "Enable QoS to prioritize packets", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbQoS->setWhatsThis(QApplication::translate("NetworkConfig", "This will enable QoS, which will attempt to prioritize voice packets over other traffic.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbQoS->setText(QApplication::translate("NetworkConfig", "Use Quality of Service", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbAutoReconnect->setToolTip(QApplication::translate("NetworkConfig", "Reconnect when disconnected", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbAutoReconnect->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Reconnect when disconnected</b>.<br />This will make Mumble try to automatically reconnect after 10 seconds if your server connection fails.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbAutoReconnect->setText(QApplication::translate("NetworkConfig", "Reconnect automatically", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbAutoConnect->setToolTip(QApplication::translate("NetworkConfig", "Reconnect to last used server when starting Mumble", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbAutoConnect->setText(QApplication::translate("NetworkConfig", "Reconnect to last server on startup", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbSuppressIdentity->setToolTip(QApplication::translate("NetworkConfig", "Don't send certificate to server and don't save passwords. (Not saved).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbSuppressIdentity->setWhatsThis(QApplication::translate("NetworkConfig", "<b>This will suppress identity information from the client.</b><p>The client will not identify itself with a certificate, even if defined, and will not cache passwords for connections. This is primarily a test-option and is not saved.</p>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbSuppressIdentity->setText(QApplication::translate("NetworkConfig", "Suppress certificate and password storage", 0, QApplication::UnicodeUTF8));
        qgbProxy->setTitle(QApplication::translate("NetworkConfig", "Proxy", 0, QApplication::UnicodeUTF8));
        qlType->setText(QApplication::translate("NetworkConfig", "Type", 0, QApplication::UnicodeUTF8));
        qcbType->clear();
        qcbType->insertItems(0, QStringList()
         << QApplication::translate("NetworkConfig", "Direct connection", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "HTTP(S) proxy", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("NetworkConfig", "SOCKS5 proxy", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        qcbType->setToolTip(QApplication::translate("NetworkConfig", "Type of proxy to connect through", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbType->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Type of proxy to connect through.</b><br />This makes Mumble connect through a proxy for all outgoing connections. Note: Proxy tunneling forces Mumble into TCP compatibility mode, causing all voice data to be sent via the control channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlHostname->setText(QApplication::translate("NetworkConfig", "Hostname", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleHostname->setToolTip(QApplication::translate("NetworkConfig", "Hostname of the proxy", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qleHostname->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Hostname of the proxy.</b><br />This field specifies the hostname of the proxy you wish to tunnel network traffic through.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qleHostname->setText(QString());
        qlPort->setText(QApplication::translate("NetworkConfig", "Port", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlePort->setToolTip(QApplication::translate("NetworkConfig", "Port number of the proxy", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlePort->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Port number of the proxy.</b><br />This field specifies the port number that the proxy expects connections on.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlePort->setInputMask(QString());
        qlePort->setText(QString());
        qlUsername->setText(QApplication::translate("NetworkConfig", "Username", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleUsername->setToolTip(QApplication::translate("NetworkConfig", "Username for proxy authentication", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qleUsername->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Username for proxy authentication.</b><br />This specifies the username you use for authenticating yourself with the proxy. In case the proxy does not use authentication, or you want to connect anonymously, simply leave this field blank.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlPassword->setText(QApplication::translate("NetworkConfig", "Password", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlePassword->setToolTip(QApplication::translate("NetworkConfig", "Password for proxy authentication", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlePassword->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Password for proxy authentication.</b><br />This specifies the password you use for authenticating yourself with the proxy. In case the proxy does not use authentication, or you want to connect anonymously, simply leave this field blank.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlePassword->setInputMask(QString());
        qlePassword->setText(QString());
        qgbMisc->setTitle(QApplication::translate("NetworkConfig", "Misc", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbImageDownload->setToolTip(QApplication::translate("NetworkConfig", "Prevent log from downloading images", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbImageDownload->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Disable image download</b><br/>\n"
"Prevents the client from downloading images embedded into chat messages with the img tag.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbImageDownload->setText(QApplication::translate("NetworkConfig", "Disable image download", 0, QApplication::UnicodeUTF8));
        qgbServices->setTitle(QApplication::translate("NetworkConfig", "Mumble services", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbAutoUpdate->setToolTip(QApplication::translate("NetworkConfig", "Check for new releases of Mumble automatically.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbAutoUpdate->setWhatsThis(QApplication::translate("NetworkConfig", "This will check for new releases of Mumble every time you start the program, and notify you if one is available.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbAutoUpdate->setText(QApplication::translate("NetworkConfig", "Check for application updates on startup", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbPluginUpdate->setToolTip(QApplication::translate("NetworkConfig", "Check for new releases of plugins automatically.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbPluginUpdate->setWhatsThis(QApplication::translate("NetworkConfig", "This will check for new releases of plugins every time you start the program, and download them automatically.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbPluginUpdate->setText(QApplication::translate("NetworkConfig", "Download plugin and overlay updates on startup", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbUsage->setToolTip(QApplication::translate("NetworkConfig", "Submit anonymous statistics to the Mumble project", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbUsage->setWhatsThis(QApplication::translate("NetworkConfig", "<b>Submit anonymous statistics.</b><br />Mumble has a small development team, and as such needs to focus its development where it is needed most. By submitting a bit of statistics you help the project determine where to focus development.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbUsage->setText(QApplication::translate("NetworkConfig", "Submit anonymous statistics", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(NetworkConfig);
    } // retranslateUi

};

namespace Ui {
    class NetworkConfig: public Ui_NetworkConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NETWORKCONFIG_H
