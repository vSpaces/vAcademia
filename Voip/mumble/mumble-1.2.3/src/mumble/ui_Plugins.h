/********************************************************************************
** Form generated from reading UI file 'Plugins.ui'
**
** Created: Fri 1. Jun 10:05:06 2012
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#ifdef ADDITIONAL_WINDOWS
#ifndef UI_PLUGINS_H
#define UI_PLUGINS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PluginConfig
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *qgbOptions;
    QVBoxLayout *vboxLayout1;
    QCheckBox *qcbTransmit;
    QGroupBox *qgbPlugins;
    QVBoxLayout *vboxLayout2;
    QTreeWidget *qtwPlugins;
    QHBoxLayout *hboxLayout;
    QPushButton *qpbReload;
    QSpacerItem *spacerItem;
    QPushButton *qpbAbout;
    QPushButton *qpbConfig;

    void setupUi(QWidget *PluginConfig)
    {
        if (PluginConfig->objectName().isEmpty())
            PluginConfig->setObjectName(QString::fromUtf8("PluginConfig"));
        PluginConfig->resize(321, 235);
        vboxLayout = new QVBoxLayout(PluginConfig);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        qgbOptions = new QGroupBox(PluginConfig);
        qgbOptions->setObjectName(QString::fromUtf8("qgbOptions"));
        vboxLayout1 = new QVBoxLayout(qgbOptions);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        qcbTransmit = new QCheckBox(qgbOptions);
        qcbTransmit->setObjectName(QString::fromUtf8("qcbTransmit"));

        vboxLayout1->addWidget(qcbTransmit);


        vboxLayout->addWidget(qgbOptions);

        qgbPlugins = new QGroupBox(PluginConfig);
        qgbPlugins->setObjectName(QString::fromUtf8("qgbPlugins"));
        vboxLayout2 = new QVBoxLayout(qgbPlugins);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        qtwPlugins = new QTreeWidget(qgbPlugins);
        qtwPlugins->setObjectName(QString::fromUtf8("qtwPlugins"));
        qtwPlugins->setRootIsDecorated(false);
        qtwPlugins->setHeaderHidden(false);
        qtwPlugins->header()->setStretchLastSection(false);

        vboxLayout2->addWidget(qtwPlugins);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        qpbReload = new QPushButton(qgbPlugins);
        qpbReload->setObjectName(QString::fromUtf8("qpbReload"));

        hboxLayout->addWidget(qpbReload);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        qpbAbout = new QPushButton(qgbPlugins);
        qpbAbout->setObjectName(QString::fromUtf8("qpbAbout"));

        hboxLayout->addWidget(qpbAbout);

        qpbConfig = new QPushButton(qgbPlugins);
        qpbConfig->setObjectName(QString::fromUtf8("qpbConfig"));

        hboxLayout->addWidget(qpbConfig);


        vboxLayout2->addLayout(hboxLayout);


        vboxLayout->addWidget(qgbPlugins);


        retranslateUi(PluginConfig);

        QMetaObject::connectSlotsByName(PluginConfig);
    } // setupUi

    void retranslateUi(QWidget *PluginConfig)
    {
        PluginConfig->setWindowTitle(QApplication::translate("PluginConfig", "Form", 0, QApplication::UnicodeUTF8));
        qgbOptions->setTitle(QApplication::translate("PluginConfig", "Options", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qcbTransmit->setToolTip(QApplication::translate("PluginConfig", "Enable plugins and transmit positional information", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qcbTransmit->setWhatsThis(QApplication::translate("PluginConfig", "This allows plugins for supported games to fetch your in-game position and transmit it with each voice packet. This enables other users to hear your voice in-game from the direction your character is in relation to their own.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qcbTransmit->setText(QApplication::translate("PluginConfig", "Link to Game and Transmit Position", 0, QApplication::UnicodeUTF8));
        qgbPlugins->setTitle(QApplication::translate("PluginConfig", "Plugins", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = qtwPlugins->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("PluginConfig", "Enabled", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("PluginConfig", "Name", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbReload->setToolTip(QApplication::translate("PluginConfig", "Reloads all plugins", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbReload->setWhatsThis(QApplication::translate("PluginConfig", "This rescans and reloads plugins. Use this if you just added or changed a plugin to the plugins directory.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbReload->setText(QApplication::translate("PluginConfig", "&Reload plugins", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbAbout->setToolTip(QApplication::translate("PluginConfig", "Information about plugin", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbAbout->setWhatsThis(QApplication::translate("PluginConfig", "This shows a small information message about the plugin.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbAbout->setText(QApplication::translate("PluginConfig", "&About", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        qpbConfig->setToolTip(QApplication::translate("PluginConfig", "Show configuration page of plugin", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        qpbConfig->setWhatsThis(QApplication::translate("PluginConfig", "This shows the configuration page of the plugin, if any.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        qpbConfig->setText(QApplication::translate("PluginConfig", "&Configure", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PluginConfig: public Ui_PluginConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUGINS_H
#endif