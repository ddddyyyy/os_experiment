//
//  main.c
//  OS
//
//  Created by 中山附一 on 2019/11/7.
//  Copyright © 2019 mdy. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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

BLOCKNODE *wating_queue = NULL;
BLOCKNODE *complete_queue;

int current_time = 0; //模拟系统时间

int count = 5; //模拟进程数目

int slice = 10; // 时间片大小

BLOCKNODE *initNode(){
    PCB *target = malloc(sizeof(PCB));
    target->need_time = rand() % 20 + 1;
    target->enter_time = current_time;
    target->running_time = 0;
    target->waiting_time = 0;
    target->status = 'W';
    target->proiority = rand() % 10;
    target->name = malloc(2*sizeof(char));
    target->name[0] = (char)(count+48);
    target->name[1] = '\0';
    
    printf("进程%s在%d的时候加入\n",target->name,current_time);
    
    --count;
    
    //初始化节点
    BLOCKNODE *temp = malloc(sizeof(BLOCKNODE));
    temp->data = target;
    temp->next = NULL;
    
    return temp;
}

//打印队列信息
void printPCB()
{
    //    return;
    printf("================================================================\n");
    BLOCKNODE *temp;
    PCB *target;
    temp = wating_queue;
    printf("当前进程的队列信息:");
    while (temp != NULL) {
        target = temp->data;
        printf("[进程名：%s,进程状态%c,运行时间%d,需要的时间%d,等待的时间%d]->",target->name,target->status,target->running_time,target->need_time,target->waiting_time);
        temp = temp->next;
    }
    printf("\n");
    temp = complete_queue;
    printf("结束进程的队列信息:");
    while (temp != NULL) {
        target = temp->data;
        printf("[进程名：%s,等待的时间%d,结束运行的时间%d]->",target->name,target->waiting_time,target->end_time);
        temp = temp->next;
    }
    printf("\n");
    printf("================================================================\n");
}

// 将完成的进程放入完成队列
void push(BLOCKNODE *node){
    printf("进程%s在%d的时候完成\n",node->data->name,current_time);
    node->next = NULL;
    if (complete_queue == NULL) {
        complete_queue = node;
    }else{
        BLOCKNODE *temp = complete_queue;
        while (temp->next!=NULL) {
            temp=temp->next;
        }
        temp->next = node;
    }
    printPCB();
}

//初始化一个进程，并加入队列
//高响应比优先
//high response rate next
PCB * addPCBHRRN(int current_time)
{
    BLOCKNODE *temp = initNode();
    PCB *target = temp->data;
    
    if (wating_queue == NULL){
        wating_queue = temp;
        target->status = 'R';
        return target;
    }
    
    BLOCKNODE *start = wating_queue;
    wating_queue->data->status='W';
    while (start->next != NULL) {
        BLOCKNODE *now = start;
        BLOCKNODE *max = now;
        float m_nrr = 0;
        while (now->next!=NULL) {
            float nrr = 1.0*(now->data->waiting_time)/now->data->need_time + 1;
            if (m_nrr == 0) {
                m_nrr = nrr;
            }else{
                if (nrr > m_nrr) {
                    //找到更大的就交换位置
                    PCB *t = max->data;
                    max->data = now->data;
                    now->data = t;
                    m_nrr = nrr;
                }
            }
            now = now->next;
        }
        start = start->next;
    }
    wating_queue->data->status = 'R';
    start->next = temp;//新加入的进程一定是在最后面
    //打印信息
    printPCB();
    
    return target;
}



//初始化一个进程，并加入队列
//短进程优先 shortest process next
PCB *addPCBSPN(int current_time)
{
    BLOCKNODE *temp = initNode();
    PCB *target = temp->data;
    
    BLOCKNODE *now;
    BLOCKNODE *pre = NULL;
    
    // 入队列
    if (wating_queue != NULL)
    {
        now = wating_queue;
        while (now != NULL)
        {
            // 短进程优先
            if (now->data->need_time > target->need_time)
            {
                if (pre != NULL)
                {
                    pre->next = temp;
                    temp->next = now;
                    break;
                }
                else
                {
                    temp->next = now;
                    now->data->status = 'W';
                    target->status = 'R';
                    wating_queue = temp;
                    break;
                }
            }
            pre = now;
            now = now->next;
        }
        if (now == NULL) {
            //加入队列尾
            pre->next = temp;
        }
    }
    else
    {
        wating_queue = temp;
        target->status = 'R';
    }
    
    //打印信息
    printPCB();
    return target;
}



