#ifndef PTRANS_H
#define PTRANS_H
#include "plib.h"
class ptrans
{
public:
    pstring strhost;
    int iport;
    ptrans(pstring strhost="9g0qenno86.54http.tech",int iport=59856);
    int sendFile(pstring strPathFull)
    {
        ptcp tc(strhost,iport);
        hlog(tc.connectx());
        hlog(tc.sendx("test","xxxxxx"));
        return 0;
    }
};

#endif // PTRANS_H
