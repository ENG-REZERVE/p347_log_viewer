#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>

#include "uki_log.hpp"

#include <iostream>

#include <sys/time.h>
#ifdef _LINUX_
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#endif
#include "p347_ipc_global.h"
#include "p347_ipc_common.h"

UKI_LOG::UKI_LOG(void) {
    wfile = NULL;
    
    //default levels
    level_file = LOG_LEVEL_NONE;
    level_daemon = LOG_LEVEL_NONE;
    level_console = LOG_LEVEL_CRITICAL;
#ifdef _LINUX_
    server_key = -1;
    server_queue_id = -1;
#endif
    //default prefix
    memset(&prefix[0],0,LOG_PREFIX_MAX_LEN);
    sprintf(&prefix[0],"Noname");

    log_lock = new TCriticalSection();
    assert(log_lock != NULL);
};

UKI_LOG::~UKI_LOG(void) {
    setConsoleLevel(LOG_LEVEL_NONE);    
    setFileLevel(LOG_LEVEL_NONE,0,NULL);
};

void UKI_LOG::setFileLevel(unsigned char lvl, bool append, const char* log_file_name) {
    char fname[255];
    struct timeval tv;
    struct tm* ptm;
    char time_string[40];
    bool generate_name = false;

    log_lock->Enter();
    
    if (lvl != LOG_LEVEL_NONE) { 
	//open new if was closed before
	if (wfile == NULL) {
		if (log_file_name == NULL) {
			generate_name = true;
		} else if (strlen(log_file_name) < 2) {
			generate_name = true;
		}
		if (generate_name) { //Use generated name
			gettimeofday(&tv,NULL);
			ptm = localtime(&tv.tv_sec);
			memset(&time_string[0],0,40);
			strftime(time_string,sizeof(time_string),"%Y%m%d_%H%M",ptm);
			    
			memset(&fname[0],0,255);
			sprintf(&fname[0],"%s__%s.log",&prefix[0],time_string);
		} else { //Copy passed name
			strcpy(&fname[0],log_file_name);
		}

		if (append)
			wfile = fopen(&fname[0],"a");
		else
			wfile = fopen(&fname[0],"w");

	    if (wfile == NULL) {
	    	selfDebug(LOG_LEVEL_CRITICAL,"Log cannot open log file %s ! err=%d\n",fname,errno);
	    }
	}
    } else { 
    	//close if was opened
    	if (wfile != NULL) {
    		fflush(wfile);
#ifdef _LINUX_
    		fsync(fileno(wfile));
#endif
    		fclose(wfile);
    		wfile = NULL;
    	}
    }
    
    level_file = lvl;
    
    log_lock->Leave();
};

void UKI_LOG::setConsoleLevel(unsigned char lvl) {
    log_lock->Enter();
    if (lvl > LOG_LEVEL_FULL)
    	level_console = LOG_LEVEL_FULL;
    else
    	level_console = lvl;
	log_lock->Leave();
    
    fflush(stdout);
};

#ifdef _LINUX_
int UKI_LOG::setDaemonLevel(unsigned char lvl, const char* keyfile_path, int custom_idx) {
    int ret = 0;

    if ((lvl > LOG_LEVEL_NONE) && (server_queue_id < 0))
		ret = initDaemonLogConnection(keyfile_path, custom_idx);

	if (ret == 0) {
		log_lock->Enter();
		if (lvl > LOG_LEVEL_FULL)
			level_daemon = LOG_LEVEL_FULL;
		else
			level_daemon = lvl;
		log_lock->Leave();
	} else {
		selfDebug(LOG_LEVEL_CRITICAL,"initDaemonLogConnection failed with %d!\n",ret);
	}
    return 0;
};

int UKI_LOG::initDaemonLogConnection(const char* keyfile_path, int custom_idx) {
	if (keyfile_path == NULL) return -EINVAL;
    int ret = 0;
    selfDebug(LOG_LEVEL_FULL,"server keyfile_path=%s, idx=%d\n",keyfile_path,custom_idx);

    server_key = ftok(keyfile_path,custom_idx);
    if (server_key < 0) {
    	ret = -errno;
    	selfDebug(LOG_LEVEL_CRITICAL,"ftok failed with %d\n",ret);
    } else {
    	selfDebug(LOG_LEVEL_FULL,"server_key ret %d\n",server_key);
    	log_lock->Enter();
    	server_queue_id = msgget(server_key, 0666);
        if (server_queue_id < 0) {
    	    ret = -errno;
    	    selfDebug(LOG_LEVEL_CRITICAL,"msgget failed with %d\n",ret);
        } else {
    	    msqid_ds statbuf;
    	    selfDebug(LOG_LEVEL_FULL,"user attached to server queue id=%d\n",server_queue_id);
    	    ret = msgctl(server_queue_id,IPC_STAT,&statbuf);
    	    if (ret == 0) {
    	    	selfDebug(LOG_LEVEL_FULL,"server key=%d, ownuid=%d, owngid=%d, cruid=%d, crgid=%d, mode=%d, seq=%d\n",
    		    statbuf.msg_perm.__key,statbuf.msg_perm.uid,statbuf.msg_perm.gid,
    					 statbuf.msg_perm.cuid,statbuf.msg_perm.cgid,
    					 statbuf.msg_perm.mode,statbuf.msg_perm.__seq);
    	    } else {
    	    	ret = -errno;
    	    	selfDebug(LOG_LEVEL_CRITICAL,"msgctl failed with %d\n",ret);
    	    }
        }
        log_lock->Leave();
    }

    return ret;
}

