#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
//This file contains the system call numbers
#include <unistd.h>
#include <cutils/log.h>
#include <sys/prctl.h>

#define TINYLOG_SLEEP -1
#define TINYLOG_EXIT -101
#define TINYLOG_START 1
#define TINYLOG_STOP 2

#undef LOG_TAG
#define LOG_TAG "[tinylogcat]"

void* tinylog_thread_loop(void* ptr);
void tinylog_exception_handler(int signo);
void stopPthread(void);
void startTinylog(void);
void exitTinylog(void);
int tinylog_thread_init(void);

