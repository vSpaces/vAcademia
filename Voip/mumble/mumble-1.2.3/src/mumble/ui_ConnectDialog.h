/********************************************************************************
** Form generated from reading UI file 'ConnectDialog.ui'
**
** Created: Fri 1. Jun 10:05:06 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_CONNECTDIALOG_H
#define UI_CONNECTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include "ConnectDialog.h"

QT_BEGIN_NAMESPACE

class Ui_ConnectDialog
{
public:
    QAction *qaFavoriteRemove;
    QAction *qaFavoriteEdit;
    QAction *qaFavoriteAddNew;
    QAction *qaFavoriteAdd;
    QAction *qaUrl;
    QAction *qaShowReachable;
    QAction *qaShowPopulated;
    QAction *qaShowAll;
    QAction *qaFavoriteCopy;
    QAction *qaFavoritePaste;
    QVBoxLayout *verticalLayout;
    ServerView *qtwServers;
    QDialogButtonBox *qdbbButtonBox;

    void setupUi(QDialog *ConnectDialog)
    {
        if (ConnectDialog->objectName().isEmpty())
            ConnectDialog->setObjectName(QString::fromUtf8("ConnectDialog"));
        ConnectDialog->resize(574, 366);
        qaFavoriteRemove = new QAction(ConnectDialog);
        qaFavoriteRemove->setObjectName(QString::fromUtf8("qaFavoriteRemove"));
        qaFavoriteEdit = new QAction(ConnectDialog);
        qaFavoriteEdit->setObjectName(QString::fromUtf8("qaFavoriteEdit"));
        qaFavoriteAddNew = new QAction(ConnectDialog);
        qaFavoriteAddNew->setObjectName(QString::fromUtf8("qaFavoriteAddNew"));
        qaFavoriteAdd = new QAction(ConnectDialog);
        qaFavoriteAdd->setObjectName(QString::fromUtf8("qaFavoriteAdd"));
        qaUrl = new QAction(ConnectDialog);
        qaUrl->setObjectName(QString::fromUtf8("qaUrl"));
        qaShowReachable = new QAction(ConnectDialog);
        qaShowReachable->setObjectName(QString::fromUtf8("qaShowReachable"));
        qaShowReachable->setCheckable(true);
        qaShowReachable->setChecked(true);
        qaShowPopulated = new QAction(ConnectDialog);
        qaShowPopulated->setObjectName(QString::fromUtf8("qaShowPopulated"));
        qaShowPopulated->setCheckable(true);
        qaShowPopulated->setChecked(true);
        qaShowAll = new QAction(ConnectDialog);
        qaShowAll->setObjectName(QString::fromUtf8("qaShowAll"));
        qaShowAll->setCheckable(true);
        qaFavoriteCopy = new QAction(ConnectDialog);
        qaFavoriteCopy->setObjectName(QString::fromUtf8("qaFavoriteCopy"));
        qaFavoritePaste = new QAction(ConnectDialog);
        qaFavoritePaste->setObjectName(QString::fromUtf8("qaFavoritePaste"));
        verticalLayout = new QVBoxLayout(ConnectDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qtwServers = new ServerView(ConnectDialog);
        qtwServers->setObjectName(QString::fromUtf8("qtwServers"));
        qtwServers->setContextMenuPolicy(Qt::CustomContextMenu);
        qtwServers->setDragEnabled(true);
        qtwServers->setDragDropMode(QAbstractItemView::DragDrop);
        qtwServers->setAlternatingRowColors(true);
        qtwServers->setAutoExpandDelay(250);
        qtwServers->setRootIsDecorated(true);
        qtwServers->setUniformRowHeights(true);
        qtwServers->setSortingEnabled(true);
        qtwServers->header()->setStretchLastSection(false);

        verticalLayout->addWidget(qtwServers);

        qdbbButtonBox = new QDialogButtonBox(ConnectDialog);
        qdbbButtonBox->setObjectName(QString::fromUtf8("qdbbButtonBox"));
        qdbbButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(qdbbButtonBox);


        retranslateUi(ConnectDialog);
        QObject::connect(qdbbButtonBox, SIGNAL(accepted()), ConnectDialog, SLOT(accept()));
        QObject::connect(qdbbButtonBox, SIGNAL(rejected()), ConnectDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConnectDialog);
    } // setupUi

    void retranslateUi(QDialog *ConnectDialog)
    {
        ConnectDialog->setWindowTitle(QApplication::translate("ConnectDialog", "Mumble Server Connect", 0, QApplication::UnicodeUTF8));
        qaFavoriteRemove->setText(QApplication::translate("ConnectDialog", "Remove from Favorites", 0, QApplication::UnicodeUTF8));
        qaFavoriteEdit->setText(QApplication::translate("ConnectDialog", "&Edit...", 0, QApplication::UnicodeUTF8));
        qaFavoriteAddNew->setText(QApplication::translate("ConnectDialog", "&Add New...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaFavoriteAddNew->setToolTip(QApplication::translate("ConnectDialog", "Add custom server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaFavoriteAdd->setText(QApplication::translate("ConnectDialog", "Add to &Favorites", 0, QApplication::UnicodeUTF8));
        qaUrl->setText(QApplication::translate("ConnectDialog", "Open &Webpage", 0, QApplication::UnicodeUTF8));
        qaShowReachable->setText(QApplication::translate("ConnectDialog", "Show &Reachable", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaShowReachable->setToolTip(QApplication::translate("ConnectDialog", "Show all servers that respond to ping", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaShowPopulated->setText(QApplication::translate("ConnectDialog", "Show &Populated", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaShowPopulated->setToolTip(QApplication::translate("ConnectDialog", "Show all servers with users", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaShowAll->setText(QApplication::translate("ConnectDialog", "Show &All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaShowAll->setToolTip(QApplication::translate("ConnectDialog", "Show all servers", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaFavoriteCopy->setText(QApplication::translate("ConnectDialog", "&Copy", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaFavoriteCopy->setToolTip(QApplication::translate("ConnectDialog", "Copy favorite link to clipboard", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qaFavoritePaste->setText(QApplication::translate("ConnectDialog", "&Paste", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qaFavoritePaste->setToolTip(QApplication::translate("ConnectDialog", "Paste favorite from clipboard", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        QTreeWidgetItem *___qtreewidgetitem = qtwServers->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("ConnectDialog", "Users", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("ConnectDialog", "Ping", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("ConnectDialog", "Servername", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConnectDialog: public Ui_ConnectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTDIALOG_H
#endif