#include "driver.h"


driver::driver()
{
	TCHAR gDriverFileName[MAX_PATH];
	TCHAR gDriverPath[MAX_PATH];

	RuningEn(gDriverFileName);
	TCHAR dir[MAX_PATH];
	TCHAR* ptr;

	GetModuleFileName(NULL, dir, MAX_PATH);//获取当前进程已加载模块的文件的完整路径，该模块必须由当前进程加载。
	if ((ptr = _tcsrchr(dir, '\\')) != NULL) *ptr = '\0';
	wsprintf(gDriverPath, _T("%s\\%s"), dir, gDriverFileName);

	this->DriverId = OLS_DRIVER_ID;
	this->DriverPath = gDriverPath;
	this->gIsNT = IsNT();
	this->gIsCpuid = IsCpuid();
	if (gIsCpuid) this->gIsMsr = IsMsr();
#ifdef TEST
	printf("%s\n", "对象初始化完毕");
#endif
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
	hSCManager = OpenSCManager(NULL, //目标计算机名，NULL表示本地计算机
		NULL, //服务管理程序系统组件数据库，可以设为SERVICES_ACTIVE_DATABASE，如果为NULL，表示默认打开SERVICES_ACTIVE_DATABASE（服务管理程序系统组件数据库）
		SC_MANAGER_ALL_ACCESS); //对SCM的权限（最高权限）
	if (hSCManager == NULL) return FALSE;
#ifdef TEST
	printf("%s\n", "已打开服务管理程序系统组件数据库");
#endif

	if (!OpenDriver())
	{
		if (gHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(gHandle);
			gHandle = INVALID_HANDLE_VALUE;
		}
		StopDriver(hSCManager, DriverId);
		RemoveDriver(hSCManager, DriverId);
		InstallDriver(hSCManager, DriverId, DriverPath);
		StartDriver(hSCManager, DriverId);
		OpenDriver();
	}
	if (hSCManager != NULL) CloseServiceHandle(hSCManager);
}

BOOL driver::OpenDriver()
{
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
	if (gHandle == INVALID_HANDLE_VALUE)
	{
#ifdef TEST
		printf("%s\n", "句柄申请失败");
		system("pause");
#endif
		return FALSE;
	}
#ifdef TEST
	printf("%s\n", "句柄申请成功");
#endif
	return TRUE;
}

BOOL driver::InstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath)
{
	SC_HANDLE	hService = NULL;
	BOOL        rCode = FALSE;
	DWORD		error = NO_ERROR;
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
#ifdef TEST
		printf("%s\n","创建服务对象失败");
		system("pause");
#endif
	}
	else
	{
#ifdef TEST
		printf("%s\n", "创建服务对象成功");
#endif
		rCode = TRUE;
		CloseServiceHandle(hService);
	}

	return rCode;
}

BOOL driver::StartDriver(SC_HANDLE hSCManager, LPCTSTR DriverId)
{
	SC_HANDLE	hService = NULL;
	BOOL		rCode = FALSE;
	DWORD		error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

	if (hService != NULL)
	{
#ifdef TEST
		printf("%s\n", "成功打开服务");
#endif
		if (!StartService(hService, 0, NULL))
		{
			error = GetLastError();
			if (error == ERROR_SERVICE_ALREADY_RUNNING)
			{
				rCode = TRUE;
			}
#ifdef TEST
			printf("%s\n", "无法运行服务");
			printf("错误代码：%d\n",error);
#endif
		}
		else
		{
#ifdef TEST
			printf("%s\n", "开始运行服务");
#endif
			rCode = TRUE;
		}
		CloseServiceHandle(hService);
	}
#ifdef TEST
	else
	{
		printf("%s\n", "打开服务失败");
		system("pause");
	}
#endif
	return rCode;
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
#ifdef TEST
		printf("%s\n", "卸载服务");
#endif
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