#ifndef _p347_IPC_GLOBAL_H_
#define _p347_IPC_GLOBAL_H_

#define p347_DEFAULT_MSG_TYPE			2
#define p347_MAX_IPC_MSG_LEN			512

typedef struct {
    long 					mtype;
    char					mtext[p347_MAX_IPC_MSG_LEN];
}t_p347_ipc_msg;

#endif
