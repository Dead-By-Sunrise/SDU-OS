#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h> // 包含处理目录的函数定义（仅类Unix系统可用）

void list_files() {
    DIR *dir; // 目录流类型，用于读取目录
    struct dirent *entry;  // 存储读取到的每个目录项的信息
    
    if((dir=opendir("."))==NULL){ // 尝试打开当前目录，检查是否打开成功
        perror("opendir() error");
    } else {
        puts("当前；目录文件夹列表：");
        while((entry=readdir(dir))!=NULL){ // 循环读取每个目录项
            printf("%s\n",entry->d_name);  // 打印目录项的名称
        }
        closedir(dir); // 完成目录读取后关闭目录流
    }
}

int main(){
    pid_t pid=fork();  // 创建子进程

    if(pid==-1){  // 错误处理：无法创建子进程
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if(pid==0){  // 子进程
        printf("子进程启动, PID = %d\n",getpid());

        // 子进程每隔3秒显示一次当前文件名列表
        while (1)
        {
            list_files();
            sleep(3); // 睡眠3秒 
        }
        
    } else {  // 父进程
        printf("父进程PID=%d; 子进程PID=%d\n",getpid(),pid);
        
        // 父进程等待子进程结束
        wait(NULL);
        printf("子进程已结束\n");
    }
    
    return EXIT_SUCCESS;
}