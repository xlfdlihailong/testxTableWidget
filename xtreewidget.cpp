#include "xtreewidget.h"
#include "ui_xtreewidget.h"

xTreeWidget::xTreeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xTreeWidget)
{
    ui->setupUi(this);

    this->isCheckable=false;
    //启用拖放相关默认关闭,不然会显示拖放图标
//    ui->treeWidget->setDragEnabled(true);
//    //设置显示将要被放置的位置
//    ui->treeWidget->setDropIndicatorShown(true);
//        //设置拖放模式为移动项目，否则为复制项目,这里是复制
//    ui->treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
//    ui->treeWidget->setDefaultDropAction(Qt::MoveAction);



    //设置列数,一般1就行,多个是为xml这种的可以
    ui->treeWidget->setColumnCount(1);
    //设置选中项
    ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    //自适应
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget->setHeaderLabel(QString::fromLocal8Bit("树状控件")); //设置头的标题
    ui->treeWidget->setHeaderHidden(true);//隐藏上述表头



//    QTreeWidgetItem*proot=this->addRoot(("根节点"));
//    QTreeWidgetItem* pjoint2=this->newJoint(("吊车架"));
//    QTreeWidgetItem* pjoint3=this->newJoint(("三级节点"));
//    QTreeWidgetItem* pjoint4=this->newJoint(("钢柱"));
//    QTreeWidgetItem* pjoint5=this->newJoint(("xlfdQProcess"));

//    proot->insertChild(0,pjoint2);
//    pjoint2->insertChild(0,pjoint3);
//    proot->addChild(pjoint4);
//    proot->insertChild(2,pjoint5);

////    QTreeWidgetItem* proot2=this->addRoot("根节点2");
//    //节点始终保持展开

//    ui->treeWidget->expandAll(); //结点全部展开


    //检测点击事件
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(itemClick(QTreeWidgetItem*,int)));
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(itemDoubleClick(QTreeWidgetItem*,int)));
    //右键菜单
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget,SIGNAL(customContextMenuRequested(const QPoint&)), this,SLOT(popMenu(const QPoint&)));
}
void xTreeWidget::itemClick(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent = item->parent();   //获取父节点
    if(NULL==parent) //注意：最顶端项是没有父节点的
        return;
    int row = parent->indexOfChild(item); //item在父项中的节点行号(从0开始)
//    qDebug()<<row;
    pstring info=plib::toGBK((pstring()<<"点击了树节点的第"<<row<<"行").c_str());
    hlog(info);
    sigClick(item,column);

    //获取数据从其他地方
    //    qDebug()<<item->checkState(0);
//    QString strRtsp=(item->data(0,Qt::UserRole+1).toString());
//    hlog(strRtsp.toStdString());
//    DataTree dt=item->data(0,Qt::UserRole+1).value<DataTree>();
    //发出限号给显示组件
//    sigClick(strRtsp);
}

void xTreeWidget::itemDoubleClick(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent = item->parent();   //获取父节点
    if(NULL==parent) //注意：最顶端项是没有父节点的
        return;


    int row = parent->indexOfChild(item); //item在父项中的节点行号(从0开始)
//        QString info=QString::fromLocal8Bit("双击了Tree控件的第")+qlib::toString(row)+QString::fromLocal8Bit("行");
        qDebug()<<row;
//    string strinfo=string("双击了Tree控件的第")+plib::toString(row)+"行";
//    hlog(plib::toChinese(strinfo.c_str()));
    //获取双击的索引地址和播放地址

    QString strRtsp=(item->data(0,Qt::UserRole+1).toString());
//    hlog(strRtsp.toStdString());
//    DataTree dt=item->data(0,Qt::UserRole+1).value<DataTree>();
    //发出限号给显示组件
    sigShowMaxIndex(strRtsp);



    //双击选中/取消选择
    if(this->isCheckable)
    {
        qDebug()<<item->checkState(0);
        if(item->checkState(0)==Qt::Checked)
        {
            item->setCheckState(0,Qt::Unchecked);
        }
        else {
            item->setCheckState(0,Qt::Checked);
        }
    }

    hlog(item->text(column));
    sigDoubleClick(item,column);
}


