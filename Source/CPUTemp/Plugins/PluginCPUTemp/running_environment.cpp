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

BOOL IsAMD()
{
	int info[4];
	__cpuid(info, 0x80000005);
	int ans = info[0] | info[1] | info[2] | info[3];
	return ans;
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
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
	HMODULE hModule = GetModuleHandle(_T("kernel32"));
	if (hModule != NULL)
	{
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hModule, "IsWow64Process");
	}

	if (fnIsWow64Process != NULL)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &isWow64)) isWow64 = FALSE;
		else isWow64 = TRUE;
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
	LPFN_GETNATIVESYSTEMINFO fnGetNativeSystemInfo = NULL;
	HMODULE hModule = GetModuleHandle(_T("kernel32"));
	if (hModule != NULL)
	{
		fnGetNativeSystemInfo = (LPFN_GETNATIVESYSTEMINFO)GetProcAddress(hModule, "GetNativeSystemInfo");
	}

	if (fnGetNativeSystemInfo != NULL)
	{
		fnGetNativeSystemInfo(&systemInfo);
		isX64 = (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
	}
	return isX64;
}

BOOL RuningEn(TCHAR gDriverPath[])
{
	TCHAR gDriverFileName[MAX_PATH];
	TCHAR dir[MAX_PATH];
	TCHAR* ptr;
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		if (_access("WinRing0.vxd", 0) == -1)
		{
			HexToChar(WinRing0_vxd, "WinRing0.vxd");
		}	
		_tcscpy_s(gDriverFileName, MAX_PATH, OLS_DRIVER_FILE_NAME_WIN_9X);
		break;
	case VER_PLATFORM_WIN32_NT:
		if (IsWow64())
		{
			if (IsX64())
			{
				if (_access("WinRing0x64.sys", 0) == -1)
				{
					HexToChar(WinRing0X64_sys, "WinRing0x64.sys");
				}
				_tcscpy_s(gDriverFileName, MAX_PATH, OLS_DRIVER_FILE_NAME_WIN_NT_X64);
			}
		}
		else
		{
			if (_access("WinRing0.sys", 0) == -1)
			{
				HexToChar(WinRing0_sys, "WinRing0.sys");
			}
			_tcscpy_s(gDriverFileName, MAX_PATH, OLS_DRIVER_FILE_NAME_WIN_NT);
		}
		break;
	default:
		break;
	}

	GetModuleFileName(NULL, dir, MAX_PATH);//��ȡ��ǰ�����Ѽ���ģ����ļ�������·������ģ������ɵ�ǰ���̼��ء�
	if ((ptr = _tcsrchr(dir, '\\')) != NULL) *ptr = '\0';
	wsprintf(gDriverPath, _T("%s\\%s"), dir, gDriverFileName);

	return TRUE;
}