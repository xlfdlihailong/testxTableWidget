#ifndef QLIB_H
#define QLIB_H
#include <QDebug>
#include <QTime>
#include <time.h>
#include <QCoreApplication>
#include <QPoint>
#include <QDataStream>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <QDebug>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QApplication>
#include <QSettings>
#include <QTextCodec>
#include <QDebug>
#include <QTime>
#include <QDir>
#include "plib/plib.h"
#include <QInputDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>
#include <QProcess>
//#include <QTimer>
#include <QWindow>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QBuffer>
#include <DbgHelp.h>
#include <QWidget>
#include <QFileInfo>
#include <QStorageInfo>
#include <QClipboard>
#include <QDataStream>
#include <QTextStream>
#include <QMimeData>
#include <QDrag>
#pragma execution_character_set("utf-8")

//使用前需要在主函数中调用qInstallMessageHandler(outputMessage); //注册MsgHandler回调函数
extern QString strFullPathLog;//若路径不存在则会自动递归创建
//#define qlog(var) qDebug().noquote()<<#var<<":"<<var;
//为了处理直接打印字符串的尴尬
#define qlogOrigin(var){\
    QString strtype=typeid(var).name();\
    if(strtype.contains("char const"))\
    qDebug().noquote()<<var;\
    else\
    qDebug().noquote()<<#var<<":"<<var;\
    }\
    //自定义输出日志--控制台和文件都输出,文件以日期为单位
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
//为了使qDebug支持打印string或其他结构体,加这个
QDebug operator<<(QDebug debug, const std::string &s);







