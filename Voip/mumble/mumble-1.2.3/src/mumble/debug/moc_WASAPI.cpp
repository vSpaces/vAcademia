/****************************************************************************
** Meta object code from reading C++ file 'WASAPI.h'
**
** Created: Thu 31. May 11:55:29 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../WASAPI.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WASAPI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WASAPISystem[] = {

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

static const char qt_meta_stringdata_WASAPISystem[] = {
    "WASAPISystem\0"
};

const QMetaObject WASAPISystem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WASAPISystem,
      qt_meta_data_WASAPISystem, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WASAPISystem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WASAPISystem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WASAPISystem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WASAPISystem))
        return static_cast<void*>(const_cast< WASAPISystem*>(this));
    return QObject::qt_metacast(_clname);
}

int WASAPISystem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_WASAPIInput[] = {

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

static const char qt_meta_stringdata_WASAPIInput[] = {
    "WASAPIInput\0"
};

const QMetaObject WASAPIInput::staticMetaObject = {
    { &AudioInput::staticMetaObject, qt_meta_stringdata_WASAPIInput,
      qt_meta_data_WASAPIInput, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WASAPIInput::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WASAPIInput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WASAPIInput::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WASAPIInput))
        return static_cast<void*>(const_cast< WASAPIInput*>(this));
    return AudioInput::qt_metacast(_clname);
}

int WASAPIInput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AudioInput::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_WASAPIOutput[] = {

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

static const char qt_meta_stringdata_WASAPIOutput[] = {
    "WASAPIOutput\0"
};

const QMetaObject WASAPIOutput::staticMetaObject = {
    { &AudioOutput::staticMetaObject, qt_meta_stringdata_WASAPIOutput,
      qt_meta_data_WASAPIOutput, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WASAPIOutput::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WASAPIOutput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WASAPIOutput::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WASAPIOutput))
        return static_cast<void*>(const_cast< WASAPIOutput*>(this));
    return AudioOutput::qt_metacast(_clname);
}

int WASAPIOutput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AudioOutput::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
