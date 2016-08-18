/********************************************************************************
** Form generated from reading UI file 'UserInformation.ui'
**
** Created: Fri 1. Jun 10:05:10 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_USERINFORMATION_H
#define UI_USERINFORMATION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UserInformation
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *qgbConnection;
    QGridLayout *gridLayout;
    QLabel *qliVersion;
    QLabel *qlVersion;
    QLabel *qliOS;
    QLabel *qlOS;
    QLabel *qliCertificate;
    QLabel *qlCertificate;
    QLabel *qliAddress;
    QLabel *qlAddress;
    QLabel *qliCELT;
    QLabel *qlCELT;
    QPushButton *qpbCertificate;
    QGroupBox *qgbPing;
    QGridLayout *gridLayout_2;
    QLabel *qliPingCount;
    QLabel *qliPingAvg;
    QLabel *qliPingVar;
    QLabel *qliTCP;
    QLabel *qlTCPCount;
    QLabel *qlTCPAvg;
    QLabel *qlTCPVar;
    QLabel *qliUDP;
    QLabel *qlUDPCount;
    QLabel *qlUDPAvg;
    QLabel *qlUDPVar;
    QGroupBox *qgbUDP;
    QGridLayout *gridLayout_3;
    QLabel *qliGood;
    QLabel *qliLate;
    QLabel *qliLost;
    QLabel *qliResync;
    QLabel *qliFromClient;
    QLabel *qlFromGood;
    QLabel *qlFromLate;
    QLabel *qlFromLost;
    QLabel *qlFromResync;
    QLabel *qliToClient;
    QLabel *qlToGood;
    QLabel *qlToLate;
    QLabel *qlToLost;
    QLabel *qlToResync;
    QGroupBox *qgbBandwidth;
    QGridLayout *gridLayout_4;
    QLabel *qliTime;
    QLabel *qlTime;
    QLabel *qliBandwidth;
    QLabel *qlBandwidth;

    void setupUi(QDialog *UserInformation)
    {
        if (UserInformation->objectName().isEmpty())
            UserInformation->setObjectName(QString::fromUtf8("UserInformation"));
        UserInformation->resize(379, 401);
        verticalLayout = new QVBoxLayout(UserInformation);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qgbConnection = new QGroupBox(UserInformation);
        qgbConnection->setObjectName(QString::fromUtf8("qgbConnection"));
        gridLayout = new QGridLayout(qgbConnection);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qliVersion = new QLabel(qgbConnection);
        qliVersion->setObjectName(QString::fromUtf8("qliVersion"));

        gridLayout->addWidget(qliVersion, 0, 0, 1, 1);

        qlVersion = new QLabel(qgbConnection);
        qlVersion->setObjectName(QString::fromUtf8("qlVersion"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qlVersion->sizePolicy().hasHeightForWidth());
        qlVersion->setSizePolicy(sizePolicy);
        qlVersion->setTextFormat(Qt::PlainText);
        qlVersion->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(qlVersion, 0, 1, 1, 2);

        qliOS = new QLabel(qgbConnection);
        qliOS->setObjectName(QString::fromUtf8("qliOS"));

        gridLayout->addWidget(qliOS, 1, 0, 1, 1);

        qlOS = new QLabel(qgbConnection);
        qlOS->setObjectName(QString::fromUtf8("qlOS"));
        sizePolicy.setHeightForWidth(qlOS->sizePolicy().hasHeightForWidth());
        qlOS->setSizePolicy(sizePolicy);
        qlOS->setTextFormat(Qt::PlainText);
        qlOS->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(qlOS, 1, 1, 1, 2);

        qliCertificate = new QLabel(qgbConnection);
        qliCertificate->setObjectName(QString::fromUtf8("qliCertificate"));

        gridLayout->addWidget(qliCertificate, 3, 0, 1, 1);

        qlCertificate = new QLabel(qgbConnection);
        qlCertificate->setObjectName(QString::fromUtf8("qlCertificate"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qlCertificate->sizePolicy().hasHeightForWidth());
        qlCertificate->setSizePolicy(sizePolicy1);
        qlCertificate->setTextFormat(Qt::PlainText);
        qlCertificate->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(qlCertificate, 3, 1, 1, 1);

        qliAddress = new QLabel(qgbConnection);
        qliAddress->setObjectName(QString::fromUtf8("qliAddress"));

        gridLayout->addWidget(qliAddress, 4, 0, 1, 1);

        qlAddress = new QLabel(qgbConnection);
        qlAddress->setObjectName(QString::fromUtf8("qlAddress"));
        sizePolicy.setHeightForWidth(qlAddress->sizePolicy().hasHeightForWidth());
        qlAddress->setSizePolicy(sizePolicy);
        qlAddress->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(qlAddress, 4, 1, 1, 2);

        qliCELT = new QLabel(qgbConnection);
        qliCELT->setObjectName(QString::fromUtf8("qliCELT"));

        gridLayout->addWidget(qliCELT, 5, 0, 1, 1);

        qlCELT = new QLabel(qgbConnection);
        qlCELT->setObjectName(QString::fromUtf8("qlCELT"));
        sizePolicy.setHeightForWidth(qlCELT->sizePolicy().hasHeightForWidth());
        qlCELT->setSizePolicy(sizePolicy);
        qlCELT->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(qlCELT, 5, 1, 1, 2);

        qpbCertificate = new QPushButton(qgbConnection);
        qpbCertificate->setObjectName(QString::fromUtf8("qpbCertificate"));
        qpbCertificate->setEnabled(false);
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(qpbCertificate->sizePolicy().hasHeightForWidth());
        qpbCertificate->setSizePolicy(sizePolicy2);
        qpbCertificate->setMaximumSize(QSize(150, 16777215));

        gridLayout->addWidget(qpbCertificate, 3, 2, 1, 1);


        verticalLayout->addWidget(qgbConnection);

        qgbPing = new QGroupBox(UserInformation);
        qgbPing->setObjectName(QString::fromUtf8("qgbPing"));
        gridLayout_2 = new QGridLayout(qgbPing);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        qliPingCount = new QLabel(qgbPing);
        qliPingCount->setObjectName(QString::fromUtf8("qliPingCount"));
        qliPingCount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(qliPingCount, 0, 1, 1, 1);

        qliPingAvg = new QLabel(qgbPing);
        qliPingAvg->setObjectName(QString::fromUtf8("qliPingAvg"));
        qliPingAvg->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(qliPingAvg, 0, 2, 1, 1);

        qliPingVar = new QLabel(qgbPing);
        qliPingVar->setObjectName(QString::fromUtf8("qliPingVar"));
        qliPingVar->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(qliPingVar, 0, 3, 1, 1);

        qliTCP = new QLabel(qgbPing);
        qliTCP->setObjectName(QString::fromUtf8("qliTCP"));

        gridLayout_2->addWidget(qliTCP, 1, 0, 1, 1);

        qlTCPCount = new QLabel(qgbPing);
        qlTCPCount->setObjectName(QString::fromUtf8("qlTCPCount"));
        qlTCPCount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlTCPCount->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(qlTCPCount, 1, 1, 1, 1);

        qlTCPAvg = new QLabel(qgbPing);
        qlTCPAvg->setObjectName(QString::fromUtf8("qlTCPAvg"));
        qlTCPAvg->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlTCPAvg->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(qlTCPAvg, 1, 2, 1, 1);

        qlTCPVar = new QLabel(qgbPing);
        qlTCPVar->setObjectName(QString::fromUtf8("qlTCPVar"));
        qlTCPVar->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlTCPVar->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(qlTCPVar, 1, 3, 1, 1);

        qliUDP = new QLabel(qgbPing);
        qliUDP->setObjectName(QString::fromUtf8("qliUDP"));

        gridLayout_2->addWidget(qliUDP, 2, 0, 1, 1);

        qlUDPCount = new QLabel(qgbPing);
        qlUDPCount->setObjectName(QString::fromUtf8("qlUDPCount"));
        qlUDPCount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlUDPCount->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(qlUDPCount, 2, 1, 1, 1);

        qlUDPAvg = new QLabel(qgbPing);
        qlUDPAvg->setObjectName(QString::fromUtf8("qlUDPAvg"));
        qlUDPAvg->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlUDPAvg->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(qlUDPAvg, 2, 2, 1, 1);

        qlUDPVar = new QLabel(qgbPing);
        qlUDPVar->setObjectName(QString::fromUtf8("qlUDPVar"));
        qlUDPVar->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlUDPVar->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(qlUDPVar, 2, 3, 1, 1);


        verticalLayout->addWidget(qgbPing);

        qgbUDP = new QGroupBox(UserInformation);
        qgbUDP->setObjectName(QString::fromUtf8("qgbUDP"));
        gridLayout_3 = new QGridLayout(qgbUDP);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        qliGood = new QLabel(qgbUDP);
        qliGood->setObjectName(QString::fromUtf8("qliGood"));
        qliGood->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(qliGood, 0, 1, 1, 1);

        qliLate = new QLabel(qgbUDP);
        qliLate->setObjectName(QString::fromUtf8("qliLate"));
        qliLate->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(qliLate, 0, 2, 1, 1);

        qliLost = new QLabel(qgbUDP);
        qliLost->setObjectName(QString::fromUtf8("qliLost"));
        qliLost->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(qliLost, 0, 3, 1, 1);

        qliResync = new QLabel(qgbUDP);
        qliResync->setObjectName(QString::fromUtf8("qliResync"));
        qliResync->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(qliResync, 0, 4, 1, 1);

        qliFromClient = new QLabel(qgbUDP);
        qliFromClient->setObjectName(QString::fromUtf8("qliFromClient"));

        gridLayout_3->addWidget(qliFromClient, 1, 0, 1, 1);

        qlFromGood = new QLabel(qgbUDP);
        qlFromGood->setObjectName(QString::fromUtf8("qlFromGood"));
        qlFromGood->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlFromGood->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_3->addWidget(qlFromGood, 1, 1, 1, 1);

        qlFromLate = new QLabel(qgbUDP);
        qlFromLate->setObjectName(QString::fromUtf8("qlFromLate"));
        qlFromLate->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlFromLate->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_3->addWidget(qlFromLate, 1, 2, 1, 1);

        qlFromLost = new QLabel(qgbUDP);
        qlFromLost->setObjectName(QString::fromUtf8("qlFromLost"));
        qlFromLost->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlFromLost->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_3->addWidget(qlFromLost, 1, 3, 1, 1);

        qlFromResync = new QLabel(qgbUDP);
        qlFromResync->setObjectName(QString::fromUtf8("qlFromResync"));
        qlFromResync->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlFromResync->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_3->addWidget(qlFromResync, 1, 4, 1, 1);

        qliToClient = new QLabel(qgbUDP);
        qliToClient->setObjectName(QString::fromUtf8("qliToClient"));

        gridLayout_3->addWidget(qliToClient, 2, 0, 1, 1);

        qlToGood = new QLabel(qgbUDP);
        qlToGood->setObjectName(QString::fromUtf8("qlToGood"));
        qlToGood->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlToGood->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_3->addWidget(qlToGood, 2, 1, 1, 1);

        qlToLate = new QLabel(qgbUDP);
        qlToLate->setObjectName(QString::fromUtf8("qlToLate"));
        qlToLate->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlToLate->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_3->addWidget(qlToLate, 2, 2, 1, 1);

        qlToLost = new QLabel(qgbUDP);
        qlToLost->setObjectName(QString::fromUtf8("qlToLost"));
        qlToLost->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlToLost->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_3->addWidget(qlToLost, 2, 3, 1, 1);

        qlToResync = new QLabel(qgbUDP);
        qlToResync->setObjectName(QString::fromUtf8("qlToResync"));
        qlToResync->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        qlToResync->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_3->addWidget(qlToResync, 2, 4, 1, 1);


        verticalLayout->addWidget(qgbUDP);

        qgbBandwidth = new QGroupBox(UserInformation);
        qgbBandwidth->setObjectName(QString::fromUtf8("qgbBandwidth"));
        gridLayout_4 = new QGridLayout(qgbBandwidth);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        qliTime = new QLabel(qgbBandwidth);
        qliTime->setObjectName(QString::fromUtf8("qliTime"));

        gridLayout_4->addWidget(qliTime, 0, 0, 1, 1);

        qlTime = new QLabel(qgbBandwidth);
        qlTime->setObjectName(QString::fromUtf8("qlTime"));
        sizePolicy.setHeightForWidth(qlTime->sizePolicy().hasHeightForWidth());
        qlTime->setSizePolicy(sizePolicy);
        qlTime->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_4->addWidget(qlTime, 0, 1, 1, 1);

        qliBandwidth = new QLabel(qgbBandwidth);
        qliBandwidth->setObjectName(QString::fromUtf8("qliBandwidth"));

        gridLayout_4->addWidget(qliBandwidth, 1, 0, 1, 1);

        qlBandwidth = new QLabel(qgbBandwidth);
        qlBandwidth->setObjectName(QString::fromUtf8("qlBandwidth"));
        sizePolicy.setHeightForWidth(qlBandwidth->sizePolicy().hasHeightForWidth());
        qlBandwidth->setSizePolicy(sizePolicy);
        qlBandwidth->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_4->addWidget(qlBandwidth, 1, 1, 1, 1);


        verticalLayout->addWidget(qgbBandwidth);


        retranslateUi(UserInformation);

        QMetaObject::connectSlotsByName(UserInformation);
    } // setupUi

    void retranslateUi(QDialog *UserInformation)
    {
        UserInformation->setWindowTitle(QApplication::translate("UserInformation", "User Information", 0, QApplication::UnicodeUTF8));
        qgbConnection->setTitle(QApplication::translate("UserInformation", "Connection Information", 0, QApplication::UnicodeUTF8));
        qliVersion->setText(QApplication::translate("UserInformation", "Version", 0, QApplication::UnicodeUTF8));
        qlVersion->setText(QString());
        qliOS->setText(QApplication::translate("UserInformation", "OS", 0, QApplication::UnicodeUTF8));
        qlOS->setText(QString());
        qliCertificate->setText(QApplication::translate("UserInformation", "Certificate", 0, QApplication::UnicodeUTF8));
        qlCertificate->setText(QString());
        qliAddress->setText(QApplication::translate("UserInformation", "IP Address", 0, QApplication::UnicodeUTF8));
        qlAddress->setText(QString());
        qliCELT->setText(QApplication::translate("UserInformation", "CELT Versions", 0, QApplication::UnicodeUTF8));
        qlCELT->setText(QString());
        qpbCertificate->setText(QApplication::translate("UserInformation", "Details...", 0, QApplication::UnicodeUTF8));
        qgbPing->setTitle(QApplication::translate("UserInformation", "Ping Statistics", 0, QApplication::UnicodeUTF8));
        qliPingCount->setText(QApplication::translate("UserInformation", "Pings received", 0, QApplication::UnicodeUTF8));
        qliPingAvg->setText(QApplication::translate("UserInformation", "Average ping", 0, QApplication::UnicodeUTF8));
        qliPingVar->setText(QApplication::translate("UserInformation", "Ping deviation", 0, QApplication::UnicodeUTF8));
        qliTCP->setText(QApplication::translate("UserInformation", "TCP (Control)", 0, QApplication::UnicodeUTF8));
        qlTCPCount->setText(QString());
        qlTCPAvg->setText(QString());
        qlTCPVar->setText(QString());
        qliUDP->setText(QApplication::translate("UserInformation", "UDP (Voice)", 0, QApplication::UnicodeUTF8));
        qlUDPCount->setText(QString());
        qlUDPAvg->setText(QString());
        qlUDPVar->setText(QString());
        qgbUDP->setTitle(QApplication::translate("UserInformation", "UDP Network statistics", 0, QApplication::UnicodeUTF8));
        qliGood->setText(QApplication::translate("UserInformation", "Good", 0, QApplication::UnicodeUTF8));
        qliLate->setText(QApplication::translate("UserInformation", "Late", 0, QApplication::UnicodeUTF8));
        qliLost->setText(QApplication::translate("UserInformation", "Lost", 0, QApplication::UnicodeUTF8));
        qliResync->setText(QApplication::translate("UserInformation", "Resync", 0, QApplication::UnicodeUTF8));
        qliFromClient->setText(QApplication::translate("UserInformation", "From Client", 0, QApplication::UnicodeUTF8));
        qlFromGood->setText(QString());
        qlFromLate->setText(QString());
        qlFromLost->setText(QString());
        qlFromResync->setText(QString());
        qliToClient->setText(QApplication::translate("UserInformation", "To Client", 0, QApplication::UnicodeUTF8));
        qlToGood->setText(QString());
        qlToLate->setText(QString());
        qlToLost->setText(QString());
        qlToResync->setText(QString());
        qgbBandwidth->setTitle(QApplication::translate("UserInformation", "Bandwidth", "GroupBox", QApplication::UnicodeUTF8));
        qliTime->setText(QApplication::translate("UserInformation", "Connection time", 0, QApplication::UnicodeUTF8));
        qlTime->setText(QString());
        qliBandwidth->setText(QApplication::translate("UserInformation", "Bandwidth", "Label", QApplication::UnicodeUTF8));
        qlBandwidth->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class UserInformation: public Ui_UserInformation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERINFORMATION_H
#endif