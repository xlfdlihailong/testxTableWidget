#include "xTableWidget.h"
#include "ui_xTableWidget.h"

xTableWidget::xTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    //    //封装设置表头字体为楷体14号最好看
    QHeaderView* pheader=ui->tableWidget->horizontalHeader();
    //    pheader->setStyleSheet("QHeaderView::section{background-color:rgb(40,143,218);font:14pt 'DejaVu Sans Mono';color: white;};");
    //        pheader->setStyleSheet("QHeaderView::section{font:14pt 'DejaVu Sans Mono';};");
    pheader->setStyleSheet("QHeaderView::section{font:bold 14pt '楷体';};");

    //header的个数,列个数
    //    qDebug()<<ui->tableWidget->horizontalHeader()->count();
    bSortUporDown=false;
    //设置不能编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置选中一行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);

    //为了自动滚动设置的//设置平滑一点
    ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    //隔行不一样颜色
    ui->tableWidget->setAlternatingRowColors(true);
    //    //设置当前选中的行数和列数初始值为-1
    this->nowRow=-1;
    this->nowCol=-1;
    //右键菜单
    ui->tableWidget->setContextMenuPolicy (Qt::CustomContextMenu);
    //弹出菜单
    pop_menu = new QMenu(ui->tableWidget);

    //右键菜单相关,右键菜单在哪个地方用click事件全局获取
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,
            SLOT(on_stuTableWidget_customContextMenuRequested(QPoint)));

    QHeaderView *headerGoods = ui->tableWidget->horizontalHeader();
    //SortIndicator为水平标题栏文字旁边的三角指示器

    //        headerGoods->setSortIndicator(0, Qt::AscendingOrder);
    headerGoods->setSortIndicatorShown(true);
    //        headerGoods->setClickable(true);

    connect(ui->tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int )),this, SLOT(sortByColumn(int)));
}

xTableWidget::~xTableWidget()
{

    delete pop_menu;
    delete ui;
}

