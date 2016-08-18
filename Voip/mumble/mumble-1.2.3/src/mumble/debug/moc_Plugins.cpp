/****************************************************************************
** Meta object code from reading C++ file 'Plugins.h'
**
** Created: Thu 31. May 11:55:25 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../Plugins.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Plugins.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifdef ADDITIONAL_WINDOWS
QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PluginConfig[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,
      23,   21,   13,   13, 0x0a,
      43,   13,   38,   13, 0x0a,
      56,   13,   13,   13, 0x0a,
      79,   13,   13,   13, 0x0a,
     101,   13,   13,   13, 0x0a,
     126,  124,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PluginConfig[] = {
    "PluginConfig\0\0save()\0r\0load(Settings)\0"
    "bool\0expert(bool)\0on_qpbConfig_clicked()\0"
    "on_qpbAbout_clicked()\0on_qpbReload_clicked()\0"
    ",\0on_qtwPlugins_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)\0"
};

const QMetaObject PluginConfig::staticMetaObject = {
    { &ConfigWidget::staticMetaObject, qt_meta_stringdata_PluginConfig,
      qt_meta_data_PluginConfig, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PluginConfig::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PluginConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PluginConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PluginConfig))
        return static_cast<void*>(const_cast< PluginConfig*>(this));
    if (!strcmp(_clname, "Ui::PluginConfig"))
        return static_cast< Ui::PluginConfig*>(const_cast< PluginConfig*>(this));
    return ConfigWidget::qt_metacast(_clname);
}

int PluginConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ConfigWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: save(); break;
        case 1: load((*reinterpret_cast< const Settings(*)>(_a[1]))); break;
        case 2: { bool _r = expert((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: on_qpbConfig_clicked(); break;
        case 4: on_qpbAbout_clicked(); break;
        case 5: on_qpbReload_clicked(); break;
        case 6: on_qtwPlugins_currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
#endif

static const uint qt_meta_data_Plugins[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x0a,
      28,    8,    8,    8, 0x0a,
      49,    8,   44,    8, 0x0a,
      57,    8,    8,    8, 0x0a,
      72,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Plugins[] = {
    "Plugins\0\0on_Timer_timeout()\0rescanPlugins()\0"
    "bool\0fetch()\0checkUpdates()\0finished()\0"
};

const QMetaObject Plugins::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Plugins,
      qt_meta_data_Plugins, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Plugins::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Plugins::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Plugins::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Plugins))
        return static_cast<void*>(const_cast< Plugins*>(this));
    return QObject::qt_metacast(_clname);
}

int Plugins::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_Timer_timeout(); break;
        case 1: rescanPlugins(); break;
        case 2: { bool _r = fetch();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: checkUpdates(); break;
        case 4: finished(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
