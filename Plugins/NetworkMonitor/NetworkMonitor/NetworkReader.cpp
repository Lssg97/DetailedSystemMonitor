#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "Ws2tcpip.h"
#include "iphlpapi.h"
#include "tlhelp32.h"
#include "NetworkReader.h"
#include "Mstcpip.h"

NetworkReader::NetworkReader()
{
	initThread();
}

const wstring& NetworkReader::getPIDName(DWORD pid)
{
	if (mpidtable.find(pid) == mpidtable.end())
	{
		HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe = { sizeof(pe) };
			BOOL ret = ::Process32First(hSnapshot, &pe);
			while (ret)
			{
				if (pid == pe.th32ProcessID)
				{
					wstring filename = pe.szExeFile;
					mpidtable[pid] = filename.substr(0, filename.rfind(L'.'));
					break;
				}
				ret = Process32Next(hSnapshot, &pe);
			}
			::CloseHandle(hSnapshot);
		}
	}
	return mpidtable[pid];
}

NetworkReader::~NetworkReader()
{
	release();
}
//
const std::vector<pair<std::wstring, ULONG64>>& NetworkReader::getPerformance()
{
	shared_lock<shared_timed_mutex> lgd(mdataMutex);
	return mperformance;
}

pair<std::wstring, ULONG64> NetworkReader::getPerformance(int i)
{
	// TODO: 在此处插入 return 语句
	shared_lock<shared_timed_mutex> lgd(mdataMutex);
	if (i < 0)
	{
		ULONG64 sum = 0;
		for (auto it : mperformance)
		{
			sum += it.second;
		}
		return make_pair(L"", sum);
	}
	else if (i < mperformance.size())
	{
		return mperformance[i];
	}
	else
		return make_pair(L"",0);
}

//
void NetworkReader::updateData(const std::map<std::wstring, ULONG64 >& data)
{
	clock_t nowtime = clock();
	unique_lock<shared_timed_mutex> lgd(mdataMutex);
	//do update
	mperformance.clear();
	//mperformance.resize(data.size());

	for (auto it : data)
	{
		ULONG64 bps = 0;
		if (it.second > mdatalast[it.first])
		{
			bps = (it.second - mdatalast[it.first]) * CLOCKS_PER_SEC / (nowtime - (ULONG64)mdatalasttime) ;
		}
		mperformance.push_back(make_pair(it.first, bps));
	}
	//mperformance.push_back(make_pair(mpidtable[pid], (cmp[i] - mdatalast[i]) * CLOCKS_PER_SEC / (nowtime - (ULONG64)mTimestamp)));

	//newdata
	mdatalast = data;
	mdatalasttime = nowtime;
	//sort result
	if (mperformance.size())
	{
		sort(mperformance.begin(), mperformance.end(),
			[](const pair<wstring, ULONG64>& x, const pair<wstring, ULONG64>& y) -> bool {
				return x.second > y.second;
			});
	}
}

void NetworkReader::initThread()
{
	//TCPTable线程
	mtcpthread.start();

}



void NetworkReader::release()
{

}

void NetworkReader::setInterval(clock_t inter)
{
	mInterval = inter;
}

clock_t NetworkReader::getInterval() const
{
	return mInterval;
}

