#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "Ws2tcpip.h"
#include "iphlpapi.h"
#include "TCPPidThread.h"
//#include "Mstcpip.h"
#include <vector>
#include <map>
#include "NetworkReader.h"

TCPPidThread::TCPPidThread()
{
}

TCPPidThread::~TCPPidThread()
{
}

void TCPPidThread::run()
{
	try
	{
		while (!this->isInterript)
		{
			clock_t time = clock();
			//UpdateTCPPid
			DWORD dwSize = { 0 };
			DWORD dwRetValue = { 0 };
			std::vector<unsigned char> buffer;

			dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);

			do
			{
				buffer.resize(dwSize, 0);
				dwRetValue = GetExtendedTcpTable(buffer.data(), &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_CONNECTIONS, 0);

			} while (dwRetValue == ERROR_INSUFFICIENT_BUFFER);

			if (dwRetValue == NO_ERROR)
			{
				std::map<std::wstring, ULONG64> data;
				//cout << "Number of Entries: " << (int)ptTable->dwNumEntries << endl << endl;
				//cout << left << setw(20) << "  Local Address" << setw(20) << "Foreign Address"
				//	<< setw(15) << "State" << setw(10) << "PID" << endl;
				PMIB_TCPTABLE_OWNER_PID ptTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(buffer.data());
				for (int i = 0; i < (int)ptTable->dwNumEntries; i++)
				{
					if (ptTable->table[i].dwState != 5)
					{
						continue;
					}
					in_addr inAddr;
					inAddr.S_un.S_addr = (u_long)ptTable->table[i].dwRemoteAddr;

					std::string szRemoteAddr(inet_ntoa(inAddr));
					if (szRemoteAddr == "127.0.0.1")
					{
						continue;
					}

					//todo：获取进程流量
					MIB_TCPROW row;
					row.dwLocalAddr = ptTable->table[i].dwLocalAddr;
					row.dwLocalPort = ptTable->table[i].dwLocalPort;
					row.dwRemoteAddr = ptTable->table[i].dwRemoteAddr;
					row.dwRemotePort = ptTable->table[i].dwRemotePort;
					row.dwState = ptTable->table[i].dwState;
					//void* processRow = &row;
					ULONG64 bytes = 0;
					std::wstring process = NetworkReader::getInstance()->getPIDName(ptTable->table[i].dwOwningPid);
					ULONG rodSize = 0, rwSize = 0;
					ULONG winStatus;
					TCP_ESTATS_DATA_RW_v0 enable;
					rwSize = sizeof(TCP_ESTATS_DATA_RW_v0);
					TCP_ESTATS_DATA_ROD_v0 dataRod;
					rodSize = sizeof(TCP_ESTATS_DATA_ROD_v0);

					winStatus = GetPerTcpConnectionEStats((PMIB_TCPROW)&row, TcpConnectionEstatsData, (PUCHAR)&enable, 0, rwSize, NULL, 0, 0, (PUCHAR)&dataRod, 0, rodSize);

					if (winStatus == NO_ERROR)
					{
						if (enable.EnableCollection == false)
						{
							enable.EnableCollection = true;
							SetPerTcpConnectionEStats((PMIB_TCPROW)&row, TcpConnectionEstatsData, (PUCHAR)&enable, 0, rwSize, 0);
							winStatus = GetPerTcpConnectionEStats((PMIB_TCPROW)&row, TcpConnectionEstatsData, (PUCHAR)&enable, 0, rwSize, NULL, 0, 0, (PUCHAR)&dataRod, 0, rodSize);
							if (winStatus == NO_ERROR && enable.EnableCollection == true)
							{
								bytes = dataRod.DataBytesIn + dataRod.DataBytesOut;
							}
						}
						else
						{
							bytes = dataRod.DataBytesIn + dataRod.DataBytesOut;
						}
					}

					if (data.find(process) != data.end())
					{
						data[process] += bytes;
					}
					else
					{
						data[process] = bytes;
					}
				}

				NetworkReader::getInstance()->updateData(data);
			}
			else
			{

			}
			clock_t interval = NetworkReader::getInstance()->getInterval();
			if (clock_t del = (clock() - time) < interval)
			{
				Sleep(interval - del);
			}
		}
	}
	catch (...)
	{

	}
}
