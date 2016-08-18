/****************************************************************************
** Meta object code from reading C++ file 'Log.h'
**
** Created: Thu 31. May 11:55:22 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../Log.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Log.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif
#ifdef ADDITIONAL_WINDOWS
QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LogConfig[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      20,   10,   10,   10, 0x0a,
      27,   10,   10,   10, 0x0a,
      47,   10,   42,   10, 0x0a,
      62,   60,   10,   10, 0x0a,
     111,   60,   10,   10, 0x0a,
     160,   60,   10,   10, 0x0a,
     215,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LogConfig[] = {
    "LogConfig\0\0accept()\0save()\0load(Settings)\0"
    "bool\0expert(bool)\0,\0"
    "on_qtwMessages_itemChanged(QTreeWidgetItem*,int)\0"
    "on_qtwMessages_itemClicked(QTreeWidgetItem*,int)\0"
    "on_qtwMessages_itemDoubleClicked(QTreeWidgetItem*,int)\0"
    "browseForAudioFile()\0"
};

const QMetaObject LogConfig::staticMetaObject = {
    { &ConfigWidget::staticMetaObject, qt_meta_stringdata_LogConfig,
      qt_meta_data_LogConfig, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LogConfig::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LogConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LogConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LogConfig))
        return static_cast<void*>(const_cast< LogConfig*>(this));
    if (!strcmp(_clname, "Ui::LogConfig"))
        return static_cast< Ui::LogConfig*>(const_cast< LogConfig*>(this));
    return ConfigWidget::qt_metacast(_clname);
}

int LogConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 4: on_qtwMessages_itemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: on_qtwMessages_itemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: on_qtwMessages_itemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: browseForAudioFile(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
#endif

static const uint qt_meta_data_Log[] = {

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
      34,    5,    4,    4, 0x0a,
      84,   68,    4,    4, 0x2a,
     123,  113,    4,    4, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_Log[] = {
    "Log\0\0t,console,terse,console_only\0"
    "log(MsgType,QString,QString,bool)\0"
    "t,console,terse\0log(MsgType,QString,QString)\0"
    "t,console\0log(MsgType,QString)\0"
};

const QMetaObject Log::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Log,
      qt_meta_data_Log, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Log::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Log::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Log::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Log))
        return static_cast<void*>(const_cast< Log*>(this));
    return QObject::qt_metacast(_clname);
}

int Log::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: log((*reinterpret_cast< MsgType(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 1: log((*reinterpret_cast< MsgType(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: log((*reinterpret_cast< MsgType(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_ValidDocument[] = {

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

static const char qt_meta_stringdata_ValidDocument[] = {
    "ValidDocument\0"
};

const QMetaObject ValidDocument::staticMetaObject = {
    { &QTextDocument::staticMetaObject, qt_meta_stringdata_ValidDocument,
      qt_meta_data_ValidDocument, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ValidDocument::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ValidDocument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ValidDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ValidDocument))
        return static_cast<void*>(const_cast< ValidDocument*>(this));
    return QTextDocument::qt_metacast(_clname);
}

int ValidDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextDocument::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_LogDocument[] = {

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
      13,   12,   12,   12, 0x0a,
      28,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LogDocument[] = {
    "LogDocument\0\0receivedHead()\0finished()\0"
};

const QMetaObject LogDocument::staticMetaObject = {
    { &QTextDocument::staticMetaObject, qt_meta_stringdata_LogDocument,
      qt_meta_data_LogDocument, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LogDocument::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LogDocument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LogDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LogDocument))
        return static_cast<void*>(const_cast< LogDocument*>(this));
    return QTextDocument::qt_metacast(_clname);
}

int LogDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextDocument::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: receivedHead(); break;
        case 1: finished(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
