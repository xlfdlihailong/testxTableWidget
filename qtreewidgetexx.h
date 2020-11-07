#ifndef QTREEWIDGETEXX_H
#define QTREEWIDGETEXX_H

#include <QWidget>
#include <QTreeWidget>
#include "qlib/qlib.h"
namespace Ui {
class QTreeWidgetExx;
}

class QTreeWidgetExx : public QTreeWidget
{
    Q_OBJECT

public:
    explicit QTreeWidgetExx(QWidget *parent = 0);
    ~QTreeWidgetExx();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
private:
//    QPoint _startPos;

//    void performDrag();
private:
    Ui::QTreeWidgetExx *ui;
};

#endif // QTREEWIDGETEXX_H
