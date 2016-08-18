/********************************************************************************
** Form generated from reading UI file 'BanEditor.ui'
**
** Created: Fri 1. Jun 10:05:06 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_BANEDITOR_H
#define UI_BANEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDateTimeEdit>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_BanEditor
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QListWidget *qlwBans;
    QVBoxLayout *vboxLayout1;
    QGridLayout *gridLayout;
    QLabel *qlIP;
    QLabel *qlMask;
    QLineEdit *qleIP;
    QSpinBox *qsbMask;
    QLineEdit *qleReason;
    QLabel *qlReason;
    QDateTimeEdit *qdteStart;
    QLabel *qlStart;
    QLabel *qlEnd;
    QDateTimeEdit *qdteEnd;
    QLabel *qliUser;
    QLabel *qlUser;
    QLabel *qliHash;
    QLabel *qlHash;
    QHBoxLayout *hboxLayout1;
    QPushButton *qpbAdd;
    QPushButton *qpbUpdate;
    QPushButton *qpbRemove;
    QSpacerItem *spacerItem;
    QDialogButtonBox *qbbButtons;

    void setupUi(QDialog *BanEditor)
    {
        if (BanEditor->objectName().isEmpty())
            BanEditor->setObjectName(QString::fromUtf8("BanEditor"));
        BanEditor->resize(458, 547);
        vboxLayout = new QVBoxLayout(BanEditor);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        qlwBans = new QListWidget(BanEditor);
        qlwBans->setObjectName(QString::fromUtf8("qlwBans"));

        hboxLayout->addWidget(qlwBans);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qlIP = new QLabel(BanEditor);
        qlIP->setObjectName(QString::fromUtf8("qlIP"));

        gridLayout->addWidget(qlIP, 0, 0, 1, 1);

        qlMask = new QLabel(BanEditor);
        qlMask->setObjectName(QString::fromUtf8("qlMask"));

        gridLayout->addWidget(qlMask, 1, 0, 1, 1);

        qleIP = new QLineEdit(BanEditor);
        qleIP->setObjectName(QString::fromUtf8("qleIP"));

        gridLayout->addWidget(qleIP, 0, 1, 1, 1);

        qsbMask = new QSpinBox(BanEditor);
        qsbMask->setObjectName(QString::fromUtf8("qsbMask"));
        qsbMask->setMinimum(8);
        qsbMask->setMaximum(128);
        qsbMask->setValue(128);

        gridLayout->addWidget(qsbMask, 1, 1, 1, 1);

        qleReason = new QLineEdit(BanEditor);
        qleReason->setObjectName(QString::fromUtf8("qleReason"));

        gridLayout->addWidget(qleReason, 2, 1, 1, 1);

        qlReason = new QLabel(BanEditor);
        qlReason->setObjectName(QString::fromUtf8("qlReason"));

        gridLayout->addWidget(qlReason, 2, 0, 1, 1);

        qdteStart = new QDateTimeEdit(BanEditor);
        qdteStart->setObjectName(QString::fromUtf8("qdteStart"));
        qdteStart->setEnabled(false);
        qdteStart->setReadOnly(false);
        qdteStart->setCalendarPopup(false);

        gridLayout->addWidget(qdteStart, 3, 1, 1, 1);

        qlStart = new QLabel(BanEditor);
        qlStart->setObjectName(QString::fromUtf8("qlStart"));

        gridLayout->addWidget(qlStart, 3, 0, 1, 1);

        qlEnd = new QLabel(BanEditor);
        qlEnd->setObjectName(QString::fromUtf8("qlEnd"));

        gridLayout->addWidget(qlEnd, 4, 0, 1, 1);

        qdteEnd = new QDateTimeEdit(BanEditor);
        qdteEnd->setObjectName(QString::fromUtf8("qdteEnd"));
        qdteEnd->setCalendarPopup(true);

        gridLayout->addWidget(qdteEnd, 4, 1, 1, 1);

        qliUser = new QLabel(BanEditor);
        qliUser->setObjectName(QString::fromUtf8("qliUser"));

        gridLayout->addWidget(qliUser, 5, 0, 1, 1);

        qlUser = new QLabel(BanEditor);
        qlUser->setObjectName(QString::fromUtf8("qlUser"));
        qlUser->setMaximumSize(QSize(500, 16777215));

        gridLayout->addWidget(qlUser, 5, 1, 1, 1);

        qliHash = new QLabel(BanEditor);
        qliHash->setObjectName(QString::fromUtf8("qliHash"));

        gridLayout->addWidget(qliHash, 6, 0, 1, 1);

        qlHash = new QLabel(BanEditor);
        qlHash->setObjectName(QString::fromUtf8("qlHash"));

        gridLayout->addWidget(qlHash, 6, 1, 1, 1);


        vboxLayout1->addLayout(gridLayout);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        qpbAdd = new QPushButton(BanEditor);
        qpbAdd->setObjectName(QString::fromUtf8("qpbAdd"));

        hboxLayout1->addWidget(qpbAdd);

        qpbUpdate = new QPushButton(BanEditor);
        qpbUpdate->setObjectName(QString::fromUtf8("qpbUpdate"));

        hboxLayout1->addWidget(qpbUpdate);

        qpbRemove = new QPushButton(BanEditor);
        qpbRemove->setObjectName(QString::fromUtf8("qpbRemove"));

        hboxLayout1->addWidget(qpbRemove);


        vboxLayout1->addLayout(hboxLayout1);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout1->addItem(spacerItem);


        hboxLayout->addLayout(vboxLayout1);


        vboxLayout->addLayout(hboxLayout);

        qbbButtons = new QDialogButtonBox(BanEditor);
        qbbButtons->setObjectName(QString::fromUtf8("qbbButtons"));
        qbbButtons->setOrientation(Qt::Horizontal);
        qbbButtons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(qbbButtons);

#ifndef QT_NO_SHORTCUT
        qlIP->setBuddy(qleIP);
        qlMask->setBuddy(qsbMask);
#endif // QT_NO_SHORTCUT

        retranslateUi(BanEditor);
        QObject::connect(qbbButtons, SIGNAL(accepted()), BanEditor, SLOT(accept()));
        QObject::connect(qbbButtons, SIGNAL(rejected()), BanEditor, SLOT(reject()));

        QMetaObject::connectSlotsByName(BanEditor);
    } // setupUi

    void retranslateUi(QDialog *BanEditor)
    {
        BanEditor->setWindowTitle(QApplication::translate("BanEditor", "Mumble - Edit Bans", 0, QApplication::UnicodeUTF8));
        qlIP->setText(QApplication::translate("BanEditor", "&Address", 0, QApplication::UnicodeUTF8));
        qlMask->setText(QApplication::translate("BanEditor", "&Mask", 0, QApplication::UnicodeUTF8));
        qleIP->setText(QString());
        qlReason->setText(QApplication::translate("BanEditor", "Reason", 0, QApplication::UnicodeUTF8));
        qlStart->setText(QApplication::translate("BanEditor", "Start", 0, QApplication::UnicodeUTF8));
        qlEnd->setText(QApplication::translate("BanEditor", "End", 0, QApplication::UnicodeUTF8));
        qliUser->setText(QApplication::translate("BanEditor", "User", 0, QApplication::UnicodeUTF8));
        qlUser->setText(QString());
        qliHash->setText(QApplication::translate("BanEditor", "Hash", 0, QApplication::UnicodeUTF8));
        qlHash->setText(QString());
        qpbAdd->setText(QApplication::translate("BanEditor", "&Add", 0, QApplication::UnicodeUTF8));
        qpbUpdate->setText(QApplication::translate("BanEditor", "&Update", 0, QApplication::UnicodeUTF8));
        qpbRemove->setText(QApplication::translate("BanEditor", "&Remove", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BanEditor: public Ui_BanEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANEDITOR_H
#endif