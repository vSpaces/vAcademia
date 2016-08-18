/********************************************************************************
** Form generated from reading UI file 'ConnectDialogEdit.ui'
**
** Created: Fri 1. Jun 10:05:06 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_CONNECTDIALOGEDIT_H
#define UI_CONNECTDIALOGEDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_ConnectDialogEdit
{
public:
    QGridLayout *gridLayout;
    QLabel *qliName;
    QLineEdit *qleName;
    QLabel *qliServer;
    QLineEdit *qleServer;
    QLabel *qliPort;
    QLineEdit *qlePort;
    QLabel *qliUsername;
    QLineEdit *qleUsername;
    QDialogButtonBox *qdbbButtonBox;
    QLabel *qliPassword;
    QLineEdit *qlePassword;
    QCheckBox *qcbShowPassword;

    void setupUi(QDialog *ConnectDialogEdit)
    {
        if (ConnectDialogEdit->objectName().isEmpty())
            ConnectDialogEdit->setObjectName(QString::fromUtf8("ConnectDialogEdit"));
        ConnectDialogEdit->resize(243, 194);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ConnectDialogEdit->sizePolicy().hasHeightForWidth());
        ConnectDialogEdit->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(ConnectDialogEdit);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qliName = new QLabel(ConnectDialogEdit);
        qliName->setObjectName(QString::fromUtf8("qliName"));

        gridLayout->addWidget(qliName, 0, 0, 1, 1);

        qleName = new QLineEdit(ConnectDialogEdit);
        qleName->setObjectName(QString::fromUtf8("qleName"));

        gridLayout->addWidget(qleName, 0, 1, 1, 1);

        qliServer = new QLabel(ConnectDialogEdit);
        qliServer->setObjectName(QString::fromUtf8("qliServer"));

        gridLayout->addWidget(qliServer, 1, 0, 1, 1);

        qleServer = new QLineEdit(ConnectDialogEdit);
        qleServer->setObjectName(QString::fromUtf8("qleServer"));

        gridLayout->addWidget(qleServer, 1, 1, 1, 1);

        qliPort = new QLabel(ConnectDialogEdit);
        qliPort->setObjectName(QString::fromUtf8("qliPort"));

        gridLayout->addWidget(qliPort, 2, 0, 1, 1);

        qlePort = new QLineEdit(ConnectDialogEdit);
        qlePort->setObjectName(QString::fromUtf8("qlePort"));

        gridLayout->addWidget(qlePort, 2, 1, 1, 1);

        qliUsername = new QLabel(ConnectDialogEdit);
        qliUsername->setObjectName(QString::fromUtf8("qliUsername"));

        gridLayout->addWidget(qliUsername, 3, 0, 1, 1);

        qleUsername = new QLineEdit(ConnectDialogEdit);
        qleUsername->setObjectName(QString::fromUtf8("qleUsername"));

        gridLayout->addWidget(qleUsername, 3, 1, 1, 1);

        qdbbButtonBox = new QDialogButtonBox(ConnectDialogEdit);
        qdbbButtonBox->setObjectName(QString::fromUtf8("qdbbButtonBox"));
        qdbbButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(qdbbButtonBox, 6, 1, 1, 1);

        qliPassword = new QLabel(ConnectDialogEdit);
        qliPassword->setObjectName(QString::fromUtf8("qliPassword"));

        gridLayout->addWidget(qliPassword, 4, 0, 1, 1);

        qlePassword = new QLineEdit(ConnectDialogEdit);
        qlePassword->setObjectName(QString::fromUtf8("qlePassword"));

        gridLayout->addWidget(qlePassword, 4, 1, 1, 1);

        qcbShowPassword = new QCheckBox(ConnectDialogEdit);
        qcbShowPassword->setObjectName(QString::fromUtf8("qcbShowPassword"));

        gridLayout->addWidget(qcbShowPassword, 5, 1, 1, 1);

#ifndef QT_NO_SHORTCUT
        qliName->setBuddy(qleName);
        qliServer->setBuddy(qleServer);
        qliPort->setBuddy(qlePort);
        qliUsername->setBuddy(qleUsername);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(qleName, qleServer);
        QWidget::setTabOrder(qleServer, qlePort);
        QWidget::setTabOrder(qlePort, qleUsername);
        QWidget::setTabOrder(qleUsername, qdbbButtonBox);

        retranslateUi(ConnectDialogEdit);
        QObject::connect(qdbbButtonBox, SIGNAL(accepted()), ConnectDialogEdit, SLOT(accept()));
        QObject::connect(qdbbButtonBox, SIGNAL(rejected()), ConnectDialogEdit, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConnectDialogEdit);
    } // setupUi

    void retranslateUi(QDialog *ConnectDialogEdit)
    {
        ConnectDialogEdit->setWindowTitle(QApplication::translate("ConnectDialogEdit", "Edit Server", 0, QApplication::UnicodeUTF8));
        qliName->setText(QApplication::translate("ConnectDialogEdit", "Label", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleName->setToolTip(QApplication::translate("ConnectDialogEdit", "Name of the server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qleName->setWhatsThis(QApplication::translate("ConnectDialogEdit", "<b>Label</b><br/>\n"
"Label of the server. This is what the server will be named like in your server list and can be chosen freely.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliServer->setText(QApplication::translate("ConnectDialogEdit", "A&ddress", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleServer->setToolTip(QApplication::translate("ConnectDialogEdit", "Internet address of the server.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qleServer->setWhatsThis(QApplication::translate("ConnectDialogEdit", "<b>Address</b><br/>\n"
"Internet address of the server. This can be a normal hostname, an IPv4/IPv6 address or a Bonjour service identifier. Bonjour service identifiers have to be prefixed with a '@' to be recognized by Mumble.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliPort->setText(QApplication::translate("ConnectDialogEdit", "&Port", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlePort->setToolTip(QApplication::translate("ConnectDialogEdit", "Port on which the server is listening", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlePort->setWhatsThis(QApplication::translate("ConnectDialogEdit", "<b>Port</b><br/>\n"
"Port on which the server is listening. If the server is identified by a Bonjour service identifier this field will be ignored.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliUsername->setText(QApplication::translate("ConnectDialogEdit", "&Username", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleUsername->setToolTip(QApplication::translate("ConnectDialogEdit", "Username to send to the server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qleUsername->setWhatsThis(QApplication::translate("ConnectDialogEdit", "<b>Username</b><br/>\n"
"Username to send to the server. Be aware that the server can impose restrictions on how a username might look like. Also your username could already be taken by another user.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliPassword->setText(QApplication::translate("ConnectDialogEdit", "Password", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlePassword->setToolTip(QApplication::translate("ConnectDialogEdit", "Password to send to the server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlePassword->setWhatsThis(QApplication::translate("ConnectDialogEdit", "<b>Password</b><br/>\n"
"Password to be sent to the server on connect. This password is needed when connecting as <i>SuperUser</i> or to a server using password authentication. If not entered here the password will be queried on connect.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbShowPassword->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        qcbShowPassword->setText(QApplication::translate("ConnectDialogEdit", "Show password", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConnectDialogEdit: public Ui_ConnectDialogEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTDIALOGEDIT_H
#endif