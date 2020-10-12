#pragma once

#include "Driver.h"
#include "running_environment.h"

#define IOCTL_OLS_READ_MSR				CTL_CODE(OLS_TYPE, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OLS_READ_PCI_CONFIG		CTL_CODE(OLS_TYPE, 0x851, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_OLS_READ_IO_PORT_DWORD	CTL_CODE(OLS_TYPE, 0x835, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_OLS_WRITE_IO_PORT_DWORD	CTL_CODE(OLS_TYPE, 0x838, METHOD_BUFFERED, FILE_WRITE_ACCESS)

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

BOOL WINAPI Rdmsr(DWORD index, PDWORD eax, PDWORD edx);
BOOL WINAPI RdmsrTx(DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR threadAffinityMask);
BOOL pciConfigRead(DWORD pciAddress, DWORD regAddress, PBYTE value, DWORD size, PDWORD error);
DWORD WINAPI ReadPciConfigDword(DWORD pciAddress, BYTE regAddress);
DWORD WINAPI ReadIoPortDword(WORD port);
BOOL WINAPI WriteIoPortDword(WORD port, DWORD value);