//
//  main.c
//  OS
//
//  Created by 中山附一 on 2019/11/7.
//  Copyright © 2019 mdy. All rights reserved.
//

#include <stdio.h>

typedef enum PROCESS_STATUS
{
    W, //wait
    R, //run
    F  //finish
} PROCESS_STATUS;

typedef struct PCB
{
    /* data */
    char *name;            //进程名
    int proiority;         //优先级
    char status; //进程状态
    int enter_time;        //进入时间
    int need_time;         //需要运行的时间
    int running_time;      //已经运行的时间
    int waiting_time;      //等待的时间
    int end_time; //进程结束的时间
    
} PCB;

typedef struct BLOCKNODE
{
    PCB *data;
    struct BLOCKNODE *next;
} BLOCKNODE;
