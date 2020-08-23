#include "driver.h"


driver::driver(LPCTSTR gDriverId, LPCTSTR gDriverPath) :
	DriverId(gDriverId),
	DriverPath(gDriverPath),
	gIsNT(IsNT()),
	gIsCpuid(IsCpuid()),
	gIsMsr((gIsCpuid) ? IsMsr() : FALSE)
{
	NewDriver();
}

driver::~driver()
{
	if (gHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(gHandle);
		gHandle = INVALID_HANDLE_VALUE;
	}
	StopDriver(hSCManager, DriverId);
	RemoveDriver(hSCManager, DriverId);
}

BOOL driver::NewDriver()
{
#ifdef _DEBUG
	time_t t = time(0);
	strftime(tmp, sizeof(tmp), "[%X]：", localtime(&t));
	fputs(tmp, fp);
	fputs("打开服务管理程序系统组件数据库->", fp);
	hSCManager = OpenSCManager(NULL, //目标计算机名，NULL表示本地计算机
		NULL, //服务管理程序系统组件数据库，可以设为SERVICES_ACTIVE_DATABASE，如果为NULL，表示默认打开SERVICES_ACTIVE_DATABASE（服务管理程序系统组件数据库）
		SC_MANAGER_ALL_ACCESS); //对SCM的权限（最高权限）
	if (hSCManager == NULL)
	{
		fputs("失败\n", fp);
		return FALSE;
	}
	fputs("成功\n", fp);
	if (!OpenDriver())
	{
		if (gHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(gHandle);
			gHandle = INVALID_HANDLE_VALUE;
		}
		StopDriver(hSCManager, DriverId);
		RemoveDriver(hSCManager, DriverId);
		if (InstallDriver(hSCManager, DriverId, DriverPath))
		{
			if (!IsSystemInstallDriver(hSCManager, DriverId, DriverPath))
			{
				SystemInstallDriver(hSCManager, DriverId, DriverPath);
			}
			if (StartDriver(hSCManager, DriverId))
			{
				OpenDriver();
			}
		}
		else return FALSE;
	}
	if (hSCManager != NULL) CloseServiceHandle(hSCManager);
#else
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);

	if (hSCManager == NULL) return FALSE;

	if (!OpenDriver())
	{
		if (gHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(gHandle);
			gHandle = INVALID_HANDLE_VALUE;
		}
		StopDriver(hSCManager, DriverId);
		RemoveDriver(hSCManager, DriverId);
		if (InstallDriver(hSCManager, DriverId, DriverPath))
		{
			//if (!IsSystemInstallDriver(hSCManager, DriverId, DriverPath))
			//{
			//	SystemInstallDriver(hSCManager, DriverId, DriverPath);
			//}
			if (StartDriver(hSCManager, DriverId))
			{
				OpenDriver();
			}
		}
		else return FALSE;
	}
	if (hSCManager != NULL) CloseServiceHandle(hSCManager);
#endif
	return TRUE;
}

