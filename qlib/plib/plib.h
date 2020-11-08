#ifndef PLIB_H
#define PLIB_H
//给别人引用xlib要放在最前面,不然会出错

//#include <afxwin.h>//无法使用 C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\atlmfc\include\afx.h:24: error: C1189: #error :  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]
#include <map>
#include <set>
#include <typeinfo>
#include <direct.h>
#include <thread>
#include <atomic>
#include <ctime>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <atltime.h>
#include <WinSock2.h>
//一定要把windows.h放到winsock2.h后,不然会出现重复定义问题
#include <windows.h>
#include <Ws2tcpip.h>
#include <TlHelp32.h>
#include <sstream>
#include <time.h>
#include <fstream>
#include <malloc.h>
#include <assert.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>
#include <stddef.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ShellAPI.h> //打开外部程序ShellExecute所需
#include <sstream>
#include <istream>
#include <ostream>
#include <algorithm>
#include <IPHlpApi.h>
#include <mutex>
#include <stack>
#include <functional>

#pragma execution_character_set("utf-8")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
using namespace std;
using namespace std::placeholders;
//别名
//这是为了给trans提供一个默认打印的函数
#define lenPack 64*1024 //一包tcp的长度
typedef unsigned char uchar;
typedef unsigned long long ulonglong;
typedef long long longlong;
#define bzero(pdata,size) memset(pdata,0,size);
#define cinit(pdata) memset(pdata,0,sizeof(pdata))//仅适用于此类型: char[xxx]不适用于指针
//用calloc，无需memset或bzero
#define HMALLOC(type,count) (type*)calloc(count,sizeof(type))
//全局变量，hlog中要用的，在头文件中extern，cpp中再申请一遍，头文件中不能赋值
extern std::string strPathLog;
extern std::string strPathConf;




//多线程,自动识别类型的日志打印,附带文件名,行数,以及变量名
#define hlogone(var) xplog(__FILE__,__LINE__,#var,var)
#define getvarname(var) #var
template <class T>
int xplog(const char* file, int line,const char* name,const  T & t)
{
    int iFd;
    //标准输入输出
    int iFd1 = 1;
    //下面开始写入
    std::ostringstream os;
    //变量名
    std::string strname(name);
    //判断,如果名字中有引号,说明是打印的hlog("xx")这种不是变量的,这样的直接就打一句话
    std::ostringstream ost;
    ost<<t;
    //如果是字符串,要加双引号,如果不是,直接判断名字和内容字符串是否相等
    if(strname==ost.str()||strname=="\""+ost.str()+"\""||strname.find("()<<")!=string::npos)
        os<<t;
    else
        os<<strname+" :  "<<t;
    std::string str=os.str();

    int len=str.size()+1024;
    char *acBuf=(char*)malloc(len);
    memset(acBuf,0,len);
    char     *pchIndex;
    time_t ts;
    char         pcHLOGPath[256 + 1];
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


//判断字符串是否为utf8
static bool common_isStringUTF8(string str)
{
    const char* arrchstr=str.c_str();
    unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
    unsigned char chr = *arrchstr;
    bool bAllAscii = true;
    for (unsigned int i = 0; arrchstr[i] != '\0'; ++i){
        chr = *(arrchstr + i);
        //判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
        if (nBytes == 0 && (chr & 0x80) != 0){
            bAllAscii = false;
        }
        if (nBytes == 0) {
            //如果不是ASCII码,应该是多字节符,计算字节数
            if (chr >= 0x80) {
                if (chr >= 0xFC && chr <= 0xFD){
                    nBytes = 6;
                }
                else if (chr >= 0xF8){
                    nBytes = 5;
                }
                else if (chr >= 0xF0){
                    nBytes = 4;
                }
                else if (chr >= 0xE0){
                    nBytes = 3;
                }
                else if (chr >= 0xC0){
                    nBytes = 2;
                }
                else{
                    return false;
                }
                nBytes--;
            }
        }
        else{
            //多字节符的非首字节,应为 10xxxxxx
            if ((chr & 0xC0) != 0x80){
                return false;
            }
            //减到为零为止
            nBytes--;
        }
    }
    //违返UTF8编码规则
    if (nBytes != 0) {
        return false;
    }
    if (bAllAscii){ //如果全部都是ASCII, 也是UTF8
        return true;
    }
    return true;
}
//只分割一个字符的
//vector<string> common_split(string str, string pattern)
static vector<string> common_split_one(string str, string pattern)
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
//多个,且去掉分隔符空格
static vector<string> common_split(string str, string pattern)
{

    string s=str;
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while(i != s.size()){
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while(i != s.size() && flag == 0){
            flag = 1;
            for(string_size x = 0; x < pattern.size(); ++x)
                if(s[i] == pattern[x]){
                    ++i;
                    flag = 0;
                    break;
                }
        }

        //找到又一个分隔符，将两个分隔符之间的字符串取出；
        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0){
            for(string_size x = 0; x < pattern.size(); ++x)
                if(s[j] == pattern[x]){
                    flag = 1;
                    break;
                }
            if(flag == 0)
                ++j;
        }
        if(i != j){
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;

}

//与linux一致,第二个参数为NULL
static int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year   = wtm.wYear - 1900;
    tm.tm_mon   = wtm.wMonth - 1;
    tm.tm_mday   = wtm.wDay;
    tm.tm_hour   = wtm.wHour;
    tm.tm_min   = wtm.wMinute;
    tm.tm_sec   = wtm.wSecond;
    tm. tm_isdst  = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
//通用时间类型
typedef struct _BASE_DATETIME {
    unsigned short JD;
    unsigned int JS;
} BASE_DATETIME;

typedef struct _DATETIME {
    int year;     //年
    int month;    //月
    int day;      //日
    int hour;     //时
    int min;      //分
    int sec;      //秒
    int msec;     //0.1毫妙
} DateTime;

/*********************************************************
    函数功能：
                检查输入的日期是否正确
    函数参数：
                year   ：年份（>=2000）
                month  ：月份（1～12）
                day    ：日（1～31,各月不同）
    函数返回值：
                  0：日期格式正确
                 -1：日期格式错误
**********************************************************/
static int CheckDate(unsigned int year, unsigned int month, unsigned int day) {
    if (year < 2000 || year > 5000)
        return -1;
    else if (month < 1 || month > 12)
        return -1;
    else if (day < 1 || day > 31)
        return -1;

    switch (month)   //4、6、9、11月每月30天，除2月外其它每月31天
    {
    case 2:
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            if (day > 29) return -1;
        } else if (day > 28) return -1;
        break;
    case 4:
        if (day > 30) return -1;
        break;
    case 6:
        if (day > 30) return -1;
        break;
    case 9:
        if (day > 30) return -1;
        break;
    case 11:
        if (day > 30) return -1;
        break;
    }
    return 0;
}


/*********************************************************
    函数功能：
                计算2000.1.1至输入日期的积日
    函数参数：
                year   ：年份
                month  ：月份（1～12）
                day    ：日（1～31）
    函数返回值：
                >=0：积日
                 -1：日期格式错误
**********************************************************/
static int GetJD(unsigned int year, unsigned int month, unsigned int day) {
    //判断日期取值是否正确
    if (CheckDate(year, month, day) < 0) {
        return -1;
    }
    unsigned int year2 = 2000;
    unsigned int month2 = 1;
    unsigned int day2 = 1;

    int nd, nm, ny; //new_day, new_month, new_year
    int od, om, oy; //old_day, oldmonth, old_year

    nm = (month2 + 9) % 12;
    ny = year2 - nm / 10;
    nd = 365 * ny + ny / 4 - ny / 100 + ny / 400 + (nm * 306 + 5) / 10 + (day2 - 1);

    om = (month + 9) % 12;
    oy = year - om / 10;
    od = 365 * oy + oy / 4 - oy / 100 + oy / 400 + (om * 306 + 5) / 10 + (day - 1);

    return (od - nd) + 1;
}


/*********************************************************
    函数功能：
                计算输入时间的积秒
    函数参数：
                hour ：时（0～23）
                min  ：分（0～59）
                sec  ：秒（0～59）
                msec ：0.1毫秒（0～9999）
    函数返回值：
                >=0：积秒
                 -1：时间格式错误
**********************************************************/
static int GetJS(unsigned short hour, unsigned short min, unsigned short sec, unsigned short msec) {
    if (hour >= 0 && hour < 24) {
        if (min >= 0 && min < 60) {
            if (sec >= 0 && sec < 60) {
                if (msec >= 0 && msec <= 9999)
                    return ((hour * 3600 + min * 60 + sec) * 10000 + msec);
                else
                    return -1;
            } else
                return -1;
        } else
            return -1;
    } else
        return -1;
}


/*********************************************************
    函数功能：
                计算当前日期的积日(自2000.1.1起)
    函数参数：
                无
    函数返回值：
                积日
**********************************************************/
static unsigned short GetCurrentJD() {
    time_t loctime;
    time(&loctime);
    struct tm tm1;
    loctime += 8 * 3600;
    tm1 = *gmtime(&loctime);   //
    return GetJD((tm1.tm_year + 1900), (tm1.tm_mon + 1), tm1.tm_mday);
}


/*********************************************************
    函数功能：
                计算当前时间的积秒(自0时0分0秒起)
    函数参数：
                无
    函数返回值：
                积秒(单位为0.1毫秒)
**********************************************************/
static unsigned int GetCurrentJS() {
    struct timeval a;
    //此处与linux不同,自定义gettimeofday,第二个参数时NULL
    //    struct timezone b;
    //    gettimeofday(&a, &b);
    gettimeofday(&a,NULL);
    return (((a.tv_sec + 28800) % 86400) * 10000 + a.tv_usec / 100); //28800=8*3600,补时区差8小时
}


/*********************************************************
    函数功能：
                计算当前日期时刻
    函数参数：
                无
    函数返回值：
                pdt：DateTime结构指针
**********************************************************/
static DateTime GetDateTime() {
    struct timeval a;
    //    struct timezone b;
    struct tm tm1;
    //    gettimeofday(&a, &b);
    gettimeofday(&a,NULL);
    a.tv_sec += 8 * 3600;   //补8小时时差
    tm1 = *gmtime((time_t*)&a.tv_sec);
    DateTime dt;
    dt.year = tm1.tm_year + 1900;
    dt.month = tm1.tm_mon + 1;
    dt.day = tm1.tm_mday;
    dt.hour = tm1.tm_hour;
    dt.min = tm1.tm_min;
    dt.sec = tm1.tm_sec;
    dt.msec = a.tv_usec / 100;
    return dt;
}

/*********************************************************
    函数功能：
                根据JD、JS计算日期时刻
    函数参数：
                JD 积日(自2000.1.1起)
                JS  天内积秒（0.1毫秒为单位，0～863999999）
                dt：DateTime结构指针，输出值
    函数返回值：
                0 成功
                -1 参数错误
**********************************************************/
static int GetDateTimeFromJDJS(unsigned short JD, unsigned int JS, DateTime &dt) {
    if (JS >= 863999999)//积秒不能大于863999999个单位的0.1毫秒（1秒）
    {
        return -1;
    }
    //计算年月日
    int Jd1, J, N1, L1, L2, L3, Y1, M1, MJd;
    JD -= 1;
    MJd = JD;
    Jd1 = (int) (2451545.0 + MJd);
    J = (int) (Jd1 + 0.5);            // 四舍五入
    // 计算
    N1 = 4 * (J + 68569) / 146097;
    L1 = J + 68569 - (N1 * 146097 + 3) / 4;
    Y1 = 4000 * (L1 + 1) / 1461001;
    L2 = L1 - (1461 * Y1) / 4 + 31;
    M1 = (80 * L2) / 2447;
    dt.day = L2 - (2447 * M1) / 80;
    L3 = M1 / 11;
    dt.month = (int) (M1 + 2 - 12 * L3);
    dt.year = (int) (100 * (N1 - 49) + Y1 + L3);
    //计算时分秒
    dt.msec = (int) (JS % 10000);
    dt.hour = (int) (JS / 36000000);
    dt.min = (int) (((JS / 10000) % 3600) / 60);
    dt.sec = (int) ((JS / 10000) % 3600 % 60);
    return 0;
}


static string GetTimeNhoursLater(int N) {
    string strRet;
    char acTemp[30] = {'\0'};
    unsigned short iJR = GetCurrentJD();
    unsigned int iJS = GetCurrentJS();
    int i10Hours = N * 3600 * 1000 * 10;
    int i24Hours = 24 * 3600 * 1000 * 10;
    if (i24Hours > i10Hours + iJS) {
        iJS += i10Hours;
    } else {
        iJS = i10Hours + iJS - i24Hours;
        iJR += 1;
    }
    DateTime stuDateTime;
    int iRet = GetDateTimeFromJDJS(iJR, iJS, stuDateTime);
    if (iRet == 0) {
        sprintf(acTemp, "'%d-%d-%d %d:%d:%d'",
                stuDateTime.year,
                stuDateTime.month,
                stuDateTime.day,
                stuDateTime.hour,
                stuDateTime.min,
                stuDateTime.sec);
        strRet = string(acTemp);
    }
    return strRet;
}


static BASE_DATETIME StringToBaseDateTime(string strTime) {
    BASE_DATETIME stuDateTime;
    if (strTime == "") {
        stuDateTime.JD = 0;
        stuDateTime.JS = 0;
        return stuDateTime;
    }
    string strYear = strTime.substr(0, 4);
    int iYear = atoi(strYear.c_str());

    string strTemp = strTime.substr(5);
    string::size_type index = strTemp.find_first_of('-');
    string strMonth = strTemp.substr(0, index);
    int iMonth = atoi(strMonth.c_str());

    strTemp = strTemp.substr(index + 1);
    index = strTemp.find_first_of(' ');
    string strDay = strTemp.substr(0, index);
    int iDay = atoi(strDay.c_str());

    strTemp = strTemp.substr(index + 1);
    index = strTemp.find_first_of(':');
    string strHour = strTemp.substr(0, index);
    int iHour = atoi(strHour.c_str());

    strTemp = strTemp.substr(index + 1);
    index = strTemp.find_first_of(':');
    string strMinitue = strTemp.substr(0, index);
    int iMinitue = atoi(strMinitue.c_str());

    strTemp = strTemp.substr(index + 1);
    string strSecond = strTemp;
    int iSecond = atoi(strSecond.c_str());

    stuDateTime.JD = GetJD(iYear, iMonth, iDay);
    stuDateTime.JS = GetJS(iHour, iMinitue, iSecond, 0);

    return stuDateTime;
}


static string GetCurrentTimeString(unsigned short iJR, unsigned int iJS) {
    string strRet;
    DateTime stuDateTime;
    char acTemp[30] = {'\0'};
    int iRet = GetDateTimeFromJDJS(iJR, iJS, stuDateTime);
    if (iRet == 0) {
        sprintf(acTemp, "'%04d-%02d-%02d %02d:%02d:%02d.%03d'",
                stuDateTime.year,
                stuDateTime.month,
                stuDateTime.day,
                stuDateTime.hour,
                stuDateTime.min,
                stuDateTime.sec,
                stuDateTime.msec / 10);
        strRet = string(acTemp);
    }
    return strRet;
}

static int pHLOGFunp(const char *file, int line,string str)//配套重载打印函数时不要加&
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
template<typename T>
void printarg(ostream& os,T a)
{
    //隔开，防止打印字符串时乱码不能分割，与序列化一个原理
    //但是不能用跟序列化一样的字符1，不然会导致序列化的混乱
    //    if(typeid(a).name()==typeid(char).name())
    //        hlogone("char");
    //    hlogone(typeid(a).name());
    //    hlogone(typeid(char).name());
    string str=(typeid(a).name());
    if(str.find("pstring")!=string::npos)
    {
        ostringstream oss;
        oss<<a;
        string strtmp=oss.str();
        string strlast=strtmp.substr(0,strtmp.size()-1);
        os<<strlast<<"\2";
    }
    else
        os<<a<<"\2";
}

template<class...Args>
void expand(Args... args)
{
    ostringstream os;
    int arr[] = {(printarg(os,args), 0)...};
    //或者改进为 std::initializer_list<int>{(printarg(args), 0)...};lumda
    string stres=os.str();
    //        hlogone(stres);
    //这个地方不能用多个字符串分割的，不然结果是空字符串的时候会自动去掉
    vector<string> ls=common_split_one(stres,string("\2"));
    ls.erase(ls.end()-1);
    //    for(int i=0;i<ls.size();i++)
    //    {
    //        hlogone(ls[i]);
    //    }
    const char* fileFullPath=ls[0].c_str();
    //    hlogone(file);
    vector<string> listfile=common_split_one(fileFullPath,"/");
    string fileNameOnly=listfile[listfile.size()-1];
    //    hlogone(fileNameOnly);
    const char* file=fileNameOnly.c_str();

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
        //        hlogone(listNamesOrigin.size());

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
    //    hlogone(listNames.size());
    //    for(int i=0;i<listNames.size();i++)
    //    {
    //        hlogone(listNames[i]);
    //    }
    //    hlogone(listValues.size());
    //    for(int i=0;i<listValues.size();i++)
    //    {
    //        hlogone(listValues[i]);
    //    }
    //整理了一遍之后，如果发现

    string strval;
    for(int i=0;i<listNames.size();i++)
    {
        //在此判断是否是单独字符串,或者是pstring()<<这种的行为
        //                hlogone(listNames[i]);
        //                hlogone(listValues[i]);
        //如果名字第一个字符和最后一个是",则说明是字符串,直接打印value,因为有可能会有这种的xfdsf",,wfew,
        if((listNames[i][0]=='"'&&listNames[i][listNames[i].size()-1]=='"')||
                (listNames[i].find("pstring()<<")!=string::npos||
                 listNames[i].find("pstring() <<")!=string::npos))
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
    pHLOGFunp(file,line,strval);
}

//支持多参数不同类型的联合打印
//支持函数嵌套带逗号，已处理
//hlog(itest,funtest(funtest2(funtest3(2),2),3,funtest2(6,8)),db,funtest2(2,3),funtest3(3));
#define hlog(...) expand(__FILE__,__LINE__,#__VA_ARGS__,__VA_ARGS__)

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
int clib_readLine(int iFildes, char *pchBuf, unsigned int uiNByte);

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
int plib_setConfString(const char *pchSectionName,const  char *pchKeyName,const char *pchString);
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
int PutIniKeyString(const char *title,const char *key,const char *val,const char *filename);

//关于socket，封装再xtcp，xudp，此处提供基本函数
int reconnectTcp(int iSock, const char* ip, int port, int iRetryCount, int iTimeoutTime);
//原有的
int ctcp_InitTcpSocket(void);
int InitUdpSocket(const char *pchIPAddr, int iPort, struct sockaddr_in *pstruAddr);
int InitSendGroupSocket(const char *pchIPAddr, const char *pchGroupIPAddr, int iGroupPort, struct sockaddr_in *pstruAddr);
int InitRecvGroupSocket(const char *pchIPAddr, const char *pchGroupIPAddr, int iGroupPort, struct sockaddr_in *pstruAddr);
//不带超时时间的连接,连不上会一直阻塞
int CreateConnection(int iSocket, const char *pchIPaddr, int iPort);
//超时,默认一秒
int CreateConnectionTimeout(int sock, const char *host, int port);
int ctcp_BindSocket(int iSocket, const char *pchIPaddr, int iPort);
int BindSocketCommon(int iSocket, const char *pchIPAddr, int iPort, struct sockaddr_in *pstruAddr);
int AcceptConnection(int iSocket);
int ctcp_CloseConnection(int iSocket);
int SendPacket(int iSocket, char *pBuffer, int uLength);
int RecvPacket(int iSocket, char *pBuffer, int uLength);

int RecvFullPacketTimeout(int iSocket, char *pBuffer, int iLength,int timeoutSec);//带超时的,默认不超时
int SendFullPacketTimeout(int iSocket, const char *pBuffer, int iLength, int timeoutSec);
//效果等同#LIBS+=-lWS2_32,不过写在代码里可以不用每次调用了
#pragma comment(lib, "WS2_32")
//转utf8,用于windows向linux发送字符串时转换,不然会乱码
static std::string common_string_To_UTF8(const std::string & str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
    ZeroMemory(pwBuf, nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char * pBuf = new char[nLen + 1];
    ZeroMemory(pBuf, nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr(pBuf);

    delete []pwBuf;
    delete []pBuf;

    pwBuf = NULL;
    pBuf  = NULL;

    return retStr;
}
static std::string common_string_To_GBK(const std::string &str)
{
    const char* src_str=str.c_str();
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if (wszGBK) delete[] wszGBK;
    if (szGBK) delete[] szGBK;
    return strTemp;
}

static std::string common_UTF8_To_string(const std::string & str)
{
    int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
    memset(pwBuf, 0, nwLen * 2 + 2);

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char * pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);

    WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr = pBuf;

    delete []pBuf;
    delete []pwBuf;

    pBuf = NULL;
    pwBuf = NULL;

    return retStr;
}
//递归获取目录下所有文件
static void common_getAllFilesAndPathsRecursion(string path, vector<string>& files,vector<string>& paths)
{
    //文件句柄
    long   hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;  //很少用的文件信息读取结构
    string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
    if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //            hlog(path);
            string stres=path;
            for(int i=0;i<stres.size();i++)
            {
                if(stres[i]=='\\')
                    stres[i]='/';
            }
            //            hlog(stres);
            if ((fileinfo.attrib &  _A_SUBDIR))  //比较文件类型是否是文件夹
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    //                    hlog(stres);
                    //                    files.push_back(p.assign(stres).append("/").append(fileinfo.name));
                    string strpath=p.assign(stres).append("/").append(fileinfo.name);
                    //                    hlog(strpath);
                    paths.push_back(strpath);
                    common_getAllFilesAndPathsRecursion(strpath, files,paths);
                }
            }
            else
            {
                //                hlog(stres);
                string strfile=p.assign(stres).append("/").append(fileinfo.name);
                //                hlog(strfile);
                files.push_back(strfile);
            }
        } while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
        _findclose(hFile);
    }
}
//递归获取路径下所有文件
static void common_getAllFilesRecurion(string path, vector<string>& files)
{
    //文件句柄
    long   hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;  //很少用的文件信息读取结构
    string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
    if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //            hlog(path);
            string stres=path;
            for(int i=0;i<stres.size();i++)
            {
                if(stres[i]=='\\')
                    stres[i]='/';
            }
            //            hlog(stres);
            if ((fileinfo.attrib &  _A_SUBDIR))  //比较文件类型是否是文件夹
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    //                    hlog(stres);
                    //                    files.push_back(p.assign(stres).append("/").append(fileinfo.name));
                    string strpath=p.assign(stres).append("/").append(fileinfo.name);
                    hlog(strpath);
                    common_getAllFilesRecurion(strpath, files);
                }
            }
            else
            {
                //                hlog(stres);
                string strfile=p.assign(stres).append("/").append(fileinfo.name);
                hlog(strfile);
                files.push_back(strfile);
            }
        } while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
        _findclose(hFile);
    }
}

