#include "plib.h"
//默认是当前目录下的xlfd.log
std::string strPathLog("xlfd.log");
std::string strPathConf("xlfd.conf");
std::mutex mut;
//序列化方案,以[为分解单位,所以字符串中不要出现[,有空格没关系,支持
template <class T>
static std::string toString(T t)//与类中重载的对应
{
    std::stringstream ss;
    ss<<t;
    return ss.str();
}







/*
 * iSock 原来的socket
 * iRetryCount: 重试次数
 * iTimeoutTime: 每次重连的超时时间
 */
int reconnectTcp(int iSock,const char* ip,int port,int iRetryCount,int iTimeoutTime)
{
    int  j;                              //循环变量j
    struct timeval tmUp;                         //超时时间
    hlog("Disconnect from server..."+std::string(strerror(errno)));
    //先关闭异常的套接字
    if(iSock>0)
    {
        //        hlog("close connection");
        ctcp_CloseConnection(iSock);
    }
    //重新初始化套接字
    iSock = ctcp_InitTcpSocket();
    //循环指定的重试连接次数,-1表无限次

    for (j=0; (j<iRetryCount)||(iRetryCount<0); j++)
    {
        //如果连接失败，则休眠指定的延迟时间，成功则进行下一个TCP连接
        if (CreateConnection(iSock, ip, port) < 0)
        {
            std::string str=toString(j+1)+"th reconnect fail!";
            hlog(str);
            Sleep(iTimeoutTime*1000);//靠这个次数来确定最大重连次数
        }
        else
        {
            //            hlog(iSock);
            std::string str=toString(j+1)+"th reconnect success!";
            hlog(str);
            break;
        }
    }
    //    hlog(iSock);
    //如果已经达到最大重试连接次数，则退出
    //    if (j == g_iConnRetryTimes)
    if (j == iRetryCount)
    {
        hlog("exceed max retry times");
        return -1;
    }
    //超时时间--该设置貌似不管用...
    tmUp.tv_sec = 1000 * iTimeoutTime;
    tmUp.tv_usec = 0;
    //设置超时时间
    if (setsockopt(iSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tmUp, sizeof(tmUp)) == -1)
    {
        //        HLOG_STRING_STRINGA("setsockopt: ", strerror(errno));
        hlog(strerror(errno));
    }
    //    hlog(iSock);
    hlog("Reconnect to server!");
    return iSock;
}

/*
 * InitTcpSocket
 * 返回值：套接字，0表示初始化套接字失败
 *
 * 初始化新的Tcp套接字。
*/
int ctcp_InitTcpSocket(void)
{
    //    int iSock = 0;

    //    //如果套接字初始化失败，则返回-1
    //    if ((iSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    //    {
    //        hlog("套接字初始化失败");
    //        return -1;
    //    }
    //初始化WSA
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
        return -2;
    }
    //创建套接字
    SOCKET iSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(iSock == INVALID_SOCKET)
    {
        hlog("socket init fail");
        return -1;
    }
    //    //    //设置接收超时时间
    //    struct timeval timeout={10,0};//秒和微妙
    //    setsockopt(iSock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
    return iSock;
}

