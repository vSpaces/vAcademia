/****************************************************************************
** Meta object code from reading C++ file 'Usage.h'
**
** Created: Thu 31. May 11:55:27 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../Usage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Usage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Usage[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x0a,
      23,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Usage[] = {
    "Usage\0\0registerUsage()\0reportJitter()\0"
};

const QMetaObject Usage::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Usage,
      qt_meta_data_Usage, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Usage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Usage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Usage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Usage))
        return static_cast<void*>(const_cast< Usage*>(this));
    return QObject::qt_metacast(_clname);
}

int Usage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: registerUsage(); break;
        case 1: reportJitter(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
