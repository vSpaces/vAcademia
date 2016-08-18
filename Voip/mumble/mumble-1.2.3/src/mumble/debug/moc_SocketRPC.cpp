/****************************************************************************
** Meta object code from reading C++ file 'SocketRPC.h'
**
** Created: Thu 31. May 11:55:28 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../SocketRPC.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SocketRPC.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SocketRPCClient[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      32,   16,   16,   16, 0x0a,
      70,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SocketRPCClient[] = {
    "SocketRPCClient\0\0disconnected()\0"
    "error(QLocalSocket::LocalSocketError)\0"
    "readyRead()\0"
};

const QMetaObject SocketRPCClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SocketRPCClient,
      qt_meta_data_SocketRPCClient, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SocketRPCClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SocketRPCClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SocketRPCClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SocketRPCClient))
        return static_cast<void*>(const_cast< SocketRPCClient*>(this));
    return QObject::qt_metacast(_clname);
}

int SocketRPCClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: disconnected(); break;
        case 1: error((*reinterpret_cast< QLocalSocket::LocalSocketError(*)>(_a[1]))); break;
        case 2: readyRead(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_SocketRPC[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SocketRPC[] = {
    "SocketRPC\0\0newConnection()\0"
};

const QMetaObject SocketRPC::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SocketRPC,
      qt_meta_data_SocketRPC, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SocketRPC::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SocketRPC::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SocketRPC::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SocketRPC))
        return static_cast<void*>(const_cast< SocketRPC*>(this));
    return QObject::qt_metacast(_clname);
}

int SocketRPC::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newConnection(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
