#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <wait.h>
#include <unistd.h>
#include <sys/select.h>
#include <semaphore.h>
#include <deque>
#include <vector>
#include <algorithm>
#include <time.h>

class Page_Frame
{
public:
    int pageframe_size;                    // 内存块数
    int occupied_size;                     // 被使用的内存块数，如果没满，直接加入就好了
    std::vector<int> pageframe_array;      // 内存块存放的访问页面的序号组成的数组
    std::vector<int> pageframe_array_time; // LRU里面的判断最久的依据

    // 初始化
    Page_Frame(int m = 0, int n = 0)
    {
        pageframe_size = m;
        occupied_size = 0;
    }
    // 输出信息的函数
    void print_info()
    {
        for (int i = 0; i < pageframe_size; i++)
        {
            if (i < pageframe_array.size()) // 内存块还没有全部被页面序号站满
            {
                printf("- Page_Frame[%d]: %4d\t", i, pageframe_array[i]);
            }
            else
            {
                printf("- Page_Frame[%d]: None\t", i); // N代表内存块为空
            }
        }
        printf("\n");
    }

    // pageframe_array_time是当下时刻的内存块存放的访问页面的序号组成的数组
    void add_pageframe_array_time()
    {
        // iter就是指针
        for (auto iter = pageframe_array_time.begin(); iter != pageframe_array_time.end(); iter++)
        {
            (*iter)++;
            /*
            (*iter)++ 表示先解引用 iter 获取到指向的元素，再对该元素进行自增操作，最后返回该元素自增前的值。
            这种写法会修改数组元素的值，但不会修改迭代器 iter 的指向。
            *iter++ 表示先解引用 iter 获取到指向的元素，再将迭代器向后移动一个位置。
            该写法会修改迭代器 iter 的指向，但不会修改数组元素的值。
            */
        }
    }
};
// 实现FIFO算法
void FIFO(int memory_blocks_number, int choice = 0)
{
    printf("%d\n",choice);
    printf("\nFIFO的内存块数： %d 块\n", memory_blocks_number);
    // 这里没有随机化数组元素，之后再加
    /*
    total_instructions为总的页面访问次数
    diseffect为缺页数
    pageid为想访问的页面的序号
    */
    int total_instructions = 12;
    srand((unsigned)time(NULL));

    int Acess_Series[total_instructions] = {3, 2, 1, 0, 3, 2, 4, 3, 2, 1, 0, 4};
    if (choice == 1)
    {
        for (int i = 0; i < total_instructions; i++)
        {
            Acess_Series[i] = rand() % 10;
        }
    }
    // srand((unsigned)time(NULL));

    // 3
    Page_Frame page_Frame(memory_blocks_number);
    std::deque<int> fifo_queue; // 双段队列
    int diseffect = 0;

    for (int i = 0; i < total_instructions; ++i)
    {
        int pageid = Acess_Series[i];
        printf("访问页面序号：%d ", pageid);
        /*
        对Acess_Series数组里面的数字，判断以下三种情况
        1 内存块中存在此页面,什么都不用做
        2 内存块中无此页面，且内存块未满，直接加入
        3 内存块中无此页面，且内存块已满，取出队列首个元素（FIFO算法）
        */
        // find返回的是指针，指向找序列里面第一个出现的地方
        // 存在此页面
        if (find(page_Frame.pageframe_array.begin(), page_Frame.pageframe_array.end(), pageid) != page_Frame.pageframe_array.end())
        {
            page_Frame.print_info();
        }
        else
        {
            // 无此页面，缺页数先加一
            diseffect++;
            fifo_queue.push_back(pageid);
            // 同时内存快未满
            if (page_Frame.occupied_size < page_Frame.pageframe_size)
            // if (page_Frame.pageframe_array.size() < page_Frame.pageframe_size)
            {
                // 直接加入
                page_Frame.occupied_size++;
                page_Frame.pageframe_array.push_back(pageid);
                page_Frame.print_info();
            }
            else
            {
                // 同时内存块已满
                int be_replaced_pageid = fifo_queue.front();
                fifo_queue.pop_front();
                auto be_replaced_pageid_iter = find(page_Frame.pageframe_array.begin(), page_Frame.pageframe_array.end(), be_replaced_pageid);
                *be_replaced_pageid_iter = pageid;
                page_Frame.print_info();
            }
        }
    }
    printf("缺页数：%d  缺页率：%f  命中率：%f\n", diseffect, (float)diseffect / total_instructions, (1 - (float)diseffect / total_instructions));
    printf("\n--------------------------------分割线-------------------------------\n");
}

