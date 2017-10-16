/* 
 * Copyright (C) 2008 HTC Corp.
 */

#ifndef PLAYER_THREAD_H
#define PLAYER_THREAD_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include <media/mediaplayer.h>
#include "utils/Log.h"
#include <utils/threads.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <sys/prctl.h>
#include <signal.h>

namespace android {

class MediaPlayer;

static int mFd;
static long mSize;
static long mSizePlay;

typedef struct node {
    char buf[50];
    struct node *next;
} Node;

class Play {
public:
	static MediaPlayer* newMediaPlayer(char *file);
	static MediaPlayer* newMediaPlayer(char *file, long size);
	static void PlayMp3(char *file);
        static void PlayMp3(char *file, float *vol, int percent);
        static Node *GetFile(char *path, char *file_type);
        static void Parse(char *path, char *type, float *buf, int percent);
        static void Free();
        static void Dump(char *file);
};

}
#endif
