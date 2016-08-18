/****************************************************************************
** Meta object code from reading C++ file 'UserModel.h'
**
** Created: Thu 31. May 11:55:25 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../UserModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UserModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UserModel[] = {

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
      11,   10,   10,   10, 0x0a,
      32,   10,   10,   10, 0x0a,
      54,   10,   10,   10, 0x0a,
      74,   10,   10,   10, 0x0a,
      89,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_UserModel[] = {
    "UserModel\0\0userTalkingChanged()\0"
    "userMuteDeafChanged()\0ensureSelfVisible()\0"
    "recheckLinks()\0updateOverlay()\0"
};

const QMetaObject UserModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_UserModel,
      qt_meta_data_UserModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UserModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UserModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UserModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UserModel))
        return static_cast<void*>(const_cast< UserModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int UserModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: userTalkingChanged(); break;
        case 1: userMuteDeafChanged(); break;
        case 2: ensureSelfVisible(); break;
        case 3: recheckLinks(); break;
        case 4: updateOverlay(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
