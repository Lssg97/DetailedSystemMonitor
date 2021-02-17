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
	sprintf(tmp, "[%02d:%02d:%02d.%03d]��", wtm.wHour, wtm.wMinute, wtm.wSecond, wtm.wMilliseconds);
	fputs(tmp, fp);
}
#endif // _DEBUG

BOOL Driver::NewDriver()
{
	hSCManager = OpenSCManager(
		NULL,					//Ŀ����������NULL��ʾ���ؼ����
		NULL,					//����������ϵͳ������ݿ⣬������ΪSERVICES_ACTIVE_DATABASE�����ΪNULL����ʾĬ�ϴ�SERVICES_ACTIVE_DATABASE������������ϵͳ������ݿ⣩
		SC_MANAGER_ALL_ACCESS	//��SCM��Ȩ�ޣ����Ȩ�ޣ�
	);
#ifdef _DEBUG
	DWORD error = GetLastError();
	GetFormatTime();
	fputs("�򿪷���������ϵͳ������ݿ⣨NewDriver��->", fp);
	if (hSCManager == NULL)
	{
		snprintf(tmp, sizeof(tmp), "ʧ��(������룺%d)\n", error);
		fputs(tmp, fp);
		return FALSE;
	}
	else fputs("�ɹ�\n", fp);
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
	//����������ж��󣬲�����һ����������������Щ����ľ��
	gHandle = CreateFile(
		_T("\\\\.\\") OLS_DRIVER_ID,	//Ҫ�򿪵��ļ�������
		GENERIC_READ | GENERIC_WRITE,	//���Ϊ GENERIC_READ ��ʾ������豸���ж����ʣ����Ϊ GENERIC_WRITE ��ʾ������豸����д���ʣ������ʹ�ã�
		0,								//������FILE_SHARE_READ �� / �� FILE_SHARE_WRITE ��ʾ������ļ����й������
		NULL,							//SECURITY_ATTRIBUTES ��ָ��һ�� SECURITY_ATTRIBUTES �ṹ��ָ�룬�������ļ��İ�ȫ���ԣ��������ϵͳ֧�ֵĻ���
		OPEN_EXISTING,					//�ļ������Ѿ����ڡ����豸���Ҫ��
		FILE_ATTRIBUTE_NORMAL,			//Ĭ������
		NULL							//�����Ϊ�㣬��ָ��һ���ļ���������ļ���������ļ��и�����չ����
	);
	if (gHandle == INVALID_HANDLE_VALUE) rCode = FALSE;
	else rCode = TRUE;

#ifdef _DEBUG
	DWORD error = GetLastError();
	GetFormatTime();
	fputs("������������OpenDriver��->", fp);
	if (!rCode)
	{
		snprintf(tmp, sizeof(tmp), "ʧ�ܣ�������룺%d��\n", error);
		fputs(tmp, fp);
	}
	else fputs("�ɹ�\n",fp);
#endif
	return rCode;
}

BOOL Driver::InstallDriver()
{
	SC_HANDLE	hService = NULL;
	BOOL        rCode = FALSE;
	DWORD		error = NO_ERROR;

	EnableSeLoadDriverPrivilege();
	//����һ��������󣬲�������ӽ�SCM���ݿ�
	hService = CreateService(
		hSCManager,				//SCM���ݿ�����ͬʱ�þ����Ҫ����SC_MANAGER_CREATE_SERVICEȨ��
		DriverId,				//Ҫ��װ��������֣����ɳ���256���ַ������ܺ���/��\�ַ���ͬʱ��Сд����
		DriverId,				//������ʾ��������
		SERVICE_ALL_ACCESS,		//�Է����Ȩ��
		SERVICE_KERNEL_DRIVER,	//�������ͣ���������
		SERVICE_DEMAND_START,	//��������ѡ���һ���������StartService����ʱ��SCM��������
		SERVICE_ERROR_NORMAL,	//���������ʽ�������������¼���־�м�¼���󣬵��������в�����
		DriverPath,				//�������·��
		NULL,					//�ڼ���˳��˷���������������ƣ�����÷��������飬ָ�� NULL ����ַ�����
		NULL,					//ָ����� lpLoadOrderGroup ������ָ��������Ψһ�ı��ֵ�ı����� ������������еı�ǩ����ָ�� NULL
		NULL,					//�շָ����Ƶķ�������˳����ϵͳ�������������ʼ֮ǰ��˫����ֹ�����ָ�롣 �������û���κ�������ϵ����ָ��Ϊ NULL ����ַ�����
		NULL,					//�÷���Ӧ���������е��ʻ������ƣ�ʹ��LocalSystem�ʻ���
		NULL					//��lpServiceStartName����ָ�����ʻ��������롣 ������ʻ�û�����룬���������LocalService��NetworkService��LocalSystem�ʻ������и÷�����ָ��һ�����ַ�����NULL
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
	fputs("�����������InstallDriver��->", fp);
	if (!rCode)
	{
		snprintf(tmp, sizeof(tmp), "ʧ�ܣ�������룺%d��\n", error);
		fputs(tmp, fp);
	}
	else fputs("�ɹ�\n", fp);
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
	fputs("�򿪷���StartDriver��->", fp);
	if (hService != NULL)
	{
		fputs("�ɹ�\n", fp);
		GetFormatTime();
		fputs("���з���StartDriver��->", fp);
		if (!StartService(hService, 0, NULL))
		{
			error = GetLastError();
			if (error == ERROR_SERVICE_ALREADY_RUNNING)
			{
				fputs("�ɹ�\n", fp);
				rCode = TRUE;
			}
			else
			{
				fputs("ʧ��", fp);
				snprintf(tmp, sizeof(tmp), "(������룺%d)\n", error);
				fputs(tmp, fp);
			}
		}
		else
		{
			fputs("�ɹ�\n", fp);
			rCode = TRUE;
		}
		CloseServiceHandle(hService);
	}
	else
	{
		error = GetLastError();
		fputs("ʧ��", fp);
		snprintf(tmp, sizeof(tmp), "(������룺%d)\n", error);
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
	fputs("ɾ��������RemoveDriver��->", fp);
	if (rCode) fputs("�ɹ�\n", fp);
	else
	{
		fputs("ʧ��", fp);
		snprintf(tmp, sizeof(tmp), "(������룺%d)\n", error);
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
	fputs("ֹͣ������StopDriver��->", fp);
	if (rCode) fputs("�ɹ�\n", fp);
	else
	{
		fputs("ʧ��", fp);
		snprintf(tmp, sizeof(tmp), "��������룺%d��\n", error);
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