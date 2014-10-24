#ifndef _p347_IPC_COMMON_H_
#define _p347_IPC_COMMON_H_

#define PB_PRESSED		0x01
#define PB_RELEASED		0x00

#define SUB_MASK_BUTTON_VIEW		0x01
#define SUB_MASK_BUTTON_ACTION		0x02
#define SUB_MASK_BATTERY_STATE		0x04
#define SUB_MASK_CPU_TEMPERATURE	0x08
#define SUB_MASK_ANY (SUB_MASK_BUTTON_VIEW | SUB_MASK_BUTTON_ACTION | SUB_MASK_BATTERY_STATE | SUB_MASK_CPU_TEMPERATURE)

enum p347_msg_type {
    //from hwmon clients to server
    p347_MSGTYPE_HWMON_UNSUBSCRIBE,
    p347_MSGTYPE_HWMON_SUBSCRIBE,
    //from server to hwmon clients
    p347_MSGTYPE_DATA_POWERBUTTON,
    p347_MSGTYPE_DATA_BATTERY,
    p347_MSGTYPE_DATA_CPUTEMP,
    //from log clients to server
    p347_MSGTYPE_LOG_UNSUBSCRIBE,
    p347_MSGTYPE_LOG_SUBSCRIBE,
    p347_MSGTYPE_LOG_FROM_SOMEWHERE,
    //from server to log clients
    p347_MSGTYPE_LOG_TO_SUBSCRIBER,
};

typedef struct {
    p347_msg_type					type;
    int								q_id;
    unsigned short					s_mask;
}t_hwmon_client_subscribe_request;

typedef struct {
    p347_msg_type					type;
    int								q_id;
    int								prefix_cnt;
}t_log_client_subscribe_request_head;

typedef struct {
    p347_msg_type					type;
    int								q_id;
}t_unsubscribe;

typedef struct {
    p347_msg_type					type;
    int								q_id;
    unsigned short					s_mask;
}t_msg_from_hwmon_client;

#endif
