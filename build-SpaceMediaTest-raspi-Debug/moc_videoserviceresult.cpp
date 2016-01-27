/****************************************************************************
** Meta object code from reading C++ file 'videoserviceresult.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/utils/videoserviceresult.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videoserviceresult.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_VideoServiceResultProcessor_t {
    QByteArrayData data[15];
    char stringdata0[253];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VideoServiceResultProcessor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VideoServiceResultProcessor_t qt_meta_stringdata_VideoServiceResultProcessor = {
    {
QT_MOC_LITERAL(0, 0, 27), // "VideoServiceResultProcessor"
QT_MOC_LITERAL(1, 28, 10), // "initResult"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 17), // "InitRequestResult"
QT_MOC_LITERAL(4, 58, 6), // "result"
QT_MOC_LITERAL(5, 65, 18), // "enablePlayerResult"
QT_MOC_LITERAL(6, 84, 20), // "assignPlaylistResult"
QT_MOC_LITERAL(7, 105, 17), // "getPlaylistResult"
QT_MOC_LITERAL(8, 123, 12), // "PlayerConfig"
QT_MOC_LITERAL(9, 136, 22), // "initRequestResultReply"
QT_MOC_LITERAL(10, 159, 14), // "QNetworkReply*"
QT_MOC_LITERAL(11, 174, 5), // "reply"
QT_MOC_LITERAL(12, 180, 23), // "enablePlayerResultReply"
QT_MOC_LITERAL(13, 204, 25), // "assignPlaylistResultReply"
QT_MOC_LITERAL(14, 230, 22) // "getPlaylistResultReply"

    },
    "VideoServiceResultProcessor\0initResult\0"
    "\0InitRequestResult\0result\0enablePlayerResult\0"
    "assignPlaylistResult\0getPlaylistResult\0"
    "PlayerConfig\0initRequestResultReply\0"
    "QNetworkReply*\0reply\0enablePlayerResultReply\0"
    "assignPlaylistResultReply\0"
    "getPlaylistResultReply"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VideoServiceResultProcessor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,
       6,    1,   60,    2, 0x06 /* Public */,
       7,    1,   63,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   66,    2, 0x0a /* Public */,
      12,    1,   69,    2, 0x0a /* Public */,
      13,    1,   72,    2, 0x0a /* Public */,
      14,    1,   75,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 8,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

void VideoServiceResultProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VideoServiceResultProcessor *_t = static_cast<VideoServiceResultProcessor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initResult((*reinterpret_cast< InitRequestResult(*)>(_a[1]))); break;
        case 1: _t->enablePlayerResult((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->assignPlaylistResult((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->getPlaylistResult((*reinterpret_cast< PlayerConfig(*)>(_a[1]))); break;
        case 4: _t->initRequestResultReply((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 5: _t->enablePlayerResultReply((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 6: _t->assignPlaylistResultReply((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 7: _t->getPlaylistResultReply((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (VideoServiceResultProcessor::*_t)(InitRequestResult );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoServiceResultProcessor::initResult)) {
                *result = 0;
            }
        }
        {
            typedef void (VideoServiceResultProcessor::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoServiceResultProcessor::enablePlayerResult)) {
                *result = 1;
            }
        }
        {
            typedef void (VideoServiceResultProcessor::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoServiceResultProcessor::assignPlaylistResult)) {
                *result = 2;
            }
        }
        {
            typedef void (VideoServiceResultProcessor::*_t)(PlayerConfig );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoServiceResultProcessor::getPlaylistResult)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject VideoServiceResultProcessor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VideoServiceResultProcessor.data,
      qt_meta_data_VideoServiceResultProcessor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *VideoServiceResultProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VideoServiceResultProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_VideoServiceResultProcessor.stringdata0))
        return static_cast<void*>(const_cast< VideoServiceResultProcessor*>(this));
    return QObject::qt_metacast(_clname);
}

int VideoServiceResultProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void VideoServiceResultProcessor::initResult(InitRequestResult _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VideoServiceResultProcessor::enablePlayerResult(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VideoServiceResultProcessor::assignPlaylistResult(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VideoServiceResultProcessor::getPlaylistResult(PlayerConfig _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
