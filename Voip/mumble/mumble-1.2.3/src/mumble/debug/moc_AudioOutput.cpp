/****************************************************************************
** Meta object code from reading C++ file 'AudioOutput.h'
**
** Created: Thu 31. May 11:55:23 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../AudioOutput.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioOutput.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE


#ifdef USE_VOICE_REC
static const uint qt_meta_data_SoundFile[] = {

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

static const char qt_meta_stringdata_SoundFile[] = {
    "SoundFile\0"
};

const QMetaObject SoundFile::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SoundFile,
      qt_meta_data_SoundFile, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SoundFile::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SoundFile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SoundFile::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SoundFile))
        return static_cast<void*>(const_cast< SoundFile*>(this));
    return QObject::qt_metacast(_clname);
}

int SoundFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_AudioOutputSample[] = {

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
      19,   18,   18,   18, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_AudioOutputSample[] = {
    "AudioOutputSample\0\0playbackFinished()\0"
};

const QMetaObject AudioOutputSample::staticMetaObject = {
    { &AudioOutputUser::staticMetaObject, qt_meta_stringdata_AudioOutputSample,
      qt_meta_data_AudioOutputSample, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioOutputSample::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioOutputSample::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioOutputSample::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioOutputSample))
        return static_cast<void*>(const_cast< AudioOutputSample*>(this));
    return AudioOutputUser::qt_metacast(_clname);
}

int AudioOutputSample::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AudioOutputUser::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: playbackFinished(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void AudioOutputSample::playbackFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
#endif
static const uint qt_meta_data_AudioOutput[] = {

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

static const char qt_meta_stringdata_AudioOutput[] = {
    "AudioOutput\0"
};

const QMetaObject AudioOutput::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_AudioOutput,
      qt_meta_data_AudioOutput, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioOutput::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioOutput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioOutput::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioOutput))
        return static_cast<void*>(const_cast< AudioOutput*>(this));
    return QThread::qt_metacast(_clname);
}

int AudioOutput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
