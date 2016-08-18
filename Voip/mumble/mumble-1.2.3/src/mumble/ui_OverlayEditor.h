/********************************************************************************
** Form generated from reading UI file 'OverlayEditor.ui'
**
** Created: Fri 1. Jun 10:05:07 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OVERLAYEDITOR_H
#define UI_OVERLAYEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_OverlayEditor
{
public:
    QGridLayout *gridLayout;
    QGroupBox *qgbState;
    QHBoxLayout *horizontalLayout;
    QRadioButton *qrbPassive;
    QRadioButton *qrbTalking;
    QRadioButton *qrbWhisper;
    QRadioButton *qrbShout;
    QGraphicsView *qgvView;
    QGroupBox *qgbZoom;
    QVBoxLayout *verticalLayout;
    QSlider *qsZoom;
    QGroupBox *qgbElements;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *qcbAvatar;
    QCheckBox *qcbUser;
    QCheckBox *qcbChannel;
    QCheckBox *qcbMutedDeafened;
    QCheckBox *qcbBox;
    QDialogButtonBox *qdbbBox;

    void setupUi(QDialog *OverlayEditor)
    {
        if (OverlayEditor->objectName().isEmpty())
            OverlayEditor->setObjectName(QString::fromUtf8("OverlayEditor"));
        OverlayEditor->resize(718, 556);
        gridLayout = new QGridLayout(OverlayEditor);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qgbState = new QGroupBox(OverlayEditor);
        qgbState->setObjectName(QString::fromUtf8("qgbState"));
        horizontalLayout = new QHBoxLayout(qgbState);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        qrbPassive = new QRadioButton(qgbState);
        qrbPassive->setObjectName(QString::fromUtf8("qrbPassive"));

        horizontalLayout->addWidget(qrbPassive);

        qrbTalking = new QRadioButton(qgbState);
        qrbTalking->setObjectName(QString::fromUtf8("qrbTalking"));
        qrbTalking->setChecked(true);

        horizontalLayout->addWidget(qrbTalking);

        qrbWhisper = new QRadioButton(qgbState);
        qrbWhisper->setObjectName(QString::fromUtf8("qrbWhisper"));

        horizontalLayout->addWidget(qrbWhisper);

        qrbShout = new QRadioButton(qgbState);
        qrbShout->setObjectName(QString::fromUtf8("qrbShout"));

        horizontalLayout->addWidget(qrbShout);


        gridLayout->addWidget(qgbState, 0, 0, 1, 2);

        qgvView = new QGraphicsView(OverlayEditor);
        qgvView->setObjectName(QString::fromUtf8("qgvView"));
        qgvView->setInteractive(true);
        qgvView->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::TextAntialiasing);

        gridLayout->addWidget(qgvView, 1, 0, 1, 1);

        qgbZoom = new QGroupBox(OverlayEditor);
        qgbZoom->setObjectName(QString::fromUtf8("qgbZoom"));
        verticalLayout = new QVBoxLayout(qgbZoom);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qsZoom = new QSlider(qgbZoom);
        qsZoom->setObjectName(QString::fromUtf8("qsZoom"));
        qsZoom->setMinimum(1);
        qsZoom->setMaximum(10);
        qsZoom->setPageStep(5);
        qsZoom->setValue(2);
        qsZoom->setOrientation(Qt::Vertical);
        qsZoom->setTickPosition(QSlider::TicksBelow);
        qsZoom->setTickInterval(1);

        verticalLayout->addWidget(qsZoom);


        gridLayout->addWidget(qgbZoom, 1, 1, 1, 1);

        qgbElements = new QGroupBox(OverlayEditor);
        qgbElements->setObjectName(QString::fromUtf8("qgbElements"));
        horizontalLayout_2 = new QHBoxLayout(qgbElements);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        qcbAvatar = new QCheckBox(qgbElements);
        qcbAvatar->setObjectName(QString::fromUtf8("qcbAvatar"));

        horizontalLayout_2->addWidget(qcbAvatar);

        qcbUser = new QCheckBox(qgbElements);
        qcbUser->setObjectName(QString::fromUtf8("qcbUser"));

        horizontalLayout_2->addWidget(qcbUser);

        qcbChannel = new QCheckBox(qgbElements);
        qcbChannel->setObjectName(QString::fromUtf8("qcbChannel"));

        horizontalLayout_2->addWidget(qcbChannel);

        qcbMutedDeafened = new QCheckBox(qgbElements);
        qcbMutedDeafened->setObjectName(QString::fromUtf8("qcbMutedDeafened"));

        horizontalLayout_2->addWidget(qcbMutedDeafened);

        qcbBox = new QCheckBox(qgbElements);
        qcbBox->setObjectName(QString::fromUtf8("qcbBox"));

        horizontalLayout_2->addWidget(qcbBox);


        gridLayout->addWidget(qgbElements, 2, 0, 1, 2);

        qdbbBox = new QDialogButtonBox(OverlayEditor);
        qdbbBox->setObjectName(QString::fromUtf8("qdbbBox"));
        qdbbBox->setOrientation(Qt::Horizontal);
        qdbbBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Reset);

        gridLayout->addWidget(qdbbBox, 3, 0, 1, 2);

        QWidget::setTabOrder(qrbPassive, qrbTalking);
        QWidget::setTabOrder(qrbTalking, qrbWhisper);
        QWidget::setTabOrder(qrbWhisper, qrbShout);
        QWidget::setTabOrder(qrbShout, qcbAvatar);
        QWidget::setTabOrder(qcbAvatar, qcbUser);
        QWidget::setTabOrder(qcbUser, qcbChannel);
        QWidget::setTabOrder(qcbChannel, qcbMutedDeafened);
        QWidget::setTabOrder(qcbMutedDeafened, qcbBox);
        QWidget::setTabOrder(qcbBox, qsZoom);
        QWidget::setTabOrder(qsZoom, qgvView);
        QWidget::setTabOrder(qgvView, qdbbBox);

        retranslateUi(OverlayEditor);
        QObject::connect(qdbbBox, SIGNAL(accepted()), OverlayEditor, SLOT(accept()));
        QObject::connect(qdbbBox, SIGNAL(rejected()), OverlayEditor, SLOT(reject()));

        QMetaObject::connectSlotsByName(OverlayEditor);
    } // setupUi

    void retranslateUi(QDialog *OverlayEditor)
    {
        OverlayEditor->setWindowTitle(QApplication::translate("OverlayEditor", "Overlay Editor", 0, QApplication::UnicodeUTF8));
        qgbState->setTitle(QApplication::translate("OverlayEditor", "State", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qrbPassive->setToolTip(QApplication::translate("OverlayEditor", "User is not talking", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qrbPassive->setText(QApplication::translate("OverlayEditor", "Passive", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qrbTalking->setToolTip(QApplication::translate("OverlayEditor", "User is talking in your channel or a linked channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qrbTalking->setText(QApplication::translate("OverlayEditor", "Talking", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qrbWhisper->setToolTip(QApplication::translate("OverlayEditor", "User is whispering to you privately", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qrbWhisper->setText(QApplication::translate("OverlayEditor", "Private Whisper", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qrbShout->setToolTip(QApplication::translate("OverlayEditor", "User is shouting to your channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qrbShout->setText(QApplication::translate("OverlayEditor", "Channel Whisper", 0, QApplication::UnicodeUTF8));
        qgbZoom->setTitle(QApplication::translate("OverlayEditor", "Zoom", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qsZoom->setToolTip(QApplication::translate("OverlayEditor", "Zoom Factor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qgbElements->setTitle(QApplication::translate("OverlayEditor", "Enabled Elements", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbAvatar->setToolTip(QApplication::translate("OverlayEditor", "User avatar, chosen by each user", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbAvatar->setText(QApplication::translate("OverlayEditor", "Avatar", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbUser->setToolTip(QApplication::translate("OverlayEditor", "User's name", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbUser->setText(QApplication::translate("OverlayEditor", "Username", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbChannel->setToolTip(QApplication::translate("OverlayEditor", "Name of user's channel, if outside your current channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbChannel->setText(QApplication::translate("OverlayEditor", "Channel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbMutedDeafened->setToolTip(QApplication::translate("OverlayEditor", "Muted or deafened", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbMutedDeafened->setText(QApplication::translate("OverlayEditor", "Mute state", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbBox->setToolTip(QApplication::translate("OverlayEditor", "Bounding box, automatically shrunk to minimum size to contain all visible elements", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        qcbBox->setText(QApplication::translate("OverlayEditor", "Bounding box", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class OverlayEditor: public Ui_OverlayEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OVERLAYEDITOR_H