BOOL driver::OpenDriver()
{
#ifdef _DEBUG
	time_t t = time(0);
	strftime(tmp, sizeof(tmp), "[%X]：", localtime(&t));
	fputs(tmp, fp);
	fputs("创建驱动对象->", fp);
#endif
	//创建或打开下列对象，并返回一个可以用来访问这些对象的句柄
	gHandle = CreateFile(
		_T("\\\\.\\") OLS_DRIVER_ID,//要打开的文件的名字
		GENERIC_READ | GENERIC_WRITE,//如果为 GENERIC_READ 表示允许对设备进行读访问；如果为 GENERIC_WRITE 表示允许对设备进行写访问（可组合使用）
		0,//不共享；FILE_SHARE_READ 和 / 或 FILE_SHARE_WRITE 表示允许对文件进行共享访问
		NULL,//SECURITY_ATTRIBUTES ，指向一个 SECURITY_ATTRIBUTES 结构的指针，定义了文件的安全特性（如果操作系统支持的话）
		OPEN_EXISTING,//文件必须已经存在。由设备提出要求
		FILE_ATTRIBUTE_NORMAL,//默认属性
		NULL//如果不为零，则指定一个文件句柄。新文件将从这个文件中复制扩展属性
	);
	
#ifdef _DEBUG
	DWORD error = GetLastError();
	if (gHandle == INVALID_HANDLE_VALUE)
	{
		fputs("失败", fp);
		snprintf(tmp, sizeof(tmp), "(错误代码：%d)\n", error);
		fputs(tmp, fp);
	}
	fputs("成功\n", fp);
	return TRUE;
#else
	if (gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	return TRUE;
#endif // DEBUG
}

BOOL driver::InstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath)
{
	SC_HANDLE	hService = NULL;
	BOOL        rCode = FALSE;
	DWORD		error = NO_ERROR;
#ifdef _DEBUG
	time_t t = time(0);

	strftime(tmp, sizeof(tmp), "[%X]：", localtime(&t));
	fputs(tmp, fp);
	fputs("创建服务对象->", fp);
	EnableSeLoadDriverPrivilege();
	//创建一个服务对象，并把它添加进SCM数据库
	hService = CreateService(hSCManager,			//SCM数据库句柄，同时该句柄需要具有SC_MANAGER_CREATE_SERVICE权限
		DriverId,				//要安装服务的名字，不可超过256个字符，不能含有/和\字符，同时大小写敏感
		DriverId,				//服务显示出的名称
		SERVICE_ALL_ACCESS,		//对服务的权限
		SERVICE_KERNEL_DRIVER,	//服务类型（驱动服务）
		SERVICE_DEMAND_START,	//服务启动选项（当一个程序调用StartService函数时，SCM启动服务）
		SERVICE_ERROR_NORMAL,	//服务错误处理方式（启动程序在事件日志中记录错误，但继续运行操作）
		DriverPath,				//服务程序路径
		NULL,					//在加载顺序此服务所属的组的名称（如果该服务不属于组，指定 NULL 或空字符串）
		NULL,					//指向接收 lpLoadOrderGroup 参数中指定的组中唯一的标记值的变量。 如果不更改现有的标签，请指定 NULL
		NULL,					//空分隔名称的服务或加载顺序组系统必须在这个服务开始之前的双空终止数组的指针。 如果服务没有任何依赖关系，请指定为 NULL 或空字符串。
		NULL,					//该服务应在其下运行的帐户的名称（使用LocalSystem帐户）
		NULL					//由lpServiceStartName参数指定的帐户名的密码。 如果该帐户没有密码，或者如果在LocalService，NetworkService或LocalSystem帐户中运行该服务，请指定一个空字符串或NULL
	);

	if (hService == NULL)
	{
		error = GetLastError();
		if (error == ERROR_SERVICE_EXISTS)
		{
			rCode = TRUE;
		}
		fputs("失败", fp);
		snprintf(tmp, sizeof(tmp), "(错误代码：%d)\n", error);
		fputs(tmp, fp);
	}
	else
	{
		fputs("成功\n", fp);
		rCode = TRUE;
		CloseServiceHandle(hService);
	}

	return rCode;
#else
	EnableSeLoadDriverPrivilege();
	hService = CreateService(hSCManager,DriverId,DriverId,SERVICE_ALL_ACCESS,SERVICE_KERNEL_DRIVER,SERVICE_DEMAND_START,SERVICE_ERROR_NORMAL,DriverPath,NULL,NULL,NULL,NULL,NULL);
	if (hService == NULL)
	{
		error = GetLastError();
		if (error == ERROR_SERVICE_EXISTS)
		{
			rCode = TRUE;
		}
	}
	else
	{
		rCode = TRUE;
		CloseServiceHandle(hService);
	}

	return rCode;
#endif // DEBUG


}

