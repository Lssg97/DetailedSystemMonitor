#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include <shared_mutex>
#include <map>
#include <unordered_map>
#include "TCPPidThread.h"

using namespace std;

class NetworkReader
{
public:
	~NetworkReader();
	const std::vector<pair<std::wstring, ULONG64>>& getPerformance() ;
	pair<std::wstring, ULONG64> getPerformance(int i);
	//const std::unordered_map<wstring, ULONG64>& getValue();
	//const wstring& getPorcessName(DWORD pid);
	const wstring& getPIDName(DWORD pid);
	void updateData(const std::map<std::wstring, ULONG64 >& data);
	void initThread();
	void release();
	void setInterval(clock_t inter);
	clock_t getInterval() const;
	static NetworkReader* getInstance() 
	{
		static NetworkReader instance;
		return &instance;
	}

private:
	NetworkReader();
	//system resource
	//LPVOID mShareMemory = NULL;
	//in_addr mAddr;
	
	//multithread
	//clock_t mTimestamp = 0;
	clock_t mInterval = CLOCKS_PER_SEC;
	shared_timed_mutex mdataMutex;
	TCPPidThread mtcpthread;

	//data
	std::map<std::wstring,ULONG64> mdatalast;
	//std::unordered_map<std::wstring, ULONG64> mdata;
	clock_t mdatalasttime = 0;
	std::vector<pair<std::wstring, ULONG64>> mperformance;

	//netdata
	std::unordered_map<DWORD, wstring> mpidtable;

	//std::unordered_map<DWORD, >
	//map<string, string> mMonitorData;
	//void updateTCP4();
	//void updateUDP4();
};
