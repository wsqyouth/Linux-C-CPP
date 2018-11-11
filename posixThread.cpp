//Create a Posix thread from the main program

#include <iostream>
#include <pthread.h>

//This function will be called from a thread

void *call_from_thread(void *) {
    std::cout << "Launched by thread" << std::endl;
    return NULL;
}

int main() {
    pthread_t t;

    //Launch a thread
    pthread_create(&t, NULL, call_from_thread, NULL);

    //Join the thread with the main thread
    pthread_join(t, NULL);

    return 0;
}



-----------
//Create a group of Posix threads from the main program

#include <iostream>
#include <pthread.h>

static const int num_threads = 5;

//This function will be called from a thread

void *call_from_thread(void *) {
    std::cout << "Launched by thread:"<<pthread_self()<<std::endl;
    return NULL;
}

int main() {
    pthread_t t[num_threads];

    //Launch a group of threads
    for (int i = 0; i < num_threads; ++i) {
        pthread_create(&t[i], NULL, call_from_thread, NULL);
    }

    std::cout << "Launched from the main\n";

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(t[i], NULL);
    }

    return 0;
}


----
//Create a group of Posix threads from the main program

#include <iostream>
#include <pthread.h>

static const int num_threads = 5;

typedef struct {
    int thread_id;
} thread_data;

//This function will be called from a thread

void *call_from_thread(void *args) {
    thread_data *my_data = (thread_data *) args;
    std::cout << "Launched by thread " << my_data->thread_id << std::endl;
    return NULL;
}

int main() {
    pthread_t t[num_threads];
    thread_data td[num_threads];

    //Launch a group of threads
    for (int i = 0; i < num_threads; ++i) {
        td[i].thread_id = i;
        pthread_create(&t[i], NULL, call_from_thread, (void *) &td[i]);
    }

    std::cout << "Launched from the main\n";

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(t[i], NULL);
    }

    return 0;
}

---
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

class A
{
public:
	A(const char* name);
	void sayHello();
	void stop();
private:
	const char* m_name ;
	pthread_t m_thread;
};

A::A(const char*name)
{
	m_name = name;
    //call class method
	auto func = [] (void* arg)
	{
		printf("work thread\n");

		A* a = (A*)arg;
		a->sayHello();
		return (void*)0;
	};
	pthread_create(&m_thread,0,func,this);
}

void A::sayHello()
{
	printf("helo,%s\n",m_name);
}

void A::stop()
{
	pthread_join(m_thread,0);
}

int main() 
{
	A a("paopao_1");
	printf("main thread\n");
	a.stop();
	return 0;
}
