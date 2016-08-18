/****************************************************************************
** Meta object code from reading C++ file 'Tray.h'
**
** Created: Mon 30. Jul 15:21:14 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Tray.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Tray.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Tray[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x0a,
      59,    5,    5,    5, 0x0a,
      79,    5,    5,    5, 0x0a,
     102,    5,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Tray[] = {
    "Tray\0\0on_Tray_activated(QSystemTrayIcon::ActivationReason)\0"
    "on_Quit_triggered()\0on_ShowLog_triggered()\0"
    "addLogMessage(QString)\0"
};

const QMetaObject Tray::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Tray,
      qt_meta_data_Tray, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Tray::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Tray::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Tray::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Tray))
        return static_cast<void*>(const_cast< Tray*>(this));
    return QObject::qt_metacast(_clname);
}

int Tray::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_Tray_activated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 1: on_Quit_triggered(); break;
        case 2: on_ShowLog_triggered(); break;
        case 3: addLogMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
