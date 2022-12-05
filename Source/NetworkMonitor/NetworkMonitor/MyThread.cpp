#include <functional>
#include "MyThread.h"

MyThread::MyThread()
{

}


MyThread::~MyThread()
{
	if (!this->isInterrupted())
	{
		this->interrupt();
	}

	if (this->th.joinable()) 
	{
		this->th.join();
	}
}

void MyThread::start()
{
	std::thread thr(std::bind(&MyThread::run, this));
	this->th = std::move(thr);
}

std::thread::id MyThread::getId()
{
	return this->th.get_id();
}

void MyThread::interrupt()
{
	this->isInterript = true;
}

bool MyThread::isInterrupted() const
{
	return this->isInterript;
}

void MyThread::join()
{
	this->th.join();
}

MyThread::MyThread(const MyThread& thread) : isInterript(false)
{

}

void MyThread::run()
{

}