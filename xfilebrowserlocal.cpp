#include "xfilebrowserlocal.h"
#include "ui_xfilebrowserlocal.h"

xFileBrowserLocal::xFileBrowserLocal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xFileBrowserLocal)
{
    ui->setupUi(this);
    strpwd=plib::pwd();
    ui->plineEditpwd->setText(strpwd.c_str());
    ui->ptab->setHeaderText(QStringList()<<strname.c_str()<<strsize.c_str()<<strtype.c_str());
    connect(ui->ptab,SIGNAL(sigDoubleClick(int,int)),this,SLOT(slotDoubleClickLocal(int,int)));
    connect(this,SIGNAL(sigUpdateTable(plist<pmap<pstring,pstring> >,int)),
            ui->ptab,SLOT(slotUpdateTable(plist<pmap<pstring,pstring> >,int)));
    ui->ptab->setColWidth(strname.c_str(),300);

    //类似windows文件管理器,把左边tree控件设置为显示我的电脑以及各个盘并展开到下一级
    ui->ptree->addRoot("我的电脑");
    plist<pmap<pstring,pstring>> lmall=(this->getDirInfo("我的电脑"));

    for(int i=0;i<lmall.size();i++)
    {
        pstring name=lmall[i][strname];
        hlog(name);
        auto joint=ui->ptree->newJoint(name.c_str());
        ui->ptree->root()->addChild(joint);
        //对每个盘再增加下一级的任务
        auto infoDir=this->getDirInfo(name);
        //        hlog(infoDir);
        //    this->strpwd=strClick;
        //        this->showDirInfo(this->strpwd);
        //加入到tree并展开
        for(pmap<pstring,pstring> mapi:infoDir)
        {
            hlog(mapi[strname]);
            QTreeWidgetItem* pchild=ui->ptree->newJoint(mapi[strname].c_str());
            joint->addChild(pchild);
        }
        //        ui->ptree->expandAll();
    }
    ui->ptree->expandItem(ui->ptree->root());
    connect(ui->ptree,SIGNAL(sigDoubleClick(QTreeWidgetItem*,int)),this,
            SLOT(slotTreeDoubleClick(QTreeWidgetItem*,int)));
    connect(ui->ptree,SIGNAL(sigClick(QTreeWidgetItem*,int)),this,
            SLOT(slotTreeClick(QTreeWidgetItem*,int)));

    std::thread(&xFileBrowserLocal::slotThreadSendData,this).detach();
}

xFileBrowserLocal::~xFileBrowserLocal()
{
    delete ui;
}

