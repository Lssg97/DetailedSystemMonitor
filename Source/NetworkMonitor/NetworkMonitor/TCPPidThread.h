#pragma once
#include "iphlpapi.h"
#include "MyThread.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")


class TCPPidThread : public MyThread
{
public:
	TCPPidThread();
	virtual ~TCPPidThread();
	virtual void run() override;
};