/*
 * CreateConnection
 * iSocket：  套接字
 * pchIPAddr：IP地址
 * iPort：    端口号
 * 返回值：   0表示成功
 *            其它表示失败
 * 根据IP地址和端口号，创建与其的连接。
*/
int CreateConnection(int iSocket, const char *pchIPAddr, int iPort)
{
    struct sockaddr_in struSin;

    if (pchIPAddr == NULL && !strcmp("", (char *)pchIPAddr))
    {
        hlog("ip addr error");
        return -1;
    }
    if (iPort <= 0)
    {
        hlog("port < =0");
        return -2;
    }

    //初始化套接字地址结构体
    struSin.sin_family = AF_INET;
    struSin.sin_port = htons(iPort);
    struSin.sin_addr.s_addr = inet_addr(pchIPAddr);

    //向目标地址发起TCP连接，返回状态信息
    //    return connect(iSocket, (struct sockaddr *)&struSin, sizeof(struSin));
    //向目标地址发起TCP连接，返回状态信息
    if(connect(iSocket, (struct sockaddr *)&struSin, sizeof(struSin))==-1)
    {
        if (errno == EINPROGRESS)
        {
            hlog("#############连接 超时##############");
            return -3;
        }
        return -5;
    }
    return 0;
}
//linux设置setsockopt不管用，windows必须用select超时
int CreateConnectionTimeout(int sock, const char *host, int port)
{
    TIMEVAL Timeout;
    Timeout.tv_sec = 1;//超时时间秒
    Timeout.tv_usec = 0;
    struct sockaddr_in address;  /* the libc network address data structure */

    //    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    address.sin_addr.s_addr = inet_addr(host); /* assign the address */
    address.sin_port = htons(port);            /* translate int2port num */
    address.sin_family = AF_INET;

    //set the socket in non-blocking
    unsigned long iMode = 1;
    int iResult = ioctlsocket(sock, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
    {
        hlog("ioctlsocket failed with error: %ld\n", iResult);
    }

    if(connect(sock,(struct sockaddr *)&address,sizeof(address))==false)
    {
        hlog("connect fail");
        return -1;
    }

    // restart the socket mode
    iMode = 0;
    iResult = ioctlsocket(sock, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
    {
        hlog("ioctlsocket failed with error: %ld\n", iResult);
    }

    fd_set Write, Err;
    FD_ZERO(&Write);
    FD_ZERO(&Err);
    FD_SET(sock, &Write);
    FD_SET(sock, &Err);

    // check if the socket is ready
    select(0,NULL,&Write,&Err,&Timeout);
    if(FD_ISSET(sock, &Write))
    {
        return 0;
    }

    return -2;
}
/*
 * BindSocket
 * iSocket：  套接字
 * pchIPAddr：IP地址
 * iPort：    端口号
 *
 * 将IP地址和端口号绑定到套接字上，并开始监听TCP连接。
*/
int ctcp_BindSocket(int iSocket, const char *pchIPAddr, int iPort)
{
    struct sockaddr_in struSin;

    if (BindSocketCommon(iSocket, pchIPAddr, iPort, &struSin) < 0)
    {
        return -1;
    }
    //开始监听TCP连接，如果出现错误则退出
    if (listen(iSocket, 20) < 0)
    {
        return -2;
    }
    return 0;
}

/*
 * BindUdpSocket
 * iSocket：  套接字
 * pchIPAddr：IP地址
 * iPort：    端口号
 *
 * 将IP地址和端口号绑定到套接字上。
*/
int BindSocketCommon(int iSocket, const char *pchIPAddr, int iPort, struct sockaddr_in *pstruAddr)
{
    int iOn = 1;
    struct linger sopt = {1, 0};

    pstruAddr->sin_family = AF_INET;
    //如果端口号是0，则使用INADDR_ANY
    pstruAddr->sin_port = iPort == 0 ? INADDR_ANY : htons(iPort);
    //如果IP地址为空，则使用INADDR_ANY
    pstruAddr->sin_addr.s_addr = pchIPAddr == NULL ? INADDR_ANY : inet_addr(pchIPAddr);

    //解决Address already in use错误
    setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iOn, sizeof(iOn));
    //setsockopt(iSocket, SOL_SOCKET, SO_REUSEPORT, &iOn, sizeof(iOn));
    setsockopt(iSocket, SOL_SOCKET, SO_LINGER, (char *)&sopt, sizeof(sopt));




    //绑定套接字到目标地址，如果出现错误则退出,c++11中bind和std中的冲突,必须这样才行::
    if (::bind(iSocket, (struct sockaddr *)pstruAddr, sizeof(struct sockaddr_in)) < 0)
    {
        return -1;
    }
    return 0;
}

/*
 * InitSendGroupSocket
 * 返回值：套接字，0表示初始化套接字失败
 *
 * 初始化组播发送Udp套接字。
*/
int InitSendGroupSocket(const char *pchIPAddr, const char *pchGroupIPAddr, int iGroupPort, struct sockaddr_in *pstruAddr)
{
    int iSock;
    struct sockaddr_in struSin;

    if ((iSock = InitUdpSocket(pchGroupIPAddr, iGroupPort, pstruAddr)) < 0)
    {
        return -1;
    }

    memset(&struSin, 0, sizeof(struSin));
    struSin.sin_family = AF_INET;
    struSin.sin_addr.s_addr = inet_addr(pchIPAddr);
    //c++11中bind和std中的冲突,必须这样才行::
    if (::bind(iSock, (struct sockaddr *)&struSin, sizeof(struSin)) < 0)
    {
        return -2;
    }
    return iSock;
}



int AcceptConnection(int iSocket)
{
    int iSock;
    int iLength;
    struct sockaddr_in struSin;

    iLength = sizeof(struSin);
    memset(&struSin,0, iLength);
ACCEPT_AGAIN:
    if ((iSock = accept(iSocket, (SOCKADDR *)&struSin, &iLength)) < 0)
    {
        if (errno == EINTR)
        {
            goto ACCEPT_AGAIN;
        }
        return -1;
    }
    return iSock;
}


/*
 * CloseConnection
 * iSocket：套接字
 *
 * 关闭TCP连接套接字。
*/
int ctcp_CloseConnection(int iSocket)
{
    //    HLOG_INT(shutdown(iSocket,SHUT_RDWR));
    //    HLOG("现在关闭套接字");
    //如果关闭套接字失败，则退出
    //    if ((closesocket(iSocket)) != 0)
    //    {
    //        perror("close() failed!");
    //        //        HLOG("关闭套接字失败");
    //        //printf("%s\n", strerror(errno));
    //        return -1;
    //    }
    //    HLOG_INT(shutdown(iSocket,SHUT_RDWR));
    if(shutdown(iSocket,SD_BOTH) != 0)
    {
        //        perror("close() failed!");
        //        HLOG("关闭套接字失败:%s",strerror(errno));
        //printf("%s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/*
 * SendPacket
 * iSocket： 套接字
 * pBuffer： TCP包
 * uiLength：指定发送包的长度
 * 返回值：  实际发送包的长度
 * EBADF:   第一个参数不合法
 * EFAULT:  参数中有一指针指向无法存取的内存空间
 * ENOTSOCK:第一个参数不是socket
 * EINTR:   被信号中断
 * EAGAIN:  此操作会令进程阻塞，但socket为不可阻塞
 * ENOBUFS: 系统的缓冲区内存不足
 * ENOMEM:  核心内存不足
 * EINVAL:  传给系统调用的参数不正确
 *
 * 向对方发送TCP包。
*/
int SendPacket(int iSocket, char *pBuffer, int uiLength)
{
    //assert(pBuffer != NULL);
    //assert(uiLength > 0);

    int iRet;
    //如果发送时被信号中断，则重新进行发送
    while ((iRet = send(iSocket, pBuffer, uiLength, 0)) < 0 && errno == EINTR);
    //如果实际发送包的长度小于0，说明发送时出现了错误，等于0表示对方关闭了连接
    return iRet;
}

/*
 * RecvPacket
 * iSocket： 套接字
 * pBuffer： TCP包
 * uiLength：指定接收包的长度
 * 返回值：  实际接收包的长度
 * 特别地，返回值<0时并且(errno == EINTR || errno == EAGAIN
 *       || errno == EWOULDBLOCK)的情况下认为连接是正常的，继续接收
 *
 * 从对方接收TCP包。
*/
int RecvPacket(int iSocket, char *pBuffer, int uiLength)
{
    //assert(pBuffer != NULL);
    //assert(uiLength > 0);

    int iRet;
    //如果接收时被信号中断，则重新进行接收
    while ((iRet = recv(iSocket, pBuffer, uiLength, 0)) < 0 && errno == EINTR);
    //如果实际接收包的长度小于0，说明接收时出现了错误，等于0表示对方关闭了连接
    return iRet;
}


int SendFullPacketTimeout(int iSocket, const char *pBuffer, int iLength, int timeoutSec)
{
    int iRemain;
    int iSendNow = 0;
    xctime tstart=clib_getTimeNow();
    //    //设置发送超时时间
    if(timeoutSec>0)
    {
        //这貌似和windows不宜样，这个设置并不管用，主要是下边管用
        struct timeval timeout={timeoutSec,0};//秒和微妙
        setsockopt(iSocket,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,sizeof(struct timeval));
    }
    //发送缓冲区
    int nSendBuf=666*1024;//设置为666K
    setsockopt(iSocket,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
    //    HLOG("start while");
    while (iSendNow < iLength)
    {
        //        HLOG("int while1");
        iRemain = iLength - iSendNow;
        int errwin=WSAGetLastError();
        //将那个两个错误处理去掉后,send会阻塞到接收端处理完之后再返回,改之前是非阻塞的所以第二包直接失败,所以应该改为这样
        //                while ((iRet = send(iSocket, pBuffer + iSend, iRet, 0)) < 0 && (errno == EINTR || errno ==EAGAIN))
        //                while ((iRemain = send(iSocket, pBuffer + iSendNow, iRemain, 0)) < 0)
        while ((iRemain = send(iSocket, pBuffer + iSendNow, iRemain, 0)) < 0
               && ( WSAGetLastError()== EINTR ||
                    WSAGetLastError() ==EAGAIN ||
                    WSAGetLastError()==EWOULDBLOCK))
        {
            iRemain = iLength - iSendNow;
            if(timeoutSec>0)
            {
                xctime tend=clib_getTimeNow();
                int ires=clib_getDiffBetweenXtime(tstart,tend);
                if(ires>=timeoutSec)
                {
                    hlog("#################发送超时################");
                    return -9;
                }
            }
        }
        //        iRemain = send(iSocket, pBuffer + iSendNow, iRemain, 0);
        //如果实际发送包的长度小于0，说明发送时出现了错误，等于0表示对方关闭了连接
        if (iRemain <= 0)
        {
            hlog(iRemain,iSendNow,errno,strerror(errno),WSAGetLastError());
            return iRemain;
        }
        iSendNow += iRemain;
    }
    return iSendNow;
}
int RecvFullPacketTimeout(int iSocket, char *pBuffer, int iLength,int timeoutSec)
{
    //    hlog(iLength);
    int iRemain;//剩余需要接收的字节数
    int iRecvNow = 0;//已经接收的字节数
    //    //设置接收超时时间
    if(timeoutSec>0)
    {
        //这地方和linux不一样,且秒数也不对?貌似是ms,必须*1000,反正这样是对的,此处管用
        struct timeval timeout={timeoutSec*1000,0};//秒和微妙
        setsockopt(iSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
    }
    // 接收缓冲区
    int nRecvBuf=666*1024;//设置为666K
    setsockopt(iSocket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

    while (iRecvNow < iLength)
    {
        iRemain = iLength - iRecvNow;//刚开始remain就是要接收的字节数
        //如果接收时被信号中断，则重新进行接收
        //碰到这几种errno，即使接收失败也不停止，继续接收
        int errwin=WSAGetLastError();
        while ((iRemain = recv(iSocket, pBuffer + iRecvNow, iRemain, MSG_WAITALL))<0
               && ( WSAGetLastError()== EINTR ||
                    WSAGetLastError() ==EAGAIN ||
                    WSAGetLastError()==EWOULDBLOCK))
            //                    while ((iRemain = recv(iSocket, pBuffer + iRecvNow, iRemain, MSG_WAITALL)) < 0)
        {
            //WriteLog(TRACE_NORMAL,"iRet:%d, errno:%d %d %d, %s",iRet, errno, EINTR , EAGAIN, strerror(errno));
            //            HLOG("iRet:%d, errno:%d %d %d, %s",iRet, errno, EINTR , EAGAIN, strerror(errno));
            iRemain = iLength - iRecvNow;
        }

        //        iRemain = recv(iSocket, pBuffer + iRecvNow, iRemain, MSG_WAITALL);
        //如果实际接收包的长度小于0，说明接收时出现了错误，等于0表示对方关闭了连接
        if (iRemain <= 0)
        {
            //            hlog(iRemain,iRecvNow,errno,WSAGetLastError());
            if(iRecvNow>0)
            {
                return iRecvNow;
            }
            else
            {
                return iRemain-1;
            }
        }
        iRecvNow += iRemain;
    }
    return iRecvNow;
}
int InitUdpSocket(const char *pchIPAddr, int iPort, struct sockaddr_in *pstruAddr)
{
    int iSock = -1;
    int iOn = 1;

    //如果套接字初始化失败，则返回-1
    if ((iSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return -2;
    }

    pstruAddr->sin_family = AF_INET;
    //如果端口号是0，则使用INADDR_ANY
    pstruAddr->sin_port = iPort == 0 ? INADDR_ANY : htons(iPort);
    //如果IP地址为空，则使用INADDR_ANY
    pstruAddr->sin_addr.s_addr = pchIPAddr == NULL ? INADDR_ANY : inet_addr(pchIPAddr);

    //解决Address already in use错误
    setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, (char*)&iOn, sizeof(iOn));
    //    HLOG_INT(iSock);
    return iSock;
}






cfile *cfileInit(const char *acPathFile,enum typexfile type)
{
    cfile* file=HMALLOC(cfile,1);


    strcpy(file->acPathFile,acPathFile);
    FILE* fp=NULL;
    //将路径提取出来
    char acpath[2048];
    memset(acpath,0,2048);
    //    clib clib=clibInit();
    //    clib.getPathFromPathAndName(acPathFile,acpath);

    strcpy(acpath,plib::getPathFromFullPath(acPathFile).c_str());
    //    HLOG_STRING(acpath);
    if(type==typesave)
    {
        //如果路径不存再，就递归创建
        if(access(acpath,0)<0)
            plib::mkdirp(string(acpath));
        //            clib.createDir(acpath);
        fp=fopen(file->acPathFile,"wb+");
    }
    else if(type==typeappend)
    {
        //如果路径不存再，就递归创建
        if(access(acpath,0)<0)
            plib::mkdirp(string(acpath));
        //            clib.createDir(acpath);
        fp=fopen(file->acPathFile,"ab+");
    }
    else if(type==typeload)
    {
        if(access(acPathFile,0)<0)
        {
            string strinfo=string(acPathFile)+"不存在";
            hlog(strinfo);
            free(file);
            return NULL;
        }
        fp=fopen(file->acPathFile,"rb");
    }
    if(fp==NULL)
        return NULL;
    file->fp=fp;

    file->destroy= &cfileDestroy;
    file->save= &cfileSave;
    file->load= &cfileLoad;
    file->move=cfilemove;
    return file;
}

//该函数只返回>=0,存储错误是=0;
int cfileSave(cfile *file, void *data, int len)
{
    return fwrite(data,1,len,file->fp);
}


int cfileDestroy(cfile *file)
{
    fclose(file->fp);
    free(file);
    file=NULL;
    return 0;
}

//该函数只返回>=0,读不到是=0;
int cfileLoad(cfile *file, void *data, int len)
{
    return fread(data,1,len,file->fp);
}
//fseek最大支持2G,大文件不可以
int cfilemove(cfile *file, unsigned long long pos)
{
    //windows下要用这个
    return _fseeki64(file->fp, pos, SEEK_CUR);
}


pfile::pfile(string strFullPath, enum typexfile type)
{
    pf=cfileInit(strFullPath.c_str(),type);
    this->strpath=strFullPath;
    this->sizeFile=plib::getFileSize(this->strpath);
    if(type==typeload)
        this->strtype="load";
    if(type==typeappend)
        this->strtype="append";
    if(type==typesave)
        this->strtype="save";
}

pfile::~pfile()
{
    free(pf);
    pf=NULL;
}
//>0才有效,<0是错误
int pfile::load(char *data, int len)
{
    int ires=cfileLoad(pf,data,len);
    if(ires==0)
        return -1;
    return ires;
}

int pfile::loadAll()
{
    this->dataAll=HMALLOC(char,this->sizeFile);
    return cfileLoad(pf,this->dataAll,this->sizeFile);
}
//该函数只返回>=0,存储错误是=0;
int pfile::save(const char *data, int len)
{
    int ires= cfileSave(pf,(void*)data,len);
    if(ires==0)
        return -1;
    return ires;
}
//<0是错误,fseek是最大支持2G,所以大于2G不支持
int pfile::move(unsigned long long pos)
{
    return cfilemove(pf,pos);
}

unsigned long long pfile::size()
{
    return pf->size(pf);
}

int pfile::close()
{
    return fclose(pf->fp);
}



plist<pstring> plib::getAllFilesRecurion(string path)
{
    vector<string> lstr;
    common_getAllFilesRecurion(path,lstr);
    plist<pstring> lres;
    for(int i=0;i<lstr.size();i++)
        lres.append(lstr[i]);
    return lres;
}

pstring plib::pwd()
{
    char buf[1024];
    getcwd(buf, sizeof(buf));
    return string(buf);
}


int plib::setPathConf(string strnewpath)
{

    strPathConf=strnewpath;
    return 0;
}



string plib::getConfString(string section, string key)
{
    if(!plib::isExsistFile(strPathConf))
    {
        hlog(pstring()<<"找不到配置文件"<<strPathConf);
        return "";
    }
    return readConfig(strPathConf,section,key);
}

bool plib::delConfString(string section, string key)
{
    hlog(strPathConf);
    if(!plib::isExsistFile(strPathConf))
    {
        hlog(pstring()<<"找不到配置文件"<<strPathConf);
        return false;
    }
    //先找section,如果找不到section,就不用加了,不然会一直找不到一直加
    pstring strall = plib::getContentsAllFromFile(strPathConf);
    //    hlog(strContentAll);
    if (strall == "")
        return false;
    //section [basic]
    pstring strSectionToAdd = "[" + section + "]";
    hlog(strall.contain(strSectionToAdd));
    //如果找不到section,删除失败
    if (!strall.contain(strSectionToAdd)) {
        hlog(pstring() << "找不到该section" << strSectionToAdd << "删除失败");
        return false;
    } else {
        //找到了这个section,再找key
        //要根据所有section分组
        //先找key,整个文件找如果没有key,删除失败
        if (!strall.contain(key)) {
            hlog(pstring() << "没有找到该key" << key << "删除失败");
            return false;
        } else {
            //找到所有key,还要看key是不是这个section的,这里用一个偷懒的办法,从key的
            //位置从后往前找section,找到的section匹配则说明是要找的,否则就不是这个section的
            //也是删除失败

            plist<int> listIndexFind = strall.findIndexAll(key);//所有key所在索引
            //            //对所有行数遍历,从行数到上一个函数倒叙,看section是否是要找的
            int iIndexFindFinal = -1;
            for (int i = 0; i < listIndexFind.size(); i++) {
                int iLineNow = listIndexFind[i];
                int iLineLast;
                if (i == 0)
                    iLineLast = 0;
                else
                    iLineLast = listIndexFind[i - 1];
                //一段一段查
                int ilineFind = -1;
                //这个就是两个key之间的字符串
                pstring strlinei = strall.substr(iLineLast, iLineNow);
                //如果两个key之间包括section,那么找到了
                if (strlinei.contain("[" + section + "]")) {
                    iIndexFindFinal = iLineNow;
                    break;
                }
            }
            if (iIndexFindFinal < 0) {
                hlog(pstring() << "没找到该section:" << section << "下的key:" << key << "所在的行,删除失败");
                return false;
            } else {
                //找到这一行要删除的,删除这一行覆盖就ok
                hlog(iIndexFindFinal);
                hlog(strall.size());
                //                hlog(strall.find(key));
                //删除位置是从这个index开始,到换行符所在
                int indexFindEnd = strall.find("\n", iIndexFindFinal);
                //如果没找到,说明这个就是最后一行,直接去掉就行
                hlog(indexFindEnd == string::npos);
                if (indexFindEnd == string::npos) {
                    strall = strall.substr(0, iIndexFindFinal);
                    hlog(plib::overrideToFile(strPathConf, strall));
                    return true;
                } else {
                    //如果找到了,说明后边还有,就删除这一段就ok
                    hlog(iIndexFindFinal, indexFindEnd, strall.size());
                    (strall.erase(iIndexFindFinal, indexFindEnd - iIndexFindFinal + 1));
                    hlog(plib::overrideToFile(strPathConf, strall));
                    return true;
                }

            }
        }

    };
}

bool plib::addConfString(string section,string key,string value)
{
    if(!plib::isExsistFile(strPathConf))
    {
        hlog(pstring()<<"找不到配置文件"<<strPathConf);
        return false;
    }
    //先找section,如果找不到section,就不用加了,不然会一直找不到一直加
    pstring strContentAll=plib::getContentsAllFromFile(strPathConf);
    //    hlog(strContentAll);
    if(strContentAll=="")
        return false;
    //section [basic]
    pstring strSectionToAdd="["+section+"]";
    pstring strConfToAdd=key+"="+value;
    hlog(strSectionToAdd,strConfToAdd);
    hlog(strContentAll.contain(strSectionToAdd));
    //如果找不到section,最下边新家section,再新加配置
    if(!strContentAll.contain(strSectionToAdd))
    {
        hlog(strSectionToAdd);
        if(!plib::appendToFileOneLine(strPathConf,strSectionToAdd))
        {
            hlog("追加section失败");
            return false;
        }

        if(!plib::appendToFileOneLine(strPathConf,strConfToAdd))
        {
            hlog("追加配置失败");
            return false;
        } else{
            return true;
        }
    } else{//如果找到了section,那么先看这个section下有没有,如果有,就不增加,

        if(plib::getConfString(section,key)!="")
        {
            hlog(pstring()<<"已经有该配置"<<key<<",无法增加,增加配置失败");
            return false;
        }
        else {// 如果没有则在section下第一行增加
            //先找section位置
            hlog(strSectionToAdd);
            int index=strContentAll.find(strSectionToAdd);
            hlog(index);
            return plib::insertStringToFile(strPathConf,"\n"+strConfToAdd,strSectionToAdd);
        }
    }
}
int plib::initLog(string strnewpath)
{
    //    //先获取纯路径
    //    string strpathonly;
    //    vector<string> vstr=split(strnewpath,"/");
    //    for(int i=0;i<vstr.size()-1;i++)
    //    {
    //        strpathonly+=vstr[i]+"/";
    //    }
    //    hlog(strpathonly);

    //    if(!isExsistDir(strpathonly))
    //    {
    //        if(!createDir(strpathonly.c_str()))
    //        {
    //            hlog("日志路径不存在，且创建失败，程序退出");
    //            return -2;
    //        }
    //        hlog("日志路径不存在，且已经递归创建成功");
    //    }
    strPathLog=strnewpath;
    return 0;
}


bool plib::isContaiAlpha(string str)
{
    for(int i=0;i<str.size();i++)
    {
        if(isalpha(str.at(i)))
            return true;
    }
    return false;
}

string  plib::hostToIp(const char *url)
{
    char* ip;
    struct hostent *host;
    WSADATA wsaData;
    int ret;
    ret = WSAStartup(0x0202, &wsaData);
    if(ret) {
        printf("error in WSAStartup: %d\n", WSAGetLastError());
        return 0;
    }

    host = gethostbyname(url);
    if(host == NULL) {

        printf("error in gethostbyname: %d\n", WSAGetLastError());
        return "";
    } else {
        printf("name: %s\naddrtype; %d\naddrlength: %d\n",
               host->h_name, host->h_addrtype, host->h_length);
        printf("ip address: %s\n",
               ip=inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
    }
    WSACleanup();
    return string(ip);
}

void plib::loginVnc(pstring strFullPathVncExe, pstring strIpWithPort, pstring strpwd)
{
    if(!plib::isExsistFile(strFullPathVncExe))
    {
        hlog(pstring()<<"can not open "<<strFullPathVncExe);
        return;
    }
    hlog(strFullPathVncExe.c_str(),strIpWithPort.c_str(),strpwd.c_str());
    CString strExe=CString(strFullPathVncExe.c_str());
    CString strIPPort=CString(strIpWithPort.c_str());
    CString strPwd=CString(strpwd.c_str());

    loginVncCString(strExe,strIPPort,strPwd);
}
void plib::loginVncCString(CString vnc, CString ip, CString pwd)
{

    ShellExecute(::GetDesktopWindow(),_T("open"),vnc,NULL,NULL,SW_NORMAL);
    HWND h=NULL;
    HWND hip=NULL;
    while(h==NULL)
        h=::FindWindowA(NULL,"VNC Viewer : Connection Details");
    while(hip==NULL)
        hip=::FindWindowExA(h,NULL,"ComboBox",NULL);


    ::SendMessage(hip, CB_RESETCONTENT, NULL, NULL);
    //ip
    sendInputString(hip,plib::CStringToString(ip));
    //ok
    HWND hok=::FindWindowExA(h,NULL,"Button","OK");

    sendAnjian(hok,VK_SPACE);


    HWND hpwd=NULL;
    while(hpwd==NULL)
        hpwd=::FindWindowA(NULL,"VNC Viewer : Authentication [No Encryption]");

    HWND hedituser=NULL;
    while(hedituser==NULL)
        hedituser=::FindWindowExA(hpwd,NULL,"Edit",NULL);

    HWND heditpwd=::FindWindowExA(hpwd,hedituser,"Edit",NULL);
    sendInputString(heditpwd,plib::CStringToString(pwd));

    HWND hok2=::FindWindowExA(hpwd,NULL,"Button","OK");
    sendAnjian(hok2,VK_SPACE);
}

void plib::loginSsh(pstring strFullPathPuttyExe, pstring strip, pstring strpwd)
{
    if(!plib::isExsistFile(strFullPathPuttyExe))
    {
        hlog(strFullPathPuttyExe,"is no valid");
        return;
    }
    hlog(strip);
    //    [strPara :  -pw scfwq_325 root@172.16.11.3]
    pstring strPara=" -pw "+strpwd+" root@"+strip;
    hlog(strPara);
    CString strFulPath=plib::StringToCString(strFullPathPuttyExe);
    CString cstrPara=plib::StringToCString(strPara);
    ShellExecute(::GetDesktopWindow(),_T("open"),strFulPath,cstrPara,NULL,SW_NORMAL);


    //不用点了,c#那个县城自动点了,有需要再打开
    //    HWND hwarn=NULL;
    //    while(hwarn==NULL)
    //        hwarn=::FindWindowA(NULL,"PuTTY 安全警告");
    //    //    hlog("find");
    //    HWND hfou=NULL;
    //    while(hfou==NULL)
    //        hfou=::FindWindowExA(hwarn,NULL,"Button","否(&N)");
    //    //    hlog("find2");
    //    //每次都点击否
    //    plib::sendButtonClick(hfou);
}


string plib::getTimeStringFromCTime(CTime time)
{
    stringstream os;
    os<<time.GetYear()<<"-"<<time.GetMonth()<<"-"<<time.GetDay()<<" "<<time.GetHour()<<":"<<time.GetMinute()<<":"<<time.GetSecond();
    return os.str();
}
CTime plib::toCTime(string str)
{
    vector<string> split=plib::split(str,"-");
    //    for(int i=0;i<split.size();i++)
    //    {
    //        cout<<split[i]<<endl;
    //    }
    CTime time(atoi(split[0].c_str()),atoi(split[1].c_str()),atoi(split[2].c_str()),atoi(split[3].c_str()),atoi(split[4].c_str()),atoi(split[5].c_str()));
    return time;
}
string plib::toString(CTime time)
{
    ostringstream os;
    os<<time.GetYear()<<"-"<<time.GetMonth()<<"-"<<time.GetDay()<<"-"<<time.GetHour()<<"-"<<time.GetMinute()<<"-"<<time.GetSecond();
    string str=os.str();
    return str;
}


void plib::postInputString(HWND hwnd, string str)
{
    CString stres=plib::StringToCString(str);
    for(int i=0 ; i<stres.GetLength() ; i++)
    {
        ::PostMessage(hwnd,WM_CHAR,stres[i] & 0xFF,0);
    }
}
void plib::postRightClick(HWND hwnd, int x, int y)
{
    ::PostMessage(hwnd, WM_RBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x,y));
    ::PostMessage(hwnd, WM_RBUTTONUP, 0, MAKELPARAM(x,y));
}
void plib::postLeftClick(HWND hwnd, int x, int y)
{
    ::PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x,y));
    ::PostMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(x,y));
}
void plib::postMouseMove(HWND hwnd,int x,int y)
{
    ::PostMessage(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(x,y));
}

void plib::delay(int wInterval)
{
    LARGE_INTEGER litmp;
    LONGLONG QPart1,QPart2;
    double dfMinus, dfFreq, dfTim;
    QueryPerformanceFrequency(&litmp);
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);
    QPart1 = litmp.QuadPart;
    do
    {
        QueryPerformanceCounter(&litmp);
        QPart2 = litmp.QuadPart;
        dfMinus = (double)(QPart2-QPart1);
        dfTim = dfMinus / dfFreq;
    }while(dfTim<0.001*wInterval);
}
void plib::postAnjian(HWND hwnd, int jianma)
{
    ::PostMessage(hwnd,WM_KEYDOWN,jianma,0);
    ::PostMessage(hwnd,WM_KEYUP,jianma,0);
}
void plib::sendInputString(HWND hwnd, string str)
{
    CString stres=plib::StringToCString(str);
    for(int i=0 ; i<stres.GetLength() ; i++)
    {
        ::SendMessage(hwnd,WM_CHAR,stres[i] & 0xFF,0);
    }
}
void plib::sendRightClick(HWND hwnd, int x, int y)
{
    ::SendMessage(hwnd, WM_RBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x,y));
    ::SendMessage(hwnd, WM_RBUTTONUP, 0, MAKELPARAM(x,y));
}
void plib::sendLeftClick(HWND hwnd, int x, int y)
{
    ::SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x,y));
    ::SendMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(x,y));
}

