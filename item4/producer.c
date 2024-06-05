#include "ipc.h"

// 获取材料的名称
int get_material(char c){
    if(c == 'A'){  // 获取烟草
        return 0;
    }
    else if(c == 'B'){  // 获取纸张
        return 1;
    }
    else if(c == 'C'){  // 获取胶水
        return 2;
    }
}

int main(int argc, char *argv[])
{
    int rate;

    // 可在在命令行第一参数指定一个进程睡眠秒数，以调解进程执行速度
    if(argv[1] != NULL) rate = atoi(argv[1]);
    else rate = 3; // 不指定为3秒

    // 共享内存使用的变量
    buff_key = 101; // 缓冲区任给的键值
    buff_num = 2;   // 缓冲区长度，两个材料
    pput_key = 102; // 生产者放产品指针的键值
    pput_num = 1;   // 指针数
    mat_key = 104;  // 当前材料的键值
    mat_num = 1;    // 指针数
    shm_flg = IPC_CREAT | 0644; // 共享内存读写权限

    // 获取缓冲区使用的共享内存，buff_ptr 指向缓冲区首地址
    buff_ptr = (char *)set_shm(buff_key, buff_num, shm_flg);
    // 获取生产者放产品位置指针 pput_ptr
    pput_ptr = (int *)set_shm(pput_key, pput_num, shm_flg);
    // 获取消费者取产品指针 mat_ptr
    mat_ptr = (int*)set_shm(mat_key,mat_num,shm_flg);
    
    // 信号量使用的变量
    prod_key = 201; // 生产者同步信号灯键值
    pmtx_key = 202; // 生产者互斥信号灯键值
    cons_key = 301; // 消费者同步信号灯键值
    cmtx_key = 302; // 消费者互斥信号灯键值
    sem_flg = IPC_CREAT | 0644;

    // 生产者同步信号灯初值设为缓冲区最大可用量
    sem_val = buff_num;
    // 获取生产者同步信号灯，引用标识存prod_sem
    prod_sem = set_sem(prod_key, sem_val, sem_flg);

    // 消费者初始无产品可取，同步信号灯初值设为0
    sem_val = 0;
    // 获取消费者同步信号灯，引用标识存cons_sem
    cons_sem = set_sem(cons_key, sem_val, sem_flg);

    // 生产者互斥信号灯初值为1
    sem_val = 1;
    // 获取生产者互斥信号灯，引用标识存pmtx_sem
    pmtx_sem = set_sem(pmtx_key, sem_val, sem_flg);

    // 三种材料，分别代表：A烟草、B纸、C胶水
    char material[3] = {'A', 'B', 'C'};
    char* material_name[3] = {"烟草", "纸", "胶水"};

    // 循环执行模拟生产者不断放产品
    while(1) {
        // 如果缓冲区满则生产者阻塞
        down(prod_sem);
        // 如果另一生产者正在放产品，本生产者阻塞
        down(pmtx_sem);

        // 用写一字符的形式模拟生产者放产品，报告本进程号和放入的字符及存放的位置
        buff_ptr[*pput_ptr] = material[*mat_ptr];
        printf("%d 生产者产出: %s to Buffer[%d]\n", getpid(), material_name[get_material(buff_ptr[*pput_ptr])], *pput_ptr);

        // 存放位置循环下移
        *pput_ptr = (*pput_ptr + 1) % buff_num;
        *mat_ptr = (*mat_ptr + 1) % 3;

        // 唤醒阻塞的生产者
        up(pmtx_sem);
        // 在放置完两个材料后，唤醒阻塞的消费者
        if(*pput_ptr == 0){ 
            up(cons_sem);
        }

        sleep(rate);
    }
    return EXIT_SUCCESS;
}