/****************************************************************************
** Meta object code from reading C++ file 'randomplaylist.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/utils/randomplaylist.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'randomplaylist.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RandomPlaylist_t {
    QByteArrayData data[3];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RandomPlaylist_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RandomPlaylist_t qt_meta_stringdata_RandomPlaylist = {
    {
QT_MOC_LITERAL(0, 0, 14), // "RandomPlaylist"
QT_MOC_LITERAL(1, 15, 4), // "next"
QT_MOC_LITERAL(2, 20, 0) // ""

    },
    "RandomPlaylist\0next\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RandomPlaylist[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::QString,

       0        // eod
};

void RandomPlaylist::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RandomPlaylist *_t = static_cast<RandomPlaylist *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { QString _r = _t->next();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject RandomPlaylist::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RandomPlaylist.data,
      qt_meta_data_RandomPlaylist,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RandomPlaylist::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RandomPlaylist::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RandomPlaylist.stringdata0))
        return static_cast<void*>(const_cast< RandomPlaylist*>(this));
    return QObject::qt_metacast(_clname);
}

int RandomPlaylist::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