plist<pmap<pstring, pstring> > xFileBrowserLocal::getDirInfo(pstring strDirDes)
{
    plist<pmap<pstring,pstring> > lmall;
    plist<pstring> listallFile;
    //    QString mFolderPath="C:/Users/Administrator/Desktop";
    QString mFolderPath=qlib::toString(strDirDes);
    //    qlog(mFolderPath);
    //    qlog(mFolderPath=="我的电脑");
    if(mFolderPath!="我的电脑")
    {
        QStringList mImgNames;
        // 获取所有文件名
        QDir dir(mFolderPath);
        mFolderPath = dir.fromNativeSeparators(mFolderPath);//  "\\"转为"/"
        //        hlog(dir.exists());
        if (!dir.exists())
            mImgNames = QStringList("");
        dir.setFilter(QDir::Files);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(QString("*.*;*.*").split(";"));
        mImgNames = dir.entryList();
        //        qlog(mImgNames);
        for (int i = 0; i < mImgNames.size(); ++i)
        {
            //        qDebug() << "entryList: " << i << "-" << mFolderPath + "/" +mImgNames[i];
            QString name=mImgNames[i];
            pstring pname=name.toStdString();

            //去掉隐藏文件以及.lnk的目录
            if(!name.contains(".lnk")&&name[0]!='.')
            {
                //                            hlog(pname,strFullPath,plib::getFileSize(strFullPath));
                //                                hlog(pname);
                listallFile.append(pname);
            }
        }

        //目录结果,先加上..上一级
        plist<pstring> listallDir;
        listallDir.append("..");
        // 获取所有文件夹名
        QDir dir2(mFolderPath);
        mFolderPath = dir2.fromNativeSeparators(mFolderPath);//  "\\"转为"/"
        if (!dir2.exists()) mImgNames = QStringList("");
        dir2.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        //    dir2.setFilter(QDir::Dirs|QDir::NoDot );
        dir2.setSorting(QDir::Name);
        mImgNames = dir2.entryList();
        for (int i = 0; i < mImgNames.size(); ++i)
        {
            //        qDebug() << "entryList: " << i << "-" << mFolderPath + "/" +mImgNames[i];
            QString namedir=mImgNames[i];
            //        hlog(namedir.toStdString());
            //去掉隐藏目录以及.lnk的目录
            if(!namedir.contains(".lnk")&&namedir[0]!='.')
            {
                //            hlog(namedir);
                listallDir.append(namedir.toStdString());
            }
        }

        //    this->strpwdlocal=path;
        //显示当前路径指定路径
        ui->plineEditpwd->setText(qlib::toString(this->strpwd));

        //        hlog(listallFile);
        for(int i=0;i<listallDir.size();i++)
        {
            pmap<pstring,pstring> mapinfo;
            mapinfo.add(strname,listallDir[i]);
            mapinfo.add(strtype,"目录");
            //要有这一句,不然会崩,因为现在pmap都比较严格
            mapinfo.add(strsize,"");
            //        hlog(mapinfo);
            lmall.append(mapinfo);
        }
        //从第三个开始,因为第一个是pwd,第二个是.
        for(int i=0;i<listallFile.size();i++)
        {
            pstring pname=listallFile[i];

            pstring strFullPath=strDirDes+"/"+pname;
            //            hlog(strFullPath);
            longlong llsize=plib::getFileSize(strFullPath);
            pmap<pstring,pstring> mapinfo;
            mapinfo.add(strname,listallFile[i]);
            mapinfo.add(strtype,"文件");
            //整数M
            mapinfo.add(strsize,plib::toString((int)1.0*llsize/1024/1024)+" MB");
            //        hlog(mapinfo);
            lmall.append(mapinfo);
        }
    }
    else//如果是我的电脑,则把盘符加进去都
    {
        auto listdevice=qlib::getAllDeviceValid();
        for(int i=0;i<listdevice.size();i++)
        {
            QString devicei=listdevice[i];
            pmap<pstring,pstring> mapinfo;
            mapinfo.add(strname,devicei.toStdString());
            mapinfo.add(strtype,"目录");
            //要有这一句,不然会崩,因为现在pmap都比较严格
            mapinfo.add(strsize,"");
            //        hlog(mapinfo);
            lmall.append(mapinfo);
        }
    }
    //    hlog(lmall);
    return lmall;
}

plist<pmap<pstring,pstring> >  xFileBrowserLocal::showDirInfo(pstring strdir)
{
    plist<pmap<pstring,pstring> > lmall=this->getDirInfo(strdir);
    //显示当前路径下的所有文件以及文件夹
    sigUpdateTable(lmall);
    return lmall;
    //选中左边tree对应的盘符



    //    ui->ptab->setRowCount(lmall.size());
    //    //    hlog(lmall);
    //    for(int i=0;i<lmall.size();i++)
    //    {
    //        pmap<pstring,pstring> mapi=lmall[i];
    //        //        hlog(mapi);
    //        ui->ptab->setItemText(i,strname.c_str(),mapi[strname].c_str());
    //        ui->ptab->setItemText(i,strsize.c_str(),mapi[strsize].c_str());
    //        ui->ptab->setItemText(i,strtype.c_str(),mapi[strtype].c_str());
    //    }

}

