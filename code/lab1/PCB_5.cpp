// #include  <stdio.h>
// #include  <sys/types.h>
// #include  <stdlib.h>
// #include  <sys/stat.h>
// #include <fcntl.h>
// #include  <error.h>
// #include  <wait.h>
// #include  <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

typedef int ElemType;
typedef struct pcb
{
    ElemType id;
    struct pcb *next;
} pcb;

pcb *NewQueue, *ReadyQueue, *RunningQueue, *BlockedQueue, *ExitQueue;

// 创建带头结点的ReadyQueue队列,尾插法
pcb *create()
{
    pcb *head = new pcb;
    head->next = NULL;
    pcb *q = head;
    int n;
    cout << "Enter the number of processes:\n";
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        pcb *p = new pcb;
        cin >> p->id;
        p->next = NULL;
        q->next = p;
        q = p;
    }
    return head;
}

// 插入结点
// 在尾部插入新的节点，就是从队列尾部入队的操作
void insert(pcb *head, pcb *node)
{
    pcb *p = head;
    while (p->next)
    {
        p = p->next;
    }
    pcb *n = new pcb;
    n->id = node->id;
    p->next = n;
    n->next = NULL;
}

// 删除结点
// 删除头结点后面的第一个结点，就是从队列头部出队的操作
void del(pcb *head)
{
    pcb *p = head->next;
    if (p)
    {
        head->next = head->next->next;
        delete p;
    }
}

// 展示各个状态的进程
void show()
{
    pcb *p;

    p = NewQueue->next;
    cout << "NewQueue:\t";
    while (p)
    {
        cout << p->id << " ";
        p = p->next;
    }
    cout << endl;

    p = ReadyQueue->next;
    cout << "ReadyQueue:\t";
    while (p)
    {
        cout << p->id << " ";
        p = p->next;
    }
    cout << endl;

    p = RunningQueue->next;
    cout << "RunningQueue:\t";
    while (p)
    {
        cout << p->id << " ";
        p = p->next;
    }
    cout << endl;

    p = BlockedQueue->next;
    cout << "BlockedQueue:\t";
    while (p)
    {
        cout << p->id << " ";
        p = p->next;
    }
    cout << endl;

    // p = ExitQueue->next;
    // cout << "ExitQueue:\t";
    // while (p)
    // {
    //     cout << p->id << " ";
    //     p = p->next;
    // }
    // cout << endl;
}

int main()
{
    NewQueue = create();
    ReadyQueue = new pcb;
    ReadyQueue->next = NULL;
    RunningQueue = new pcb;
    RunningQueue->next = NULL;
    BlockedQueue = new pcb;
    BlockedQueue->next = NULL;
    ExitQueue = new pcb;
    ExitQueue->next = NULL;
    // RunningQueue和BlockedQueue没有对第一个结点的id赋值，在insert（）之后，属于带头结点的队列，保持和ReadyQueue一样的结构
    show();
    int ChangeNum;
    cout << "Please choose one of the following actions" << endl;
    cout << "0:exit" << endl;
    cout << "1:Dispatch(ReadyQueue->RunningQueue)" << endl;
    cout << "2:Timeout(RunningQueue->ReadyQueue)" << endl;
    cout << "3:Event Wait(RunningQueue->BlockedQueue)" << endl;
    cout << "4:Event Occurs(BlockedQueue->ReadyQueue)" << endl;
    cout << "5:Admit(NewQueue->ReadyQueue)" << endl;
    cout << "6:Release(RunningQueue->Exit)" << endl;

    while (cin >> ChangeNum)
    {
        if (ChangeNum == 0)
        {
            break;
        }
        else if (ChangeNum == 1)
        {
            if (ReadyQueue->next)
            {
                if (RunningQueue->next)
                {
                    insert(BlockedQueue, RunningQueue->next);
                    del(RunningQueue);
                }
                insert(RunningQueue, ReadyQueue->next);
                del(ReadyQueue);
            }
            else
            {
                if (BlockedQueue->next)
                {
                    insert(BlockedQueue, RunningQueue->next);
                    del(RunningQueue);
                    insert(RunningQueue, BlockedQueue->next);
                    del(BlockedQueue);
                }
            }
        }
        else if (ChangeNum == 2)
        {
            if (RunningQueue->next)
            {
                if (ReadyQueue->next)
                {
                    insert(ReadyQueue, RunningQueue->next);
                    del(RunningQueue);
                    insert(RunningQueue, ReadyQueue->next);
                    del(ReadyQueue);
                }
            }
        }
        else if (ChangeNum == 3)
        {
            if (RunningQueue->next)
            {
                if (ReadyQueue->next)
                {
                    insert(BlockedQueue, RunningQueue->next);
                    del(RunningQueue);
                    insert(RunningQueue, ReadyQueue->next);
                    del(ReadyQueue);
                }
                else
                {
                    insert(BlockedQueue, RunningQueue->next);
                    del(RunningQueue);
                    insert(RunningQueue, BlockedQueue->next);
                    del(BlockedQueue);
                }
            }
        }
        else if (ChangeNum == 4)
        {
            if (BlockedQueue->next)
            {
                insert(ReadyQueue, BlockedQueue->next);
                del(BlockedQueue);
            }
        }
        else if (ChangeNum == 5)
        {
            if (NewQueue->next)
            {
                insert(ReadyQueue, NewQueue->next);
                del(NewQueue);
            }
        }
        else if (ChangeNum == 6)
        {
            if (RunningQueue->next)
            {
                insert(ExitQueue, RunningQueue->next);
                del(RunningQueue);
                // del(ExitQueue);释放处于结束状态的进程结点
                // 也可以直接释放RunnningQueue的结点
                if (ReadyQueue->next)
                {
                    insert(RunningQueue, ReadyQueue->next);
                    del(ReadyQueue);
                }
            }
        }
        else
        {
            cout << "You can only type in numbers in range(0~6)" << endl;
        }
        show();
        cout << "Please choose one of the following actions" << endl;
        cout << "0:exit" << endl;
        cout << "1:Dispatch(ReadyQueue->RunningQueue)" << endl;
        cout << "2:Timeout(RunningQueue->ReadyQueue)" << endl;
        cout << "3:Event Wait(RunningQueue->BlockedQueue)" << endl;
        cout << "4:Event Occurs(BlockedQueue->ReadyQueue)" << endl;
        cout << "5:Admit(NewQueue->ReadyQueue)" << endl;
        cout << "6:Release(RunningQueue->Exit)" << endl;
    }

    return 0;
}