//void NetworkReader::mdataADDTcp(DWORD dwPort, ULONG64 ul64Flow)
//{
//	DWORD dwPid = 0;
//	MIB_TCPTABLE_OWNER_PID* stcTcpTable = NULL;
//	DWORD szTcpTableSize = 0;
//	//获取TCP表大小
//	GetExtendedTcpTable(stcTcpTable, &szTcpTableSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
//	//分配内存
//	stcTcpTable = (MIB_TCPTABLE_OWNER_PID*)malloc(szTcpTableSize);
//	ZeroMemory(stcTcpTable, szTcpTableSize);
//	//获取TCP表
//	if (NO_ERROR != GetExtendedTcpTable(stcTcpTable, &szTcpTableSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0))
//	{
//		free(stcTcpTable);
//		return;
//	}
//	for (int i = 0; i < stcTcpTable->dwNumEntries; i++)
//	{
//		if (stcTcpTable->table[i].dwLocalPort == dwPort)
//			dwPid = stcTcpTable->table[i].dwOwningPid;
//	}
//	free(stcTcpTable);
//}
//
//void NetworkReader::mdataADDUdp(DWORD dwPort, ULONG64 ul64Flow)
//{
//	DWORD dwPid = 0;
//	MIB_UDPTABLE_OWNER_PID* stcUdpTable = NULL;
//	DWORD szUdpTableSize = 0;
//	//获取UDP表大小
//	GetExtendedUdpTable(stcUdpTable, &szUdpTableSize, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0);
//	//分配内存
//	stcUdpTable = (MIB_UDPTABLE_OWNER_PID*)malloc(szUdpTableSize);
//	ZeroMemory(stcUdpTable, szUdpTableSize);
//	//获取UDP表
//	if (NO_ERROR != GetExtendedUdpTable(stcUdpTable, &szUdpTableSize, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0))
//	{
//		//在实际测试中这个地方确实偶尔是会失败的,不过在频繁的更新中一两次失败无关紧要
//		free(stcUdpTable);
//		return;
//	}
//	for (int i = 0; i < stcUdpTable->dwNumEntries; i++)
//	{
//		if (stcUdpTable->table[i].dwLocalPort == dwPort)
//			dwPid = stcUdpTable->table[i].dwOwningPid;
//		//此处已经找到进程对应的pid了,同时包的大小和是上传还是下载已经通过参数传进来了。
//	}
//	free(stcUdpTable);
//	//对数据的处理
//	//...
//}


