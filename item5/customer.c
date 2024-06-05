#include "ipc.h"

int main(int argc,char *argv[]){
    srand(time(NULL));
    Msg_buf msg_arg;

    struct msqid_ds msg_sofa;
    struct msqid_ds msg_wait;

    q_flg = IPC_CREAT | 0644;
    q_sofa_key = 100;
    q_sofa_id = set_msq(q_sofa_key,q_flg);

    q_wait_key = 200;
    q_wait_id = set_msq(q_wait_key,q_flg);

    sem_flg = IPC_CREAT | 0644;
    s_customer_key = 300;
    s_customer_val = 0;
    s_customer_sem = set_sem(s_customer_key,s_customer_val,sem_flg);

    s_account_key = 400;
    s_account_val = 1;
    s_account_sem = set_sem(s_account_key,s_account_val,sem_flg);

    // 顾客序号
    int customernum = 1;

    while(1){
        msgctl(q_sofa_id,IPC_STAT,&msg_sofa);  // 阻塞式读取msgid_ds结构填充入缓冲区

        // 沙发有空位
        if(msg_sofa.msg_qnum < 4){ 
            quest_flg = IPC_NOWAIT;

            // 等待室有人
            if(msgrcv(q_wait_id,&msg_arg,sizeof(msg_arg),WAIT,quest_flg) >= 0){ 
                msg_arg.mtype = SOFA;
                printf("%d 号顾客从等待室坐入沙发\n",msg_arg.mid);
                msgsnd(q_sofa_id,&msg_arg,sizeof(msg_arg),IPC_NOWAIT);
            }
            // 等待室无人
            else{  
                msg_arg.mtype = SOFA;
                msg_arg.mid = customernum;
                customernum++;
                printf("%d 号顾客从外面坐入沙发\n",msg_arg.mid);
                msgsnd(q_sofa_id,&msg_arg,sizeof(msg_arg),IPC_NOWAIT);
            }

        }
        // 沙发满员
        else{
            msgctl(q_wait_id,IPC_STAT,&msg_wait);

            // 等待室有空位
            if(msg_wait.msg_qnum < 13){  
                msg_arg.mtype = WAIT;
                msg_arg.mid = customernum;
                customernum++;
                printf("沙发座满,%d 号新顾客进入等候室\n",msg_arg.mid);
                msgsnd(q_wait_id,&msg_arg,sizeof(msg_arg),IPC_NOWAIT);
            }
            // 等待室满员
            else{
                printf("等候室满,%d 号新顾客没有进入理发店\n",customernum);
                down(s_customer_sem);
            }
        }

        // 用进程休眠模拟顾客随机到达时间
        sleep(rand()%5+1); 
        // sleep(3);
    }
    
    return EXIT_SUCCESS;
}