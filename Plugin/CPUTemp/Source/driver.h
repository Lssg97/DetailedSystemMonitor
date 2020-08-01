#pragma once


#include <windows.h>
#include <winioctl.h>
#include <tchar.h>
#include "running_environment.h"

#ifdef _DEBUG
#include <stdio.h>
#include <time.h>
#endif

#define OLS_TYPE				 40000

#define OLS_DRIVER_ID			_T("WinRing0_1_2_0")
#define IOCTL_OLS_READ_MSR		CTL_CODE(OLS_TYPE, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)

class driver
{
public:
	driver(LPCTSTR gDriverId, LPCTSTR gDriverPath);
	BOOL WINAPI Rdmsr(DWORD index, PDWORD eax, PDWORD edx);
	BOOL WINAPI RdmsrTx(DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR threadAffinityMask);
	~driver();
private:
	HANDLE		gHandle = INVALID_HANDLE_VALUE;		//���
	SC_HANDLE	hSCManager = NULL;					//����������ϵͳ������ݿ�
	LPCTSTR		DriverId = NULL;					//��������
	LPCTSTR		DriverPath = NULL;					//����·��
	BOOL		gIsNT = FALSE;						//�Ƿ�ΪNTϵͳ
	BOOL		gIsCpuid = FALSE;					//�Ƿ�֧��CPUID
	BOOL		gIsMsr = FALSE;						//�Ƿ�֧��Msr����
	BOOL NewDriver();
	BOOL OpenDriver();
	BOOL InstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);
	BOOL StartDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
	BOOL StopDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
	BOOL RemoveDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
	BOOL SystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);
	BOOL IsSystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);

	BOOL EnableSeLoadDriverPrivilege();
#ifdef _DEBUG
	FILE* fp = fopen("log.txt", "a");
	char tmp[64];
#endif
};

