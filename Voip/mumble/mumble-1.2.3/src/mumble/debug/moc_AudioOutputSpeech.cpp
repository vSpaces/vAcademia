#include "mumble_pch.hpp" 
#include "../AudioOutputSpeech.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioOutputSpeech.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AudioOutputSpeech[] = {

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

static const char qt_meta_stringdata_AudioOutputSpeech[] = {
    "AudioOutputSpeech\0"
};

const QMetaObject AudioOutputSpeech::staticMetaObject = {
    { &AudioOutputUser::staticMetaObject, qt_meta_stringdata_AudioOutputSpeech,
      qt_meta_data_AudioOutputSpeech, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioOutputSpeech::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioOutputSpeech::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioOutputSpeech::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioOutputSpeech))
        return static_cast<void*>(const_cast< AudioOutputSpeech*>(this));
    return AudioOutputUser::qt_metacast(_clname);
}

int AudioOutputSpeech::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AudioOutputUser::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE