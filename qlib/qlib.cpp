#include "qlib.h"
QString strFullPathLog("xlfd.log");
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString level;
    switch (type) {
    case QtDebugMsg:
        level = "Debug";
        break;
        //    case QtInfoMsg:
        //        level = "Info ";
        //        break;
    case QtWarningMsg:
        level = "Warn ";
        break;
    case QtCriticalMsg:
        level = "Error";
        break;
    case QtFatalMsg:
        level = "Fatal";
        break;
    default:;
    }


    // 输出到日志文件, 格式: 时间 - [Level] (文件名:行数, 函数): 消息
    QString fileName = context.file;
    int index = fileName.lastIndexOf(QDir::separator());
    fileName = fileName.mid(index + 1);


    QString strlog=QString("[%1]-[tid: %2]-[%3:%4]: %5")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"))
            .arg(plib::getTid())
            .arg(fileName)
            .arg(context.line)
            .arg(msg);


    // 输出到标准输出
    QByteArray localMsg = strlog.toLocal8Bit();
    std::cout << std::string(localMsg)<<std::endl;

    //输出到文件
    QString strToday=QString("%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));


    //处理过的,主要针对xlfd.log这种不带全路径的
    QString strFullPathLogProcess=strFullPathLog;
    //如果路径中不带/或者\\,则就是上述这种的,自己加上
    if(!strFullPathLog.contains("\\")&&!strFullPathLog.contains("/"))
    {
        strFullPathLogProcess="./"+strFullPathLogProcess;
    }
    //    hlog(strFullPathLogProcess.toStdString());
    //解析路径,文件名用的
    QFileInfo fileInfo(strFullPathLogProcess);
    //    hlog(strFullPathLogProcess.toStdString());
    //    hlog(fileInfo.path().toStdString());
    //先判断有没有这个目录,没有的话先递归创建
    QDir mDir2;
    if(!mDir2.exists(fileInfo.path()))
    {
        mDir2.mkpath(fileInfo.path());
    }

    // 这个地方根据linux和windows的不同要改
    QFile file(fileInfo.path()+"\\"+fileInfo.baseName()+strToday+"."+fileInfo.suffix());
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    //保存为utf8格式,编码也是这个格式,否则会乱码
    text_stream.setCodec("UTF-8");
    text_stream << strlog << "\n";
    file.flush();
    file.close();

    mutex.unlock();
}
//为了使qDebug支持打印string或其他结构体,加这个
QDebug operator<<(QDebug debug, const std::string &s)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << s.c_str();

    return debug;
}
qlib::qlib()
{

}

void qlib::openExe(QString path)
{
    //    QProcess::start("cmd",QStringList()<<"/c"<<path);
}

QString qlib::saveDialogFile(QString qstrPathDefault)
{
    QFileDialog *fd=new QFileDialog(NULL,("保存文件"),qstrPathDefault,"zip(*.zip *.zip)");
    fd->setAcceptMode(QFileDialog::AcceptSave);//设置文件对话框为保存模式

    if(fd->exec()==QFileDialog::Accepted) // ok
    {
        QString file=fd->selectedFiles()[0];
        qDebug()<<file;
        return file;
    }
    else
        return NULL;
}

qlibRand::qlibRand()
{
    qsrand(time(NULL));
}

int qlibRand::rand(int s, int e)
{
    return s+qrand()%(e-s);
}
btcp::btcp()
{

}

btcp::btcp(string ip, int port)
{
    this->ip=ip;
    this->port=port;
}

btcp::btcp(QString ip, int port)
{
    this->ip=ip.toStdString();
    this->port=port;
}

