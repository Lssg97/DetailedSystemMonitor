#pragma once

#include <windows.h>
#include <winioctl.h>
#include <tchar.h>

#ifdef _DEBUG
#include <stdio.h>
#include <time.h>
#endif

#define OLS_TYPE						40000

#define OLS_DRIVER_ID					_T("WinRing0_1_2_0")


class Driver
{
public:
	Driver(LPCTSTR gDriverId, LPCTSTR gDriverPath);
	HANDLE GetHandle();
	~Driver();
private:
	HANDLE		gHandle = INVALID_HANDLE_VALUE;		//句柄
	SC_HANDLE	hSCManager = NULL;					//服务管理程序系统组件数据库
	LPCTSTR		DriverId = NULL;					//驱动名称
	LPCTSTR		DriverPath = NULL;					//驱动路径

	BOOL NewDriver();
	BOOL OpenDriver();
	BOOL InstallDriver();
	BOOL StartDriver();
	BOOL StopDriver();
	BOOL RemoveDriver();
	BOOL SystemInstallDriver();
	BOOL IsSystemInstallDriver();
	VOID DeleteDriver();

	BOOL EnableSeLoadDriverPrivilege();
#ifdef _DEBUG
	VOID GetFormatTime();
	FILE* fp = fopen("log.txt", "a");
	char tmp[64];
#endif
};

