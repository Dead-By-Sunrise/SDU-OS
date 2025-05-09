/*
* Filename : Myvmrp.cc
* copyright : Unalome_3301
* Function : 模拟虚拟内存页置换算法的程序
*/
#include "Myvmrp.h"

Replace::Replace()
{
    int i;

    //设定总得访问页数,并分配相应的引用页号和淘汰页号记录数组空间
    cout << "Please input page numbers :";
    cin >> PageNumber;
    ReferencePage = new int[sizeof(int) * PageNumber];
    EliminatePage = new int[sizeof(int) * PageNumber];

    //输入引用页号序列(页面走向),初始化引用页数组
    cout << "Please input reference page string :";
    for (i = 0; i < PageNumber; i++)
        cin >> ReferencePage[i]; //引用页暂存引用数组

    //设定内存实页数(帧数),并分配相应的实页号记录数组空间(页号栈)
    cout << "Please input page frames :";
    cin >> FrameNumber;
    PageFrames = new int[sizeof(int) * FrameNumber];
    Referencebit = new int[sizeof(int) * FrameNumber];
    count = new int[sizeof(int) * FrameNumber];
    Modificationbit = new int[sizeof(int) * FrameNumber];
}

Replace::~Replace(){
}

void Replace::InitSpace(char * MethodName)
{
    int i;

    cout << endl << MethodName << endl;
    FaultNumber = 0;
    //引用还未开始,-1表示无引用页
    for (i = 0; i < PageNumber; i++)
        EliminatePage[i] = -1;
    for(i = 0; i < FrameNumber; i++)
        PageFrames[i] = -1;
}

//分析统计选择的算法对于当前输入的页面走向的性能
void Replace::Report(void)
{
    //报告淘汰页顺序
    cout << endl << "Eliminate page:";
    for(int i=0; EliminatePage[i]!=-1; i++)
        cout << EliminatePage[i] << " ";

    //报告缺页数和缺页率
    cout << endl << "Number of page faults = " << FaultNumber << endl;
    cout << setw(6) << setprecision(3) ;
    cout << "Rate of page faults = " << 100*(float)FaultNumber/(float)PageNumber <<
    "%" <<endl;
}

