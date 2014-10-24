
#include "log_client.hpp"
#include "p347_ipc_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

//-----------------------------------Constructor and Destructor

#ifdef _LINUX_
p347_LogClient::p347_LogClient(void) : p347_IPCClient()
#else
p347_LogClient::p347_LogClient(void) : TThread(true), UKI_LOG()
#endif
{
#ifndef _LINUX_
    ev_end = new TEvent(0,true,true,"",false);
    assert(ev_end != NULL);
#endif
    setPrefix("LogClient");

    broad_client = new p347_BroadcastClient();
    assert(broad_client != NULL);
}

p347_LogClient::~p347_LogClient()
{
    c_data.disconnect();

    delete broad_client;
    broad_client = NULL;
#ifndef _LINUX_
    if (!Terminated) {
        Terminate();
    }
    ev_end->WaitFor(INFINITE);
    delete ev_end;
#endif
}    

void p347_LogClient::callbackNewLocalMessage(std::string msg_time,
                                     std::string msg_prefix,
                                     std::string msg_message) {
    logMessage(LOG_LEVEL_MAIN,"LocalDevice ||| %s %s: %s",msg_time.c_str(),msg_prefix.c_str(),msg_message.c_str());

}

/*
void p347_LogClient::callbackNewNetworkMessage(std::string device_name,
                                       std::string msg_time,
                                       std::string msg_prefix,
                                       std::string msg_message) {
    logMessage(LOG_LEVEL_MAIN,"%s ||| %s %s: %s",device_name.c_str(),msg_time.c_str(),msg_prefix.c_str(),msg_message.c_str());
}
*/

void p347_LogClient::callbackNewNetworkMessage(int data_len, void *data) {
    logMessage(LOG_LEVEL_FULL,"callbackNewNetworkMessage: %d bytes from %p\n",data_len,data);
}

void p347_LogClient::onIncomingNetworkMessage(int data_len, void *data) {
    if ((data_len <= 0) || (data == NULL)) {
        logMessage(LOG_LEVEL_CRITICAL,"void data callback from network\n");
    } else {
        callbackNewNetworkMessage(data_len,data);
    }
}

//---------------------------------------------------------------------------------
int p347_LogClient::subscribeToMessages(std::vector<std::string>* prefixes) {
	//if (prefixes == NULL) return -EINVAL;
	//if (prefixes->size() <= 0) return -EINVAL;

#ifdef _LINUX_
	t_log_client_subscribe_request_head head;
	head.q_id = own_queue_id;
	head.type = p347_MSGTYPE_LOG_SUBSCRIBE;
	if (prefixes == NULL)
		head.prefix_cnt = 0;
	else
		head.prefix_cnt = prefixes->size();

	int s_ptr = sizeof(t_log_client_subscribe_request_head);
	int send_len = s_ptr + head.prefix_cnt*LOG_PREFIX_MAX_LEN;

	char* sendbuff = (char*)malloc(send_len);
	assert(sendbuff != NULL);
	memset(&sendbuff[0],0,send_len);
	memcpy(&sendbuff[0],&head,sizeof(t_log_client_subscribe_request_head));

	for (int i=0; i<head.prefix_cnt; i++) {
		strncpy(&sendbuff[s_ptr],prefixes->at(i).c_str(),LOG_PREFIX_MAX_LEN);
		s_ptr += LOG_PREFIX_MAX_LEN;
	}

	int ret = sendMessageToIPCServer(&sendbuff[0],send_len);

	free(sendbuff);
#else
    int ret = 0;
#endif
    //TODO: subscribe to broadcast

	return ret;
}


int p347_LogClient::unsubscribe(void) {
#ifdef _LINUX_
    t_unsubscribe	tuns;

    tuns.q_id = own_queue_id;
    tuns.type = p347_MSGTYPE_LOG_UNSUBSCRIBE;

	int ret = sendMessageToIPCServer(&tuns,sizeof(t_unsubscribe));
#else
    int ret = 0;
#endif
    return ret;
}

//------------------------------------------------------------------Execute
void p347_LogClient::Execute(void) {
#ifdef _LINUX_
    int i;
    int ret;
    int len_supposed;
    int rsize;
    t_p347_ipc_msg msg;
#endif
    ev_end->ResetEvent();

    c_data = broad_client->sig_data.connect(boost::bind(&p347_LogClient::onIncomingNetworkMessage, this, _1, _2));
    broad_client->Resume();

    while (!Terminated) {
#ifdef _LINUX_
		if (own_queue_id > 0) {
			ret = waitMessageInfinite(&msg);
			if (ret > 0) {
				p347_msg_type* mt = (p347_msg_type*)&msg.mtext[0];
				switch (*mt) {
					case p347_MSGTYPE_LOG_TO_SUBSCRIBER: {
						if (ret != (LOG_MESSAGE_TOTAL_SIZE + sizeof(p347_msg_type))) {
							logMessage(LOG_LEVEL_CRITICAL,"Invalid log message len: %d instead of %d\n",ret,LOG_MESSAGE_TOTAL_SIZE);
							callbackNewMessage(NULL); //signalize invalid
						} else {
							t_log_message* new_mess = (t_log_message*)&msg.mtext[sizeof(p347_msg_type)];
							//printf("before callback: %s %s %s\n",&new_mess->time[0],&new_mess->prefix[0],&new_mess->message[0]);
							callbackNewMessage(new_mess);
						}
					break; }
					default: {
						logMessage(LOG_LEVEL_CRITICAL,"unknown message type=%d\n",*mt);
					break; }
				};
			} else {
				ret = -errno;
				logMessage(LOG_LEVEL_CRITICAL,"msgrcv failed with ret %d\n",ret);
			}
		} else {
			sleep(1);
		}
#else
        Sleep(1000);
#endif
    } //end of while
    
    ev_end->SetEvent();
    logMessage(LOG_LEVEL_MAIN,"LogClient Terminated\n");
}
