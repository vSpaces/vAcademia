/********************************************************************************
** Form generated from reading UI file 'GlobalShortcutTarget.ui'
**
** Created: Fri 1. Jun 10:05:09 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GLOBALSHORTCUTTARGET_H
#define UI_GLOBALSHORTCUTTARGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GlobalShortcutTarget
{
public:
    QGridLayout *gridLayout;
    QRadioButton *qrbUsers;
    QStackedWidget *qswStack;
    QWidget *qwChannelPage;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *qgbChannel;
    QVBoxLayout *verticalLayout_3;
    QTreeWidget *qtwChannels;
    QHBoxLayout *horizontalLayout;
    QLabel *qlGroup;
    QLineEdit *qleGroup;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *qcbLinks;
    QCheckBox *qcbChildren;
    QWidget *qwUserPage;
    QVBoxLayout *verticalLayout;
    QGroupBox *qgbUser;
    QGridLayout *gridLayout_2;
    QListWidget *qlwUsers;
    QComboBox *qcbUser;
    QPushButton *qpbAdd;
    QPushButton *qpbRemove;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *qbbButtons;
    QRadioButton *qrbChannel;
    QGroupBox *qgbModifiers;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *qcbForceCenter;

    void setupUi(QDialog *GlobalShortcutTarget)
    {
        if (GlobalShortcutTarget->objectName().isEmpty())
            GlobalShortcutTarget->setObjectName(QString::fromUtf8("GlobalShortcutTarget"));
        GlobalShortcutTarget->resize(421, 310);
        gridLayout = new QGridLayout(GlobalShortcutTarget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qrbUsers = new QRadioButton(GlobalShortcutTarget);
        qrbUsers->setObjectName(QString::fromUtf8("qrbUsers"));

        gridLayout->addWidget(qrbUsers, 1, 0, 1, 1);

        qswStack = new QStackedWidget(GlobalShortcutTarget);
        qswStack->setObjectName(QString::fromUtf8("qswStack"));
        qwChannelPage = new QWidget();
        qwChannelPage->setObjectName(QString::fromUtf8("qwChannelPage"));
        verticalLayout_2 = new QVBoxLayout(qwChannelPage);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        qgbChannel = new QGroupBox(qwChannelPage);
        qgbChannel->setObjectName(QString::fromUtf8("qgbChannel"));
        verticalLayout_3 = new QVBoxLayout(qgbChannel);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        qtwChannels = new QTreeWidget(qgbChannel);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("Name"));
        qtwChannels->setHeaderItem(__qtreewidgetitem);
        qtwChannels->setObjectName(QString::fromUtf8("qtwChannels"));
        qtwChannels->setIndentation(10);
        qtwChannels->setRootIsDecorated(true);
        qtwChannels->setSortingEnabled(true);
        qtwChannels->setHeaderHidden(true);

        verticalLayout_3->addWidget(qtwChannels);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        qlGroup = new QLabel(qgbChannel);
        qlGroup->setObjectName(QString::fromUtf8("qlGroup"));

        horizontalLayout->addWidget(qlGroup);

        qleGroup = new QLineEdit(qgbChannel);
        qleGroup->setObjectName(QString::fromUtf8("qleGroup"));

        horizontalLayout->addWidget(qleGroup);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        qcbLinks = new QCheckBox(qgbChannel);
        qcbLinks->setObjectName(QString::fromUtf8("qcbLinks"));

        horizontalLayout_2->addWidget(qcbLinks);

        qcbChildren = new QCheckBox(qgbChannel);
        qcbChildren->setObjectName(QString::fromUtf8("qcbChildren"));

        horizontalLayout_2->addWidget(qcbChildren);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout_2->addWidget(qgbChannel);

        qswStack->addWidget(qwChannelPage);
        qwUserPage = new QWidget();
        qwUserPage->setObjectName(QString::fromUtf8("qwUserPage"));
        verticalLayout = new QVBoxLayout(qwUserPage);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qgbUser = new QGroupBox(qwUserPage);
        qgbUser->setObjectName(QString::fromUtf8("qgbUser"));
        gridLayout_2 = new QGridLayout(qgbUser);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        qlwUsers = new QListWidget(qgbUser);
        qlwUsers->setObjectName(QString::fromUtf8("qlwUsers"));

        gridLayout_2->addWidget(qlwUsers, 0, 0, 4, 1);

        qcbUser = new QComboBox(qgbUser);
        qcbUser->setObjectName(QString::fromUtf8("qcbUser"));

        gridLayout_2->addWidget(qcbUser, 0, 1, 1, 1);

        qpbAdd = new QPushButton(qgbUser);
        qpbAdd->setObjectName(QString::fromUtf8("qpbAdd"));

        gridLayout_2->addWidget(qpbAdd, 1, 1, 1, 1);

        qpbRemove = new QPushButton(qgbUser);
        qpbRemove->setObjectName(QString::fromUtf8("qpbRemove"));

        gridLayout_2->addWidget(qpbRemove, 2, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 94, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 3, 1, 1, 1);


        verticalLayout->addWidget(qgbUser);

        qswStack->addWidget(qwUserPage);

        gridLayout->addWidget(qswStack, 2, 0, 1, 2);

        qbbButtons = new QDialogButtonBox(GlobalShortcutTarget);
        qbbButtons->setObjectName(QString::fromUtf8("qbbButtons"));
        qbbButtons->setOrientation(Qt::Horizontal);
        qbbButtons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(qbbButtons, 4, 0, 1, 2);

        qrbChannel = new QRadioButton(GlobalShortcutTarget);
        qrbChannel->setObjectName(QString::fromUtf8("qrbChannel"));

        gridLayout->addWidget(qrbChannel, 1, 1, 1, 1);

        qgbModifiers = new QGroupBox(GlobalShortcutTarget);
        qgbModifiers->setObjectName(QString::fromUtf8("qgbModifiers"));
        verticalLayout_4 = new QVBoxLayout(qgbModifiers);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        qcbForceCenter = new QCheckBox(qgbModifiers);
        qcbForceCenter->setObjectName(QString::fromUtf8("qcbForceCenter"));

        verticalLayout_4->addWidget(qcbForceCenter);


        gridLayout->addWidget(qgbModifiers, 3, 0, 1, 2);

        QWidget::setTabOrder(qrbUsers, qrbChannel);
        QWidget::setTabOrder(qrbChannel, qlwUsers);
        QWidget::setTabOrder(qlwUsers, qcbUser);
        QWidget::setTabOrder(qcbUser, qpbAdd);
        QWidget::setTabOrder(qpbAdd, qpbRemove);
        QWidget::setTabOrder(qpbRemove, qtwChannels);
        QWidget::setTabOrder(qtwChannels, qleGroup);
        QWidget::setTabOrder(qleGroup, qcbLinks);
        QWidget::setTabOrder(qcbLinks, qcbChildren);
        QWidget::setTabOrder(qcbChildren, qcbForceCenter);
        QWidget::setTabOrder(qcbForceCenter, qbbButtons);

        retranslateUi(GlobalShortcutTarget);
        QObject::connect(qbbButtons, SIGNAL(accepted()), GlobalShortcutTarget, SLOT(accept()));
        QObject::connect(qbbButtons, SIGNAL(rejected()), GlobalShortcutTarget, SLOT(reject()));

        qswStack->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(GlobalShortcutTarget);
    } // setupUi

    void retranslateUi(QDialog *GlobalShortcutTarget)
    {
        GlobalShortcutTarget->setWindowTitle(QApplication::translate("GlobalShortcutTarget", "Whisper Target", 0, QApplication::UnicodeUTF8));
        qrbUsers->setText(QApplication::translate("GlobalShortcutTarget", "Whisper to list of Users", 0, QApplication::UnicodeUTF8));
        qgbChannel->setTitle(QApplication::translate("GlobalShortcutTarget", "Channel Target", 0, QApplication::UnicodeUTF8));
        qlGroup->setText(QApplication::translate("GlobalShortcutTarget", "Restrict to Group", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleGroup->setToolTip(QApplication::translate("GlobalShortcutTarget", "If specified, only members of this group will receive the whisper.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        qcbLinks->setToolTip(QApplication::translate("GlobalShortcutTarget", "The whisper will also be transmitted to linked channels.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbLinks->setText(QApplication::translate("GlobalShortcutTarget", "Shout to Linked channels", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbChildren->setToolTip(QApplication::translate("GlobalShortcutTarget", "The whisper will also be sent to the subchannels of the channel target.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbChildren->setText(QApplication::translate("GlobalShortcutTarget", "Shout to subchannels", 0, QApplication::UnicodeUTF8));
        qgbUser->setTitle(QApplication::translate("GlobalShortcutTarget", "List of users", 0, QApplication::UnicodeUTF8));
        qpbAdd->setText(QApplication::translate("GlobalShortcutTarget", "Add", 0, QApplication::UnicodeUTF8));
        qpbRemove->setText(QApplication::translate("GlobalShortcutTarget", "Remove", 0, QApplication::UnicodeUTF8));
        qrbChannel->setText(QApplication::translate("GlobalShortcutTarget", "Shout to Channel", 0, QApplication::UnicodeUTF8));
        qgbModifiers->setTitle(QApplication::translate("GlobalShortcutTarget", "Modifiers", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbForceCenter->setToolTip(QApplication::translate("GlobalShortcutTarget", "Do not send positional audio information when using this whisper shortcut.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbForceCenter->setText(QApplication::translate("GlobalShortcutTarget", "Ignore positional audio", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GlobalShortcutTarget: public Ui_GlobalShortcutTarget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GLOBALSHORTCUTTARGET_H