//跳过[之前的所有
static void common_skipDescription(istream &is){
    while(1)
    {
        char ch=is.get();
        if(ch=='[')
        {
            break;
        }
    }
}

//is提取字符串,跳过空格,根据[]来判断
static string common_istreamToString(istream& is)
{
    string all;
    char ch=is.get();
    while(ch!=']')
    {
        all.push_back(ch);
        ch=is.get();

    }
    return all;
}

static bool common_isIncludeChinese(string stro)//返回0：无中文，返回1：有中文
{
    const char *str=stro.c_str();
    char c;
    while(1)
    {
        c=*str++;
        if (c==0) break;  //如果到字符串尾则说明该字符串没有中文字符
        if (c&0x80)        //如果字符高位为1且下一字符高位也是1则有中文字符
            if (*str & 0x80) return 1;
    }
    return 0;
}
static string common_getSystem()
{
    //这样会弹黑框,界面程序不允许
    //    int judge = system("cls");
    //    if(judge == 0)
    //    {
    //        return "windows";
    //    }
    //    else
    //    {
    //        return "linux";
    //    }

#ifdef __unix

#ifdef __linux
    return "linux";
#endif
#ifdef __sun
#ifdef __sparc
    return ("Sun SPARC");
#else
    return ("Sun X86");
#endif
#endif
#ifdef _AIX
    return ("AIX");
#endif

#else

#ifdef WINVER
    return "windows";
#endif

#endif
}
static string common_toChinese(string str)
{

    bool isHaveChinese=common_isIncludeChinese(str);
    //    hlog(isHaveChinese);
    //如果不含有中文,直接返回
    if(!isHaveChinese)
        return str;

    string strSystem=common_getSystem();
    bool iutf8=common_isStringUTF8(str);

    //    hlog(strSystem,iutf8);
    //如果是windows,且含有中文,且字符串不是GBK而是UTF8,则要转换成GBK
    if(strSystem=="windows"&&iutf8)
    {
        return common_string_To_GBK(str);
    }
    else if(strSystem=="linux"&&!iutf8)
    {
        //如果是linux,且含有中文,且不是UTF8,要转成UTF8
        return common_string_To_UTF8(str);
    }
    return str;
}

//cmd执行结果
//CmdExc cmd("ipconfig");
//hlog(cmd.getOutput());
class CmdExc
{
public:
    CmdExc(std::string cmd,std::string mode="rt")
    {
        m_fp__=_popen(cmd.c_str(),mode.c_str());
        char buf[2048]={0};
        if(NULL != m_fp__){
            int read_len;
            while((read_len=fread(buf,sizeof(buf)-1,1,m_fp__))>0){
                m_strOutput__+=buf;
                memset(buf,0,sizeof(buf));
            }
        }
    }

    virtual ~CmdExc()
    {
        if(NULL != m_fp__){
            _pclose(m_fp__);
        }
    }

    std::string getOutput() const
    {
        return m_strOutput__;
    }

private:
    std::string m_strOutput__;
    FILE* m_fp__;
};




/* Type define */
typedef unsigned char byte;
typedef unsigned long ulong;

using std::string;
using std::ifstream;

/* MD5 declaration. */
class MD5 {
public:
    MD5();
    MD5(const void *input, size_t length);
    MD5(const string &str);
    MD5(ifstream &in);
    void update(const void *input, size_t length);
    void update(const string &str);
    void update(ifstream &in);
    const byte* digest();
    string toString();
    void reset();
private:
    void update(const byte *input, size_t length);
    void final();
    void transform(const byte block[64]);
    void encode(const ulong *input, byte *output, size_t length);
    void decode(const byte *input, ulong *output, size_t length);
    string bytesToHexString(const byte *input, size_t length);

    /* class uncopyable */
    MD5(const MD5&);
    MD5& operator=(const MD5&);
private:
    ulong _state[4];	/* state (ABCD) */
    ulong _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
    byte _buffer[64];	/* input buffer */
    byte _digest[16];	/* message digest */
    bool _finished;		/* calculate finished ? */

    static const byte PADDING[64];	/* padding for calculate */
    static const char HEX[16];
    static const size_t BUFFER_SIZE = 1024;
};


//使用类代替plib中的函数,首先创建license,仅第一次,之后必须去掉创建license这个
class Expire
{
    //time struct for checking expire
    typedef struct STRUCT_TIME
    {
        CTime timeInit;
        CTime timeLast;
        bool isExpire;
    }STRUCT_TIME;
    long long expireSeconds;
    string strpath;
public:
    Expire(long long expireSeconds,string strpath="C:\\Program Files (x86)\\system.dat")
    {
        this->expireSeconds=expireSeconds;
        this->strpath=strpath;
    }
    long long getExpireSeconds()
    {
        return this->expireSeconds;
    }
    string getStrPath()
    {
        return this->strpath;
    }

    //创建license文件
    bool createLicense();
    bool isExpire();
    //将线程函数声明为静态函数或者友元函数，但是静态函数只能访问静态成员变量，所以利用参数将this传进函数来，就可以通过该指针访问类成员变量和函数了。具体例子：
    static DWORD WINAPI thread(LPVOID pm);
    void startCheck();
};
//time struct for checking expire
typedef struct STRUCT_TIME
{
    CTime timeInit;
    CTime timeLast;
    bool isExpire;
}STRUCT_TIME;





bool readConfig(const string& filename, map<string, string>& mContent, const char* section);
map<string, string> readConfig(const string filename, const string section);
string readConfig(const string filename, const string section,const string key);
bool AnalyseLine(const string & line, string & key, string & val);
void Trim(string & str);
bool IsSpace(char c);
//重写的vector和string类




//序列化方案
template <class T>
static string common_toString(T t)//与类中重载的对应
{
    stringstream ss;
    ss<<t;
    return ss.str();
}
template <class T>
static void common_toClass(string str,T& t)
{
    stringstream ss(str);
    ss>>t;
}
template <class T>
static T common_toClass(string str)
{
    T t;
    stringstream ss(str);
    ss>>t;
    return t;
}
//is跳过n个字符
static void common_skipCountch(istream &is,int len)
{
    for(int i=0;i<len;i++)
    {
        char ch=   is.get();
    }
}


//将十六进制的字符串转换成char
static char common_hstrtoch(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return (char)n;
}
template <class T>
class plist :public vector<T>
{
public:
    plist(){}

    plist(size_t count)
    {
        T t;
        for(int i=0;i<count;i++)
        {
            this->push_back(t);
        }
    }
    plist(size_t count,T t)
    {
        for(int i=0;i<count;i++)
        {
            this->push_back(t);
        }
    }

    plist(vector<T> v)
    {
        for(int i=0;i<v.size();i++)
        {
            this->push_back(v[i]);
        }
    }

    void append(T t)
    {
        this->push_back(t);
    }

    T& getDataStart()
    {
        return this->at(0);
    }
    T& getDataEnd()
    {
        return this->at(this->size()-1);
    }
    bool contains(T t)
    {
        if(std::find(this->begin(),this->end(),t)!=this->end())
            return true;
        else
            return false;
    }

    void sortASC()
    {
        sort(this->begin(),this->end());
    }
    void sortDESC()
    {
        sort(this->rbegin(),this->rend());
    }
    //返回 索引
    int find(T t)
    {
        typename plist<T>::iterator it=std::find(this->begin(),this->end(),t);
        if(it==this->end())
        {
            return -1;
        }
        return it-this->begin();
    }


    ////strDES是要找的目标,stri是列表内部的循环
    //    bool fun_pd(pstring strDES,pstring stri)
    //    {
    //        if(stri==strDES+"111")
    //            return true;
    //        return false;
    //    }
    //    int main(int argc, char *argv[])
    //    {
    //        plist<pstring> lstr;
    //        lstr.append("xlfd");
    //        lstr.append("zero");
    //        lstr.append("nidaye111");
    //        hlog(lstr);
    //        hlog(lstr.findfun("nidaye",fun_pd));
    ////因为find_if只有一个参数,所以要用bind处理转换
    //    int findfun(T tfind, bool (*fun_condition)(T t, T tf)) {
    //        typename plist<T>::iterator it = std::find_if(this->begin(),
    //                                                      this->end(), std::bind(fun_condition, tfind, placeholders::_1));
    //        if (it == this->end()) {
    //            return -1;
    //        }
    //        return it - this->begin();
    //    }
    /*
         * 该函数覆盖了根据结构体字段查找以及直接查找结构体 T2可以是T
         * class CTest
    {
    public:
        int id;
        double db;
        pstring str;
        CTest()
        {
            id=3;
            db=2.43;
            str="xlfd";
        }
        CTest(int wid,double wdb,pstring wstr)
        {
            id=wid;
            db=wdb;
            str=wstr;
        }
        friend ostream& operator<<(ostream& os,CTest x)
        {
            os<<x.id<<" "<<x.db<<" "<<x.str;
            return os;
        }
        bool operator==(CTest t2)
        {
            if(this->id==t2.id&&this->db==t2.db&&this->str==t2.str)
                return true;
            return false;
        }
    };
    bool fun_find(double ifind,CTest t)
    {
        if(t.db==ifind)
            return true;
        return false;
    }
             pmap<pstring,CTest> mt;
        hlog(mt["xlfd"]);
        plist<CTest> ltest;
        ltest.append(CTest(111,1.11,"aaaa"));
        CTest t2(222,2.22,"bbbbbb");
        ltest.append(t2);
        hlog(ltest);
        hlog(ltest.find(CTest(222,2.22,"bbbbbb")));
        hlog(ltest.findx(2.22,fun_find));
         */
    template<class T2>
    int findfun(T2 tfind, bool(*fun_condition)(T2 tfind, T tinner)) {
        typename plist<T>::iterator it = std::find_if(this->begin(), this->end(),
                                                      std::bind(fun_condition, tfind, placeholders::_1));
        if (it == this->end()) {
            return -1;
        }
        return it - this->begin();
    }
    //去重复,自定义结构体需要重载<和==
    void deleteRepeat()
    {
        sort(this->begin(),this->end());
        this->erase(unique(this->begin(), this->end()), this->end());
    }

    //erase int pos
    bool deleteByIndex(size_t pos)
    {
        if(pos>=this->size())
        {
            string info="pos>=this->size()";
            hlog(info);
            return false;
        }
        this->erase(this->begin()+pos);
        return true;
    }

    bool deleteBegin()
    {
        if(this->size()==0)
            return false;
        this->erase(this->begin());
        return true;
    }
    bool deleteEnd()
    {
        if(this->size()==0)
            return false;
        this->erase(this->end()-1);
        return true;
    }

    void clear()
    {
        int len=this->size();
        for(int i=0;i<len;i++)
            this->erase(this->begin());
    }

    //加上这个才能提示
    T& operator[](size_t i)
    {
        assert(i<this->size());
        return ((T&)(this->at(i)));
    }

    friend ostream& operator<<(ostream &os,plist *xv)
    {
        os<<"len: "<<xv->size()<<" pvector\n{\n";
        for(int i=0;i<xv->size();i++)
        {
            os<<xv->at(i)<<"\n";
        }

        os<<"}";
        return os;
    }
    friend ostream& operator<<(ostream &os,plist xv)
    {
        os<<"len: "<<xv.size()<<" pvector\n{\n";
        for(int i=0;i<xv.size();i++)
        {
            os<<xv[i]<<"\n";
        }
        os<<"}";
        return os;
    }
    friend istream & operator>>(istream &is,plist &xv)
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
    plist &operator<<(const T &data)
    {
        this->append(data);
        return *this;
    }
    //转为vector
    vector<T> toVector()
    {
        vector<T> v;
        for(int i=0;i<this->size();i++)
        {
            v.push_back(this->at(i));
        }
        return v;
    }


    //     //保存,有序列化之后不需要这种传统方式了,直接用
    //    plib::saveSerialization
    //    plib::loadSerialization
    int save(string path)
    {
        FILE *fp;
        if((fp=fopen(path.c_str(),"wb"))==NULL)
        {
            return -1;
        }
        size_t len=this->size();
        fwrite(&len,sizeof(size_t),1,fp);
        int i;
        for(i=0;i<this->size();i++)
        {
            fwrite(&(this->at(i)),sizeof(T),1,fp);
        }
        fclose(fp);
        return 0;
    }
    /*
     * 读取向量
     * vector<test> vr=pvector<TASK_STATUS_STRUCT>::load(path);
     */
    static plist<T> load(string path)
    {
        plist<T>  lr;
        FILE *fp;
        if((fp=fopen(path.c_str(),"rb"))==NULL)
        {
            return lr;
        }
        //首先要清空lr，不然一直增长
        lr.clear();
        size_t len=0;
        fread(&len,sizeof(size_t),1,fp);
        //        XCLOG_INT(len);
        int i;
        for(i=0;i<len;i++)
        {
            void* rd=(void*)malloc(sizeof(T));
            fread(rd,sizeof(T),1,fp);
            lr.push_back((*(T*)rd));
            free(rd);rd=NULL;
        }
        fclose(fp);
        return lr;
    }
};


template<class T1, class T2>
class pmap : public map<T1, T2> {
public:
    pmap() {}


    friend ostream &operator<<(ostream &os, pmap *xv) {
        os << "len: " << xv->size() << " pmap[";
        for (typename map<T1, T2>::iterator iter = xv->begin(); iter != xv->end(); iter++) {
            os << iter->first << ":" << iter->second <<" ";
        }
        os << "]";
        return os;
    }


    friend ostream &operator<<(ostream &os, pmap xv) {
        os << "len: " << xv.size() << " pmap[";
        for (typename map<T1, T2>::iterator iter = xv.begin(); iter != xv.end(); iter++) {
            os << iter->first << ":" << iter->second << " ";
        }
        os << "]";
        return os;
    }

    friend istream &operator>>(istream &is, pmap &p) {
        common_skipCountch(is, 5);
        size_t len;
        is >> len;
        //        hlog(len);
        common_skipCountch(is, 6);
        for (size_t i = 0; i < len; i++) {
            T1 t1;
            T2 t2;
            is >> t1;
            common_skipCountch(is, 1);
            is >> t2;
            //            p[t1] = t2;
            p.add(t1,t2);
            common_skipCountch(is, 1);//跳过空格
        }
        common_skipCountch(is, 1);
        return is;
    }

    //typename 迭代申请时必须加这个
    plist<T1> getKeys() {
        plist<T1> listkey;
        for (typename map<T1, T2>::iterator iter = this->begin(); iter != this->end(); iter++)
            listkey.push_back(iter->first);
        return listkey;
    }

    //typename 迭代申请时必须加这个
    plist<T2> getValues() {
        plist<T2> listvalue;
        for (typename map<T1, T2>::iterator iter = this->begin(); iter != this->end(); iter++)
            listvalue.push_back(iter->second);
        return listvalue;
    }

    T1 getKey(size_t i) {
        assert(i<this->size());
        return this->getKeys()[i];
    }

    //引用,可以直接用这个改了,除了m[key]=val之外
    T2& getValue(size_t i) {
        assert(i<this->size());
        //        return this->getValues()[i];
        typename map<T1,T2>::iterator it=this->begin();
        for(int index=0;index<i;index++)
        {
            it++;
        }
        return it->second;
    }
    //先看看有没有,如果有就覆盖,如果没有就增加
    void add(T1 key, T2 val) {
        if(this->find(key)==this->end()){
            this->insert(pair<T1, T2>(key, val));
        } else{
            this->at(key)=val;
        }
    }
    //这个改造是使map[key].可提示,并且没有key时不再插入,插入统一用add
    //便于维护列表
    T2 &operator[](T1 key) {
        assert(this->containsKey(key));
        return (T2 &) (this->at(key));
    }


    bool containsKey(T1 t1) {
        //        plist<T1> listkey = this->getKeys();
        //        return listkey.contains(t1);
        return this->find(t1) != this->end();
    }

    bool containsValue(T2 t2) {
        plist<T2> listval = this->getValues();
        return listval.contains(t2);
    }

