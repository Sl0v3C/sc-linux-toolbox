/*
 * find_file find the mp3 files using sigle link data structure
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

#define LOG_TAG "YY FindFile"

#include "media_player.h"

using namespace android;

Node *head = NULL;

//创建链表，头结点data=0，pNext=NULL;
bool CreateNodeList()
{
    head = (Node *)malloc(sizeof(struct node));
    if (NULL == head) {
        return false;
    } else {
        head->next = NULL;
        return true;
    }
}

//删除节点
void freeNode()
{
    Node *p, *q;
    p = head;

    while(p->next != NULL) {
        q = p->next;
        p->next = p->next->next;
        free(q);
    }
    free(p);

    head = NULL;
}

//增加节点
bool addNode(Node* node)
{
    Node *p;
    p = head;
    while (p->next != NULL)
        p = p->next;
    p->next = node;
    node->next = NULL;

    return true;
}

Node *Play::GetFile(char *path, char *file_type)
{
    char buf[100];  // 这里buf的大小一定要够大，不然会报错 FORTIFY: vsprintf: prevented write past end of buffer
    bool ret;
    FILE *fp;
    Node *slink;
    sprintf(buf, "ls %s | grep -i .%s > /data/local/tmp/1.txt", path, file_type);

    //printf("buf is %s\n", buf);

    system(buf);  // 将以mp3不分大小写的后缀名的文件保存到/data/local/tmp/1.txt之中
    fp = fopen("/data/local/tmp/1.txt", "r");
    if (fp == NULL) {
        printf("Fail to open file\n");
        exit(-1);
    }
    
    if (!CreateNodeList()) {    // 创建单链表
        ALOGE("Fail to create single link!");
        exit(-1);
    }
    
    do {     // 循环每行的获取歌曲名字，并添加到单链表中
        slink = (Node *)malloc(sizeof(Node));
        addNode(slink);
    } while(fgets(slink->buf, 50, fp) !=NULL);
    
    fclose(fp);

    system("rm /data/local/tmp/1.txt");  // 删除tmp文件
    if (head->next != NULL)
        return head->next;  // 返回第一个节点
    else
        return NULL;      // 否则返回NULL
} 

void Play::Parse(char *path, char *type, float *vol, int percent)
{
    Node *sl;
    char buf[100], tmp[100];

    sl = (Node *)malloc(sizeof(struct node));

    sl = Play::GetFile(path, type);

    while(sl != NULL) {
        //##################下面这行非常重要，搞了一下午#######################
        sl->buf[strlen(sl->buf) - 1] = '\0'; //由于fgets获取到的字符串一定要将最后的字符赋成'\0'，否则会打开文件失败等
        sprintf(buf, "%s%s", path, sl->buf);
        sprintf(tmp, "chmod 0666 %s", buf);
        system(tmp); // 给歌曲设置权限，避免无法读取
        printf("buf %s\n", buf);
        Play::PlayMp3(buf, vol, percent); // 将歌曲名字/音量大小/播放音频文件的百分度设置下去，并最终播放
        //Play::PlayMp3(buf);  // only play mp3 except volume & percent
        sl = sl->next;
    }

}

void Play::Free()
{
    freeNode();  // 一定要free节点来释放内存，避免内存泄漏
}