// 开始跑进程
// 最高响应比或者最短进程
void HRRNorSPN(int HRRNorSPN){
    if (HRRNorSPN) {
        addPCBSPN(0);//短进程
    }else{
        addPCBHRRN(0); //高响应优先
    }
    while (wating_queue != NULL || count > 0)
    {
        current_time += 1;
        if (count > 0 && rand() % 10 == 3)
        {
            if (HRRNorSPN) {
                addPCBSPN(0);//短进程
            }else{
                addPCBHRRN(0); //高响应优先
            }
        }
        if (wating_queue != NULL) {
            PCB *temp = wating_queue->data;
            temp->running_time += 1;
            BLOCKNODE *t = wating_queue->next;
            while (t!=NULL) {
                t->data->waiting_time++;
                t = t->next;
            }
            if (temp->running_time >= temp->need_time)
            {
                //进程结束
                temp->status = 'F';
                temp->end_time = current_time;
                BLOCKNODE *t = wating_queue;
                wating_queue = wating_queue->next;
                if (wating_queue != NULL) {
                    wating_queue->data->status = 'R';
                }
                push(t);
                printPCB();
            }
        }
    }
    printf("运行总时间为%d\n",current_time);
    int total = 0;
    while (complete_queue!=NULL) {
        total += (complete_queue->data->end_time - complete_queue->data->enter_time);
        complete_queue = complete_queue->next;
    }
    printf("平均周转时间为%f\n",total/5.0);
}


void addPCBRR(){
    BLOCKNODE *temp = initNode();
    
    if (wating_queue == NULL) {
        wating_queue = temp;
        temp->data->status='R';
    }else{
        BLOCKNODE *root = wating_queue;
        while (root->next!=NULL) {
            root=root->next;
        }
        root->next = temp;
    }
    printPCB();
}


//round robin
void RR(){
    int current_slice = slice;
    addPCBRR();
    while (wating_queue != NULL || count > 0){
        //加入新进程
        if (count > 0 && rand() % 10 == 3)
        {
            addPCBRR();
        }
        current_time += 1;//系统时间+
        //队列不空闲才需要进行
        if (NULL != wating_queue) {
             --current_slice;//当前程序的时间片-
            PCB *temp = wating_queue->data;//当前进程
            temp->running_time ++;//程序运行时间+
            //其他进程等待时间+
            BLOCKNODE *t = wating_queue ->next;
            while (t!=NULL) {
                t->data->waiting_time++;
                t=t->next;
            }
            
            if (temp->running_time >= temp->need_time) {
                //中止进程
                temp->status = 'F';
                temp->end_time = current_time;
                //移除进程进完成队列
                BLOCKNODE *old = wating_queue;
                wating_queue = wating_queue->next;
                if (wating_queue!=NULL) {
                    wating_queue->data->status='R';
                }
                push(old);
                current_slice = slice;//重置时间片
            }
        }
        if (current_slice == 0) {
            //时间片结束，不止一个进程的时候切换进程
            if (wating_queue!=NULL && wating_queue->next !=NULL) {
                BLOCKNODE *root = wating_queue;
                root->data->status='W';
                while (root->next!=NULL) {
                    root=root->next;
                }
                root->next = wating_queue;
                wating_queue->next = NULL;
                wating_queue = root;
                wating_queue->data->status='R';
            }
            current_slice = slice;//重置时间片
            printf("时间片切换\n");
            printPCB();
        }
    }
    printf("运行总时间为%d\n",current_time);
    int total = 0;
    while (complete_queue!=NULL) {
        total += (complete_queue->data->end_time - complete_queue->data->enter_time);
        complete_queue = complete_queue->next;
    }
    printf("平均周转时间为%f\n",total/5.0);
}


int main()
{
    srand((unsigned)time(NULL));
    for (int i=0; i<20; i++) {
        current_time = 0;
        count = 5;
        HRRNorSPN(1); //(0)高响应优先或者最短进程优先(1)
        RR(); //时间片轮转
    }
    return 0;
}
