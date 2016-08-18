/********************************************************************************
** Form generated from reading UI file 'RichTextEditorLink.ui'
**
** Created: Fri 1. Jun 10:05:10 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_RICHTEXTEDITORLINK_H
#define UI_RICHTEXTEDITORLINK_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_RichTextEditorLink
{
public:
    QGridLayout *gridLayout;
    QLabel *qlUrl;
    QLineEdit *qleUrl;
    QLabel *qlText;
    QLineEdit *qleText;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RichTextEditorLink)
    {
        if (RichTextEditorLink->objectName().isEmpty())
            RichTextEditorLink->setObjectName(QString::fromUtf8("RichTextEditorLink"));
        RichTextEditorLink->resize(295, 95);
        gridLayout = new QGridLayout(RichTextEditorLink);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qlUrl = new QLabel(RichTextEditorLink);
        qlUrl->setObjectName(QString::fromUtf8("qlUrl"));

        gridLayout->addWidget(qlUrl, 0, 0, 1, 1);

        qleUrl = new QLineEdit(RichTextEditorLink);
        qleUrl->setObjectName(QString::fromUtf8("qleUrl"));

        gridLayout->addWidget(qleUrl, 0, 1, 1, 1);

        qlText = new QLabel(RichTextEditorLink);
        qlText->setObjectName(QString::fromUtf8("qlText"));

        gridLayout->addWidget(qlText, 1, 0, 1, 1);

        qleText = new QLineEdit(RichTextEditorLink);
        qleText->setObjectName(QString::fromUtf8("qleText"));

        gridLayout->addWidget(qleText, 1, 1, 1, 1);

        buttonBox = new QDialogButtonBox(RichTextEditorLink);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 2);

#ifndef QT_NO_SHORTCUT
        qlUrl->setBuddy(qleUrl);
        qlText->setBuddy(qleText);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(qleUrl, qleText);
        QWidget::setTabOrder(qleText, buttonBox);

        retranslateUi(RichTextEditorLink);
        QObject::connect(buttonBox, SIGNAL(accepted()), RichTextEditorLink, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RichTextEditorLink, SLOT(reject()));

        QMetaObject::connectSlotsByName(RichTextEditorLink);
    } // setupUi

    void retranslateUi(QDialog *RichTextEditorLink)
    {
        RichTextEditorLink->setWindowTitle(QApplication::translate("RichTextEditorLink", "Add Link", 0, QApplication::UnicodeUTF8));
        qlUrl->setText(QApplication::translate("RichTextEditorLink", "URL", 0, QApplication::UnicodeUTF8));
        qlText->setText(QApplication::translate("RichTextEditorLink", "Text", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RichTextEditorLink: public Ui_RichTextEditorLink {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RICHTEXTEDITORLINK_H
#endif