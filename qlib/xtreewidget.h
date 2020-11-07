#ifndef XTREEWIDGET_H
#define XTREEWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include "qlib/qlib.h"
namespace Ui {
class xTreeWidget;
}

class xTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit xTreeWidget(QWidget *parent = nullptr);
    ~xTreeWidget();

    void expandAll();
    QTreeWidgetItem* newJoint(QString name);
    QTreeWidgetItem* addRoot(QString name);
    QVector<QTreeWidgetItem*> getAllItems();
    //获取当前节点在本层树中的索引
    int getIndexOfCurrentLevel();
    //获取节点的所有子节点
    QVector<QTreeWidgetItem*> getChilds(QTreeWidgetItem* pItemCurrent);
    //获取当前节点的所有子节点
    QVector<QTreeWidgetItem*> getChildsNow();
    //获取节点的兄弟节点(包括自己)也就是当前level的所有节点
    QVector<QTreeWidgetItem*> getSiblings(QTreeWidgetItem* pItemCurrent);
    //获取当前节点的兄弟节点(包括自己)也就是当前level的所有节点
    QVector<QTreeWidgetItem*> getSiblingsNow();

    //获取root节点
    QTreeWidgetItem* root();

    //弹出信息提示框
    void messageBox(QString strDefault=QString::fromLocal8Bit("这是个信息提示框"));
    //弹出输入框,使用方法如下
    //QString strinput;
    //if(qlib::inputbox(strinput))
    //{
    //    qlog(strinput);
    //}
    //支持中文确定和取消了
    bool inputbox(QString &strInput, QString strdefault=QString::fromLocal8Bit("您要输入的文本"));
signals:
    void sigTransSystemNameAndStationName(pstring strSystem,pstring strStation);
    void sigShowMaxIndex(QString url);
    void sigClick(QString url);
private slots:
    void popMenu(const QPoint &);
    void itemClick(QTreeWidgetItem *item, int column);
    void itemDoubleClick(QTreeWidgetItem *item, int column);
    void slotDeleteItem();
    void slotAddItem();
    void slotAddItemAfter();
    void slotItemUpdate();
private:
    QPoint startPos;
    Ui::xTreeWidget *ui;
    bool isCheckable;//是否可选
};

#endif // XTREEWIDGET_H
