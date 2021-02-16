#pragma warning(disable:4996)
#include "Driver.h"


Driver::Driver(LPCTSTR gDriverId, LPCTSTR gDriverPath) :
	DriverId(gDriverId),
	DriverPath(gDriverPath)
{
	NewDriver();
}

Driver::~Driver()
{
	DeleteDriver();
	if (hSCManager != NULL) CloseServiceHandle(hSCManager);
}

HANDLE Driver::GetHandle()
{
	return gHandle;
}

#ifdef _DEBUG
VOID Driver::GetFormatTime()
{
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	sprintf(tmp, "[%02d:%02d:%02d.%03d]：", wtm.wHour, wtm.wMinute, wtm.wSecond, wtm.wMilliseconds);
	fputs(tmp, fp);
}
#endif // _DEBUG

BOOL Driver::NewDriver()
{
	hSCManager = OpenSCManager(
		NULL,					//目标计算机名，NULL表示本地计算机
		NULL,					//服务管理程序系统组件数据库，可以设为SERVICES_ACTIVE_DATABASE，如果为NULL，表示默认打开SERVICES_ACTIVE_DATABASE（服务管理程序系统组件数据库）
		SC_MANAGER_ALL_ACCESS	//对SCM的权限（最高权限）
	);
#ifdef _DEBUG
	DWORD error = GetLastError();
	GetFormatTime();
	fputs("打开服务管理程序系统组件数据库（NewDriver）->", fp);
	if (hSCManager == NULL)
	{
		snprintf(tmp, sizeof(tmp), "失败(错误代码：%d)\n", error);
		fputs(tmp, fp);
		return FALSE;
	}
	else fputs("成功\n", fp);
#else
	if (hSCManager == NULL) return FALSE;
#endif

	if (!OpenDriver())
	{
		DeleteDriver();
		if (InstallDriver())
		{
			if (StartDriver())
			{
				OpenDriver();
			}
		}
		else return FALSE;
	}

	return TRUE;
}

BOOL Driver::OpenDriver()
{
	BOOL rCode = FALSE;
	//创建或打开下列对象，并返回一个可以用来访问这些对象的句柄
	gHandle = CreateFile(
		_T("\\\\.\\") OLS_DRIVER_ID,	//要打开的文件的名字
		GENERIC_READ | GENERIC_WRITE,	//如果为 GENERIC_READ 表示允许对设备进行读访问；如果为 GENERIC_WRITE 表示允许对设备进行写访问（可组合使用）
		0,								//不共享；FILE_SHARE_READ 和 / 或 FILE_SHARE_WRITE 表示允许对文件进行共享访问
		NULL,							//SECURITY_ATTRIBUTES ，指向一个 SECURITY_ATTRIBUTES 结构的指针，定义了文件的安全特性（如果操作系统支持的话）
		OPEN_EXISTING,					//文件必须已经存在。由设备提出要求
		FILE_ATTRIBUTE_NORMAL,			//默认属性
		NULL							//如果不为零，则指定一个文件句柄。新文件将从这个文件中复制扩展属性
	);
	if (gHandle == INVALID_HANDLE_VALUE) rCode = FALSE;
	else rCode = TRUE;

#ifdef _DEBUG
	DWORD error = GetLastError();
	GetFormatTime();
	fputs("创建驱动对象（OpenDriver）->", fp);
	if (!rCode)
	{
		snprintf(tmp, sizeof(tmp), "失败（错误代码：%d）\n", error);
		fputs(tmp, fp);
	}
	else fputs("成功\n",fp);
#endif
	return rCode;
}

BOOL Driver::InstallDriver()
{
	SC_HANDLE	hService = NULL;
	BOOL        rCode = FALSE;
	DWORD		error = NO_ERROR;

	EnableSeLoadDriverPrivilege();
	//创建一个服务对象，并把它添加进SCM数据库
	hService = CreateService(
		hSCManager,				//SCM数据库句柄，同时该句柄需要具有SC_MANAGER_CREATE_SERVICE权限
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
	}
	else
	{
		rCode = TRUE;
		CloseServiceHandle(hService);
	}
#ifdef _DEBUG
	GetFormatTime();
	fputs("创建服务对象（InstallDriver）->", fp);
	if (!rCode)
	{
		snprintf(tmp, sizeof(tmp), "失败（错误代码：%d）\n", error);
		fputs(tmp, fp);
	}
	else fputs("成功\n", fp);
#endif
	return rCode;
}

BOOL Driver::StartDriver()
{
	SC_HANDLE	hService = NULL;
	BOOL		rCode = FALSE;
	DWORD		error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
#ifdef _DEBUG
	GetFormatTime();
	fputs("打开服务（StartDriver）->", fp);
	if (hService != NULL)
	{
		fputs("成功\n", fp);
		GetFormatTime();
		fputs("运行服务（StartDriver）->", fp);
		if (!StartService(hService, 0, NULL))
		{
			error = GetLastError();
			if (error == ERROR_SERVICE_ALREADY_RUNNING)
			{
				fputs("成功\n", fp);
				rCode = TRUE;
			}
			else
			{
				fputs("失败", fp);
				snprintf(tmp, sizeof(tmp), "(错误代码：%d)\n", error);
				fputs(tmp, fp);
			}
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

BOOL Driver::RemoveDriver()
{
	SC_HANDLE   hService = NULL;
	BOOL        rCode = FALSE;
	DWORD		error = NO_ERROR;
	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

	if (hService == NULL)
	{
		rCode = TRUE;
	}
	else
	{
		rCode = DeleteService(hService);
		error = GetLastError();
		CloseServiceHandle(hService);
	}
#ifdef _DEBUG
	GetFormatTime();
	fputs("删除驱动（RemoveDriver）->", fp);
	if (rCode) fputs("成功\n", fp);
	else
	{
		fputs("失败", fp);
		snprintf(tmp, sizeof(tmp), "(错误代码：%d)\n", error);
		fputs(tmp, fp);
	}
#endif // _DEBUG
	return rCode;
}

BOOL Driver::StopDriver()
{
	SC_HANDLE		hService = NULL;
	BOOL			rCode = FALSE;
	SERVICE_STATUS	serviceStatus;
	DWORD		error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

	if (hService != NULL)
	{
		EnableSeLoadDriverPrivilege();
		rCode = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
		error = GetLastError();
		CloseServiceHandle(hService);
	}
#ifdef _DEBUG
	GetFormatTime();
	fputs("停止驱动（StopDriver）->", fp);
	if (rCode) fputs("成功\n", fp);
	else
	{
		fputs("失败", fp);
		snprintf(tmp, sizeof(tmp), "（错误代码：%d）\n", error);
		fputs(tmp, fp);
	}
#endif // _DEBUG
	return rCode;
}

BOOL Driver::SystemInstallDriver()
{
	SC_HANDLE	hService = NULL;
	BOOL		rCode = FALSE;
	DWORD		error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

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

	return rCode;
}

BOOL Driver::IsSystemInstallDriver()
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

VOID Driver::DeleteDriver()
{
	if (gHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(gHandle);
		gHandle = INVALID_HANDLE_VALUE;
	}
	StopDriver();
	RemoveDriver();
}

BOOL Driver::EnableSeLoadDriverPrivilege()
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