#include "driver.h"


driver::driver()
{
	TCHAR gDriverFileName[MAX_PATH];
	TCHAR gDriverPath[MAX_PATH];

	RuningEn(gDriverFileName);
	TCHAR dir[MAX_PATH];
	TCHAR* ptr;

	GetModuleFileName(NULL, dir, MAX_PATH);//��ȡ��ǰ�����Ѽ���ģ����ļ�������·������ģ������ɵ�ǰ���̼��ء�
	if ((ptr = _tcsrchr(dir, '\\')) != NULL) *ptr = '\0';
	wsprintf(gDriverPath, _T("%s\\%s"), dir, gDriverFileName);

	this->DriverId = OLS_DRIVER_ID;
	this->DriverPath = gDriverPath;
	this->gIsNT = IsNT();
	this->gIsCpuid = IsCpuid();
	if (gIsCpuid) this->gIsMsr = IsMsr();
#ifdef TEST
	printf("%s\n", "�����ʼ�����");
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
	hSCManager = OpenSCManager(NULL, //Ŀ����������NULL��ʾ���ؼ����
		NULL, //����������ϵͳ������ݿ⣬������ΪSERVICES_ACTIVE_DATABASE�����ΪNULL����ʾĬ�ϴ�SERVICES_ACTIVE_DATABASE������������ϵͳ������ݿ⣩
		SC_MANAGER_ALL_ACCESS); //��SCM��Ȩ�ޣ����Ȩ�ޣ�
	if (hSCManager == NULL) return FALSE;
#ifdef TEST
	printf("%s\n", "�Ѵ򿪷���������ϵͳ������ݿ�");
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
	if (gHandle == INVALID_HANDLE_VALUE)
	{
#ifdef TEST
		printf("%s\n", "�������ʧ��");
		system("pause");
#endif
		return FALSE;
	}
#ifdef TEST
	printf("%s\n", "�������ɹ�");
#endif
	return TRUE;
}

BOOL driver::InstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath)
{
	SC_HANDLE	hService = NULL;
	BOOL        rCode = FALSE;
	DWORD		error = NO_ERROR;
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
#ifdef TEST
		printf("%s\n","�����������ʧ��");
		system("pause");
#endif
	}
	else
	{
#ifdef TEST
		printf("%s\n", "�����������ɹ�");
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
		printf("%s\n", "�ɹ��򿪷���");
#endif
		if (!StartService(hService, 0, NULL))
		{
			error = GetLastError();
			if (error == ERROR_SERVICE_ALREADY_RUNNING)
			{
				rCode = TRUE;
			}
#ifdef TEST
			printf("%s\n", "�޷����з���");
			printf("������룺%d\n",error);
#endif
		}
		else
		{
#ifdef TEST
			printf("%s\n", "��ʼ���з���");
#endif
			rCode = TRUE;
		}
		CloseServiceHandle(hService);
	}
#ifdef TEST
	else
	{
		printf("%s\n", "�򿪷���ʧ��");
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
		printf("%s\n", "ж�ط���");
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