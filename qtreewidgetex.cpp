#include "qtreewidgetex.h"

QTreeWidgetEx::QTreeWidgetEx(QWidget *parent):QTreeWidget(parent)
{
    setAcceptDrops(true);
}

void QTreeWidgetEx::dragEnterEvent(QDragEnterEvent *e)
{
    QTreeWidgetEx *source = qobject_cast<QTreeWidgetEx *>(e->source());
    if(source)
    {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    }
}

void QTreeWidgetEx::dragMoveEvent(QDragMoveEvent *e)
{
    QTreeWidgetEx *source = qobject_cast<QTreeWidgetEx *>(e->source());
    if(source)
    {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    }
}

void QTreeWidgetEx::dropEvent(QDropEvent *e)
{
//    QTreeWidgetEx *source = qobject_cast<QTreeWidgetEx *>(e->source());
//    if(source)
//    {

//        QTreeWidgetItem *item = this->itemAt(e->pos()); //当前位置的item
//        if( item == nullptr)   //如果放下的位置没有item，则退出，没有这句话死机！
//            return;
//        if( -1 == this->indexOfTopLevelItem(item) && (-1 == this->indexOfTopLevelItem(currentItem())) ||
//                -1 != this->indexOfTopLevelItem(item) && (-1 != this->indexOfTopLevelItem(currentItem()))
//                )  //如果“放下位置的item是顶层item，且原来的是顶层”或者“放下的不是顶层，且原来也不是顶层”
//        {
//            qDebug()<<"放下的文本是："<<e->mimeData()->text();
//            item->setText(currentColumn(),e->mimeData()->text());
//            e->setDropAction(Qt::MoveAction);
//            e->accept();
//        }

//    }
}

void QTreeWidgetEx::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        _startPos = e->pos();
    }
//    QTreeWidget::mousePressEvent(e);
}

void QTreeWidgetEx::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        int distance = (e->pos() - _startPos).manhattanLength();
        if(distance >= QApplication::startDragDistance())   //当拖动距离大于一个推荐抖动距离时，表示同意已经拖动操作了
            performDrag();
    }
//    QTreeWidget::mouseMoveEvent(e);
}

void QTreeWidgetEx::performDrag()
{
    QTreeWidgetItem *item = currentItem();
    if(item)  //必须是非顶层item才可以移动数据
        //    if(item)
    {
        //        QMimeData *mineData = new QMimeData;
        //        if(column != 4)   //只有第四列才可以移动数据
        //            return;
        //        mineData->setText(item->text(column));
        //        qDebug()<<item->text(column);

        //        QDrag *drag = new QDrag(this);
        //        drag->setMimeData(mineData);
        //        drag->exec(Qt::MoveAction);


        QByteArray data;
        QDataStream stream(&data,QIODevice::WriteOnly);
        stream << 3.444;
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("xlfd",data);
        qDebug()<<"##########"<<data;
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec();
    }
}
