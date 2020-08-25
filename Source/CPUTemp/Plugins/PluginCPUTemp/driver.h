#pragma once


#include <windows.h>
#include <winioctl.h>
#include <tchar.h>
#include "running_environment.h"

#ifdef _DEBUG
#include <stdio.h>
#include <time.h>
#endif

#define OLS_TYPE						40000

#define OLS_DRIVER_ID					_T("WinRing0_1_2_0")
#define IOCTL_OLS_READ_MSR				CTL_CODE(OLS_TYPE, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OLS_READ_PCI_CONFIG		CTL_CODE(OLS_TYPE, 0x851, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_OLS_READ_IO_PORT_DWORD	CTL_CODE(OLS_TYPE, 0x835, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_OLS_WRITE_IO_PORT_DWORD	CTL_CODE(OLS_TYPE, 0x838, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define PciBusDevFunc(bus,dev,func)	(((bus&0xFF)<<8) | ((dev&0x1F)<<3) | (func&7))
#define PciGetBus(address)			((address>>8) & 0xFF)
#define PciGetDev(address)			((address>>3) & 0x1F)
#define PciGetFunc(address)			(address&7)

#define DeviceSlot(uDevice, uFunction) ((((uDevice)&0x1f)<<3)|((uFunction)&0x07))
#define GetDevice(uBus,uSlot,uAddress) (0x80000000L |((uBus&0xff)<<16)|(uSlot<<8)|(uAddress&~3))

class driver
{
public:
	driver(LPCTSTR gDriverId, LPCTSTR gDriverPath);
	BOOL WINAPI Rdmsr(DWORD index, PDWORD eax, PDWORD edx);
	BOOL WINAPI RdmsrTx(DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR threadAffinityMask);
	BOOL pciConfigRead(DWORD pciAddress, DWORD regAddress, PBYTE value, DWORD size, PDWORD error);
	DWORD WINAPI ReadPciConfigDword(DWORD pciAddress, BYTE regAddress);
	DWORD WINAPI ReadIoPortDword(WORD port);
	BOOL WINAPI WriteIoPortDword(WORD port, DWORD value);
	BOOL get_bus_dev(UINT devieid, INT* BUS, INT* DEV);
	DWORD get_temp();
	DWORD GetTemp(DWORD_PTR threadAffinityMask);
	~driver();
private:
	HANDLE		gHandle = INVALID_HANDLE_VALUE;		//句柄
	SC_HANDLE	hSCManager = NULL;					//服务管理程序系统组件数据库
	LPCTSTR		DriverId = NULL;					//驱动名称
	LPCTSTR		DriverPath = NULL;					//驱动路径
	BOOL		gIsNT = FALSE;						//是否为NT系统
	BOOL		gIsCpuid = FALSE;					//是否支持CPUID
	BOOL		gIsMsr = FALSE;						//是否支持Msr操作
	typedef struct  _OLS_READ_PCI_CONFIG_INPUT {
		ULONG PciAddress;
		ULONG PciOffset;
	}OLS_READ_PCI_CONFIG_INPUT;
	typedef struct  _OLS_WRITE_IO_PORT_INPUT {
		ULONG	PortNumber;
		union {
			ULONG   LongData;
			USHORT  ShortData;
			UCHAR   CharData;
		};
	}OLS_WRITE_IO_PORT_INPUT;
	BOOL NewDriver();
	BOOL OpenDriver();
	BOOL InstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);
	BOOL StartDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
	BOOL StopDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
	BOOL RemoveDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
	BOOL SystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);
	BOOL IsSystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);

	BOOL EnableSeLoadDriverPrivilege();
#ifdef _DEBUG
	FILE* fp = fopen("log.txt", "a");
	char tmp[64];
#endif
};

