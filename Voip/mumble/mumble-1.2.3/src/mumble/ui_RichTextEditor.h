/********************************************************************************
** Form generated from reading UI file 'RichTextEditor.ui'
**
** Created: Fri 1. Jun 10:05:09 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS
#ifndef UI_RICHTEXTEDITOR_H
#define UI_RICHTEXTEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "RichTextEditor.h"

QT_BEGIN_NAMESPACE

class Ui_RichTextEditor
{
public:
    QAction *qaBold;
    QAction *qaItalic;
    QAction *qaUnderline;
    QAction *qaColor;
    QAction *qaLink;
    QAction *qaImage;
    QWidget *qwRich;
    QVBoxLayout *verticalLayout_2;
    QToolBar *qtbToolBar;
    RichTextHtmlEdit *qteRichText;
    QWidget *qwPlain;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *qptePlainText;

    void setupUi(QTabWidget *RichTextEditor)
    {
        if (RichTextEditor->objectName().isEmpty())
            RichTextEditor->setObjectName(QString::fromUtf8("RichTextEditor"));
        RichTextEditor->resize(700, 518);
        RichTextEditor->setWindowTitle(QString::fromUtf8("TabWidget"));
        RichTextEditor->setTabPosition(QTabWidget::South);
        qaBold = new QAction(RichTextEditor);
        qaBold->setObjectName(QString::fromUtf8("qaBold"));
        qaBold->setCheckable(true);
        QIcon icon;
#ifdef USE_QTSVG
        icon.addFile(QString::fromUtf8("skin:actions/format-text-bold.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaBold->setIcon(icon);
        qaItalic = new QAction(RichTextEditor);
        qaItalic->setObjectName(QString::fromUtf8("qaItalic"));
        qaItalic->setCheckable(true);
        QIcon icon1;
#ifdef USE_QTSVG
        icon1.addFile(QString::fromUtf8("skin:actions/format-text-italic.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaItalic->setIcon(icon1);
        qaUnderline = new QAction(RichTextEditor);
        qaUnderline->setObjectName(QString::fromUtf8("qaUnderline"));
        qaUnderline->setCheckable(true);
        QIcon icon2;
#ifdef USE_QTSVG
        icon2.addFile(QString::fromUtf8("skin:actions/format-text-underline.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaUnderline->setIcon(icon2);
        qaColor = new QAction(RichTextEditor);
        qaColor->setObjectName(QString::fromUtf8("qaColor"));
        qaLink = new QAction(RichTextEditor);
        qaLink->setObjectName(QString::fromUtf8("qaLink"));
        QIcon icon3;
#ifdef USE_QTSVG
        icon3.addFile(QString::fromUtf8("skin:mimetypes/text-html.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaLink->setIcon(icon3);
        qaImage = new QAction(RichTextEditor);
        qaImage->setObjectName(QString::fromUtf8("qaImage"));
        QIcon icon4;
#ifdef USE_QTSVG
        icon4.addFile(QString::fromUtf8("skin:mimetypes/image-x-generic.svg"), QSize(), QIcon::Normal, QIcon::Off);
#endif
        qaImage->setIcon(icon4);
        qwRich = new QWidget();
        qwRich->setObjectName(QString::fromUtf8("qwRich"));
        verticalLayout_2 = new QVBoxLayout(qwRich);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        qtbToolBar = new QToolBar(qwRich);
        qtbToolBar->setObjectName(QString::fromUtf8("qtbToolBar"));

        verticalLayout_2->addWidget(qtbToolBar);

        qteRichText = new RichTextHtmlEdit(qwRich);
        qteRichText->setObjectName(QString::fromUtf8("qteRichText"));
        qteRichText->setAutoFormatting(QTextEdit::AutoAll);

        verticalLayout_2->addWidget(qteRichText);

        RichTextEditor->addTab(qwRich, QString());
        qwPlain = new QWidget();
        qwPlain->setObjectName(QString::fromUtf8("qwPlain"));
        verticalLayout = new QVBoxLayout(qwPlain);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qptePlainText = new QPlainTextEdit(qwPlain);
        qptePlainText->setObjectName(QString::fromUtf8("qptePlainText"));

        verticalLayout->addWidget(qptePlainText);

        RichTextEditor->addTab(qwPlain, QString());

        retranslateUi(RichTextEditor);

        RichTextEditor->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(RichTextEditor);
    } // setupUi

    void retranslateUi(QTabWidget *RichTextEditor)
    {
        qaBold->setText(QApplication::translate("RichTextEditor", "&Bold", 0, QApplication::UnicodeUTF8));
        qaBold->setShortcut(QApplication::translate("RichTextEditor", "Ctrl+B", 0, QApplication::UnicodeUTF8));
        qaItalic->setText(QApplication::translate("RichTextEditor", "&Italic", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaItalic->setToolTip(QApplication::translate("RichTextEditor", "Italic", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaItalic->setShortcut(QApplication::translate("RichTextEditor", "Ctrl+I", 0, QApplication::UnicodeUTF8));
        qaUnderline->setText(QApplication::translate("RichTextEditor", "Underline", 0, QApplication::UnicodeUTF8));
        qaUnderline->setShortcut(QApplication::translate("RichTextEditor", "Ctrl+U", 0, QApplication::UnicodeUTF8));
        qaColor->setText(QApplication::translate("RichTextEditor", "Color", 0, QApplication::UnicodeUTF8));
        qaLink->setText(QApplication::translate("RichTextEditor", "Insert Link", 0, QApplication::UnicodeUTF8));
        qaLink->setShortcut(QApplication::translate("RichTextEditor", "Ctrl+L", 0, QApplication::UnicodeUTF8));
        qaImage->setText(QApplication::translate("RichTextEditor", "Insert Image", 0, QApplication::UnicodeUTF8));
        RichTextEditor->setTabText(RichTextEditor->indexOf(qwRich), QApplication::translate("RichTextEditor", "Display", 0, QApplication::UnicodeUTF8));
        RichTextEditor->setTabText(RichTextEditor->indexOf(qwPlain), QApplication::translate("RichTextEditor", "Source Text", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(RichTextEditor);
    } // retranslateUi

};

namespace Ui {
    class RichTextEditor: public Ui_RichTextEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RICHTEXTEDITOR_H
#endif