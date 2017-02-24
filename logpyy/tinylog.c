/*
 * tinylog.c capture the logcat logs.
 *                                                                                                                                                                                                                  
 * Copyright (C) 2016 Sl0v3C.
 *
 * Author: Sl0v3C <pyy101727@gmail.com>
 *
 * License terms:
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#include "tinylog.h"

pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pcond = PTHREAD_COND_INITIALIZER;

static int tinylog_tid = -1;
static int tinylog_cmd = -1;
static int pid = -1;
static pthread_t tinylog_thread;
FILE *ppin = NULL;
FILE *fpout = NULL;

void* tinylog_thread_loop(void *ptr)
{
    tinylog_tid = gettid();
    ALOGD("tinylog_tid:  %d pid %d", tinylog_tid, *(int *)ptr);
    prctl(PR_SET_NAME, (unsigned long)"TinylogcatThrd", 0, 0, 0);
    if (signal(SIGSEGV, tinylog_exception_handler) == SIG_ERR) {
        ALOGE("Can't set tinylog exception callback handler to solve SIGNAL 11");
    }
    while(1) {
        pthread_mutex_lock(&pmutex);
	if (TINYLOG_SLEEP == tinylog_cmd) {
            ALOGI("tinylog_thread_loop SLEEPING");
            pthread_cond_wait(&pcond, &pmutex);
            ALOGI("tinylog_thread_loop RUNNING");
            pthread_mutex_unlock(&pmutex); 
            continue;
        }

        switch(tinylog_cmd) {
        case TINYLOG_START:
            fpout = fopen("/data/tinylogcat.txt", "w");
	    ppin = popen("logcat -v threadtime", "r");
            ALOGI(" TINYLOG_START");
            char buf[1024];
            if (NULL == ppin || NULL == fpout) {	    
		ALOGE("popen/file open failed error: %s", strerror(errno));
                tinylog_cmd = TINYLOG_SLEEP;
                pthread_mutex_unlock(&pmutex);
                continue;
            }
	    while (tinylog_cmd == TINYLOG_START && fgets(buf, sizeof(buf) - 1, ppin) != NULL) {
		fputs(buf, fpout);
            }
            ALOGD("TINYLOG START done!");
	    if (ppin) {
		pclose(ppin);
		ppin = NULL;
		ALOGI("Stop the tinylogcat!!!!");
	    }
	    if (fpout) {
		fclose(fpout);
		fpout = NULL;
		ALOGI("Close the file data/tinylogcat.txt");
	    }
            break;

        case TINYLOG_STOP:
            ALOGI(" TINYLOG_STOP");
            tinylog_cmd = TINYLOG_SLEEP;
            break;
        case TINYLOG_EXIT:
            ALOGI(" TINYLOG_EXIT");
            tinylog_cmd = TINYLOG_SLEEP;
            return NULL;
        default:
            tinylog_cmd = TINYLOG_SLEEP;
	    break;
        }
        pthread_mutex_unlock(&pmutex); 
   }
    
   return NULL; 
}

void tinylog_exception_handler(int signo)
{
    ALOGD("Handler signo %d Signal 11 for tinylog_exception_callback", signo);

    char name[20];
    sprintf(name, "kill -9 %d", tinylog_tid);
    ALOGD("%s", name);
    system(name);
}

void stopPthread(void)
{
    tinylog_cmd = TINYLOG_STOP;
}

void startTinylog(void)
{
    tinylog_cmd = TINYLOG_START;
    pthread_cond_signal(&pcond);
}

void exitTinylog(void)
{
    tinylog_cmd = TINYLOG_EXIT;
    pthread_cond_signal(&pcond);
}

int tinylog_thread_init(void)
{
    pid = getpid();
    ALOGI("PID %d", pid);
    pthread_mutex_init(&pmutex, (const pthread_mutexattr_t *) NULL);
    pthread_cond_init(&pcond, (const pthread_condattr_t *) NULL);
    pthread_create(&tinylog_thread, NULL, tinylog_thread_loop, (void *)&pid);
    ALOGI("create tinylog thread");

    return 0;
}