void xTableWidget::setHeaderText(QStringList list)
{
    //    listsort.resize(list.size());
    //    for(int i=0;i<listsort.size();i++)
    //    {
    //        listsort[i]=true;
    //    }
    this->listNameCol=list;
    //首先要设置列个数,才能设置表头内容
    ui->tableWidget->setColumnCount(list.size());
    //设置表头内容
    ui->tableWidget->setHorizontalHeaderLabels(list);

    //    //以下这两句,既自适应了表格,又能指定某列适应文字
    //x先自适应宽度
    //        ui->tableWidget->horizontalHeader()->setSectionResizeMode (QHeaderView::Stretch);
    //    //     //然后设置要根据内容使用宽度的列
    //        ui->tableWidget->horizontalHeader()->setSectionResizeMode (2, QHeaderView::ResizeToContents);
    //    ui->tableWidget->setColumnWidth(0,50);
    //    ui->tableWidget->horizontalHeader()->setSectionResizeMode (5, QHeaderView::ResizeToContents);

    //除最后一列外默认200宽度
    ui->tableWidget->setColumnWidth(0,200);
    //    ui->tableWidget->setColumnWidth(1,230);
    //    ui->tableWidget->setColumnWidth(2,100);
    //    for(int i=0;i<this->getColCount()-1;i++)
    //    {
    //        ui->tableWidget->setColumnWidth(i,300);
    //    }
    //设置列宽可拖动
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    //最后一列拉满
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    //设置行高可拖动,一般不用
    //     ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void xTableWidget::updateSort()
{
    //    if(iColSort>0)
    //        sortByColumn(iColSort);

}

void xTableWidget::setRowCount(int rowCount, bool isHaveCheckBox)
{
    //    //先把原来的释放--这里不能释放，要不然原来的就没了
    //    for(int i=0;i<this->getRowCount();i++)
    //    {
    //        for(int j=0;j<this->getColCount();j++)
    //        {
    //            QTableWidgetItem* pitem=ui->tableWidget->item(i,j);
    //            delete pitem;
    //            pitem=NULL;
    //        }
    //    }
    //比如追加下边的
    ui->tableWidget->setRowCount(rowCount);
    this->isHaveCheckBox=isHaveCheckBox;


    if(!isHaveCheckBox)
        return;
    //以下是复选框
    //0是选择框
    for(int i=0;i<rowCount;i++)
    {
        //不居中,用QTableWidgetItem
        //                QTableWidgetItem* pCheck=new QTableWidgetItem();
        //                pCheck->setCheckState(Qt::Unchecked);

        //                ui->tableWidget->setItem(i,0,pCheck);


        //为了居中,要用布局
        QWidget *widget = new QWidget(this);
        QHBoxLayout *layout = new QHBoxLayout(this);
        QCheckBox *box = new QCheckBox(this);
        layout->addWidget(box);
        layout->setMargin(0);
        layout->setAlignment(box,Qt::AlignCenter);
        //        layout->setAlignment(box,Qt::AlignLeft);
        widget->setLayout(layout);
        ui->tableWidget->setCellWidget(i,0,widget);

        //        //获取CheckBox的指针的方法
        //        QWidget *pwidget = (QWidget *)ui->tableWidget->cellWidget(i, 0);
        //        QCheckBox *ckb = (QCheckBox *)pwidget->children().at(1);
        //        ckb->setChecked(true);

    }
    //默认按钮
    //反选
    pActionSelect=new QAction(QString::fromLocal8Bit("选择"),this);
    pop_menu->addAction(pActionSelect);
    connect(pActionSelect,SIGNAL(triggered(bool)),this,SLOT(slotSelect()));

    //全选
    pActionUnSelect=new QAction(QString::fromLocal8Bit("取消选择"),this);
    pop_menu->addAction(pActionUnSelect);
    connect(pActionUnSelect,SIGNAL(triggered(bool)),this,SLOT(slotUnSelect()));


    //反选
    pActionSelectAll=new QAction(QString::fromLocal8Bit("选择所有行"),this);
    pop_menu->addAction(pActionSelectAll);
    connect(pActionSelectAll,SIGNAL(triggered(bool)),this,SLOT(slotSelectAll()));

    //全选
    pActionUnSelectAll=new QAction(QString::fromLocal8Bit("取消选择所有"),this);
    pop_menu->addAction(pActionUnSelectAll);
    connect(pActionUnSelectAll,SIGNAL(triggered(bool)),this,SLOT(slotUnSelectAll()));
    //该功能只能选中一行
    //            ui->tableWidget->selectRow(0);
    //            ui->tableWidget->selectRow(2);
    //            ui->tableWidget->selectAll();

}
//追加一行 ,返回当前最后一行索引，便于下边操作
int xTableWidget::addRow()
{
    int rowold=this->getRowCount();
    this->setRowCount(rowold+1);
    return rowold;
}

int xTableWidget::findIndexByKeyColumn(QString strValueToFind, QString strNameCol)
{
    int ifind=-1;
    for(int i=0;i<this->getRowCount();i++)
    {
        QString stri=this->getItemText(i,strNameCol);
        if(stri==strValueToFind)
        {
            ifind=i;
            break;
        }
    }
    return ifind;
}

int xTableWidget::findIndexByIndexColumn(QString strValueToFind, int indexCol)
{
    int ifind=-1;
    for(int i=0;i<this->getRowCount();i++)
    {
        QString stri=this->getItemText(i,indexCol);
        if(stri==strValueToFind)
        {
            ifind=i;
            break;
        }
    }
    return ifind;
}

void xTableWidget::slotUpdateTable(plist<pmap<pstring, pstring> > lmdata, int keyid)
{
    hlog(lmdata);
    //先获取原来的行数,如果是0,则说明是第一次,直接赋值就行
    int ilenold=this->getRowCount();
    if(ilenold==0)
    {
        this->setRowCount(lmdata.size());
        for(int i=0;i<lmdata.size();i++)
        {
            pmap<pstring,pstring> mapi=lmdata[i];
            for(int j=0;j<mapi.size();j++)
            {
                this->setItemText(i,j,mapi.getValue(j).c_str());
            }
        }
    }
    else//更新,要根据keyid去找
    {
        plist<pstring> listKeyId;
        for(int i=0;i<lmdata.size();i++)
        {
            //先更新
            pmap<pstring,pstring> mapi=lmdata[i];

            pstring strNameKeyColumn=mapi.getValue(keyid);
            //先找到索引
            int ifind=this->findIndexByIndexColumn(strNameKeyColumn.c_str(),keyid);
            //            hlog(ifind);
            //如果没找到的话,就增加一行
            if(ifind<0)
            {
                //                hlog("######## nofind #########");
                int indexAdd=this->addRow();
                for(int j=0;j<mapi.size();j++)
                {
                    this->setItemText(indexAdd,j,mapi.getValue(j).c_str());
                }
            }
            else//如果找到的话,就更新
            {
                //加入到更新列表中
                listKeyId.append(strNameKeyColumn);
                for(int j=0;j<mapi.size();j++)
                {
                    this->setItemText(ifind,j,mapi.getValue(j).c_str());
                }
            }
        }
        //检查数据个数是否小于原来行数,如果小于,则要删数据
        if(ilenold>lmdata.size())
        {
            //            hlog("##### < #####");

            //比对数据keyid和列表keyid找出少的
            int iCountReduce=ilenold-listKeyId.size();
            hlog(iCountReduce);
            hlog(listKeyId);
            //要找出少的列表,都删掉,一条一条的删
            for(int i=0;i<iCountReduce;i++)
            {
                //遍历界面keyid
                for(int j=0;j<this->getRowCount();j++)
                {
                    pstring strKeyIdUI=this->getItemText(j,keyid).toStdString();
                    //如果列表中不包含,说明这一行没了,要删除
                    if(!listKeyId.contains(strKeyIdUI))
                    {
                        this->removeRow(j);
                        break;
                    }
                }
            }
        }
    }
}



void xTableWidget::setToBottom()
{
    //有问题，加了会出现timer的问题QBasicTimer can only be used with threads started with QThread
    //    ui->tableWidget->scrollToBottom();
    int t=ui->tableWidget->verticalScrollBar()->maximum();
    ui->tableWidget->verticalScrollBar()->setValue(t);//设置滚动条滚动到最大值
}

void xTableWidget::setItemText(int i, int j, QString str)
{
    //    //先释放
    //    QTableWidgetItem* pItemOld= ui->tableWidget->item(i,j);
    //    delete pItemOld;
    //    //每次新建一个有内存泄露的问题
    //    QTableWidgetItem* pItemStr=new QTableWidgetItem();
    //    pItemStr->setText(str);
    //    //居中
    //    pItemStr->setTextAlignment(Qt::AlignCenter);

    //    //    pItemStr->setTextAlignment(Qt::AlignLeft);
    //    //先判断一下和原来的是不是一样,如果一样就不设置了,这个很重要
    //    //因为正常情况下如果第一次设置没问题,第二次设置全一样的会卡
    //    QString strOrigin=this->getItemText(i,j);
    //    if(str!=strOrigin)
    //    {
    //        ui->tableWidget->setItem(i,j,pItemStr);
    //        //刷新
    //        ui->tableWidget->update();
    //    }


    //每次新建一个有内存泄露的问题,判断没有就新建，有就直接赋值
    QTableWidgetItem* pItemStr=NULL;
    pItemStr=ui->tableWidget->item(i,j);

    //如果已经存在,就改一下就行
    if(pItemStr!=NULL)
    {
        //        qDebug()<<"$$$$$$$$$$$$$$$$$$$$";
        //        qDebug()<<"####"<<(pItemStr!=NULL)<<"##### "<<i<<","<<j<<" "<<str<<" ######";
        pItemStr->setText(str);
        //        //刷新
        //        ui->tableWidget->update();
    }
    else
    {
        //        qDebug()<<"###############";
        //不存在，新建
        pItemStr=new QTableWidgetItem(str);
        //居中
        pItemStr->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,j,pItemStr);
    }

    //有问题，加了会出现timer的问题QBasicTimer can only be used with threads started with QThread
    //显示最后一行 //如果滚动的话，一直显示最后一行
    //    ui->tableWidget->scrollToBottom();

    //    /////////////////
    //    int t=ui->tableWidget->verticalScrollBar()->maximum();
    //    ui->tableWidget->verticalScrollBar()->setValue(t);//设置滚动条滚动到最大值
//    this->setToBottom();
}