btcp::btcp(int iSockAccept, string ip, int port)
{
    this->sock=iSockAccept;
    this->ip=ip;
    this->port=port;
}
typedef struct STRUCT_PARA
{
    btcp* ptcpAll;
    btcp* ptcpClient;
    void (*thread_user)(btcp* para);
}STRUCT_PARA;
//static void* thread_process(void* para)
DWORD WINAPI thread_processxb(LPVOID para)
{
    qlog("in thread_process");
    STRUCT_PARA* pmix=(STRUCT_PARA*)para;

    while(true)
    {
        //执行子函数
        if(pmix->ptcpClient->recvx()>0)
        {
            pmix->thread_user(pmix->ptcpClient);
        }
        else
        {
            //            hlog("客户端断开，接收失败");
            break;
        }
    }
    //    //查找
    //    qlog(pmix->ptcpClient->sock);
    //    for(pvector<ptcp*>::iterator it=pmix->ptcpAll->plistClient.begin();it!=pmix->ptcpAll->plistClient.end();it++)
    //    {
    //        ptcp* tcptmp=*it;
    //        hlog(tcptmp->sock);
    //        hlog(pmix->ptcpClient->sock);
    //        if(pmix->ptcpClient->sock==(tcptmp->sock))
    //        {
    //            //            hlog("before close");
    //            pmix->ptcpClient->close();
    //            //            hlog("before delete");
    //            delete pmix->ptcpClient;
    //            //            hlog("before erase");
    //            pmix->ptcpAll->plistClient.erase(it);

    //            //打印当前
    //            hlog("#######delete a client,now all clients:");
    //            hlog(pmix->ptcpAll->plistClient.size());
    //            for(int i=0;i<pmix->ptcpAll->plistClient.size();i++)
    //            {
    //                hlog(*(pmix->ptcpAll->plistClient[i]));
    //            }
    //            delete pmix;
    //            break;
    //        }
    //    }
    return NULL;
}
int btcp::setThreadServerb(void (*fun_thread)(btcp *))
{
    int iSock=ctcp_InitTcpSocket();
    if(iSock<0)
    {
        qlog("iSock<0");
        ctcp_CloseConnection(iSock);
        return -1;
    }

    int ibind=ctcp_BindSocket(iSock,this->ip.c_str(),port);
    if(ibind<0)
    {
        qlog("bind fail");
        qlog(strerror(errno));
        ctcp_CloseConnection(iSock);
        return -2;
    }


    this->sock=iSock;
    //    //创建一个统计线程
    //    pthread_t tid_netManage;
    //    pthread_create(&tid_netManage,NULL,thread_netManage,this);
    //不停等待，来一个，创建一个线程
    while(true)
    {
        //        xtcp* xtcpClient=tcp->accept(tcp);
        //        HLOG_XP(xtcpClient);
        //        int iSockAccept=AcceptConnection(this->sock);
        int iClientSock=-1;
        int iLength;
        struct sockaddr_in struSin;
        iLength=sizeof(struSin);
        memset(&struSin,0,iLength);
        iClientSock = accept(this->sock, (struct sockaddr *)&struSin, &iLength);

        string clientip=string(inet_ntoa(struSin.sin_addr));


        if(iClientSock<0)
        {
            //            HLOG("accept error:%s\n",strerror(errno));
            continue;
        }
        qlog(clientip);

        //这个地方必须要new，不然传参之后会被释放，覆盖,在线程中释放去
        STRUCT_PARA *sPara=new STRUCT_PARA;
        memset(sPara,0,sizeof(STRUCT_PARA));
        sPara->ptcpClient=new btcp(iClientSock,clientip,this->port);

        sPara->ptcpAll=this;
        sPara->thread_user=fun_thread;

        HANDLE handle = CreateThread(NULL, 0, thread_processxb, (LPVOID)sPara, 0, NULL);
    }
}




/*
 * 打开对话框浏览文件，返回路径，若取消了返回NULL
 */
QString qlib::openDialogFile(QString qstrPathDefault)
{
    //    QFileDialog *fd=new QFileDialog(NULL,("打开文件"),qstrPathDefault,"zip(*.zip *.zip)");
    QFileDialog *fd=new QFileDialog(NULL,("打开文件"),qstrPathDefault,"*(*.* *.*)");
    //     fd->setFilter(QString("zip(*.zip *.zip)"));
    if(fd->exec()==QFileDialog::Accepted) // ok
    {
        QString file=fd->selectedFiles()[0];
        qDebug()<<file;
        return file;
    }
    else
        return NULL;
}

//打开对话框多选并返回所选文件的所有路
QStringList qlib::openDialogFiles(QString qstrPathDefault)
{
    QStringList files;
    QFileDialog *fd=new QFileDialog(NULL,"打开文件",qstrPathDefault);
    //多
    fd->setFileMode(QFileDialog::ExistingFiles);
    if(fd->exec()==QFileDialog::Accepted) // ok
    {
        files=fd->selectedFiles();
        qDebug()<<files;
        return files;
    }
    else
        return files;
}

QString qlib::openDialogSelectDir(QString qstrPathDefault)
{
    QFileDialog* openFilePath = new QFileDialog(NULL, "select dirs",qstrPathDefault); //打开一个目录选择对话
    openFilePath-> setFileMode( QFileDialog::DirectoryOnly );
    QString filepath= openFilePath->getExistingDirectory();
    //如果最后不是斜杠,要加上
    if(filepath[filepath.length()-1]!='\\')
    {
        filepath.append('\\');
    }
    qDebug()<<filepath;
    openFilePath->deleteLater();
    return filepath;
}

bool qlib::mkdir(QString path)
{
    QString stres;
    QDir *temp = new QDir;
    bool exist = temp->exists(path);
    if(exist)
    {
        //        QMessageBox::warning(NULL,("mkdir"),("mkdir fail!-----exsist!"));

        stres=(("创建文件夹失败"));
        qlog(stres);
        return false;
    }
    else
    {
        bool ok = temp->mkdir(path);
        if( ok )
        {
            //            QMessageBox::warning(NULL,("mkdir"),("mkdir success!"));

            stres=(("创建文件夹成功"));
            qlog(stres);
        }
        return ok;
    }
}

void qlib::messageBox(QString strDefault)
{
    //该句无法按钮中文
    //    QMessageBox::information(NULL,("提示信息"),strDefault);
    QMessageBox qMsgBox(QMessageBox::Information,("提示信息"),strDefault);
    qMsgBox.setStandardButtons(QMessageBox::Ok);
    qMsgBox.setButtonText(QMessageBox::Ok,("确定"));
    qMsgBox.exec();
}
bool qlib::inputbox(QString &strInput, QString strdefault)
{
    QInputDialog inputDialog;
    inputDialog.setWindowTitle(("输入框"));
    inputDialog.setLabelText(strdefault);
    inputDialog.setOkButtonText(("确定"));
    inputDialog.setCancelButtonText(("取消"));
    inputDialog.setTextValue(("请输入内容"));
    if (inputDialog.exec())
    {
        strInput=inputDialog.textValue();
        return true;
    }
    return false;

}

bool qlib::confirmbox(QString strcontent)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(NULL,("确认信息"),strcontent,QMessageBox::Yes | QMessageBox::Cancel);
    if (reply == QMessageBox::Yes)
        return true;
    else if (reply == QMessageBox::Cancel)
        return false;
}



