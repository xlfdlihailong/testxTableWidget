#include "qlib.h"
#include "plib/ptrans.h"

void show(pstring str)
{
    hlog(str);
}


std::string getCMD(char * cmd){
    FILE *file;
    char ptr[1024] = { 0 };
    char tmp[1024] = { 0 };
    strcat_s(ptr, cmd);
    string result = "";
    if ((file = _popen(ptr, "r")) != NULL)
    {
        while (fgets(tmp, 1024, file) != NULL){
            result = result + tmp;
        }
        _pclose(file);
    }
    return result;
}


int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    qlib::initLog("xlfd.log");

//    hlog(plib::pwd());
//    hlog(plib::xshFile("106.12.222.93","ps -ef|grep ssh"));
//    hlog(plib::xshFile("106.12.222.93","df -h"));

//    hlog(plib::xsh("106.12.222.93","ps -ef|grep ssh"));
//    hlog(plib::xsh("106.12.222.93","df -h"));
//    std::thread([]{
//        hlog(plib::xsh("106.12.222.93","ls -l /root/Desktop |grep -v total| awk '{print $5,$9,$1}'"));
//    }).detach();
      pstring stres=(plib::xsh("106.12.222.93","ls /root/Desktop"));
      pliststring lres=stres.split("\n");
      for(int i=0;i<lres.size();i++)
      {
          hlog(lres[i],plib::toChinese(lres[i]),plib::isUTF8(lres[i].c_str()),plib::toGBK(lres[i].c_str()));

      }

//    plib::sleep(2000);
    //        hlog(plib::xsh("192.168.72.216","ifconfig"));
//            hlog(plib::xsh("192.168.72.216","ls -al"));
    //    hlog(plib::xsh("172.16.11.6","xsh scfwq_325 130.242.60.241 'ifconfig|grep inet'","scfwq_325"));

//            hlog(plib::getCmdOutputFun("qxshnofile.exe 106.12.222.93 \"df -h\" npants0703*** root 22",show));
//    hlog(plib::getCmdOutput("qxshnofile.exe 106.12.222.93 pwd npants0703*** root 22"));


//            hlog(getCMD("qxshnofile.exe 106.12.222.93 \"df -h\" npants0703*** root 22"));
//    xprocess proc("qxshnofile.exe",QStringList()<<"106.12.222.93"<<"ifconfig"<<"npants0703***"<<"root"<<"22");
//    xprocess proc("C:\\Users\\Administrator\\Desktop\\xlib\\windows\\projects\\qtest\\debug\\qtest.exe");

//    proc.open();

    return a.exec();
}