BOOL driver::StartDriver(SC_HANDLE hSCManager, LPCTSTR DriverId)
{
	SC_HANDLE	hService = NULL;
	BOOL		rCode = FALSE;
	DWORD		error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
#ifdef _DEBUG
	char tmp[64];
	time_t t = time(0);
	strftime(tmp, sizeof(tmp), "[%X]：", localtime(&t));
	fputs(tmp, fp);
	fputs("打开服务->", fp);
	if (hService != NULL)
	{
		fputs("成功\n", fp);
		time_t t = time(0);
		strftime(tmp, sizeof(tmp), "[%X]：", localtime(&t));
		fputs(tmp, fp);
		fputs("运行服务->", fp);
		if (!StartService(hService, 0, NULL))
		{
			error = GetLastError();
			if (error == ERROR_SERVICE_ALREADY_RUNNING)
			{
				rCode = TRUE;
			}
			fputs("失败", fp);
			snprintf(tmp, sizeof(tmp), "(错误代码：%d)\n", error);
			fputs(tmp, fp);
		}
		else
		{
			fputs("成功\n", fp);
			rCode = TRUE;
		}
		CloseServiceHandle(hService);
	}
	else
	{
		error = GetLastError();
		fputs("失败", fp);
		snprintf(tmp, sizeof(tmp), "(错误代码：%d)\n", error);
		fputs(tmp, fp);
	}
	return rCode;
#else
	if (hService != NULL)
	{
		if (!StartService(hService, 0, NULL))
		{
			error = GetLastError();
			if (error == ERROR_SERVICE_ALREADY_RUNNING)
			{
				rCode = TRUE;
			}
		}
		else
		{
			rCode = TRUE;
		}
		CloseServiceHandle(hService);
	}
	return rCode;
#endif // DEBUG
}

BOOL driver::RemoveDriver(SC_HANDLE hSCManager, LPCTSTR DriverId)
{
	SC_HANDLE   hService = NULL;
	BOOL        rCode = FALSE;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
	if (hService == NULL)
	{
		rCode = TRUE;
	}
	else
	{
		rCode = DeleteService(hService);
		CloseServiceHandle(hService);
	}

	return rCode;
}

BOOL driver::StopDriver(SC_HANDLE hSCManager, LPCTSTR DriverId)
{
	SC_HANDLE		hService = NULL;
	BOOL			rCode = FALSE;
	SERVICE_STATUS	serviceStatus;
	DWORD		error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

	if (hService != NULL)
	{
		rCode = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
		CloseServiceHandle(hService);
	}

	return rCode;
}

BOOL driver::SystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath)
{
	SC_HANDLE	hService = NULL;
	BOOL		rCode = FALSE;
	DWORD		error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

#ifdef _DEBUG
	time_t t = time(0);
	strftime(tmp, sizeof(tmp), "[%X]：", localtime(&t));
	fputs(tmp, fp);
	fputs("创建系统服务->", fp);

	if (hService != NULL)
	{
		rCode = ChangeServiceConfig(hService,
			SERVICE_KERNEL_DRIVER,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			DriverPath,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
		);
		error = GetLastError();
		CloseServiceHandle(hService);
	}

	if (rCode)
	{
		fputs("成功\n", fp);
	}
	else
	{
		fputs("失败", fp);
		snprintf(tmp, sizeof(tmp), "(错误代码：%d)\n", error);
		fputs(tmp, fp);
	}
#else
	if (hService != NULL)
	{
		rCode = ChangeServiceConfig(hService,
			SERVICE_KERNEL_DRIVER,
			SERVICE_BOOT_START,
			SERVICE_ERROR_NORMAL,
			DriverPath,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
		);
		error = GetLastError();
		CloseServiceHandle(hService);
	}
#endif // _DEBUG

	return rCode;
}

