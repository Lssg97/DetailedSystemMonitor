#pragma once
#include <thread>
#include <atomic>

class MyThread
{
public:
	MyThread();
	virtual ~MyThread();

	void start();
	std::thread::id getId();
	void interrupt();
	bool isInterrupted() const;
	void join();
	MyThread(const MyThread& thread);

protected:
	virtual void run();
	std::atomic<bool> isInterript = false;
	std::thread  th;
};