void xTableWidget::setItemText(int row, QString strNameCol, QString str)
{
    int iIndexFind=-1;
    for(int i=0;i<this->listNameCol.size();i++)
    {
        if(strNameCol==this->listNameCol[i])
        {
            iIndexFind=i;
            break;
        }
    }
    if(iIndexFind<0)
    {
        //        qDebug()<<("nofind");
        return;
    }
    else
    {
        this->setItemText(row,iIndexFind,str);
    }

}

void xTableWidget::setItemWidget(int i, int j, QTableWidgetItem *pItem)
{

    ui->tableWidget->setItem(i,j,pItem);
}

QString xTableWidget::getItemText(int i, int j)
{
    //没有这个条目的时候要置位""
    QTableWidgetItem* pItem=ui->tableWidget->item(i,j);
    if(pItem==NULL)
        return "";
    else
        return pItem->text();
}

QString xTableWidget::getItemText()
{
    return getItemText(this->nowRow,this->nowCol);
}

QString xTableWidget::getItemText(int row, QString strNameCol)
{
    int iIndexFind=-1;
    for(int i=0;i<this->listNameCol.size();i++)
    {
        if(strNameCol==this->listNameCol[i])
        {
            iIndexFind=i;
            break;
        }
    }
    if(iIndexFind<0)
    {
        qDebug()<<("nofind");
        return "";
    }
    else
    {
        return this->getItemText(row,iIndexFind);
    }
}