BOOL driver::IsSystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath)
{
	SC_HANDLE				hService = NULL;
	BOOL					rCode = FALSE;
	DWORD					dwSize;
	LPQUERY_SERVICE_CONFIG	lpServiceConfig = NULL;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

	if (hService != NULL)
	{
		if (QueryServiceConfig(hService, NULL, 0, &dwSize) == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpServiceConfig = (LPQUERY_SERVICE_CONFIG)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
		}

		if (QueryServiceConfig(hService, lpServiceConfig, dwSize, &dwSize) && lpServiceConfig)
		{
			if (lpServiceConfig->dwStartType == SERVICE_AUTO_START)
			{
				rCode = TRUE;
			}
		}

		CloseServiceHandle(hService);

		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpServiceConfig);
	}

	return rCode;
}

BOOL WINAPI driver::Rdmsr(DWORD index, PDWORD eax, PDWORD edx)
{
	if (gHandle == INVALID_HANDLE_VALUE) return FALSE;

	if (eax == NULL || edx == NULL || gIsMsr == FALSE) return FALSE;

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	BYTE	outBuf[8] = { 0 };

	result = DeviceIoControl(
		gHandle,				// 需要执行操作的设备句柄。该设备通常是卷，目录，文件或流，使用 CreateFile 函数打开获取设备句柄。具体的见备注
		IOCTL_OLS_READ_MSR,		// 操作的控制代码，该值标识要执行的特定操作以及执行该操作的设备的类型，有关控制代码的列表，请参考备注。
		&index,					// （可选）指向输入缓冲区的指针。
		sizeof(index),			// 输入缓冲区以字节为单位的大小。单位为字节。
		&outBuf,				// （可选）指向输出缓冲区的指针。
		sizeof(outBuf),			// 输出缓冲区以字节为单位的大小。单位为字节。
		&returnedLength,		// （可选）指向一个变量的指针，该变量接收存储在输出缓冲区中的数据的大小。
		NULL					// （可选）指向OVERLAPPED结构的指针。
	);

	if (result)
	{
		memcpy(eax, outBuf, 4);
		memcpy(edx, outBuf + 4, 4);
		return TRUE;
	}
	else return FALSE;
}

BOOL WINAPI driver::RdmsrTx(DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR threadAffinityMask)
{
	BOOL		result = FALSE;
	DWORD_PTR	mask = 0;
	HANDLE		hThread = NULL;

	if (gIsNT)
	{
		hThread = GetCurrentThread();
		mask = SetThreadAffinityMask(hThread, threadAffinityMask);
		if (mask == 0)
		{
			return FALSE;
		}
	}

	result = Rdmsr(index, eax, edx);

	if (gIsNT)
	{
		SetThreadAffinityMask(hThread, mask);
	}

	return result;
}