#endif

int UKI_LOG::setPrefix(const char* new_prefix) {
    size_t slen;

    if (new_prefix == NULL) return -EINVAL;
    slen = strlen(new_prefix);
    //printf("slen = %d\n",slen);
    if (slen > (LOG_PREFIX_MAX_LEN-2))
    	slen = LOG_PREFIX_MAX_LEN - 2;

    log_lock->Enter();
    memset(&prefix[0],0,LOG_PREFIX_MAX_LEN);
    snprintf(&prefix[0],slen+1,"%s",&new_prefix[0]);
    log_lock->Leave();

    return 0;
}; 

void UKI_LOG::selfDebug(unsigned char level, const char* message, ...) {
	if ((level <= level_console) && (message != NULL)) {
		va_list p;
		struct timeval tv;
		struct tm* ptm;
		char time_string[LOG_TIME_STRING_MAX_LEN];
		long milliseconds;

		gettimeofday(&tv,NULL);
		ptm = localtime(&tv.tv_sec);
		strftime(time_string,sizeof(time_string),"%Y-%m-%d %H:%M",ptm);
		milliseconds = tv.tv_usec / 1000;

		va_start(p,message);
		printf("%s self-debug: %s.%03ld ",&prefix[0],time_string,milliseconds);
		vprintf(message,p);
		va_end(p);
	}
}

void UKI_LOG::logMessage(unsigned char level, const char* message, ...) {
    va_list p;
        
    struct timeval tv;
    struct tm* ptm;
    char time_string[LOG_TIME_STRING_MAX_LEN];
    long milliseconds;

    //printf("message %s, level %d, level_daemon %d\n",message,level,level_daemon);

    if ((level == LOG_LEVEL_NONE) || (level > LOG_LEVEL_FULL)) return; //no logging here
    if ((level_console == LOG_LEVEL_NONE) && (level_file == LOG_LEVEL_NONE) && (level_daemon == LOG_LEVEL_NONE)) return;
    
    log_lock->Enter();                                
    if ((level <= level_console) || (level <= level_file) || (level <= level_daemon)) { //get time
    	gettimeofday(&tv,NULL);
        ptm = localtime(&tv.tv_sec);
        strftime(time_string,sizeof(time_string),"%Y-%m-%d %H:%M",ptm);
        milliseconds = tv.tv_usec / 1000;
                                        			
        va_start(p,message);
        if (level <= level_console) {
            char total_msg[1024];
            memset(&total_msg[0],0,1024);
            sprintf(&total_msg[0],"%s: %s.%03ld ",&prefix[0],time_string,milliseconds);
            //printf("%s: %s.%03ld ",&prefix[0],time_string,milliseconds);
            //std::cerr << "%s: %s.%03ld ",&prefix[0],time_string,milliseconds;
            //vprintf(message,p);
            vsprintf(&total_msg[strlen(total_msg)],message,p);

            std::cerr << total_msg;
            //std::cout << total_msg;
        }

        if (level <= level_file) {
    	    fprintf(wfile,"%s: %s.%03ld ",&prefix[0],time_string,milliseconds);
            vfprintf(wfile,message,p);
        }
#ifdef _LINUX_
        if ((server_queue_id >= 0) && (level <= level_daemon)) {
        	t_p347_ipc_msg msg;
        	msg.mtype = p347_DEFAULT_MSG_TYPE;
        	memset(&msg.mtext[0],0,p347_MAX_IPC_MSG_LEN);

        	p347_msg_type* mt = (p347_msg_type*)&msg.mtext[0];
        	*mt = p347_MSGTYPE_LOG_FROM_SOMEWHERE;

        	t_log_message* tlm = (t_log_message*)&msg.mtext[sizeof(p347_msg_type)];
        	snprintf(&(tlm->prefix[0]),LOG_PREFIX_MAX_LEN,&prefix[0]);
        	snprintf(&(tlm->time[0]),LOG_TIME_STRING_MAX_LEN,&time_string[0]);
            vsnprintf(&(tlm->message[0]),LOG_MSG_STRING_MAX_LEN,&message[0],p);

        	int ret = msgsnd(server_queue_id,&msg,LOG_MESSAGE_TOTAL_SIZE + sizeof(p347_msg_type),0);
        	if (ret < 0) {
        		ret = -errno;
        		printf("%s self-debug: %s.%03ld Send message to log daemon failed with %d\n",
        				&prefix[0],time_string,milliseconds,ret);
        	} else {
        		//printf("UKI_LOG msgsnd ret %d\n",ret);
        	}
        }
#endif
        va_end(p);
    }
    log_lock->Leave();
}