void xTableWidget::removeRow(int row)
{
    ui->tableWidget->removeRow(row);
    this->setRowCount(this->getRowCount()-1);
}

void xTableWidget::clear()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}

void xTableWidget::addAction(QAction *paction)
{
    pop_menu->addAction(paction);
}

QCheckBox *xTableWidget::getCheckBox(int row)
{
    QWidget *pwidget = (QWidget *)ui->tableWidget->cellWidget(row, 0);
    QCheckBox *ckb = (QCheckBox *)pwidget->children().at(1);
    //    ckb->setChecked(true);
    return ckb;
}

void xTableWidget::setChecked(int row, bool isChecked)
{
    QCheckBox* pch=this->getCheckBox(row);
    pch->setChecked(isChecked);
    //    //如果是选中状态,择需要选中该行
    //    if(pch->isChecked())
    //        ui->tableWidget->selectRow(row);
}

QVector<int> xTableWidget::getAllRowsChecked()
{
    QVector<int>  listrow;
    for(int i=0;i<this->getRowCount();i++)
    {
        QCheckBox* pch=this->getCheckBox(i);
        if(pch->isChecked())
            listrow.push_back(i);
    }
    return listrow;
}

QVector<int> xTableWidget::getAllRowsSelected()
{
    QVector<int> listrow;
    //获取选中的行
    QList<QTableWidgetSelectionRange> ranges=ui->tableWidget->selectedRanges();

    int count=ranges.count();

    for(int i=0;i<count;i++)
    {
        int topRow=ranges.at(i).topRow();
        int bottomRow=ranges.at(i).bottomRow();
        for(int j=topRow;j<=bottomRow;j++)
        {
            //            qDebug()<<"selectRow"<<j;
            listrow.push_back(j);
        }
    }
    return listrow;
}

void xTableWidget::setColWidth(int col,int width)
{
    ui->tableWidget->setColumnWidth(col,width);
}


int xTableWidget::getRowCount()
{
    return ui->tableWidget->rowCount();
}

