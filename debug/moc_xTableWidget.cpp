/****************************************************************************
** Meta object code from reading C++ file 'xtablewidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../xtablewidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'xtablewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_xTableWidget_t {
    QByteArrayData data[20];
    char stringdata0[274];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_xTableWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_xTableWidget_t qt_meta_stringdata_xTableWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "xTableWidget"
QT_MOC_LITERAL(1, 13, 14), // "sigDoubleClick"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 3), // "row"
QT_MOC_LITERAL(4, 33, 3), // "col"
QT_MOC_LITERAL(5, 37, 5), // "clear"
QT_MOC_LITERAL(6, 43, 15), // "slotUpdateTable"
QT_MOC_LITERAL(7, 59, 18), // "plist<pliststring>"
QT_MOC_LITERAL(8, 78, 6), // "lmdata"
QT_MOC_LITERAL(9, 85, 5), // "keyid"
QT_MOC_LITERAL(10, 91, 26), // "on_tableWidget_cellClicked"
QT_MOC_LITERAL(11, 118, 6), // "column"
QT_MOC_LITERAL(12, 125, 44), // "on_stuTableWidget_customConte..."
QT_MOC_LITERAL(13, 170, 3), // "pos"
QT_MOC_LITERAL(14, 174, 12), // "sortByColumn"
QT_MOC_LITERAL(15, 187, 32), // "on_tableWidget_cellDoubleClicked"
QT_MOC_LITERAL(16, 220, 10), // "slotSelect"
QT_MOC_LITERAL(17, 231, 12), // "slotUnSelect"
QT_MOC_LITERAL(18, 244, 13), // "slotSelectAll"
QT_MOC_LITERAL(19, 258, 15) // "slotUnSelectAll"

    },
    "xTableWidget\0sigDoubleClick\0\0row\0col\0"
    "clear\0slotUpdateTable\0plist<pliststring>\0"
    "lmdata\0keyid\0on_tableWidget_cellClicked\0"
    "column\0on_stuTableWidget_customContextMenuRequested\0"
    "pos\0sortByColumn\0on_tableWidget_cellDoubleClicked\0"
    "slotSelect\0slotUnSelect\0slotSelectAll\0"
    "slotUnSelectAll"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_xTableWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   74,    2, 0x08 /* Private */,
       6,    2,   75,    2, 0x08 /* Private */,
      10,    2,   80,    2, 0x08 /* Private */,
      12,    1,   85,    2, 0x08 /* Private */,
      14,    1,   88,    2, 0x08 /* Private */,
      15,    2,   91,    2, 0x08 /* Private */,
      16,    0,   96,    2, 0x08 /* Private */,
      17,    0,   97,    2, 0x08 /* Private */,
      18,    0,   98,    2, 0x08 /* Private */,
      19,    0,   99,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int,    8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,   11,
    QMetaType::Void, QMetaType::QPoint,   13,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void xTableWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        xTableWidget *_t = static_cast<xTableWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigDoubleClick((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->clear(); break;
        case 2: _t->slotUpdateTable((*reinterpret_cast< plist<pliststring>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->on_tableWidget_cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->on_stuTableWidget_customContextMenuRequested((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 5: _t->sortByColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_tableWidget_cellDoubleClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->slotSelect(); break;
        case 8: _t->slotUnSelect(); break;
        case 9: _t->slotSelectAll(); break;
        case 10: _t->slotUnSelectAll(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (xTableWidget::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&xTableWidget::sigDoubleClick)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject xTableWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_xTableWidget.data,
      qt_meta_data_xTableWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *xTableWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *xTableWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_xTableWidget.stringdata0))
        return static_cast<void*>(const_cast< xTableWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int xTableWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void xTableWidget::sigDoubleClick(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