    bool deleteByKey(T1 t) {
        typename map<T1, T2>::iterator iter;
        for (iter = this->begin(); iter != this->end(); iter++) {
            //找到t，删除
            if (iter->first == t) {
                //                this->erase(iter);
                break;
            }
        }
        if(iter==this->end())
        {
            return false;
        }
        else
        {
            this->erase(iter);
            return true;
        }
    }
};
//1字节的都要用这个,需要打印0x十六进制时使用
class pbyte
{
public:
    unsigned char data;
    pbyte()
    {

    }
    pbyte(char ch)
    {
        this->data=ch;
    }
    char toChar()
    {
        return data;
    }

    friend ostream& operator<<(ostream& os,pbyte* x)
    {
        //虽然是长度5，但是到流里面是4
        char ac[5];
        memset(ac,0,sizeof(ac));
        //02能保证一定是长度4，即使是\0
        sprintf(ac,"0x%02X",x->data);
        os<<ac;
        return os;
    }
    //只是打印的时候打成0，实际上值还是\0，不然的话打印出问题
    //直接用16进制打印也没问题
    friend ostream& operator<<(ostream& os,pbyte x)
    {
        //虽然是长度5，但是到流里面是4
        char ac[5];
        memset(ac,0,sizeof(ac));
        //02能保证一定是长度4，即使是\0
        sprintf(ac,"0x%02X",x.data);
        os<<ac;
        return os;
    }
    friend istream& operator>>(istream& is,pbyte& x)
    {
        char ar[4];
        for(int i=0;i<4;i++)
        {
            char ch=is.get();
            ar[i]=ch;
        }
        x.data=common_hstrtoch(ar);

        return is;
    }
    bool operator ==(pbyte &temp)
    {
        if(this->data==temp.data)
            return true;
        return false;
    }
    bool operator ==(char dw)
    {
        if(this->data==dw)
            return true;
        return false;
    }
    pbyte& operator=(const char &dw)
    {
        this->data=dw;
        return *this;
    }
};

//为了序列化传输数据用的
//两种方式,这里使用的是直接继承的方式,也可以内部成员包装,太low没用
//而且有些函数不用重写了比如size等
//如果发送大的数据会产生效率问题,这是因为char数组最快,所以这个只是娱乐用
//不适合发送大文件,读取大文件存储,
class plistbyte:public plist<pbyte>
{
public:
    plistbyte()
    {

    }
    plistbyte(char* data,int len)
    {
        this->setData(data,len);
    }
    plistbyte(plist<pbyte> ls)
    {
        this->assign(ls.begin(),ls.end());
    }

    //不能挨个赋值,不然特别慢,这类似memcpy
    void setData(char* data,int len)
    {
        this->assign(data,data+len);
    }

    //获取char*数据,长度为size
    char* getData()
    {
        return (char*)(&(*(this->begin())));
    }

    friend ostream& operator<<(ostream &os,plistbyte bs)
    {
        os<<(plist<pbyte>)bs;
        return os;
    }
    friend istream& operator>>(istream &is,plistbyte &bs)
    {
        is>>(plist<pbyte>&)bs;
        return is;
    }
};


//因为有中文乱码问题,所以直接封装到这里面togbk,但是会乱码,还是去掉吧
class pstring:public string
{

public:
    pstring()
    {
    }

    //构造函数都用这一个,所以只要改这个就行
    void init(const char* arrchData,int len=0)
    {
        //不写len就是直接赋值,写了就指定长度
        if(len==0)
            this->assign(arrchData);
        else
            this->assign(arrchData,len);
    }

    pstring(string str)
    {
        this->init(str.c_str(),str.size());
    }
    //是否重载了构造函数，就不需要重载赋值运算符，自己就自动了
    pstring(const char *acstr)
    {
        this->init(acstr);
    }
    pstring(const char* acstr,int len)
    {
        this->init(acstr,len);
    }
    //找到所有字符串索引
    plist<int> findIndexAll(pstring strfind)
    {
        plist<int> li;
        int index=0;
        while(1)
        {
            int indexLast=index;
            //            hlog(index);
            index=this->find(strfind,indexLast+1);
            //            hlog(index);
            if(index==string::npos)
                break;
            li.append(index);
        }
        return li;
    }
    //获取去掉空格的字符串
    pstring getStringNoSpace()
    {
        plist<pstring> lstr=this->split(" ");
        pstring stres="";
        for(int i=0;i<lstr.size();i++)
        {
            stres+=lstr[i];
        }
        return stres;
    }

    //分割字符串
    plist<pstring> split(string pattern)
    {
        pstring stro=*this;
        vector<string> vres=common_split(stro,pattern);
        plist<pstring> pres;
        for(int i=0;i<vres.size();i++)
        {
            pres.append(vres[i]);
        }
        return pres;
    }
    //    "12212"这个字符串的所有"12"都替换成"21"，结果是什么21221一次替换
    pstring &replaceOnce(const  string&  old_value, const   string&   new_value)
    {
        for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())
        {
            if(   (pos=this->find(old_value,pos)) != string::npos   )
            {
                this->replace(pos,old_value.length(),new_value);
            }
            else
            {
                break;
            }
        }
        return   *this;
    }
    //    "12212"这个字符串的所有"12"都替换成"21"，结果是什么22211迭代递归替换,也就是替换一次
    //之后发现还有12,继续,直到这个字符串没有12全替换成21为止
    pstring&   replace_all(const  string&  old_value, const  string&  new_value)
    {
        while(true)
        {
            string::size_type   pos(0);
            if(   (pos=this->find(old_value)) != string::npos   )
            {
                this->replace(pos,old_value.length(),new_value);
            }
            else
            {
                break;
            }
        }
        return   *this;;
    }
    bool contain(string str)
    {
        if(this->find(str)==string::npos)
            return false;
        else
            return true;
    }

    bool isNum();
    friend ostream& operator<<(ostream& os,pstring str)
    {
        //根据\0来分割,呵呵呵,发现os<<的时候遇到\0会停止，所以ppair就会显示不对
        //        os<<"pstring["<<str.data()<<"]";

        os<<str.data()<<'\0';
        return os;
    }

    friend istream& operator>>(istream& is,pstring& pstr)
    {
        pstr.clear();
        //        common_skipDescription(is);
        //        char ch=is.get();
        //        //通过这个结尾来判断
        //        while(ch!=']')
        //        {
        //            pstr.push_back(ch);
        //            ch=is.get();
        //        }
        //        return is;

        char ch=is.get();
        while(ch!='\0')
        {
            pstr.push_back(ch);
            ch=is.get();
        }
        return is;
    }
    template <class T>
    pstring& operator<<(const T &data)
    {
        stringstream ss;
        if(typeid(T)==typeid(pstring))
        {
            //<<pstring时因为有增加\0所以要特殊处理
            pstring stres=*(pstring*)(&data);
            //            hlogone(stres.size());
            //            hlogone(stres[stres.size()-1]);
            //            string stres2=stres;
            //            hlog(stres2.size());
            ss<<stres.c_str();
            //            hlogone("is self");
        }
        else
            ss<<data;
        this->append(ss.str());
        return *this;
    }

};

//两种方式,这里使用的是直接继承的方式,也可以内部成员包装,太low没用
class pliststring:public plist<pstring>
{
public:
    pliststring(){}
    pliststring(plist<pstring> ls)
    {
        for(int i=0;i<ls.size();i++)
        {
            this->append(ls[i]);
        }
    }
    pstring join(pstring strdem)
    {
        if(this->size()==0)
            return "";
        pstring stres;
        for(int i=0;i<this->size()-1;i++)
        {
            stres=stres+this->at(i)+strdem;
        }
        stres+=this->at(this->size()-1);
        return stres;
    }
    friend ostream& operator<<(ostream &os,pliststring pl)
    {
        os<<(plist<pstring>)pl;
        return os;
    }
    friend istream& operator>>(istream &is,pliststring &pl)
    {
        is>>(plist<pstring>&)pl;
        return is;
    }

    pliststring &operator<<(const pstring &str)
    {
        this->append(str);
        return *this;
    }
};

//set有自动排序,去重复的功能
template <class T>
class pset:public set<T>
{
public:
    pset(){}

    void append(T t)
    {
        this->insert(t);
    }
    friend ostream& operator<<(ostream& os,pset *s)
    {

        os<<"len: "<<s->size()<<" pset[";
        //遍历数据，用迭代器遍历数据
        //linux下需要加typename，不然不识别
        for (typename set<T>::iterator it = s->begin(); it != s->end();it++)
        {
            T t=*it;
            os << t<<" ";//不能直接用*it,不知道为啥,以后就这样吧
        }

        os<<"]";
        //这里用到了set中的元素已经从小到大排好序的性质
        return os;
    }

    friend ostream& operator<<(ostream& os,pset s)
    {

        os<<"len: "<<s.size()<<" pset[";
        //遍历数据，用迭代器遍历数据
        //linux下需要加typename，不然不识别
        for (typename set<T>::iterator it = s.begin(); it != s.end();it++)
        {
            T t=*it;
            os << t<<" ";//不能直接用*it,不知道为啥,以后就这样吧
        }

        os<<"]";
        //这里用到了set中的元素已经从小到大排好序的性质
        return os;
    }

    friend istream& operator>>(istream &is,pset &p)
    {
        common_skipCountch(is,5);
        size_t len;
        is>>len;
        hlog(len);
        common_skipCountch(is,6);
        for(int i=0;i<len;i++)
        {
            T t;
            is>>t;
            p.insert(t);
            common_skipCountch(is,1);
        }
        common_skipCountch(is,1);
        return is;
    }
};

enum typeServerOrClient{server,client};
//正常还是组播
enum typeNormalOrGroup{normal,group};
typedef struct cudp
{
    int type;

    enum typeServerOrClient typeSorC;  //服务端还是客户端
    enum typeNormalOrGroup typeNorG;   //正常还是组播
    int sock;
    int port;
    char ip[20];


    struct sockaddr_in  struUdpSendAddr;         /*组播地址结构*/
    int (*send)(struct cudp* udp,const char* data,int ilength);
    int (*recv)(struct cudp* udp,char* data,int ilength);
    int (*recvTimeout)(struct cudp* udp,char* data,int ilength,int timeoutsecond);
    int (*destroy)(struct cudp* udp);
}cudp;
//自定义
//普通udp
//server
int cudpDestroy(cudp *udp);
//当ipgroup==NULL时为正常，否则为组播,iplocal如果是本机的，要用clib获取，不要写127.0.0.1
cudp* cudpInit(const char *iplocal, int port, enum typeServerOrClient typeSorC,  const char *ipgroup);
cudp* cudpRecvInit(const char *pchIPAddr, int iPort);
//client
cudp* cudpSendInit(const char *pchIPAddr, int iPort);
//group server
static cudp* cudpGroupRecvInit(const char *pchIPAddr, const char *pchGroupIPAddr, int iGroupPort);
//group client
static cudp* cudpGroupSendInit(const char *pchIPAddr,const char *pchGroupIPAddr, int iGroupPort);
int cudpSend(cudp *udp, const char *data, int ilength);
int cudpGroupSend(cudp *udp, const char *data, int ilength);
int cudpRecv(cudp *udp, char *data, int ilength);
int cudpRecvTimeout(cudp *udp, char *data, int ilength,int itmeout);
void ctcp_CloseConnectionOld(int iSocket);

//原有的
int cudp_InitUdpSocket(const char *pchIPAddr, int iPort, struct sockaddr_in *pstruAddr);
static int cudp_InitSendGroupSocket(const char *pchIPAddr, const char *pchGroupIPAddr, int iGroupPort,
                                    struct sockaddr_in *pstruAddr);
static int cudp_InitRecvGroupSocket(const char *pchIPAddr, const char *pchGroupIPAddr, int iGroupPort,
                                    struct sockaddr_in *pstruAddr);
int cudp_BindUdpSocket(int iSocket, const char *pchIPAddr, int iPort, struct sockaddr_in *pstruAddr);
int cudp_ServerInit(char *pchIPAddr, int iPort);


extern std::mutex mut;
typedef struct STRUCT_HEADER
{
    int type;
    int length;
    char arrchType[50];//一般不超过50，类型
}STRUCT_HEADER;

/*
 * 需要自己关闭socket，调用close
 * 考虑多线程的问题,一个界面如果有刷新线程,建议另来一个连接
 * 一个连接干一个事情,server端是来一个连接新建一个线程单独处理不影响
 * client端则要自己注意,一个线程干一个事情,用recvx的时候注意一下,因为recvx用到了全局变量data,不要多个线程收就ok,不然会错乱
 */
class ptcp
{
public:
    string strCtcptype;
    pstring data;
    plist<ptcp*> listclient;

    string ip;
    int port;
    int sock;
    string type;
    int length;
    bool bIsConnecting=false;//判断是否在重连,防止多线程同时重连
    const char *getData();//接收的数据，用于转换
    pstring getDataToString()
    {
        return this->data;
    }

    //因为初始化可能失败,所以吧不会失败的放到这里面,会失败的放到init中.可智能判断是否是域名,
    //若是域名,则connect和reconnect中会自动进行域名解析判断,这样是为了防止没网的时候发数据
    //但没进入到重连阶段,挂在域名解析了
    ptcp(string ip, int port)
    {
        this->init(ip,port);
    }

    ptcp()
    {

    }
    //接受连接时用,只会是server //该方法不需要init
    ptcp(int iSockAccept,string ip,int port);
    ~ptcp();//因为连接的特殊性，一般手动关闭，所以在此不释放，否则导致多次释放的问题，也不能重载<<

    int init(string ip, int port);

    //    bool bConnected()
    //    {
    //        struct tcp_info info;
    //        int len=sizeof(info);
    //        getsockopt(sock, IPPROTO_TCP, tcp_info, &info, (socklen_t *)&len);
    //        if((info.tcpi_state==TCP_ESTABLISHED))  // 则说明未断开  else 断开
    //            return true;
    //        return false;
    //    }

    //默认超时时间1秒
    int connect();//当是客户端的使用用这个
    //针对客户端，断线重连,重试次数，次数之间的间隔,默认参数代表无限重连
    //加了是否在重连判断,因此可以在任何情况下连不上就调用该函数,可以保证
    //同一时间只有一个在重连,重连的标准是发送或者接收失败,只有这两种情况
    int reconnect(int iretrycount=-1,int itimesecond=1);
    //连接失败后线程重连,不需要提示的话就这样,如果需要提示则在ui中封装
    //void MainWindow::slotThreadConnect()
    //{
    //    if(ptc->connect()<0)
    //    {
    //        sigShowStatusBar("连接服务器失败!重连中......");
    //        ptc->reconnect();
    //    }
    //    sigShowStatusBar("连接服务器成功");
    //}
    //或者做个回调函数
    //这个就包括了连接和线程重连,可以每次发送或者接收失败都调用一次connectx
    //同时reconnect中有判断,可以保证始终只有一个线程在重连
    int connectx()
    {
        //先关闭上一个,防止超时接收但是上一个连接还是正常的情况会连接越来越多
        this->close();
        if(this->connect()<0)
        {
            hlog(pstring()<<"连接"<<this->ip<<":"<<this->port<<"失败,重连中...");
            std::thread(&ptcp::threadReconnect,this).detach();
            return -1;
        }
        hlog(pstring()<<"连接"<<this->ip<<":"<<this->port<<"成功!");
        return 1;
    }
    void threadReconnect()
    {
        //        hlog(pstring()<<"连接"<<this->ip<<":"<<this->port<<"失败,重连中...");
        //如果返回-的说明已经有其他线程在重连,那么这个重连判定失败sleep1秒
        if (this->reconnect() < 0) {
            Sleep(1000);
        } else{
            hlog(pstring() << "重新连接" << this->ip << ":" << this->port << "成功!");
        }
    }
    //带回调函数的connect


    //也就是关闭连接
    int close();
    //若想操作应该返回plist<ptcp *>*类型
    plist<ptcp *> getAllClients();
    //内部删除listclient用
    void deleteClientByIndex(int i);
    //设置多线程server，已封装内部释放等，直接获取xptcp指针,供服务端使用
    //要增加，定时与客户端通信，发送探测包，如果发送失败则关闭该连接，并从统计的连接中去掉
    //使用时，函数中不用再写while循环，已经封装了，直接recvp判断类型就可以了
    int setThreadServerDetach(void (*fun_thread)(ptcp* ptcp),int type=0);

    //带listclient
    int setThreadServer(void (*fun_thread)(ptcp* ptcp),int type=0);

    //专门为类成员函数准备的
    template <class T>
    int setThreadServerClass(void (T::*funtest)(ptcp* para),T* pthis)
    {
        int iSock=ctcp_InitTcpSocket();
        if(iSock<0)
        {
            hlog("iSock<0");
            ctcp_CloseConnection(iSock);
            return -1;
        }
        int ibind=ctcp_BindSocket(iSock,this->ip.c_str(),this->port);
        if(ibind<0)
        {
            hlog("bind fail");
            hlog(strerror(errno));
            ctcp_CloseConnection(iSock);
            return -2;
        }
        this->sock=iSock;

        //不停等待，来一个，创建一个线程
        while(true)
        {
            int iClientSock=-1;
            int iLength;
            struct sockaddr_in struSin;
            iLength=sizeof(struSin);
            memset(&struSin,0,iLength);
            iClientSock = accept(this->sock, (struct sockaddr *)&struSin, (socklen_t*)&iLength);
            if(iClientSock<0)
                continue;
            //            hlog(iClientSock);
            string clientip=string(inet_ntoa(struSin.sin_addr));
            //            hlog(clientip);
            ptcp* ptnew=new ptcp(iClientSock,clientip,this->port);

            //            std::function<void (void)> func = std::bind(funtest,pthis,ptnew);
            //            std::thread thserver(func);

            std::thread thserver(funtest,pthis,ptnew);
            //        //如果不调用,线程完了不自动释放资源
            thserver.detach();

        }
    }
    //专门为类成员函数准备的,直接多线程
    //ps->setTcpServerxClassThread(&MainWindow::slotThreadServer2,this);
    //参数ptcp* p,这个没有封装业务函数,只能自己写while循环
    //    void MainWindow::slotThreadServer2(ptcp *p)
    //    {
    //        while(1)
    //        {
    //            if(p->recvx()<0)
    //                break;
    //            if(p->strtype=="test")
    //            {
    //                int irecv=p->getClass<int>();
    //                hlog(irecv);
    //            }
    //        }
    //    }
    template <class T>
    void setThreadServerClassDetach(void (T::*funtest)(ptcp* para),T* pthis)
    {
        std::thread thServer(&ptcp::setThreadServerClass<T>,this,funtest,pthis);
        thServer.detach();
    }

    //要map的key使用自定义的值，则必须要重载<，const必须这样，不然不认
    bool operator<(const ptcp & tcp) const;


    template <class T>
    int recv(T& t)
    {
        if(this->strCtcptype=="init")
        {
            hlog(pstring()<<"TCP未初始化,请先指定tcp的类型是客户端还是服务端 "<<this->ip<<":"<<this->port);
            return -999;
        }
        //        return tcp->recv(tcp,&t,sizeof(t));
        int ires= RecvFullPacketTimeout(this->sock,(char*)&t,sizeof(t),-1);
        //        //如果发送失败,包括超时,就重连,是客户端才行
        if (ires < 0) {
            if(this->strCtcptype=="client")
            {
                this->connectx();
            }
        }
        return ires;
    }
    //基本接收，发送
    int recv(char* acrecv, int len, int timeout=-1)
    {
        if(this->strCtcptype=="init")
        {
            hlog(pstring()<<"TCP未初始化,请先指定tcp的类型是客户端还是服务端 "<<this->ip<<":"<<this->port);
            return -999;
        }
        int ires= RecvFullPacketTimeout(this->sock,acrecv,len,timeout);
        //        //如果发送失败,包括超时,就重连,是客户端才行
        if (ires < 0) {
            hlog(ires);
            if(this->strCtcptype=="client")
            {
                this->connectx();
            }
        }
        return ires;
    }

