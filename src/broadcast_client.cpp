#include "broadcast_client.hpp"
#define SOCKET_BUFF_MAX_LEN     1024

p347_BroadcastClient::p347_BroadcastClient(void) : TThread(true), UKI_LOG(){
    ev_end = new TEvent(0,true,true,"",false);
    assert(ev_end != NULL);

    ev_end->SetEvent();
    sock = INVALID_SOCKET;

    lock = new TCriticalSection();
    assert(lock != NULL);

    setPrefix("BRCClient");
    setConsoleLevel(LOG_LEVEL_MAIN);
}

p347_BroadcastClient::~p347_BroadcastClient(void) {
    closePort();

    if (!Terminated) {
        Terminate();
    }
    ev_end->WaitFor(INFINITE);
    delete ev_end;

    lock->Enter(); lock->Leave();
    delete lock;
}

int p347_BroadcastClient::setPort(int port_number) {
    int ret = 0;
    memset(&addr, 0x00, sizeof(sockaddr_in));
    memset(&saddr, 0x00, sizeof(sockaddr_in));

#ifndef _LINUX_
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0) {
        logMessage(LOG_LEVEL_CRITICAL,"WSAStartup failed with %d\n",ret);
        return ret;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        logMessage(LOG_LEVEL_CRITICAL,"Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return -2;
    }

    logMessage(LOG_LEVEL_MAIN,"The Winsock 2.2 dll was found okay\n");
#endif

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //oleg proto=0
    if (sock == INVALID_SOCKET) {
#ifdef _LINUX_
        ret = -errno;
#else
        ret = WSAGetLastError();
#endif
        logMessage(LOG_LEVEL_CRITICAL,"socket creation failed with %d\n",ret);
        WSACleanup();
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_number);
        ret = bind(sock,(const sockaddr *)&addr, sizeof(sockaddr_in));
        if (ret < 0) {
            ret = -errno;
            logMessage(LOG_LEVEL_CRITICAL,"socket bind ret %d\n",ret);
        }
    }

    return ret;
}

void p347_BroadcastClient::closePort(void) {
    if (sock != INVALID_SOCKET) {
        sendto(sock,NULL,0,0,(const sockaddr *)&addr,sizeof(sockaddr_in));
#ifdef _LINUX_
        close(sock);
#else
        closesocket(sock);
        WSACleanup();
#endif
        sock = INVALID_SOCKET;
    }
}

//------------------------------------------------------------------------

void p347_BroadcastClient::Execute(void) {
    int ret = 0;
    char buff[SOCKET_BUFF_MAX_LEN];
    ev_end->ResetEvent();

    socklen_t slen = sizeof(saddr);
    //int slen = sizeof(saddr);
    while (!Terminated) {
        if (sock != INVALID_SOCKET) {
            ret = recvfrom(sock,&buff[0],SOCKET_BUFF_MAX_LEN,0,(sockaddr*)&saddr,&slen);
            logMessage(LOG_LEVEL_MAIN,"Broadcast read ret %d\n",ret);
        } else {
            ret = -1;
            //logMessage(LOG_LEVEL_MAIN,"Invalid socket\n");
        }

        if (Terminated) break;

        if (ret > 0) {
            //lock->Enter();
            sig_data(ret,&buff[0]);
            //lock->Leave();
        } else {
#ifdef _LINUX_
            usleep(1000);
#else
            Sleep(100);
#endif
        }
    }

    ev_end->SetEvent();
    logMessage(LOG_LEVEL_MAIN,"Broadcast Client terminated\n");
}
