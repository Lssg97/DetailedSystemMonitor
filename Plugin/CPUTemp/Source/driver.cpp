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
	strftime(tmp, sizeof(tmp), "[%X]��", localtime(&t));
	fputs(tmp, fp);
	fputs("�򿪷���������ϵͳ������ݿ�->", fp);
	hSCManager = OpenSCManager(NULL, //Ŀ����������NULL��ʾ���ؼ����
		NULL, //����������ϵͳ������ݿ⣬������ΪSERVICES_ACTIVE_DATABASE�����ΪNULL����ʾĬ�ϴ�SERVICES_ACTIVE_DATABASE������������ϵͳ������ݿ⣩
		SC_MANAGER_ALL_ACCESS); //��SCM��Ȩ�ޣ����Ȩ�ޣ�
	if (hSCManager == NULL)
	{
		fputs("ʧ��\n", fp);
		return FALSE;
	}
	fputs("�ɹ�\n", fp);
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
	strftime(tmp, sizeof(tmp), "[%X]��", localtime(&t));
	fputs(tmp, fp);
	fputs("������������->", fp);
#endif
	//����������ж��󣬲�����һ����������������Щ����ľ��
	gHandle = CreateFile(
		_T("\\\\.\\") OLS_DRIVER_ID,//Ҫ�򿪵��ļ�������
		GENERIC_READ | GENERIC_WRITE,//���Ϊ GENERIC_READ ��ʾ������豸���ж����ʣ����Ϊ GENERIC_WRITE ��ʾ������豸����д���ʣ������ʹ�ã�
		0,//������FILE_SHARE_READ �� / �� FILE_SHARE_WRITE ��ʾ������ļ����й������
		NULL,//SECURITY_ATTRIBUTES ��ָ��һ�� SECURITY_ATTRIBUTES �ṹ��ָ�룬�������ļ��İ�ȫ���ԣ��������ϵͳ֧�ֵĻ���
		OPEN_EXISTING,//�ļ������Ѿ����ڡ����豸���Ҫ��
		FILE_ATTRIBUTE_NORMAL,//Ĭ������
		NULL//�����Ϊ�㣬��ָ��һ���ļ���������ļ���������ļ��и�����չ����
	);
	
#ifdef _DEBUG
	DWORD error = GetLastError();
	if (gHandle == INVALID_HANDLE_VALUE)
	{
		fputs("ʧ��", fp);
		snprintf(tmp, sizeof(tmp), "(������룺%d)\n", error);
		fputs(tmp, fp);
	}
	fputs("�ɹ�\n", fp);
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

	strftime(tmp, sizeof(tmp), "[%X]��", localtime(&t));
	fputs(tmp, fp);
	fputs("�����������->", fp);
	EnableSeLoadDriverPrivilege();
	//����һ��������󣬲�������ӽ�SCM���ݿ�
	hService = CreateService(hSCManager,			//SCM���ݿ�����ͬʱ�þ����Ҫ����SC_MANAGER_CREATE_SERVICEȨ��
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
		fputs("ʧ��", fp);
		snprintf(tmp, sizeof(tmp), "(������룺%d)\n", error);
		fputs(tmp, fp);
	}
	else
	{
		fputs("�ɹ�\n", fp);
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
	strftime(tmp, sizeof(tmp), "[%X]��", localtime(&t));
	fputs(tmp, fp);
	fputs("�򿪷���->", fp);
	if (hService != NULL)
	{
		fputs("�ɹ�\n", fp);
		time_t t = time(0);
		strftime(tmp, sizeof(tmp), "[%X]��", localtime(&t));
		fputs(tmp, fp);
		fputs("���з���->", fp);
		if (!StartService(hService, 0, NULL))
		{
			error = GetLastError();
			if (error == ERROR_SERVICE_ALREADY_RUNNING)
			{
				rCode = TRUE;
			}
			fputs("ʧ��", fp);
			snprintf(tmp, sizeof(tmp), "(������룺%d)\n", error);
			fputs(tmp, fp);
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
	strftime(tmp, sizeof(tmp), "[%X]��", localtime(&t));
	fputs(tmp, fp);
	fputs("����ϵͳ����->", fp);

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
		fputs("�ɹ�\n", fp);
	}
	else
	{
		fputs("ʧ��", fp);
		snprintf(tmp, sizeof(tmp), "(������룺%d)\n", error);
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
		gHandle,				// ��Ҫִ�в������豸��������豸ͨ���Ǿ�Ŀ¼���ļ�������ʹ�� CreateFile �����򿪻�ȡ�豸���������ļ���ע
		IOCTL_OLS_READ_MSR,		// �����Ŀ��ƴ��룬��ֵ��ʶҪִ�е��ض������Լ�ִ�иò������豸�����ͣ��йؿ��ƴ�����б���ο���ע��
		&index,					// ����ѡ��ָ�����뻺������ָ�롣
		sizeof(index),			// ���뻺�������ֽ�Ϊ��λ�Ĵ�С����λΪ�ֽڡ�
		&outBuf,				// ����ѡ��ָ�������������ָ�롣
		sizeof(outBuf),			// ������������ֽ�Ϊ��λ�Ĵ�С����λΪ�ֽڡ�
		&returnedLength,		// ����ѡ��ָ��һ��������ָ�룬�ñ������մ洢������������е����ݵĴ�С��
		NULL					// ����ѡ��ָ��OVERLAPPED�ṹ��ָ�롣
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