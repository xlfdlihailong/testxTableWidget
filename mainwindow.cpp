#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ptable->setHeaderText(QStringList()<<"序列号"<<"name"<<"不知道");

    connect(this,SIGNAL(sigUpdateTable(plist<pmap<pstring,pstring>>, pstring)),
            ui->ptable,SLOT(slotUpdateTable(plist<pmap<pstring,pstring> >,pstring)));
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
        m0[strid]="111";
        m0["name"]="xlfd";
        lmdata.append(m0);
        pmap<pstring,pstring> m1;
        m1[strid]="222";
        m1["name"]="zero";
        lmdata.append(m1);
        if(count>2)
        {
            pmap<pstring,pstring> m2;
            m2[strid]="333";
            m2["name"]="yyyy";
            lmdata.append(m2);
            pmap<pstring,pstring> m3;
            m3[strid]="444";
            m3["name"]="uuuuu";
            lmdata.append(m3);
        }
        if(count>4)
        {
            lmdata.deleteBegin();
            lmdata.deleteEnd();
        }
        //        hlog(lmdata);
        sigUpdateTable(lmdata,strid);
        plib::sleep(1000);
        count++;
    }
}
