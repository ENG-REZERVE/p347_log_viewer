#ifndef BROADCAST_CLIENT_HPP
#define BROADCAST_CLIENT_HPP

#define DEFAULT_LOGCAST_PORT        10225

#ifdef _LINUX_
#include <arpa/inet.h>
#else
#include <ws2tcpip.h>
//#include <winsock2.h>
#endif

#include "os_define.h"

#include "thread.hpp"
#include "uki_log.hpp"

#include <boost/signals2.hpp>
#include <boost/bind.hpp>

class p347_BroadcastClient : public TThread, public UKI_LOG {
public:
    p347_BroadcastClient(void);
    ~p347_BroadcastClient(void);
    boost::signals2::signal<void (int data_len, void* data)> sig_data;
    int setPort(int port_number);
    void closePort(void);
protected:
    virtual void Execute(void);
private:
    TEvent*		ev_end;
    TCriticalSection* lock;
    sockaddr_in addr,saddr;
    SOCKET      sock;
};

#endif // BROADCAST_CLIENT_HPP
