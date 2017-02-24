/*
 * media_player implement by using MediaPlayer in Android.
 *                                                                                                                                                                                                                  
 * Copyright (C) 2016 Sl0v3C.
 *
 * Author: Sl0v3C <pyy101727@gmail.com>
 *
 * Use libmedia of Andriod
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

using namespace android;
/* It seems that there is no setDataSource(file, NULL) any longer
MediaPlayer* Play::newMediaPlayer(char *file)
{
    MediaPlayer *mp = new MediaPlayer();
    mp->reset();
    if (mp->setDataSource(file, NULL) == NO_ERROR) {
        mp->setAudioStreamType(AUDIO_STREAM_MUSIC);
        mp->prepare();
    } else {
        ALOGE("Failed to load CameraService sounds: %s", file);
        return NULL;
    }
    
    return mp; 
}
*/
static int player_tid = -1;

MediaPlayer* Play::newMediaPlayer(char *file, long size)
{
    MediaPlayer *mp = new MediaPlayer();
    mp->reset();

    if (mp->setDataSource(mFd, 0, size) == NO_ERROR) {
        mp->setAudioStreamType(AUDIO_STREAM_MUSIC);
        mp->prepare();
    } else {
        ALOGE("Failed to load CameraService sounds: %s", file);
        return NULL;
    }

    return mp;
}

void yyaudioplayer_exception_handler(int signo)
{
    printf("Handler signo %d Signal 11 for yyaudioplayer_exception_callback", signo);

    Play::Free();
    char name[20];
    sprintf(name, "kill -9 %d", player_tid);
    printf("%s", name);
    system(name);
}

void* thread_play_func(void *arg)
{
    player_tid = gettid();
    prctl(PR_SET_NAME, (unsigned long)"yyaudioplayerThrd", 0, 0, 0);
    if (signal(SIGSEGV, yyaudioplayer_exception_handler) == SIG_ERR) {
        printf("Can't set exception callback handler to solve SIGNAL 11\n");
    }

    MediaPlayer* player = (MediaPlayer *)arg;
    player->start();

    while(player->isPlaying());

    player->stop();
    player->disconnect();
     
    pthread_exit((void *)"Playback is done!\n");
    return NULL;
}

void Play::PlayMp3(char *file, float *vol, int percent)
{
    int res;
    pthread_t tid;
    void *thread_result;

    mFd = open(file, O_RDWR, 0666);
    if (mFd == -1) {
        printf("Error: Fail to open %s\n", file);
        exit(-3);
    }

    mSize = lseek(mFd, 0, SEEK_END);   //return the file size - offset
    mSizePlay = percent * mSize / 100;

    MediaPlayer *mPlayer = Play::newMediaPlayer(file, mSizePlay);
 
    mPlayer->setVolume(vol[0], vol[1]);

    res = pthread_create(&tid, NULL, thread_play_func, (void *)mPlayer);
    if (res != 0) {
        perror("thread creation failed");
        exit(-1);
    }
    
    res = pthread_join(tid, &thread_result);
    if (res != 0) {
        perror("thread join failed");
        exit(-2);
    }

    printf("Thread 2 joined, return %s\n", (char *)thread_result);
}

void Play::PlayMp3(char *file)
{
    int res;
    pthread_t tid;
    void *thread_result;

    mFd = open(file, O_RDWR, 0666);
    if (mFd == -1) {
        printf("Error: Fail to open %s\n", file);
        exit(-3);
    }

    mSize = lseek(mFd, 0, SEEK_END);   //return the file size - offset
    mSizePlay = 100 * mSize / 100; // this will play the whole music, since is 100 percent

    MediaPlayer *mPlayer = Play::newMediaPlayer(file, mSizePlay);  // Use this one

    res = pthread_create(&tid, NULL, thread_play_func, (void *)mPlayer);
    if (res != 0) {
        perror("thread creation failed");
        exit(-1);
    }
    
    res = pthread_join(tid, &thread_result);
    if (res != 0) {
        perror("thread join failed");
        exit(-2);
    }

    printf("Thread 1 joined, return %s\n", (char *)thread_result);
}

