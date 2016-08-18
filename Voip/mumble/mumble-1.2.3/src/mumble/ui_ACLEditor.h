/********************************************************************************
** Form generated from reading UI file 'ACLEditor.ui'
**
** Created: Fri 1. Jun 10:05:06 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS

#ifndef UI_ACLEDITOR_H
#define UI_ACLEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "RichTextEditor.h"

QT_BEGIN_NAMESPACE

class Ui_ACLEditor
{
public:
    QVBoxLayout *vboxLayout;
    QTabWidget *qtwTab;
    QWidget *qwChannel;
    QFormLayout *formLayout;
    QLabel *qlChannelName;
    QLineEdit *qleChannelName;
    QLabel *qlChannelDescription;
    QLabel *qlChannelPassword;
    QLineEdit *qleChannelPassword;
    QCheckBox *qcbChannelTemporary;
    QSpinBox *qsbChannelPosition;
    QLabel *qlChannelPosition;
    RichTextEditor *rteChannelDescription;
    QWidget *qwGroups;
    QVBoxLayout *vboxLayout1;
    QGroupBox *qgbGroups;
    QHBoxLayout *hboxLayout;
    QComboBox *qcbGroupList;
    QPushButton *qpbGroupAdd;
    QPushButton *qpbGroupRemove;
    QCheckBox *qcbGroupInherit;
    QCheckBox *qcbGroupInheritable;
    QCheckBox *qcbGroupInherited;
    QGroupBox *qgbGroupMembers;
    QGridLayout *gridLayout;
    QLabel *qliGroupAdd;
    QLabel *qliGroupInherit;
    QListWidget *qlwGroupAdd;
    QListWidget *qlwGroupRemove;
    QListWidget *qlwGroupInherit;
    QComboBox *qcbGroupAdd;
    QPushButton *qpbGroupAddAdd;
    QComboBox *qcbGroupRemove;
    QPushButton *qpbGroupRemoveAdd;
    QPushButton *qpbGroupAddRemove;
    QPushButton *qpbGroupRemoveRemove;
    QPushButton *qpbGroupInheritRemove;
    QLabel *qliGroupRemove;
    QWidget *qwACL;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *qlVerticalACL;
    QGroupBox *qgbACLs;
    QGridLayout *gridLayout1;
    QListWidget *qlwACLs;
    QCheckBox *qcbACLInherit;
    QPushButton *qpbACLUp;
    QPushButton *qpbACLDown;
    QPushButton *qpbACLAdd;
    QPushButton *qpbACLRemove;
    QGroupBox *qgbACLapply;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *qcbACLApplySubs;
    QCheckBox *qcbACLApplyHere;
    QGroupBox *qgbACLugroup;
    QGridLayout *gridLayout2;
    QLabel *qliACLGroup;
    QComboBox *qcbACLGroup;
    QLabel *qliACLUser;
    QComboBox *qcbACLUser;
    QGroupBox *qgbACLpermissions;
    QDialogButtonBox *qdbbButtons;

    void setupUi(QDialog *ACLEditor)
    {
        if (ACLEditor->objectName().isEmpty())
            ACLEditor->setObjectName(QString::fromUtf8("ACLEditor"));
        ACLEditor->resize(607, 509);
        vboxLayout = new QVBoxLayout(ACLEditor);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        qtwTab = new QTabWidget(ACLEditor);
        qtwTab->setObjectName(QString::fromUtf8("qtwTab"));
        qwChannel = new QWidget();
        qwChannel->setObjectName(QString::fromUtf8("qwChannel"));
        formLayout = new QFormLayout(qwChannel);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        qlChannelName = new QLabel(qwChannel);
        qlChannelName->setObjectName(QString::fromUtf8("qlChannelName"));

        formLayout->setWidget(0, QFormLayout::LabelRole, qlChannelName);

        qleChannelName = new QLineEdit(qwChannel);
        qleChannelName->setObjectName(QString::fromUtf8("qleChannelName"));

        formLayout->setWidget(0, QFormLayout::FieldRole, qleChannelName);

        qlChannelDescription = new QLabel(qwChannel);
        qlChannelDescription->setObjectName(QString::fromUtf8("qlChannelDescription"));

        formLayout->setWidget(1, QFormLayout::LabelRole, qlChannelDescription);

        qlChannelPassword = new QLabel(qwChannel);
        qlChannelPassword->setObjectName(QString::fromUtf8("qlChannelPassword"));

        formLayout->setWidget(2, QFormLayout::LabelRole, qlChannelPassword);

        qleChannelPassword = new QLineEdit(qwChannel);
        qleChannelPassword->setObjectName(QString::fromUtf8("qleChannelPassword"));

        formLayout->setWidget(2, QFormLayout::FieldRole, qleChannelPassword);

        qcbChannelTemporary = new QCheckBox(qwChannel);
        qcbChannelTemporary->setObjectName(QString::fromUtf8("qcbChannelTemporary"));

        formLayout->setWidget(4, QFormLayout::FieldRole, qcbChannelTemporary);

        qsbChannelPosition = new QSpinBox(qwChannel);
        qsbChannelPosition->setObjectName(QString::fromUtf8("qsbChannelPosition"));
        qsbChannelPosition->setMaximumSize(QSize(16777215, 16777215));
        qsbChannelPosition->setMaximum(99);

        formLayout->setWidget(3, QFormLayout::FieldRole, qsbChannelPosition);

        qlChannelPosition = new QLabel(qwChannel);
        qlChannelPosition->setObjectName(QString::fromUtf8("qlChannelPosition"));

        formLayout->setWidget(3, QFormLayout::LabelRole, qlChannelPosition);

        rteChannelDescription = new RichTextEditor(qwChannel);
        rteChannelDescription->setObjectName(QString::fromUtf8("rteChannelDescription"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(rteChannelDescription->sizePolicy().hasHeightForWidth());
        rteChannelDescription->setSizePolicy(sizePolicy);

        formLayout->setWidget(1, QFormLayout::FieldRole, rteChannelDescription);

        qtwTab->addTab(qwChannel, QString());
        qwGroups = new QWidget();
        qwGroups->setObjectName(QString::fromUtf8("qwGroups"));
        vboxLayout1 = new QVBoxLayout(qwGroups);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        qgbGroups = new QGroupBox(qwGroups);
        qgbGroups->setObjectName(QString::fromUtf8("qgbGroups"));
        hboxLayout = new QHBoxLayout(qgbGroups);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        qcbGroupList = new QComboBox(qgbGroups);
        qcbGroupList->setObjectName(QString::fromUtf8("qcbGroupList"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qcbGroupList->sizePolicy().hasHeightForWidth());
        qcbGroupList->setSizePolicy(sizePolicy1);
        qcbGroupList->setMaximumSize(QSize(300, 16777215));
        qcbGroupList->setSizeIncrement(QSize(0, 0));
        qcbGroupList->setEditable(true);

        hboxLayout->addWidget(qcbGroupList);

        qpbGroupAdd = new QPushButton(qgbGroups);
        qpbGroupAdd->setObjectName(QString::fromUtf8("qpbGroupAdd"));

        hboxLayout->addWidget(qpbGroupAdd);

        qpbGroupRemove = new QPushButton(qgbGroups);
        qpbGroupRemove->setObjectName(QString::fromUtf8("qpbGroupRemove"));
        qpbGroupRemove->setAutoDefault(false);

        hboxLayout->addWidget(qpbGroupRemove);

        qcbGroupInherit = new QCheckBox(qgbGroups);
        qcbGroupInherit->setObjectName(QString::fromUtf8("qcbGroupInherit"));

        hboxLayout->addWidget(qcbGroupInherit);

        qcbGroupInheritable = new QCheckBox(qgbGroups);
        qcbGroupInheritable->setObjectName(QString::fromUtf8("qcbGroupInheritable"));

        hboxLayout->addWidget(qcbGroupInheritable);

        qcbGroupInherited = new QCheckBox(qgbGroups);
        qcbGroupInherited->setObjectName(QString::fromUtf8("qcbGroupInherited"));
        qcbGroupInherited->setEnabled(false);

        hboxLayout->addWidget(qcbGroupInherited);


        vboxLayout1->addWidget(qgbGroups);

        qgbGroupMembers = new QGroupBox(qwGroups);
        qgbGroupMembers->setObjectName(QString::fromUtf8("qgbGroupMembers"));
        gridLayout = new QGridLayout(qgbGroupMembers);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qliGroupAdd = new QLabel(qgbGroupMembers);
        qliGroupAdd->setObjectName(QString::fromUtf8("qliGroupAdd"));

        gridLayout->addWidget(qliGroupAdd, 0, 0, 1, 3);

        qliGroupInherit = new QLabel(qgbGroupMembers);
        qliGroupInherit->setObjectName(QString::fromUtf8("qliGroupInherit"));

        gridLayout->addWidget(qliGroupInherit, 0, 6, 1, 1);

        qlwGroupAdd = new QListWidget(qgbGroupMembers);
        qlwGroupAdd->setObjectName(QString::fromUtf8("qlwGroupAdd"));

        gridLayout->addWidget(qlwGroupAdd, 1, 0, 1, 3);

        qlwGroupRemove = new QListWidget(qgbGroupMembers);
        qlwGroupRemove->setObjectName(QString::fromUtf8("qlwGroupRemove"));

        gridLayout->addWidget(qlwGroupRemove, 1, 3, 1, 3);

        qlwGroupInherit = new QListWidget(qgbGroupMembers);
        qlwGroupInherit->setObjectName(QString::fromUtf8("qlwGroupInherit"));

        gridLayout->addWidget(qlwGroupInherit, 1, 6, 2, 1);

        qcbGroupAdd = new QComboBox(qgbGroupMembers);
        qcbGroupAdd->setObjectName(QString::fromUtf8("qcbGroupAdd"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(qcbGroupAdd->sizePolicy().hasHeightForWidth());
        qcbGroupAdd->setSizePolicy(sizePolicy2);
        qcbGroupAdd->setEditable(true);
        qcbGroupAdd->setInsertPolicy(QComboBox::NoInsert);
        qcbGroupAdd->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

        gridLayout->addWidget(qcbGroupAdd, 2, 0, 1, 2);

        qpbGroupAddAdd = new QPushButton(qgbGroupMembers);
        qpbGroupAddAdd->setObjectName(QString::fromUtf8("qpbGroupAddAdd"));

        gridLayout->addWidget(qpbGroupAddAdd, 2, 2, 1, 1);

        qcbGroupRemove = new QComboBox(qgbGroupMembers);
        qcbGroupRemove->setObjectName(QString::fromUtf8("qcbGroupRemove"));
        sizePolicy2.setHeightForWidth(qcbGroupRemove->sizePolicy().hasHeightForWidth());
        qcbGroupRemove->setSizePolicy(sizePolicy2);
        qcbGroupRemove->setEditable(true);
        qcbGroupRemove->setInsertPolicy(QComboBox::NoInsert);
        qcbGroupRemove->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

        gridLayout->addWidget(qcbGroupRemove, 2, 3, 1, 2);

        qpbGroupRemoveAdd = new QPushButton(qgbGroupMembers);
        qpbGroupRemoveAdd->setObjectName(QString::fromUtf8("qpbGroupRemoveAdd"));

        gridLayout->addWidget(qpbGroupRemoveAdd, 2, 5, 1, 1);

        qpbGroupAddRemove = new QPushButton(qgbGroupMembers);
        qpbGroupAddRemove->setObjectName(QString::fromUtf8("qpbGroupAddRemove"));

        gridLayout->addWidget(qpbGroupAddRemove, 3, 0, 1, 3);

        qpbGroupRemoveRemove = new QPushButton(qgbGroupMembers);
        qpbGroupRemoveRemove->setObjectName(QString::fromUtf8("qpbGroupRemoveRemove"));

        gridLayout->addWidget(qpbGroupRemoveRemove, 3, 3, 1, 3);

        qpbGroupInheritRemove = new QPushButton(qgbGroupMembers);
        qpbGroupInheritRemove->setObjectName(QString::fromUtf8("qpbGroupInheritRemove"));

        gridLayout->addWidget(qpbGroupInheritRemove, 3, 6, 1, 1);

        qliGroupRemove = new QLabel(qgbGroupMembers);
        qliGroupRemove->setObjectName(QString::fromUtf8("qliGroupRemove"));

        gridLayout->addWidget(qliGroupRemove, 0, 3, 1, 3);


        vboxLayout1->addWidget(qgbGroupMembers);

        qtwTab->addTab(qwGroups, QString());
        qwACL = new QWidget();
        qwACL->setObjectName(QString::fromUtf8("qwACL"));
        horizontalLayout = new QHBoxLayout(qwACL);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        qlVerticalACL = new QVBoxLayout();
        qlVerticalACL->setObjectName(QString::fromUtf8("qlVerticalACL"));
        qgbACLs = new QGroupBox(qwACL);
        qgbACLs->setObjectName(QString::fromUtf8("qgbACLs"));
        gridLayout1 = new QGridLayout(qgbACLs);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        qlwACLs = new QListWidget(qgbACLs);
        qlwACLs->setObjectName(QString::fromUtf8("qlwACLs"));

        gridLayout1->addWidget(qlwACLs, 0, 0, 2, 5);

        qcbACLInherit = new QCheckBox(qgbACLs);
        qcbACLInherit->setObjectName(QString::fromUtf8("qcbACLInherit"));

        gridLayout1->addWidget(qcbACLInherit, 2, 0, 1, 1);

        qpbACLUp = new QPushButton(qgbACLs);
        qpbACLUp->setObjectName(QString::fromUtf8("qpbACLUp"));
        qpbACLUp->setAutoDefault(false);

        gridLayout1->addWidget(qpbACLUp, 2, 1, 1, 1);

        qpbACLDown = new QPushButton(qgbACLs);
        qpbACLDown->setObjectName(QString::fromUtf8("qpbACLDown"));
        qpbACLDown->setAutoDefault(false);

        gridLayout1->addWidget(qpbACLDown, 2, 2, 1, 1);

        qpbACLAdd = new QPushButton(qgbACLs);
        qpbACLAdd->setObjectName(QString::fromUtf8("qpbACLAdd"));
        qpbACLAdd->setAutoDefault(false);

        gridLayout1->addWidget(qpbACLAdd, 2, 3, 1, 1);

        qpbACLRemove = new QPushButton(qgbACLs);
        qpbACLRemove->setObjectName(QString::fromUtf8("qpbACLRemove"));
        qpbACLRemove->setAutoDefault(false);

        gridLayout1->addWidget(qpbACLRemove, 2, 4, 1, 1);


        qlVerticalACL->addWidget(qgbACLs);

        qgbACLapply = new QGroupBox(qwACL);
        qgbACLapply->setObjectName(QString::fromUtf8("qgbACLapply"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(qgbACLapply->sizePolicy().hasHeightForWidth());
        qgbACLapply->setSizePolicy(sizePolicy3);
        horizontalLayout_2 = new QHBoxLayout(qgbACLapply);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        qcbACLApplySubs = new QCheckBox(qgbACLapply);
        qcbACLApplySubs->setObjectName(QString::fromUtf8("qcbACLApplySubs"));

        horizontalLayout_2->addWidget(qcbACLApplySubs);

        qcbACLApplyHere = new QCheckBox(qgbACLapply);
        qcbACLApplyHere->setObjectName(QString::fromUtf8("qcbACLApplyHere"));

        horizontalLayout_2->addWidget(qcbACLApplyHere);


        qlVerticalACL->addWidget(qgbACLapply);

        qgbACLugroup = new QGroupBox(qwACL);
        qgbACLugroup->setObjectName(QString::fromUtf8("qgbACLugroup"));
        sizePolicy3.setHeightForWidth(qgbACLugroup->sizePolicy().hasHeightForWidth());
        qgbACLugroup->setSizePolicy(sizePolicy3);
        gridLayout2 = new QGridLayout(qgbACLugroup);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        qliACLGroup = new QLabel(qgbACLugroup);
        qliACLGroup->setObjectName(QString::fromUtf8("qliACLGroup"));

        gridLayout2->addWidget(qliACLGroup, 0, 0, 1, 1);

        qcbACLGroup = new QComboBox(qgbACLugroup);
        qcbACLGroup->setObjectName(QString::fromUtf8("qcbACLGroup"));
        sizePolicy2.setHeightForWidth(qcbACLGroup->sizePolicy().hasHeightForWidth());
        qcbACLGroup->setSizePolicy(sizePolicy2);
        qcbACLGroup->setEditable(true);

        gridLayout2->addWidget(qcbACLGroup, 0, 1, 1, 1);

        qliACLUser = new QLabel(qgbACLugroup);
        qliACLUser->setObjectName(QString::fromUtf8("qliACLUser"));

        gridLayout2->addWidget(qliACLUser, 1, 0, 1, 1);

        qcbACLUser = new QComboBox(qgbACLugroup);
        qcbACLUser->setObjectName(QString::fromUtf8("qcbACLUser"));
        sizePolicy2.setHeightForWidth(qcbACLUser->sizePolicy().hasHeightForWidth());
        qcbACLUser->setSizePolicy(sizePolicy2);
        qcbACLUser->setEditable(true);
        qcbACLUser->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

        gridLayout2->addWidget(qcbACLUser, 1, 1, 1, 1);


        qlVerticalACL->addWidget(qgbACLugroup);


        horizontalLayout->addLayout(qlVerticalACL);

        qgbACLpermissions = new QGroupBox(qwACL);
        qgbACLpermissions->setObjectName(QString::fromUtf8("qgbACLpermissions"));

        horizontalLayout->addWidget(qgbACLpermissions);

        qtwTab->addTab(qwACL, QString());

        vboxLayout->addWidget(qtwTab);

        qdbbButtons = new QDialogButtonBox(ACLEditor);
        qdbbButtons->setObjectName(QString::fromUtf8("qdbbButtons"));
        qdbbButtons->setOrientation(Qt::Horizontal);
        qdbbButtons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(qdbbButtons);

#ifndef QT_NO_SHORTCUT
        qliGroupAdd->setBuddy(qlwGroupAdd);
        qliGroupInherit->setBuddy(qlwGroupInherit);
        qliGroupRemove->setBuddy(qlwGroupRemove);
        qliACLGroup->setBuddy(qcbACLGroup);
        qliACLUser->setBuddy(qcbACLUser);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(qtwTab, qleChannelName);
        QWidget::setTabOrder(qleChannelName, qleChannelPassword);
        QWidget::setTabOrder(qleChannelPassword, qsbChannelPosition);
        QWidget::setTabOrder(qsbChannelPosition, qcbChannelTemporary);
        QWidget::setTabOrder(qcbChannelTemporary, qcbGroupList);
        QWidget::setTabOrder(qcbGroupList, qpbGroupAdd);
        QWidget::setTabOrder(qpbGroupAdd, qpbGroupRemove);
        QWidget::setTabOrder(qpbGroupRemove, qcbGroupInherit);
        QWidget::setTabOrder(qcbGroupInherit, qcbGroupInheritable);
        QWidget::setTabOrder(qcbGroupInheritable, qcbGroupInherited);
        QWidget::setTabOrder(qcbGroupInherited, qlwGroupAdd);
        QWidget::setTabOrder(qlwGroupAdd, qcbGroupAdd);
        QWidget::setTabOrder(qcbGroupAdd, qpbGroupAddAdd);
        QWidget::setTabOrder(qpbGroupAddAdd, qpbGroupAddRemove);
        QWidget::setTabOrder(qpbGroupAddRemove, qlwGroupRemove);
        QWidget::setTabOrder(qlwGroupRemove, qcbGroupRemove);
        QWidget::setTabOrder(qcbGroupRemove, qpbGroupRemoveAdd);
        QWidget::setTabOrder(qpbGroupRemoveAdd, qpbGroupRemoveRemove);
        QWidget::setTabOrder(qpbGroupRemoveRemove, qlwGroupInherit);
        QWidget::setTabOrder(qlwGroupInherit, qpbGroupInheritRemove);
        QWidget::setTabOrder(qpbGroupInheritRemove, qlwACLs);
        QWidget::setTabOrder(qlwACLs, qcbACLInherit);
        QWidget::setTabOrder(qcbACLInherit, qpbACLUp);
        QWidget::setTabOrder(qpbACLUp, qpbACLDown);
        QWidget::setTabOrder(qpbACLDown, qpbACLAdd);
        QWidget::setTabOrder(qpbACLAdd, qpbACLRemove);
        QWidget::setTabOrder(qpbACLRemove, qcbACLApplySubs);
        QWidget::setTabOrder(qcbACLApplySubs, qcbACLApplyHere);
        QWidget::setTabOrder(qcbACLApplyHere, qcbACLGroup);
        QWidget::setTabOrder(qcbACLGroup, qcbACLUser);
        QWidget::setTabOrder(qcbACLUser, qdbbButtons);

        retranslateUi(ACLEditor);
        QObject::connect(qdbbButtons, SIGNAL(accepted()), ACLEditor, SLOT(accept()));
        QObject::connect(qdbbButtons, SIGNAL(rejected()), ACLEditor, SLOT(reject()));
        QObject::connect(qlwGroupInherit, SIGNAL(doubleClicked(QModelIndex)), qpbGroupInheritRemove, SLOT(animateClick()));
        QObject::connect(qleChannelName, SIGNAL(returnPressed()), ACLEditor, SLOT(accept()));
        QObject::connect(qleChannelPassword, SIGNAL(returnPressed()), ACLEditor, SLOT(accept()));

        qtwTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ACLEditor);
    } // setupUi

    void retranslateUi(QDialog *ACLEditor)
    {
        ACLEditor->setWindowTitle(QApplication::translate("ACLEditor", "Dialog", 0, QApplication::UnicodeUTF8));
        qlChannelName->setText(QApplication::translate("ACLEditor", "Name", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleChannelName->setToolTip(QApplication::translate("ACLEditor", "Enter the channel name here.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qleChannelName->setWhatsThis(QApplication::translate("ACLEditor", "<b>Name</b><br />Enter the channel name in this field. The name has to comply with the restriction imposed by the server you are connected to.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlChannelDescription->setText(QApplication::translate("ACLEditor", "Description", 0, QApplication::UnicodeUTF8));
        qlChannelPassword->setText(QApplication::translate("ACLEditor", "Password", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qleChannelPassword->setToolTip(QApplication::translate("ACLEditor", "Enter the channel password here.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qleChannelPassword->setWhatsThis(QApplication::translate("ACLEditor", "<b>Password</b><br />This field allows you to easily set and change the password of a channel. It uses Mumble's access tokens feature in the background. Use ACLs and groups if you need more fine grained and powerful access control.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbChannelTemporary->setToolTip(QApplication::translate("ACLEditor", "Check to create a temporary channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbChannelTemporary->setWhatsThis(QApplication::translate("ACLEditor", "<b>Temporary</b><br />\n"
"When checked the channel created will be marked as temporary. This means when the last player leaves it the channel will be automatically deleted by the server.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbChannelTemporary->setText(QApplication::translate("ACLEditor", "Temporary", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsbChannelPosition->setToolTip(QApplication::translate("ACLEditor", "Channel positioning facility value", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qsbChannelPosition->setWhatsThis(QApplication::translate("ACLEditor", "<b>Position</b><br/>\n"
"This value enables you to change the way Mumble arranges the channels in the tree. A channel with a higher <i>Position</i> value will always be placed below one with a lower value and the other way around. If the <i>Position</i> value of two channels is equal they will get sorted alphabetically by their name.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qlChannelPosition->setText(QApplication::translate("ACLEditor", "Position", 0, QApplication::UnicodeUTF8));
        qtwTab->setTabText(qtwTab->indexOf(qwChannel), QApplication::translate("ACLEditor", "&Properties", 0, QApplication::UnicodeUTF8));
        qgbGroups->setTitle(QApplication::translate("ACLEditor", "Group", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbGroupList->setToolTip(QApplication::translate("ACLEditor", "List of groups", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbGroupList->setWhatsThis(QApplication::translate("ACLEditor", "<b>Group</b><br />\n"
"These are all the groups currently defined for the channel. To create a new group, just type in the name and press enter.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qpbGroupAdd->setToolTip(QApplication::translate("ACLEditor", "Add new group", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbGroupAdd->setWhatsThis(QApplication::translate("ACLEditor", "<b>Add</b><br/>\n"
"Add a new group.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbGroupAdd->setText(QApplication::translate("ACLEditor", "Add", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbGroupRemove->setToolTip(QApplication::translate("ACLEditor", "Remove selected group", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbGroupRemove->setWhatsThis(QApplication::translate("ACLEditor", "<b>Remove</b><br />This removes the currently selected group. If the group was inherited, it will not be removed from the list, but all local information about the group will be cleared.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbGroupRemove->setText(QApplication::translate("ACLEditor", "Remove", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbGroupInherit->setToolTip(QApplication::translate("ACLEditor", "Inherit group members from parent", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbGroupInherit->setWhatsThis(QApplication::translate("ACLEditor", "<b>Inherit</b><br />This inherits all the members in the group from the parent, if the group is marked as <i>Inheritable</i> in the parent channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbGroupInherit->setText(QApplication::translate("ACLEditor", "Inherit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbGroupInheritable->setToolTip(QApplication::translate("ACLEditor", "Make group inheritable to sub-channels", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbGroupInheritable->setWhatsThis(QApplication::translate("ACLEditor", "<b>Inheritable</b><br />This makes this group inheritable to sub-channels. If the group is non-inheritable, sub-channels are still free to create a new group with the same name.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbGroupInheritable->setText(QApplication::translate("ACLEditor", "Inheritable", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbGroupInherited->setToolTip(QApplication::translate("ACLEditor", "Group was inherited from parent channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbGroupInherited->setWhatsThis(QApplication::translate("ACLEditor", "<b>Inherited</b><br />This indicates that the group was inherited from the parent channel. You cannot edit this flag, it's just for information.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbGroupInherited->setText(QApplication::translate("ACLEditor", "Inherited", 0, QApplication::UnicodeUTF8));
        qgbGroupMembers->setTitle(QApplication::translate("ACLEditor", "Members", 0, QApplication::UnicodeUTF8));
        qliGroupAdd->setText(QApplication::translate("ACLEditor", "Members", 0, QApplication::UnicodeUTF8));
        qliGroupInherit->setText(QApplication::translate("ACLEditor", "Inherited members", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlwGroupAdd->setToolTip(QApplication::translate("ACLEditor", "Contains the list of members added to the group by this channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlwGroupAdd->setWhatsThis(QApplication::translate("ACLEditor", "<b>Members</b><br />\n"
"This list contains all members that were added to the group by the current channel. Be aware that this does not include members inherited by higher levels of the channel tree. These can be found in the <i>Inherited members</i> list. To prevent this list to be inherited by lower level channels uncheck <i>Inheritable</i> or manually add the members to the <i>Excluded members</i> list.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qlwGroupRemove->setToolTip(QApplication::translate("ACLEditor", "Contains a list of members whose group membership will not be inherited from the parent channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlwGroupRemove->setWhatsThis(QApplication::translate("ACLEditor", "<b>Excluded members</b><br />\n"
"Contains a list of members whose group membership will not be inherited from the parent channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qlwGroupInherit->setToolTip(QApplication::translate("ACLEditor", "Contains the list of members inherited by other channels.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlwGroupInherit->setWhatsThis(QApplication::translate("ACLEditor", "<b>Inherited members</b><br />\n"
"Contains the list of members inherited by the current channel. Uncheck <i>Inherit</i> to prevent inheritance from higher level channels.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbGroupAdd->setToolTip(QApplication::translate("ACLEditor", "Add member to group", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbGroupAdd->setWhatsThis(QApplication::translate("ACLEditor", "Type in the name of a user you wish to add to the group and click Add.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbGroupAddAdd->setText(QApplication::translate("ACLEditor", "Add", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbGroupRemove->setToolTip(QApplication::translate("ACLEditor", "Remove member from group", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbGroupRemove->setWhatsThis(QApplication::translate("ACLEditor", "Type in the name of a user you wish to remove from the group and click Add.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbGroupRemoveAdd->setText(QApplication::translate("ACLEditor", "Add", 0, QApplication::UnicodeUTF8));
        qpbGroupAddRemove->setText(QApplication::translate("ACLEditor", "Remove", 0, QApplication::UnicodeUTF8));
        qpbGroupRemoveRemove->setText(QApplication::translate("ACLEditor", "Remove", 0, QApplication::UnicodeUTF8));
        qpbGroupInheritRemove->setText(QApplication::translate("ACLEditor", "Exclude", 0, QApplication::UnicodeUTF8));
        qliGroupRemove->setText(QApplication::translate("ACLEditor", "Excluded members", 0, QApplication::UnicodeUTF8));
        qtwTab->setTabText(qtwTab->indexOf(qwGroups), QApplication::translate("ACLEditor", "&Groups", 0, QApplication::UnicodeUTF8));
        qgbACLs->setTitle(QApplication::translate("ACLEditor", "Active ACLs", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qlwACLs->setToolTip(QApplication::translate("ACLEditor", "List of entries", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qlwACLs->setWhatsThis(QApplication::translate("ACLEditor", "This shows all the entries active on this channel. Entries inherited from parent channels will be shown in italics.<br />ACLs are evaluated top to bottom, meaning priority increases as you move down the list.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        qcbACLInherit->setToolTip(QApplication::translate("ACLEditor", "Inherit ACL of parent?", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbACLInherit->setWhatsThis(QApplication::translate("ACLEditor", "This sets whether or not the ACL up the chain of parent channels are applied to this object. Only those entries that are marked in the parent as \"Apply to sub-channels\" will be inherited.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbACLInherit->setText(QApplication::translate("ACLEditor", "Inherit ACLs", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbACLUp->setToolTip(QApplication::translate("ACLEditor", "Move entry up", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbACLUp->setWhatsThis(QApplication::translate("ACLEditor", "This moves the entry up in the list. As entries are evaluated in order, this may change the effective permissions of users. You cannot move an entry above an inherited entry, if you really need that you'll have to duplicate the inherited entry.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbACLUp->setText(QApplication::translate("ACLEditor", "&Up", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbACLDown->setToolTip(QApplication::translate("ACLEditor", "Move entry down", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbACLDown->setWhatsThis(QApplication::translate("ACLEditor", "This moves the entry down in the list. As entries are evaluated in order, this may change the effective permissions of users.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbACLDown->setText(QApplication::translate("ACLEditor", "&Down", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbACLAdd->setToolTip(QApplication::translate("ACLEditor", "Add new entry", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbACLAdd->setWhatsThis(QApplication::translate("ACLEditor", "This adds a new entry, initially set with no permissions and applying to all.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbACLAdd->setText(QApplication::translate("ACLEditor", "&Add", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbACLRemove->setToolTip(QApplication::translate("ACLEditor", "Remove entry", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbACLRemove->setWhatsThis(QApplication::translate("ACLEditor", "This removes the currently selected entry.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbACLRemove->setText(QApplication::translate("ACLEditor", "&Remove", 0, QApplication::UnicodeUTF8));
        qgbACLapply->setTitle(QApplication::translate("ACLEditor", "Context", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbACLApplySubs->setToolTip(QApplication::translate("ACLEditor", "Entry should apply to sub-channels.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbACLApplySubs->setWhatsThis(QApplication::translate("ACLEditor", "This makes the entry apply to sub-channels of this channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbACLApplySubs->setText(QApplication::translate("ACLEditor", "Applies to sub-channels", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbACLApplyHere->setToolTip(QApplication::translate("ACLEditor", "Entry should apply to this channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbACLApplyHere->setWhatsThis(QApplication::translate("ACLEditor", "This makes the entry apply to this channel.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbACLApplyHere->setText(QApplication::translate("ACLEditor", "Applies to this channel", 0, QApplication::UnicodeUTF8));
        qgbACLugroup->setTitle(QApplication::translate("ACLEditor", "User/Group", 0, QApplication::UnicodeUTF8));
        qliACLGroup->setText(QApplication::translate("ACLEditor", "Group", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbACLGroup->setToolTip(QApplication::translate("ACLEditor", "Group this entry applies to", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbACLGroup->setWhatsThis(QApplication::translate("ACLEditor", "This controls which group of users this entry applies to.<br />Note that the group is evaluated in the context of the channel the entry is used in. For example, the default ACL on the Root channel gives <i>Write</i> permission to the <i>admin</i> group. This entry, if inherited by a channel, will give a user write privileges if he belongs to the <i>admin</i> group in that channel, even if he doesn't belong to the <i>admin</i> group in the channel where the ACL originated.<br />If a group name starts with '!', its membership is negated, and if it starts with '~', it is evaluated in the channel the ACL was defined in, rather than the channel the ACL is active in.<br />If a group name starts with '#', it is interpreted as an access token. Users must have entered whatever follows the '#' in their list of access tokens to match. This can be used for very simple password access to channels for non-authenticated users.<br />If a group name starts with '$', it will only match users whose certificate hash matches what "
                        "follows the '$'.<br />A few special predefined groups are:<br /><b>all</b> - Everyone will match.<br /><b>auth</b> - All authenticated users will match.<br /><b>sub,a,b,c</b> - User currently in a sub-channel minimum <i>a</i> common parents, and between <i>b</i> and <i>c</i> channels down the chain. See the website for more extensive documentation on this one.<br /><b>in</b> - Users currently in the channel will match (convenience for '<i>sub,0,0,0</i>').<br /><b>out</b> - Users outside the channel will match (convenience for '<i>!sub,0,0,0</i>').<br />Note that an entry applies to either a user or a group, not both.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qliACLUser->setText(QApplication::translate("ACLEditor", "User ID", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbACLUser->setToolTip(QApplication::translate("ACLEditor", "User this entry applies to", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbACLUser->setWhatsThis(QApplication::translate("ACLEditor", "This controls which user this entry applies to. Just type in the user name and hit enter to query the server for a match.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qgbACLpermissions->setTitle(QApplication::translate("ACLEditor", "Permissions", 0, QApplication::UnicodeUTF8));
        qtwTab->setTabText(qtwTab->indexOf(qwACL), QApplication::translate("ACLEditor", "&ACL", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ACLEditor: public Ui_ACLEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACLEDITOR_H

#endif // ADDITIONAL_WINDOWS