int xTableWidget::getColCount()
{
    return ui->tableWidget->columnCount();
}

int xTableWidget::getRowNow()
{
    return this->nowRow;
}

int xTableWidget::getColNow()
{
    return this->nowCol;
}


//这是左键点的
void xTableWidget::on_tableWidget_cellClicked(int row, int column)
{
    this->nowRow=row;
    this->nowCol=column;

    //    qDebug()<<"Row: "<<this->nowRow<<",Col: "<<this->nowCol;
    if(this->isHaveCheckBox)
    {
        qDebug()<<"check status: "<<getCheckBox(this->nowRow)->isChecked();
        qDebug()<<"all checked: "<<this->getAllRowsChecked();
    }

}
//右键菜单相关,右键菜单在哪个地方用click事件全局获取
void xTableWidget::on_stuTableWidget_customContextMenuRequested(QPoint pos)
{
    //获取点击的tablewidget中行和列
    QModelIndex index = ui->tableWidget->indexAt (pos);
    this->nowRow = index.row ();//获得QTableWidget列表点击的行数
    this->nowCol=index.column();
    //    qDebug()<<this->nowRow;
    //    qDebug()<<this->nowCol;
    //最后再显示,不然出问题
    pop_menu->exec(QCursor::pos());
}

void xTableWidget::sortByColumn(int col)
{    qDebug()<<"you click header col: "<<col;

     //     //如果是升序,则改为降,反之亦然
     //     if(listsort[col])
     //     {
     //         ui->tableWidget->sortItems(col, Qt::DescendingOrder);
     //         listsort[col]=false;
     //     }
     //     else
     //     {
     //         ui->tableWidget->sortItems(col, Qt::AscendingOrder);
     //         listsort[col]=true;
     //     }
     qDebug()<<"you click header col: "<<col;
      //      this->iColSort=col;
      if(bSortUporDown)
      {
          ui->tableWidget->sortItems(col, Qt::AscendingOrder);
          bSortUporDown=false;
      }
      else
      {
          ui->tableWidget->sortItems(col, Qt::DescendingOrder);
          bSortUporDown=true;
      }
}



//双击改为选中该行,再点击就取消选中
void xTableWidget::on_tableWidget_cellDoubleClicked(int row, int column)
{
    this->nowRow=row;
    this->nowCol=column;
    qDebug()<<"doubleClick Row: "<<this->nowRow<<",Col: "<<this->nowCol;
    //不再弹出右键菜单,改为选中或者不选中

    if(this->isHaveCheckBox)
    {
        //    pop_menu->exec(QCursor::pos());
        QCheckBox* pch=this->getCheckBox(this->nowRow);
        if(pch->isChecked())
            this->setChecked(this->nowRow,false);
        //        pch->setChecked(false);
        else
            this->setChecked(this->nowRow,true);
        //        pch->setChecked(true);

    }
    emit sigDoubleClick(row,column);
}

void xTableWidget::slotSelect()
{
    qDebug()<<"select";
    //获取选中的行
    QVector<int> listRowSeleted=this->getAllRowsSelected();
    //将鼠标选中的行选中
    qDebug()<<listRowSeleted;

    for(int i=0;i<listRowSeleted.size();i++)
    {
        int row=listRowSeleted[i];
        this->setChecked(row,true);
    }
}

void xTableWidget::slotUnSelect()
{
    qDebug()<<"select";
    //获取选中的行
    QVector<int> listRowSeleted=this->getAllRowsSelected();
    //将鼠标选中的行选中
    qDebug()<<listRowSeleted;

    for(int i=0;i<listRowSeleted.size();i++)
    {
        int row=listRowSeleted[i];
        this->setChecked(row,false);
    }
}

void xTableWidget::slotSelectAll()
{
    for(int i=0;i<this->getRowCount();i++)
    {
        this->setChecked(i,true);
    }
}

void xTableWidget::slotUnSelectAll()
{
    for(int i=0;i<this->getRowCount();i++)
    {
        this->setChecked(i,false);
    }
}