void LRU()
{
    int total_instructions = 20;

    int Acess_Series[total_instructions] = {1, 8, 1, 7, 8, 2, 7, 2, 1, 8, 3, 8, 2, 1, 3, 1, 7, 1, 3, 7};
    for (int i = 0; i < total_instructions; i++)
    {
        Acess_Series[i] = rand() % 10;
    }

    Page_Frame page_Frame(4); // 是一个数组
    int diseffect = 0;        // 缺页数
    for (int i = 0; i < total_instructions; i++)
    {
        int pageid = Acess_Series[i];
        printf("访问页面序号：%d ", pageid);
        /*
       对Acess_Series数组里面的数字，判断以下三种情况
       1 内存块中存在此页面,则把其次数置为0即可
       2 内存块中无此页面，且内存块未满，直接加入
       3 内存块中无此页面，且内存块已满，找到最近最久未使用的页面
       */
        std::vector<int>::iterator iter;
        if ((iter = find(page_Frame.pageframe_array.begin(), page_Frame.pageframe_array.end(), pageid)) != page_Frame.pageframe_array.end())
        {
            page_Frame.add_pageframe_array_time();
            int position = std::distance(page_Frame.pageframe_array.begin(), iter);
            page_Frame.pageframe_array_time.at(position) = 0; // 新出现一次，就置成0,越大代表越久没有访问，0代表刚刚访问
            page_Frame.print_info();
        }
        else
        {
            // 内存中无此页面,缺页数加一
            diseffect++;
            // 且内存块未满，直接加入
            if (page_Frame.occupied_size < page_Frame.pageframe_size)
            {
                page_Frame.add_pageframe_array_time();
                page_Frame.occupied_size++;
                page_Frame.pageframe_array.push_back(pageid);
                page_Frame.pageframe_array_time.push_back(0); // 0代表刚刚访问
                page_Frame.print_info();
            }
            else
            {
                // 且内存块已满，找到最近最久未使用的页面,也就之找到times次数最大的数组下标
                page_Frame.add_pageframe_array_time();
                auto max_iter = std::max_element(page_Frame.pageframe_array_time.begin(), page_Frame.pageframe_array_time.end());
                int max_position = std::distance(page_Frame.pageframe_array_time.begin(), max_iter);
                page_Frame.pageframe_array.at(max_position) = pageid;
                page_Frame.pageframe_array_time.at(max_position) = 0;
                page_Frame.print_info();
            }
        }
    }
    printf("缺页数：%d  缺页率：%f  命中率：%f\n", diseffect, (float)diseffect / total_instructions, (1 - (float)diseffect / total_instructions));
    printf("\n--------------------------------分割线-------------------------------\n");
}
int main()
{

    // srand((unsigned)time(NULL));

    // FIFO(3);
    // FIFO(4);
    // printf("_____________________________");
    // LRU();

    sem_t *mutex;
    // create mutex semaphore
    if ((mutex = sem_open("/1mutex_semaphore", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
    {
        perror("mutex_semphore error");
        exit(1);
    }

    pid_t pid;
    for (int childprocess = 1; childprocess <= 2; childprocess++)
    {
        while ((pid = fork()) == -1)
            ;
        if (pid == 0)
        {
            // printf("%d %d %d\n", childprocess, getpid(), getppid());
            if (childprocess == 1) // 子进程1
            {
                sem_wait(mutex);
                printf("FIFO子进程\n");
                int n;
                printf("是否选择要看Belady异常 1-看 2-不看");
                scanf("%d", &n);
                if (n == 1)
                {
                    FIFO(3);
                    FIFO(4);
                }
                else
                {
                    srand((unsigned)time(NULL));
                    FIFO(4, 1);
                }

                sem_post(mutex);
            }

            else // 子进程2
            {
                sem_wait(mutex);
                printf("LRU子进程\n");
                srand((unsigned)time(NULL));
                LRU();
                sem_post(mutex);
            }
            exit(0);
        }
        else
            wait(NULL);
    }
    // delete semaphore
    if (sem_unlink("/1mutex_semaphore") == -1)
    {
        perror("semaphore unlink error");
        exit(1);
    }
    return 0;
}