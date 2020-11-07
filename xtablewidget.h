#ifndef XTABLEWIDGET_H
#define XTABLEWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QCheckBox>
#include <QTableWidgetItem>
#include <QScrollBar>
#include "qlib/qlib.h"
namespace Ui {
class Form;
}

class xTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit xTableWidget(QWidget *parent = 0);
    ~xTableWidget();

    //首先设置列表头,第一步
    void setHeaderText(QStringList list);
    //根据列名称找到列是第几列
    int getIndexByNameCol(QString strNameCol);
    //在设置格子内容之前必须先设置行数,自带设置checkbox
    void setRowCount(int rowCount,bool isHaveCheckBox=false);
    //追加一行 ,返回当前最后一行索引，便于下边操作，下边操作就用这个返回的索引
    int  addRow();
    //获取当前鼠标所在位置的行的第j列的文本
    QString getItemTextNowByColIndex(int col);
    //获取当前鼠标所在位置的行的列名的文本
    QString getItemTextNowByColName(QString strNameCol);
    //滚到最后
    void setToBottom();
    //设置格子内容,第一次调用之前必须先设置行数
    void setItemText(int i,int j,QString str);
    //根据列名设置内容
    void setItemText(int row,QString strNameCol,QString str);

    //设置每一列的列宽，如果最后一列一直要拉，则把最后一列列宽也设置一个小值就行了
    void setColWidth(int col, int width);
    //根据列名设置列宽
    void setColWidth(QString strNameCol,int width);

    //设置widget
    void setItemWidget(int i,int j,QTableWidgetItem *pItem);
    QString getItemText(int i,int j);
    //获取当前行列蚊子
    QString getItemText();
    //根据行数和列名获取当前内容
    QString getItemText(int row,QString strNameCol);
    void removeRow(int row);
    void clear();
    //右键弹出菜单添加选项,为了外部MainWindow中使用
    void addAction(QAction *paction);
    //获取某一行的checkbox
    QCheckBox* getCheckBox(int row);
    //选择,获取最终状态,要根据选择状态选中当前行
    void setChecked(int row,bool isChecked);
    //获取所有选中行(用checkbox选中的)
    QVector<int> getAllRowsChecked();

    //用鼠标选中的所有行,一般用于后续checkbox选中
    QVector<int> getAllRowsSelected();

    //获取行数
    int getRowCount();
    int getColCount();
    //获取右键点击的行数和列数
    int getRowNow();
    int getColNow();
signals:
    void sigDoubleClick(int row,int col);

private slots:

    //支持排序追踪的界面更新,按格式来
    void slotUpdateTable(plist<pmap<pstring,pstring>> lmdata, int keyid);
    void on_tableWidget_cellClicked(int row, int column);
    //右键菜单相关,右键菜单在哪个地方用click事件全局获取
    void on_stuTableWidget_customContextMenuRequested(QPoint pos);

    void sortByColumn(int col);

    //双击改为选中该行,再点击就取消选中
    void on_tableWidget_cellDoubleClicked(int row, int column);

    void slotSelect();
    void slotUnSelect();
    void slotSelectAll();
    void slotUnSelectAll();
private:
    //根据key列找行索引
    int findIndexByKeyColumn(QString strValueToFind,QString strNameCol);
    //根据key列列数找行索引
    int findIndexByIndexColumn(QString strValueToFind,int indexCol);
    Ui::Form *ui;
    //当前选中的行数
    int nowRow;
    //当前选中的列数
    int nowCol;
    //弹出菜单
    QMenu *pop_menu;
    //右键选中当前鼠标选中的多行
    QAction* pActionSelect;
    QAction* pActionUnSelect;

    QAction* pActionSelectAll;
    QAction* pActionUnSelectAll;
    QStringList listNameCol;//所有列名
    bool isHaveCheckBox;//是否有选择框
    bool bSortUporDown;//全局是升序还是降序排列
    //    int iColSort=-1;
    //    QVector<bool> listsort;//每一列都保存一个如何排序的,记住,默认升序
};

#endif // FORM_H
