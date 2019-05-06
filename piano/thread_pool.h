#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#define MAX_WAITING_TASKS 1000
#define MAX_ACTIVE_THREADS 20
struct task
{
	void*(*do_task)(void*arg);//函数指针，指向任务要执行的函数
	void*arg;//任务执行时函数所带的参数
	struct task*next;
};
typedef struct thread_pool//线程池的头结点
{
	pthread_mutex_t mutex;//互斥锁，用来保护线程池的
	pthread_cond_t cond;//条件变量
	bool shutdown; //是否退出，线程销毁标记
	struct task*first;//指向第一个任务节点
	
	pthread_t *tids;//指向线程ID的数组
	
	unsigned int max_waiting_tasks;//最大执行的任务数
	unsigned char waiting_tasks;//待执行的任务数
	unsigned char active_threads;//正在服役的线程数
	
}thread_pool;

bool init_pool(thread_pool*pool,unsigned int threads_number);
bool add_task(thread_pool*pool,void*(*do_task)(void*arg),void*arg);
int add_thread(thread_pool*pool,unsigned char additional_threads);
int remove_thread(thread_pool*pool,unsigned char remove_threads);
bool destroy_pool(thread_pool*pool);

void *routine(void*arg);
#endif