xTreeWidget::~xTreeWidget()
{
    delete ui;
}

void xTreeWidget::expandItem(QTreeWidgetItem *pitem)
{
    ui->treeWidget->expandItem(pitem);
}

void xTreeWidget::expandAll()
{
    ui->treeWidget->expandAll();
}

QTreeWidgetItem* xTreeWidget::newJoint(QString name)
{
    //    //一般默认使用colum=1,多个的以后再试
    QTreeWidgetItem *pAdd=new QTreeWidgetItem(QStringList()<<name);

    pAdd->setIcon(0,QIcon(":/icons/摄像头.png"));//设置图标

    //    //就算可复选,也不包括根节点
    //    if(pItemParent!=NULL&&this->isCheckable)
    //    {
    //        pAdd->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    //        pAdd->setCheckState(0, Qt::Checked);
    //    }



    return pAdd;
}

QTreeWidgetItem* xTreeWidget::addRoot(QString name)
{
    QTreeWidgetItem *pAdd=new QTreeWidgetItem(ui->treeWidget,QStringList()<<name);
    pAdd->setIcon(0,QIcon(":/icons/层级.png"));//设置图标
    return pAdd;
}

QVector<QTreeWidgetItem *> xTreeWidget::getAllItems()
{
    QVector<QTreeWidgetItem *>res;
    //遍历treeWidget
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it) {
        //do something like
        QString str=(*it)->text(0);
        //        qDebug()<<str;
        res.push_back(*it);
        ++it;
    }
    return res;
}

int xTreeWidget::getIndexOfCurrentLevel()
{
    QTreeWidgetItem* pItemCurrent=ui->treeWidget->currentItem();
    int index=pItemCurrent->parent()->indexOfChild(pItemCurrent);
    return index;
}

QVector<QTreeWidgetItem *> xTreeWidget::getChilds(QTreeWidgetItem *pItemCurrent)
{
    QVector<QTreeWidgetItem *>res;
    for(int i=0;i<pItemCurrent->childCount();i++)
    {
        //        qDebug()<<"ri: "<<pItemCurrent->child(i)->text(0)<<" end";
        res.append(pItemCurrent->child(i));
    }
    return res;
}

QVector<QTreeWidgetItem *> xTreeWidget::getChildsNow()
{
    return this->getChilds(ui->treeWidget->currentItem());
}

QVector<QTreeWidgetItem *> xTreeWidget::getSiblings(QTreeWidgetItem *pItemCurrent)
{
    return this->getChilds(pItemCurrent->parent());
}

QVector<QTreeWidgetItem *> xTreeWidget::getSiblingsNow()
{
    return this->getSiblings(ui->treeWidget->currentItem());
}

QTreeWidgetItem *xTreeWidget::root()
{
    return this->getAllItems()[0];
}



void xTreeWidget::popMenu(const QPoint &)
{
    QTreeWidgetItem* curItem=ui->treeWidget->currentItem();  //**获取当前被点击的节点
    if(curItem == NULL || curItem->parent() == NULL)
        return;           //右键的位置在空白位置右击或者点击的是顶层item

    qDebug()<<curItem->text(0);


    //创建一个action
    QAction actionDeleteItem(QString::fromLocal8Bit("&删除节点"),this);
    connect(&actionDeleteItem, SIGNAL(triggered()), this, SLOT(slotDeleteItem()));

    QAction actionAddItem(QString::fromLocal8Bit("&在当前节点前添加节点"),this);
    connect(&actionAddItem, SIGNAL(triggered()), this, SLOT(slotAddItem()));

    QAction actionAddItemAfter(QString::fromLocal8Bit("&在当前节点后添加节点"),this);
    connect(&actionAddItemAfter, SIGNAL(triggered()), this, SLOT(slotAddItemAfter()));

    QAction actionItemUpdate(QString::fromLocal8Bit("&修改节点"),this);
    connect(&actionItemUpdate, SIGNAL(triggered()), this, SLOT(slotItemUpdate()));

//    //创建一个菜单栏
//    QMenu menu(ui->treeWidget);

//    menu.addAction(&actionAddItem);
//    menu.addAction(&actionAddItemAfter);
//    menu.addAction(&actionItemUpdate);
//    menu.addAction(&actionDeleteItem);

//    menu.exec(QCursor::pos());  //在当前鼠标位置显示
}




