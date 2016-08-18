/****************************************************************************
** Meta object code from reading C++ file 'ServerHandler.h'
**
** Created: Thu 31. May 11:55:23 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../ServerHandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ServerHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ServerHandler[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   15,   14,   14, 0x05,
      74,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      88,   86,   14,   14, 0x09,
     113,   14,   14,   14, 0x09,
     141,   14,   14,   14, 0x09,
     176,   14,   14,   14, 0x09,
     235,   86,   14,   14, 0x09,
     296,   14,   14,   14, 0x09,
     327,   14,   14,   14, 0x09,
     338,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ServerHandler[] = {
    "ServerHandler\0\0,reason\0"
    "disconnected(QAbstractSocket::SocketError,QString)\0"
    "connected()\0,\0message(uint,QByteArray)\0"
    "serverConnectionConnected()\0"
    "serverConnectionTimeoutOnConnect()\0"
    "serverConnectionStateChanged(QAbstractSocket::SocketState)\0"
    "serverConnectionClosed(QAbstractSocket::SocketError,QString)\0"
    "setSslErrors(QList<QSslError>)\0"
    "udpReady()\0sendPing()\0"
};

const QMetaObject ServerHandler::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ServerHandler,
      qt_meta_data_ServerHandler, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ServerHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ServerHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ServerHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ServerHandler))
        return static_cast<void*>(const_cast< ServerHandler*>(this));
    return QThread::qt_metacast(_clname);
}

int ServerHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: disconnected((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: connected(); break;
        case 2: message((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 3: serverConnectionConnected(); break;
        case 4: serverConnectionTimeoutOnConnect(); break;
        case 5: serverConnectionStateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 6: serverConnectionClosed((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: setSslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 8: udpReady(); break;
        case 9: sendPing(); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ServerHandler::disconnected(QAbstractSocket::SocketError _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ServerHandler::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
