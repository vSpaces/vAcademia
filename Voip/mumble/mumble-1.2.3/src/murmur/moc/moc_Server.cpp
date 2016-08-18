/****************************************************************************
** Meta object code from reading C++ file 'Server.h'
**
** Created: Mon 30. Jul 15:18:18 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Server.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Server.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LogEmitter[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_LogEmitter[] = {
    "LogEmitter\0\0msg\0newLogEntry(QString)\0"
};

const QMetaObject LogEmitter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LogEmitter,
      qt_meta_data_LogEmitter, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LogEmitter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LogEmitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LogEmitter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LogEmitter))
        return static_cast<void*>(const_cast< LogEmitter*>(this));
    return QObject::qt_metacast(_clname);
}

int LogEmitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newLogEntry((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void LogEmitter::newLogEntry(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_SslServer[] = {

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

static const char qt_meta_stringdata_SslServer[] = {
    "SslServer\0"
};

const QMetaObject SslServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_SslServer,
      qt_meta_data_SslServer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SslServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SslServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SslServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SslServer))
        return static_cast<void*>(const_cast< SslServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int SslServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Server[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      26,   22,    7,    7, 0x05,
      57,   55,    7,    7, 0x05,
      97,   55,    7,    7, 0x05,
     125,   55,    7,    7, 0x05,
     178,  175,    7,    7, 0x05,
     232,  226,    7,    7, 0x05,
     307,  175,    7,    7, 0x05,
     346,  175,    7,    7, 0x05,
     381,   55,    7,    7, 0x05,
     407,   55,    7,    7, 0x05,
     433,   55,    7,    7, 0x05,
     465,    7,    7,    7, 0x05,
     495,    7,    7,    7, 0x05,
     522,    7,    7,    7, 0x05,
     552,    7,    7,    7, 0x05,
     588,    7,    7,    7, 0x05,
     619,    7,    7,    7, 0x05,
     654,  650,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
     698,    7,    7,    7, 0x0a,
     728,    7,    7,    7, 0x0a,
     739,    7,    7,    7, 0x0a,
     748,    7,    7,    7, 0x0a,
     760,   55,    7,    7, 0x0a,
     815,    7,    7,    7, 0x0a,
     849,  842,    7,    7, 0x0a,
     886,   55,    7,    7, 0x2a,
     911,    7,    7,    7, 0x0a,
     926,   55,    7,    7, 0x0a,
     959,    7,    7,    7, 0x0a,
     972,    7,    7,    7, 0x0a,
     984,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Server[] = {
    "Server\0\0reqSync(uint)\0,id\0"
    "tcpTransmit(QByteArray,uint)\0,\0"
    "registerUserSig(int&,QMap<int,QString>)\0"
    "unregisterUserSig(int&,int)\0"
    "getRegisteredUsersSig(QString,QMap<int,QString>&)\0"
    ",,\0getRegistrationSig(int&,int,QMap<int,QString>&)\0"
    ",,,,,\0"
    "authenticateSig(int&,QString&,QList<QSslCertificate>,QString,bool,QStr"
    "ing)\0"
    "setInfoSig(int&,int,QMap<int,QString>)\0"
    "setTextureSig(int&,int,QByteArray)\0"
    "idToNameSig(QString&,int)\0"
    "nameToIdSig(int&,QString)\0"
    "idToTextureSig(QByteArray&,int)\0"
    "userStateChanged(const User*)\0"
    "userConnected(const User*)\0"
    "userDisconnected(const User*)\0"
    "channelStateChanged(const Channel*)\0"
    "channelCreated(const Channel*)\0"
    "channelRemoved(const Channel*)\0,,,\0"
    "contextAction(const User*,QString,uint,int)\0"
    "regSslError(QList<QSslError>)\0finished()\0"
    "update()\0newClient()\0"
    "connectionClosed(QAbstractSocket::SocketError,QString)\0"
    "sslError(QList<QSslError>)\0,,cCon\0"
    "message(uint,QByteArray,ServerUser*)\0"
    "message(uint,QByteArray)\0checkTimeout()\0"
    "tcpTransmitData(QByteArray,uint)\0"
    "doSync(uint)\0encrypted()\0udpActivated(int)\0"
};

const QMetaObject Server::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Server,
      qt_meta_data_Server, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Server::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Server::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Server::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Server))
        return static_cast<void*>(const_cast< Server*>(this));
    return QThread::qt_metacast(_clname);
}

int Server::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reqSync((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 1: tcpTransmit((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 2: registerUserSig((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QMap<int,QString>(*)>(_a[2]))); break;
        case 3: unregisterUserSig((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: getRegisteredUsersSig((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QMap<int,QString>(*)>(_a[2]))); break;
        case 5: getRegistrationSig((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QMap<int,QString>(*)>(_a[3]))); break;
        case 6: authenticateSig((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< const QList<QSslCertificate>(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< const QString(*)>(_a[6]))); break;
        case 7: setInfoSig((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QMap<int,QString>(*)>(_a[3]))); break;
        case 8: setTextureSig((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QByteArray(*)>(_a[3]))); break;
        case 9: idToNameSig((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: nameToIdSig((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 11: idToTextureSig((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: userStateChanged((*reinterpret_cast< const User*(*)>(_a[1]))); break;
        case 13: userConnected((*reinterpret_cast< const User*(*)>(_a[1]))); break;
        case 14: userDisconnected((*reinterpret_cast< const User*(*)>(_a[1]))); break;
        case 15: channelStateChanged((*reinterpret_cast< const Channel*(*)>(_a[1]))); break;
        case 16: channelCreated((*reinterpret_cast< const Channel*(*)>(_a[1]))); break;
        case 17: channelRemoved((*reinterpret_cast< const Channel*(*)>(_a[1]))); break;
        case 18: contextAction((*reinterpret_cast< const User*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 19: regSslError((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 20: finished(); break;
        case 21: update(); break;
        case 22: newClient(); break;
        case 23: connectionClosed((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 24: sslError((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 25: message((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2])),(*reinterpret_cast< ServerUser*(*)>(_a[3]))); break;
        case 26: message((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 27: checkTimeout(); break;
        case 28: tcpTransmitData((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 29: doSync((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 30: encrypted(); break;
        case 31: udpActivated((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 32;
    }
    return _id;
}

// SIGNAL 0
void Server::reqSync(unsigned int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Server::tcpTransmit(QByteArray _t1, unsigned int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Server::registerUserSig(int & _t1, const QMap<int,QString> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Server::unregisterUserSig(int & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Server::getRegisteredUsersSig(const QString & _t1, QMap<int,QString> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Server::getRegistrationSig(int & _t1, int _t2, QMap<int,QString> & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Server::authenticateSig(int & _t1, QString & _t2, const QList<QSslCertificate> & _t3, const QString & _t4, bool _t5, const QString & _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Server::setInfoSig(int & _t1, int _t2, const QMap<int,QString> & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Server::setTextureSig(int & _t1, int _t2, const QByteArray & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Server::idToNameSig(QString & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Server::nameToIdSig(int & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Server::idToTextureSig(QByteArray & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Server::userStateChanged(const User * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void Server::userConnected(const User * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void Server::userDisconnected(const User * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void Server::channelStateChanged(const Channel * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void Server::channelCreated(const Channel * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void Server::channelRemoved(const Channel * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void Server::contextAction(const User * _t1, const QString & _t2, unsigned int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}
QT_END_MOC_NAMESPACE