    template <class T>
    int send(T t)
    {
        if(this->strCtcptype=="init")
        {
            hlog(pstring()<<"TCP未初始化,请先指定tcp的类型是客户端还是服务端 "<<this->ip<<":"<<this->port);
            return -999;
        }
        //        return tcp->send(tcp,&t,sizeof(t));
        //        return SendFullPacket(this->sock,(char*)&t,sizeof(t));
        int ires= SendFullPacketTimeout(this->sock,(char*)&t,sizeof(t),5);
        //        //如果发送失败,包括超时,就重连,是客户端才行
        if (ires < 0) {
            if(this->strCtcptype=="client")
            {
                this->connectx();
            }
        }
        return ires;
    }
    int send(const char* acsend,int len)
    {
        if(this->strCtcptype=="init")
        {
            hlog(pstring()<<"TCP未初始化,请先指定tcp的类型是客户端还是服务端 "<<this->ip<<":"<<this->port);
            return -999;
        }
        //    return tcp->send(tcp,acsend,len);
        int ires= SendFullPacketTimeout(this->sock,acsend,len,9);
        //        //如果发送失败,包括超时,就重连,是客户端才行
        if (ires < 0) {
            if(this->strCtcptype=="client")
            {
                this->connectx();
            }
        }
        return ires;
    }

    int sendzstring(string strtype, string content)
    {
        return sendx(strtype,content.c_str(),content.size());
    }
    /*序列化方案
     * //send用这个,支持发送类，字符串，不支持结构体
     * 结构体直接用sendx(三参数形式)
     */
    template <class T>
    int sendx(string type,T t)//############send用这个,只适用于结构体
    {
        /*如果发的是字符串，则直接发送字符串，不转了，如果不是，则要
        //转成字符串发送，发送的字符串是全的，只有反序列化时才会出现不全
        //的情况，因此发送全用string*/
        if(typeid(t)==typeid(string)||typeid(t)==typeid(pstring))
        {
            return this->sendzstring(type,*((string*)(&t)));
        }
        return this->sendzstring(type,common_toString(t));
    }
    int sendx(pstring  type,const char *pdata=NULL, int length=0)
    {
        //这是为了防止这样的调用:ptcp->sendx("res","xfdsfds")
        if(pdata!=NULL&&length==0)
            return sendx(type,pdata,(string(pdata).size()));

        STRUCT_HEADER sHeader;
        memset(&sHeader,0,sizeof(STRUCT_HEADER));
        //        sHeader.type=type;
        strcpy(sHeader.arrchType,type.c_str());
        sHeader.length=length;
        pstring strall;
        strall.resize(sizeof(STRUCT_HEADER)+length);
        char* pstart=(char*)(&(*(strall.begin())));
        memcpy(pstart,&sHeader,sizeof(STRUCT_HEADER));
        //非空的话要把数据弄上去
        if(pdata!=NULL)
        {
            memcpy(pstart+sizeof(STRUCT_HEADER),pdata,length);
        }
        int iResSend=this->send(strall.c_str(),strall.size());
        if(iResSend!=strall.size())
        {
            hlog("发送数据失败，未全部发送：",iResSend,strall.size());
            return -5;
        }
        if(iResSend==sizeof(STRUCT_HEADER))
        {
            //如果只发了类型，则返回数据长度0
            return 0;
        }
        return sHeader.length;
    }
    //>=0表示接收成功==0表示只发类型
    int recvx(int timeout=-1)
    {
        STRUCT_HEADER sHeader;
        memset(&sHeader,0,sizeof(STRUCT_HEADER));
        int iresHeader=-1;
        if((iresHeader=this->recv((char*)&sHeader,sizeof(STRUCT_HEADER),timeout))<0)
        {
            hlog("接收包头失败");
            //不要打印，不然会一直打印，因为如果接收不到就一直打
            return iresHeader;
        }
        //        hlog(iresHeader);
        this->type.assign(sHeader.arrchType);
        if(sHeader.length==0)
        {
            hlog("接收到只类型帧");
            return 0;
        }
        if(sHeader.length<0)
        {
            hlog("收到包头中的长度为负，非法，接收失败");
            return -9;
        }
        this->length=sHeader.length;
        this->data.resize(sHeader.length);
        int iRecvDataRes=this->recv((char*)(&(*(this->data.begin()))),sHeader.length,timeout);
        if(iRecvDataRes<0)
        {
            hlog(pstring()<<"接收数据失败:"<<iRecvDataRes);
            return iRecvDataRes;
        }
        if(iRecvDataRes!=sHeader.length)
        {
            hlog("接收到的数据长度与包头中的长度不一致，接收失败");
            return -8;
        }
        return iRecvDataRes;
    }


    template <class T>
    T getClass()
    {
        //        hlog(typeid(T).name());

        //如果本来就是string，就别转了，不然出问题
        //        if(typeid(T).name()==typeid(string).name())
        //        return strdata;


        return common_toClass<T>(this->data);
    }

    //支持hlog，重写cout,前提是不能自动释放
    friend ostream& operator<<(ostream &os,ptcp tcp)
    {
        os<<"[ip: "<<tcp.ip<<"   port: "<<tcp.port<<"  sock: "<<tcp.sock<<"   type: "<<tcp.strCtcptype<<"]";
        if(tcp.strCtcptype=="server")
        {
            string allclients;
            if(tcp.listclient.size()>0)
            {
                allclients="  当前连接到本tcp服务的客户端如下:\n{\n";
                if(tcp.listclient.size()>0)
                {
                    for(int i=0;i<tcp.listclient.size()-1;i++)
                    {
                        allclients+=tcp.listclient[i]->ip+" ";
                    }
                    allclients+=tcp.listclient[tcp.listclient.size()-1]->ip+"\n}";
                }
                else
                    allclients+="}";
            }
            else
            {
                allclients=" 当前无客户端连接到本tcp服务";
            }
            os<<allclients;
        }
        return os;
    }

    //可打印ptcp*的，加一个就可以了！
    //支持hlog，重写cout,前提是不能自动释放
    friend ostream& operator<<(ostream &os,ptcp* tcp)
    {
        os<<"[ip: "<<tcp->ip<<"   port: "<<tcp->port<<"  sock: "<<tcp->sock<<"   type: "<<tcp->strCtcptype<<"]";
        if(tcp->strCtcptype=="server")
        {
            string allclients;
            if(tcp->listclient.size()>0)
            {
                allclients="  当前连接到本tcp服务的客户端如下:\n{\n";
                if(tcp->getAllClients().size()>0)
                {
                    for(int i=0;i<tcp->getAllClients().size()-1;i++)
                    {
                        allclients+=tcp->getAllClients()[i]->ip+" ";
                    }
                    allclients+=tcp->getAllClients()[tcp->getAllClients().size()-1]->ip+"\n}";
                }
                else
                    allclients+="}";
            }
            else
            {
                allclients=" 当前无客户端连接到本tcp服务";
            }
            os<<allclients;
        }
        return os;
    }

    //    //支持hlog，重写cout,前提是不能自动释放
    //    friend ostream& operator<<(ostream &os,ptcp ptcp)
    //    {
    //        os<<"[ip: "<<ptcp.ip<<"   port: "<<ptcp.port<<"  sock: "<<ptcp.sock<<"]";
    //        return os;
    //    }
    //    //支持hlog，重写cout,前提是不能自动释放
    //    friend ostream& operator<<(ostream &os,ptcp *ptcp)
    //    {
    //        os<<"[ip: "<<ptcp->ip<<"   port: "<<ptcp->port<<"  sock: "<<ptcp->sock<<"]";
    //        return os;
    //    }



    void reconnect2()
    {
        while(this->connect()<0)
        {
            Sleep(1000);
        }
        hlog("reconnect2 success");
    }

private:

};




typedef struct xctime
{
    int type;
    int size;
    unsigned short ushYear;
    unsigned char uchMonth;
    unsigned char uchDay;
    unsigned char uchHour;
    unsigned char uchMinute;
    unsigned char uchSecond;
    unsigned short ushMSecond;

    struct timeval struTime;
    int (*show)(struct xtime s_xtime);
}xctime;

xctime xtime_init();
//时间相关
xctime clib_getTimeAdd(xctime src, time_t sec);
xctime clib_getTimeSub(xctime src, time_t sec);
xctime clib_getTimeNow();
double clib_getDiffBetweenXtime(xctime t1, xctime t2);
int clib_getStringFromXtime(xctime t, char *str);
int clib_getStringTimeNow(char *pch);
time_t clib_getNowSeconds();
xctime clib_getTimeFromSeconds(const time_t sec);
time_t clib_getSecondsFromXtime(xctime time);
int clib_isBetweenXtime(xctime t, xctime t1, xctime t2);


xctime clib_getTimeFromString(const char *pch);



//相减精确到ms
class ptime
{
    xctime time;
    void init();
    ptime(xctime time);
public:
    //获取当前时间
    ptime();
    //重新获取当前时间
    void setNowTime();
    ptime(int year,int mon,int day,int hour,int min,int sec);
    //xxxx-xx-xx xx:xx:xx
    ptime(string strTime);
    ptime(const char* acTime);

    //2020-09-16 00:44:27
    static pstring getTimeFullNoMsNow();
    //00:44:27
    static pstring getTimeNoMsNow();
    //00:44:27.854
    static pstring getTimeNow();
    //2020-09-16 00:44:27.854
    static pstring getTimeFullNow();
    //2020-09-16
    static pstring getDateNow();

    static double getDiff(clock_t t1, clock_t t2);
    static ptime getTimeFromSeconds(int64_t secs);

    unsigned short year();
    unsigned char  mon();
    unsigned char  day();
    unsigned char  hour();
    unsigned char  minute();
    unsigned char  second();
    unsigned short msecond();



    string toString();
    pstring toPString()
    {
        return pstring(this->toString());
    }

    //支持hlog，重写cout,前提是不能自动释放
    friend ostream& operator<<(ostream &os,ptime time)
    {
        os<<time.toString();
        return os;
    }
    friend istream& operator>>(istream& is,ptime& data)
    {
        string str;
        for(int i=0;i<23;i++)
            str.push_back(is.get());
        //        hlogone(str);
        xctime ct=clib_getTimeFromString(str.c_str());
        data.time=ct;
        return is;
    }
    //this -time2
    double operator -(const ptime &time2);
    //this + secs
    ptime operator +(const time_t secs);
    //this-secs
    ptime operator -(const time_t secs);
    //比较
    bool operator<(const ptime & tm) const
    {
        if(clib_getDiffBetweenXtime(tm.time,this->time)<0)
            return true;
        else
            return false;
    }
    bool operator==(const ptime & tm)const
    {
        if(clib_getDiffBetweenXtime(tm.time,this->time)==0)
            return true;
        else
            return false;
    }
};

class protect{
public:
    ptime tdes;
    ptime tlast;
    friend ostream& operator<<(ostream& os,protect p)
    {
        os<<p.tdes<<" "<<p.tlast;
        return os;
    }

    friend istream& operator>>(istream& is,protect& p)
    {
        is>>p.tdes;
        is.get();
        is>>p.tlast;
        return is;
    }
};





//类型 保存，追加，载入
enum typexfile{typesave,typeappend,typeload};
typedef struct cfile
{
    int type;
    char acPathFile[2048];
    FILE* fp;
    /*
     * 无论是save还是append，只要打开文件save就是追加
     * 不同的是，在上个文件追加还是覆盖
     */
    int (*save)(struct cfile* file,void* data,int len);
    //当遇到文件末尾，返回0
    int (*load)(struct cfile* file,void* data,int len);
    int (*move)(struct cfile* file,unsigned long long pos);
    int (*destroy)(struct cfile* file);

    unsigned long long (*size)(struct cfile* file);
}cfile;
cfile* cfileInit(const char *acPathFile, enum typexfile typeFile);
//该函数只返回>=0,存储错误是=0;
int cfileSave(cfile *file, void *data, int len);

//该函数只返回>=0,读不到是=0;
int cfileLoad(cfile *file, void *data, int len);
int cfilemove(cfile *file, unsigned long long pos);
int cfileDestroy(cfile *file);
//读取，保存，追加等功能
//读取，保存，追加等功能
class pfile
{
    cfile* pf;
public:
    char* dataAll;
    //文件长度
    ulonglong sizeFile;
    string strtype;
    string strpath;
    pfile(string strFullPath, typexfile type);
    ~pfile();
    //>0才有效,<表示读不到了,返回的是实际读取的字节数
    int load(char *data, int len);
    //需要释放
    int loadAll();
    // 读取到pstirng中,如果size==0表示读不到了
    //用来读一包的，一般最大512k
    pstring load(int len)
    {
        pstring strdata;
        strdata.resize(len);
        int iread=this->load((char*)(&(*(strdata.begin()))),len);
        if(iread<0)
        {
            return pstring();
        }
        strdata.resize(iread);
        return strdata;
    }

    bool isOpen()
    {
        if(pf==NULL)
            return false;
        return true;
    }
    //该函数只返回>=0,存储错误是=0;
    int save(const char *data, int len);

    int move(unsigned long long pos);
    unsigned long long size();

    //只适合结构体这种的
    template <class T>
    int load(T &t)
    {
        return load((char*)&t,sizeof(t));
    }
    //只适合结构体这种的,把pstring string也放到这了
    template <class T>
    int save(T t)
    {
        if(typeid(t)==typeid(string)||typeid(t)==typeid(pstring))
        {
            string str=*((string*)(&t));
            return this->save(str.c_str(),str.size());
        }
        return save((char*)&t,sizeof(t));
    }
    friend ostream& operator<<(ostream& os,pfile f)
    {
        os<<"path: "<<f.strpath<<"  type: "<<f.strtype;
        return os;
    }
    friend ostream& operator<<(ostream& os,pfile *pf)
    {
        os<<"path: "<<pf->strpath<<"  type: "<<pf->strtype;
        return os;
    }
    int close();
};


class plib
{
public:
    static string getTimeHHMMSS()
    {
        string strTmNow = plib::getTimeNow();
        return strTmNow.substr(11, 8);
    }

    //获取当前积秒
    static unsigned int getCurrentJS() {
        return GetCurrentJS();
    }

    //根据积日积秒获取时间
    static int getDateTimeFromJDJS(unsigned short JD, unsigned int JS, DateTime &dt) {
        return GetDateTimeFromJDJS(JD, JS, dt);
    }

    static string getStringTimeFromJDJS(unsigned short JD, unsigned int JS) {
        DateTime dtres;
        if (plib::getDateTimeFromJDJS(JD, JS, dtres) < 0)
            return "";
        char data[50];
        cinit(data);
        sprintf(data, "%d-%02d-%02d %02d:%02d:%02d", dtres.year, dtres.month, dtres.day,
                dtres.hour, dtres.min, dtres.sec);
        //        hlog(data);
        return data;
    }

    static string getStringTimeWithoutDateFromJS(unsigned int JS) {
        pstring strTimeFull = plib::getStringTimeFromJDJS(plib::getCurrentJD(), JS);
        return strTimeFull.split(" ")[1];
    }

    //获取当前积日
    static unsigned short getCurrentJD() {
        return GetCurrentJD();
    }
    //将0x类的16进制字符串转为int
    static int toHex(const char* ac0xHex)
    {
        //先去掉0x
        char acNakeNum[10];
        bzero(acNakeNum,10);
        memcpy(acNakeNum,ac0xHex+2,strlen(ac0xHex)-2);
        //        HLOG_STRING(acNakeNum);
        //之后再改
        int  i,t;
        int sum=0;
        for(i=0;acNakeNum[i];i++)
        {
            if(acNakeNum[i]<='9')
                t=acNakeNum[i]-'0';
            else
                t=acNakeNum[i]-'a'+10;
            sum=sum*16+t;
        }
        //        HLOG_INT(sum);
        return sum;
    }
    //0xFF 一个字节
    static string toStringHex1Byte(int hex) {
        char data[20];
        cinit(data);
        //unsigned char 为了防止出现一堆ff
        sprintf(data, "0x%02x", (unsigned char) hex);
        return data;
    }

    //0x0023 一个字符是两字节
    static string toStringHex2Byte(int hex) {
        char data[20];
        cinit(data);
        sprintf(data, "0x%04x", (unsigned short) hex);
        return data;
    }

    //0x00000023 是四字节 两个十六进制是一个字节
    static string toStringHex4Byte(int hex) {
        char data[20];
        cinit(data);
        sprintf(data, "0x%08x", (unsigned int) hex);
        return data;
    }
    static int getLastError()
    {
        return WSAGetLastError();
    }

    //    //FATAL ERROR: Network error: Connection timed out
    //    hlog(strTasks);
    //    if(strTasks.contain("FATAL ERROR"))
    //    {
    //        sigShowStatus("连接失败",1000);
    //        this->bgettingData=false;
    //        return;
    //    }
    static pstring getTasksFromHost(pstring host)
    {
        pstring path=(plib::getConfStringRemoteHost(host,"/opt/sc/csgl/SCService/sc.conf","SHARE_STORE_PATH"));
        if(path=="")
        {
            return pstring()<<"根据远程配置文件/sc.conf获取队列路径SHARE_STORE_PATH失败,请检查网络或配置项";
        }
        pstring pathWithXiegang=plib::getPathWithXiegang(path);
        pstring pathFull=pathWithXiegang+"string.dat";
        hlog(pathFull);
        pstring cmd="cat "+pathFull;
        hlog(cmd);
        pstring strTaskAll=plib::xshplink(host,cmd);
        //获取队列失败了,判断一下原因
        if(strTaskAll=="")
        {
            hlog(plib::xshplink(host,"ls -l string.dat | awk '{print $5}'"));
        }
        return strTaskAll;
    }

    static pstring getTasksFromHostBridge(pstring host)
    {
        pstring path=(plib::getConfStringRemoteHostBridge(host,"/opt/sc/csgl/SCService/sc.conf","SHARE_STORE_PATH"));
        hlog(path);
        if(path=="")
        {
            return pstring()<<"根据远程配置文件/sc.conf获取队列路径SHARE_STORE_PATH失败,请检查网络或配置项";
        }
        pstring pathWithXiegang=plib::getPathWithXiegang(path);
        pstring pathFull=pathWithXiegang+"string.dat";
        hlog(pathFull);
        pstring cmd="cat "+pathFull;
        hlog(cmd);
        pstring strTaskAll=plib::xshbridge(host,cmd);
        hlog(strTaskAll);
        //获取队列失败了,判断一下原因
        if(strTaskAll=="")
        {
            hlog(plib::xshbridge(host,"stat --format=%s /opt/sc/csgl/SCService/string.dat"));
            hlog("队列为空");
        }
        return strTaskAll;
    }