void plib::sendAnjian(HWND hwnd, int jianma)
{
    ::SendMessage(hwnd,WM_KEYDOWN,jianma,0);
    ::SendMessage(hwnd,WM_KEYUP,jianma,0);
}

void plib::sendButtonClick(HWND h)
{
    ::SendMessageA(h,BM_CLICK,NULL,NULL);
}

int plib::getDigit(int i)
{
    int k=0;
    while(i)
    {
        i=i/10;
        k++;
    }
    return k;
}

string plib::getStringFromFile(string path)
{

    std::ifstream t(path.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string contents(buffer.str());
    t.close();
    return contents;
}

bool plib::insertStringToFile(string path, string content, string keyFind)
{
    pstring strAll=plib::getContentsAllFromFile(path);
    int ifind=strAll.find(keyFind)+keyFind.size();
    if(ifind==string::npos)
    {
        hlog(pstring()<<"未找到该字符串"<<content<<"插入失败");
        return false;
    }
    pstring strfront=strAll.substr(0,ifind);
    pstring strbehind=strAll.substr(ifind,strAll.size());
    pstring strAllNew=strfront+content+strbehind;
    return plib::overrideToFile(path,strAllNew);
}

bool plib::appendToFileOneLine(string path, string content)
{
    ofstream os;
    os.open(path.c_str(),ios::app);
    if(!os)
    {
        string info="Open file fail "+path;
        hlog(info);
        return false;
    }
    os<<endl<<content;
    os.close();
    return true;
}

bool plib::overrideToFile(string path, string content)
{
    ofstream os;
    os.open(path.c_str());
    os<<content;
    os.close();
    return true;
}

int plib::getNowYear()
{
    time_t timep;
    struct tm *p;
    time(&timep);
    p=gmtime(&timep);
    //    printf("%d%d%d",(1900+p->tm_year), (1+p->tm_mon),p->tm_mday);
    //    printf("%s%d;%d;%d\n", wday[p->tm_wday], p->tm_hour, p->tm_min, p->tm_sec);
    int year=1900+p->tm_year;
    //    XCLOG_INT(year);
    return year;
}

void plib::writeStringToFile(string path, string str)
{
    std::ofstream out(path.c_str());
    out<<str;
    out.close();
}


unsigned long plib::getULNetTime2()
{
    int nRet;

    /* Initialize Winsock */
    WORD wVersionRequested;
    WSADATA wsaData;
    int nErrCode;

    wVersionRequested = MAKEWORD(2, 2);
    nErrCode = WSAStartup(wVersionRequested, &wsaData);
    if (0 != nErrCode)
    {
        return 0;
    }

    /* Get server IP */
    struct hostent *host;
    char *pServerIP;

    host = gethostbyname("time.nist.gov");
    if (NULL == host)
    {
        return 0;
    }

    pServerIP = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);

    /* Connect to time server, and get time */
    SOCKET sockfd;

    //    char cTimeBuf[40] = { 0 };
    unsigned long ulTime = 0;
    int nTry = 0;
    do
    {
        if (5 == nTry++)
        {
            return 0;
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (INVALID_SOCKET == sockfd)
        {
            continue;
        }

        int TimeOut = 3000;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut));

        sockaddr_in    addr;

        memset(&addr, 0, sizeof(addr));
        addr.sin_family         = AF_INET;
        addr.sin_port           = htons(37);
        addr.sin_addr.s_addr    = inet_addr(pServerIP);

        nRet = connect(sockfd, (sockaddr *)&addr, sizeof(addr));
        if (SOCKET_ERROR == nRet)
        {
            continue;
        }

        nRet = recv(sockfd, (char *)&ulTime, sizeof(ulTime), 0);
        if ((SOCKET_ERROR != nRet) && (0 != nRet))
        {
            break;
        }

        //        int nErr = WSAGetLastError();
        //        printf("error %d\n",nErr);
        //        printf("[%d]%s", nErr, ConvertErrcodeToString(nErr));

        closesocket(sockfd);
    } while (1);

    closesocket(sockfd);

    unsigned long ulTimehl = ntohl(ulTime);
    return ulTimehl;
}

CTime plib::getCTimeNetTime()
{
    unsigned long ulTime=getULNetTime2();
    if (ulTime > 0)
    {

        FILETIME ft;
        SYSTEMTIME st;


        st.wYear = 1900;
        st.wMonth = 1;
        st.wDay = 1;

        st.wHour = 8;
        st.wMinute = 0;
        st.wSecond = 0;
        st.wMilliseconds = 0;

        SystemTimeToFileTime(&st, &ft);


        LONGLONG *pLLong = (LONGLONG *)&ft;


        *pLLong += (LONGLONG) 10000000 * ulTime;


        FileTimeToSystemTime(&ft, &st);
        CTime temp(st.wYear,st.wMonth,st.wDay,
                   st.wHour,st.wMinute,st.wSecond);
        return temp;
    }
    else
    {
        CTime temp(2222,11,30,23,59,59);
        return temp;
    }
}

bool plib::openExe(string strFullPath, string strPara)
{
    CString cstrFullPath=plib::StringToCString(strFullPath);
    if(strPara=="")
    {
        return ShellExecute(::GetDesktopWindow(),_T("open"),cstrFullPath,NULL,NULL,SW_SHOWNA);
    }
    else
    {
        CString cstrPara=plib::StringToCString(strPara);
        return ShellExecute(::GetDesktopWindow(),_T("open"),cstrFullPath,cstrPara,NULL,SW_NORMAL);
    }
}

void plib::closeProcessbyHandle(HWND h)
{
    ::SendMessage(h, WM_CLOSE, 0, 0);
}

COLORREF plib::getColorFromHWND(HWND h, int x, int y)
{
    HDC hDC = ::GetWindowDC(h); // 窗口的DC
    //        POINT pt; // 给定的坐标
    COLORREF color; // 存储颜色
    //        pt.x =x;
    //        pt.y = y;
    //        ::ScreenToClient(h, &pt);
    //    hlog(pt.x,pt.y);
    //        return pt;
    color = ::GetPixel(hDC, x, y);
    return color;
}

SIZE plib::getSizeFromHWND(HWND h)
{
    RECT rctA; //定义一个RECT结构体，存储窗口的长宽高
    ::GetWindowRect(h,&rctA);//通过窗口句柄获得窗口的大小存储在rctA结构中
    int width = rctA.right - rctA.left;   //窗口的宽度
    int height = rctA.bottom - rctA.top;  //窗口的高度
    //    hlog(width);
    //    hlog(height);
    SIZE sz;
    sz.cx=width;
    sz.cy=height;
    return sz;
}

POINT plib::getPointClientFromScreen(HWND h, int x,int y)
{
    //    HWND hWnd=plib::findWindow("Notepad",NULL);
    //    hlog(plib::getContent(hWnd));
    //有另一个::GetDC
    //    HDC hDC = ::GetWindowDC(hWnd); // 窗口的DC
    POINT pt; // 给定的坐标
    //    COLORREF color; // 存储颜色
    pt.x =x;
    pt.y = y;
    ::ScreenToClient(h, &pt);
    //    hlog(pt.x,pt.y);
    return pt;
    //    color = ::GetPixel(hDC, pt.x, pt.y);
}


HWND plib::findWindow(const char *arrchClass,const char *arrchTitle)
{
    return ::FindWindowA(arrchClass,arrchTitle);
}

HWND plib::findWindowEx(HWND hparent, HWND hchildafter, const char *arrchClass, const char *arrchTitle)
{
    return ::FindWindowExA(hparent,hchildafter,arrchClass,arrchTitle);
}

string plib::getContentByHWND(HWND h)
{
    char arrchTitle[MAX_PATH] = { 0 };
    //利用window消息机制也是可以的
    SendMessageA(h, WM_GETTEXT, MAX_PATH, (LPARAM)arrchTitle);
    //                 GetWindowTextA(h3, wszTitle, MAX_PATH);
    //                        ::GetClassNameA(h3,(LPSTR)str,100);
    //                        GetWindowText(h3, (LPWSTR)str, sizeof(str));   //获取控件的TEXT
    //                        ::SendMessage(h3, WM_GETTEXT, 100, (LPARAM)str);
    //                ::GetWindowText(h3,(LPWSTR)str,100);
    //第三个和第四个参数是缓存
    //    hlog(arrchTitle);
    return string(arrchTitle);
}