void xTreeWidget::slotDeleteItem()
{
    QTreeWidgetItem* curItem=ui->treeWidget->currentItem();
    curItem->parent()->removeChild(ui->treeWidget->currentItem());
}

void xTreeWidget::slotAddItem()
{
    QString strInput;
    if(inputbox(strInput))
    {
        //获取所有兄弟节点
        QVector<QTreeWidgetItem*> listNowLevel=this->getSiblingsNow();
        //        qlog(listNowLevel.size());
        //text不能重复
        bool isRepeat=false;
        for(int i=0;i<listNowLevel.size();i++)
        {
            QTreeWidgetItem* pitem=listNowLevel[i];
            if(pitem->text(0)==strInput)
            {
                isRepeat=true;
                break;
            }
        }

        if(isRepeat)
        {
            messageBox(QString::fromLocal8Bit(
                           "当前层中已有该名称的节点,无法添加 [")+strInput+"]");
            return;
        }

        //获取当前级中的index
        int index=this->getIndexOfCurrentLevel();
        //        qlog(index);
        QTreeWidgetItem* pItemCurrent=ui->treeWidget->currentItem();
        QTreeWidgetItem* pNewJoint=this->newJoint(strInput);
        pItemCurrent->parent()->insertChild(index,pNewJoint);
    }
}

void xTreeWidget::slotAddItemAfter()
{
    QString strInput;
    if(inputbox(strInput))
    {
        //获取所有兄弟节点
        QVector<QTreeWidgetItem*> listNowLevel=this->getSiblingsNow();
        //        qlog(listNowLevel.size());
        //text不能重复
        bool isRepeat=false;
        for(int i=0;i<listNowLevel.size();i++)
        {
            QTreeWidgetItem* pitem=listNowLevel[i];
            if(pitem->text(0)==strInput)
            {
                isRepeat=true;
                break;
            }
        }

        if(isRepeat)
        {
            messageBox("["+strInput+QString::fromLocal8Bit("]在该层中已存在,添加失败"));
            return;
        }

        //获取当前级中的index
        int index=this->getIndexOfCurrentLevel();
        //        qlog(index);
        QTreeWidgetItem* pItemCurrent=ui->treeWidget->currentItem();
        QTreeWidgetItem* pNewJoint=this->newJoint(strInput);
        pItemCurrent->parent()->insertChild(index+1,pNewJoint);
    }
}

void xTreeWidget::slotItemUpdate()
{
    QString strInput;
    if(inputbox(strInput))
    {
        //获取所有兄弟节点
        QVector<QTreeWidgetItem*> listNowLevel=this->getSiblingsNow();
        //        qlog(listNowLevel.size());
        //text不能重复
        bool isRepeat=false;
        for(int i=0;i<listNowLevel.size();i++)
        {
            QTreeWidgetItem* pitem=listNowLevel[i];
            if(pitem->text(0)==strInput)
            {
                isRepeat=true;
                break;
            }
        }

        if(isRepeat)
        {
            messageBox("["+strInput+QString::fromLocal8Bit("]在该层中已存在,修改失败"));
            return;
        }
        ui->treeWidget->currentItem()->setText(0,strInput);
    }
}
void xTreeWidget::messageBox(QString strDefault)
{
    QMessageBox::information(NULL,QString::fromLocal8Bit("提示信息"),strDefault);
}
bool xTreeWidget::inputbox(QString &strInput, QString strdefault)
{
    QInputDialog inputDialog;
    inputDialog.setWindowTitle(QString::fromLocal8Bit("输入框"));
    inputDialog.setLabelText(strdefault);
    inputDialog.setOkButtonText(QString::fromLocal8Bit("确定"));
    inputDialog.setCancelButtonText(QString::fromLocal8Bit("取消"));
    inputDialog.setTextValue(QString::fromLocal8Bit("请输入内容"));
    if (inputDialog.exec())
    {
        strInput=inputDialog.textValue();
        return true;
    }
    return false;

}
