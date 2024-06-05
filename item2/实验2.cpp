#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

// f(x)=x!
int fx(int x){
    if(x==1) return 1;
    else return fx(x-1)*x;
}

// f(y) is Fibonacci
int fy(int y){
    if(y==1 || y==2) return 1;
    else return fy(y-1)+fy(y-2);
}

int main(){
    int x,y;
    printf("请输入x和y的值:");
    scanf("%d %d", &x,&y);

    int pipefd[2]; // 定义数组用于存放管道的文件描述符
    if(pipe(pipefd)<0){
        perror("创建管道失败");
        exit(EXIT_FAILURE);
    }

    pid_t pid_fx=fork(); // 创建子进程
    if(pid_fx==0){
        // 子进程1：计算f(x)
        close(pipefd[0]); // 关闭管道读端
        int result_fx=fx(x); // 调用fx函数计算
        write(pipefd[1],&result_fx,sizeof(result_fx)); // 结果写入管道
        printf("f(x)=%d\n",result_fx);
        close(pipefd[1]); // 写完数据立即关闭
        exit(EXIT_SUCCESS); // 子进程正常退出
    }

    pid_t pid_fy=fork();
    if(pid_fy==0){
        // 子进程2：计算f(y)
        close(pipefd[0]);
        int result_fy=fy(y);
        write(pipefd[1],&result_fy,sizeof(result_fy));
        printf("f(y)=%d\n",result_fy);
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }

    close(pipefd[1]); // 父进程关闭管道写端
    int result_fx, result_fy;
    read(pipefd[0],&result_fx,sizeof(result_fx)); // 从管道读取f(x)结果
    read(pipefd[0],&result_fy,sizeof(result_fy)); // 从管道读取f(y)结果
    close(pipefd[0]); // 关闭读端
    int result_fxy = result_fx+result_fy; // 计算f(x,y)
    printf("f(x,y)=%d\n",result_fxy); // 输出最终结果

    wait(NULL); // 等待所有进程结束
    return EXIT_SUCCESS;
}