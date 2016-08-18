/****************************************************************************
** Meta object code from reading C++ file 'DirectSound.h'
**
** Created: Thu 31. May 11:55:29 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../AudioInFromStream/AudioInStream.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioInStream.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AudioOutStream[] = {

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

static const char qt_meta_stringdata_AudioOutStream[] = {
    "AudioOutStream\0"
};

const QMetaObject AudioOutStream::staticMetaObject = {
    { &AudioOutput::staticMetaObject, qt_meta_stringdata_AudioOutStream,
      qt_meta_data_AudioOutStream, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioOutStream::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioOutStream::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioOutStream::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioOutStream))
        return static_cast<void*>(const_cast< AudioOutStream*>(this));
    return AudioOutput::qt_metacast(_clname);
}

int AudioOutStream::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AudioOutput::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}

static const uint qt_meta_data_AudioInStream[] = {

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

static const char qt_meta_stringdata_AudioInStream[] = {
    "AudioInStream\0"
};

const QMetaObject AudioInStream::staticMetaObject = {
    { &AudioInput::staticMetaObject, qt_meta_stringdata_AudioInStream,
      qt_meta_data_AudioInStream, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioInStream::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioInStream::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioInStream::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioInStream))
        return static_cast<void*>(const_cast< AudioInStream*>(this));
    return AudioInput::qt_metacast(_clname);
}

int AudioInStream::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AudioInput::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
