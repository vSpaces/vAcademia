/****************************************************************************
** Meta object code from reading C++ file 'NetworkConfig.h'
**
** Created: Thu 31. May 11:55:27 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../NetworkConfig.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkConfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkConfig[] = {

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
      15,   14,   14,   14, 0x0a,
      24,   14,   14,   14, 0x0a,
      33,   31,   14,   14, 0x0a,
      53,   14,   48,   14, 0x0a,
      68,   66,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetworkConfig[] = {
    "NetworkConfig\0\0accept()\0save()\0r\0"
    "load(Settings)\0bool\0expert(bool)\0v\0"
    "on_qcbType_currentIndexChanged(int)\0"
};

const QMetaObject NetworkConfig::staticMetaObject = {
    { &ConfigWidget::staticMetaObject, qt_meta_stringdata_NetworkConfig,
      qt_meta_data_NetworkConfig, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkConfig::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkConfig))
        return static_cast<void*>(const_cast< NetworkConfig*>(this));
    if (!strcmp(_clname, "Ui::NetworkConfig"))
        return static_cast< Ui::NetworkConfig*>(const_cast< NetworkConfig*>(this));
    return ConfigWidget::qt_metacast(_clname);
}

int NetworkConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ConfigWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: save(); break;
        case 2: load((*reinterpret_cast< const Settings(*)>(_a[1]))); break;
        case 3: { bool _r = expert((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: on_qcbType_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
