//
//  disk_schedule.c
//  OS
//
//  Created by MDY on 2019/11/25.
//  Copyright © 2019 mdy. All rights reserved.
//

#include "disk_schedule.h"

int start = 100; //当前磁道位置
int isLeft = 0;  //当前指针移动方向


void print_list(int* list,int count){
    for (int i=0; i<count; ++i) {
        printf("%-5d",list[i]);
    }
    printf("\n");
}

int FCFS(int* requests,int count){
    int result = 0;
    int t_start = start; //当前磁道
    for (int i = 0; i < count; ++i) {
        result += abs(t_start-requests[i]);
        t_start = requests[i];
    }
    return result;
}

int SSTF(int* requests,int count){
    int t_start = start; //当前磁道
    int *sort_requests = malloc(sizeof(int)*count);//排序后的磁道请求
    int *visited = malloc(sizeof(int)*count);//记录是否访问过该磁道
    memset(visited,0,count*sizeof(int));
    for (int i = 0; i < count; ++i) {
        int min_index = -1;
        for(int j = 0;j < count;++j){
            if(visited[j] == 0 && (min_index == -1 || abs(requests[min_index]-t_start) > abs(requests[j]-t_start))){
                min_index = j;
            }
        }
        t_start = requests[min_index];//更新当前磁道位置
        sort_requests[i] = requests[min_index];
        visited[min_index] = 1;//记录该磁道已访问
    }
    return FCFS(sort_requests, count);
}

int SCAN(int* requests,int count){
    int max = -1;//请求中的最大值
    int min = -1;// 最小值
    int t_start = start; //当前磁道
    int t_isLeft = isLeft;//当前运动方向
    int *sort_requests = malloc(sizeof(int)*count);
    int *visited = malloc(sizeof(int)*count);
    memset(visited,0,count*sizeof(int));
    for (int i = 0; i < count; ++i) {
        int min_index = -1;
        for(int j = 0;j < count;++j){
            if (i == 0) {
                if (max < requests[j]) {
                    max = requests[j];
                }
                if (min == -1 || min > requests[j]) {
                    min = requests[j];
                }
            }
            if(visited[j] == 0){
                if (
                    (
                     (t_isLeft && t_start > requests[j])||(!t_isLeft && t_start < requests[j]) //判断该请求的磁道是否个当前运行方向一致
                     )
                    &&
                    (
                    min_index == -1|| //初始的时候直接默认第一个找到的值
                    (
                     (t_isLeft && requests[j] > requests[min_index]) // 向左边找最大值
                     || (!t_isLeft && requests[j] < requests[min_index])) // 向右边找最小值
                    )
                    ){
                    min_index = j;
                }
            }
        }
        if (min_index == -1) {
            if (isLeft) {
                //说明一开始就已经在最右边了
                i = 0;
                t_isLeft = 1;
            }else{
                i = 0;
                t_isLeft = 0;
            }
        }
        if (max == requests[min_index]) {
            //说明已经在最右边了
            t_isLeft = 1;
        }
        if (min == requests[min_index]) {
            //说明已经在最左边了
            t_isLeft = 0;
        }
        t_start = requests[min_index];
        sort_requests[i] = requests[min_index];
        visited[min_index] = 1;
    }
//    print_list(sort_requests, count);
    return FCFS(sort_requests, count);
}

int NSCAN(int* requests,int count){
    int max = -1;//请求中的最大值
    int t_start = start; //当前磁道
    int *sort_requests = malloc(sizeof(int)*count);
    int *visited = malloc(sizeof(int)*count);
    memset(visited,0,count*sizeof(int));
    for (int i = 0; i < count; ++i) {
        int min_index = -1;
        for(int j = 0;j < count;++j){
            if (i == 0) {
                if (max < requests[j]) {
                    max = requests[j];
                }
            }
            if(visited[j] == 0){
                if (
                    t_start < requests[j]
                    &&
                    (
                    min_index == -1 || //初始的时候直接默认第一个找到的值
                    requests[j] < requests[min_index]// 向右边找最小值
                    )
                ){
                    min_index = j;
                }
            }
        }
        if (min_index == -1) {
            //说明已经在最右边了
            t_start = 0;
        }else{
            t_start = requests[min_index];
            sort_requests[i] = requests[min_index];
            visited[min_index] = 1;
        }
        if (max == requests[min_index]) {
            //说明已经在最右边了
            t_start = 0;
        }
    }
//    print_list(sort_requests, count);
    return FCFS(sort_requests, count);
}


int main(){
    // 磁盘请求
    int requests[9] = {55,58,39,18,90,160,150,38,184};//磁盘请求，元素的意义为请求的磁道
//    int requests[5] = {55,58,39,18,90};
//    printf("扫描算法（SACN）的平均寻道时间为：%.1f\n",SCAN(requests, 5)/5.0f);
    printf("先到先来（FCFS）的平均寻道时间为：%.1f\n",FCFS(requests, 9)/9.0f);
    printf("最短寻道时间优先（SSTF）的平均寻道时间为：%.1f\n",SSTF(requests, 9)/9.0f);
    printf("扫描算法（SACN）的平均寻道时间为：%.1f\n",SCAN(requests, 9)/9.0f);
    printf("循环扫描算法（CSACN）的平均寻道时间为：%.1f\n",NSCAN(requests, 9)/9.0f);
}