//void NetworkReader::updateTCP4()
//{
	//DWORD dwSize = { 0 };
	//DWORD dwRetValue = { 0 };
	//std::vector<unsigned char> buffer;

	////ptTable = (MIB_TCPTABLE_OWNER_PID*)MALLOC(sizeof(MIB_TCPTABLE_OWNER_PID));
	//dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);


	//do
	//{
	//	buffer.resize(dwSize, 0);
	//	dwRetValue = GetExtendedTcpTable(buffer.data(), &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_CONNECTIONS, 0);

	//} while (dwRetValue == ERROR_INSUFFICIENT_BUFFER);

	////char szLocalAddr[32];
	////char szRemoteAddr[32];
	//if (dwRetValue == NO_ERROR)
	//{
	//	//cout << "Number of Entries: " << (int)ptTable->dwNumEntries << endl << endl;
	//	//cout << left << setw(20) << "  Local Address" << setw(20) << "Foreign Address"
	//	//	<< setw(15) << "State" << setw(10) << "PID" << endl;
	//	PMIB_TCPTABLE_OWNER_PID ptTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(buffer.data());
	//	clock_t now = clock();
	//	for (int i = 0; i < (int)ptTable->dwNumEntries; i++)
	//	{
	//		if (ptTable->table[i].dwState != 5)
	//		{
	//			continue;
	//		}
	//		struct in_addr inAddr;
	//		inAddr.S_un.S_addr = (u_long)ptTable->table[i].dwRemoteAddr;

	//		string szRemoteAddr(inet_ntoa(inAddr));
	//		if (szRemoteAddr == "127.0.0.1")
	//		{
	//			continue;
	//		}

	//		//inAddr.S_un.S_addr = (u_long)ptTable->table[i].dwLocalAddr;
	//		//string szLocalAddr(inet_ntoa(inAddr));
	//		//u_short dwLocalPort = htons(ptTable->table[i].dwLocalPort);
	//		//u_short dwRemotePort = htons(ptTable->table[i].dwRemotePort);
	//		DWORD pid = ptTable->table[i].dwOwningPid;
	//		NetworkPerformanceItem data;
	//		data.Timestamp = now;

	//		//获取进程名
	//		if (mpidtable.find(pid) == mpidtable.end())
	//		{
	//			HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);
	//			if ( hSnapshot != INVALID_HANDLE_VALUE)
	//			{
	//				PROCESSENTRY32 pe = { sizeof(pe) };
	//				BOOL ret = ::Process32First(hSnapshot, &pe);
	//				while (ret)
	//				{
	//					if (pid == pe.th32ProcessID)
	//					{
	//						mpidtable[pid] = pe.szExeFile;
	//						break;
	//					}
	//					ret = Process32Next(hSnapshot, &pe);
	//				}
	//				::CloseHandle(hSnapshot);
	//			}
	//		}

	//		data.ProcessName = mpidtable[pid];
	//		//todo：获取进程流量
	//		MIB_TCPROW row;
	//		row.dwLocalAddr = ptTable->table[i].dwLocalAddr;
	//		row.dwLocalPort = ptTable->table[i].dwLocalPort;
	//		row.dwRemoteAddr = ptTable->table[i].dwRemoteAddr;
	//		row.dwRemotePort = ptTable->table[i].dwRemotePort;
	//		row.dwState = ptTable->table[i].dwState;
	//		void* processRow = &row;
	//		if (row.dwRemoteAddr != 0)
	//		{
	//			ULONG rodSize = 0, rwSize = 0;
	//			ULONG winStatus;
	//			//PUCHAR ros = NULL, rod = NULL;
	//			TCP_ESTATS_DATA_RW_v0 enable;
	//			rwSize = sizeof(TCP_ESTATS_DATA_RW_v0);
	//			TCP_ESTATS_DATA_ROD_v0 dataRod;
	//			rodSize = sizeof(TCP_ESTATS_DATA_ROD_v0);

	//			//if (rosSize != 0) {
	//			//	ros = (PUCHAR)malloc(rosSize);
	//			//	if (ros == NULL) {
	//			//		wprintf(L"\nOut of memory");
	//			//		return networkPerformanceItems;
	//			//	}
	//			//	else
	//			//		memset(ros, 0, rosSize); // zero the buffer
	//			//}
	//			//if (rodSize != 0) {
	//			//	rod = (PUCHAR)malloc(rodSize);
	//			//	if (rod == NULL) {
	//			//		free(ros);
	//			//		wprintf(L"\nOut of memory");
	//			//		return networkPerformanceItems;
	//			//	}
	//			//	else
	//			//		memset(rod, 0, rodSize); // zero the buffer
	//			//}

	//			winStatus = GetPerTcpConnectionEStats((PMIB_TCPROW)&row, TcpConnectionEstatsData, (PUCHAR)&enable, 0, rwSize, NULL, 0, 0, (PUCHAR)&dataRod, 0, rodSize);

	//			if (winStatus == NO_ERROR)
	//			{
	//				if (enable.EnableCollection == false)
	//				{
	//					enable.EnableCollection = true;
	//					SetPerTcpConnectionEStats((PMIB_TCPROW)&row, TcpConnectionEstatsData, (PUCHAR)&enable, 0, rwSize, 0);
	//					winStatus = GetPerTcpConnectionEStats((PMIB_TCPROW)&row, TcpConnectionEstatsData, (PUCHAR)&enable, 0, rwSize, NULL, 0, 0, (PUCHAR)&dataRod, 0, rodSize);
	//					if (winStatus == NO_ERROR)
	//					{
	//						data.BytesTotal = dataRod.DataBytesIn + dataRod.DataBytesOut;
	//					}
	//				}
	//				else
	//				{
	//					data.BytesTotal = dataRod.DataBytesIn + dataRod.DataBytesOut;
	//				}
	//			}
	//		}
	//		if (mdata.find(data.ProcessName) != mdata.end())
	//		{
	//			mdata[data.ProcessName].BytesTotal += data.BytesTotal;
	//		}
	//		else 
	//		{
	//			mdata[data.ProcessName] = data;
	//		}
	//	}
	//}
	//else
	//{
	//	
	//}

	//for (auto it : mdata)
	//{
	//	ULONG64 bps = 0;
	//	if (it.second.BytesTotal > mdatalast[it.first].BytesTotal)
	//	{
	//		bps = (it.second.BytesTotal - mdatalast[it.first].BytesTotal) / ((it.second.Timestamp - mdatalast[it.first].Timestamp) / CLOCKS_PER_SEC);
	//	}
	//	mperformance.push_back(make_pair(it.first, bps));
	//}
	//if (mperformance.size())
	//{
	//	sort(mperformance.begin(), mperformance.end(),
	//		[](const pair<wstring, ULONG64>& x, const pair<wstring, ULONG64>& y) -> bool {
	//			return x.second > y.second;
	//		});
	//}

	//return;
//}