    static pmap<pstring, pmap<pstring, pstring > > getAllServerCSGL()
    {
        pmap<pstring,pmap<pstring,pstring> > mall;
        //增加各系统host
        pmap<pstring,pstring> mapgf;
        mapgf["KSC"]="172.16.141.5";
        mapgf["MYC"]="172.16.77.6";
        mapgf["SYC"]="172.16.171.5";
        mapgf["DTC"]="172.16.11.3";
        mapgf["KR1"]="130.242.60.241";
        mapgf["YD1"]="172.16.112.2";
        mapgf["YE2"]="172.17.2.2";
        mall["高分"]=mapgf;
        pmap<pstring,pstring> mapxd;
        mapxd["KSC"]="172.16.142.2";
        mapxd["MYC"]="172.16.78.2";
        mapxd["SYC"]="172.16.172.1";
        mapxd["DTC"]="172.16.12.3";

        mall["先导"]=mapxd;
        pmap<pstring,pstring> mapz3;
        mapz3["KSC"]="172.16.143.1";
        mapz3["MYC"]="172.16.80.1";
        mapz3["MYC"]="172.16.80.2";
        mapz3["SYC"]="172.16.173.1";
        mapz3["DTC"]="172.16.13.2";
        mall["资三"]=mapz3;
        pmap<pstring,pstring> mapgf4;
        mapgf4["DTC"]="172.16.2.110";
        mapgf4["MYC"]="172.16.2.111";
        mapgf4["QBJ"]="172.16.2.104";
        mall["高分四号"]=mapgf4;
        pmap<pstring,pstring> mapkj;
        mapkj["KSC"]="172.16.144.2";
        mapkj["MYC"]="172.16.81.2";
        mapkj["DTC"]="172.16.14.2";
        mapkj["SYC"]="172.16.174.2";
        mapkj["YD1"]="172.16.112.1";
        mapkj["KR1"]="130.242.60.243";
        mall["空基"]=(mapkj);
        pmap<pstring,pstring> mapfenfa;
        mapfenfa["DTC"]="172.16.2.93";
        mapfenfa["ZYD"]="172.16.13.1";
        mapfenfa["VCRE1"]="172.16.14.9";
        mapfenfa["GF4D"]="172.16.2.110";//会与高分4dtc重复,不加了
        mapfenfa["GFD"]="172.16.11.16";
        mapfenfa["DTC1"]="172.16.2.94";
        mapfenfa["MDJ"]="172.16.2.102";
        mall["传输管理控制"]=(mapfenfa);
        return mall;
    }
    static pmap<pstring, pmap<pstring, pstring > > getAllServerCSGLWithoutGF4AndCsglControl()
    {
        pmap<pstring,pmap<pstring,pstring> > mall;
        //增加各系统host
        pmap<pstring,pstring> mapgf;
        mapgf["KSC"]="172.16.141.5";
        mapgf["MYC"]="172.16.77.6";
        mapgf["SYC"]="172.16.171.5";
        mapgf["DTC"]="172.16.11.3";
        mapgf["KR1"]="130.242.60.241";
        mapgf["YD1"]="172.16.112.2";
        mapgf["YE2"]="172.17.2.2";
        mall["高分"]=mapgf;
        pmap<pstring,pstring> mapxd;
        mapxd["KSC"]="172.16.142.2";
        mapxd["MYC"]="172.16.78.2";
        mapxd["SYC"]="172.16.172.1";
        mapxd["DTC"]="172.16.12.3";

        mall["先导"]=mapxd;
        pmap<pstring,pstring> mapz3;
        mapz3["KSC"]="172.16.143.1";
        mapz3["MYC"]="172.16.80.1";
        mapz3["MYC"]="172.16.80.2";
        mapz3["SYC"]="172.16.173.1";
        mapz3["DTC"]="172.16.13.2";
        mall["资三"]=mapz3;

        pmap<pstring,pstring> mapkj;
        mapkj["KSC"]="172.16.144.2";
        mapkj["MYC"]="172.16.81.2";
        mapkj["DTC"]="172.16.14.2";
        mapkj["SYC"]="172.16.174.2";
        mapkj["YD1"]="172.16.112.1";
        mapkj["KR1"]="130.242.60.243";
        mall["空基"]=(mapkj);

        return mall;
    }
    static pstring getConfStringRemoteHostBridge(pstring host,pstring path,pstring key,pstring pwd="scfwq_325",pstring hostbridge="172.16.11.6",pstring pwdbridge="scfwq_325")
    {
        pstring cmd="cat "+path+"|grep "+key+"=";
        hlog(cmd);
        pstring stres=xshbridge(host,cmd,pwd,hostbridge,pwdbridge);
        hlog(stres);
        if(stres.contain("\n"))
        {
            hlog("警告：找到多个key值,需要去掉重复",stres);
            stres=stres.split("\n")[0];
            pliststring listres=stres.split("=");
            stres=listres[listres.size()-1];
            //            hlog(listres);
            //            hlog(stres);
            return stres;
        }
        //找不到的话是空
        //        hlog(stres);
        if(stres!="")
        {
            pliststring listres=stres.split("=");
            stres=listres[listres.size()-1];
        }

        return stres;
    }
    //如果没斜杠,加上,有就不动
    static pstring getPathWithXiegang(pstring path)
    {
        if(path[path.size()-1]!='/')
            path=path+"/";
        return path;
    }

    static pstring getConfStringRemoteHost(pstring host,pstring path,pstring key,pstring pwd="scfwq_325")
    {
        pstring cmd="cat "+path+"|grep "+key+"=";
        //        hlog(cmd);
        pstring stres=xshplink(host,cmd,pwd);
        //        hlog(stres);
        if(stres.contain("\n"))
        {
            //            hlog("警告：找到多个key值,需要去掉重复",stres);
            stres=stres.split("\n")[0];
            //            hlog(stres);
            pliststring listres=stres.split("=");
            stres=listres[listres.size()-1];
            //            hlog(listres);
            //            hlog(stres);
            return stres;
        }
        //找不到的话是空
        hlog(stres);
        if(stres!="")
        {
            pliststring listres=stres.split("=");
            stres=listres[listres.size()-1];
            //            hlog(listres);
            //            hlog(stres);
        }

        return stres;
    }


    static string getTimeNow()
    {
        ptime tnow;
        string strTimeNow=plib::toString(tnow);
        return strTimeNow;
    }
    //20200602191846188
    static string getTimeNowNoSplit()
    {
        pstring strnow=plib::getTimeNow();
        pliststring listsplit=strnow.split(": -.");
        return listsplit.join("");
    }
    static string getDateNow()
    {
        string strname=getTimeNow();
        return strname.substr(0,10);
    }
    static string getTimeNowHHMMSS()
    {
        string strtm=plib::getTimeNow();
        return strtm.substr(11,8);
    }

    static pstring toChinese(string str)
    {
        return common_toChinese(str);

    }
    /*首先，汉字是双字节的编码格式！
        汉字的第一字节：是从0xB0   开始编码
        汉字的第二字节：是从0xA1   开始编码
        有了以上的了解，就不难识别出是中文，还是其他的编码！
         程序中只需要判断（   第一字节>=0xB0   &&   第二字节>=0xA1   ）就好！
         */
    static int isIncludeChinese(string str)//返回0：无中文，返回1：有中文
    {
        return common_isIncludeChinese(str);
    }
    static pstring getSystem()
    {
        return common_getSystem();
    }

    //    static string toGBK(const char* str)
    //    {
    //        return Common_UTF8toGBK(str);
    //    }

    static string toUTF8(const char* str)
    {
        return common_string_To_UTF8(str);
    }

    //判断字符串是否为utf8
    static bool isUTF8(const char* str)
    {
        return common_isStringUTF8(str);
    }
    //这个地方是为了与windows保持统一,因为linux下不用转
    static string toGBK(const char* str)
    {
        return common_string_To_GBK(str);
    }



