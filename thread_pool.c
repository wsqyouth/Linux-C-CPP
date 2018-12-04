//参考https://github.com/wangbojing/threadpool/blob/master/thread_pool_simple.c#L87
//调试成功了线程池，但是还是不太懂，主要是代码你中有我，我中有你，不太方便看得懂，后面继续理解
//gcc thread_pool.c -g  -o thread_pool -pthread   ./thread_pool

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#define LL_ADD(item,list) do {  \
	item->prev = NULL;			\
	item->next = list;			\
	list = item;				\
}while(0)

#define LL_REMOVE(item,list) do {		\
	if(item->prev != NULL)	item->prev->next = item->next;		\
	if(item->next != NULL)  item->next->prev = item->prev;		\
	if(list == item) list = item->next;							\
	item->prev = item->next = NULL;								\
}while(0)

//线程工人
typedef struct NWORKER {
	pthread_t thread;
	int terminate;
	struct NWORKQUEUE *workqueue;
	struct NWORKER *prev;
	struct NWORKER *next;
} nWorker;

//工作队列
typedef struct NJOB {
	void (*job_function)(struct NJOB *job);
	void *user_data;
	struct NJOB *prev;
	struct NJOB *next;
} nJob;

//管理
typedef struct NWORKQUEUE {
	struct NWORKER *workers;
	struct NJOB *waiting_jobs;
	pthread_mutex_t jobs_mtx;
	pthread_cond_t jobs_cond;
} nWorkQueue;

typedef nWorkQueue nThreadPool;

//在worker线程中，处理workqueue
static void *workerThread(void *ptr) {
	nWorker *worker = (nWorker*) ptr;
	while(1) {
		pthread_mutex_lock(&worker->workqueue->jobs_mtx);

		while(worker->workqueue->waiting_jobs == NULL) {
			if(worker->terminate) break;
			pthread_cond_wait(&worker->workqueue->jobs_cond,&worker->workqueue->jobs_mtx);
		}

		if(worker->terminate) {
			pthread_mutex_unlock(&worker->workqueue->jobs_mtx);
			break;
		}
		
		nJob *job = worker->workqueue->waiting_jobs;
		if(NULL != job) {
			LL_REMOVE(job,worker->workqueue->waiting_jobs);
		}

		pthread_mutex_unlock(&worker->workqueue->jobs_mtx);

		if(job == NULL) continue;

		job->job_function(job);
	}

	free(worker);
	pthread_exit(NULL);
}

//创建workqueue，同时将其添加到worker中,在worker线程中处理
int threadPoolCreate(nThreadPool *workqueue,int numWorkers) {
	if(numWorkers < 1) numWorkers = 1;
	memset(workqueue,0,sizeof(nThreadPool));
	
	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
	memcpy(&workqueue->jobs_cond,&blank_cond,sizeof(workqueue->jobs_cond));

	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
	memcpy(&workqueue->jobs_mtx,&blank_mutex,sizeof(workqueue->jobs_mtx));

	for(int i = 0; i < numWorkers; ++i) {
		nWorker *worker = (nWorker*) malloc(sizeof(nWorker));
		if(NULL == worker) {
			perror("malloc");
			return 1;
		}

		memset(worker,0,sizeof(nWorker));
		worker->workqueue = workqueue;

		int ret = pthread_create(&worker->thread,NULL,workerThread,(void*)worker);
		if(ret) {
			perror("pthread_create");
			free(worker);

			return 1;
		}
		LL_ADD(worker,worker->workqueue->workers);
	}

	return 0;
}

//清空workqueue
void threadPoolShutdown(nThreadPool *workqueue) {
	nWorker *worker = NULL;

	for(worker = workqueue->workers;worker != NULL; worker = worker->next) {
		worker->terminate = 1;
	}

	pthread_mutex_lock(&workqueue->jobs_mtx);

	workqueue->workers = NULL;
	workqueue->waiting_jobs = NULL;
	pthread_cond_broadcast(&workqueue->jobs_cond);

	pthread_mutex_unlock(&workqueue->jobs_mtx);
}
//给workqueue添加job
void threadPoolQueue(nThreadPool *workqueue, nJob *job) {

	pthread_mutex_lock(&workqueue->jobs_mtx);

	LL_ADD(job,workqueue->waiting_jobs);

	pthread_cond_signal(&workqueue->jobs_cond);
	pthread_mutex_unlock(&workqueue->jobs_mtx);
}
//----------debug thread pool --------
#define KING_MAX_THREAD    3 
#define KING_COUNTER_SIZE  100

void king_counter(nJob *job) {
	int index = *(int*)job->user_data;
	printf("index: %d,selfid : %lu\n",index,pthread_self());

	free(job->user_data);
	free(job);
}

int main() {
	nThreadPool pool;

	threadPoolCreate(&pool,KING_MAX_THREAD);

	for(int i=0; i<KING_COUNTER_SIZE;++i) {
		nJob *job = (nJob*)malloc(sizeof(nJob));
		if(NULL == job) {
			perror("malloc");
			exit(1);
		}

		job->job_function = king_counter;
		job->user_data = malloc(sizeof(int));
		*(int*)job->user_data = i;

		threadPoolQueue(&pool,job);
	}
	//不可省略，主线程不能先于子线程exit
	getchar();	
	printf("hello\n");
	return 0;
}
