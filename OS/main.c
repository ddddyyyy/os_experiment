//
//  main.c
//  OS
//
//  Created by 中山附一 on 2019/11/9.
//  Copyright © 2019 mdy. All rights reserved.
//

#include <stdio.h>
#include "process_schedule.h"
#include "MemoryBlock.h"


int main()
{
    printf("进程调度实验\n");
    process_test();
    printf("内存分配实验\n");
    memory_test();
    printf("磁盘访问实验\n");
    disk_test();
    return 1;
}