    //递归删除非空文件夹,不能删除文件
    static bool removeDir(const char* szFileDir)
    {
        std::string strDir = szFileDir;
        if (strDir.at(strDir.length()-1) != '\\')
            strDir += '\\';
        WIN32_FIND_DATAA wfd;
        HANDLE hFind = FindFirstFileA((strDir + "*.*").c_str(),&wfd);
        if (hFind == INVALID_HANDLE_VALUE)
            return false;
        do
        {
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (stricmp(wfd.cFileName,".") != 0 &&
                        stricmp(wfd.cFileName,"..") != 0)
                    removeDir( (strDir + wfd.cFileName).c_str());
            }
            else
            {
                DeleteFileA( (strDir + wfd.cFileName).c_str());
            }
        }
        while (FindNextFileA(hFind,&wfd));
        FindClose(hFind);
        RemoveDirectoryA(szFileDir);
        return true;
    }

    //可删非空文件夹和文件
    static bool rm(pstring path)
    {
        //判断类型删除
        string strtype=plib::getPathType(path);
        if(strtype=="file")
            return remove(path.c_str());
        else if(strtype=="dir")
            return removeDir(path.c_str());
    }

    //获取线程id
    static long getTid()
    {
        return GetCurrentThreadId();
    }
    //都是全路径,重命名文件
    static bool mv(string src,string dst)
    {
        const char* source=src.c_str();
        const char* newname=dst.c_str();
        if(!_access(source,0))//如果文件存在:
        {
            if(!rename(source,newname))//删除成功
            {
                hlog(pstring()<<source<<" 成功重命名为: "<<newname);
                return true;
            }
            else//无法重命名:文件打开或无权限执行重命名
            {
                hlog(pstring()<<"文件无法重命名(可能原因如下):\n"
                     <<"\t"<<"1. "<<newname<<" 已存在\n"
                     <<"\t"<<"2. "<<newname<<" 正在使用,未关闭.\n"
                     <<"\t"<<"3. "<<"你没有权限重命名此文件.");
                return false;
            }
        }
        else//文件不存在
        {
            hlog(pstring()<<source<<" 不存在,无法重命名.");
            return false;
        }
    }
    //普通转ut8
    static std::string string_To_UTF8(const std::string & str)
    {
        int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
        wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
        ZeroMemory(pwBuf, nwLen * 2 + 2);
        ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
        int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
        char * pBuf = new char[nLen + 1];
        ZeroMemory(pBuf, nLen + 1);
        ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
        std::string retStr(pBuf);
        delete []pwBuf;
        delete []pBuf;
        pwBuf = NULL;
        pBuf = NULL;
        return retStr;
    }
    //utf8转普通
    static std::string UTF8_To_string(const std::string & str)
    {
        int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

        wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
        memset(pwBuf, 0, nwLen * 2 + 2);

        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

        int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

        char * pBuf = new char[nLen + 1];
        memset(pBuf, 0, nLen + 1);

        WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

        std::string retStr = pBuf;

        delete []pBuf;
        delete []pwBuf;

        pBuf = NULL;
        pwBuf = NULL;

        return retStr;
    }



    //中文转URL代码
    static pstring  UrlUTF8(const char * str)
    {
        string tt;
        pstring dd;
        GB2312ToUTF_8(tt,str,strlen(str));
        int len=tt.length();
        for (int i=0;i<len;i++)
        {
            if(isalnum((BYTE)tt.at(i))) //判断字符中是否有数组或者英文
            {
                char tempbuff[2]={0};
                sprintf_s(tempbuff,"%c",(BYTE)tt.at(i));
                dd.append(tempbuff);
            }
            else if (isspace((BYTE)tt.at(i)))
            {
                dd.append("+");
            }
            else
            {
                char tempbuff[4];
                sprintf_s(tempbuff,"%%%X%X",((BYTE)tt.at(i)) >>4,((BYTE)tt.at(i)) %16);
                dd.append(tempbuff);
            }

        }
        return dd;
    }

    static void GB2312ToUTF_8(string& pOut,const char *pText, int pLen)
    {
        char buf[4];
        memset(buf,0,4);

        pOut.clear();

        int i = 0;
        while(i < pLen)
        {
            //如果是英文直接复制就可以
            if( pText[i] >= 0)
            {
                char asciistr[2]={0};
                asciistr[0] = (pText[i++]);
                pOut.append(asciistr);
            }
            else
            {
                WCHAR pbuffer;
                Gb2312ToUnicode(&pbuffer,pText+i);

                UnicodeToUTF_8(buf,&pbuffer);

                pOut.append(buf);

                i += 2;
            }
        }

        return;
    }
    static void Gb2312ToUnicode(WCHAR* pOut,const char *gbBuffer)
    {
        ::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
        return;
    }
    static void UTF_8ToUnicode(WCHAR* pOut,const char *pText)
    {
        char* uchar = (char *)pOut;

        uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
        uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

        return;
    }
    static void  UnicodeToUTF_8(char* pOut,WCHAR* pText)
    {
        // 注意 WCHAR高低字的顺序,低字节在前，高字节在后
        char* pchar = (char *)pText;

        pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
        pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
        pOut[2] = (0x80 | (pchar[0] & 0x3F));

        return;
    }

    /*将大写字母转换成小写字母*/
    static int tolower(int c)
    {
        if (c >= 'A' && c <= 'Z')
        {
            return c + 'a' - 'A';
        }
        else
        {
            return c;
        }
    }
    //将十六进制的字符串转换成整数
    static int htoi(const char s[])
    {
        int i;
        int n = 0;
        if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
        {
            i = 2;
        }
        else
        {
            i = 0;
        }
        for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
        {
            if (tolower(s[i]) > '9')
            {
                n = 16 * n + (10 + tolower(s[i]) - 'a');
            }
            else
            {
                n = 16 * n + (tolower(s[i]) - '0');
            }
        }
        return n;
    }
    //将十六进制的字符串转换成char
    static char hstrtoch(char s[])
    {
        return common_hstrtoch(s);
    }
    //跨平台统一接口
    static void sleep(int ms)
    {
        Sleep(ms);
    }
    //根据所有ip获取本机ip
    static pstring getIPLocal()
    {
        plist<pstring> lres;
        plist<pstring> listip=getIPsAll();
        for(int i=0;i<listip.size();i++)
        {
            plist<pstring> listipi=listip[i].split(",");


            //        hlog(listipi);
            bool bvalid=true;
            for(int j=0;j<listipi.size();j++)
            {
                plist<pstring> lj=listipi[j].split(":");
                pstring stresj=(lj[lj.size()-1]);
                //            hlog(stresj);
                if(stresj=="0.0.0.0")
                {
                    bvalid=false;
                    break;
                }
            }
            if(bvalid)
                lres.append(listipi[0]);


        }
        //            hlog(lres);
        pstring stres;
        if(lres.size()==0)
        {
            stres="无法获本机地址,请检查网络或网卡!";
            return stres;
        }
        stres=lres[0].split(":")[2];
        //    hlog(stres);
        //        string iplocal=string("本机IP: ")+stres;
        //        hlog(iplocal);
        return stres;
    }
    /*
     * 只要youyi个是0.0.0.0就代表不是本机地址
    [10:38:59.213]-[tid:28760]-[plibmain.cpp:646] getIpAll() :  len: 8 pvector
    {
    netcard0 ip addr:ip:0.0.0.0,mask:0.0.0.0,gate:0.0.0.0.
    netcard1 ip addr:ip:0.0.0.0,mask:0.0.0.0,gate:0.0.0.0.
    netcard2 ip addr:ip:0.0.0.0,mask:0.0.0.0,gate:0.0.0.0.
    netcard3 ip addr:ip:192.168.31.126,mask:255.255.255.0,gate:192.168.31.1.
    netcard4 ip addr:ip:0.0.0.0,mask:0.0.0.0,gate:0.0.0.0.
    netcard5 ip addr:ip:0.0.0.0,mask:0.0.0.0,gate:0.0.0.0.
    netcard6 ip addr:ip:192.168.226.1,mask:255.255.255.0,gate:0.0.0.0.
    netcard7 ip addr:ip:192.168.248.1,mask:255.255.255.0,gate:0.0.0.0.
    }
    */
    static plist<pstring> getIPsAll() {
        plist<pstring> result;
        IP_ADAPTER_INFO *pAdpFree=NULL;
        IP_ADAPTER_INFO *pIpAdpInfo=(IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
        unsigned long ulBufLen=sizeof(IP_ADAPTER_INFO);
        int ret;
        //第一次调用获取需要开辟的内存空间大小
        if((ret=GetAdaptersInfo(pIpAdpInfo,&ulBufLen))==ERROR_BUFFER_OVERFLOW){
            free(pIpAdpInfo);
            //分配实际所需要的内存空间
            pIpAdpInfo=(IP_ADAPTER_INFO*)malloc(ulBufLen);
            if(NULL == pIpAdpInfo){
                return result;
            }
        }

        char ip[256];
        if((ret=GetAdaptersInfo(pIpAdpInfo,&ulBufLen))==NO_ERROR){
            pAdpFree=pIpAdpInfo;

            for(int i=0;pIpAdpInfo;i++){
                string addr;
                _snprintf(ip,sizeof(ip),"netcard%d ip addr:",i);
                addr+=ip;
                IP_ADDR_STRING *pIps=&pIpAdpInfo->IpAddressList;
                while(pIps){
                    _snprintf(ip,sizeof(ip),"ip:%s,mask:%s,gate:%s",pIps->IpAddress.String,
                              pIps->IpMask.String,pIpAdpInfo->GatewayList.IpAddress.String);
                    addr+=ip;
                    //                cout<<pIps->IpAddress.String<<endl;
                    //                cout<<pIps->IpMask.String<<endl;
                    //                cout<<pIpAdpInfo->GatewayList.IpAddress.String<<endl;
                    pIps=pIps->Next;
                }
                result.push_back(addr);
                pIpAdpInfo=pIpAdpInfo->Next;
            }
        }
        if(pAdpFree){
            free(pAdpFree);
        }
        return result;

    }

    //三次有一次通就可以,防止vpn状态不稳不断重启
    static bool ping3()
    {
        return ping("172.16.12.1")||ping("172.16.11.3")||ping("172.16.11.6");
    }

    //直接用tcp connect22端口
    static bool ping(pstring ip)
    {
        ptcp tcpPing(ip,22);
        if(tcpPing.connect()<0)
            return false;
        return true;
        /*这个调用windowscmd的方法有延迟,弃用了
        //cmd ping -w(超时时间ms) -n次数默认4
        pstring cmd=pstring("ping -w 1000 -n 1 ")+ip;
        //                hlog(cmd);
        pstring res=getCmdOutput(cmd);
//        hlog(res);
//        hlog(res.contain(plib::toChinese("请求超时")));
        //如果包含请求超时,则说明没通,否则就是通
        //因为改成utf8后,调用windows的程序返回的是gbk,所以用中文要转
        if(res.contain(plib::toChinese("请求超时"))||res.contain(plib::toChinese("无法访问目标主机"))||
                res.contain(plib::toChinese("一般故障"))||res.contain(plib::toChinese("传输失败"))||
                res.contain(plib::toChinese("找不到主机"))||res.contain(plib::toChinese("FATAL ERROR")))
        {
//            hlog("fail");
            return false;
        }
        else
            return true;
            */
    }

    static wstring Str2Wstr(string str)
    {
        unsigned len = str.size() * 2;// 预留字节数
        setlocale(LC_CTYPE, "");     //必须调用此函数
        wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
        mbstowcs(p, str.c_str(), len);// 转换
        std::wstring str1(p);
        delete[] p;// 释放申请的内存
        return str1;
    }

    //getcmdoutput creatprocess隐藏命令行窗口不支持>写入文件,而且plink自身有bug执行的时候会读取不到输出
    //unable to read from standard output,所以自己写一个bat文件把>写入文件集成,然后用getcmdoutput调用
    //就可以支持隐藏命令行窗口且从文件读正常,文件名区分线程和时间ms,这样不计较耗时,能用就ok
    //因为批处理不认识|,所以如果命令里面含有|,则要改成^| ^的意思是取消转义
    static pstring xshbkgetCmdoutput(pstring host,pstring cmd,pstring pwd="scfwq_325")
    {
        //因为直接读取的时候,调试时候没事,但是release的时候会出错,无法读取from standard output,所以采用直接存文件然后再删除的方式
        //由于可能多个线程同时调用,所以加上线程id和ms数,然后再把文件删除,虽然效率不高但是可稳定使用就行
        hlog(plib::pwd());
        if(!plib::isExsistFile("plink.exe"))
        {
            hlog("there is no plink.exe");
            return "";
        }
        //这里是plink是自动接收key版本的,且要加-batch,不然输错密码会卡住
        //写入文件,因为有可能多线程写入,次序写入,因此最好用不同的文件名,线程id+ms文件名可以
        pstring strname=plib::getTimeNowNoSplit();
        //        hlog(strname);
        strname=strname+plib::toString(plib::getTid())+".tmp";
        hlog(strname);
        pstring strCMDAll=pstring("plink.bat "+pwd+" "+host+" \""+cmd+"\" "+strname);
        hlog(strCMDAll);
        if(strCMDAll.contain("|"))
        {
            strCMDAll.replaceOnce("|","^|");
        }
        hlog(strCMDAll);
        //执行,写入文件
        plib::getCmdOutput(strCMDAll);
        //读取文件
        //        plist<pstring> info=plib::getAllLinesFromFile(strname);
        //        hlog(info);
        pstring res=plib::getStringFromFile(strname);
        plib::rm(strname);
        return res;
    }
    //getcmdoutput creatprocess隐藏命令行窗口不支持>写入文件,而且plink自身有bug执行的时候会读取不到输出
    //unable to read from standard output,所以自己写一个bat文件把>写入文件集成,然后用getcmdoutput调用
    //就可以支持隐藏命令行窗口且从文件读正常,文件名区分线程和时间ms,这样不计较耗时,能用就ok
    //因为批处理不认识|,所以如果命令里面含有|,则要改成^| ^的意思是取消转义
    //即时是这样getcmdOutput依然不支持plink还是unable报错,于是改为shellexecute,问题解决
    static pstring xshplink(pstring host,pstring cmd,pstring pwd="scfwq_325")
    {
        //因为直接读取的时候,调试时候没事,但是release的时候会出错,无法读取from standard output,所以采用直接存文件然后再删除的方式
        //由于可能多个线程同时调用,所以加上线程id和ms数,然后再把文件删除,虽然效率不高但是可稳定使用就行
        //        hlog(plib::pwd());
        if(!plib::isExsistFile("plink.exe"))
        {
            hlog("there is no plink.exe");
            return "";
        }
        if(!plib::isExsistFile("plink.bat"))
        {
            hlog("there is no plink.bat");
            return "";
        }
        //这里是plink是自动接收key版本的,且要加-batch,不然输错密码会卡住
        //写入文件,因为有可能多线程写入,次序写入,因此最好用不同的文件名,线程id+ms文件名可以
        pstring strname=plib::getTimeNowNoSplit();
        //        hlog(strname);
        strname=strname+plib::toString(plib::getTid())+".tmp";
        //        hlog(strname);
        pstring strCMDAll=pstring(pwd+" "+host+" \""+cmd+"\" "+strname);
        //        hlog(strCMDAll);
        if(strCMDAll.contain("|"))
        {
            strCMDAll.replaceOnce("|","^|");
        }
        //        hlog(strCMDAll);

        //用这个的时候程序和参数要分开
        plib::openExeHideWaitingFinish("plink.bat",strCMDAll);

        pstring res=plib::getStringFromFile(strname);
        plib::rm(strname);
        return res;
    }
    //调用的qssh中的remoteprocess封装的
    static pstring xsh(pstring host,pstring cmd,pstring pwd="npants0703***",pstring user="root",int port=22)
    {
        //因为直接读取的时候,调试时候没事,但是release的时候会出错,无法读取from standard output,所以采用直接存文件然后再删除的方式
        //由于可能多个线程同时调用,所以加上线程id和ms数,然后再把文件删除,虽然效率不高但是可稳定使用就行
        //        hlog(plib::pwd());
        if(!plib::isExsistFile("qxsh.exe"))
        {
            hlog("there is no qxsh.exe");
            return "";
        }

        //这里是plink是自动接收key版本的,且要加-batch,不然输错密码会卡住
        //写入文件,因为有可能多线程写入,次序写入,因此最好用不同的文件名,线程id+ms文件名可以
        pstring strname=plib::getTimeNowNoSplit();
        //        hlog(strname);
        strname=strname+plib::toString(plib::getTid())+".tmp";
        //        hlog(strname);
        pstring strCMDAll=pstring(host+" \""+cmd+"\" "+pwd+" "+user+" "+plib::toString(port)+" "+strname);
//        hlog(strCMDAll);
        //        if(strCMDAll.contain("|"))
        //        {
        //            strCMDAll.replaceOnce("|","^|");
        //        }
        //        hlog(strCMDAll);

        //用这个的时候程序和参数要分开
        plib::openExeHideWaitingFinish("qxsh.exe",strCMDAll);

        pstring res=plib::getStringFromFile(strname);
        plib::rm(strname);
        //xsh获取的结果是gbk的,要转为utf8
        res=plib::toUTF8(res.c_str());
        return res;
    }
    //直接获取命令行结果,有问题,有时间调
    static pstring xshDirect(pstring host,pstring cmd,pstring pwd="npants0703***",pstring user="root",int port=22)
    {
        //因为直接读取的时候,调试时候没事,但是release的时候会出错,无法读取from standard output,所以采用直接存文件然后再删除的方式
        //由于可能多个线程同时调用,所以加上线程id和ms数,然后再把文件删除,虽然效率不高但是可稳定使用就行
        //        hlog(plib::pwd());
        if(!plib::isExsistFile("qxshnofile.exe"))
        {
            hlog("there is no qxshnofile.exe");
            return "";
        }

        pstring strCMDAll=pstring(host+" \""+cmd+"\" "+pwd+" "+user+" "+plib::toString(port));

        strCMDAll="qxshnofile.exe "+strCMDAll;
        hlog(strCMDAll);
        pstring res=plib::getCmdOutput(strCMDAll);
        return res;
    }

    static pstring xshtest(pstring host,pstring cmd,pstring pwd="scfwq_325")
    {
        //        hlog(plib::pwd());
        if(!plib::isExsistFile("plink.exe"))
        {
            hlog("there is no plink.exe");
            return "";
        }
        //这里是plink是自动接收key版本的,且要加-batch,不然输错密码会卡住
        pstring strCMDAll=pstring("plink.exe -batch -pw ")+pwd+" root@"+host+" "+"\""+cmd+"\"";
        //                hlog(strCMDAll);
        return plib::getCmdOutput(strCMDAll);
    }
    //通过中转服务器hlog(plib::getShellOutputXshBridge("130.242.60.241","172.16.11.6","scfwq_325","df -h"));
    static string xshbridge(string stripDest,string cmd,string strpwd="scfwq_325",string stripBridge="172.16.11.6",string pwdbridge="scfwq_325")
    {
        string info="xsh "+strpwd+" "+stripDest+" '"+cmd+"'";
        hlog(info);
        return xshplink(stripBridge,info,pwdbridge);
    }
    //获取远程根目录使用大小
    static string getRemoteRootDiskUsage(string strhost,string pwd="scfwq_325")
    {
        //        string cmd="xsh scfwq_325 "+strhost+" \"df -h |grep -w '/'\"";
        //                hlog(cmd);
        //        pstring pers=(plib::getShellOutput(cmd));

        pstring pers=plib::xshplink(strhost,"df -h |grep -w '/'",pwd);
        //        hlog(pers);
        //这个错误一般是连接失败
        if(pers.contain("error"))
        {
            hlog(strhost,pers,"连接失败");
            return "";
        }
        if(pers.contain("Access denied"))
        {
            hlog(strhost,pers,"密码错误");
            return "";
        }
        plist<pstring> listp=pers.split(" ");
        //        hlog(listp);
        string strfind;
        for(int i=0;i<listp.size();i++)
        {
            if(listp[i].find('%')!=string::npos)
            {
                strfind=listp[i];
                break;
            }
        }
        string stres=strhost+":"+strfind;
        //        hlog(stres);
        return  stres;
    }
    //获取远程根目录使用大小
    static string getRemoteRootDistUsageBridge(string strhost,string pwd="scfwq_325",string strbridge="172.16.11.6",string pwdbridge="scfwq_325")
    {
        //        string cmd="xsh scfwq_325 "+strhost+" \"df -h |grep -w '/'\"";
        //        //        hlog(cmd);
        //        pstring pers=(plib::getShellOutput(cmd));

        pstring pers=xshbridge(strhost,"df -h |grep -w '/'",pwd,strbridge,pwdbridge);



        //        hlog(pers);
        plist<pstring> listp=pers.split(" ");
        //        hlog(listp);
        string strfind;
        for(int i=0;i<listp.size();i++)
        {
            if(listp[i].find('%')!=string::npos)
            {
                strfind=listp[i];
                break;
            }
        }
        string stres=strhost+":"+strfind;
        return  stres;
    }
    //完美解决弹出黑框问题,适用于需要过程的 直接cmd的话hlog(plib::getCmdOutput("cmd /c dir"));
    //    hlog(trans.uploadFileCallback(host,"D:\\qt-opensource-windows-x86-mingw492-5.5.0.exe",
    //                                  std::bind(&Test::show,&t,placeholders::_1)));
    static pstring getCmdOutputFun(pstring pszCmd,std::function<void(pstring)> fun)
    {
        //因为要调用其他程序,所以要转成gbk,因为windows默认是gbk编码,不然的话会乱码不识别
        //即时转为gbk,但是传到linux下依然是乱码,但是不影响传输了已经
        wstring pszCmd_w = Str2Wstr(plib::toChinese(pszCmd));
        //        wcout << "pszCmd_w is " << pszCmd_w << endl;
        // 创建匿名管道,write->read;
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        HANDLE hRead, hWrite;
        if (!CreatePipe(&hRead, &hWrite, &sa, 0))
        {
            hlog("@ CreatePipe failed!");
            return (" ");
        }
        //        cout << "@0" << endl;
        // 设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite
        STARTUPINFO si = { sizeof(STARTUPINFO) }; // Pointer to STARTUPINFO structure;
        GetStartupInfo(&si);
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        //si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE; //隐藏窗口；
        si.hStdError = hWrite;
        si.hStdError = hWrite;
        si.hStdOutput = hWrite; //管道的输入端口连接命令行的输出；
        // 启动命令行
        PROCESS_INFORMATION pi;// Pointer to PROCESS_INFORMATION structure;
        if (!CreateProcess(NULL,
                           (LPWSTR)pszCmd_w.c_str(),
                           NULL,
                           NULL,
                           TRUE,
                           //FALSE,          // Set handle inheritance to FALSE
                           NULL,
                           //                           CREATE_NEW_CONSOLE,
                           //0,              // No creation flags
                           NULL,
                           NULL,
                           &si,
                           &pi))
        {
            hlog("@ CreateProcess failed!,no find this cmd");
            //            hlog(GetLastError(),WSAGetLastError());
            return ("Cannot create process");
        }
        //        hlog("create process success");
        ////        wstring pszCmd_w = Str2Wstr(pszCmd);
        //        const char* pszCmd_w=pszCmd.c_str();
        //        hlog(pszCmd_w);
        //        //        wcout << "pszCmd_w is " << pszCmd_w << endl;
        //        // 创建匿名管道,write->read;
        //        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        //        HANDLE hRead, hWrite;
        //        if (!CreatePipe(&hRead, &hWrite, &sa, 0))
        //        {
        //            hlog("@ CreatePipe failed!");
        //            return (" ");
        //        }
        //        //        cout << "@0" << endl;
        //        // 设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite
        //        STARTUPINFOA ssi = { sizeof(LPSTARTUPINFOA) }; // Pointer to STARTUPINFO structure;
        //        LPSTARTUPINFOA si=&ssi;
        //        GetStartupInfoA(si);
        //        si->dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        //        //si.dwFlags = STARTF_USESHOWWINDOW;
        //        si->wShowWindow = SW_HIDE; //隐藏窗口；
        //        si->hStdError = hWrite;
        //        si->hStdError = hWrite;
        //        si->hStdOutput = hWrite; //管道的输入端口连接命令行的输出；
        //        // 启动命令行
        //        PROCESS_INFORMATION pi;// Pointer to PROCESS_INFORMATION structure;
        //        if (!CreateProcessA(NULL,
        //                           (LPSTR)pszCmd_w,
        //                           NULL,
        //                           NULL,
        //                           TRUE,
        //                           //FALSE,          // Set handle inheritance to FALSE
        //                           NULL,
        //                           //                           CREATE_NEW_CONSOLE,
        //                           //0,              // No creation flags
        //                           NULL,
        //                           NULL,
        //                           si,
        //                           &pi))
        //        {
        //            hlog("@ CreateProcess failed!,no find this cmd");
        //            //            hlog(GetLastError(),WSAGetLastError());
        //            return ("Cannot create process");
        //        }
        CloseHandle(hWrite);//关闭管道的输入端口；
        // 读取命令行返回值
        string strRetTmp;
        char buff[65535] = { 0 };
        int len=65535;
        bzero(buff,len);
        DWORD dwRead = 0;
        strRetTmp = buff;
        //如果拷贝的话,过程中的输出,比如传输百分比显示在这里
        while (ReadFile(hRead, buff, len, &dwRead, NULL))//从管道的输出端获取命令行写入的数据；
        {
            //                        cout << "buff = " << buff << endl;
            //            hlog(buff);
            fun(buff);
            strRetTmp += buff;
        }
        CloseHandle(hRead);//关闭管道的输出端口；
        //        cout << "strRetTmp:"<<strRetTmp << endl;
        //不用加finish,因为如果是传输的话,一定最后是100%,用100来判断就行了
        //        fun("finish");
        return strRetTmp;
    }
    //完美解决弹出黑框问题  直接cmd的话hlog(plib::getCmdOutput("cmd /c dir"));
    static pstring getCmdOutput(pstring pszCmd)
    {
        //因为要调用其他程序比如pscp,所以要转成gbk,因为windows默认是gbk编码,不然的话会乱码
        //不要转了,如果为了pscp改成gbk,那ping就用不了了,辣鸡,pscp就别用中文了,
        wstring pszCmd_w = Str2Wstr(plib::toChinese(pszCmd));
        //        wstring pszCmd_w = Str2Wstr((pszCmd));
        //        wcout << "pszCmd_w is " << pszCmd_w << endl;
        // 创建匿名管道,write->read;
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        HANDLE hRead, hWrite;
        if (!CreatePipe(&hRead, &hWrite, &sa, 0))
        {
            hlog("@ CreatePipe failed!");
            return (" ");
        }
        //        cout << "@0" << endl;
        // 设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite
//        STARTUPINFO si = { sizeof(STARTUPINFO) }; // Pointer to STARTUPINFO structure;
        STARTUPINFO si;
        si.cb=sizeof(STARTUPINFO);
        GetStartupInfo(&si);
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        //si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE; //隐藏窗口；
//        si.hStdError = hWrite;
        si.hStdError = hWrite;
        si.hStdOutput = hWrite; //管道的输入端口连接命令行的输出；
        // 启动命令行
        PROCESS_INFORMATION pi;// Pointer to PROCESS_INFORMATION structure;
        if (!CreateProcess(NULL,
                           (LPWSTR)pszCmd_w.c_str(),
                           NULL,
                           NULL,
                           TRUE,
                           //FALSE,          // Set handle inheritance to FALSE
                           NULL,
                           //                           CREATE_NEW_CONSOLE,
                           //0,              // No creation flags
                           NULL,
                           NULL,
                           &si,
                           &pi))
        {
            hlog("@ CreateProcess failed!,no find this cmd");
            //            hlog(GetLastError(),WSAGetLastError());
            return ("Cannot create process");
        }

        CloseHandle(hWrite);//关闭管道的输入端口；
        // 读取命令行返回值
        string strRetTmp;
        char buff[65535] = { 0 };
        int len=65535;
        bzero(buff,len);
        DWORD dwRead = 0;
        strRetTmp = buff;
        //如果拷贝的话,过程中的输出,比如传输百分比显示在这里
        while (ReadFile(hRead, buff, len, &dwRead, NULL))//从管道的输出端获取命令行写入的数据；
        {
            //                        cout << "buff = " << buff << endl;
            hlog(buff);
            //不要加这个打印,否则全变了gbk utf8不显示
            //            hlog(plib::toChinese(buff));
//                        hlog(WSAGetLastError());
            strRetTmp += buff;
        }
        CloseHandle(hRead);//关闭管道的输出端口；

        //        cout << "strRetTmp:"<<strRetTmp << endl;
        return strRetTmp;
    }

    //    //获取windows cmd执行结果 会弹黑框,去掉
    //    //windows只认C:\Users\这种的
    //    static pstring getCmdOutput(pstring cmd)
    //    {
    //        //原来的,不支持ping,改成现在这种可以
    ////        CmdExc ce(cmd);
    ////        return ce.getOutput();
    //        pstring res;
    //        FILE * fp;
    //        if ((fp = _popen(cmd.c_str(), "r")) == NULL)
    //        {
    //            printf("Popen Error!\n");
    //            return "";
    //        }
    //        else
    //        {
    //            char pRetMsg[6666];
    //            cinit(pRetMsg);
    //            //get lastest result
    //            while (fgets(pRetMsg, sizeof(pRetMsg), fp) != NULL)
    //            {
    //                res+=pRetMsg;
    //            }

    //            if ((_pclose(fp)) == -1)
    //            {
    //                printf("close popenerror!\n");
    //                return "";
    //            }
    //            return res;
    //        }
    //    }

    static bool isDirEmpty(string path)
    {
        string strpath;
        if(path[path.size()-1]!='/')
            strpath=path+"/";
        else
            strpath=path;

        long hFind=0;
        //文件信息
        struct _finddata_t fileinfo;  //很少用的文件信息读取结构
        if((hFind=_findfirst(string(strpath+"*.*").c_str(),&fileinfo)) != -1)
        {

            bool EmptyDirectory = true;
            do
            {
                //                hlog(fileinfo.name);

                if(strcmp(fileinfo.name,".") == 0
                        || strcmp(fileinfo.name,"..") == 0)
                {
                    continue;
                }
                else
                {
                    //有子文件夹也算非空
                    EmptyDirectory = false;
                    break;
                }
            } while (_findnext(hFind, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
            _findclose(hFind);
            if(EmptyDirectory)
            {
                //                    cout<<"Empty"<<endl;
                return true;
            }
            else
            {
                //                    cout<<"Not Empty"<<endl;
                //                hlog("not");
                return false;
            }
        }
        else
        {
            //                cout<<"Path does not exist."<<endl;
            return true;
        }
    }
    static string getPathType(string path)
    {
        if(access(path.c_str(),0)<0)
            return "noaccess";
        string strpathnoxg=path;
        if(strpathnoxg[strpathnoxg.size()-1]=='/')
            strpathnoxg=strpathnoxg.substr(0,strpathnoxg.size()-1);
        //文件句柄
        long   hFile = 0;
        //文件信息
        struct _finddata_t fileinfo;  //很少用的文件信息读取结构
        string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
        if ((hFile = _findfirst(p.assign(strpathnoxg).c_str(), &fileinfo)) != -1)
        {
            if ((fileinfo.attrib &  _A_SUBDIR))  //比较文件类型是否是文件夹
            {
                return ("dir");
            }
            else
            {
                return ("file");
            }
        }
        return "";
    }
    static plist<plist<pstring> > getAllFilesAndPathsRecurision(const string path)
    {
        //获取全路径
        vector<string> lfall;
        vector<string> lpall;
        common_getAllFilesAndPathsRecursion(path,lfall,lpall);

        plist<pstring> plfall;
        plist<pstring> plpall;
        for(int i=0;i<lfall.size();i++)
            plfall.append(lfall[i]);
        for(int i=0;i<lpall.size();i++)
            plpall.append(lpall[i]);


        //    hlog(plpall);
        //获取相对路径等
        int lenRemove=path.size();
        if(path[path.size()-1]!='/')
            lenRemove++;
        //        hlog(path);
        //    hlog(path.size());
        hlog(lenRemove);
        //    相对路径的文件和相对路径的目录,要加上上一目录，不然考过去全是文件夹下的所有东西

        string strLastDir=path;
        if(path[path.size()-1]!='/')
            strLastDir+="/";
        //    hlog(strLastDir);
        plist<pstring> lspit=pstring(strLastDir).split("/");
        //        hlog(lspit);
        pstring pstrlast=lspit[lspit.size()-2];
        //    hlog(pstrlast);
        strLastDir=pstrlast+"/";
        //    hlog(strLastDir);




        plist<pstring> plf;
        plist<pstring> plp;
        for(int i=0;i<plfall.size();i++)
        {
            //要增加上一目录
            //        plf.append(strLastDir+lfall[i].substr(lenRemove,lfall[i].size()));
            plf.append(lfall[i].substr(lenRemove,lfall[i].size()));
        }
        for(int i=0;i<plpall.size();i++)
        {
            //        plp.append(strLastDir+lpall[i].substr(lenRemove,lpall[i].size()));
            plp.append(lpall[i].substr(lenRemove,lpall[i].size()));
        }


        plist<plist<pstring> > lres;
        lres.append(plfall);
        lres.append(plpall);
        lres.append(plf);
        lres.append(plp);
        return lres;
    }
    static void mkdirp(string path)
    {
        string strpath=path;
        if(path[path.size()-1]!='/')
            strpath+="/";
        //    hlog(strpath);
        int epos=strpath.length();
        int fpos=strpath.find("/",0);
        //    hlog(fpos);
        string subdir;
        do
        {
            epos=strpath.find("/",fpos+2);
            if(epos!=-1)
            {
                subdir=strpath.substr(0,epos);
                //            hlog(subdir);
                (_mkdir(subdir.c_str()));
            }
            fpos=epos;
        }while(fpos!=-1);
    }
    static string getmd5(string file)
    {
        ifstream in(file.c_str(), ios::binary);
        if (!in)
            return "";

        MD5 md5;
        std::streamsize length;
        char buffer[512*1024];
        memset(buffer,0,512*1024);
        while (!in.eof()) {
            in.read(buffer, 512*1024);
            length = in.gcount();
            if (length > 0)
                md5.update(buffer, length);
        }
        in.close();
        return md5.toString();
    }
    //打开的时候要转成gbk,不然windows下有问题
    static long long  getFileSize(string strfullpath)
    {
        //        ulonglong size=-1;
        //        // C++获取文件大小方法二
        //        FILE* file = fopen(strfullpath.c_str(), "rb");
        //        if (file)
        //        {
        //            size = filelength(fileno(file));
        //            hlog(size);
        //            fclose(file);
        //        }
        //        return size;


        long long  size=-1;
        //方法五，使用标准C++库函数实现
        fstream ffile;
        //        ffile.open(strfullpath, ios::in | ios::binary | ios::_Nocreate);
        ffile.open(plib::toChinese(strfullpath), ios::in | ios::binary | ios::_Nocreate);
        if (ffile.is_open())
        {
            ffile.seekg(0, ffile.end);
            __int64 fileSize = ffile.tellg();
            ffile.close();
            size=fileSize;
        }
        else
        {
            hlog("open file fail");
        }
        return size;

    }

    static bool checkone()
    {
        string strpath=(plib::getCurrentUserDir());
        strpath+="test.dat";

        //如果找不到文件,则false
        if(!plib::isExsistFile(strpath))
        {
            hlog("找不到文件");
            hlog(strpath);
            return false;
        }
        protect pload=plib::loadSerialization<protect>(strpath);
        //        hlog(pload);
        double dbModify=pload.tdes-pload.tlast;
        //        hlog(dbModify);
        if(dbModify!=0)
        {
            hlog("文件被修改了");
            hlog(strpath);
            return false;
        }
        //存一下
        pload.tlast=pload.tdes;
        plib::saveSerialization(strpath,pload);

        ptime timeDes=pload.tdes;
        ptime timeNow;

        //如果超时了
        double dbres=timeNow-timeDes;
        hlog(dbres);
        if(dbres>0)
        {
            string strinfo="已超过软件试用期,当前时间为"+plib::toString(timeNow)+",到期时间是"+plib::toString(timeDes);
            hlog(strinfo);
            return false;
        }


        return true;
        //如果当前时间超过目标时间,则false
    }
    static void threadall()
    {
        bool bres=checkone();
        while(bres)
        {
            bres=(checkone());
            Sleep(5000);
        }
        hlog("程序退出");
        exit(0);
    }
    static void checkExpire()
    {
        thread th(threadall);
        th.detach();
    }
    //获取当前用户目录
    static string getCurrentUserDir()
    {
        std::string path= "";
        const char* homeProfile = "USERPROFILE";
        char homePath[1024] = {0};
        unsigned int pathSize = GetEnvironmentVariableA(homeProfile, homePath, 1024);
        if (pathSize == 0 || pathSize > 1024)
        {
            // 获取失败 或者 路径太长
            //          int ret = GetLastError();
            return path;
        }
        else
        {
            path = std::string(homePath);
            path.push_back('\\');
            return path;
        }
    }
    static plist<pstring> getAllFilesRecurion(string path);
    // c:/Users/...
    static pstring pwd();
    /*读取配置文件
     * [section]
     * key=value
     * 默认是当前目录的xlfd.conf，支持中文
     * 配置文件请按格式来,否则不保证函数正确
     * section后边不要加注释,配置项后边可以加注释,可以有空行,key后要紧跟=不要有空格,key前边也不要有空格
     */
    static string getConfString(string section, string key);
    //删除配置
    static bool delConfString(string section,string key);
    //根据section和key增加配置,已有的不加,么有section加section
    static bool addConfString(string section, string key, string value);
    static bool setConfString(string section,string key,string value)
    {
        if(!plib::isExsistFile(strPathConf))
        {
            hlog(pstring()<<"找不到配置文件"<<strPathConf);
            return false;
        }
        string strPathPack=strPathConf;
        if(strPathConf.find("\\")==string::npos&&strPathConf.find("/")==string::npos)
            strPathPack="./"+strPathConf;
        hlog(strPathPack);
        //        //这种方式会导致写入两个port不知道为啥
        //        if(::WritePrivateProfileStringA(section.c_str(),key.c_str(),value.c_str(),strPathPack.c_str())>0)
        //            return true;
        //        return false;
        int ires=PutIniKeyString(section.c_str(),key.c_str(),value.c_str(),strPathPack.c_str());
        hlog(ires);
        if(ires<0)
            return false;
        return true;


        //        int ires=plib_setConfString(section.c_str(),key.c_str(),value.c_str());
        ////        int ires=PutIniKeyString(section.c_str(),key.c_str(),value.c_str());
        //        hlog(ires);
        //        if(ires<0)
        //        {
        //            string info="读取配置文件失败，请检查路径"+strPathConf+"或section:"+section+"配置项"+key+"是否正确";
        //            hlog(info);
        //            return false;
        //        }
        //        else
        //        {
        //            string info="成功将["+section+"]下的配置项"+key+"的值改为"+value;
        //            hlog(info);
        //            return true;
        //        }
    }

    static int setPathConf(string strnewpath);
    static int initLog(string strnewpath);


    /*string str=string("    pstring name;\n") +
                  "    double db;\n" +
                  "    int id;";
                  原始字符串如上所示,直接复制到idea中去生成字符串,所有字段的,然后调用这个函数
                  解析,生成is和os,简化序列化过程
                   要求是字段名必须之后紧跟分号，要不然识别不了 int data; unsigned long long udata;
                   对内部有指针的不支持序列化,声明后边不要带注释,不支持char数组
        */
    static void generateISandOS(string str)
    {

        //根据行分割先
        vector<string> lstr=split(str,"\n");

        for(int i=0;i<lstr.size();i++)
        {
            string pi=lstr[i];

            vector<string> listpi=split(pi," ");


            //再把每个根据空格分割,第一个带分号的一定是字段名字
            for(int j=0;j<listpi.size();j++)
            {
                string pj=listpi[j];
                if(pj.size()>0&&pj[pj.size()-1]==';')
                {
                    lstr[i]=pj;

                    break;
                }
            }
            //去掉分号
            pi=lstr[i];
            lstr[i]=pi.substr(0,pi.size()-1);
        }
        //现在是纯字段名的列表


        //现在开始组ostream字符串
        string stros="os";
        for(int i=0;i<lstr.size();i++)
        {
            string pi=lstr[i];
            //这是变量名
            //            hlog(pi);
            //要支持data[333]这种的要在此处判断,去掉括号后边的东西
            string pval=pi;
            //            hlog(pval);
            int ifind=pval.find('[');
            pval=pval.substr(0,ifind);
            //            hlog(pval);
            stros+="<<\""+pi+": \"<<x."+pval+"<<\" \"";
        }
        stros+=";";
        hlog(stros);
        //现在开始组istream字符串
        string stris="";
        for(int i=0;i<lstr.size();i++)
        {
            string pi=lstr[i];
            string pval=pi;
            //            hlog(pval);
            int ifind=pval.find('[');
            pval=pval.substr(0,ifind);
            stris+=string("plib::skipCountch(is,")+plib::toString(pi.size()+2)+");is>>x."+pval+";";
            //最后去掉空格，不然有字符串的话会失败
            stris+="is.get();";
        }
        hlog(stris);
    }
    //is跳过n个字符
    static void skipCountch(istream &is,int len)
    {
        for(int i=0;i<len;i++)
            is.get();
    }
    //is提取字符串,跳过空格,根据[]来判断
    static string istreamToString(istream& is)
    {
        return common_istreamToString(is);
    }
    //跳过[之前的所有
    static void skipDescription(istream &is){
        return common_skipDescription(is);
    }
    //是否包含字母
    static bool isContaiAlpha(string str);
    static string hostToIp(const char * url);
    //login vncserver on linux/unix
    static void loginVnc(pstring strFullPathVncExe,pstring strIpWithPort,pstring strpwd);
    static void loginVncCString(CString vnc,CString ip,CString pwd);
    static void loginSsh(pstring strFullPathPuttyExe,pstring strip,pstring strpwd);

    //putty.exe直接放到plib文件夹下,虽然最后编译的exe放到plib下的debug下,但是编译的时候就在plib下
    static void ssh(pstring ip,pstring pwd)
    {
        return loginSsh("putty.exe",ip,pwd);
    }
    static void vnc(pstring ip,pstring pwd)
    {
        return loginVnc("vncviewer.exe",ip,pwd);
    }

    static string getTimeStringFromCTime(CTime time);
    //CTime与string互相转化
    static CTime toCTime(string str);
    static string toString(CTime time);

    //判断文件是否存在
    static bool isExsistFile(string strpath);
    //判断文件夹是否存在
    static bool isExsistDir(string strpath);


    template <class T>
    static void saveSerialization(string path,T t)
    {
        ofstream outFile(path);
        outFile<<t;
        //关闭文件
        outFile.close();
    }
    template <class T>
    static T loadSerialization(string path)
    {
        T t;
        ifstream fin( path);
        fin>>t;
        fin.close();
        return t;
    }

    static int saveo(string path,char* data,int len)
    {
        FILE *fp;
        if((fp=fopen(path.c_str(),"wb"))==NULL)
        {
            std::cout<<"open fail"<<std::endl;
            return -1;
        }
        fwrite(data,len,1,fp);
        fclose(fp);
        return 0;
    }
    //保存任意类型的结构或类
    template <class T>
    static int save(string path,T l)
    {
        FILE *fp;
        if((fp=fopen(path.c_str(),"wb"))==NULL)
        {
            std::cout<<"open fail"<<std::endl;
            return -1;
        }
        fwrite(&l,sizeof(T),1,fp);
        fclose(fp);
        return 0;
    }


    /* 读取结构体
     * test t=plib::load<test>("/opt/test.dat");*/
    template <class T>
    static T load(string path)
    {
        T lr;
        FILE *fp;
        if((fp=fopen(path.c_str(),"rb"))==NULL)
        {
            std::cout<<"open fail"<<std::endl;
            return lr;
        }
        fread(&lr,sizeof(T),1,fp);
        fclose(fp);
        return lr;
    }

    //保存向量
    template <class T>
    static int savevector(string path,vector<T> l)
    {
        FILE *fp;
        if((fp=fopen(path.c_str(),"wb"))==NULL)
        {
            return -1;
        }
        size_t len=l.size();
        fwrite(&len,sizeof(size_t),1,fp);
        int i;
        for(i=0;i<l.size();i++)
        {
            fwrite(&(l[i]),sizeof(T),1,fp);
        }
        fclose(fp);
        return 0;
    }

    /*读取向量
     * vector<test> vr=plib::loadvector<test>(path);*/
    template <class T>
    static vector<T> loadvector(string path)
    {
        vector<T>  lr;
        FILE *fp;
        if((fp=fopen(path.c_str(),"rb"))==NULL)
        {
            return lr;
        }
        //首先要清空lr，不然一直增长
        lr.clear();
        size_t len=0;
        fread(&len,sizeof(size_t),1,fp);
        //        XCLOG_INT(len);
        int i;
        for(i=0;i<len;i++)
        {
            void* rd=(void*)malloc(sizeof(T));
            fread(rd,sizeof(T),1,fp);
            lr.push_back((*(T*)rd));
            free(rd);rd=NULL;
        }
        fclose(fp);
        return lr;
    }


    //判断过期以及获取CTime时间方法,通过网络时间判断
    static bool isExpire(int year,int month,int day,int hour,int minite,int second);
    //获取网络时间
    static CTime getCTimeNetTime();

    //打开程序,等待执行完成,一般用于写入文件后从文件读取结果
    static void openExeHideWaitingFinish(string strFullPath,string strPara="")
    {
        //        hlog(strFullPath,strPara);
        CString cpath=plib::StringToCString(strFullPath);
        CString cpara=plib::StringToCString(strPara);
        SHELLEXECUTEINFO ShExecInfo = {0};
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShExecInfo.hwnd = NULL;
        ShExecInfo.lpVerb = NULL;
        //        ShExecInfo.lpFile = L"plink.bat";//调用的程序名
        ShExecInfo.lpFile=cpath;
        if(strPara=="")
            ShExecInfo.lpParameters=NULL;
        else
        {
            //ShExecInfo.lpParameters = L"scfwq_325 172.16.11.6 \"ps -ef^|grep SCS\" zzz.txt";//调用程序的命令行参数
            ShExecInfo.lpParameters =cpara;//调用程序的命令行参数
        }
        ShExecInfo.lpDirectory = NULL;
        ShExecInfo.nShow = SW_HIDE;//窗口状态为隐藏
        ShExecInfo.hInstApp = NULL;
        ShellExecuteEx(&ShExecInfo);
        WaitForSingleObject(ShExecInfo.hProcess,INFINITE);////等到该进程结束
        //        hlog("end");
    }
    //打开窗口隐藏不见,这是为了配合嵌入窗口
    static  bool openExeHide(string strFullPath, string strPara="")
    {
        CString cstrFullPath=plib::StringToCString(strFullPath);
        if(strPara=="")
        {
            return ShellExecute(::GetDesktopWindow(),_T("open"),cstrFullPath,NULL,NULL,SW_HIDE);
        }
        else
        {
            CString cstrPara=plib::StringToCString(strPara);
            return ShellExecute(::GetDesktopWindow(),_T("open"),cstrFullPath,cstrPara,NULL,SW_HIDE);
        }
    }
    //自动操作类--后台
    //这两个打开外部程序一般没问题,但是打开teamviewer8时出错,打开就程序推出了,要用qt的qprocess打开也不行,
    //最后不用这个,用C#封装的dll即可
    static bool openExe(string strFullPath,string strPara="");


    //通过程序名杀死进程
    static bool closeProcByName(pstring name)
    {
        CString cstrname=StringToCString(name);
        vector<CString> vcstr;
        vcstr.push_back(cstrname);
        return LookUpProcessAndKill(vcstr);
    }

    /*******************************************************************************
    函数名称  : LookUpProcessAndKill
    函数描述  : 遍历进程找到需要杀死的进程
    输入参数  : N/A
    输出参数  : N/A
    返回值    : N/A
    作者      : N/A
    *******************************************************************************/
    static BOOL  LookUpProcessAndKill(const  vector<CString>  &vecProcessName)
    {
        BOOL bRet = FALSE;
        HANDLE SnapShot, ProcessHandle;
        SHFILEINFO shSmall;
        PROCESSENTRY32 ProcessInfo;

        CString strSearchName;
        CString strRunProcessName;
        SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if(SnapShot != NULL)
        {
            ProcessInfo.dwSize = sizeof(ProcessInfo);  // 设置ProcessInfo的大小
            BOOL Status = Process32First(SnapShot, &ProcessInfo);

            //提升升级工具进程权限为SE_DEBUG_NAME，否则XP下无法杀掉进程
            HANDLE hToken;
            if(OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
            {
                LUID luid;
                if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
                {
                    TOKEN_PRIVILEGES TokenPrivileges;
                    TokenPrivileges.PrivilegeCount = 1;
                    TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                    TokenPrivileges.Privileges[0].Luid = luid;
                    AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, 0, NULL, NULL);
                }
                CloseHandle(hToken);
            }

            while(Status)
            {
                // 获取进程文件信息
                SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall, sizeof(shSmall), SHGFI_ICON|SHGFI_SMALLICON);

                // 检测进程是否需要关闭
                strRunProcessName = ProcessInfo.szExeFile;
                strRunProcessName.MakeLower();
                for (vector<CString>::size_type it = 0; it != vecProcessName.size(); ++ it)
                {
                    strSearchName = vecProcessName[it];
                    strSearchName.MakeLower();
                    if(strSearchName == strRunProcessName)
                    {
                        // 获取进程句柄，强行关闭
                        ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessInfo.th32ProcessID);
                        if (ProcessHandle != NULL)
                        {
                            bRet = TerminateProcess(ProcessHandle, 1);
                            CloseHandle(ProcessHandle);
                        }

                        break;
                    }

                }

                // 获取下一个进程的信息
                Status = Process32Next(SnapShot, &ProcessInfo);
            }
        }

        return  bRet;
    }
    static void closeProcessbyHandle(HWND h);
    static bool closeProcByTitle(pstring name)
    {
        HWND h=plib::findWindow(NULL,name.c_str());
        if(h)
        {
            plib::closeProcessbyHandle(h);
            hlog(pstring()<<"成功杀死进程"<<name.c_str());
            return true;
        }
        else
        {
            hlog(pstring()<<"没有找到该进程:"<<name.c_str());
            return false;
        }
    }

    //获取客户坐标像素点颜色
    static COLORREF getColorFromHWND(HWND h,int x,int y);
    //获取窗口大小
    static SIZE getSizeFromHWND(HWND h);
    //从屏幕坐标转换为客户坐标
    static POINT getPointClientFromScreen(HWND h, int x, int y);
    //#32770 (对话框)这种的,不要后边的括号,只要前边就行,或者用抓抓,别用spy
    //当easyconnect这种有两个窗口的,用findwindow只能找到那个没用的,要用
    //common_getWindowAllByTitle
    static HWND findWindow(const char *arrchClass, const char *arrchTitle);
    //第二参数用于多个同样标题或类名的时候,找到第一个把第一个h放after就可以找了
    static HWND findWindowEx(HWND hparent,HWND hchildafter,const char* arrchClass,const char* arrchTitle);
    //寻找同级别下个窗口
    static HWND findWindowNext(HWND h)
    {
        return ::GetNextWindow(h,GW_HWNDNEXT);
    }

    //通过索引查找相应控件句柄,比如先找到父句柄,下边都是一样的,则可以根据索引来确定是哪个
    //index从1开始,如果是0,则返回父句柄
    static HWND findWindowByIndex(HWND hwndParent, int index)
    {
        if (index == 0)
        {
            return hwndParent;
        }
        else
        {
            int ct = 0;
            HWND result = NULL;
            do
            {
                result = plib::findWindowEx(hwndParent, result, NULL, NULL);
                if (result !=NULL)
                {
                    ++ct;
                }
            } while (ct < index && result != NULL);
            return result;
        }
    }


    //通过WM_GETTEST获取控件内容
    static string getContentByHWND(HWND h);
    //判断窗口是否可见
    static bool isWindowVisible(HWND h)
    {
        return ::IsWindowVisible(h);
    }

    //根据句柄获取标题
    static string getWindowTitleByHWND(HWND h)
    {
        char arrchWindowTitle[256];
        cinit(arrchWindowTitle);
        ::GetWindowTextA(h,arrchWindowTitle,sizeof(arrchWindowTitle));
        return string(arrchWindowTitle);
    }
    //根据句柄获取类名
    static string getWindowClassByHWND(HWND h)
    {
        char arrchClassName[256];
        cinit(arrchClassName);
        GetClassNameA(h, arrchClassName, sizeof(arrchClassName));
        return string(arrchClassName);
    }
    //获取所有相同标题的句柄集合//顶级可见窗口
    static plist<HWND> getWindowAllByTitle(string title)
    {
        plist<HWND> lres;
        HWND h=plib::findWindow(NULL,title.c_str());
        if(plib::isWindowVisible(h))
            lres.append(h);
        HWND hnext=plib::findWindowNext(h);
        while(hnext)
        {
            if(plib::getWindowTitleByHWND(hnext)==title&&
                    plib::isWindowVisible(hnext))
                lres.append(hnext);
            hnext=plib::findWindowNext(hnext);
        }
        return lres;
    }
    //获取所有相同标题的可见句柄集合,子集窗口
    static plist<HWND> getWindowAllByTitleInHWND(HWND hparent,string title)
    {
        plist<HWND> lres;
        //        HWND h=plib::findWindow(NULL,title.c_str());
        HWND h=plib::findWindowEx(hparent,NULL,NULL,title.c_str());
        //找不到就是空
        if(h==NULL)
            return lres;
        //找到了也要判断一下是不是不可见的
        if(plib::isWindowVisible(h))
            lres.append(h);
        HWND hnext=plib::findWindowNext(h);
        while(hnext)
        {
            //            hlog(hnext);
            //可见窗口且标题一样
            if(plib::getWindowTitleByHWND(hnext)==title&&
                    plib::isWindowVisible(hnext))
                lres.append(hnext);
            hnext=plib::findWindowNext(hnext);
        }
        return lres;
    }



    //mfc版messagebox
    static void messagebox(string str);
    static void postInputString(HWND hwnd, string str);
    static void postRightClick(HWND hwnd, int x, int y);
    static void postLeftClick(HWND hwnd, int x, int y);
    static void postMouseMove(HWND hwnd,int x,int y);
    static void delay(int sec);
    static void postAnjian(HWND hwnd, int jianma);
    static void sendInputString(HWND hwnd, string str);
    static void sendRightClick(HWND hwnd, int x, int y);
    static void sendLeftClick(HWND hwnd, int x, int y);
    static void sendAnjian(HWND hwnd, int jianma);
    static void sendButtonClick(HWND h);

    //字符串分割
    static vector<string> split(string str, string pattern);
    //获取整数的位数
    static int getDigit(int i);
    //从文件获取字符串
    static string getStringFromFile(string path);
    static plist<pstring> getAllLinesFromFile(string path)
    {
        plist<pstring> vstr;
        //以下是C读法，有个弊端是将换行副也读取了
        //    FILE *fp = fopen(path.c_str(),"r");
        //    if(fp==NULL)
        //    {
        //        return vstr;
        //    }
        //    char buff[1024];
        //    while(fgets(buff , 1024 , fp)!= NULL)
        //    {
        //        vstr.push_back(string(buff));
        //        //        hlog(buff);
        //    }
        //    fclose(fp);

        //以下是C+读发法，会自动去掉换符号，前提是不能设置一字节对齐，不然用不了
        ifstream fin(path.c_str());
        if(!fin)
        {
            return vstr;
        }
        string stread;
        while(getline(fin,stread))
        {
            //        hlog(stread);
            vstr.push_back(stread);
        }

        fin.close();

        return vstr;
    }

    //由keyFind字符串位置后插入文本,暂时先整个覆盖
    static bool insertStringToFile(string path,string content,string keyFind);
    //追加写入文件
    static bool appendToFileOneLine(string path, string content);
    //覆盖写入文件,如果不存在，则创建
    static bool overrideToFile(string path, string content);
    static pstring getContentsAllFromFile(const string path) {
        ifstream is(path.c_str());
        if (!is) {
            hlog("open file fail");
            return "";
        }
        std::stringstream buffer;
        buffer << is.rdbuf();
        string contents(buffer.str());
        hlog(contents);
        is.close();
        return contents;
    }
    //获取当前年
    static int getNowYear();
    //向文件写入字符串
    static void writeStringToFile(string path,string str);


    //序列化方案,以[为分解单位,所以字符串中不要出现[,有空格没关系,支持
    template <class T>
    static string toString(T t)//与类中重载的对应
    {
        stringstream ss;
        ss<<t;
        return ss.str();
    }

    template <class T>
    static T toClass(string str)
    {
        T t;
        stringstream ss(str);
        ss>>t;
        return t;
    }

    template <class T>
    static void toClass(string str,T &t)
    {
        stringstream ss(str);
        ss>>t;
    }




    static string getPathFromFullPath(string strFullPath)
    {
        vector<string> lstr=split(strFullPath,"/");
        string stres;
        for(int i=0;i<lstr.size()-1;i++)
            stres+=lstr[i]+"/";
        return stres;
    }

    //支持/ 以及\\以及只有名字没有路径
    static string getFileNameFromFullPath(string strFullPath)
    {
        //两种都支持
        //        vector<string> lstr=split(strFullPath,"\\/");
        pliststring lsplit=pstring(strFullPath).split("\\/");
        return lsplit[lsplit.size()-1];
    }





    static CString StringToCString(const string str)
    {
        return CString(str.c_str());
    }
    static string CStringToString( CString theCStr)
    {
        const int theCStrLen = theCStr.GetLength();
        char *buffer = (char*)malloc(sizeof(char)*(theCStrLen+1));
        memset((void*)buffer, 0, sizeof(buffer));
        WideCharToMultiByte(CP_UTF8, 0, static_cast<CString>(theCStr).GetBuffer(), theCStrLen, buffer, sizeof(char)*(theCStrLen+1), NULL, NULL);

        std::string STDStr(buffer);
        //        free(buffer);
        return STDStr;
    }
private:
    /*时间相关函数,供xIsExpire调用,
     *需要#include <atltime.h>
     *   #include <WinSock2.h>
     *  #include <windows.h>
     *   #pragma comment(lib, "WS2_32")等
     */

    /*第二种方法,比较靠谱*/
    static unsigned long getULNetTime2();

};




