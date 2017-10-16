/*
 * yyaudioplayer play music files by using MediaPlayer in Android.
 *                                                                                                                                                                                                                  
 * Copyright (C) 2016 Sl0v3C.
 *
 * Author: Sl0v3C <pyy101727@gmail.com>
 *
 * Using libmedia of Andriod
 *
 * License terms:
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#define LOG_TAG "YYAudioPlayer"

#include "media_player.h"
#include <cutils/compiler.h>

using namespace android;

#define MAX 257

static int line;

void typePrint(char *buf, int n)
{
    int a = 0, b = 0;
    for (; n < 256; n++){
        buf[n] = '\0';
    }
    while(a < 256){
        printf("%06X  ", line);
        while(b < 16) {
            for (int i = 0; i < 4; i++) {
                printf("%02X", buf[i + b + a]);
            }
            printf(" ");
            b += 4;
        }
        printf("*");
        for (int i = 0; i < 16; i++) {
            if (CC_UNLIKELY(!isprint(buf[i + a]))) {
                printf(".");
            } else
                printf("%c", buf[i + a]);
        }
        printf("*\n");
        b = 0;
        line += 16;
        a += 16;
    }
}

void Play::Dump(char *file)
{
    int fd;
    int n = 0;
    char buf[MAX];
    fd = open(file, O_RDWR, 0666);
    if (-1 == fd) {
        perror("fail to open file");
        exit(-1);
    }

    while((n = read(fd, buf, MAX - 1)) > 0) {
        typePrint(buf, n);
    }
}

int main(int argc, char** argv)
{
    float vol[2];
    int percent;
    char *buf;

    if (argc != 7) {
        printf("please input right parameter !\n");
        printf("Usage; %s path/ lVolume(0-100) rVolume(0-100) percent(0-100) type(mp3) fileToDump(/path/file)\n", argv[0]);
        return -1;
    } else {
        vol[0] = (float)atoi(argv[2])/(float)100;    
        vol[1] = (float)atoi(argv[3])/(float)100;    
        percent =  atoi(argv[4]);
    printf("path: %s, lVolume %f, rVolume %f, Play percent is %d%% file_type is %s file to dump is %s\n", argv[1], vol[0], vol[1], percent, argv[5], argv[6]);
    

    sp<ProcessState> proc(ProcessState::self()); 

    ProcessState::self()->startThreadPool();

    Play::Dump(argv[6]);
    Play::Parse(argv[1], argv[5], vol, percent);

    IPCThreadState::self()->joinThreadPool();
    Play::Free();
    }

    return 0;
}
