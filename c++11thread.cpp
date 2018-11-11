//demo.cpp
#include <iostream>
#include <thread>
#include <string>

using namespace std;

int main() 
{
	auto func = [] (string name) {
		cout << "hello" << name << endl;
	};
	thread t(func,"tom");

	t.join();

	return 0;
}

-----
//Create a C++11 thread from the main program

#include <iostream>
#include <thread>

//This function will be called from a thread

void call_from_thread() {
    std::cout << "Hello, World!" << std::endl;
}

int main() {
    //Launch a thread
    std::thread t1(call_from_thread);

    //Join the thread with the main thread
    t1.join();
    
    return 0;
}

-----
#include <iostream>
#include <thread>
#include <string>

void thread_fun(std::string &str) {
	std::cout << "thread fun";
	std::cout << "msg is = " << str << std::endl;
	str = "hello";
}

int main()
{
	std::string str = "katty";
	std::thread t(&thread_fun,std::ref(str));
	std::cout<< "main thread = " << str << std::endl;
	t.join();
	return 0;
}


//类成员函数作为线程函数
#include <pthread.h>
#include <iostream>
#include <thread>
#include <string>
using namespace std;
class A
{
public:
	A(string& name);
	void sayHello();
	void stop();
private:
	string m_name;
	thread* m_thread;
};

A::A(string& name)
{
	m_name = name;
    //call class method
	auto func = [] (void* arg)
	{
		std::cout << "work thread\n";

		A* a = (A*)arg;
		a->sayHello();
		return (void*)0;
	};
	m_thread = new thread(func,this);
}

void A::sayHello()
{
	std::cout << "hello," << m_name <<std::endl;
}

void A::stop()
{
	m_thread->join();
}

int main() 
{
	string str = "paopao";
	A a(str);
	std::cout << "main thread\n";
	a.stop();
	return 0;
}