BOOL driver::pciConfigRead(DWORD pciAddress, DWORD regAddress, PBYTE value, DWORD size, PDWORD error)
{
	if (gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if (value == NULL)
	{
		return FALSE;
	}
	// alignment check
	if (size == 2 && (regAddress & 1) != 0)
	{
		return FALSE;
	}
	if (size == 4 && (regAddress & 3) != 0)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	OLS_READ_PCI_CONFIG_INPUT inBuf;

	inBuf.PciAddress = pciAddress;
	inBuf.PciOffset = regAddress;

	result = DeviceIoControl(
		gHandle,
		IOCTL_OLS_READ_PCI_CONFIG,
		&inBuf,
		sizeof(inBuf),
		value,
		size,
		&returnedLength,
		NULL
	);

	if (error != NULL)
	{
		*error = GetLastError();
	}

	if (result)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

DWORD WINAPI driver::ReadPciConfigDword(DWORD pciAddress, BYTE regAddress)
{
	DWORD ret;
	if (pciConfigRead(pciAddress, regAddress, (PBYTE)&ret, sizeof(ret), NULL))
	{
		return ret;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}

DWORD WINAPI driver::ReadIoPortDword(WORD port)
{
	if (gHandle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD	port4 = port;
	DWORD	value = 0;

	result = DeviceIoControl(
		gHandle,
		IOCTL_OLS_READ_IO_PORT_DWORD,
		&port4,
		sizeof(port4),	// required 4 bytes
		&value,
		sizeof(value),
		&returnedLength,
		NULL
	);

	return value;
}

BOOL WINAPI driver::WriteIoPortDword(WORD port, DWORD value)
{
	if (gHandle == INVALID_HANDLE_VALUE) return FALSE;

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD   length = 0;
	OLS_WRITE_IO_PORT_INPUT inBuf;

	inBuf.LongData = value;
	inBuf.PortNumber = port;
	length = offsetof(OLS_WRITE_IO_PORT_INPUT, CharData) +
		sizeof(inBuf.LongData);

	result = DeviceIoControl(
		gHandle,
		IOCTL_OLS_WRITE_IO_PORT_DWORD,
		&inBuf,
		length,
		NULL,
		0,
		&returnedLength,
		NULL
	);
	if (result) return TRUE;
	else return FALSE;
}

BOOL driver::get_bus_dev(UINT devieid, INT* BUS, INT* DEV)
{
	UINT address, value;
	for (UINT bus = 0; bus < 128; bus++)
	{
		for (UINT dev = 0; dev < 32; dev++)
		{
			for (UINT func = 0; func < 8; func++)
			{
				address = PciBusDevFunc(bus, dev, func);
				value = ReadPciConfigDword(address, 0x00);
				if (value == devieid)
				{
					*BUS = bus;
					*DEV = dev;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

DWORD driver::get_temp()
{
	INT bus, dev;
	if (!get_bus_dev(0x1103, &bus, &dev))
	{
		return FALSE;
	}

	UINT slot = DeviceSlot(dev, 0x3);
	UINT IO_ADDRE = GetDevice(bus, slot, 0xE4);
	
	DWORD CPUTemp;

	if (WriteIoPortDword(0xCF8, IO_ADDRE)) CPUTemp = ReadIoPortDword(0xCFC);

	INT CPUInfo[4], g_Offset, K;

	__cpuid(CPUInfo,1);
	INT t = CPUInfo[0];
	INT family = ((t >> 20) & 0xFF) + ((t >> 8) & 0xF), model = ((t >> 12) & 0xF0) + ((t >> 4) & 0xF), stepping = t & 0xF;

	if (family == 0xF && !(((model == 4) && (stepping == 0)) || ((model == 5) && (stepping <= 1)))) K = 8;
	if (family > 0xF) K = 10;

	if (model >= 0x69 && model != 0xc1 && model != 0x6c && model != 0x7c) g_Offset = 49 - 21;
	else g_Offset = 49;

	CPUTemp = (CPUTemp >> 16) & 0xFF;

	if (K == 8) CPUTemp -= g_Offset;
	if (K == 10) CPUTemp /= 8;

	return CPUTemp;
}

DWORD driver::GetTemp(DWORD_PTR threadAffinityMask)
{
	DWORD		result = FALSE;
	DWORD_PTR	mask = 0;
	HANDLE		hThread = NULL;

	if (gIsNT)
	{
		hThread = GetCurrentThread();
		mask = SetThreadAffinityMask(hThread, threadAffinityMask);
		if (mask == 0)
		{
			return FALSE;
		}
	}

	result = get_temp();

	if (gIsNT)
	{
		SetThreadAffinityMask(hThread, mask);
	}

	return result;
}

BOOL driver::EnableSeLoadDriverPrivilege()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return false;
	}

	if (!LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &sedebugnameValue))
	{
		__try
		{
			if (hToken)
			{
				CloseHandle(hToken);
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {};
		return false;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		__try
		{
			if (hToken)
			{
				CloseHandle(hToken);
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {};
		return false;
	}

	return true;
}