#ifndef QTREEWIDGETEX_H
#define QTREEWIDGETEX_H
#include <QTreeWidget>
#include <QDragEnterEvent>
#include <QApplication>
#include <QMimeData>
#include <QDebug>
#include <QDrag>
class QTreeWidgetEx : public QTreeWidget
{
    Q_OBJECT
public:
    QTreeWidgetEx(QWidget *parent = 0);
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
private:
    QPoint _startPos;

    void performDrag();
};

#endif // QTREEWIDGETEX_H
