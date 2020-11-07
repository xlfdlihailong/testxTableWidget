#include "pudp.h"
void thread_test()
{
    pudp udp;
    udp.init(3333);
//    hlog(udp.bind());
    hlog(udp.bind("224.0.0.3"));
    while(1)
    {
        hlog(udp.recvx());
//        if(udp.recvx()<0)
//           break;
//        hlog(udp.type);
        udp.setHostDES("224.0.0.4");
//        udp.setHostDES(plib::getIPLocal());
        hlog(udp.sendx("xlfd",55.555));
    }
    hlog("finish");
}
void thread_test2()
{
    pudp udptest;
    udptest.init(3333);
    hlog(udptest.bind("224.0.0.3"));
    while(1)
    {
        hlog(udptest.recvx());
//        hlog(udptest.type);
//        hlog(udptest.getClass<double>());
    }
}

int main(int argc, char *argv[]) {


    std::thread(thread_test).detach();
    std::thread(thread_test2).detach();

    plib::sleep(1000);
    plib::getIPLocal();
    pudp udp;
    udp.init(3333);

    hlog(udp.bind("224.0.0.4"));
//    hlog(udp.bind());
    udp.setHostDES("224.0.0.3");
//    udp.setHostDES(plib::getIPLocal());
    hlog(udp.sendx("test",("zzzzz")));

    while(1)
    {
        hlog(udp.recvx());
        plib::sleep(1000);
    }
    return 0;
}

