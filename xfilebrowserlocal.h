#ifndef XFILEBROWSERLOCAL_H
#define XFILEBROWSERLOCAL_H

#include <QWidget>
#include "qlib/qlib.h"
#include <QTreeWidgetItem>
namespace Ui {
class xFileBrowserLocal;
}
/*
 * 自动刷新文件夹内数据
 */
class xFileBrowserLocal : public QWidget
{
    Q_OBJECT

public:
    explicit xFileBrowserLocal(QWidget *parent = 0);
    ~xFileBrowserLocal();
    pstring strpwd;
    //统一名称,包括map,表头
    pstring strtype="类型";
    pstring strname="名称";
    pstring strsize="大小";
    plist<pmap<pstring,pstring>> getDirInfo(pstring strDirDes);
    plist<pmap<pstring, pstring> > showDirInfo(pstring strdir);
signals:
    void sigUpdateTable(plist<pmap<pstring,pstring>> lmdata, int keyid=0);
private slots:
    void slotDoubleClickLocal(int row,int col);
    void slotThreadSendData();
    void slotTreeDoubleClick(QTreeWidgetItem *item, int column);
    void slotTreeClick(QTreeWidgetItem* item,int column);
private:
    Ui::xFileBrowserLocal *ui;

};

#endif // XFILEBROWSERLOCAL_H