void xFileBrowserLocal::slotDoubleClickLocal(int row, int col)
{
    hlog(row,col);
    //不管点的哪里,获取第一列
    QString strName=ui->ptab->getItemText(row,0);
    hlog(strName);
    //    hlog(plib::isIncludeChinese(strName.toStdString()));
    //    if(plib::isIncludeChinese(strName.toStdString()))
    //    {
    //        qlib::messageBox("暂不支持上传中文目录,请改为英文名后再试,目前仅支持上传中文文件");
    //        return;
    //    }
    //如果点击了..
    if(strName=="..")
    {
        hlog("点击了上一级");
        hlog(strpwd);
        //        //要到上一级去
        //        if(strpwdlocal=="/")
        //        {
        //            qlib::messageBox("已经是根目录,无法定位到上一级");
        //            return;
        //        }
        pliststring listpath=strpwd.split("/");
        hlog(listpath);
        listpath.deleteEnd();
        hlog(listpath);
        pstring pathres=listpath.join("/");
        hlog(pathres);
        //如果到了根目录,比如C:则需要加上/不然有问题,windows
        if(pathres.size()==2)
        {
            pathres+="/";
            hlog(pathres);
        }
        //如果到了再上一级,就是我的电脑了
        if(pathres.size()==0)
        {
            pathres="我的电脑";
        }
        //如果==我的电脑长度加2,说明是我的电脑点进去的--我的电脑/C:/
        //要把前边的我的电脑去掉

        qlog(pathres,pathres.size());

        this->strpwd=pathres;
        ui->plineEditpwd->setText(qlib::toString(this->strpwd));
        this->showDirInfo(this->strpwd);
        return;
    }
    //获取类型
    QString strType=ui->ptab->getItemText(row,2);
    //    hlog(strType.toStdString());
    //如果是目录或者是目录快捷方式,排除了链接
    if(strType=="目录"||strType=="目录快捷方式")
    {

        //获取新的路径,一般都是不带/的
        hlog(strpwd);
        pstring pathres;
        if(strpwd=="/")
            pathres=strpwd+strName.toStdString();
        else
            pathres=strpwd+"/"+strName.toStdString();
        hlog(pathres);
        //要去掉我的电脑
        if(pathres.contain("我的电脑")&&pathres.contain(":/"))
        {
            pathres=pathres.substr(13,pathres.size());
            hlog(pathres);
        }
        this->strpwd=pathres;

        ui->plineEditpwd->setText(qlib::toString(this->strpwd));
        this->showDirInfo(this->strpwd);
    }
    //目前只有文件支持上传中文,其他都不支持
    if(strType=="文件")
    {
        pstring name=strName.toStdString();
        hlog(name);
        pstring strPathFull=this->strpwd+"/"+name;
        hlog(strPathFull);
        //        hlog(this->strpwdremote);
        //        //传输文件
        //        ptrans->uploadThread(strPathFull,this->strpwdremote,std::bind(&MainWindow::showstr,this,placeholders::_1));
    }
}

void xFileBrowserLocal::slotThreadSendData()
{
    while(1)
    {
        this->showDirInfo(this->strpwd);
        plib::sleep(1000);
    }
}
//双击的话,要赋值下一级
void xFileBrowserLocal::slotTreeDoubleClick(QTreeWidgetItem *item, int column)
{
    pstring strClick=(item->text(column)).toStdString();
    hlog(strClick);
    pstring strPathFull=strClick;

    if(strClick.substr(1,2)!=":/")
    {
        //这个string只是当前节点的name,要获取所有节点加起来,才是完整路径
        QTreeWidgetItem* pfather=item->parent();
        while(1)
        {
            pstring text=pfather->text(column).toStdString();
            hlog(text);
            if(text.substr(1,2)==":/")
            {
                strPathFull=text+strClick;
                break;
            }
            pfather=pfather->parent();
            strPathFull=text+strClick;
        }
    }
    hlog(strPathFull);
    auto infoDir=this->getDirInfo(strPathFull);
    //        hlog(infoDir);
    //    this->strpwd=strClick;
    //        this->showDirInfo(this->strpwd);
    //加入到tree并展开
    for(pmap<pstring,pstring> mapi:infoDir)
    {
        //        hlog(mapi[strname]);
        QTreeWidgetItem* pchild=ui->ptree->newJoint(mapi[strname].c_str());
        item->addChild(pchild);
    }
}
void xFileBrowserLocal::slotTreeClick(QTreeWidgetItem *item, int column)
{
    pstring strClick=(item->text(column)).toStdString();
    this->strpwd=strClick;
    auto infoDir=this->showDirInfo(this->strpwd);
    //    hlog(infoDir);
}
