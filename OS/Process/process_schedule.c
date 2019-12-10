//
//  process_schedule.c
//  OS
//
//  Created by 中山附一 on 2019/11/9.
//  Copyright © 2019 mdy. All rights reserved.
//

#include "process_schedule.h"

int run_time[5] = {4,3,4,2,4};
int resource[3] = {10,15,12}; //总资源
int **alloc_matrix;//分配矩阵
int **max_matrix; //最大需求矩阵
int *available_matrix;//可利用资源
int *finsh;//是否完成

BLOCKNODE *waiting_queue = NULL;
BLOCKNODE *complete_queue = NULL;
BLOCKNODE *blocked_queue = NULL;

int current_time = 0; //模拟系统时间

int count = 5; //模拟进程数目

int slice = 5; // 时间片大小

BLOCKNODE *initNode(){
    PCB *target = malloc(sizeof(PCB));
    target->need_time = run_time[5-count];
    target->enter_time = current_time;
    target->running_time = 0;
    target->waiting_time = 0;
    target->status = 'W';
    target->proiority = rand() % 10;
    target->name = malloc(2*sizeof(char));
    target->name[0] = (char)(count+48);
    target->name[1] = '\0';
    target->need_resource = malloc(sizeof(int)*3);
    for (int i=0; i<3; ++i) {
        target->need_resource[i] = 0;
        max_matrix[count - 1][i] = rand()% resource[i] + 1;
        alloc_matrix[count-1][i] = 0;
    }

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
    printf("==========================当前进程信息================================\n\n");
    BLOCKNODE *temp;
    PCB *target;
    temp = waiting_queue;
    printf("进程名  进程状态  已经运行时间  需要运行的时间  等待时间  结束时间\n");
    while (temp != NULL) {
        target = temp->data;
    printf("%5s%8c%12d%13d%9d%8d\n",
           target->name,target->status,target->running_time,target->need_time,target->waiting_time,-1);
        temp = temp->next;
    }
    temp = complete_queue;
    while (temp != NULL) {
        target = temp->data;
        printf("%5s%8c%12d%13d%9d%8d\n",
           target->name,target->status,target->running_time,target->need_time,target->waiting_time,target->enter_time+target->waiting_time+target->need_time);
        temp = temp->next;
    }
    temp = blocked_queue;
    while (temp != NULL) {
        target = temp->data;
        printf("%5s%8c%12d%13d%9d%8d\n",
           target->name,target->status,target->running_time,target->need_time,target->waiting_time,-1);
        temp = temp->next;
    }
    printf("\n");
    
    printf("最大需求矩阵   已分配矩阵  总资源数目\n");
        for (int i = 0; i < 5; ++i) {
            for(int j=0;j<3;++j){
                printf("%-2d ",max_matrix[i][j]);
            }
            printf("    ");
            for(int j=0;j<3;++j){
                printf("%-2d ",alloc_matrix[i][j]);
            }
            if(i == 0){
                printf(" ");
                for(int j=0;j<3;++j){
                    printf("%-2d ",resource[j]);
                }
            }
            printf("\n");
        }
    
    printf("===================================================================\n\n");
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

// 将完成的进程放入堵塞队列
void push_block(BLOCKNODE *node){
    printf("进程%s在%d的时候堵塞\n",node->data->name,current_time);
    node->next = NULL;
    if (blocked_queue == NULL) {
        blocked_queue = node;
    }else{
        BLOCKNODE *temp = blocked_queue;
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
    //初始化节点信息
    BLOCKNODE *temp = initNode();
    PCB *target = temp->data;
    
    if (waiting_queue == NULL){
        //就绪队列为空说明没有其他的进程，因此直接赋值
        waiting_queue = temp;
        target->status = 'R';
        return target;
    }
    
    BLOCKNODE *start = waiting_queue;
    waiting_queue->data->status='W';
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
    waiting_queue->data->status = 'R';
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
    if (waiting_queue != NULL)
    {
        now = waiting_queue;
        while (now != NULL)
        {
            // 短进程优先
            if (now->data->need_time - now->data->running_time > target->need_time)
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
                    waiting_queue = temp;
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
        waiting_queue = temp;
        target->status = 'R';
    }
    
    //打印信息
    printPCB();
    return target;
}


// 开始跑进程
// 最高响应比或者最短进程
void HRRNorSPN(int HRRNorSPN){
    waiting_queue = complete_queue = blocked_queue = NULL;
    current_time = 0;
    count = 5;
    //最大，分配矩阵
    max_matrix = malloc(sizeof(int*)*count);
    alloc_matrix = malloc(sizeof(int*)*count);
    for(int i = 0;i<count;++i){
        max_matrix[i] = malloc(sizeof(int)*3);
        alloc_matrix[i] = malloc(sizeof(int)*3);
        for (int j=0; j<3; j++) {
            alloc_matrix[i][j]=0;
            max_matrix[i][j]=0;
        }
    }
    if (HRRNorSPN) {
        addPCBSPN(0);//短进程
    }else{
        addPCBHRRN(0); //高响应优先
    }
    while (waiting_queue != NULL || count > 0)
    {
        current_time += 1;
        if (waiting_queue != NULL) {
            //就绪队列第一个进程即为当前进程
            PCB *temp = waiting_queue->data;
            temp->running_time += 1;
            //增加其他进程的等待时间
            BLOCKNODE *t = waiting_queue->next;
            while (t!=NULL) {
                t->data->waiting_time++;
                t = t->next;
            }
            if (temp->running_time >= temp->need_time)
            {
                //进程结束
                temp->status = 'F';
                temp->end_time = current_time;
                BLOCKNODE *t = waiting_queue;
                waiting_queue = waiting_queue->next;
                if (waiting_queue != NULL) {
                    waiting_queue->data->status = 'R';
                }
                push(t);
                printPCB();
            }
        }
        if (count > 0)
        {
            //随机加入进程
            if (HRRNorSPN) {
                addPCBSPN(current_time);//短进程
            }else{
                addPCBHRRN(current_time); //高响应优先
            }
        }
    }
    printf("运行总时间为%d\n",current_time);
    int total = 0;
    while (complete_queue!=NULL) {
        total += (complete_queue->data->waiting_time + complete_queue->data->need_time);
        complete_queue = complete_queue->next;
    }
    printf("平均周转时间为%f\n",total/5.0);
}


void addPCBRR(){
    BLOCKNODE *temp = initNode();
    
    if (waiting_queue == NULL) {
        waiting_queue = temp;
        temp->data->status='R';
    }else{
        BLOCKNODE *root = waiting_queue;
        while (root->next!=NULL) {
            root=root->next;
        }
        root->next = temp;
    }
    printPCB();
}

//是否符合资源安全
int is_completed(int *f){
    int flag = 1;
    for (int i = 0; i < 5; ++i) {
        if (!f[i]) {
            flag = 0;
            break;
        }
    }
    return flag;
}

//银行家算法
int bank_algorithm(){
    int temp_available[3];
    int temp_finsh[5];
    //备份可利用资源
    for(int i = 0;i<3;++i){
        temp_available[i] = available_matrix[i];
    }
    for(int i = 0;i<5;++i){
        temp_finsh[i] = finsh[i];
    }
    for (int z = 0; z < 5; ++z) {
            //校验是否会发生死锁
            for (int i = 0; i < 5;++i) {
                int lock = 0;
                if (!temp_finsh[i]) {
                    for (int j = 0; j < 3; ++j) {
                        if (max_matrix[i][j] - alloc_matrix[i][j] > temp_available[j]) {
                            lock = 1;
                            break;
                        }
                    }
                    if (!lock) {
                        //资源充足就释放资源，给其他的进程
                        for (int j = 0; j < 3; ++j) {
                            temp_available[j] +=  alloc_matrix[i][j];
                        }
                        temp_finsh[i] = 1;
                    }
                }
            }
    }
    return is_completed(temp_finsh);//判断finsh数组是否满足条件
}

//给进程随机分配资源
void alloc_resource(BLOCKNODE *head){
    printf("****************************开始分配资源****************************\n");
    if (NULL != head) {
        PCB *data = head->data;
        int index = data->name[0] - 49;//拿到当前进程在资源数组中的下标
        int *temp = malloc(sizeof(int)*3); //存放之前分配资源的数据
        int success = 1;
        for(int i=0;i<3;++i){
            temp[i] = alloc_matrix[index][i];
            if (data->need_resource[i] ==0 && alloc_matrix[index][i] != max_matrix[index][i]){
                //如果不相等就进行
                //之前没有存在资源申请就进行随机资源申请
                int t = max_matrix[index][i]-alloc_matrix[index][i];
                if (t == 0) {
                    continue;
                }
                data->need_resource[i] = rand()%(t + 1);
            }
            if (data->need_resource[i] > available_matrix[i]) {
                data->status = 'B';//等待
                success = 0;
                break;
            }
        }
        //存在足够的资源进行分配
        if (success) {
            for (int i = 0; i<3; ++i) {
                available_matrix[i] -= data->need_resource[i];
                alloc_matrix[index][i] += data->need_resource[i];
            }
            if (!bank_algorithm()) {
                printf("进程%s资源分配失败，不符合资源安全性\n",data->name);
                //不满足银行家算法，退回资源
                for (int i = 0; i<3; ++i) {
                    available_matrix[i] += data->need_resource[i];
                    alloc_matrix[index][i] -= data->need_resource[i];
                    data->status = 'B';//等待
                }
            }else{
                printf("进程%s资源分配成功\n",data->name);
                printPCB();
                int flag = 1;
                //满足的话就检测是否达到最大需求
                for (int i = 0; i<3; ++i) {
                    if (max_matrix[index][i] != alloc_matrix[index][i]) {
                        flag = 0;
                    }
                    data->need_resource[i] = 0; //清空请求记录
                }
                if (flag) {
                    //进程终止
                    data->status = 'F';
                }else{
                    data->status = 'R';
                }
            }
        }else{
            printf("进程%s资源分配失败，资源不足\n",data->name);
        }
    }
    printf("****************************结束分配资源****************************\n");
}

//释放资源
void release_reource(){
    PCB *data = waiting_queue->data;
    int index = data->name[0] - 49;
    for(int i =0;i<3;++i){
        available_matrix[i] += alloc_matrix[index][i];
        finsh[index] = 1;
        alloc_matrix[index][i] = 0;
    }
}

void RR_With_Resource(){
    waiting_queue = complete_queue = blocked_queue = NULL;
    current_time = 0;
    count = 5;
    available_matrix = malloc(sizeof(int)*3);//剩余资源
    for(int i =0;i<3;++i){
        available_matrix[i] = resource[i];
    }
    //最大，分配矩阵
    max_matrix = malloc(sizeof(int*)*count);
    alloc_matrix = malloc(sizeof(int*)*count);
    for(int i = 0;i<count;++i){
        max_matrix[i] = malloc(sizeof(int)*3);
        alloc_matrix[i] = malloc(sizeof(int)*3);
        for (int j=0; j<3; j++) {
            alloc_matrix[i][j]=0;
            max_matrix[i][j]=0;
        }
    }
    //完成
    finsh = malloc(sizeof(int)*count);
    
    for(int i = 4;i>=0;--i){
        finsh[i]=0;
        addPCBRR();
    }
    
    int current_slice = slice;
    
    while (waiting_queue != NULL || count > 0){
        current_time += 1;//系统时间+
        //队列不空闲才需要进行
        if (NULL != waiting_queue) {
             --current_slice;//当前程序的时间片-
            PCB *temp = waiting_queue->data;//当前进程
            
            temp->running_time ++;//程序运行时间+
            //其他进程等待时间+
            BLOCKNODE *t = waiting_queue ->next;
            while (t!=NULL) {
                t->data->waiting_time++;
                t=t->next;
            }
            //堵塞进程等待时间j+
            t = blocked_queue;
            while (t!=NULL) {
                t->data->waiting_time++;
                t=t->next;
            }
            
            
            if(rand() % 3 == 1){
                alloc_resource(waiting_queue);//申请资源
            }
            
            if(temp->status == 'F'){
                release_reource();//清空资源
                //中止进程
                temp->end_time = current_time;
                //移除进程进完成队列
                BLOCKNODE *old = waiting_queue;
                waiting_queue = waiting_queue->next;
                push(old);
                if (waiting_queue!=NULL) {
                    waiting_queue->data->status='R';
                }else{
                    //重新分配堵塞进程的资源
                    old = blocked_queue;
                    BLOCKNODE *pre = NULL;
                    while (old!=NULL) {
                        alloc_resource(old);
                        if (old->data->status != 'B') {
                            //分配成功则将进程重新返回就绪队列
                            if (pre!=NULL) {
                                pre->next = old->next;
                            }else{
                                blocked_queue = blocked_queue->next;
                            }
                            old->next = waiting_queue;
                            waiting_queue = old;
                            break;
                        }
                        pre = old;
                        old = old->next;
                    }
                }
                current_slice = slice;//重置时间片
            }else if(temp->status == 'B'){
                //将阻塞的进程已到队列尾
                BLOCKNODE *old = waiting_queue;
                waiting_queue = waiting_queue->next;
                if (waiting_queue!=NULL) {
                    waiting_queue->data->status='R';
                }
                push_block(old);
            }
            
        }
        if (current_slice == 0) {
            //时间片结束，不止一个进程的时候切换进程
            if (waiting_queue!=NULL && waiting_queue->next !=NULL) {
                BLOCKNODE *root = waiting_queue;
                root->data->status='W';
                while (root->next!=NULL) {
                    root=root->next;
                }
                root->next = waiting_queue;
                waiting_queue = waiting_queue->next;
                waiting_queue->data->status='R';
                root->next->next = NULL;
            }
            current_slice = slice;//重置时间片
            printf("时间片切换\n");
            printPCB();
        }
    }
    printf("运行总时间为%d\n",current_time);
    int total = 0;
    while (complete_queue!=NULL) {
        total += (complete_queue->data->waiting_time + complete_queue->data->need_time);
        complete_queue = complete_queue->next;
    }
    printf("平均周转时间为%f\n",total/5.0);
}

//round robin
void RR(){
    waiting_queue = complete_queue = blocked_queue = NULL;
    current_time = 0;
    count = 5;
    available_matrix = malloc(sizeof(int)*3);//剩余资源
    for(int i =0;i<3;++i){
        available_matrix[i] = resource[i];
    }
    //最大，分配矩阵
    max_matrix = malloc(sizeof(int*)*count);
    alloc_matrix = malloc(sizeof(int*)*count);
    for(int i = 0;i<count;++i){
        max_matrix[i] = malloc(sizeof(int)*3);
        alloc_matrix[i] = malloc(sizeof(int)*3);
        for (int j=0; j<3; j++) {
            alloc_matrix[i][j]=0;
            max_matrix[i][j]=0;
        }
    }
    //完成
    finsh = malloc(sizeof(int)*count);
    for (int j=0; j<3; j++) {
        finsh[j]=0;
    }
    
    int current_slice = slice;//初始化时间片
    addPCBRR();//加入新进程
    while (waiting_queue != NULL || count > 0){
        //加入新进程
        if (current_time >= 1 && count > 0)
        {
            addPCBRR();
        }
        current_time += 1;//系统时间+
        //队列不空闲才需要进行
        if (NULL != waiting_queue) {
             --current_slice;//当前程序的时间片-
            PCB *temp = waiting_queue->data;//当前进程
            
            temp->running_time ++;//程序运行时间+
            //其他进程等待时间+
            BLOCKNODE *t = waiting_queue ->next;
            while (t!=NULL) {
                t->data->waiting_time++;
                t=t->next;
            }
            
            //按照进程运行时间进行进程终止
            if (temp->running_time >= temp->need_time) {
                //中止进程
                temp->status = 'F';
                temp->end_time = current_time;
                //移除进程进完成队列
                BLOCKNODE *old = waiting_queue;
                waiting_queue = waiting_queue->next;
                if (waiting_queue!=NULL) {
                    waiting_queue->data->status='R';
                }
                push(old);
                current_slice = slice;//重置时间片
            }
            
        }
        if (current_slice == 0) {
            //时间片结束，不止一个进程的时候切换进程
            if (waiting_queue!=NULL && waiting_queue->next !=NULL) {
                BLOCKNODE *root = waiting_queue;
                BLOCKNODE *temp = root->next;
                root->data->status='W';
                while (root->next!=NULL) {
                    root=root->next;
                }
                root->next = waiting_queue;
                waiting_queue->next = NULL;
                waiting_queue = temp;
                waiting_queue->data->status='R';
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


//测试数据
//进程       5    4    3    2    1
//到达时间    0    1    2    3    4
//服务时间    4    3    4    2    4
/// 进程调度模拟
void process_test(){
    srand((unsigned)time(NULL));
    printf("短进程优先模拟\n\n");
    HRRNorSPN(1); //(0)高响应优先或者最短进程优先(1)
    printf("\n\n\n\n\n");
    printf("高响应优先模拟\n\n");
    HRRNorSPN(0); //(0)高响应优先或者最短进程优先(1)
    printf("\n\n\n\n\n");
    printf("时间片轮转模拟\n\n");
    RR(); //时间片轮转
    printf("\n\n\n\n\n");
    printf("银行家算法模拟\n\n");
    RR_With_Resource();
}
