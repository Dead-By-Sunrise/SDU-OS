#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>

#define BUFSZ 256

//建立或获取ipc的一组函数的原型说明
int get_ipc_id(char *proc_file, key_t key);
char *set_shm(key_t shm_key, int shm_num, int shm_flag);
int set_msq(key_t msq_key, int msq_flag);
int set_sem(key_t sem_key, int sem_val, int sem_flag);
int down(int sem_id);
int up(int sem_id);

/*信号灯控制用的共同体*/
typedef union semuns {
    int val;
} Sem_uns;

/* 消息结构体*/
typedef struct msgbuf {
    long mtype;
    char mtext[1];
} Msg_buf;



// 生产消费者共享的缓冲区和相关变量
key_t buff_key;  // 缓冲区键值
int buff_num;   // 缓冲区大小
char *buff_ptr;  // 指向缓冲区的指针

// 生产者放产品位置的共享指针
key_t pput_key;
int pput_num;
int *pput_ptr;

// 当前材料的共享指针
key_t mat_key;
int mat_num;
int *mat_ptr;

// 消费者取产品位置的共享指针
key_t cget_key;
int cget_num;
int *cget_ptr;

// 生产者有关的信号量
key_t prod_key;
key_t pmtx_key;
int prod_sem;
int pmtx_sem;
 
// 消费者有关的信号量
key_t cons_key;
key_t cmtx_key;
int cons_sem;
int cmtx_sem;

// 用于设置信号量和共享内存的标志和值
int sem_val;
int sem_flg;
int shm_flg;