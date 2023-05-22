// #include  <stdio.h>
// #include  <sys/types.h>
// #include  <stdlib.h>
// #include  <sys/stat.h>
// #include <fcntl.h>
// #include  <error.h>
// #include  <wait.h>
// #include  <unistd.h>

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unistd.h>

#define BUFFER_SIZE 8 // 缓冲区大小
using namespace std;

vector<int> buffer(BUFFER_SIZE); // 缓冲区
queue<int> pQueue;				 // 生产者等待队列
queue<int> cQueue;				 // 消费者等待队列
int number;						 // 0-99
int writeptr, readptr;			 // 写指针　读指针
int consumer, producer;			 // 消费者等待数量　生产者等待数量

// 初始化
void init()
{
	for (int i = 0; i < BUFFER_SIZE; ++i)
	{
		buffer[i] = 0;
	}
	while (!pQueue.empty())
	{
		pQueue.pop();
	}
	while (!cQueue.empty())
	{
		cQueue.pop();
	}
	number = 0;
	writeptr = readptr = 0;
	consumer = producer = 0;
}

// 查找空位
int findEmpty(vector<int> &v, int len)
{
	for (int i = 0; i < len; ++i)
	{
		if (v[i] == 0)
		{
			return i;
		}
	}
	return -1;
}

// 查找脏位
int findDirty(vector<int> &v, int len)
{
	for (int i = 0; i < len; ++i)
	{
		if (v[i] > 0)
		{
			return i;
		}
	}
	return -1;
}

// 缓冲区是否满
bool isFull(vector<int> &v, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (v[i] == 0)
		{
			return false;
		}
	}
	return true;
}

// 缓冲区是否空
bool isEmpty(vector<int> &v, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (v[i] != 0)
		{
			return false;
		}
	}
	return true;
}

// 遍历展示队列中的内容
void showQueue(queue<int> q)
{
	int len = q.size();
	for (int i = 0; i < len; i++)
	{
		cout << q.front() << "	";
		q.pop();
	}
}

// 显示缓冲区内容
void show()
{
	cout << "_________________________________________";
	cout << "________________________________________________" << endl;
	for (int i = 0; i < BUFFER_SIZE; ++i)
	{
		if (buffer[i] > 0)
		{
			printf("|   %3d    ", buffer[i]);
			// cout << "|\t" << buffer[i] << "    ";
		}
		else
		{
			printf("|          ");

			// cout << "|\t";
			// cout << "    ";
		}
	}
	cout << "|\n";
	cout << "_________________________________________";
	cout << "________________________________________________" << endl;

	cout << "\n写指针 = " << writeptr << "\t读指针 = " << readptr << "\t";

	// 打印生产者
	if (producer)
	{
		cout << "\n生产者等待 : " << producer << "\n";
		cout << "生产者等待队列: ";
		showQueue(pQueue);
	}
	else
	{
		cout << "\n生产者 ready\t";
	}

	// 打印消费者
	if (consumer)
	{
		cout << "\n消费者等待 : " << consumer << "\n";
		// 不用显示cQueue里面的数值，事实上，里面的number数值不增加
		//  cout << "cQueue: ";
		//  showQueue(cQueue);
	}
	else
	{
		cout << "\n消费者 ready";
	}
	cout << endl;
}

// 生产
void produce()
{
	if (findEmpty(buffer, BUFFER_SIZE) == -1)
	{
		// 缓冲区满,阻塞
		number = number + 1;
		producer++;
		pQueue.push(number);
	}
	else if (isEmpty(buffer, BUFFER_SIZE))
	{
		// 缓冲区空
		if (cQueue.empty())
		{
			// 消费者等待队列为空,直接写入即可
			number = number + 1;
			buffer[writeptr] = number;
		}
		else
		{
			// 有阻塞的消费者，先写入再读出
			number = number + 1;
			buffer[writeptr] = number;
			show(); // 不仅在produce函数运行全部完成后show，在中间就show一次
			sleep(2);
			buffer[readptr] = 0;
			readptr = (readptr + 1) % BUFFER_SIZE;
			consumer--;
			cQueue.pop();
		}
		writeptr = (writeptr + 1) % BUFFER_SIZE;
	}
	else
	{
		// 缓冲区不空也不满,直接写入即可
		number = number + 1;
		buffer[writeptr] = number;
		writeptr = (writeptr + 1) % BUFFER_SIZE;
	}
}

// 消费
void consume()
{
	if (findDirty(buffer, BUFFER_SIZE) == -1) // buffer里面全都是0
	{
		// 缓冲区空,则阻塞
		// ++number;但事实上不能让number++，不然number的数值就变了，
		consumer++;
		cQueue.push(number);
	}
	else if (isFull(buffer, BUFFER_SIZE))
	{
		// 缓冲区满
		if (!pQueue.empty())
		{
			// 生产者等待队列不空，则先读出再写入
			buffer[readptr] = 0;
			readptr = (readptr + 1) % BUFFER_SIZE;
			show(); // 不仅在consume函数运行全部完成后show，在中间就show一次
			sleep(2);
			buffer[writeptr] = pQueue.front();
			writeptr = (writeptr + 1) % BUFFER_SIZE;
			pQueue.pop();
			producer--;
		}
		else
		{
			// 生产者队列为空
			buffer[readptr] = 0;
			readptr = (readptr + 1) % BUFFER_SIZE;
		}
	}
	else
	{
		// 缓冲区不空也不满,直接读取即可
		buffer[readptr] = 0;
		readptr = (readptr + 1) % BUFFER_SIZE;
	}
}

int main()
{
	cout << "p-生产    c-消费    e-退出" << endl;

	init();
	char c;
	cin >> c;
	while (c)
	{
		if (c == 'e')
		{
			break;
		}
		else if (c == 'p')
		{
			produce();
		}
		else if (c == 'c')
		{
			consume();
		}
		else
		{
			cout << "Please enter p-生产    c-消费    e-退出" << endl;
		}
		show();
		cin >> c;
	}
	return 0;
}
