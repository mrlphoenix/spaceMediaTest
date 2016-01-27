/****************************************************************************
** Meta object code from reading C++ file 'instagramrecentpostviewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/widgets/instagramrecentpostviewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'instagramrecentpostviewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_InstagramRecentPostViewer_t {
    QByteArrayData data[7];
    char stringdata0[78];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_InstagramRecentPostViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_InstagramRecentPostViewer_t qt_meta_stringdata_InstagramRecentPostViewer = {
    {
QT_MOC_LITERAL(0, 0, 25), // "InstagramRecentPostViewer"
QT_MOC_LITERAL(1, 26, 6), // "reload"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 13), // "replyFinished"
QT_MOC_LITERAL(4, 48, 14), // "QNetworkReply*"
QT_MOC_LITERAL(5, 63, 5), // "reply"
QT_MOC_LITERAL(6, 69, 8) // "setSound"

    },
    "InstagramRecentPostViewer\0reload\0\0"
    "replyFinished\0QNetworkReply*\0reply\0"
    "setSound"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_InstagramRecentPostViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    1,   30,    2, 0x0a /* Public */,
       6,    1,   33,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void InstagramRecentPostViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        InstagramRecentPostViewer *_t = static_cast<InstagramRecentPostViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->reload(); break;
        case 1: _t->replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 2: _t->setSound((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

const QMetaObject InstagramRecentPostViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_InstagramRecentPostViewer.data,
      qt_meta_data_InstagramRecentPostViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *InstagramRecentPostViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *InstagramRecentPostViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_InstagramRecentPostViewer.stringdata0))
        return static_cast<void*>(const_cast< InstagramRecentPostViewer*>(this));
    if (!strcmp(_clname, "SoundWidgetInfo"))
        return static_cast< SoundWidgetInfo*>(const_cast< InstagramRecentPostViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int InstagramRecentPostViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
