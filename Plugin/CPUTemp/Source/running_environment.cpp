#include "running_environment.h"

BOOL IsNT()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	/*
	dwOSVersionInfoSize
	以字节为单位，指定这个数据结构的大小。须在调用GetVersionEx（OSVERSIONINFO）函数之前，将这个成员设置为sizeof(OSVERSIONINFO)。
	*/
	GetVersionEx(&osvi);
	/*
	dwPlatformId
	标识操作系统平台。这个成员可以是下列值之一：
	值：						平台：
	VER_PLATFORM_WIN32s			Win32 on Windows 3.1.
	VER_PLATFORM_WIN32_WINDOWS	Win32 on Windows 95.
	VER_PLATFORM_WIN32_NT		Win32 on Windows NT.
	*/
	return (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

/*
英特尔奔腾或更高版本的处理器支持“CPUID”指令。此外，最新的英特尔80486处理器可能支持“CPUID”指令。
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
		except参数的值有以下三种：
		EXCEPTION_CONTINUE_EXECUTION (–1)：	异常被忽略，控制流将在异常出现的点之后，继续恢复运行。
		EXCEPTION_CONTINUE_SEARCH (0)：			异常不被识别，也即当前的这个__except模块不是这个异常错误所对应的正确的异常处理模块。系统将继续到上 __try __except域中继续查找一个恰当的__except模块。
		EXCEPTION_EXECUTE_HANDLER (1)：			异常已经被识别，控制流将进入到__except模块中运行异常处理代码
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
判断是否支持 RDMSR 和 WRMSR
*/
BOOL IsMsr()
{
	// MSR：标准特征标志 EDX 的第5位

	int info[4];
	__cpuid(info, 0x1);

	return ((info[3] >> 5) & 1);
}

/*
确定指定进程是否运行在64位操作系统的32环境（Wow64）下。
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
确定指定进程是否运行在64位操作系统。
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

	GetModuleFileName(NULL, dir, MAX_PATH);//获取当前进程已加载模块的文件的完整路径，该模块必须由当前进程加载。
	if ((ptr = _tcsrchr(dir, '\\')) != NULL) *ptr = '\0';
	wsprintf(gDriverPath, _T("%s\\%s"), dir, gDriverFileName);

	return TRUE;
}