//
//  MemoryBlock.c
//  OS
//
//  Created by 中山附一 on 2019/11/12.
//  Copyright © 2019 mdy. All rights reserved.
//

#include "MemoryBlock.h"

//作业请求操作序列，第一位为作业id，第二位为请求大小，第三位为申请(0)/x释放(1)
int requests[11][3] = {
    {1,130,0},{2,60,0},{3,100,0},{2,60,1},{4,200,0},{3,100,1},
    {1,130,1},{5,140,0},{6,60,0},{7,50,0},{8,60,0},
};


MemoryBlock *list;
MemoryBlock *alloc_list;
int min = 0;

void sort(){
    
}

void print_M(){
    printf("==========================当前分区信息================================\n\n");
    MemoryBlock *temp = list;
    printf("%-14s%-14s%-14s%s\n","起始地址","分区大小","分区状态","占用分区的进程");
    while (temp != NULL) {
        printf("%-9d%-9d%-9d%d\n",temp->start,temp->size,temp->state,temp->pid);
        temp = temp->rear;
    }
    temp = alloc_list;
    while (temp != NULL) {
        printf("%-9d%-9d%-9d%d\n",temp->start,temp->size,temp->state,temp->pid);
        temp = temp->rear;
    }
    printf("===================================================================\n\n");
}

void add_alloc_list(MemoryBlock *block){
    if (alloc_list == NULL) {
        alloc_list = block;
        block->front = NULL;
        block->rear = NULL;
    }else{
        block->rear = alloc_list;
        block->front = NULL;
        alloc_list->front = block;
        alloc_list = block;
    }
}

void remove_block_from_list(MemoryBlock *temp){
    if (temp->rear != NULL) {
        temp->rear->front = temp->front;
    }
    if (temp->front != NULL) {
        temp->front->rear = temp->rear;
    }
}

/// 分配作业 首次适应
/// @param request 作业请求
/// @return NULL说明分配失败
MemoryBlock *allco(int *request){
    
    MemoryBlock *temp = list;
    while (temp!=NULL) {
        if (temp->state==0) {
            if (temp->size== request[1] || temp->size == request[1] + min) {
                //正好合适或者无法再分
                temp->pid =  request[0];
                temp->state = 1;
                //从空闲队列移除一个内存块
                remove_block_from_list(temp);
                //加入分配队列
                add_alloc_list(temp);
                printf("[start:%d,size:%d,state:%d,pid:%d]分配\n",temp->start,temp->size,temp->state,temp->pid);
                break;
            }else if(temp->size > request[1]){
                //大于就要分割
                MemoryBlock *node = malloc(sizeof(MemoryBlock));
                node->start = temp->start;
                node->state = 1;
                node->pid = request[0];
                node->size = request[1];
                
                temp->size -= request[1];
                temp->start = node->start + request[1];
                
                //加入已分配的内存块的队列
                add_alloc_list(node);
                printf("[start:%d,size:%d,state:%d,pid:%d]分配\n",node->start,node->size,node->state,node->pid);
                temp = node;
                break;
            }
        }
        temp = temp->rear;
    }
    print_M();
    return temp;
}


/// 释放作业 首次适应
/// @param request 释放作业请求
void release(int *request,int firstOrBest){
    MemoryBlock *temp = alloc_list;
    while (temp!=NULL) {
        if (temp->pid == request[0] && (temp->size == request[1] || temp->size == request[1] + min) ) {
            MemoryBlock *block = list;//遍历空闲队列
            MemoryBlock *front,*rear;
            rear = front = NULL;
            int temp_start = temp->start + temp->size;
            while (NULL != block) {
                if (block->start + block->size == temp->start) {
                    //存在后继内存块
                    rear = block;
                }
                if (temp_start == block->start) {
                    //存在前驱内存块
                    front = block;
                }
                block = block->rear;
            }
            temp->state = 0;
            temp->pid = -1;
            //合并内存块
            if (front != NULL) {
                remove_block_from_list(front);
                temp->size += front->size;
                if (list == front) {
                    list = list->rear;
                    list->front = NULL;
                }
                free(front);
            }
            if (rear != NULL) {
                remove_block_from_list(rear);
                temp->size += rear->size;
                temp->start = rear->start;
                if (list == rear) {
                    list = list->rear;
                    list->front = NULL;
                }
                free(rear);
            }
            //从列表中移除要释放的这个内存块
            remove_block_from_list(temp);
            
            //排除在分配队列首的情况
            if (temp==alloc_list) {
                alloc_list = alloc_list->rear;
            }
            
            
            //将释放的空间加入空闲队列
            block = list;
            while (block != NULL) {
                if ((firstOrBest == 0 && block->start > temp->start)
                    ||
                    (firstOrBest == 1 && block->size > temp->size)) {
                    temp->rear = block;
                    block->front = temp;
                    temp->front = NULL;
                    if (list == block) {
                        list = temp;
                    }
                    break;
                }
                if (block->rear == NULL) {
                    block->rear = temp;
                    temp->front = block;
                    temp->rear = NULL;
                }
                block = block->rear;
            }
        }
        temp = temp->rear;
    }
    print_M();
}



int main(){
    list = malloc(sizeof(MemoryBlock));
    list->front = NULL;
    list->rear = NULL;
    list->size = 640;
    list->state = 0;
    list->start = 0;
    list->pid = -1;
    print_M();
    for (int i = 0; i < 11; ++i) {
        if (requests[i][2] == 0) {
            allco(requests[i]);
        }else{
            release(requests[i],1);
        }
    }
    return 0;
}

