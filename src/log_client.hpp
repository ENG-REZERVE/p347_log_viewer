#ifndef _P347_LOG_CLIENT_H_
#define _P347_LOG_CLIENT_H_

#ifdef _LINUX_
#include "ipc_client.hpp"
#endif

#include "broadcast_client.hpp"

#include <vector>
#include <string>

#ifdef _LINUX_
class p347_LogClient : public p347_IPCClient {
#else
class p347_LogClient : public TThread, public UKI_LOG {
#endif
public:
    p347_LogClient(void);
    ~p347_LogClient(void);
    //----------------------------------------------------------
    //virtual void callbackNewLocalMessage(t_log_message* msg);
    virtual void callbackNewLocalMessage(std::string msg_time,
                                         std::string msg_prefix,
                                         std::string msg_message);
    virtual void callbackNewNetworkMessage(int data_len, void *data);
    int subscribeToMessages(std::vector<std::string>* prefixes);
    int unsubscribe(void);
    p347_BroadcastClient* broad_client;
protected:
    void Execute(void);
private:
#ifndef _LINUX_
    TEvent*		ev_end;
#endif
    void onIncomingNetworkMessage(int data_len, void *data);
    boost::signals2::connection     c_data;
};

#endif