//最近最旧未用置换算法
void Replace::Lru(void)
{
    int i,j,k,l,next;

    InitSpace("LRU");
    //循环装入引用页
    for(k = 0, l = 0; k < PageNumber; k++) {
        next = ReferencePage[k];
        //检测引用页当前是否已在实存
        for(i = 0; i < FrameNumber; i++) {
            if(next == PageFrames[i]) {
                //引用页已在实存将其调整到页记录栈顶
                next = PageFrames[i];
                for(j = i; j > 0; j--)
                    PageFrames[j] = PageFrames[j-1];
                PageFrames[0]=next;
                break;
            }
        }

        if(PageFrames[0] == next) {
            //如果引用页已放栈顶，则为不缺页，报告当前内存页号
            for(j = 0; j < FrameNumber; j++)
                if(PageFrames[j]>=0)
                    cout << PageFrames[j] << " ";
            cout << endl;
            continue; //继续装入下一页
        }
        else
            // 如果引用页还未放栈顶，则为缺页，缺页数加1
            FaultNumber++;

        //栈底页号记入淘汰页数组中
        EliminatePage[l] = PageFrames[FrameNumber-1];
        //向下压栈
        for(j = FrameNumber - 1; j > 0; j--)
            PageFrames[j] = PageFrames[j - 1];
        PageFrames[0]=next; //引用页放栈顶
        //报告当前实存中页号
        for(j = 0; j < FrameNumber; j++)
            if(PageFrames[j] >= 0)
                cout << PageFrames[j] << " ";

        //报告当前淘汰的页号
        if(EliminatePage[l] >= 0)
            cout << "->" << EliminatePage[l++] << endl;
        else
           cout << endl;
    }

    //分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}

//先进先出置换算法
void Replace::Fifo(void)
{
    int i,j,k,l,next;

    InitSpace("FIFO");
    //循环装入引用页
    for(k = 0,j = l = 0; k < PageNumber; k++) {
        next = ReferencePage[k];
        //如果引用页已在实存中，报告实存页号
        for(i = 0; i < FrameNumber; i++)
            if(next == PageFrames[i])
                break;

        if(i < FrameNumber) {
            for(i = 0; i < FrameNumber; i++)
                if(PageFrames[i] >= 0)
                    cout << PageFrames[i] << " ";
            cout << endl;
            continue; // 继续引用下一页
        }

        //引用页不在实存中，缺页数加1
        FaultNumber++;
        EliminatePage[l] = PageFrames[j]; //最先入页号记入淘汰页数组
        PageFrames[j] = next; //引用页号放最先入页号处
        j = (j + 1) % FrameNumber; //最先入页号循环下移

        //报告当前实存页号和淘汰页号
        for(i = 0; i < FrameNumber; i++)
            if(PageFrames[i] >= 0)
                cout << PageFrames[i] << " ";

        if(EliminatePage[l] >= 0)
            cout << "->" << EliminatePage[l++] << endl;
        else
            cout << endl;
    }

    //分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}

//时钟(二次机会)置换算法
void Replace::Clock(void)
{
    int i,j,k,l,next;

    InitSpace("Clock");
    for(k = 0, j = l = 0; k < PageNumber; k++){
        next = ReferencePage[k];
        //检测引用页当前是否已在实存
        for(i = 0; i < FrameNumber; i++){
            if(next == PageFrames[i]){
                // 引用位设为1
                Referencebit[i] = 1;
                break;
            }
        }
        if(i < FrameNumber){
            for(i = 0; i < FrameNumber; i++){
                cout << PageFrames[i] << " ";
            }
            cout << endl;
            continue;
        }

        //首次扫描：找出引用位和修改位都为0的页面
        bool found = false;
        for(int tmp = 0; tmp < FrameNumber; tmp++){
            if(Referencebit[j] == 0 && Modificationbit[j] == 0){
                found = true;
                break;
            }
            if(Referencebit[j] == 1){
                Referencebit[j] = 0;
            }
            j = (j + 1) % FrameNumber;
        }

        //若未发现，再次扫描找引用位为0的页面
        if(!found){
            for(int scan = 0; scan < FrameNumber; scan++){
                if(Referencebit[j]==0){
                    found = true;
                    break;
                }
                j = (j + 1) % FrameNumber;
            }
        }

        //仍未找到，则任意替换当前页面
        if(!found){
            j = (j + 1) % FrameNumber;
        }


        //引用页不在实存中，缺页数加1
        FaultNumber++;
        EliminatePage[l] = PageFrames[j]; //最先入页号记入淘汰页数组
        PageFrames[j] = next; //引用页号放最先入页号处
        Referencebit[j] = 1;  //引用位置1
        Modificationbit[j] = 0; //修改位置0
        j = (j + 1) % FrameNumber; //最先入页号循环下移


        //报告当前实存页号和淘汰页号
        for(i = 0; i < FrameNumber; i++){
            if(PageFrames[i] >= 0){
                cout << PageFrames[i] << " ";
            }
        }
        if(EliminatePage[l] >= 0){
            cout << "->" << EliminatePage[l++] << endl;
        }
        else{
            cout << endl;
        }
    }
    Report();
}

//增强二次机会置换算法
void Replace::Eclock (void)
{
    int i,j,k,l,next;

    InitSpace("EClock");
    for(k = 0, j = l = 0; k < PageNumber; k++){
        next = ReferencePage[k];
        //检测引用页当前是否已在实存
        for(i = 0; i < FrameNumber; i++){
            if(next == PageFrames[i]){
                // 引用位设为1
                Referencebit[i] = 1;
                break;
            }
        }
        if(i < FrameNumber){
            for(i = 0; i < FrameNumber; i++){
                cout << PageFrames[i] << " ";
            }
            cout << endl;
            continue;
        }

        //如果引用位为1，则判断后重新置0
        while(Referencebit[j] == 1){
            Referencebit[j] = 0;
            j = (j + 1) % FrameNumber;
        }

        //引用页不在实存中，缺页数加1
        FaultNumber++;
        EliminatePage[l] = PageFrames[j]; //最先入页号记入淘汰页数组
        PageFrames[j] = next; //引用页号放最先入页号处
        Referencebit[j] = 1;  //引用页号置1
        j = (j + 1) % FrameNumber; //最先入页号循环下移


        //报告当前实存页号和淘汰页号
        for(i = 0; i < FrameNumber; i++){
            if(PageFrames[i] >= 0){
                cout << PageFrames[i] << " ";
            }
        }
        if(EliminatePage[l] >= 0){
            cout << "->" << EliminatePage[l++] << endl;
        }
        else{
            cout << endl;
        }
    }
    Report();
}

//最不经常使用置换算法
void Replace::Lfu(void)
{
    int i,k,l,next;

    InitSpace("Lfu");
    for(k = 0, l = 0; k < PageNumber; k++){
        next = ReferencePage[k];
        //检测引用页当前是否已在实存
        for(i = 0; i < FrameNumber; i++){
            if(next == PageFrames[i]){
                count[i]++; //累计使用次数
                break;
            }
        }
        if(i < FrameNumber){
            for(i = 0; i < FrameNumber; i++){
                cout << PageFrames[i] << " ";
            }
            cout << endl;
            continue;
        }

        //引用页不在实存中，缺页数加1
        FaultNumber++;

        int idx = 0;
        for(i = 0; i < FrameNumber; i++){
            if(count[i] < count[idx]){
                idx = i;
            }
        }

        EliminatePage[l] = PageFrames[idx]; //索引页号记入淘汰页数组
        PageFrames[idx] = next; //引用页号放入索引页号处
        count[idx] = 1;  //累加器重新计数
        //j = (j + 1) % FrameNumber; //页号循环下移


        //报告当前实存页号和淘汰页号
        for(i = 0; i < FrameNumber; i++){
            if(PageFrames[i] >= 0){
                cout << PageFrames[i] << " ";
            }
        }
        if(EliminatePage[l] >= 0){
            cout << "->" << EliminatePage[l++] << endl;
        }
        else{
            cout << endl;
        }
    }
    Report();
}

//最经常使用置换算法
void Replace::Mfu(void)
{
    int i,k,l,next;

    InitSpace("Mfu");
    for(i = 0; i < FrameNumber; i++){
        count[i] = 1;
    }
    for(k = 0, l = 0; k < PageNumber; k++){
        next = ReferencePage[k];
        //检测引用页当前是否已在实存
        for(i = 0; i < FrameNumber; i++){
            if(next == PageFrames[i]){
                count[i]++; //累计使用次数
                break;
            }
        }
        if(i < FrameNumber){
            for(i = 0; i < FrameNumber; i++){
                cout << PageFrames[i] << " ";
            }
            cout << endl;
            continue;
        }

        //引用页不在实存中，缺页数加1
        FaultNumber++;

        int idx = 0;
        for(i = 0; i < FrameNumber; i++){
            if(count[i] > count[idx]){
                idx = i;
            }
        }

        EliminatePage[l] = PageFrames[idx]; //索引页号记入淘汰页数组
        PageFrames[idx] = next; //引用页号放入索引页号处
        count[idx] = 0;  //累加器重新计数
        //j = (j + 1) % FrameNumber; //页号循环下移


        //报告当前实存页号和淘汰页号
        for(i = 0; i < FrameNumber; i++){
            if(PageFrames[i] >= 0){
                cout << PageFrames[i] << " ";
            }
        }
        if(EliminatePage[l] >= 0){
            cout << "->" << EliminatePage[l++] << endl;
        }
        else{
            cout << endl;
        }
    }
    Report();
}

int main(int argc, char *argv[])
{
    Replace * vmrp = new Replace();
    vmrp->Fifo();
    vmrp->Lru();
    vmrp->Clock();
    vmrp->Eclock();
    vmrp->Lfu();
    vmrp->Mfu();
    return 0;
}