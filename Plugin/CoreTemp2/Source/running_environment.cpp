#include "running_environment.h"

BOOL IsNT()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	/*
	dwOSVersionInfoSize
	���ֽ�Ϊ��λ��ָ��������ݽṹ�Ĵ�С�����ڵ���GetVersionEx��OSVERSIONINFO������֮ǰ���������Ա����Ϊsizeof(OSVERSIONINFO)��
	*/
	GetVersionEx(&osvi);
	/*
	dwPlatformId
	��ʶ����ϵͳƽ̨�������Ա����������ֵ֮һ��
	ֵ��						ƽ̨��
	VER_PLATFORM_WIN32s			Win32 on Windows 3.1.
	VER_PLATFORM_WIN32_WINDOWS	Win32 on Windows 95.
	VER_PLATFORM_WIN32_NT		Win32 on Windows NT.
	*/
	return (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

/*
Ӣ�ض����ڻ���߰汾�Ĵ�����֧�֡�CPUID��ָ����⣬���µ�Ӣ�ض�80486����������֧�֡�CPUID��ָ�
*/
BOOL IsCpuid()
{
	__try
	{
		int info[4];
		__cpuid(info, 0x0);//https://docs.microsoft.com/de-de/previous-versions/visualstudio/visual-studio-2008/hskdteyh(v=vs.90)?redirectedfrom=MSDN
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
		/*
		except������ֵ���������֣�
		EXCEPTION_CONTINUE_EXECUTION (�C1)��	�쳣�����ԣ������������쳣���ֵĵ�֮�󣬼����ָ����С�
		EXCEPTION_CONTINUE_SEARCH (0)��			�쳣����ʶ��Ҳ����ǰ�����__exceptģ�鲻������쳣��������Ӧ����ȷ���쳣����ģ�顣ϵͳ���������� __try __except���м�������һ��ǡ����__exceptģ�顣
		EXCEPTION_EXECUTE_HANDLER (1)��			�쳣�Ѿ���ʶ�𣬿����������뵽__exceptģ���������쳣�������
		*/
	{
		return FALSE;
	}

	return TRUE;
}

/*
�ж��Ƿ�֧�� RDMSR �� WRMSR
*/
BOOL IsMsr()
{
	// MSR����׼������־ EDX �ĵ�5λ

	int info[4];
	__cpuid(info, 0x1);

	return ((info[3] >> 5) & 1);
}

/*
ȷ��ָ�������Ƿ�������64λ����ϵͳ��32������Wow64���¡�
*/
BOOL IsWow64()
{
	BOOL isWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");

	if (fnIsWow64Process != NULL)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &isWow64)) isWow64 = FALSE;
	}
	return isWow64;
}

/*
ȷ��ָ�������Ƿ�������64λ����ϵͳ��
*/
BOOL IsX64()
{
	SYSTEM_INFO systemInfo;
	BOOL isX64 = FALSE;
	LPFN_GETNATIVESYSTEMINFO fnGetNativeSystemInfo = (LPFN_GETNATIVESYSTEMINFO)GetProcAddress(GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");

	if (fnGetNativeSystemInfo != NULL)
	{
		fnGetNativeSystemInfo(&systemInfo);
		isX64 = (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
	}
	return isX64;
}

BOOL RuningEn(TCHAR gDriverFileName[])
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		return 0;
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		_tcscpy_s(gDriverFileName, MAX_PATH, OLS_DRIVER_FILE_NAME_WIN_9X);
		return 0;
		break;
	case VER_PLATFORM_WIN32_NT:
		if (IsWow64())
		{
			if (IsX64())
			{
				_tcscpy_s(gDriverFileName, MAX_PATH, OLS_DRIVER_FILE_NAME_WIN_NT_X64);
			}
		}
		else
		{
			_tcscpy_s(gDriverFileName, MAX_PATH, OLS_DRIVER_FILE_NAME_WIN_NT);
		}
		return 0;
		break;
	default:
		return 0;
		break;
	}
}