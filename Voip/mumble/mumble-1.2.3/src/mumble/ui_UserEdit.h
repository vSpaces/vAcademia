/********************************************************************************
** Form generated from reading UI file 'UserEdit.ui'
**
** Created: Fri 1. Jun 10:05:09 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_USEREDIT_H
#define UI_USEREDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UserEdit
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *qgbUserList;
    QGridLayout *gridLayout;
    QPushButton *qpbRemove;
    QListWidget *qlwUserList;
    QDialogButtonBox *qbbButtons;

    void setupUi(QDialog *UserEdit)
    {
        if (UserEdit->objectName().isEmpty())
            UserEdit->setObjectName(QString::fromUtf8("UserEdit"));
        UserEdit->resize(471, 401);
        verticalLayout = new QVBoxLayout(UserEdit);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qgbUserList = new QGroupBox(UserEdit);
        qgbUserList->setObjectName(QString::fromUtf8("qgbUserList"));
        gridLayout = new QGridLayout(qgbUserList);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qpbRemove = new QPushButton(qgbUserList);
        qpbRemove->setObjectName(QString::fromUtf8("qpbRemove"));

        gridLayout->addWidget(qpbRemove, 0, 1, 1, 1);

        qlwUserList = new QListWidget(qgbUserList);
        qlwUserList->setObjectName(QString::fromUtf8("qlwUserList"));
        qlwUserList->setSortingEnabled(true);

        gridLayout->addWidget(qlwUserList, 0, 0, 1, 1);


        verticalLayout->addWidget(qgbUserList);

        qbbButtons = new QDialogButtonBox(UserEdit);
        qbbButtons->setObjectName(QString::fromUtf8("qbbButtons"));
        qbbButtons->setOrientation(Qt::Horizontal);
        qbbButtons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(qbbButtons);

        QWidget::setTabOrder(qlwUserList, qpbRemove);
        QWidget::setTabOrder(qpbRemove, qbbButtons);

        retranslateUi(UserEdit);
        QObject::connect(qbbButtons, SIGNAL(accepted()), UserEdit, SLOT(accept()));
        QObject::connect(qbbButtons, SIGNAL(rejected()), UserEdit, SLOT(reject()));

        QMetaObject::connectSlotsByName(UserEdit);
    } // setupUi

    void retranslateUi(QDialog *UserEdit)
    {
        UserEdit->setWindowTitle(QApplication::translate("UserEdit", "Registered Users", 0, QApplication::UnicodeUTF8));
        qgbUserList->setTitle(QApplication::translate("UserEdit", "Registered Users", 0, QApplication::UnicodeUTF8));
        qpbRemove->setText(QApplication::translate("UserEdit", "Remove", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class UserEdit: public Ui_UserEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USEREDIT_H
#endif