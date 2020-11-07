#include "qtreewidgetexx.h"
#include "ui_qtreewidgetexx.h"

QTreeWidgetExx::QTreeWidgetExx(QWidget *parent) :
    QTreeWidget(parent),
    ui(new Ui::QTreeWidgetExx)
{
    ui->setupUi(this);
//       //启用拖放相关默认关闭,不然会显示拖放图标
//    setAcceptDrops(true);
}

QTreeWidgetExx::~QTreeWidgetExx()
{
    delete ui;
}


void QTreeWidgetExx::dragEnterEvent(QDragEnterEvent *e)
{
    QTreeWidgetExx *source = qobject_cast<QTreeWidgetExx *>(e->source());
    if(source)
    {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    }
}

void QTreeWidgetExx::dragMoveEvent(QDragMoveEvent *e)
{
    QTreeWidgetExx *source = qobject_cast<QTreeWidgetExx *>(e->source());
    if(source)
    {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    }
}

void QTreeWidgetExx::dropEvent(QDropEvent *e)
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

void QTreeWidgetExx::mousePressEvent(QMouseEvent *e)
{
//    qDebug()<<("mouse press");
    //    QTreeWidgetItem* pItem=qobject_cast<QTreeWidgetItem*>(e->source());
    //    QTreeWidgetItem *pItem = qobject_cast<QTreeWidgetItem*>(this->itemAt(e->pos()));
    QTreeWidgetItem* pItem=this->itemAt(e->pos());
    //    pItem->setSelected(true);
    this->setCurrentItem(pItem);
    //    qDebug()<<pItem->text();
    //    if(e->button() == Qt::LeftButton)
    //    {
    //        _startPos = e->pos();
    //    }
    QTreeWidget::mousePressEvent(e);
    //    QTreeWidget::mouseDoubleClickEvent(e);
}

void QTreeWidgetExx::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        //        int distance = (e->pos() - _startPos).manhattanLength();
        //        if(distance >= QApplication::startDragDistance())   //当拖动距离大于一个推荐抖动距离时，表示同意已经拖动操作了
        //            performDrag();


        QTreeWidgetItem *item = currentItem();
        if(item)  //必须是非顶层item才可以移动数据
        {
            //        qDebug()<<item->text(0);
            QString strRtsp=item->data(0,Qt::UserRole+1).toString();
            //        DataTree dt=item->data(0,Qt::UserRole+1).value<DataTree>();
            //        hlog(strRtsp.toStdString().c_str());
            //            qlog(strRtsp);

            QByteArray data;
            QDataStream stream(&data,QIODevice::WriteOnly);
            stream << strRtsp;
            QMimeData *mimeData = new QMimeData;
            mimeData->setData("xlfd",data);


            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->exec();
        }
    }
    QTreeWidget::mouseMoveEvent(e);
}

//void QTreeWidgetExx::performDrag()
//{
//    QTreeWidgetItem *item = currentItem();
//    if(item)  //必须是非顶层item才可以移动数据
//    {
////        qDebug()<<item->text(0);
//        QString strRtsp=item->data(0,Qt::UserRole+1).toString();
////        DataTree dt=item->data(0,Qt::UserRole+1).value<DataTree>();
////        hlog(strRtsp.toStdString().c_str());
//        qlog(strRtsp);

//        QByteArray data;
//        QDataStream stream(&data,QIODevice::WriteOnly);
//        stream << strRtsp;
//        QMimeData *mimeData = new QMimeData;
//        mimeData->setData("xlfd",data);


//        QDrag *drag = new QDrag(this);
//        drag->setMimeData(mimeData);
//        drag->exec();
//    }
//}
