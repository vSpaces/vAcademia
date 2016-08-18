/********************************************************************************
** Form generated from reading UI file 'Tokens.ui'
**
** Created: Fri 1. Jun 10:05:09 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS
#ifndef UI_TOKENS_H
#define UI_TOKENS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Tokens
{
public:
    QVBoxLayout *verticalLayout;
    QListWidget *qlwTokens;
    QHBoxLayout *hboxLayout;
    QPushButton *qpbAdd;
    QPushButton *qpbRemove;
    QDialogButtonBox *qbbButtons;

    void setupUi(QDialog *Tokens)
    {
        if (Tokens->objectName().isEmpty())
            Tokens->setObjectName(QString::fromUtf8("Tokens"));
        Tokens->resize(514, 659);
        verticalLayout = new QVBoxLayout(Tokens);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qlwTokens = new QListWidget(Tokens);
        qlwTokens->setObjectName(QString::fromUtf8("qlwTokens"));
        qlwTokens->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        qlwTokens->setAlternatingRowColors(true);
        qlwTokens->setSelectionMode(QAbstractItemView::MultiSelection);
        qlwTokens->setSortingEnabled(true);

        verticalLayout->addWidget(qlwTokens);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        qpbAdd = new QPushButton(Tokens);
        qpbAdd->setObjectName(QString::fromUtf8("qpbAdd"));

        hboxLayout->addWidget(qpbAdd);

        qpbRemove = new QPushButton(Tokens);
        qpbRemove->setObjectName(QString::fromUtf8("qpbRemove"));

        hboxLayout->addWidget(qpbRemove);


        verticalLayout->addLayout(hboxLayout);

        qbbButtons = new QDialogButtonBox(Tokens);
        qbbButtons->setObjectName(QString::fromUtf8("qbbButtons"));
        qbbButtons->setOrientation(Qt::Horizontal);
        qbbButtons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(qbbButtons);

        QWidget::setTabOrder(qlwTokens, qpbAdd);
        QWidget::setTabOrder(qpbAdd, qpbRemove);
        QWidget::setTabOrder(qpbRemove, qbbButtons);

        retranslateUi(Tokens);
        QObject::connect(qbbButtons, SIGNAL(accepted()), Tokens, SLOT(accept()));
        QObject::connect(qbbButtons, SIGNAL(rejected()), Tokens, SLOT(reject()));

        QMetaObject::connectSlotsByName(Tokens);
    } // setupUi

    void retranslateUi(QDialog *Tokens)
    {
        Tokens->setWindowTitle(QApplication::translate("Tokens", "Mumble - Access Tokens", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlwTokens->setToolTip(QApplication::translate("Tokens", "List of access tokens on current server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlwTokens->setWhatsThis(QApplication::translate("Tokens", "<b>This is an editable list of access tokens on the connected server.</b>\n"
"<br />\n"
"An access token is a text string, which can be used as a password for very simple access management on channels. Mumble will remember the tokens you've used and resend them to the server next time you reconnect, so you don't have to enter these every time.\n"
"", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qpbAdd->setToolTip(QApplication::translate("Tokens", "Add a token", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qpbAdd->setText(QApplication::translate("Tokens", "&Add", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbRemove->setToolTip(QApplication::translate("Tokens", "Remove a token", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qpbRemove->setText(QApplication::translate("Tokens", "&Remove", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Tokens: public Ui_Tokens {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOKENS_H
#endif