//使用全局变量不安全,多线程没法用,已经重新写了,使用getnextwindow来实现,再plib中
////因为遍历窗口的函数不能放到成员函数中,所以用全局变量代替
////所有顶层可见窗口
//extern pmap<HWND,string> mapWindowHWNDAll;
////EnumWindowsProc具有特殊性,不能h定义cpp实现,必须放在cpp实现
////所以该调用函数也只能这里定义cpp实现
//plist<HWND> common_getWindowAllByTitle(string title);





//回执结构体
class preply
{
public:
    preply()
    {

    }

    preply(bool bres,pstring strinfo)
    {
        this->bres=bres;
        this->strinfo=strinfo;
    }
    void set(bool bres,pstring strinfo)
    {
        this->bres=bres;
        this->strinfo=strinfo;
    }

    bool bres;
    pstring strinfo;
    friend ostream& operator<<(ostream& os,preply x)
    {
        os<<"bres: "<<x.bres<<" "<<"strinfo: "<<x.strinfo<<" ";
        return os;
    }
    friend istream& operator>>(istream& is,preply& x)
    {
        plib::skipCountch(is,6);is>>x.bres;is.get();plib::skipCountch(is,9);is>>x.strinfo;is.get();
        return is;
    }

};



//好像很慢,暂时不用,回头研究
class CPing
{
public:
    CPing(void);
    ~CPing(void);