void plib::messagebox(string str)
{
    //                CString stres="xlfds";
    ::MessageBox(NULL,(plib::StringToCString(str)),L"信息提示",0);
    \
}




bool plib::isExpire(int year, int month, int day, int hour, int minite, int second)
{
    CTime nowtime=getCTimeNetTime();
    cout<<nowtime.GetYear()<<"-"<<nowtime.GetMonth()<<"-"
       <<nowtime.GetDay()<<" "<<nowtime.GetHour()<<":"
      <<nowtime.GetMinute()<<":"<<nowtime.GetSecond();
    //    CString temp;temp.Format(L"%d:%d:%d   %d:%d:%d",nowtime.GetYear(),nowtime.GetMonth(),nowtime.GetDay(),nowtime.GetHour(),nowtime.GetMinute(),nowtime.GetSecond());
    CTime settime=CTime(year,month,day,hour,minite,second);
    if(nowtime>settime)
        return true;
    else
        return false;
}


vector<string> plib::split(string str, string pattern)
{
    string::size_type pos;
    vector<string> result;
    str+=pattern;
    int size=str.size();
    for(int i=0;i<size;i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

bool plib::isExsistFile(string strpath)
{
    fstream _file;
    _file.open(strpath,ios::in);
    if(!_file)
        return false;
    else
        return true;
}

bool plib::isExsistDir(string strpath)
{

    if(access(strpath.c_str(),0)<0)
        return false;
    else
        return true;
}







bool pstring::isNum()
{
    stringstream sin(*this);
    double d;
    char c;
    if(!(sin >> d))
    {
        return false;
    }
    if (sin >> c)
    {
        return false;
    }
    return true;

}









//接收一次后使用，根据使用的结构体转换
const char *ptcp::getData()
{
    //    return (char*)(&(*(this->data.begin())));
    return this->data.c_str();
}



ptcp::ptcp(int iSockAccept, string ip, int port)
{
    this->sock=iSockAccept;
    this->init(ip,port);
    this->strCtcptype="server";
}


ptcp::~ptcp()
{

}

int ptcp::init(string ip, int port)
{
    this->ip=ip;
    this->port=port;
    this->strCtcptype="init";
    return 0;
}



//默认超时时间1秒
int ptcp::connect()
{
    this->strCtcptype="client";
    int iSock=ctcp_InitTcpSocket();
    if(iSock<0)
    {
        hlog("iSock<0, connect fail");
        ctcp_CloseConnection(iSock);
        return -1;
    }

    string strRealIP;
    //通过ip中是否有字母来判断是否是域名
    if(plib::isContaiAlpha(this->ip))
        strRealIP=plib::hostToIp(this->ip.c_str());
    else
        strRealIP=this->ip;

    //默认超时时间1秒
    if(CreateConnectionTimeout(iSock,strRealIP.c_str(),port)!=0)
        //    if(CreateConnection(iSock,strRealIP.c_str(),port)!=0)
    {
        //        hlog("CreateConnection fail, connect fail");
        return -2;
    }

    this->sock=iSock;


    return this->sock;
}

int ptcp::close()
{
    return ctcp_CloseConnection(this->sock);
    //shutrdwr双向关闭
    //    return shutdown(this->sock,SHUT_RDWR);
}

plist<ptcp *> ptcp::getAllClients()
{
    return this->listclient;
}

void ptcp::deleteClientByIndex(int i)
{
    this->listclient.deleteByIndex(i);
}








typedef struct STRUCT_PARA
{
    int itype;//server类型,是int类型还是string 0标识string，1标识int,2是nopack
    ptcp* ptcpClient;
    ptcp* ptcpOriginal;
    void (*thread_user)(ptcp* para);
}STRUCT_PARA;


typedef struct STRUCT_AUXTCP_PARA
{
    void (*thread_user)(class ptcp* para);
    int itype;
    ptcp* pOriginal;
}STRUCT_AUXTCP_PARA;

void * threadTcpServerNoDetach(void* para)
{
    STRUCT_AUXTCP_PARA* psp=(STRUCT_AUXTCP_PARA*)para;
    //    hlog(psp->itype);
    //    hlog(psp->pOriginal->ip);
    //    psp->pOriginal->setThreadServer(psp->thread_user,psp->itype);
    psp->pOriginal->setThreadServer(psp->thread_user,psp->itype);
    delete psp;
    return NULL;
}
//设置多线程server，已封装内部释放等，直接获取xptcp指针,供服务端使用
//要增加，定时与客户端通信，发送探测包，如果发送失败则关闭该连接，并从统计的连接中去掉
//使用时，函数中不用再写while循环，已经封装了，直接recvp判断类型就可以了
int ptcp::setThreadServerDetach(void (*fun_thread)(ptcp *), int type)
{
    STRUCT_AUXTCP_PARA *psp=new STRUCT_AUXTCP_PARA;
    psp->itype=type;
    psp->thread_user=fun_thread;
    psp->pOriginal=this;

    std::thread thServer(threadTcpServerNoDetach,psp);
    thServer.detach();
    return 0;
}

void* thread_process_para(void* para)
{
    STRUCT_PARA* pmix=(STRUCT_PARA*)para;
    //    hlog(pmix->itype);

    hlog(pstring()<<"####### 新增了客户端 "<<pmix->ptcpClient->ip<<" 线程id:"<<(plib::getTid())<<",当前客户端个数:"<<pmix->ptcpOriginal->getAllClients().size()<<" #########");
    //    hlog(*(pmix->ptcpOriginal));
    //    hlog(pmix->ptcpOriginal->listclient.size());

    //没有封装int类型或者string类型，纯发送接收，适合有固定接口的对外项目
    if(pmix->itype==2)
    {
        pmix->thread_user(pmix->ptcpClient);
    }
    else
    {
        while(true)
        {//执行子函数
            if(pmix->itype==0)
            {
                if(pmix->ptcpClient->recvx()<0)
                    break;
                pmix->thread_user(pmix->ptcpClient);
            }
        }
    }

    string strIpExit;
    //删除列表中的客户端
    for(int i=0;i<pmix->ptcpOriginal->listclient.size();i++)
    {
        //找到然后删除
        ptcp* pti=pmix->ptcpOriginal->listclient[i];
        if(pti->sock==pmix->ptcpClient->sock)
        {
            strIpExit=pti->ip;
            mut.lock();
            //如果提取出来再删除就没用，必须得内部删除，一定要注意
            pmix->ptcpOriginal->deleteClientByIndex(i);
            mut.unlock();
            break;
        }
    }
    hlog(pstring()<<"####### 客户端 "<<strIpExit<<" 退出，线程id:"<<(plib::getTid())<<",当前客户端个数:"<<pmix->ptcpOriginal->getAllClients().size()<<" #########");
    //关闭这个socket
    pmix->ptcpClient->close();
    delete (pmix->ptcpClient);
    delete (pmix);
    return NULL;
}

int ptcp::setThreadServer(void (*fun_thread)(ptcp *), int type)
{
    int iSock=ctcp_InitTcpSocket();
    if(iSock<0)
    {
        hlog("iSock<0");
        ctcp_CloseConnection(iSock);
        return -1;
    }

    int ibind=ctcp_BindSocket(iSock,this->ip.c_str(),port);
    if(ibind<0)
    {
        hlog("bind fail");
        hlog(strerror(errno));
        ctcp_CloseConnection(iSock);
        return -2;
    }
    this->sock=iSock;
    this->strCtcptype="server";
    //不停等待，来一个，创建一个线程
    while(true)
    {
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
        //        hlog(clientip);

        //这个地方必须要new，不然传参之后会被释放，覆盖,在线程中释放去
        STRUCT_PARA *sPara=new STRUCT_PARA;
        memset(sPara,0,sizeof(STRUCT_PARA));
        sPara->ptcpClient=new ptcp(iClientSock,clientip,this->port);

        sPara->ptcpOriginal=this;
        sPara->thread_user=fun_thread;
        sPara->itype=type;

        mut.lock();
        listclient.append((sPara->ptcpClient));
        mut.unlock();
        //        HANDLE handle = CreateThread(NULL, 0, thread_process, (LPVOID)sPara, 0, NULL);
        std::thread thServer(thread_process_para,sPara);
        thServer.detach();
    }
}







bool ptcp::operator<(const ptcp &tcp) const
{
    return this->sock<tcp.sock;
}




int ptcp::reconnect(int iretrycount, int itimesecond)
{
    //先判断是否在重连,如果在重连,说明别的线程在连,就不重连了
    if(this->bIsConnecting)
    {
        hlog("已经有线程在重连,无需重连");
        return -78;
    }
    //如果没有再重连,则置为true标示我在重连
    mut.lock();;
    this->bIsConnecting=true;
    mut.unlock();;
    string strRealIP;
    //若是域名,则先解析
    if(plib::isContaiAlpha(this->ip))
    {
        int count=0;
        while((strRealIP=plib::hostToIp(this->ip.c_str()))=="")
        {
            if(count==iretrycount)
            {
                hlog("超过尝试次数,重连失败");
                return -1;
            }
            hlog("远程服务器域名解析错误,正在重试中");
            Sleep(itimesecond*1000);
            count++;
        }
    }
    else
        strRealIP=this->ip;

    int iSocketReconnect=-4;
    iSocketReconnect=reconnectTcp(this->sock,strRealIP.c_str(),this->port,iretrycount,itimesecond);
    if(iSocketReconnect<0)
        return -1;
    this->sock=iSocketReconnect;
    mut.lock();;
    this->bIsConnecting=false;
    mut.unlock();;
    return this->sock;
}


















bool Expire::createLicense()
{
    //第一次的时候要用这些新建文件,然后有dat之后就注释掉,用最后一句正常运行
    if(plib::isExsistFile(this->strpath))
    {
        hlog(this->strpath);
        hlog("########### there is already one license dat ,no create #########");
        return false;
    }
    else
    {
        STRUCT_TIME stime;
        memset(&stime,0,sizeof(STRUCT_TIME));

        CTime timelast=CTime::GetCurrentTime();
        CTime timenow=CTime::GetCurrentTime();
        string strlast=plib::toString(timelast);
        string strnow=plib::toString(timenow);

        hlog(strlast);
        hlog(strnow);


        stime.timeInit=CTime::GetCurrentTime();
        stime.timeLast=CTime::GetCurrentTime();
        stime.isExpire=0;

        plib::save(this->strpath,stime);
        hlog("########### create license successfully #########");
        return true;
    }
}

bool Expire::isExpire()
{
    bool res=false;
    ifstream rfile(this->strpath);

    if(rfile)
    {

        STRUCT_TIME stime;
        memset(&stime,0,sizeof(STRUCT_TIME));
        stime=plib::load<STRUCT_TIME>(this->strpath);
        if(stime.isExpire==1)
        {
            hlog("############# file has record expire! exit ############");
            res=true;
        }

        CTime now=CTime::GetCurrentTime();
        //        plib::showTime(stime.timeInit);
        //        plib::showTime(stime.timeLast);
        hlog(plib::getTimeStringFromCTime(stime.timeInit));
        hlog(plib::getTimeStringFromCTime(stime.timeLast));
        //        cout<<"isExpire: "<<stime.isExpire<<endl;
        hlog(stime.isExpire);
        //        plib::showTime(now);
        hlog(plib::getTimeStringFromCTime(now));


        CTimeSpan tspan=now-stime.timeInit;
        long long spanFromInitTime=tspan.GetTimeSpan();
        //        cout<<"span from init: "<<span<<endl;
        hlog(spanFromInitTime);

        if(spanFromInitTime>this->expireSeconds)
        {
            //            cout<<"normal expire!"<<endl;
            hlog(this->expireSeconds);
            hlog(spanFromInitTime);
            hlog("######################### normal expire! ##########################");
            res= true;
        }
        if(spanFromInitTime<0)
        {
            //            cout<<"you modify time <init time expire!"<<endl;
            hlog("########################## you modify time <init time #################");
            res= true;
        }
        CTimeSpan tspan2=now-stime.timeLast;
        long long  spanFromLastTime=tspan2.GetTimeSpan();
        //        cout<<"span2 from last: "<<span2<<endl;
        hlog(spanFromLastTime);
        if(spanFromLastTime<0)
        {
            //            cout<<"you modify time <last time expire!"<<endl;
            hlog("########################### you modify time <last time!#################");
            res= true;
        }


        if(res==true)
            stime.isExpire=1;

        stime.timeLast=now;
        plib::save(this->strpath,stime);
    }
    else
    {
        res=true;
        hlog("##############Could not find license!############## ");
    }
    rfile.close();
    return res;
}

DWORD Expire::thread(LPVOID pm)
{
    Expire* pexpire=(Expire*)pm;
    hlog(pexpire->strpath);
    hlog(pexpire->expireSeconds);
    while(true)
    {
        if(pexpire->isExpire())
        {
            hlog("############### expired! exit! #################");
            exit(0);
        }
        Sleep(1000);
    }
    return 0;
}

void Expire::startCheck()
{
    HANDLE h=CreateThread(NULL,0,thread,this,0,NULL);
    //这个是等待,如果多线程就不能加,不然卡死在这了
    //    WaitForSingleObject(h,INFINITE);
}















bool readConfig(const string& filename, map<string, string>& mContent, const char* section)
{
    mContent.clear();
    ifstream infile(filename.c_str());
    if (!infile)
    {
        hlog("file open error!");
        return false;
    }
    string line, key, value;
    int pos = 0;
    string Tsection = string("[") + section + "]";
    bool flag = false;
    while (getline(infile, line))
    {
        if(!flag)
        {
            pos = line.find(Tsection, 0);
            if(-1 == pos)
            {
                continue;
            }
            else
            {
                flag = true;
                getline(infile, line);
            }
        }
        if(0 < line.length() && '[' == line.at(0))
        {
            break;
        }
        if (0 < line.length() && AnalyseLine(line, key, value))
        {

            if (value.length() > 0)
            {
                if(value[value.size()-1] == '\r')
                {
                    value[value.size()-1]='\0';
                }
            }
            mContent[key] = value;
        }
    }
    infile.close();
    return true;
}
map<string, string> readConfig(const string filename, const string section)
{
    map<string,string> mContent;
    ifstream infile(filename.c_str());
    if (!infile)
    {
        hlog("file open error!");
        return mContent;
    }
    string line, key, value;
    int pos = 0;
    string Tsection = string("[") + section + "]";
    bool flag = false;
    while (getline(infile, line))
    {
        if(!flag)
        {
            pos = line.find(Tsection, 0);
            if(-1 == pos)
            {
                continue;
            }
            else
            {
                flag = true;
                getline(infile, line);
            }
        }
        if(0 < line.length() && '[' == line.at(0))
        {
            break;
        }
        if (0 < line.length() && AnalyseLine(line, key, value))
        {

            if (value.length() > 0)
            {
                if(value[value.size()-1] == '\r')
                {
                    value[value.size()-1]='\0';
                }
            }
            mContent[key] = value;
        }
    }
    infile.close();
    return mContent;
}

string readConfig(const string filename, const string section, const string key)
{
    map<string, string> pres=readConfig(filename,section);
    return pres[key];
}
bool AnalyseLine(const string & line, string & key, string & val)
{
    if (line.empty())
    {
        return false;
    }
    int start_pos = 0, end_pos = line.size() - 1, pos = 0;
    if ((pos = line.find('#')) != -1)
    {
        if (0 == pos)
        {//行的第一个字符就是注释字符
            return false;
        }
        end_pos = pos - 1;
    }
    string new_line = line.substr(start_pos, start_pos + 1 - end_pos);  // 预处理，删除注释部分

    if ((pos = new_line.find('=')) == -1)
    {
        return false;  // 没有=号
    }

    key = new_line.substr(0, pos);
    val = new_line.substr(pos + 1, end_pos + 1- (pos + 1));

    Trim(key);
    if (key.empty())
    {
        return false;
    }
    Trim(val);
    return true;
}

void Trim(string & str)
{
    if (str.empty())
    {
        return;
    }
    int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i)
    {
        if (!IsSpace(str[i]))
        {
            break;
        }
    }
    if (i == str.size())
    { //全部是空白字符串
        str = "";
        return;
    }

    start_pos = i;

    for (i = str.size() - 1; i >= 0; --i)
    {
        if (!IsSpace(str[i]))
        {
            break;
        }
    }
    end_pos = i;

    str = str.substr(start_pos, end_pos - start_pos + 1);
}

bool IsSpace(char c)
{
    if (' ' == c || '\t' == c)
    {
        return true;
    }
    return false;
}



xctime xtime_init()
{
    xctime s_xtime;
    s_xtime.size=sizeof(xctime);

    return s_xtime;
}
xctime clib_getTimeAdd(xctime src, time_t sec)
{
    time_t isrc= clib_getSecondsFromXtime(src);
    time_t res=isrc+sec;
    return clib_getTimeFromSeconds(res);
}


xctime clib_getTimeSub(xctime src, time_t sec)
{
    time_t isrc= clib_getSecondsFromXtime(src);
    time_t res=isrc-sec;
    return clib_getTimeFromSeconds(res);
}


xctime clib_getTimeNow()
{
    //    struct tm t;   //tm结构指针
    time_t now;  //声明time_t类型变量
    time(&now);      //获取系统日期和时间
    //    localtime_s(&t, &now);   //获取当地日期和时间



    struct tm tmt;
    struct timeval tvt;
    gettimeofday(&tvt,NULL);
    //    localtime_r(&tvt.tv_sec,&tmt);
    //localtime_s也是用来获取系统时间，运行于windows平台下，与localtime_r只有参数顺序不一样
    localtime_s(&tmt,&now);
    xctime time=xtime_init();

    time.ushYear=tmt.tm_year+1900;
    time.uchMonth=tmt.tm_mon+1;
    time.uchDay=tmt.tm_mday;
    time.uchHour=tmt.tm_hour;
    time.uchMinute=tmt.tm_min;
    time.uchSecond=tmt.tm_sec;

    time.ushMSecond=tvt.tv_usec/1000;
    time.struTime=tvt;
    //    HLOG_INT(time.uchMSecond);
    return time;
}


double clib_getDiffBetweenXtime(xctime t1, xctime t2)
{
    double diff= (t2.struTime.tv_sec-t1.struTime.tv_sec)+(double)(t2.struTime.tv_usec-t1.struTime.tv_usec)/1000000;//微秒
    //    hlogone(diff);
    return diff;
}


int clib_getStringFromXtime(xctime t, char *str)
{
    int year=t.ushYear;
    char pcyear[5];memset(pcyear,0,5);
    sprintf(pcyear,"%d",year);

    int mon=t.uchMonth;
    char pcmon[3];memset(pcmon,0,3);
    if(mon<10)
        sprintf(pcmon,"0%d",mon);
    else
        sprintf(pcmon,"%d",mon);

    mon=t.uchDay;
    char pcDay[3];memset(pcDay,0,3);
    if(mon<10)
        sprintf(pcDay,"0%d",mon);
    else
        sprintf(pcDay,"%d",mon);

    mon=t.uchHour;
    char pcHour[3];memset(pcHour,0,3);
    if(mon<10)
        sprintf(pcHour,"0%d",mon);
    else
        sprintf(pcHour,"%d",mon);


    mon=t.uchMinute;
    char pcMinute[3];memset(pcMinute,0,3);
    if(mon<10)
        sprintf(pcMinute,"0%d",mon);
    else
        sprintf(pcMinute,"%d",mon);

    mon=t.uchSecond;
    char pcSecond[3];memset(pcSecond,0,3);
    if(mon<10)
        sprintf(pcSecond,"0%d",mon);
    else
        sprintf(pcSecond,"%d",mon);

    mon=t.ushMSecond;
    //    hlog(mon);
    char pcMSecond[4];memset(pcMSecond,0,4);
    if(mon<10)
        sprintf(pcMSecond,"00%d",mon);
    else if(mon>10&&mon<100)
        sprintf(pcMSecond,"0%d",mon);
    else
        sprintf(pcMSecond,"%d",mon);

    //    hlog(pcMSecond);

    sprintf(str,"%s-%s-%s %s:%s:%s.%s",
            pcyear,pcmon,pcDay,pcHour,pcMinute,pcSecond,pcMSecond);
    return 0;
}


int clib_getStringTimeNow(char *pch)
{
    xctime timenow= clib_getTimeNow();
    clib_getStringFromXtime(timenow, pch);
    return 0;
}


time_t clib_getNowSeconds()
{
    time_t sec=time((time_t*)NULL);
    if(sec<0)
    {
        hlog("get seconds error");
        return -1;
    }
    return sec;
}

xctime clib_getTimeFromSeconds(const time_t sec)
{
    struct tm tmt;
    localtime_s(&tmt,&sec);
    xctime time=xtime_init();
    time.ushYear=tmt.tm_year+1900;
    time.uchMonth=tmt.tm_mon+1;
    time.uchDay=tmt.tm_mday;
    time.uchHour=tmt.tm_hour;
    time.uchMinute=tmt.tm_min;
    time.uchSecond=tmt.tm_sec;
    //    time.ushMSecond
    return time;
}

xctime clib_getTimeFromString(const char *pch)
{
    xctime time=xtime_init();
    if(strlen(pch)!=23)
    {
        hlog("要获取时间的字符串是个非法字符串，长度不是19");
        return time;
    }
    //    HLOG_STRING(pch);

    //cstring_split有问题，没用

    char acyear[10];memset(acyear,0,10);
    memcpy(acyear,pch,4);
    //    HLOG_STRING(acyear);
    char acmon[10];memset(acmon,0,10);
    memcpy(acmon,pch+5,2);
    //    HLOG_STRING(acmon);
    char acday[10];memset(acday,0,10);
    memcpy(acday,pch+8,2);
    //    HLOG_STRING(acday);
    char achour[10];memset(achour,0,10);
    memcpy(achour,pch+11,2);
    //    HLOG_STRING(achour);
    char acmin[10];memset(acmin,0,10);
    memcpy(acmin,pch+14,2);
    //    HLOG_STRING(acmin);
    char acsec[10];memset(acsec,0,10);
    memcpy(acsec,pch+17,2);

    char acM[10];memset(acM,0,10);
    memcpy(acM,pch+20,3);
    //    HLOG_STRING(acsec);
    //     cstring_toStringFix(strYear,acyear);
    //     cstring_toStringFix(strMonth,acmon);
    //     cstring_toStringFix(strDay,acday);
    //     cstring_toStringFix(strHour,achour);
    //     cstring_toStringFix(strMinute,acmin);
    //     cstring_toStringFix(strSecond,acsec);

    //    HLOG_STRING(acyear);
    //    HLOG_STRING(acmon);
    //    HLOG_STRING(acday);
    //    HLOG_STRING(achour);
    //    HLOG_STRING(acmin);
    //    HLOG_STRING(acsec);


    time.ushYear=atoi(acyear);
    time.uchMonth=atoi(acmon);
    time.uchDay=atoi(acday);
    time.uchHour=atoi(achour);
    time.uchMinute=atoi(acmin);
    time.uchSecond=atoi(acsec);

    time.ushMSecond=atoi(acM);
    //    HLOG_TIME(time);

    //    listleft->destroy(listleft);
    //    listright->destroy(listright);
    //    listStr->destroy(listStr);
    //    str->destroy(str);




    struct tm tm_;
    tm_.tm_year  = time.ushYear-1900;
    tm_.tm_mon   = time.uchMonth-1;
    tm_.tm_mday  = time.uchDay;
    tm_.tm_hour  = time.uchHour;
    tm_.tm_min   = time.uchMinute;
    tm_.tm_sec   = time.uchSecond;
    tm_.tm_isdst = 0;

    time_t t_ = mktime(&tm_); //已经减了8个时区

    time.struTime.tv_sec=t_;
    //    time.struTime.tv_usec=0;
    time.struTime.tv_usec=time.ushMSecond*1000;//微妙数=毫秒数*1000
    return time;
}

time_t clib_getSecondsFromXtime(xctime time)
{
    struct tm tmt1;
    tmt1.tm_year=time.ushYear-1900;
    tmt1.tm_mon=time.uchMonth-1;
    tmt1.tm_mday=time.uchDay;
    tmt1.tm_hour=time.uchHour;
    tmt1.tm_min=time.uchMinute;
    tmt1.tm_sec=time.uchSecond;
    return mktime(&tmt1);
}


ptime::ptime()
{
    init();
    this->time=clib_getTimeNow();
}

void ptime::init()
{
    gettimeofday(&(this->time.struTime),NULL); //gettimeofday(&start,&tz);结果一样
}

void ptime::setNowTime()
{
    this->init();
    this->time=clib_getTimeNow();
}


ptime::ptime(xctime time)
{
    init();
    this->time=time;
}

ptime::ptime(int year, int mon, int day, int hour, int min, int sec)
{
    init();
    //直接赋值有错，还是直接用字符串比较好
    //    this->time.ushYear=year;
    //    this->time.uchMonth=mon;
    //    this->time.uchDay=day;
    //    this->time.uchHour=hour;
    //    this->time.uchMinute=min;
    //    this->time.uchSecond=sec;

    string strtime=plib::toString(year)+"-";
    string strmon;
    if(mon<10)
        strmon="0"+plib::toString(mon);
    else
        strmon=plib::toString(mon);
    string strday;
    if(day<10)
        strday="0"+plib::toString(day);
    else
        strday=plib::toString(day);
    string strhour;
    if(hour<10)
        strhour="0"+plib::toString(hour);
    else
        strhour=plib::toString(hour);
    string strmin;
    if(min<10)
        strmin="0"+plib::toString(min);
    else
        strmin=plib::toString(min);
    string strsec;
    if(sec<10)
        strsec="0"+plib::toString(sec);
    else
        strsec=plib::toString(sec);

    strtime=strtime+strmon+"-"+strday+" "+strhour+":"+strmin+":"+strsec;
    //    hlog(strtime);

    this->time=clib_getTimeFromString(strtime.c_str());
}

ptime::ptime(string strTime)
{
    init();
    //    clib c=clibInit();
    //    this->time=c.getTimeFromString(strTime.c_str());
    this->time=clib_getTimeFromString(strTime.c_str());
}
ptime::ptime(const char* acTime)
{
    init();
    //    clib c=clibInit();
    //    this->time=c.getTimeFromString(acTime);
    this->time=clib_getTimeFromString(acTime);
}
pstring ptime::getTimeNow() {
    pstring strTimeNow=plib::getTimeNow();
//    hlog(strTimeNow);
    return strTimeNow.substr(11,strTimeNow.size());
}

pstring ptime::getTimeNoMsNow() {
    return plib::getTimeHHMMSS();
}

pstring ptime::getTimeFullNoMsNow() {
    pstring strTimeFull=ptime::getTimeFullNow();
    return strTimeFull.substr(0,strTimeFull.size()-4);
}

pstring ptime::getTimeFullNow() {
    char acTime[22];
    bzero(acTime, 22);
    clib_getStringTimeNow(acTime);
    return string(acTime);
}

pstring ptime::getDateNow() {
    string time = getTimeFullNow();
    return time.substr(0, 10);
}
unsigned short ptime::year()
{
    return this->time.ushYear;
}

unsigned char ptime::mon()
{
    return this->time.uchMonth;
}

unsigned char ptime::day()
{
    return this->time.uchDay;
}

unsigned char  ptime::hour()
{
    return this->time.uchHour;
}

unsigned char  ptime::minute()
{
    return this->time.uchMinute;
}

unsigned char ptime::second()
{
    return this->time.uchSecond;
}

unsigned short ptime::msecond()
{
    return this->time.ushMSecond;
}

double ptime::getDiff(clock_t t1, clock_t t2)
{
    return ((double)(t1-t2))/CLOCKS_PER_SEC;
}

ptime ptime::getTimeFromSeconds(int64_t secs)
{
    return ptime(clib_getTimeFromSeconds(secs));
}

string  ptime::toString()
{
    char acTime[30];
    clib_getStringFromXtime(this->time,acTime);
    return string(acTime);
}

double ptime::operator -(const ptime &time2)
{
    //    hlog(time2);
    //    hlog(*this);
    //    HLOG_TIME(time2.time);
    //    HLOG_TIME(this->time);
    return clib_getDiffBetweenXtime(time2.time,this->time);
}


ptime ptime::operator +(const time_t secs)
{
    return ptime(clib_getTimeAdd(this->time,secs));
}

ptime ptime::operator -(const int64_t secs)
{
    return ptime(clib_getTimeSub(this->time,secs));
}






/* Constants for MD5Transform routine. */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


/* F, G, H and I are basic MD5 functions.
*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
*/
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + ac; \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + ac; \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + ac; \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + ac; \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }


