#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>

int flag = 0;

void add(int sig){
    flag++;
}
void minus(int sig){
    flag--;
}

int main(){
    // 设置信号处理函数
    signal(SIGINT,add); // Ctrl+C
    signal(SIGTSTP,minus);  // Ctrl+Z

    int pid = fork();

    if(pid == 0){   // 执行子进程
        while(1){
            setpriority(PRIO_PROCESS,pid,flag); // 按进程ID指定优先级
            printf("child pid = %d\n",pid);  // 进程ID
            printf("child process priority = %d\n",getpriority(PRIO_PROCESS,pid));  // 优先级
            printf("child process scheduler = %d\n",sched_getscheduler(pid)); // 调度策略
            printf("\n");
            sleep(2);
        }
    }
    else if(pid > 0){   // 执行父进程
        while(1){
            setpriority(PRIO_PROCESS,getpid(),flag); // 按进程ID指定优先级
            printf("father pid = %d\n",getpid());  // 进程ID
            printf("father process priority = %d\n",getpriority(PRIO_PROCESS,getpid()));  // 优先级
            printf("father process scheduler = %d\n",sched_getscheduler(getpid())); // 调度策略
            printf("\n");
            sleep(2);
        }
    }
}