static int qpHLOGFunp(const char *file, int line,string str)//配套重载打印函数时不要加&
{
    int iFd;
    //标准输入输出
    int iFd1 = 1;

    int len=str.size()+1024;
    char *acBuf=(char*)malloc(len);
    memset(acBuf,0,len);
    char     *pchIndex;
    time_t ts;
    char         pcHLOGPath[1024 + 1];
    strcpy(pcHLOGPath,strPathLog.c_str());

    pchIndex = strrchr(pcHLOGPath, '.');
    if (pchIndex == NULL)
        pchIndex = pcHLOGPath + strlen(pcHLOGPath);
    //到名字前
    memcpy(acBuf, pcHLOGPath, pchIndex - pcHLOGPath);
    ts = time(0);
    char tmp1[64];
    memset(tmp1,0,64);
    strftime(tmp1, sizeof(tmp1), "%Y-%m-%d",localtime(&ts) );
    //后面加上日期
    sprintf(acBuf+strlen(acBuf),"%s.log",tmp1);
    //打开自定义的文件,如果文件打开失败，则退出
    if ((iFd = open(acBuf, O_RDWR | O_APPEND | O_CREAT, 0666)) == -1)
    {
        std::cout<<"open file fail "<<std::endl<<acBuf<<std::endl;
        free(acBuf);
        acBuf=NULL;
        return -1;
    }
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    char data[50]={0};
    sprintf(data,"[%02d:%02d:%02d.%03d",wtm.wHour,wtm.wMinute,wtm.wSecond,wtm.wMilliseconds);
    strcpy(acBuf,data);
    std::string str2(file);
    //这个地方windows和linux不一样,要注意
    std::string pattern="\\";
    std::string::size_type pos;
    std::vector<std::string> result;
    str2+=pattern;
    int size=str2.size();
    for(int i=0;i<size;i++)
    {
        pos=str2.find(pattern,i);
        if(pos<size)
        {
            std::string s=str2.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    const char* fileOnly=result[result.size()-1].c_str();
    sprintf(acBuf + strlen(acBuf), "]-[tid:%5ld]-[%s:%d]", GetCurrentThreadId(),fileOnly,line);
    sprintf(acBuf,"%-40s",acBuf);

    str+='\n';
    int lenRes=strlen(acBuf)+str.size();
    memcpy(acBuf+strlen(acBuf),str.c_str(),str.size());
    write(iFd, acBuf, lenRes);
    write(iFd1, acBuf, lenRes);

    close(iFd);
    free(acBuf);
    acBuf=NULL;
    return 0;
}
static QString QCommon_unicodeToUtf8(QString Str)
{
    QString filename = Str;
    do
    {
        int idx = filename.indexOf("\\u");
        //        hlog(idx);
        QString strHex = filename.mid(idx, 6);
        strHex = strHex.replace("\\u", QString());
        int nHex = strHex.toInt(0, 16);
        filename.replace(idx, 6, QChar(nHex));
    }
    while (filename.indexOf("\\u") != -1);
    return filename;
}

//hlog("发神鼎飞丹砂");
//qlog("发到手发到手");
//QString info="佛挡杀佛色发";
//hlog(info);
//qlog(info);
//pstring str="法第三方士大夫";
//hlog(str);
//qlog(str);
//string str2="发到手";
//hlog(str2);
//qlog(str2);
//char data[255];cinit(data);strcpy(data,"分为非");
//hlog(data);
//qlog(data);
//char* data2="发到手发到手";
//hlog(data2);
//qlog(data2);
//QStringList lstr=QStringList()<<"佛挡杀佛文访问"<<"fwfewfwefwe氛围";
//qlog(lstr);
template<class T>
static QString QCommon_toString(const T & t)
{
    pstring strtype=(typeid(T).name());
    QString stres;
    //    hlog(strtype);
    //如果是c字符串char*
    if(strtype.contain("char const [")||strtype.contain("char ["))
    {
        //        hlog("this is const char array");
        stres=(((char*)(&t)));
        return stres;
    }
    else if(strtype.contain("char const *")||strtype.contain("char *"))
    {
        //        hlog("this is char *");
        //        hlog(t);
        string str=plib::toString(t);
        //        hlog(str);
        stres=QString::fromStdString(str);
        return stres;
        //        hlog(t);
        //直接转不行...
        //        stres=((char*)((&t)));
    }
    //如果是c++字符串string或者pstring
    else if(typeid(T)==typeid(pstring)||typeid(T)==typeid(string))
    {
        //        hlog("this is c++ string");
        //            pstring strcpp=*((pstring*)(&t));
        stres=(QString::fromStdString(*((string*)(&t))));
        return stres;
    }
    else if(typeid(T)==typeid(QString))
    {
        //        hlog("this is QString string");
        stres=(*(QString*)(&t));
        return stres;
    }
    else
    {

        QBuffer bug;
        bug.open(QIODevice::ReadWrite);
        QDebug * deb = new QDebug(&bug);

        (*deb)<<t;
        delete deb;
        QString str(bug.data());
        //        QString str=QString::fromLatin1(bug.data());
        stres= str.mid(0,str.size()-1);
    }
    //    QStringList lstr=QStringList()<<"佛挡杀佛文访问"<<"fwfewfwefwe氛围";
    //这种的需要转
    QString stresutf8= QCommon_unicodeToUtf8(stres);
    return stresutf8;
}
template<typename T>
void qprintarg(ostream& os,T a)
{
    //隔开，防止打印字符串时乱码不能分割，与序列化一个原理
    //但是不能用跟序列化一样的字符1，不然会导致序列化的混乱
    QString qstra=QCommon_toString(a);
    string stres=qstra.toStdString();
    os<<stres<<"\2";
}

template<class...Args>
void qexpand(Args... args)
{
    ostringstream os;
    int arr[] = {(qprintarg(os,args), 0)...};
    //或者改进为 std::initializer_list<int>{(printarg(args), 0)...};lumda
    string stres=os.str();
    //    hlog(stres);
    //这个地方不能用多个字符串分割的，不然结果是空字符串的时候会自动去掉
    vector<string> ls=common_split_one(stres,string("\2"));
    ls.erase(ls.end()-1);
    //        for(int i=0;i<ls.size();i++)
    //        {
    //            hlog(ls[i],ls[i].size(),i);
    //        }
    const char* file=ls[0].c_str();
    //    hlog(file);
    int line=atoi(ls[1].c_str());

    //这个一定是实际参数个数，
    vector<string> listValues;
    //    hlogone(ls.size());
    for(int i=3;i<ls.size();i++)
    {
        listValues.push_back(ls[i]);
    }

    //真实参数个数
    //    hlogone(listValues.size());
    //最终结果
    vector<string> listNames;
    //如果真是参数个数是1，则不用分了
    if(listValues.size()==1)
    {
        listNames.push_back(ls[2]);

    }
    else//否则要根据逗号分
    {
        //先按逗号分，后来再整理ls[2]是传进来的参数名字，带逗号的
        vector<string> listNamesOrigin=common_split(ls[2],",");
        //                hlog(listNamesOrigin.size());

        //如果name数正好等于value数，那就正好了，不然就要考虑其他情况
        if(listNamesOrigin.size()==listValues.size())
        {
            for(int i=0;i<listNamesOrigin.size();i++)
            {
                listNames.push_back(listNamesOrigin[i]);
            }
        }
        else
        {
            for(int i=0;i<listNamesOrigin.size();i++)
            {
                string stri=listNamesOrigin[i];
                int il=0;
                int ir=0;
                int ishuangyinhao=0;
                //算（-）的个数
                for(int q=0;q<stri.size();q++)
                {
                    if(stri[q]=='(')
                        il++;
                    if(stri[q]==')')
                        ir++;
                    if(stri[q]=='\"')
                        ishuangyinhao++;
                }
                int icha=il-ir;
                //        hlogone(icha);

                //如果这一项不包扩括号和引号，则就是变量
                if(icha==0&&ishuangyinhao==0)
                {
                    listNames.push_back(stri);
                    //            hlogone(stri);
                }//优先级是先处理引号
                else if(ishuangyinhao>0)
                {
                    string strjoin=stri;
                    //            hlogone(stri);
                    //从这个开始往后数
                    for(int j=i+1;j<listNamesOrigin.size();j++)
                    {
                        string strj=listNamesOrigin[j];
                        strjoin+=","+strj;
                        for(int p=0;p<strj.size();p++)
                        {
                            if(strj[p]=='"')
                            {
                                ishuangyinhao--;
                                //                        hlogone(ishuangyinhao);
                            }
                        }
                        //为了防止互相干扰
                        if(((ishuangyinhao==0)))
                        {
                            //把i指针移到这
                            i=j;
                            break;
                        }
                    }

                    //            hlogone(strjoin);
                    listNames.push_back(strjoin);
                }
                //只包含（但不包含），则要跟下一个结合，遇到（增加，遇到）减少，直到为0为止算一个
                else if(icha>0)
                {
                    string strjoin=stri;
                    //从这个开始往后数
                    for(int j=i+1;j<listNamesOrigin.size();j++)
                    {
                        string strj=listNamesOrigin[j];
                        strjoin+=","+strj;
                        for(int p=0;p<strj.size();p++)
                        {
                            if(strj[p]=='(')
                                il++;
                            if(strj[p]==')')
                                ir++;
                        }
                        //为了防止互相干扰
                        if(((il-ir==0)))
                        {
                            //把i指针移到这
                            i=j;
                            break;
                        }
                    }
                    listNames.push_back(strjoin);
                }
            }
        }
    }
    //打印
    //        hlog(listNames.size());
    //        for(int i=0;i<listNames.size();i++)
    //        {
    //            hlog(listNames[i]);
    //        }
    ////        hlog(listValues.size());
    //        for(int i=0;i<listValues.size();i++)
    //        {
    //            hlog(listValues[i]);
    //        }
    //整理了一遍之后，如果发现

    string strval;
    for(int i=0;i<listNames.size();i++)
    {
        //在此判断是否是单独字符串,或者是pstring()<<这种的行为
        //        hlog(listNames[i]);
        //        string stri=listNames[i];
        //        hlog(stri.size());
        //        hlog(stri[0]=='"',stri[stri.size()-1]=='"');
        //        hlog(stri[stri.size()-1]=='"');
        //                hlogone(listValues[i]);
        //如果名字第一个字符和最后一个是",则说明是字符串,直接打印value,因为有可能会有这种的xfdsf",,wfew,
        if((listNames[i][0]=='"'&&listNames[i][listNames[i].size()-1]=='"')||
                (listNames[i].find("()<<")!=string::npos))
        {
            strval+="["+listValues[i]+"] ";
        }
        else
        {
            strval+="["+listNames[i]+" : "+listValues[i]+"] ";
        }
    }

    strval=strval.substr(0,strval.size()-1);
    //    hlogone(strval);
    qpHLOGFunp(file,line,strval);
}

//支持多参数不同类型的联合打印
//支持函数嵌套带逗号，已处理
//hlog(itest,funtest(funtest2(funtest3(2),2),3,funtest2(6,8)),db,funtest2(2,3),funtest3(3));

//hlog("发神鼎飞丹砂");
//qlog("发到手发到手");
//QString info="佛挡杀佛色发";
//hlog(info);
//qlog(info);
//pstring str="法第三方士大夫";
//hlog(str);
//qlog(str);
//string str2="发到手";
//hlog(str2);
//qlog(str2);
//char data[255];cinit(data);strcpy(data,"分为非");
//hlog(data);
//qlog(data);
//char* data2="发到手发到手";
//hlog(data2);
//qlog(data2);
#define qlog(...) qexpand(__FILE__,__LINE__,#__VA_ARGS__,__VA_ARGS__)



class xlfdQProcess:public QProcess{
public:
    xlfdQProcess(QString path)
    {
        this->strpath=path;
    }
    //非阻塞
    void openExeDetach()
    {
        this->start("cmd",QStringList()<<"/c"<<strpath);
    }
    //阻塞--不管用,以后再改
    void openExeJoin()
    {
        this->start("cmd",QStringList()<<"/c"<<strpath);
        this->waitForFinished();
    }
private:
    QString strpath;
};
//有问题待修改,用的plib的getCmdOutput
class xprocess:public QObject
{
    Q_OBJECT
public:
    QProcess* pcmd;
    QString  strpath;
    QStringList listPara;
    xprocess(QString path,QStringList lpara=QStringList())
    {
        this->listPara=lpara;
        pcmd=new QProcess(this);
        this->strpath=path;
        connect(pcmd,SIGNAL(readyReadStandardOutput()) , this , SLOT(slotOutput()));
        connect(pcmd,SIGNAL(readyReadStandardError()) , this , SLOT(slotError()));
        connect(pcmd,SIGNAL(finished(int)),this,SLOT(slotFinish()));

    }
    void open()
    {
        //        hlog(strpath);
//        qlog(listPara);
        if(listPara.size()>0)
            pcmd->start(strpath,this->listPara);
        else
            pcmd->start(strpath);
                    //        pcmd->startDetached("cmd",QStringList()<<"/c"<<"dir");
                    //        pcmd->startDetached("cmd",QStringList()<<"/c"<<strpath);
                    //         pcmd->startDetached("cmd",QStringList()<<strpath);
                    //        pcmd->startDetached(strpath);
                    //        pcmd->waitForFinished();
    }

    ~xprocess()
    {
        if(pcmd)
        {
            pcmd->close();
            pcmd->waitForFinished();
        }
        delete pcmd;
    }

private slots:
    void slotOutput()
    {
        QString stres=pcmd->readAllStandardOutput();
        hlog(stres);
    }
    void slotError()
    {
        QString stres=pcmd->readAllStandardError();
        hlog(stres);
    }

    void slotFinish()
    {
//        hlog("finish");
        //        QString stres=pcmd->readAllStandardOutput();
        //        hlog(stres);
    }

};


static int GenerateMiniDump(PEXCEPTION_POINTERS pExceptionPointers)
{
    // 定义函数指针
    typedef BOOL(WINAPI * MiniDumpWriteDumpT)(
                HANDLE,
                DWORD,
                HANDLE,
                MINIDUMP_TYPE,
                PMINIDUMP_EXCEPTION_INFORMATION,
                PMINIDUMP_USER_STREAM_INFORMATION,
                PMINIDUMP_CALLBACK_INFORMATION
                );
    // 从 "DbgHelp.dll" 库中获取 "MiniDumpWriteDump" 函数
    MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
    HMODULE hDbgHelp = LoadLibraryA(("DbgHelp.dll"));
    if (NULL == hDbgHelp)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

    if (NULL == pfnMiniDumpWriteDump)
    {
        FreeLibrary(hDbgHelp);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    // 创建 dmp 文件件
    TCHAR szFileName[MAX_PATH] = { 0 };
    TCHAR szVersion[] = L"DumpFile";
    SYSTEMTIME stLocalTime;
    GetLocalTime(&stLocalTime);
    wsprintf(szFileName, L"%s-%04d%02d%02d-%02d%02d%02d.dmp",
             szVersion, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
             stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
    HANDLE hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
                                  FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    if (INVALID_HANDLE_VALUE == hDumpFile)
    {
        FreeLibrary(hDbgHelp);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    // 写入 dmp 文件
    MINIDUMP_EXCEPTION_INFORMATION expParam;
    expParam.ThreadId = GetCurrentThreadId();
    expParam.ExceptionPointers = pExceptionPointers;
    expParam.ClientPointers = FALSE;
    pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                         hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &expParam : NULL), NULL, NULL);
    // 释放文件
    CloseHandle(hDumpFile);
    FreeLibrary(hDbgHelp);
    return EXCEPTION_EXECUTE_HANDLER;
}

static LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
    // 这里做一些异常的过滤或提示
    if (IsDebuggerPresent()) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    return GenerateMiniDump(lpExceptionInfo);
}


class qlib
{
public:
    qlib();

    static QString getCopyInClipboard()
    {
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        QString originalText = clipboard->text();         //获取剪贴板上文本信息
        return originalText;
    }
    static void setCopyInClipboard(QString info)
    {
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        clipboard->setText(info);
    }


    static QStringList getAllDeviceValid()
    {
        QStringList res;
        //获取所有正常磁盘
        foreach(const QStorageInfo & storage, QStorageInfo::mountedVolumes())
        {
            if (storage.isValid() && storage.isReady())
            {
                if (!storage.isReadOnly())
                {
                    res.append(storage.rootPath());
                    //                    qlog(storage.rootPath());

                    //                    hlog(storage.rootPath().toStdString());
                    //                qDebug() << "rootpath:" << storage.rootPath();
                    //                qDebug() << "name:" << storage.name();
                    //                hlog(storage.name().toStdString());
                    //                qlog(storage.name());
                    //                qDebug() << "displayname:" << storage.displayName();
                    //                qDebug() << "fileSystemType:" << storage.fileSystemType();
                    //                qDebug() << "size:" << storage.bytesTotal() / 1000 / 1000 << "MB";
                    //                qDebug() << "availableSize:" << storage.bytesAvailable() / 1000 / 1000 << "MB";
                }
            }

        }
        return res;
    }

    //中文无乱码的读取写入
    static QString getConfString(QString joint,QString key,QString path="xlfd.conf")
    {
        QSettings *configIniWrite = new QSettings(path, QSettings::IniFormat);
        configIniWrite->setIniCodec(QTextCodec::codecForName("GB2312")); //在此添加设置，即可读写ini文件中的中文
        //向ini文件中写入内容,setValue函数的两个参数是键值对
        //向ini文件的第一个节写入内容,ip节下的第一个参数
        QString strInfo="/"+joint+"/"+key;
        //        configIniWrite->setValue("/ip/first", "192.168.0.1");
        //        value=QString(plib::toUTF8(value.toStdString().c_str()).c_str());

        QString stread=configIniWrite->value(strInfo).toString();
        qlog(stread);

        //写入完成后删除指针
        delete configIniWrite;
        return stread;
    }
    //中文无乱码的读取写入
    static void setConfString(QString joint,QString key,QString value,QString path="xlfd.conf")
    {
        QSettings *configIniWrite = new QSettings(path, QSettings::IniFormat);
        configIniWrite->setIniCodec(QTextCodec::codecForName("GB2312")); //在此添加设置，即可读写ini文件中的中文
        //向ini文件中写入内容,setValue函数的两个参数是键值对
        //向ini文件的第一个节写入内容,ip节下的第一个参数
        QString strInfo="/"+joint+"/"+key;
        //        configIniWrite->setValue("/ip/first", "192.168.0.1");
        //        value=QString(plib::toUTF8(value.toStdString().c_str()).c_str());
        configIniWrite->setValue(strInfo,value);
        //写入完成后删除指针
        delete configIniWrite;
    }
    //    需要增加这两个才有用?好像不需要?实测一下回头
    //#CONFIG+=force_debug_info
    //#CONFIG+=separate_debug_info
    static void genDump()
    {
        SetUnhandledExceptionFilter(ExceptionFilter);
    }

    //qlib::openDir("C:/");
    static void openDir(QString dir)
    {
        QDesktopServices::openUrl(QUrl("file:"+(dir), QUrl::TolerantMode));
    }

    //正常":/qss/flatwhite.css"
    //名字是flatwhite.css//默认黑色风格
    static void loadQss(QApplication* pa,QString name="psblack.css")
    {
        QString strpath=":/qss/"+name;
        qDebug()<<strpath;
        //加载样式表
        QFile file(":/qss/"+name);

        if (file.open(QFile::ReadOnly)) {
            qDebug("open success");
            QString qss = QLatin1String(file.readAll());
            QString paletteColor = qss.mid(20, 7);
            pa->setPalette(QPalette(QColor(paletteColor)));
            pa->setStyleSheet(qss);
            file.close();
        }
        else {
            qDebug("open fail");
        }
    }

    static void openExe(QString path);
    template <class T>
    static QByteArray toByteArray(T t)
    {
        //        QPoint pos(5, 7);
        //        qDebug()<<pos;
        //        QPoint tmp;
        //        qDebug()<<tmp;
        QByteArray datagram;
        QDataStream out(&datagram, QIODevice::ReadWrite);   //使用QDataStream将Qpoint序列化进QByteArray内
        out.setVersion(QDataStream::Qt_4_8);                //设置Qt串行化版本 使用Qt4.8版本流
        out<<t;
        //        qDebug()<<datagram.length();
        return datagram;
    }
    template <class T>
    static T toClass(QByteArray datagram)
    {
        T t;
        QDataStream dsIn(&datagram,QIODevice::ReadWrite);  //使用QDataStream将QByteArray反序列化
        dsIn.setVersion(QDataStream::Qt_4_8);             //设置Qt串行化版本 使用Qt4.8版本流
        dsIn>>t;
        //        qDebug()<<tmp;
        return t;
    }
    //     @brief ToString 任何数据到字符串的处理，依赖于QDebug
    //    hlog("发神鼎飞丹砂");
    //    qlog("发到手发到手");
    //    QString info="佛挡杀佛色发";
    //    hlog(info);
    //    qlog(info);
    //    pstring str="法第三方士大夫";
    //    hlog(str);
    //    qlog(str);
    //    string str2="发到手";
    //    hlog(str2);
    //    qlog(str2);
    //    char data[255];cinit(data);strcpy(data,"分为非");
    //    hlog(data);
    //    qlog(data);
    //    char* data2="发到手发到手";
    //    hlog(data2);
    //    qlog(data2);
    //    QStringList lstr=QStringList()<<"佛挡杀佛文访问"<<"fwfewfwefwe氛围";
    //    qlog(lstr);
    template<class T>
    static QString toString(const T & t)
    {
        return QCommon_toString(t);
        //        pstring strtype=(typeid(T).name());
        ////        hlog(strtype);
        //        //如果是c字符串char*
        //        if(strtype.contain("char const ["))
        //        {
        ////            hlog("this is const char array");
        //            return QString((char*)(&t));
        //        }
        //        //如果是c++字符串string或者pstring
        //        if(typeid(T)==typeid(pstring)||typeid(T)==typeid(string))
        //        {
        ////            hlog("this is c++ string");
        ////            pstring strcpp=*((pstring*)(&t));
        //            return QString::fromStdString(*((string*)(&t)));
        //        }
        //        if(typeid(T)==typeid(QString))
        //        {
        ////            hlog("this is QString string");
        //            return *(QString*)(&t);
        //        }

        //        QBuffer bug;
        //        bug.open(QIODevice::ReadWrite);
        //        QDebug * deb = new QDebug(&bug);

        //        (*deb)<<t;
        //        delete deb;
        //        QString str(bug.data());
        ////        QString str=QString::fromLatin1(bug.data());
        //        QString stres= str.mid(0,str.size()-1);
        ////        return stres;
        //        return unicodeToUtf8(stres);
    }
    //unicode to utf8\u53d1这种
    static QString unicodeToUtf8(QString Str)
    {
        return QCommon_unicodeToUtf8(Str);
        //        QString filename = Str;
        //        do {
        //            int idx = filename.indexOf("\\u");
        //            QString strHex = filename.mid(idx, 6);
        //            strHex = strHex.replace("\\u", QString());
        //            int nHex = strHex.toInt(0, 16);
        //            filename.replace(idx, 6, QChar(nHex));
        //        } while (filename.indexOf("\\u") != -1);
        //        return filename;
    }

    //但有时你又无法得知QByteArray中的文字编码到底是GBK还是UTF-8，所以不知道用哪个
    static QString GetCorrectUnicode(const QByteArray &ba)
    {
        QTextCodec::ConverterState state;
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString text = codec->toUnicode( ba.constData(), ba.size(), &state);
        qDebug() <<state.invalidChars;
        //等于是是GBK
        if (state.invalidChars == 4)
        {
            text = QTextCodec::codecForName( "GBK" )->toUnicode(ba);
        }
        else if(state.invalidChars == 3)
        {//>>.3是unicode
            text = QTextCodec::codecForName( "Unicode" )->toUnicode(ba);
        }
        else
        {
            text = ba;
        }
        return text;
    }
    //常用c++类型也在这里面
    static void initLog(QString strLogFullPath="xlfd.log")
    {
        plib::initLog(strLogFullPath.toStdString().c_str());
        strFullPathLog=strLogFullPath;
        qInstallMessageHandler(outputMessage);

        qRegisterMetaType<pstring>("pstring");
        qRegisterMetaType<string>("string");
        qRegisterMetaType<pliststring>("pliststring");
        qRegisterMetaType<QVector<int>>("QVector<int>");
        qRegisterMetaType<plist<pmap<pstring,pstring>>>("plist<pmap<pstring,pstring>>");

        //        qRegisterMetaType<plist>("plist");
        //        qRegisterMetaType<pmap>("pmap");
        //        qRegisterMetaType<pset>("pset");

    }
    //保存单个文件
    static QString saveDialogFile(QString qstrPathDefault="C:/Program Files/MATLAB/R2010b/bin");
    //选择单个文件
    static QString openDialogFile(QString qstrPathDefault="D:/");
    //选择多个文件
    static QStringList openDialogFiles(QString qstrPathDefault="C:/desktop/401/highspeedphotos/photos/SCENE5");
    //选择文件夹,已做后期处理,结果总是最后带斜杠的
    static QString openDialogSelectDir(QString qstrPathDefault="./");
    //创建文件夹,创建目录
    static bool mkdir(QString path="./");
    //弹出信息提示框
    static void messageBox(QString strDefault=("这是个信息提示框"));
    //弹出输入框,使用方法如下
    //QString strinput;
    //if(qlib::inputbox(strinput))
    //{
    //    qlog(strinput);
    //}
    //支持中文确定和取消了
    static bool inputbox(QString &strInput, QString strdefault=("您要输入的文本"));




    //弹出确认框
    static bool confirmbox(QString strcontent);

    static QVector<QMap<QString,QString>> vp2qvp(plist<pmap<pstring,pstring> > pres)
    {
        QVector<QMap<QString,QString>> qres;
        for(int i=0;i<pres.size();i++)
        {
            QMap<QString,QString> qmapi;
            pmap<pstring,pstring> mapi=pres[i];
            for( pmap<pstring,pstring>::iterator iter = mapi.begin(); iter != mapi.end(); iter++)
            {
                pstring pkey=iter->first;
                pstring pval=iter->second;

                qmapi.insert(QString::fromStdString(pkey),
                             QString::fromStdString(pval));
            }
            qres.append(qmapi);
        }
        return qres;
    }
    static QVector<QVector<QString> > vvs2qvvs(plist<plist<pstring> > pres)
    {
        QVector<QVector<QString> > vvs;
        for(int i=0;i<pres.size();i++)
        {
            QVector<QString> qi;
            for(int j=0;j<pres[i].size();j++)
            {
                qi.append(QString::fromStdString(pres[i][j]));
            }
            vvs.append(qi);
        }
        return vvs;
    }
    template <class T>
    static plist<T> toPlist(QVector<T> qlist)
    {
        plist<T> lres;
        for(int i=0;i<qlist.size();i++)
        {
            lres.append(qlist[i]);
        }
        return lres;
    }
    template <class T1,class T2>
    static pmap<T1,T2> toPmap(QMap<T1,T2> qmap)
    {
        pmap<T1,T2> mres;
        for(typename QMap<T1,T2>::iterator it=qmap.begin();it!=qmap.end();it++)
        {
            mres.append(it.key(),it.value());
        }
        return mres;
    }
    template <class T1,class T2>
    static QMap<T1,T2> toQmap(pmap<T1,T2> map)
    {
        QMap<T1,T2> mres;
        for(int i=0;i<map.size();i++)
        {
            T1 t1=map.getKey(i);
            T2 t2=map[t1];
            mres.insert(t1,t2);
        }
        return mres;
    }
};





class qlibRand
{
public:
    qlibRand();
    int rand(int s,int e);//[s,e)
};
class btcp:public ptcp{
public:
    btcp();
    btcp(string ip,int port);
    btcp(QString ip,int port);
    btcp(int iSockAccept,string ip,int port);//接受时用,只会是客户端 //该方法不需要init
    int setThreadServerb(void (*fun_thread)(btcp * ptcp));
    template <class T>
    int sendb(string strtype,T t)//############send用这个
    {
        //        string strdata=libBoost::toString(t);

        QByteArray qba=qlib::toByteArray(t);
        //        QByteArray qba=toByteArray(t);
        //        qlog(qba);
        //        qlog(qba.count());
        //        qlog(qba.data());
        //        string strdata=string(qba.data());//如果这样,肯定要失败的,因为不知道长度,必须按长度来赋值
        string strdata;
        for(int i=0;i<qba.length();i++)
            strdata.push_back(qba.data()[i]);
        //        qlog(strdata.size());
        return sendx(strtype,strdata);
    }
    //只发包头
    int sendb(string strtype)
    {
        return sendx(strtype);
    }
    int recvb(int timeout=-1)
    {
        return recvx(timeout);
    }


    template <class T>
    T getClass()
    {
        //必须指定长度,不然无法使用,因为不知道char数组的具体长度
        //        return toClass<T>(QByteArray(this->getData(),this->length));
        return qlib::toClass<T>(QByteArray(this->getData(),this->length));
    }
private:
};

//要加static 不然多个项目重复使用会有重定义错误
static ostream& operator<<(ostream& os,QString str)
{
    os<<pstring(str.toStdString());
    return os;
}
static istream& operator>>(istream& is,QString &str)
{
    pstring data;
    is>>data;
    //    hlog(data);
    str=qlib::toString(data);
    return is;
}
static ostream& operator<<(ostream& os,QStringList xv)
{
    os<<"len: "<<xv.size()<<" QStringList\n{\n";
    for(int i=0;i<xv.size();i++)
    {
        os<<xv[i]<<"\n";
    }
    os<<"}";
    return os;
}
static istream & operator>>(istream &is,QStringList &xv)
{
    for(int i=0;i<5;i++)
        is.get();

    size_t len=0;
    is>>len;
    //7是纯名字的长度pvector
    common_skipCountch(is,11+4);
    for(int i=0;i<len;i++)
    {
        QString t;
        is>>t;

        xv.append(t);
        common_skipCountch(is,1);//跳过空格
    }
    common_skipCountch(is,1);
    return is;
}

template<class T>
static ostream& operator<<(ostream &os,QList<T> xv)
{
    os<<"len: "<<xv.size()<<" QList\n{\n";
    for(int i=0;i<xv.size();i++)
    {
        os<<xv[i]<<"\n";
    }
    os<<"}";
    return os;
}
template<class T>
static istream & operator>>(istream &is,QList<T> &xv)
{
    for(int i=0;i<5;i++)
        is.get();

    size_t len=0;
    is>>len;
    //7是纯名字的长度pvector
    common_skipCountch(is,5+4);
    for(int i=0;i<len;i++)
    {
        T t;
        is>>t;

        xv.append(t);
        common_skipCountch(is,1);//跳过空格
    }
    common_skipCountch(is,1);
    return is;
}

template<class T>
static ostream& operator<<(ostream &os,QVector<T> xv)
{
    os<<"len: "<<xv.size()<<" QVector\n{\n";
    for(int i=0;i<xv.size();i++)
    {
        os<<xv[i]<<"\n";
    }
    os<<"}";
    return os;
}
template<class T>
static istream & operator>>(istream &is,QVector<T> &xv)
{
    for(int i=0;i<5;i++)
        is.get();

    size_t len=0;
    is>>len;
    //7是纯名字的长度pvector
    common_skipCountch(is,7+4);
    for(int i=0;i<len;i++)
    {
        T t;
        is>>t;

        xv.append(t);
        common_skipCountch(is,1);//跳过空格
    }
    common_skipCountch(is,1);
    return is;
}



template<class T1,class T2>
static ostream& operator<<(ostream& os,QMap<T1,T2> qmap)
{
    os<<qlib::toPmap(qmap);
    return os;
}
template<class T1,class T2>
static istream& operator>>(istream& is,QMap<T1,T2> &qmap)
{
    pmap<T1,T2> mapcpp;
    is>>mapcpp;
    hlog(mapcpp);
    qmap =qlib::toQmap(mapcpp);
    return is;
}

#endif // QLIB_H
