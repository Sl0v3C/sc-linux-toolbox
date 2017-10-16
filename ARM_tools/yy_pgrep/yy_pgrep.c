/*
 * yy_pgrep like pgrep to return the PID number by process name.
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>

#define MAX 1024 * 4
#define PATH_SIZE 32
#define PROC "/proc/"

int find(const char *path, const char *name, const char *needFind)
{
    char *p, *s;
    char char_all[MAX] = "\0";
    char buf[MAX] = "\0";
    int fd, n, len;
    int ret = 0;

    if ((fd = open(path, O_RDONLY)) == -1) {
        perror("fail to open");
        return ret;
    }

    if ((n = read(fd, char_all, MAX)) == -1) {
        perror("fail to read");
        goto err;
    }

    char_all[n] = '\0';  // add \0
    p = strstr(char_all, name);
    if (p == NULL)
        goto err;
    s = strstr(p, "\n");
    if (s == NULL)
        goto err;
    len = s - p + 1;
    
    strncpy(buf, p + 6, len - 6);
//    printf("buf %s needFind %s strlen %d\n", buf, needFind, strlen(needFind));
    if (strncmp(buf, needFind, strlen(needFind)) == 0) {
        p = strstr(char_all, "Pid:");
        if (p == NULL)
            goto err;
        s = strstr(p, "\n");
        if (s == NULL)
            goto err;
        len = s - p + 1;
        memset(buf, 0, MAX);
        strncpy(buf, p + 5, len - 5);
        ret = atoi(buf); 
//        printf("buf %s %d", buf, atoi(buf));
    }

err:
    close(fd);
    return ret;
}


int search(char *ptr, const char *p_dir, const char *needFind)
{
    char *path = ptr;
    int n = 0;
    strcat(path, p_dir);
    strcat(path, "/status");
    n = find(path, "Name:", needFind);

    return n;
}


void Usage(const char *str)
{
    printf("Usage: %s findProcessName\n", str);

    return ;
}

int isStrAllDigit(const char *str)
{
    char ch;

    while ((ch = *str++) != '\0') {
        if (isdigit(ch))
            continue;
        else
            return 0;        
    }
    
    return 1;
}

int main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *dir;
    int res = -1;
    char path[PATH_SIZE] = "\0";
   
    if (argc == 1) {
        printf("Invalid Argument\n");
        Usage(argv[0]);
        return -1;
    }

    if ((dp = opendir(PROC)) == NULL) {
        perror("fail to opendir");
        exit(-1);
    }

    while ((dir = readdir(dp)) != NULL) {
        if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0) || !isStrAllDigit(dir->d_name)) 
            continue;
        strcpy(path, PROC);
//        printf("path %s argv[1] %s d_name %s\n", path, argv[1], dir->d_name);
        if ((res = search(path, dir->d_name, argv[1])) > 0) {
//          printf("Find out the process %s Pid is %d, return %d\n", argv[1], res, res);
            printf("%d", res);
            return res;
        }
    }

    if (closedir(dp) == -1) {
        perror("fail to closedir");
        exit(1);
    }

    printf("Cannot find the process named %s\n", argv[1]);

    return res;
}


