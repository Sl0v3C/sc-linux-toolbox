/*
 * logpyy.c capture the logcat logs.
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
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdarg.h>


void usage(void)
{
    ALOGI("===========================================================\n");
    ALOGI("Usage: input args:\n");
    ALOGI(" logstart: to logcat to file /data/tinylogcat.txt\n");
    ALOGI(" logstop: to stop logcat\n");
    ALOGI(" logexit: to exit logcat thread\n");
    ALOGI(" exit to quit the binary\n");
    ALOGI("===========================================================\n");
    ALOGI("Please input arg:\n");
}

int main(void)
{
    int ret = -1;
    char buf[128];

    while (1) {
        usage();
        scanf("%s", buf);
        if (strncmp(buf, "logstart", 8) == 0) {
            ALOGI("TINYLOG start\n");
	    ret = tinylog_thread_init();
	    startTinylog();
        } else if (strncmp(buf, "logstop", 7) == 0 && 0 == ret) {
            ALOGI("TINYLOG stop\n");
            stopPthread();
        } else if (strncmp(buf, "logexit", 7) == 0 && 0 == ret) {
            ALOGI("TINYLOG exit\n");
	    exitTinylog();
        } else if (strncmp(buf, "exit", 4) == 0){
            ALOGI("Quiting the binary\n");
            return 0;
        } else {
            continue;
        }
    }

    return 0;
}