    bool CanPing(const char* pDscAddr);
    void Ping(const char* pDscAddr, int nPackNum = 4, int nPackSize = 32);

private:
    bool Init();
    bool FInit();
    bool ConstructIcmpMessage(char* pDataBuff, int nDataSize);
    USHORT CheckSum(USHORT* pBuff, int nSize);
    bool DecodeIpPacket(char* pBuff, int nBuffSize, sockaddr_in* pAddr);
    bool PingHelper(const char* pDscAddr, int nPackNum = 4, int nPackSize = 32);

private:
    SOCKET m_socketRaw;
    bool m_bPrintInfo;
};
void printString(pstring str);
//#pragma execution_character_set("utf-8")


//以下是获取传输所有服务器信息类
class TASK_HEAD {
public:
    int type;
    int length;
    TASK_HEAD()
    {
        this->length=0;
    }
};
class TASK_ADDCONF{
public:
    char arrchkey[50];
    char arrchval[50];
    TASK_ADDCONF()
    {
        bzero(arrchkey,sizeof(arrchkey));
        bzero(arrchval,sizeof(arrchval));
    }
};

#define HTYPEGETCONF 91111111
#define HTYPEADDCONF 91111112
//获取配置文件端口8888
class CSGLStation {
public:
    pstring strName;//站点名称
    pstring strip;//站点ip
    pmap<pstring, pstring> mapconf;//该站点所有配置
    pstring strPathFullConf;//配置文件路径

    ptcp *tcpc;

    CSGLStation() {
        strPathFullConf = "/opt/sc/csgl/SCService/sc.conf";
    }

    CSGLStation(pstring name, pstring strip, pstring pathconf = "/opt/sc/csgl/SCService/sc.conf") {
        this->strName = name;
        this->strip = strip;
        this->strPathFullConf = pathconf;
        this->tcpc = new ptcp(strip, 8888);
    }
    pmap<pstring,pstring> getConfMap()
    {
        return mapconf;
    }
    void connect()
    {
        hlog(this->tcpc->connectx());
    }
    bool addConf(pstring key,pstring val)
    {
        hlog(key,val);
        TASK_HEAD head;
        head.type=HTYPEADDCONF;
        TASK_ADDCONF struAddConf;
        strcpy(struAddConf.arrchkey,key.c_str());
        strcpy(struAddConf.arrchval,val.c_str());
        //发过去
        if(tcpc->send(head)<0)
        {
            plib::sleep(1000);
            return false;
        }
        if(tcpc->send(struAddConf)<0)
        {
            plib::sleep(1000);
            return false;
        }
        //接收bool
        bool bRes;
        if(tcpc->recv(bRes)<0)
        {
            plib::sleep(1000);
            return false;
        }
        hlog(bRes);
        return bRes;
    }
    //获取配置到map中,用tcp形式,需要server配合
    void getConfStation() {
        //获取配置文件各个行,用xsh返回太慢,改成tcp获取
        pstring strContentConf = getConfUseTcp();
        //        pstring strContentConf=plib::xsh(this->strip,"cat "+this->strPathFullConf);
        //        hlog(strContentConf.contain("\0"));
        //        hlog(strContentConf.size(),strContentConf.find("\0"));
        //按行分割,顺便去掉空行
        pliststring listConf = strContentConf.split("\n");

        //        hlog(listConf);
        //去掉带#的
        pliststring listConfNoComment;
        for (int i = 0; i < listConf.size(); i++) {
            if (!listConf[i].contain("#"))
                listConfNoComment.append(listConf[i]);
        }
        //        hlog(listConfNoComment);
        //然后可以逐条解析然后赋值给map了
        for (int i = 0; i < listConfNoComment.size(); i++) {
            pstring strConfi = listConfNoComment[i];//这就是一条配置A=B
            pliststring listConfi = strConfi.split("=");
            //            hlog(listConfi);
            //要加个判断,因为还有[basic]这样的就一个,要略过,不然崩溃
            if (listConfi.size() == 2)
            {
                //                mapconf[listConfi[0]] = listConfi[1];
                //                hlog(listConfi);
                //                mut.lock();
                mapconf.add(listConfi[0],listConfi[1]);
                //                mapconf[listConfi[0]] = listConfi[1];
                //                mut.unlock();
            }
        }
        //        hlog(mapconf);
        //        pstring strtest=plib::toString(mapconf);
        //        hlog(strtest,strtest.size(),strtest.find("\0"),string::npos);
        //        hlog(plib::toClass<pmap<pstring,pstring>>(strtest));
    }

    friend ostream &operator<<(ostream &os, CSGLStation x) {
        os << "strName: " << x.strName << " " << "strip: " << x.strip << " " << "mapconf: " << x.mapconf << " "
           << "strPathFullConf: " << x.strPathFullConf << " ";
        return os;
    }

    friend istream &operator>>(istream &is, CSGLStation &x) {
        plib::skipCountch(is, 9);
        is >> x.strName;
        is.get();
        plib::skipCountch(is, 7);
        is >> x.strip;
        is.get();
        plib::skipCountch(is, 9);
        is >> x.mapconf;
        is.get();
        plib::skipCountch(is, 17);
        is >> x.strPathFullConf;
        is.get();
        return is;
    }

private:

    //要有服务端,超时1秒
    pstring getConfUseTcp() {
        //获取配置文件
        TASK_HEAD head;
        head.type = HTYPEGETCONF;
        head.length = 0;
        if (tcpc->send(head) < 0) {
            //            hlog("send head fail");
            plib::sleep(1000);
            return "";
        }
        //等待接收配置文件
        //    先接包头
        TASK_HEAD pheadRecv;
        if (tcpc->recv((char *) &pheadRecv, sizeof(TASK_HEAD), 3) < 0) {
            hlog("recv head fail");
            //            plib::sleep(1000);
            return "";
        }
        //        hlog(pheadRecv.length);
        char *pdataRecv = HMALLOC(char, pheadRecv.length);
        if (tcpc->recv(pdataRecv, pheadRecv.length, 3) < 0) {
            //            hlog("recv data fail");
            return "";
        }
        return (pdataRecv);
    }
};

class CSGLSystem {
public:
    pstring strName;    //系统名称
    pmap<pstring, CSGLStation> mapStation;//各个站点
    CSGLSystem() {

    }

    CSGLSystem(pstring name, pmap<pstring, CSGLStation> mapStation) {
        this->strName = name;
        this->mapStation = mapStation;
    }

    CSGLStation getStationByName(pstring strNameStation) {
        return mapStation[strNameStation];
    }

    void connectAllStation()
    {
        for(int i=0;i<mapStation.size();i++)
        {
            mapStation.getValue(i).connect();
        }
    }
    //当前所有系统站获取配置函数
    void getConfSystem() {
        for (int i = 0; i < mapStation.size(); i++) {
            mapStation.getValue(i).getConfStation();
        }
    }

    pstring getStationNameByIndex(int index) {
        return mapStation.getKey(index);
    }

    CSGLStation& getStationByIndex(int index) {
        return mapStation.getValue(index);
    }

    size_t size() {
        return mapStation.size();
    }

    friend ostream &operator<<(ostream &os, CSGLSystem x) {
        os << "strName: " << x.strName << " " << "mapStation: " << x.mapStation << " ";
        return os;
    }

    friend istream &operator>>(istream &is, CSGLSystem &x) {
        plib::skipCountch(is, 9);
        is >> x.strName;
        is.get();
        plib::skipCountch(is, 12);
        is >> x.mapStation;
        is.get();
        return is;
    }

};

class CSGLAll {
public:
    pmap<pstring, CSGLSystem> mapSystem;//各个系统
    CSGLAll() {
        pmap<pstring, CSGLStation> mapStationGF;
        mapStationGF.add("DTC",CSGLStation("DTC", "172.16.11.3"));
        mapStationGF.add("KSC",CSGLStation("KSC", "172.16.141.5"));
        mapStationGF.add("MYC",CSGLStation("MYC", "172.16.77.6"));
        mapStationGF.add("SYC", CSGLStation("SYC", "172.16.171.5"));
        mapStationGF.add("KR1",CSGLStation("KR1", "130.242.60.241"));
        mapStationGF.add("YD1",CSGLStation("YD1", "172.16.112.2"));
        mapStationGF.add("YE2", CSGLStation("YE2", "172.17.2.2"));
        CSGLSystem systemGF("高分系统", mapStationGF);

        //        this->mapSystem.add("高分系统",systemGF);

        pmap<pstring, CSGLStation> mapStationKJ;
        mapStationKJ.add("DTC",CSGLStation("DTC", "172.16.14.2"));
        mapStationKJ.add("KSC",CSGLStation("KSC", "172.16.144.2"));
        mapStationKJ.add("MYC", CSGLStation("MYC", "172.16.81.2"));
        mapStationKJ.add("SYC", CSGLStation("SYC", "172.16.174.2"));
        mapStationKJ.add("KR1",CSGLStation("KR1", "130.242.60.243"));
        mapStationKJ.add("YD1", CSGLStation("YD1", "172.16.112.1"));
        mapStationKJ.add("YF3",CSGLStation("YF3", "172.16.120.2"));
        CSGLSystem systemKJ("空基系统", mapStationKJ);

        this->mapSystem.add("空基系统",systemKJ);

    }

    CSGLStation getStation(pstring strNameSystem, pstring strNameStation) {
        return mapSystem[strNameSystem].getStationByName(strNameStation);
    }

    CSGLStation& getStation(pstring strIpStation) {
        for (int i = 0; i < mapSystem.size(); i++) {
            CSGLSystem& systemi = this->getSystemByIndex(i);
            for (int j = 0; j < systemi.size(); j++) {
                CSGLStation& stationj = systemi.getStationByIndex(j);
                if (stationj.strip == strIpStation)
                    return stationj;
            }
        }
    }

    CSGLSystem getSystemByName(pstring strNameSystem) {
        return mapSystem[strNameSystem];
    }

    pstring getSystemNameByIndex(int index) {
        return mapSystem.getKey(index);
    }

    CSGLSystem& getSystemByIndex(int index) {
        return mapSystem.getValue(index);
    }

    size_t size() {
        return mapSystem.size();
    }
    void connectAll()
    {
        for(int i=0;i<mapSystem.size();i++)
        {
            mapSystem.getValue(i).connectAllStation();
        }

    }
    void getConfAll() {
        for (int i = 0; i < mapSystem.size(); i++)
        {
            mapSystem.getValue(i).getConfSystem();
        }
    }

    friend ostream &operator<<(ostream &os, CSGLAll x) {
        os << "mapSystem: " << x.mapSystem << " ";
        return os;
    }

    friend istream &operator>>(istream &is, CSGLAll &x) {
        plib::skipCountch(is, 11);
        is >> x.mapSystem;
        is.get();
        return is;
    }

};

#endif // PLIB_H
