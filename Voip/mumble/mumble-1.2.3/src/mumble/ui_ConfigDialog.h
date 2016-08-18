/********************************************************************************
** Form generated from reading UI file 'ConfigDialog.ui'
**
** Created: Fri 1. Jun 10:05:05 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS
#ifndef UI_CONFIGDIALOG_H
#define UI_CONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigDialog
{
public:
    QGridLayout *gridLayout;
    QListWidget *qlwIcons;
    QCheckBox *qcbExpert;
    QHBoxLayout *horizontalLayout;
    QDialogButtonBox *pageButtonBox;
    QSpacerItem *spacerItem;
    QDialogButtonBox *dialogButtonBox;
    QStackedWidget *qswPages;

    void setupUi(QDialog *ConfigDialog)
    {
        if (ConfigDialog->objectName().isEmpty())
            ConfigDialog->setObjectName(QString::fromUtf8("ConfigDialog"));
        ConfigDialog->resize(709, 505);
        ConfigDialog->setSizeGripEnabled(true);
        gridLayout = new QGridLayout(ConfigDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qlwIcons = new QListWidget(ConfigDialog);
        qlwIcons->setObjectName(QString::fromUtf8("qlwIcons"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qlwIcons->sizePolicy().hasHeightForWidth());
        qlwIcons->setSizePolicy(sizePolicy);
        qlwIcons->setIconSize(QSize(24, 24));
        qlwIcons->setResizeMode(QListView::Adjust);
        qlwIcons->setLayoutMode(QListView::Batched);
        qlwIcons->setUniformItemSizes(true);

        gridLayout->addWidget(qlwIcons, 0, 0, 1, 1);

        qcbExpert = new QCheckBox(ConfigDialog);
        qcbExpert->setObjectName(QString::fromUtf8("qcbExpert"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qcbExpert->sizePolicy().hasHeightForWidth());
        qcbExpert->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(qcbExpert, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pageButtonBox = new QDialogButtonBox(ConfigDialog);
        pageButtonBox->setObjectName(QString::fromUtf8("pageButtonBox"));
        pageButtonBox->setOrientation(Qt::Horizontal);
        pageButtonBox->setStandardButtons(QDialogButtonBox::Reset|QDialogButtonBox::RestoreDefaults);

        horizontalLayout->addWidget(pageButtonBox);

        spacerItem = new QSpacerItem(474, 22, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);

        dialogButtonBox = new QDialogButtonBox(ConfigDialog);
        dialogButtonBox->setObjectName(QString::fromUtf8("dialogButtonBox"));
        dialogButtonBox->setOrientation(Qt::Horizontal);
        dialogButtonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(dialogButtonBox);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 3);

        qswPages = new QStackedWidget(ConfigDialog);
        qswPages->setObjectName(QString::fromUtf8("qswPages"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(qswPages->sizePolicy().hasHeightForWidth());
        qswPages->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(qswPages, 0, 1, 2, 1);


        retranslateUi(ConfigDialog);
        QObject::connect(dialogButtonBox, SIGNAL(accepted()), ConfigDialog, SLOT(accept()));
        QObject::connect(dialogButtonBox, SIGNAL(rejected()), ConfigDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigDialog)
    {
        ConfigDialog->setWindowTitle(QApplication::translate("ConfigDialog", "Mumble Configuration", 0, QApplication::UnicodeUTF8));
        qcbExpert->setText(QApplication::translate("ConfigDialog", "Advanced", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConfigDialog: public Ui_ConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGDIALOG_H

#endif