const byte MD5::PADDING[64] = { 0x80 };
const char MD5::HEX[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

/* Default construct. */
MD5::MD5() {
    reset();
}

/* Construct a MD5 object with a input buffer. */
MD5::MD5(const void *input, size_t length) {
    reset();
    update(input, length);
}

/* Construct a MD5 object with a string. */
MD5::MD5(const string &str) {
    reset();
    update(str);
}

/* Construct a MD5 object with a file. */
MD5::MD5(ifstream &in) {
    reset();
    update(in);
}

/* Return the message-digest */
const byte* MD5::digest() {
    if (!_finished) {
        _finished = true;
        final();
    }
    return _digest;
}

/* Reset the calculate state */
void MD5::reset() {

    _finished = false;
    /* reset number of bits. */
    _count[0] = _count[1] = 0;
    /* Load magic initialization constants. */
    _state[0] = 0x67452301;
    _state[1] = 0xefcdab89;
    _state[2] = 0x98badcfe;
    _state[3] = 0x10325476;
}

/* Updating the context with a input buffer. */
void MD5::update(const void *input, size_t length) {
    update((const byte*)input, length);
}

/* Updating the context with a string. */
void MD5::update(const string &str) {
    update((const byte*)str.c_str(), str.length());
}

/* Updating the context with a file. */
void MD5::update(ifstream &in) {

    if (!in)
        return;

    std::streamsize length;
    char buffer[BUFFER_SIZE];
    while (!in.eof()) {
        in.read(buffer, BUFFER_SIZE);
        length = in.gcount();
        if (length > 0)
            update(buffer, length);
    }
    in.close();
}

/* MD5 block update operation. Continues an MD5 message-digest
operation, processing another message block, and updating the
context.
*/
void MD5::update(const byte *input, size_t length) {

    ulong i, index, partLen;

    _finished = false;

    /* Compute number of bytes mod 64 */
    index = (ulong)((_count[0] >> 3) & 0x3f);

    /* update number of bits */
    if((_count[0] += ((ulong)length << 3)) < ((ulong)length << 3))
        _count[1]++;
    _count[1] += ((ulong)length >> 29);

    partLen = 64 - index;

    /* transform as many times as possible. */
    if(length >= partLen) {

        memcpy(&_buffer[index], input, partLen);
        transform(_buffer);

        for (i = partLen; i + 63 < length; i += 64)
            transform(&input[i]);
        index = 0;

    } else {
        i = 0;
    }

    /* Buffer remaining input */
    memcpy(&_buffer[index], &input[i], length-i);
}

/* MD5 finalization. Ends an MD5 message-_digest operation, writing the
the message _digest and zeroizing the context.
*/
void MD5::final() {

    byte bits[8];
    ulong oldState[4];
    ulong oldCount[2];
    ulong index, padLen;

    /* Save current state and count. */
    memcpy(oldState, _state, 16);
    memcpy(oldCount, _count, 8);

    /* Save number of bits */
    encode(_count, bits, 8);

    /* Pad out to 56 mod 64. */
    index = (ulong)((_count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    update(PADDING, padLen);

    /* Append length (before padding) */
    update(bits, 8);

    /* Store state in digest */
    encode(_state, _digest, 16);

    /* Restore current state and count. */
    memcpy(_state, oldState, 16);
    memcpy(_count, oldCount, 8);
}

/* MD5 basic transformation. Transforms _state based on block. */
void MD5::transform(const byte block[64]) {

    ulong a = _state[0], b = _state[1], c = _state[2], d = _state[3], x[16];

    decode(block, x, 64);

    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    _state[0] += a;
    _state[1] += b;
    _state[2] += c;
    _state[3] += d;
}

/* Encodes input (ulong) into output (byte). Assumes length is
a multiple of 4.
*/
void MD5::encode(const ulong *input, byte *output, size_t length) {

    for(size_t i=0, j=0; j<length; i++, j+=4) {
        output[j]= (byte)(input[i] & 0xff);
        output[j+1] = (byte)((input[i] >> 8) & 0xff);
        output[j+2] = (byte)((input[i] >> 16) & 0xff);
        output[j+3] = (byte)((input[i] >> 24) & 0xff);
    }
}

/* Decodes input (byte) into output (ulong). Assumes length is
a multiple of 4.
*/
void MD5::decode(const byte *input, ulong *output, size_t length) {

    for(size_t i=0, j=0; j<length; i++, j+=4) {
        output[i] = ((ulong)input[j]) | (((ulong)input[j+1]) << 8) |
                (((ulong)input[j+2]) << 16) | (((ulong)input[j+3]) << 24);
    }
}

/* Convert byte array to hex string. */
string MD5::bytesToHexString(const byte *input, size_t length) {
    string str;
    str.reserve(length << 1);
    for(size_t i = 0; i < length; i++) {
        int t = input[i];
        int a = t / 16;
        int b = t % 16;
        str.append(1, HEX[a]);
        str.append(1, HEX[b]);
    }
    return str;
}

/* Convert digest to string value */
string MD5::toString() {
    return bytesToHexString(digest(), 16);
}





cudp *cudpInit(const char *iplocal, int port, enum typeServerOrClient typeSorC, const char*ipgroup)
{
    cudp* udp=NULL;
    if(ipgroup==NULL)
    {
        if(typeSorC==server)
            udp= cudpRecvInit(iplocal, port);
        if(typeSorC==client)
            udp= cudpSendInit(iplocal, port);
        if(udp==NULL)
            return NULL;
        udp->typeNorG=normal;
    }
    else
    {
        if(typeSorC==server)
            udp= cudpGroupRecvInit(iplocal, ipgroup, port);
        if(typeSorC==client)
            udp= cudpGroupSendInit(iplocal, ipgroup, port);
        udp->typeNorG=group;
    }
    //udp最大不能超过64k，一般最大就用63k把，这样设置没用
    //    int isize=1*1024*1024;//发送缓冲区
    //    HLOG_INT(setsockopt(udp->sock,SOL_SOCKET,SO_SNDBUF,&isize,sizeof(isize)));
    return udp;
}



cudp *xudpInitPackage(const char *pchIPAddr, int iPort)
{
    cudp* udp=HMALLOC(cudp,1);

    //初始化WSA
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
        return NULL;
    }

    udp->port=iPort;
    strcpy(udp->ip,pchIPAddr);
    udp->send= &cudpSend;
    udp->recv= &cudpRecv;
    udp->recvTimeout=cudpRecvTimeout;
    udp->destroy= &cudpDestroy;
    udp->struUdpSendAddr.sin_family = AF_INET;
    return udp;
}


/*建立和传输代理的连接new
* ConnTrans
* pchIPAddr：本机188IP地址
* iPort ：188本机端口
* 返回值：大于0表示套接字
*         -1表示建立连接失败
*/
int cudp_ServerInit(char *pchIPAddr, int iPort)
{
    struct sockaddr_in  struUdpSendAddr;         /*组播地址结构*/
    int iSock = cudp_InitUdpSocket(pchIPAddr, iPort, &struUdpSendAddr);
    hlog("after update:",iSock);

    //建立并绑定，这是server端，用的是本机IP以及接收端口
    //如果建立连接失败，则返回错误
    if (cudp_BindUdpSocket(iSock, pchIPAddr, iPort, &struUdpSendAddr) < 0)
    {
        iSock = -1;
    }
    hlog("after bind:",iSock);
    return iSock;
}

///*udp发送函数
//*SendUdp
//*arrchBuf：  发送的字符串
//*ilength：  发送字符串的长度
//*arrchIP：	向谁发送的IP地址
//*iPort: 端口号
//*返回值 返回发送包的长度
//*/
//int SendUdp(char* arrchBuf,int ilength,char* arrchIP,int iPort)
//{
//    struct sockaddr_in  struUdpSendAddr;         /*组播地址结构*/
//    int iRet;//return parameter which repre length of packet
//    //发送时是对方IP地址，发送端口，不需绑定
//    int iSock = cudp_InitUdpSocket(arrchIP,iPort,&struUdpSendAddr);
//    if (iSock<0)
//    {
//        HLOG_STRING_STRINGA("Bind khd socket failed:",strerror(errno));
//        close(iSock);
//        return -1;
//    }
//    //xlog(xp, "address=%s,port=%d",g_struGlobal.struCfg.arrchTransAgentIP1,g_struGlobal.struCfg.iUdpPortSend);
//    //        xlog(xp, "address=%s,port=%d",arrchIP,iPort);
//    iRet=sendto(iSock, arrchBuf, ilength, 0, (struct sockaddr *)&struUdpSendAddr, sizeof(struct sockaddr_in));
//    //        xlog(xp, "the length of package I send is %d", iRet);
//    close(iSock);
//    return iRet;
//}


int cudpSend(cudp *udp, const char *data, int ilength)
{
    return sendto(udp->sock, data, ilength, 0, (struct sockaddr *)&(udp->struUdpSendAddr), sizeof(struct sockaddr_in));
}

int cudpGroupSend(cudp *udp, const char *data, int ilength)
{
    udp->struUdpSendAddr.sin_port = htons(udp->port);
    udp->struUdpSendAddr.sin_addr.s_addr = inet_addr(udp->ip);
    return sendto(udp->sock, data, ilength, 0, (struct sockaddr *)&(udp->struUdpSendAddr), sizeof(struct sockaddr_in));
}

int cudpRecv(cudp *udp, char *data, int ilength)
{
    int iSockLen = sizeof(struct sockaddr_in);
    int iRet = recvfrom(udp->sock, data, ilength, 0, (struct sockaddr *)&(udp->struUdpSendAddr), &iSockLen);
    return iRet;
}


cudp *cudpGroupRecvInit(const char *pchIPAddr,const char *pchGroupIPAddr, int iGroupPort)
{
    cudp* udp=xudpInitPackage(pchGroupIPAddr,iGroupPort);
    int iSock = cudp_InitRecvGroupSocket(pchIPAddr, pchGroupIPAddr, iGroupPort, &(udp->struUdpSendAddr));
    if (iSock<=0)
    {
        hlog("Bind khd socket failed:",strerror(errno));
        udp->destroy(udp);
        return NULL;
    }
    udp->sock=iSock;
    hlog(udp->sock);
    hlog(udp->ip);
    return udp;
}


cudp *cudpGroupSendInit(const char *pchIPAddr,const  char *pchGroupIPAddr, int iGroupPort)
{
    cudp* udp=xudpInitPackage(pchGroupIPAddr,iGroupPort);
    //因为是组播，所以发送函数有所不同，使用组播发送
    udp->send= &cudpGroupSend;

    int iSock = cudp_InitSendGroupSocket(pchIPAddr, pchGroupIPAddr, iGroupPort, &(udp->struUdpSendAddr));
    if (iSock<=0)
    {
        hlog("Bind khd socket failed:",strerror(errno));
        udp->destroy(udp);
        return NULL;
    }
    hlog(iSock);
    udp->sock=iSock;
    hlog(udp->sock);
    hlog(udp->ip);
    return udp;
}


cudp *cudpRecvInit(const char *pchIPAddr, int iPort)
{
    cudp* udp=xudpInitPackage(pchIPAddr,iPort);
    int iSock = cudp_InitUdpSocket(pchIPAddr, iPort, &(udp->struUdpSendAddr));
    if (iSock<0)
    {
        hlog("Init udp socket failed:",strerror(errno));
        close(iSock);
        free(udp);
        return NULL;
    }
    //建立并绑定，这是server端，用的是本机IP以及接收端口
    //如果建立连接失败，则返回错误
    if (cudp_BindUdpSocket(iSock, pchIPAddr, iPort, &(udp->struUdpSendAddr)) < 0)
    {
        hlog("bind fail");
        udp->destroy(udp);
        return NULL;
    }

    udp->sock=iSock;
    //    udp->port=iPort;
    //    strcpy(udp->ip,pchIPAddr);

    //    udp->send=&cudpSend;
    //    udp->recv=&cudpRecv;
    return udp;
}


cudp *cudpSendInit(const char *pchIPAddr, int iPort)
{
    cudp* udp=xudpInitPackage(pchIPAddr,iPort);
    int iSock = cudp_InitUdpSocket(pchIPAddr, iPort, &(udp->struUdpSendAddr));
    if (iSock<0)
    {
        hlog("Init udp socket failed:",strerror(errno));

        udp->destroy(udp);
        return NULL;
    }
    udp->sock=iSock;
    return udp;
}

/*
 * ctcp_CloseConnection
 * iSocket：套接字
 *
 * 关闭TCP连接套接字。
*/
void ctcp_CloseConnectionOld(int iSocket)
{
    //    HLOG_INT(shutdown(iSocket,SHUT_RDWR));
    //    HLOG("现在关闭套接字");
    //如果关闭套接字失败，则退出
    if ((close(iSocket)) != 0)
    {
        perror("close() failed!");
        hlog("关闭套接字失败");
        //printf("%s\n", strerror(errno));
    }
    //    HLOG_INT(shutdown(iSocket,SHUT_RDWR));
    return;
}
int cudpDestroy(cudp *udp)
{
    //    ctcp_CloseConnection(udp->sock);
    if ((closesocket(udp->sock)) != 0)
    {
        perror("close() failed!");
        //        HLOG("关闭套接字失败");
        //printf("%s\n", strerror(errno));
        return -1;
    }
    if(udp!=NULL)
    {
        free(udp);
        udp=NULL;
    }
    return 0;
}
/*
 * cudp_InitUdpSocket
 * 返回值：套接字，小于0表示初始化套接字失败
 *
 * 初始化新Udp套接字。
*/
int cudp_InitUdpSocket(const char *pchIPAddr, int iPort, struct sockaddr_in *pstruAddr)
{
    int iSock = -1;
    int iOn = 1;

    //如果套接字初始化失败，则返回-1
    if ((iSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return -2;
    }

    pstruAddr->sin_family = AF_INET;
    //如果端口号是0，则使用INADDR_ANY
    pstruAddr->sin_port = iPort == 0 ? INADDR_ANY : htons(iPort);
    //如果IP地址为空，则使用INADDR_ANY
    pstruAddr->sin_addr.s_addr = pchIPAddr == NULL ? INADDR_ANY : inet_addr(pchIPAddr);

    //解决Address already in use错误
    setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&iOn, sizeof(iOn));
    //    HLOG_INT(iSock);
    return iSock;
}
/*
 * cudp_InitSendGroupSocket
 * 返回值：套接字，0表示初始化套接字失败
 *
 * 初始化组播发送Udp套接字。
*/
int cudp_InitSendGroupSocket(const char *pchIPAddr, const char *pchGroupIPAddr, int iGroupPort,
                             struct sockaddr_in *pstruAddr)
{
    int iSock;
    struct sockaddr_in struSin;

    if ((iSock = cudp_InitUdpSocket(pchGroupIPAddr, iGroupPort, pstruAddr)) < 0)
    {
        return -1;
    }

    memset(&struSin, 0, sizeof(struSin));
    struSin.sin_family = AF_INET;
    struSin.sin_addr.s_addr = inet_addr(pchIPAddr);
    //绑定套接字到目标地址，如果出现错误则退出,c++11中bind和std中的冲突,必须这样才行::
    if (::bind(iSock, (struct sockaddr *)&struSin, sizeof(struSin)) < 0)
    {
        return -2;
    }
    return iSock;
}

/*
 * cudp_InitRecvGroupSocket
 * 返回值：套接字，0表示初始化套接字失败
 *
 * 初始化组播接收Udp套接字。
*/
int cudp_InitRecvGroupSocket(const char *pchIPAddr, const char *pchGroupIPAddr, int iGroupPort,
                             struct sockaddr_in *pstruAddr)
{
    int iSock;
    struct  ip_mreq   mreq;

    if ((iSock = cudp_InitUdpSocket(NULL, iGroupPort, pstruAddr)) < 0)
    {
        return -1;
    }

    //绑定套接字到目标地址，如果出现错误则退出
    if (::bind(iSock, (struct sockaddr *)pstruAddr, sizeof(struct sockaddr_in)) < 0)
    {
        return -2;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(pchGroupIPAddr);
    mreq.imr_interface.s_addr = inet_addr(pchIPAddr);
    //将本机地址加入到组播
    if (setsockopt(iSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) <0)
    {
        return   -3;
    }
    return   iSock;
}

/*
 * cudp_BindUdpSocket
 * iSocket：  套接字
 * pchIPAddr：IP地址
 * iPort：    端口号
 *
 * 将IP地址和端口号绑定到套接字上。
*/
int cudp_BindUdpSocket(int iSocket, const char *pchIPAddr, int iPort, struct sockaddr_in *pstruAddr)
{
    int iOn = 1;
    struct linger sopt = {1, 0};

    pstruAddr->sin_family = AF_INET;
    //如果端口号是0，则使用INADDR_ANY
    pstruAddr->sin_port = iPort == 0 ? INADDR_ANY : htons(iPort);
    //如果IP地址为空，则使用INADDR_ANY
    pstruAddr->sin_addr.s_addr = pchIPAddr == NULL ? INADDR_ANY : inet_addr(pchIPAddr);

    //解决Address already in use错误
    setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&iOn, sizeof(iOn));
    //setsockopt(iSocket, SOL_SOCKET, SO_REUSEPORT, &iOn, sizeof(iOn));
    setsockopt(iSocket, SOL_SOCKET, SO_LINGER, (const char *)&sopt, sizeof(sopt));
    //绑定套接字到目标地址，如果出现错误则退出
    if (::bind(iSocket, (struct sockaddr *)pstruAddr, sizeof(struct sockaddr_in)) < 0)
    {
        return -1;
    }
    return 0;
}



int cudpRecvTimeout(cudp *udp, char *data, int ilength, int itmeout)
{
    //    //设置接收超时时间
    if(itmeout>0)
    {
        //这貌似和windows不宜样，这个设置并不管用，主要是下边管用
        struct timeval timeout={itmeout,0};//秒和微妙
        setsockopt(udp->sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
    }
    int iSockLen = sizeof(struct sockaddr_in);
    int iRet = recvfrom(udp->sock, data, ilength, 0, (struct sockaddr *)&(udp->struUdpSendAddr), &iSockLen);
    return iRet;
}


void printString(pstring str)
{
    hlog(str.c_str());
}

//BOOL CALLBACK EnumChildProc(_In_ HWND hwnd, _In_ LPARAM lParam)
//{

//    char szTitle[MAX_PATH] = { 0 };
//    char szClass[MAX_PATH] = { 0 };
//    int nMaxCount = MAX_PATH;

//    LPSTR lpClassName = szClass;
//    LPSTR lpWindowName = szTitle;

//    GetWindowTextA(hwnd, lpWindowName, nMaxCount);
//    GetClassNameA(hwnd, lpClassName, nMaxCount);
////    cout << "[Child window] window handle: " << hwnd << " window name: "
////         << lpWindowName << " class name " << lpClassName << endl;
//    //    }
//    return TRUE;
//}
//BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
//{
//    //可见窗口才遍历
//    if(IsWindowVisible(hwnd))
//    {
//        /*
//    Remarks
//    The EnumWindows function does not enumerate child windows,
//    with the exception of a few top-level windows owned by the
//    system that have the WS_CHILD style.
//    */
//        char szTitle[MAX_PATH] = { 0 };
//        char szClass[MAX_PATH] = { 0 };
//        int nMaxCount = MAX_PATH;

//        LPSTR lpClassName = szClass;
//        LPSTR lpWindowName = szTitle;

//        GetWindowTextA(hwnd, lpWindowName, nMaxCount);
//        GetClassNameA(hwnd, lpClassName, nMaxCount);
////        cout << "[Parent window] window handle: " << hwnd << " window name: "
////             << lpWindowName << " class name " << lpClassName << endl;
//        mut.lock();
//        mapWindowHWNDAll.append(hwnd,lpWindowName);
//        mut.unlock();
//        EnumChildWindows(hwnd, EnumChildProc, lParam);
//    }
//    return true;
//}
////多线程不能用
//plist<HWND> common_getWindowAllByTitle(string title)
//{
//    plist<HWND> listres;
//    mut.lock();
//    mapWindowHWNDAll.clear();
//    mut.unlock();
//    EnumWindows(EnumWindowsProc, NULL);
////    hlog(mapWindowHWNDAll.size());
//    for(int i=0;i<mapWindowHWNDAll.size();i++)
//    {
//        HWND hi=mapWindowHWNDAll.getKeys()[i];
//        string ti=mapWindowHWNDAll[hi];
//        if(ti==title)
//        {
////            hlog(ti);
//            listres.append(hi);
//        }
//    }
//    return listres;
//}
//这是为了ping类
#define ICMP_ECHO 8  // 定义回显报文代码
#define ICMP_ECHOREPLY 0
#define ICMP_MIN 8  // 最小8字节ICMP包
#define MAX_ICMP_PACKET 1024 // 最大ICMP大小
// IP头首部结构
struct IpHeader
{
    unsigned int h_len:4;   // 首部长度
    unsigned int version:4;   // IP版本
    unsigned char tos;    // 服务类型
    unsigned short total_len;  // 包总长度
    unsigned short ident;   // 标识符
    unsigned short frag_and_flags; // 标志
    unsigned char ttl;    // 生存周期
    unsigned char proto;   // protocol (TCP, UDP etc) 协议类型
    unsigned short checksum;  // IP检验和
    unsigned int sourceIP;   // 源地址IP
    unsigned int destIP;   // 目的地址IP
};

// ICMP 首部结构
struct IcmpHeader
{
    BYTE i_type; // 类型
    BYTE i_code; // 代码类型
    USHORT i_cksum; // 检验和
    USHORT i_id; // 地址
    USHORT i_seq; // 发送顺序
    // 增加一个时间戳
    ULONG timestamp;
};

CPing::CPing(void)
{
    m_socketRaw = INVALID_SOCKET;
    m_bPrintInfo = false;
    Init();
}


CPing::~CPing(void)
{
    FInit();
}

bool CPing::Init()
{
    WSADATA wsaData = {0};
    if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
        return false;

    //创建发送套接字
    // 注：为了使用发送接收超时设置(即设置SO_RCVTIMEO, SO_SNDTIMEO)，
    // 必须将标志位设为WSA_FLAG_OVERLAPPED !
    m_socketRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_socketRaw == INVALID_SOCKET)
        return false;

    // 设置一个有接收时限的套接字
    int nTimeout = 1000;
    int nResult = setsockopt(m_socketRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeout, sizeof(nTimeout));
    if(nResult == SOCKET_ERROR)
        return false;

    //设置一个有发送时限的套接字
    nTimeout = 1000;
    nResult = setsockopt(m_socketRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&nTimeout, sizeof(nTimeout));
    if(nResult == SOCKET_ERROR)
        return false;

    return true;
}

bool CPing::FInit()
{
    WSACleanup();

    return true;
}

bool CPing::PingHelper(const char* pDscAddr, int nPackNum , int nDataSize )
{
    if (pDscAddr == NULL || nPackNum <= 0)
        return false;

    // 进行域名解析
    sockaddr_in addrDsc = {0};

    hostent* pHostent = gethostbyaddr(pDscAddr, strlen(pDscAddr), AF_INET);
    if (pHostent != NULL)
    {
        addrDsc.sin_family = AF_INET;
        addrDsc.sin_addr.S_un.S_addr = inet_addr(pDscAddr);
    }
    else
    {
        pHostent = gethostbyname(pDscAddr);
        if (pHostent == NULL)
            return false;

        addrDsc.sin_family = pHostent->h_addrtype;
        memcpy(&(addrDsc.sin_addr), pHostent->h_addr, pHostent->h_length);
    }

    if (m_bPrintInfo)
    {
        std::cout << "Pinging " << inet_ntoa(addrDsc.sin_addr);
        std::cout << " with " << nDataSize << " bytes of data:" << std::endl << std::endl;
    }

    char szIcmpData[MAX_ICMP_PACKET] = {0};
    char szRecvBuff[MAX_ICMP_PACKET] = {0};
    ConstructIcmpMessage(szIcmpData, nDataSize);
    IcmpHeader* pIcmpHeader = reinterpret_cast<IcmpHeader*>(szIcmpData);
    int nPackSize = nDataSize + sizeof(IcmpHeader);
    int nSeqNO = 0;
    int nRet = 0;
    sockaddr_in addrSrc = {0};
    int nRecvPacket = 0;
    for (int i = 0; i < nPackNum; ++i)
    {
        pIcmpHeader->i_cksum = 0; // 先把ICMP的检验和置零
        pIcmpHeader->i_seq = nSeqNO++; // 发送顺序加一
        pIcmpHeader->timestamp = GetTickCount(); // 记录时间
        pIcmpHeader->i_cksum = CheckSum((USHORT*)szIcmpData, nPackSize);// 计算检验和
        nRet = sendto(m_socketRaw, szIcmpData, nPackSize, 0, (sockaddr*)&addrDsc, sizeof(addrDsc));
        if (nRet == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAETIMEDOUT) // 发送超时
            {
                if (m_bPrintInfo)
                {
                    std::cout << "Request timed out." << std::endl;
                }
                continue;
            }

            return false;
        }

        int nSrcLen = sizeof(addrSrc);
        nRet = recvfrom(m_socketRaw, szRecvBuff, sizeof(szRecvBuff), 0, (sockaddr*)&addrSrc, &nSrcLen);
        if (nRet == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAETIMEDOUT) // 接收超时
            {
                if (m_bPrintInfo)
                {
                    std::cout << "Request timed out." << std::endl;
                }
                continue;
            }

            return false;
        }

        if (DecodeIpPacket(szRecvBuff, nRet, &addrSrc))
            ++nRecvPacket;

        if (m_bPrintInfo)
            Sleep(1000);
    }

    if (m_bPrintInfo)
    {
        std::cout << std::endl << "Packets: Sent = " << nPackNum;
        std::cout << ", Received = " << nRecvPacket;
        std::cout << ", Lost = " << nPackNum-nRecvPacket;
        std::cout << " (" << (nPackNum-nRecvPacket)*100/nPackNum;
        std::cout << "% loss)" << std::endl;
    }

    return nRecvPacket > 0;
}

