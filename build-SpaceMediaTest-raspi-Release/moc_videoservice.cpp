/****************************************************************************
** Meta object code from reading C++ file 'videoservice.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/utils/videoservice.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videoservice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_VideoService_t {
    QByteArrayData data[19];
    char stringdata0[394];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VideoService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VideoService_t qt_meta_stringdata_VideoService = {
    {
QT_MOC_LITERAL(0, 0, 12), // "VideoService"
QT_MOC_LITERAL(1, 13, 10), // "initResult"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 17), // "InitRequestResult"
QT_MOC_LITERAL(4, 43, 6), // "result"
QT_MOC_LITERAL(5, 50, 18), // "enablePlayerResult"
QT_MOC_LITERAL(6, 69, 20), // "assignPlaylistResult"
QT_MOC_LITERAL(7, 90, 17), // "getPlaylistResult"
QT_MOC_LITERAL(8, 108, 12), // "PlayerConfig"
QT_MOC_LITERAL(9, 121, 24), // "initVideoRequestFinished"
QT_MOC_LITERAL(10, 146, 14), // "QNetworkReply*"
QT_MOC_LITERAL(11, 161, 5), // "reply"
QT_MOC_LITERAL(12, 167, 27), // "enablePlayerRequestFinished"
QT_MOC_LITERAL(13, 195, 37), // "assignPlaylistToPlayerRequest..."
QT_MOC_LITERAL(14, 233, 26), // "getPlaylistRequestFinished"
QT_MOC_LITERAL(15, 260, 28), // "initVideoRequestFinishedSlot"
QT_MOC_LITERAL(16, 289, 31), // "enablePlayerRequestFinishedSlot"
QT_MOC_LITERAL(17, 321, 41), // "assignPlaylistToPlayerRequest..."
QT_MOC_LITERAL(18, 363, 30) // "getPlaylistRequestFinishedSlot"

    },
    "VideoService\0initResult\0\0InitRequestResult\0"
    "result\0enablePlayerResult\0"
    "assignPlaylistResult\0getPlaylistResult\0"
    "PlayerConfig\0initVideoRequestFinished\0"
    "QNetworkReply*\0reply\0enablePlayerRequestFinished\0"
    "assignPlaylistToPlayerRequestFinished\0"
    "getPlaylistRequestFinished\0"
    "initVideoRequestFinishedSlot\0"
    "enablePlayerRequestFinishedSlot\0"
    "assignPlaylistToPlayerRequestFinishedSlot\0"
    "getPlaylistRequestFinishedSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VideoService[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       5,    1,   77,    2, 0x06 /* Public */,
       6,    1,   80,    2, 0x06 /* Public */,
       7,    1,   83,    2, 0x06 /* Public */,
       9,    1,   86,    2, 0x06 /* Public */,
      12,    1,   89,    2, 0x06 /* Public */,
      13,    1,   92,    2, 0x06 /* Public */,
      14,    1,   95,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    1,   98,    2, 0x0a /* Public */,
      16,    1,  101,    2, 0x0a /* Public */,
      17,    1,  104,    2, 0x0a /* Public */,
      18,    1,  107,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 8,    4,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

void VideoService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VideoService *_t = static_cast<VideoService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initResult((*reinterpret_cast< InitRequestResult(*)>(_a[1]))); break;
        case 1: _t->enablePlayerResult((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->assignPlaylistResult((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->getPlaylistResult((*reinterpret_cast< PlayerConfig(*)>(_a[1]))); break;
        case 4: _t->initVideoRequestFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 5: _t->enablePlayerRequestFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 6: _t->assignPlaylistToPlayerRequestFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 7: _t->getPlaylistRequestFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 8: _t->initVideoRequestFinishedSlot((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 9: _t->enablePlayerRequestFinishedSlot((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 10: _t->assignPlaylistToPlayerRequestFinishedSlot((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 11: _t->getPlaylistRequestFinishedSlot((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
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
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 11:
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
            typedef void (VideoService::*_t)(InitRequestResult );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoService::initResult)) {
                *result = 0;
            }
        }
        {
            typedef void (VideoService::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoService::enablePlayerResult)) {
                *result = 1;
            }
        }
        {
            typedef void (VideoService::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoService::assignPlaylistResult)) {
                *result = 2;
            }
        }
        {
            typedef void (VideoService::*_t)(PlayerConfig );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoService::getPlaylistResult)) {
                *result = 3;
            }
        }
        {
            typedef void (VideoService::*_t)(QNetworkReply * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoService::initVideoRequestFinished)) {
                *result = 4;
            }
        }
        {
            typedef void (VideoService::*_t)(QNetworkReply * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoService::enablePlayerRequestFinished)) {
                *result = 5;
            }
        }
        {
            typedef void (VideoService::*_t)(QNetworkReply * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoService::assignPlaylistToPlayerRequestFinished)) {
                *result = 6;
            }
        }
        {
            typedef void (VideoService::*_t)(QNetworkReply * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VideoService::getPlaylistRequestFinished)) {
                *result = 7;
            }
        }
    }
}

const QMetaObject VideoService::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VideoService.data,
      qt_meta_data_VideoService,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *VideoService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VideoService::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_VideoService.stringdata0))
        return static_cast<void*>(const_cast< VideoService*>(this));
    return QObject::qt_metacast(_clname);
}

int VideoService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void VideoService::initResult(InitRequestResult _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VideoService::enablePlayerResult(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VideoService::assignPlaylistResult(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VideoService::getPlaylistResult(PlayerConfig _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void VideoService::initVideoRequestFinished(QNetworkReply * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void VideoService::enablePlayerRequestFinished(QNetworkReply * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void VideoService::assignPlaylistToPlayerRequestFinished(QNetworkReply * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void VideoService::getPlaylistRequestFinished(QNetworkReply * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
