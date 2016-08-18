/****************************************************************************
** Meta object code from reading C++ file 'AudioStats.h'
**
** Created: Thu 31. May 11:55:22 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/
#include "mumble_pch.hpp" 
#include "../AudioStats.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioStats.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AudioBar[] = {

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

static const char qt_meta_stringdata_AudioBar[] = {
    "AudioBar\0"
};

const QMetaObject AudioBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AudioBar,
      qt_meta_data_AudioBar, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioBar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioBar))
        return static_cast<void*>(const_cast< AudioBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int AudioBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
#ifdef ADDITIONAL_WINDOWS
static const uint qt_meta_data_AudioEchoWidget[] = {

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

static const char qt_meta_stringdata_AudioEchoWidget[] = {
    "AudioEchoWidget\0"
};

const QMetaObject AudioEchoWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_AudioEchoWidget,
      qt_meta_data_AudioEchoWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioEchoWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioEchoWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioEchoWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioEchoWidget))
        return static_cast<void*>(const_cast< AudioEchoWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int AudioEchoWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_AudioNoiseWidget[] = {

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
      24,   18,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AudioNoiseWidget[] = {
    "AudioNoiseWidget\0\0event\0"
    "paintEvent(QPaintEvent*)\0"
};

const QMetaObject AudioNoiseWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AudioNoiseWidget,
      qt_meta_data_AudioNoiseWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioNoiseWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioNoiseWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioNoiseWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioNoiseWidget))
        return static_cast<void*>(const_cast< AudioNoiseWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int AudioNoiseWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: paintEvent((*reinterpret_cast< QPaintEvent*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_AudioStats[] = {

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
      12,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AudioStats[] = {
    "AudioStats\0\0on_Tick_timeout()\0"
};

const QMetaObject AudioStats::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AudioStats,
      qt_meta_data_AudioStats, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioStats::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioStats::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioStats::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioStats))
        return static_cast<void*>(const_cast< AudioStats*>(this));
    if (!strcmp(_clname, "Ui::AudioStats"))
        return static_cast< Ui::AudioStats*>(const_cast< AudioStats*>(this));
    return QDialog::qt_metacast(_clname);
}

int AudioStats::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_Tick_timeout(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
#endif