bool CPing::ConstructIcmpMessage(char* pDataBuff, int nDataSize)
{
    if (pDataBuff == NULL)
        return false;

    IcmpHeader* pIcmpHeader = (IcmpHeader*)pDataBuff;
    pIcmpHeader->i_type  = ICMP_ECHO;     // 设置回显报文
    pIcmpHeader->i_code  = 0;
    pIcmpHeader->i_id  = (USHORT)GetCurrentProcessId();// 获取当前进程ID
    pIcmpHeader->i_cksum = 0;       // 初始化检验和为0
    pIcmpHeader->i_seq  = 0;       // 顺序符

    char* pDataPart = pDataBuff + sizeof(IcmpHeader);  // 给数据区赋地址
    memset(pDataPart, 'E', nDataSize);      // 把数据区初始化

    return true;
}

// 计算校验和
USHORT CPing::CheckSum(USHORT* pBuff, int nSize)
{
    unsigned long ulChkSum = 0; // 给检验和置0
    while(nSize > 1)
    {
        ulChkSum += *pBuff++; // 把所有IP数据报的数据都加到一起
        nSize -= sizeof(USHORT);// 每次计算两个十二位二进制和 （每次减两个字节）
    }
    if(nSize != 0)
    {
        ulChkSum += *(UCHAR*)pBuff; // 判断当只剩下一个字节没有加到检验和时在这里加上
    }
    ulChkSum = (ulChkSum >> 16) + (ulChkSum & 0XFFFF); // 先按32bit计算二进制和得到cksum，然后把高位低位相加
    //得到16bit的和。
    ulChkSum += (ulChkSum >>16); // 把高16位和低16位相加溢出的进位再加到校验和中

    return (USHORT)(~ulChkSum);  // 取反得到检验和
}

