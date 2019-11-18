//
//  MemoryBlock.c
//  OS
//
//  Created by 中山附一 on 2019/11/12.
//  Copyright © 2019 mdy. All rights reserved.
//

#include "MemoryBlock.h"

//作业请求操作序列，第一位为作业id，第二位为请求大小，第三位为申请(0)/x释放(1)
int requests[11][3] = {{1,130,0},{2,60,0},{3,100,0},{2,60,1},{4,200,0},{3,100,1},
                     {1,130,1},{5,140,0},{6,60,0},{7,50,0},{8,60,0},
};


MemoryBlock *list;
int min = 0;

void sort(){
    
}

void print_M(){
    MemoryBlock *temp = list;
    while (temp != NULL) {
        printf("[start:%d,size:%d,state:%d,pid:%d]->",temp->start,temp->size,temp->state,temp->pid);
        temp = temp->rear;
    }
    printf("\n");
}

/// 分配作业
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
                printf("[start:%d,size:%d,state:%d,pid:%d]分配\n",temp->start,temp->size,temp->state,temp->pid);
                break;
            }else if(temp->size > request[1]){
                //大于就要分割
                MemoryBlock *node = malloc(sizeof(MemoryBlock));
                node->start = temp->start;
                
                node->rear = temp->rear;
                if (temp->rear != NULL) {
                    temp->rear->front = node;
                }
                
                node->front = temp;
                temp->rear = node;
                
                node->state = 1;
                node->pid = request[0];
                node->size = request[1];
                
                temp->size -= request[1];
                temp->start = node->start + request[1];
                
                printf("[start:%d,size:%d,state:%d,pid:%d]分配\n",node->start,node->size,node->state,node->pid);
                
                break;
            }
        }
        temp = temp->rear;
    }
    print_M();
    return NULL;
}

void release(int *request){
    MemoryBlock *temp = list;
    while (temp!=NULL) {
        if (temp->pid == request[0] && (temp->size == request[1] || temp->size == request[1] + min) ) {
            if (NULL != temp->front && 0 == temp->front->state) {
                temp->size+=temp->front->size;
                //前面为空闲
                if (temp->front->front != NULL) {
                    temp->front->front->rear = temp;
                }
                MemoryBlock *t = temp->front;
                temp->front = temp->front->front;
                free(t);
            }
            if (NULL != temp->rear && 0 == temp->rear->state) {
                //后面为空闲
                temp->rear->front = temp->front;
                temp->front->rear = temp->rear;
                temp->rear->size += temp->size;
                free(temp);
            }else{
                temp->pid = -1;
                temp->state = 0;
            }
            break;
        }
        temp = temp->rear;
    }
    
    print_M();
}

//int main(){
//    list = malloc(sizeof(MemoryBlock));
//    list->front = NULL;
//    list->rear = NULL;
//    list->size = 640;
//    list->state = 0;
//    list->start = 0;
//    list->pid = -1;
//    for (int i = 0; i < 11; ++i) {
//        if (requests[i][2] == 0) {
//            allco(requests[i]);
//        }else{
//            release(requests[i]);
//        }
//    }
//    return 0;
//}
//
