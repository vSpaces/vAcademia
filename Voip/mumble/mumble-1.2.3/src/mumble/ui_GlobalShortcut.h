/********************************************************************************
** Form generated from reading UI file 'GlobalShortcut.ui'
**
** Created: Fri 1. Jun 10:05:08 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GLOBALSHORTCUT_H
#define UI_GLOBALSHORTCUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GlobalShortcut
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *qgbShortcuts;
    QGridLayout *gridLayout;
    QPushButton *qpbAdd;
    QPushButton *qpbRemove;
    QSpacerItem *horizontalSpacer;
    QTreeWidget *qtwShortcuts;

    void setupUi(QWidget *GlobalShortcut)
    {
        if (GlobalShortcut->objectName().isEmpty())
            GlobalShortcut->setObjectName(QString::fromUtf8("GlobalShortcut"));
        GlobalShortcut->resize(645, 468);
        verticalLayout = new QVBoxLayout(GlobalShortcut);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qgbShortcuts = new QGroupBox(GlobalShortcut);
        qgbShortcuts->setObjectName(QString::fromUtf8("qgbShortcuts"));
        gridLayout = new QGridLayout(qgbShortcuts);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qpbAdd = new QPushButton(qgbShortcuts);
        qpbAdd->setObjectName(QString::fromUtf8("qpbAdd"));

        gridLayout->addWidget(qpbAdd, 1, 0, 1, 1);

        qpbRemove = new QPushButton(qgbShortcuts);
        qpbRemove->setObjectName(QString::fromUtf8("qpbRemove"));
        qpbRemove->setEnabled(false);

        gridLayout->addWidget(qpbRemove, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(59, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 2, 1, 1);

        qtwShortcuts = new QTreeWidget(qgbShortcuts);
        qtwShortcuts->setObjectName(QString::fromUtf8("qtwShortcuts"));
        qtwShortcuts->setEditTriggers(QAbstractItemView::AllEditTriggers);
        qtwShortcuts->setAlternatingRowColors(true);
        qtwShortcuts->setRootIsDecorated(false);
        qtwShortcuts->setUniformRowHeights(true);
        qtwShortcuts->header()->setDefaultSectionSize(100);
        qtwShortcuts->header()->setMinimumSectionSize(50);
        qtwShortcuts->header()->setStretchLastSection(false);

        gridLayout->addWidget(qtwShortcuts, 0, 0, 1, 3);


        verticalLayout->addWidget(qgbShortcuts);


        retranslateUi(GlobalShortcut);

        QMetaObject::connectSlotsByName(GlobalShortcut);
    } // setupUi

    void retranslateUi(QWidget *GlobalShortcut)
    {
        qgbShortcuts->setTitle(QApplication::translate("GlobalShortcut", "Shortcuts", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbAdd->setToolTip(QApplication::translate("GlobalShortcut", "Add new shortcut", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbAdd->setWhatsThis(QApplication::translate("GlobalShortcut", "This will add a new global shortcut", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbAdd->setText(QApplication::translate("GlobalShortcut", "&Add", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbRemove->setToolTip(QApplication::translate("GlobalShortcut", "Remove selected shortcut", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbRemove->setWhatsThis(QApplication::translate("GlobalShortcut", "This will permanently remove a selected shortcut.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbRemove->setText(QApplication::translate("GlobalShortcut", "&Remove", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = qtwShortcuts->headerItem();
        ___qtreewidgetitem->setText(3, QApplication::translate("GlobalShortcut", "Suppress", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(2, QApplication::translate("GlobalShortcut", "Shortcut", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("GlobalShortcut", "Data", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("GlobalShortcut", "Function", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qtwShortcuts->setToolTip(QApplication::translate("GlobalShortcut", "List of configured shortcuts", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Q_UNUSED(GlobalShortcut);
    } // retranslateUi

};

namespace Ui {
    class GlobalShortcut: public Ui_GlobalShortcut {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GLOBALSHORTCUT_H
