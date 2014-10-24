#ifndef _UKI_LOG_H_
#define _UKI_LOG_H_

#include "os_define.h"

#define LOG_LEVEL_NONE			0
#define LOG_LEVEL_CRITICAL		1
#define LOG_LEVEL_MAIN			2
#define LOG_LEVEL_FULL			3

#define LOG_PREFIX_MAX_LEN		12
#define LOG_TIME_STRING_MAX_LEN 30
#define LOG_MSG_STRING_MAX_LEN	255
#define LOG_MESSAGE_TOTAL_SIZE	(LOG_PREFIX_MAX_LEN + LOG_TIME_STRING_MAX_LEN + LOG_MSG_STRING_MAX_LEN)

#include <stdio.h>
//#include <sys/types.h>
#include "syncobjs.hpp"

#define INTERNAL_MSG_DELIMITER      ":::"

typedef struct {
	char			time[LOG_TIME_STRING_MAX_LEN];
	char			prefix[LOG_PREFIX_MAX_LEN];
	char			message[LOG_MSG_STRING_MAX_LEN];
}t_log_message;

class UKI_LOG {
public:
    UKI_LOG(void);
    ~UKI_LOG(void);
    void setFileLevel(unsigned char lvl, bool append, const char* log_file_name);
    void setConsoleLevel(unsigned char lvl);
#ifdef _LINUX_
    int setDaemonLevel(unsigned char lvl, const char* keyfile_path, int custom_idx);
#endif
    int setPrefix(const char* new_prefix);
    void logMessage(unsigned char level, const char* message, ...);
protected:
    //--------------------------------
    unsigned char	level_file;
    unsigned char	level_console;
    unsigned char	level_daemon;
    char			prefix[LOG_PREFIX_MAX_LEN];
    //--------------------------------
    FILE			*wfile;
#ifdef _LINUX_
    key_t			server_key;
    int				server_queue_id;
    int initDaemonLogConnection(const char* keyfile_path, int custom_idx);
#endif
    TCriticalSection	*log_lock;   
private:
    void selfDebug(unsigned char level, const char* message, ...);
};

#endif
