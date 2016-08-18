/****************************************************************************
** Meta object code from reading C++ file 'ClientUser.h'
**
** Created: Thu 31. May 11:55:27 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../ClientUser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ClientUser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ClientUser[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      29,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      50,   47,   11,   11, 0x0a,
      87,   82,   11,   11, 0x0a,
     106,  101,   11,   11, 0x0a,
     129,  120,   11,   11, 0x0a,
     147,   82,   11,   11, 0x0a,
     166,   82,   11,   11, 0x0a,
     184,  101,   11,   11, 0x0a,
     211,  202,   11,   11, 0x0a,
     246,  236,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClientUser[] = {
    "ClientUser\0\0talkingChanged()\0"
    "muteDeafChanged()\0ts\0"
    "setTalking(Settings::TalkState)\0mute\0"
    "setMute(bool)\0deaf\0setDeaf(bool)\0"
    "suppress\0setSuppress(bool)\0"
    "setLocalMute(bool)\0setSelfMute(bool)\0"
    "setSelfDeaf(bool)\0priority\0"
    "setPrioritySpeaker(bool)\0recording\0"
    "setRecording(bool)\0"
};

const QMetaObject ClientUser::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ClientUser,
      qt_meta_data_ClientUser, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClientUser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClientUser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClientUser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClientUser))
        return static_cast<void*>(const_cast< ClientUser*>(this));
    if (!strcmp(_clname, "User"))
        return static_cast< User*>(const_cast< ClientUser*>(this));
    return QObject::qt_metacast(_clname);
}

int ClientUser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: talkingChanged(); break;
        case 1: muteDeafChanged(); break;
        case 2: setTalking((*reinterpret_cast< Settings::TalkState(*)>(_a[1]))); break;
        case 3: setMute((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: setDeaf((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: setSuppress((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: setLocalMute((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: setSelfMute((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: setSelfDeaf((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: setPrioritySpeaker((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: setRecording((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void ClientUser::talkingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ClientUser::muteDeafChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
