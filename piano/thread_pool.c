#include "thread_pool.h"
void handler(void*arg)
{
	pthread_mutex_unlock((pthread_mutex_t*)arg);
}
void *routine(void*arg)//任务执行函数
{
	thread_pool*pool = (thread_pool*)arg;//参数是线程池的头结点
	struct task*p;
	while(1)
	{
		//为了防止取消后死锁，注册一个处理例程handler
		pthread_cleanup_push(handler,(void*)&pool->mutex);
		//访问任务队列之前加锁
		pthread_mutex_lock(&pool->mutex);
		
		//=======================
		//1.若果没有任务，且线程池没有被关闭，进入条件变量等待队列中休眠
		while(pool->waiting_tasks == 0 && !pool->shutdown)
		{
			pthread_cond_wait(&pool->cond,&pool->mutex);
			//没有任务，但是线程池没有被关闭，就阻塞在这里。
			//唤醒的地方有两个：一个就是添加任务时候，还有一个就是销毁线程池的时候
		}
		
		//2.如果没有任务，线程池关闭，立即释放互斥锁，并且退出
		if(pool->waiting_tasks == 0 && pool->shutdown)
		{
			pthread_mutex_unlock(&pool->mutex);	
			pthread_exit(NULL);//
		}
		
		//3.有任务，就消费任务队列上的任务
		p = pool->first->next;//第一个任务节点没有赋值，所以跳过
		pool->first->next = p->next;
		pool->waiting_tasks--;
		
		//释放互斥锁，并弹栈handler(但是不执行)
		pthread_mutex_unlock(&pool->mutex);
		pthread_cleanup_pop(0);
		
		//执行任务，并且在此期间禁止响应取消请求
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
		(p->do_task)(p->arg);//执行任务
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
		
		free(p);//释放掉以被执行的任务节点的空间
	}
	//pthread_exit(NULL);
}

//初始化一个线程池
bool init_pool(thread_pool*pool,unsigned int threads_number)
{
	pthread_mutex_init(&pool->mutex,NULL);
	pthread_cond_init(&pool->cond,NULL);
	
	pool->shutdown = false;//关闭销毁线程池的标识
	pool->first =
	(struct task*)malloc(sizeof(struct task));//创建第一个任务节点并用
											//first指向它，注意第一个任务节点没有赋值
	pool->tids = malloc(sizeof(pthread_t)*MAX_ACTIVE_THREADS);
	if(pool->first == NULL || pool->tids ==NULL)
	{
		perror("allocate memory error:");
		return false;
	}
	
	pool->first->next = NULL;
	
	pool->max_waiting_tasks = MAX_WAITING_TASKS;
	pool->waiting_tasks = 0;
	pool->active_threads = threads_number;
	
	//创建threads_number线程来运行任务执行函数
	int i;
	for(i = 0; i <pool->active_threads;i++ )
	{
		if(pthread_create(&(pool->tids[i]),NULL,routine,(void*)pool) != 0)
		{
			perror("create thread error:");
			return false;
		}
	}
	
	return true;
}

//添加任务
bool add_task(thread_pool*pool,void*(*do_task)(void*arg),void*arg)
{
	//创建一个新的任务节点
	struct task*new_task = malloc(sizeof(struct task));
	if(new_task == NULL)
	{
		perror("allocate new_task memory error:");
		return false;
	}
	new_task->do_task = do_task;
	new_task->arg = arg;
	new_task->next = NULL;
	
	//访问任务队列之前加锁
	pthread_mutex_lock(&pool->mutex);
	
	if(pool->waiting_tasks == MAX_WAITING_TASKS)
	{
		 pthread_mutex_unlock(&pool->mutex);
		 printf("too many tasks\n");
		 free(new_task);
		 return false;
	}
	
	struct task*p = pool->first;
	while(p->next != NULL)
	{
		p = p->next;
	}
	
	p->next = new_task;
	pool->waiting_tasks++;
	
	//释放互斥锁，并唤醒其中一个阻塞在条件变量上的线程
	pthread_mutex_unlock(&pool->mutex);
	pthread_cond_signal(&pool->cond);//唤醒条件变量
	
	return true;
}
//添加线程
int add_thread(thread_pool*pool,unsigned char additional_threads)
{
	if(additional_threads == 0)
	{
		return 0;
	}
	unsigned char number = additional_threads + pool->active_threads;
	int i,increase = 0;
	for(i = pool->active_threads;i < number && i< MAX_ACTIVE_THREADS;i++)
	{
		if(pthread_create(&(pool->tids[i]),NULL,routine,(void*)pool) != 0)
		{
			perror("add thread error:");
			if(increase == 0)
				return -1;
			break;
		}
		increase++;
	}
	pool->active_threads += increase;
	
	return increase;
}
/*
	remove_thread:移除线程
	返回值：
			返回成功删除的线程数
*/

int remove_thread(thread_pool*pool,unsigned char remove_threads)
{
	if(remove_threads == 0)
	{
		return 0;
	}
	
	int remain_threads = pool->active_threads - remove_threads;
	remain_threads = remain_threads > 0 ? remain_threads : 1;
	
	//循环的指定数目的线程
	int i,re,delete = 0;
	for(i = pool->active_threads-1; i >remain_threads-1;i-- )
	{
		re = pthread_cancel(pool->tids[i]);
		if(re != 0)
		{
			break;
		}
		delete++;
	}
	
	pool->active_threads = i+1;
	return delete;

}
//销毁一个线程池
bool destroy_pool(thread_pool*pool)
{
	pool->shutdown = true;
	pthread_cond_broadcast(&pool->cond);
	
	//等待所有的线程退出
	int i;
	for(i = 0; i<pool->active_threads;i++)
	{
		pthread_join(pool->tids[i],NULL);
	}
	
	//释放动态空间
	free(pool->first);
	free(pool->tids);
	free(pool);
	
	return true;
}