bool CPing::DecodeIpPacket(char* pBuff, int nBuffSize, sockaddr_in* pAddr)
{
    if (pBuff == NULL || nBuffSize <= 0)
        return false;

    IpHeader* pIpHeader = (IpHeader*)pBuff;
    unsigned short usIpHeaderLen = (pIpHeader->h_len) * 4 ; //计算IP首部的长度
    IcmpHeader* pIcmpHeader = (IcmpHeader*)(pBuff + usIpHeaderLen);
    //判断是否是回显应答报文
    if (pIcmpHeader->i_type != ICMP_ECHOREPLY)
        return false;

    // 判断是否是当前进程的IP地址，确认是此次的回显报文。
    if (pIcmpHeader->i_id != (USHORT)GetCurrentProcessId())
        return false;

    if (nBuffSize < usIpHeaderLen + ICMP_MIN)
        return false;

    if (m_bPrintInfo)
    {
        std::cout << "Reply from " << inet_ntoa(pAddr->sin_addr);
        std::cout << ": bytes="<< nBuffSize-usIpHeaderLen-ICMP_MIN;
        std::cout << " time=" << GetTickCount()-pIcmpHeader->timestamp;
        std::cout << "ms TTL=" << pIpHeader->ttl << std::endl;
    }

    return true;
}

