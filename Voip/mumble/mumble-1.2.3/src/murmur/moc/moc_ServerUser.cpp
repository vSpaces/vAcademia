/****************************************************************************
** Meta object code from reading C++ file 'ServerUser.h'
**
** Created: Mon 30. Jul 15:25:17 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ServerUser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ServerUser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ServerUser[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_ServerUser[] = {
    "ServerUser\0"
};

const QMetaObject ServerUser::staticMetaObject = {
    { &Connection::staticMetaObject, qt_meta_stringdata_ServerUser,
      qt_meta_data_ServerUser, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ServerUser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ServerUser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ServerUser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ServerUser))
        return static_cast<void*>(const_cast< ServerUser*>(this));
    if (!strcmp(_clname, "User"))
        return static_cast< User*>(const_cast< ServerUser*>(this));
    return Connection::qt_metacast(_clname);
}

int ServerUser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Connection::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
