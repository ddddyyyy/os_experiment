//
//  MemoryBlock.h
//  OS
//
//  Created by MDY on 2019/11/12.
//  Copyright © 2019 mdy. All rights reserved.
//

#ifndef MemoryBlock_h
#define MemoryBlock_h

#include <stdio.h>
#include <stdlib.h>

typedef struct MemoryBlock{
    struct MemoryBlock *front; //前面的内存块
    struct MemoryBlock *rear; //后面的内存块
    int size;  //当前内存块的大小
    int state; //内存块的状态
    int start; //当前内存块的开始地址
    int pid;   //作业的id
    int bid;   //当前分区的id，用于分区释放
}MemoryBlock;

#endif /* MemoryBlock_h */
