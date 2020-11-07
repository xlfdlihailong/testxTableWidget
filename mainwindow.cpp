#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ptable->setHeaderText(QStringList()<<"序列号"<<"名称"<<"测试");
//    ui->ptable->setColWidth("名称",200);

    connect(this,SIGNAL(sigUpdateTable(plist<pmap<pstring,pstring>>, int)),
            ui->ptable,SLOT(slotUpdateTable(plist<pmap<pstring,pstring> >,int)));
    std::thread(&MainWindow::slotThreadSendData,this).detach();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotThreadSendData()
{
    int count=0;
    while(1)
    {
        pstring strid="序列号";
        plist<pmap<pstring,pstring>> lmdata;
        pmap<pstring,pstring> m0;
        m0.add(strid,"111");
        m0.add("name","xlfd");
        lmdata.append(m0);
        pmap<pstring,pstring> m1;
        m1.add(strid,"222");
        m1.add("name","zero");
        lmdata.append(m1);
        if(count>2)
        {
            pmap<pstring,pstring> m2;
            m2.add(strid,"333");
            m2.add("name","yyyy");
            lmdata.append(m2);
            pmap<pstring,pstring> m3;
            m3.add(strid,"444");
            m3.add("name","uuuuu");
            lmdata.append(m3);
        }
        if(count>4)
        {
            lmdata.deleteBegin();
            lmdata.deleteEnd();
        }
        //        hlog(lmdata);
        sigUpdateTable(lmdata);
        plib::sleep(2000);
        count++;
    }
}
