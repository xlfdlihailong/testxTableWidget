/****************************************************************************
** Meta object code from reading C++ file 'xfilebrowserlocal.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../xfilebrowserlocal.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'xfilebrowserlocal.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_xFileBrowserLocal_t {
    QByteArrayData data[15];
    char stringdata0[188];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_xFileBrowserLocal_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_xFileBrowserLocal_t qt_meta_stringdata_xFileBrowserLocal = {
    {
QT_MOC_LITERAL(0, 0, 17), // "xFileBrowserLocal"
QT_MOC_LITERAL(1, 18, 14), // "sigUpdateTable"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 29), // "plist<pmap<pstring,pstring> >"
QT_MOC_LITERAL(4, 64, 6), // "lmdata"
QT_MOC_LITERAL(5, 71, 5), // "keyid"
QT_MOC_LITERAL(6, 77, 20), // "slotDoubleClickLocal"
QT_MOC_LITERAL(7, 98, 3), // "row"
QT_MOC_LITERAL(8, 102, 3), // "col"
QT_MOC_LITERAL(9, 106, 18), // "slotThreadSendData"
QT_MOC_LITERAL(10, 125, 19), // "slotTreeDoubleClick"
QT_MOC_LITERAL(11, 145, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(12, 162, 4), // "item"
QT_MOC_LITERAL(13, 167, 6), // "column"
QT_MOC_LITERAL(14, 174, 13) // "slotTreeClick"

    },
    "xFileBrowserLocal\0sigUpdateTable\0\0"
    "plist<pmap<pstring,pstring> >\0lmdata\0"
    "keyid\0slotDoubleClickLocal\0row\0col\0"
    "slotThreadSendData\0slotTreeDoubleClick\0"
    "QTreeWidgetItem*\0item\0column\0slotTreeClick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_xFileBrowserLocal[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       1,    1,   49,    2, 0x26 /* Public | MethodCloned */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   52,    2, 0x08 /* Private */,
       9,    0,   57,    2, 0x08 /* Private */,
      10,    2,   58,    2, 0x08 /* Private */,
      14,    2,   63,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    7,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11, QMetaType::Int,   12,   13,
    QMetaType::Void, 0x80000000 | 11, QMetaType::Int,   12,   13,

       0        // eod
};

void xFileBrowserLocal::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        xFileBrowserLocal *_t = static_cast<xFileBrowserLocal *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigUpdateTable((*reinterpret_cast< plist<pmap<pstring,pstring> >(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->sigUpdateTable((*reinterpret_cast< plist<pmap<pstring,pstring> >(*)>(_a[1]))); break;
        case 2: _t->slotDoubleClickLocal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->slotThreadSendData(); break;
        case 4: _t->slotTreeDoubleClick((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->slotTreeClick((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (xFileBrowserLocal::*_t)(plist<pmap<pstring,pstring>> , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&xFileBrowserLocal::sigUpdateTable)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject xFileBrowserLocal::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_xFileBrowserLocal.data,
      qt_meta_data_xFileBrowserLocal,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *xFileBrowserLocal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *xFileBrowserLocal::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_xFileBrowserLocal.stringdata0))
        return static_cast<void*>(const_cast< xFileBrowserLocal*>(this));
    return QWidget::qt_metacast(_clname);
}

int xFileBrowserLocal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void xFileBrowserLocal::sigUpdateTable(plist<pmap<pstring,pstring>> _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
