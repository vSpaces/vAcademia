/********************************************************************************
** Form generated from reading UI file 'TextMessage.ui'
**
** Created: Fri 1. Jun 10:05:08 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTMESSAGE_H
#define UI_TEXTMESSAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include "RichTextEditor.h"

QT_BEGIN_NAMESPACE

class Ui_TextMessage
{
public:
    QVBoxLayout *vboxLayout;
    QVBoxLayout *verticalLayout;
    RichTextEditor *rteMessage;
    QCheckBox *qcbTreeMessage;
    QDialogButtonBox *qbbButtons;

    void setupUi(QDialog *TextMessage)
    {
        if (TextMessage->objectName().isEmpty())
            TextMessage->setObjectName(QString::fromUtf8("TextMessage"));
        TextMessage->resize(397, 297);
        vboxLayout = new QVBoxLayout(TextMessage);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        rteMessage = new RichTextEditor(TextMessage);
        rteMessage->setObjectName(QString::fromUtf8("rteMessage"));

        verticalLayout->addWidget(rteMessage);


        vboxLayout->addLayout(verticalLayout);

        qcbTreeMessage = new QCheckBox(TextMessage);
        qcbTreeMessage->setObjectName(QString::fromUtf8("qcbTreeMessage"));

        vboxLayout->addWidget(qcbTreeMessage);

        qbbButtons = new QDialogButtonBox(TextMessage);
        qbbButtons->setObjectName(QString::fromUtf8("qbbButtons"));
        qbbButtons->setOrientation(Qt::Horizontal);
        qbbButtons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(qbbButtons);


        retranslateUi(TextMessage);
        QObject::connect(qbbButtons, SIGNAL(accepted()), TextMessage, SLOT(accept()));
        QObject::connect(qbbButtons, SIGNAL(rejected()), TextMessage, SLOT(reject()));

        QMetaObject::connectSlotsByName(TextMessage);
    } // setupUi

    void retranslateUi(QDialog *TextMessage)
    {
#ifndef QT_NO_TOOLTIP
        qcbTreeMessage->setToolTip(QApplication::translate("TextMessage", "If checked the message is recursively sent to all subchannels", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbTreeMessage->setText(QApplication::translate("TextMessage", "Send recursively to subchannels", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(TextMessage);
    } // retranslateUi

};

namespace Ui {
    class TextMessage: public Ui_TextMessage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTMESSAGE_H
