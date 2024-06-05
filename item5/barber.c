#include "ipc.h"

int main(int argc,char *argv[]){
    // 伪随机数生成器
    srand(time(NULL));

    Msg_buf msg_arg;

    // msqid_ds：存储消息队列状态的结构体
    struct msqid_ds msg_sofa;
    struct msqid_ds msg_wait;

    // 创建消息队列
    q_flg = IPC_CREAT | 0644;
    // 建立沙发消息队列
    q_sofa_key = 100;
    q_sofa_id = set_msq(q_sofa_key,q_flg);
    // 建立等候室消息队列
    q_wait_key = 200;
    q_wait_id = set_msq(q_wait_key,q_flg);

    // 创建信号量
    sem_flg = IPC_CREAT | 0644;
    // 建立一个同步顾客信号量
    s_customer_key = 300;
    s_customer_val = 0;
    s_account_sem = set_sem(s_customer_key,s_customer_val,sem_flg);
    // 建立一个互斥帐本信号量
    s_account_key = 400;
    s_account_val = 1;
    s_account_sem = set_sem(s_account_key,s_account_val,sem_flg);

    // 建立三个理发师子进程
    int pid[3];
    int i;
    for(int i=0;i<3;i++){
        pid[i] = fork();
        if(pid[i]==0){
            while(1){
                // 获取消息队列状态信息
                msgctl(q_sofa_id,IPC_STAT,&msg_sofa);

                if(msg_sofa.msg_qnum == 0){  // 消息数为0
                    printf("%d 号理发师睡眠\n",getpid());
                }

                // 以阻塞式从沙发队列接收一条消息
                msgrcv(q_sofa_id,&msg_arg,sizeof(msg_arg),SOFA,0);
                // 唤醒顾客进程
                up(s_customer_sem);
                printf("%d 号理发师给 %d 号顾客理发\n",getgid(),msg_arg.mid);
                // 用进程休眠一个随机时间模拟理发过程
                sleep(rand()%5+10);
                // sleep(3);
                // 若有理发师正在收钱，则阻塞
                down(s_account_sem);
                printf("%d 号理发师向 %d 号顾客收费\n",getpid(),msg_arg.mid);
                up(s_account_sem);
            }
        }
    }

    // 程序正常退出
    return EXIT_SUCCESS;
}