bool CPing::CanPing(const char* pDscAddr)
{
    m_bPrintInfo = false;
    return PingHelper(pDscAddr);
}

void CPing::Ping(const char* pDscAddr, int nPackNum , int nPackSize )
{
    m_bPrintInfo = true;
    PingHelper(pDscAddr, nPackNum, nPackSize);
}


/*
 *
 *	名称:	ReadLine ()
 *
 *	参数1: iFildes			文件描述符
 *	参数2: pchBuf				接收缓冲区
 *	参数3: uiNByte			缓冲区长度
 *
 *	返回值:	若成功则返回该行的长度，否则返回-1。
 *
 *	功能:	从文件中读一行。
 *
*/
int clib_readLine(int iFildes, char *pchBuf, unsigned int uiNByte)
{
    int	iResult;						//返回值
    int	i;
    int	iLineStart=0;				//行起始位置
    char arrchTmpBuf[256];	//临时缓冲区
    int	iTmpResult;					//临时返回值

    //初始化数组
    memset(arrchTmpBuf,	0, 256);

    //从文件中读取一定字节。
    iResult = read(iFildes,pchBuf,uiNByte);
    hlogone(iResult);
    hlogone(pchBuf);
    if (iResult	== -1)
    {
        return -1;
    }
    else if(iResult ==	0)
    {
        return 0;
    }

    //查找换行符。
    for	(i=0;i<iResult; i++)
    {
        if (pchBuf[i] == 10)
        {
            if ((i==0)||(pchBuf[i-1] ==10))
            {
                iLineStart = i + 1;
                continue;
            }
            else
            {
                break;
            }
        }
    }//for (i=0; i<iResult; i++)

    //重置文件指针。
    if (i < iResult)
    {
        if (iLineStart != 0)
        {
            //拷贝一行字符
            memcpy(pchBuf, pchBuf+iLineStart, i-iLineStart);
            //移动文件指针
            lseek(iFildes, i-iResult+1,	SEEK_CUR);
            return i-iLineStart;
        }
        else
        {
            //移动文件指针
            lseek(iFildes, i-iResult+1,	SEEK_CUR);
            return i;
        }
    }
    else
    {
        iTmpResult = read(iFildes, arrchTmpBuf,	256);
        if (iTmpResult == 0)
        {
            if (iLineStart != 0)
            {
                //拷贝一行字符
                memcpy(pchBuf, pchBuf+iLineStart, i-iLineStart);
                return i-iLineStart;
            }
            else
            {
                return i;
            }
        }
        else
        {
            //移动文件指针
            lseek(iFildes, 0-iTmpResult, SEEK_CUR);
            errno = EOVERFLOW;
            return -1;
        }
    }
}


/*
 * 函数名：         PutIniKeyString
 * 入口参数：        title
 *                      配置文件中一组数据的标识
 *                  key
 *                      这组数据中要读出的值的标识
 *                  val
 *                      更改后的值
 *                  filename
 *                      要读取的文件路径
 * 返回值：         成功返回  0
 *                  否则返回 -1
 */
int PutIniKeyString(const char *title,const char *key,const char *val,const char *filename)
{
    FILE *fpr, *fpw;
    int  flag = 0;
    char sLine[1024], sTitle[32], *wTmp;

    sprintf(sTitle, "[%s]", title);
    if (NULL == (fpr = fopen(filename, "r")))
        hlog("fopen original file");// 读取原文件
    sprintf(sLine, "%s.tmp", filename);
    if (NULL == (fpw = fopen(sLine,    "w")))
        hlog("fwrite tmp file");// 写入临时文件

    hlog(sTitle);

    while (NULL != fgets(sLine, 1024, fpr))
    {
        hlog(sLine);
        if (2 != flag) { // 如果找到要修改的那一行，则不会执行内部的操作
            wTmp = strchr(sLine, '=');
            if ((NULL != wTmp) && (1 == flag)) {
                if (0 == strncmp(key, sLine, wTmp-sLine)) { // 长度依文件读取的为准
                    flag = 2;// 更改值，方便写入文件
                    sprintf(wTmp + 1, "%s\n", val);
                }
            } else {
                if (0 == strncmp(sTitle, sLine, strlen(sLine) - 1)) { // 长度依文件读取的为准
                    flag = 1; // 找到标题位置
                }
            }
        }

        fputs(sLine, fpw); // 写入临时文件
    }
    fclose(fpr);
    fclose(fpw);

    sprintf(sLine, "%s.tmp", filename);
    hlog(sLine,filename);
    //    int ires= rename(sLine, filename);// 将临时文件更新到原文件
    ////    plib::mv
    //    plib::rm(filename);

    //    hlog(ires);
    //    return ires;
    //    remove(filename);
    hlog(plib::rm(filename));
    return plib::mv(sLine,filename);
}
/*
 *
 *	名称:	WritePrivateProfileString	()
 *
 *	参数1: pchSectionName		节名
 *	参数2: pchKeyName				键名
 *	参数3: pchString				包含键名及键值的字符串
 *	参数4: pchFileName			配置文件名
 *
 *	返回值:	若成功则返回0，否则返回-1。
 * 如果有，则会改值，如果没找到的话，不做任何操作，返回-3
 *	功能:	将一个Key和它的值写入指定的配置文件。
 *
*/
int plib_setConfString(const char *pchSectionName,const  char *pchKeyName,const char *pchString)
{
    int	iFd;									//文件句柄
    char arrchBuf[256];				//缓存
    int	iResult;							//返回值
    int	i;
    int	iSectionFinded;				//节名找到标志
    int	iTmpFd;								//临时文件句柄
    int	iTmpResult;						//临时返回值
    int	iTotalLen;						//总长度
    char arrchTmpBuf[256];		//临时缓冲区
    struct stat	struFileStat;
    char arrchTmpString[256];	//临时字符串

    //初始化数组
    memset(arrchBuf, 0,	256);
    memset(arrchTmpBuf,	0, 256);
    memset(arrchTmpString, 0,	256);

    //为变量赋初值
    iTotalLen=0;
    iSectionFinded=0;

    //打开指定的文件。
    //    iFd	=	open(gs_strConfPath,	O_RDWR);
    iFd	=	open(strPathConf.c_str(),	O_RDWR);
    if (iFd	== -1)
    {
        hlog(pstring()<<"open config file "<<strPathConf<<" fail");
        return -1;
    }

    fstat(iFd, &struFileStat);

    while(1)
    {
        //从文件中读一行。
        iResult	= clib_readLine(iFd, arrchBuf, 256);
        hlogone(iResult);
        hlogone(arrchBuf);
        if (iResult == -1)
        {
            //关闭文件
            close(iFd);
            return -2;
        }
        else if	(iResult == 0)
        {
            //这里注释，表示没有找到的话就不做任何操作
            //            if (iSectionFinded == 0)
            //            {
            //                sprintf(arrchBuf,	"\n[%s]\n",	pchSectionName);
            //                write(iFd, arrchBuf, strlen(arrchBuf));
            //            }

            //            sprintf(arrchBuf,	"%s=%s\n", pchKeyName, pchString);
            //            write(iFd, arrchBuf, strlen(arrchBuf));

            close(iFd);
            errno	=	ENOENT;
            return -3;
        }

        if (iSectionFinded ==	0)
        {
            //查找section。
            for	(i=0;	i<iResult; i++)
            {
                //忽略空格和制表符。
                if ((arrchBuf[i]==9) ||	(arrchBuf[i]==32))
                {
                    continue;
                }

                //若第i个字符不为'['，终止循环，从新读一行
                if (arrchBuf[i]	!= '[')
                {
                    break;
                }

                //若此节名与要查找的节名长度不同，终止循环，从新读一行
                if (arrchBuf[i+strlen(pchSectionName)+1] !=	']')
                {
                    break;
                }

                //若此节名与要查找的节名不同，终止循环，从新读一行
                if (strncmp(&arrchBuf[i+1],	pchSectionName,	strlen(pchSectionName))	!= 0)
                {
                    break;
                }
                else
                {
                    iSectionFinded = 1;
                    break;
                }
            }//for (i=0; i<iResult;	i++)
        }
        else
        {
            //查找key，并替换原来的值。
            for	(i=0;	i<iResult; i++)
            {
                //忽略空格和制表符。
                if ((arrchBuf[i]==9) ||	(arrchBuf[i]==32))
                {
                    continue;
                }

                if (arrchBuf[i]	!= '[')
                {
                    if ((arrchBuf[i+strlen(pchKeyName)]	!= 9)	&& (arrchBuf[i+strlen(pchKeyName)] !=	32)	&&
                            (arrchBuf[i+strlen(pchKeyName)]	!= '='))
                    {
                        break;
                    }

                    if (strncmp(&arrchBuf[i],	pchKeyName,	strlen(pchKeyName))	!= 0)
                    {
                        break;
                    }
                }

                //                iTmpFd = open("~123456.tmp", O_RDWR	|	O_CREAT	|	O_TRUNC, S_IRUSR | S_IWUSR);
                //windows这里要用这个
                iTmpFd = open("~123456.tmp", O_RDWR	|	O_CREAT	|	O_TRUNC, S_IREAD | S_IWRITE);
                if (iTmpFd ==	-1)
                {
                    close(iFd);

                    return -4;
                }

                if (arrchBuf[i]	== '[')
                {
                    //移动文件指针
                    lseek(iFd, 0-iResult-1,	SEEK_CUR);
                }

                iTmpResult = read(iFd, arrchTmpBuf,	256);
                while	(iTmpResult	!= 0)
                {
                    iTotalLen	+= iTmpResult;
                    write(iTmpFd,	arrchTmpBuf, iTmpResult);

                    iTmpResult = read(iFd, arrchTmpBuf,	256);
                }//while (iTmpResult !=	0)

                if (arrchBuf[i]	== '[')
                {
                    //移动文件指针
                    lseek(iFd, 0-iTotalLen,	SEEK_END);
                }
                else
                {
                    //移动文件指针
                    lseek(iFd, 0-iResult-iTotalLen-1,	SEEK_END);
                }

                //移动文件指针
                lseek(iTmpFd,	0-iTotalLen, SEEK_CUR);

                sprintf(arrchTmpString,	"%s=%s\n", pchKeyName, pchString);
                write(iFd, arrchTmpString, strlen(arrchTmpString));
                iTmpResult = read(iTmpFd,	arrchTmpBuf, 256);
                while	((iTmpResult !=	-1)	&& (iTotalLen	>	0))
                {
                    write(iFd, arrchTmpBuf,	iTmpResult);
                    iTotalLen	-= iTmpResult;
                    iTmpResult = read(iTmpFd,	arrchTmpBuf, 256);
                }//while ((iTmpResult	!= -1) &&	(iTotalLen > 0))

                if (arrchBuf[i]	!= '[')
                {
                    if (strlen(arrchTmpString) < (size_t)(iResult+1))
                    {
                        //                        ftruncate(iFd, struFileStat.st_size+strlen(arrchTmpString)-iResult-1);
                        //windows下改变文件大小用这个
                        _chsize_s(iFd, struFileStat.st_size+strlen(arrchTmpString)-iResult-1);
                    }
                }

                close(iFd);
                close(iTmpFd);
                unlink("~123456.tmp");
                return 0;
            }
        }//for (i=0; i<iResult;	i++)
    }//while (1)
}
